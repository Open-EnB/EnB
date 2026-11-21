"""Names the addresses in a program's crash log with its VC6 map file (L2).

    python tools/harness/symbolize.py MAPFILE LOGFILE      every CRASH: line named (stubs.log, or d3d8shot.log:
                                                           the proxy's first-chance lines, a tick column first)
    python tools/harness/symbolize.py MAPFILE ADDR...      the given addresses

The map's public symbols ("Address Publics by Value Rva+Base Lib:Object") are sorted by address;
an address belongs to the last symbol at or below it. Static functions are listed in the map's
"Static symbols" section too, and are included.
"""
import bisect
import re
import sys

LINE = re.compile(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(f\s+)?(?:i\s+)?(\S+)?\s*$')


def load(mapfile):
    syms = []
    for line in open(mapfile, encoding='latin1'):
        m = LINE.match(line)
        if m:
            syms.append((int(m.group(2), 16), m.group(1), m.group(4) or '', bool(m.group(3))))
    syms.sort()
    return syms


def undecorate(name):
    try:
        import ctypes
        buf = ctypes.create_string_buffer(1024)
        fn = ctypes.windll.dbghelp.UnDecorateSymbolName
        if fn(name.encode(), buf, 1024, 0x1000 | 0x0002 | 0x0004 | 0x0080 | 0x0200):   # name only-ish
            return buf.value.decode('latin1')
    except Exception:
        pass
    return name


def name_of(syms, addr):
    keys = [s[0] for s in syms]
    i = bisect.bisect_right(keys, addr) - 1
    if i < 0:
        return '?'
    a, n, obj, _ = syms[i]
    return f'{undecorate(n)}+0x{addr - a:X} ({obj})'


CRASH = re.compile(r'^(?:\s*\d+ )?(CRASH:.*)$')     # d3d8shot.log's lines carry a GetTickCount column first


def crash_line(line):
    """The CRASH: text of a log line (stubs.log's, or d3d8shot.log's after its tick column), else None."""
    m = CRASH.match(line.rstrip('\r\n'))
    return m.group(1) if m else None


def crash_addrs(text):
    """The code addresses of a 'CRASH: code C at EIP (address A) stack: S1 S2 ...' line (d3d8shot.c Safe_Crash_Log,
    and the stand-in builds' handler before it): EIP, then the stack's, those inside the exe's image only."""
    m = re.match(r'CRASH: code \w+ at ([0-9A-Fa-f]{8}) \(address \w+\) stack:((?: [0-9A-Fa-f]{8})*)', text)
    if m:
        vals = [int(m.group(1), 16)] + [int(x, 16) for x in m.group(2).split()]
    else:
        vals = [int(x, 16) for x in re.findall(r'(?:at|stack \+[0-9A-F]+) ([0-9A-F]{8})', text)]
    return [v for v in vals if 0x401000 <= v < 0x1000000]


def main():
    syms = load(sys.argv[1])
    args = sys.argv[2:]
    if len(args) == 1 and not re.fullmatch(r'(0x)?[0-9a-fA-F]+', args[0]):
        for line in open(args[0], encoding='latin1'):
            text = crash_line(line)
            if text:
                print(text)
                for v in crash_addrs(text)[:12]:
                    print(f'    {v:08X} {name_of(syms, v)}')
        return
    for a in args:
        v = int(a, 16)
        print(f'{v:08X} {name_of(syms, v)}')


if __name__ == '__main__':
    main()
