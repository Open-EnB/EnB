"""Quick lookups in client.exe, with every address named from what the project already knows.

    python client.py dis ADDR [--len N]     disassemble one function (or N bytes), targets named
    python client.py name ADDR...            what an address is: symbol, containing function, string
    python client.py refs ADDR...            every place (any section) holding the address as a dword
    python client.py callers ADDR...         direct calls and jumps to a function, also through its thunks

Names come from docs/data/symbols.json (proven: exact, vtable, strings, thunks), then from the
gaps reports (docs/data/gaps-*.json: located but not byte-exact, shown as `~Name?`), and an
incremental-link thunk is followed to its target. For vtables use vtables.py (`--seed`).
"""
import argparse
import bisect
import glob
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import DEFAULT_CLIENT, ClientImage  # noqa: E402

DATA = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data'))


class Names:
    """Client address -> name, from the project's symbol map and gaps reports."""

    def __init__(self, img):
        self.img = img
        self.proven = {}
        self.guessed = {}
        path = os.path.join(DATA, 'symbols.json')
        if os.path.exists(path):
            for s in json.load(open(path)):
                self.proven.setdefault(int(s['address'], 16), s['name'])
        for f in sorted(glob.glob(os.path.join(DATA, 'gaps-*.json'))):
            if f.endswith('-classified.json'):
                continue
            for g in json.load(open(f)):
                if g.get('address') and g.get('similarity') is not None:
                    a = int(g['address'], 16)
                    old = self.guessed.get(a)
                    if not old or g['similarity'] > old[1]:
                        self.guessed[a] = (g['function'], g['similarity'])
        self.thunks = img.incremental_thunks()
        self._starts = None

    def starts(self):
        if self._starts is None:
            s = set(self.img.function_starts()[0]) | set(self.proven) | set(self.guessed)
            self._starts = sorted(s)
        return self._starts

    def of(self, va):
        """Name of an address, or None."""
        if va in self.thunks:
            target = self.thunks[va]
            return f'j_{self.of(target) or f"0x{target:X}"}'
        if va in self.proven:
            return self.proven[va]
        if va in self.guessed:
            return f'~{self.guessed[va][0]}?'
        return None

    def containing(self, va):
        """(function start, its name) for an address in .text."""
        st = self.starts()
        i = bisect.bisect_right(st, va) - 1
        if i < 0:
            return None, None
        return st[i], self.of(st[i])

    def describe(self, va):
        parts = []
        n = self.of(va)
        if n:
            parts.append(n)
        sec = section_of(self.img, va)
        if sec == '.text' and not n:
            f, fn = self.containing(va)
            if f is not None:
                parts.append(f'in {fn or "?"} (0x{f:X}+0x{va - f:X})')
        elif sec in ('.rdata', '.data') and self.img.offset(va) is not None:
            s = self.img.cstr(va, 200)
            if s and len(s) >= 3 and all(32 <= ord(c) < 127 or c in '\t\r\n' for c in s):
                parts.append(repr(s))
            else:
                target = self.img.u32(va)
                tn = self.of(target)
                if tn:
                    parts.append(f'-> {tn}')
        return ', '.join(parts) if parts else None


def section_of(img, va):
    """Section by virtual size, so uninitialised data (.data past its raw bytes) counts too."""
    for name, (start, vsize, raw, rsize) in img.sections.items():
        if start <= va < start + max(vsize, rsize):
            return name
    return None


HEX = re.compile(r'0x[0-9a-f]{5,8}')


def annotate(names, ins):
    """Names for the addresses an instruction mentions: call targets and data, not local jumps."""
    notes = []
    for m in HEX.findall(ins.op_str):
        va = int(m, 16)
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.mnemonic != 'jmp'):
            d = names.of(va) if ins.mnemonic == 'call' else None
        elif ins.mnemonic == 'jmp':
            d = names.of(va)
        else:
            d = names.describe(va)
        if d:
            notes.append(f'{m}={d}')
    return '; '.join(notes)


def cmd_dis(img, names, a):
    va = int(a.addr, 16)
    size = a.len or 0x4000
    last_target = va
    for ins in img.disasm(va, size):
        note = annotate(names, ins)
        print(f'{ins.address:08X}  {ins.mnemonic:6s} {ins.op_str}' + (f'    ; {note}' if note else ''))
        if ins.mnemonic.startswith('j') and HEX.fullmatch(ins.op_str.strip() or 'x'):
            last_target = max(last_target, int(ins.op_str, 16))
        if a.len:
            continue
        # end of the function: a return or jump past every branch target seen so far
        if ins.mnemonic in ('ret', 'jmp') and ins.address >= last_target:
            nxt = ins.address + ins.size
            if ins.mnemonic == 'ret' or not (va <= last_target < nxt):
                break


def cmd_name(img, names, a):
    for x in a.addr:
        va = int(x, 16)
        print(f'0x{va:X}  {section_of(img, va) or "-"}  {names.describe(va) or "?"}')


def cmd_refs(img, names, a):
    want = {struct.pack('<I', int(x, 16)): int(x, 16) for x in a.addr}
    for needle, target in want.items():
        print(f'0x{target:X} ({names.of(target) or "?"}):')
        for name in img.sections:
            start, end, raw = img.section(name)
            blob = img.data[raw:raw + (end - start)]
            i = blob.find(needle)
            while i >= 0:
                va = start + i
                where = ''
                if name == '.text':
                    f, fn = names.containing(va)
                    where = f'  in {fn or "?"} (0x{f:X})' if f is not None else ''
                elif name in ('.rdata', '.data'):
                    where = '  (vtable or data)'
                print(f'   {name:6s} 0x{va:X}{where}')
                i = blob.find(needle, i + 1)


def cmd_callers(img, names, a):
    start, end, raw = img.section('.text')
    body = img.data[raw:raw + (end - start)]
    targets = {}
    for x in a.addr:
        t = int(x, 16)
        targets[t] = t
        for th, dst in names.thunks.items():
            if dst == t:
                targets[th] = t
    hits = {t: [] for t in set(targets.values())}
    for m in re.finditer(rb'[\xE8\xE9]', body):
        o = m.start()
        if o + 5 > len(body):
            break
        dst = (start + o + 5 + struct.unpack_from('<i', body, o + 1)[0]) & 0xFFFFFFFF
        if dst in targets and (start + o) not in names.thunks:
            hits[targets[dst]].append((start + o, body[o] == 0xE9, dst != targets[dst]))
    for t, lst in hits.items():
        print(f'0x{t:X} ({names.of(t) or "?"}): {len(lst)} caller(s)')
        for va, is_jmp, via_thunk in lst:
            f, fn = names.containing(va)
            kind = 'jmp' if is_jmp else 'call'
            print(f'   0x{va:X} {kind}{" (thunk)" if via_thunk else ""}  in {fn or "?"} (0x{f:X})')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    sub = ap.add_subparsers(dest='cmd', required=True)
    d = sub.add_parser('dis')
    d.add_argument('addr')
    d.add_argument('--len', type=lambda s: int(s, 0), help='bytes to disassemble (default: to the function end)')
    for c in ('name', 'refs', 'callers'):
        sub.add_parser(c).add_argument('addr', nargs='+')
    a = ap.parse_args()
    img = ClientImage(a.client)
    names = Names(img)
    {'dis': cmd_dis, 'name': cmd_name, 'refs': cmd_refs, 'callers': cmd_callers}[a.cmd](img, names, a)


if __name__ == '__main__':
    main()
