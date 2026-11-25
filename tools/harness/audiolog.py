"""The audio event log (test-plan C2): the MSS32.DLL logging proxy's build and its log's reader.

    python tools/harness/audiolog.py gen [--dll PATH]          regenerate mss32log.c's forwarded-export block
    python tools/harness/audiolog.py build [--pool 1]          mss32log.c -> build/mss32log/mss32.dll (VC6 VM)
    python tools/harness/audiolog.py summary LOG [--data DIR]  events by kind, the distinct sounds (named)
    python tools/harness/audiolog.py sequence LOG [--data DIR] the ordered start / stop list, timing stripped
    python tools/harness/audiolog.py diff A B [--all] [--time-tol MS] [--float-tol X] [--data DIR]
                                                               the first difference of two logs' sequences

LOG is an audio.jsonl (e2e.py --audio leaves it in the run folder) or a run folder holding one.

The proxy (tools/harness/mss32log.c) is placed by clientrun.Inst.use_audio_log in an instance folder: the game's
Mss32.dll copied there as mss32r.dll, the proxy over Mss32.dll, MSS32LOG_FILE naming the log. It exports every
one of the real DLL's 344 names (Miles 6.1a), logs the client's calls of the ones written out in mss32log.c and
jumps straight to the real DLL for the rest. `gen` writes the jump block from the DLL's export table (names and
argument bytes from the decorated names); `build` compiles it the way run_reference.py builds d3d8shot and
checks that the built DLL exports exactly the real one's names (a wrapper with the wrong argument bytes would
export another name).

Names: a memory sample is logged by content (size and FNV-1a 64 hash); --data DIR (default machine.py
client_data's Data) names it: every loose .wav / .mp3 under DIR and every entry of DIR's MIX1 mix files whose
size matches a logged sample is hashed, and the matches cached in .work/audio-names.json.

sequence: one line per start / stop / end / pause / resume / close event: the event, the handle's kind (s 2D
sample, t 3D sample, m stream) and its sound (the stream's file, the sample's name or hash:size). It is what
AU-01 compares; --kinds mt keeps only those handle kinds (retail's 2D UI sounds follow the virtual cursor's
hovers, so their order and count vary run to run: README "Audio log"). diff --time-tol MS also compares each
item's time (ms since the log opened) within MS; diff --counts compares each sound's start / stop counts with
the order ignored; diff --all compares every event's fields instead (i, t, tick, f and the position fields left
out; floats within --float-tol, default 0).
"""
import argparse
import json
import os
import re
import shutil
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(HERE, 'mss32log.c')
OUT_DIR = os.path.join(REPO, 'build', 'mss32log')
NAMES = os.path.join(REPO, '.work', 'audio-names.json')
LOG_NAME = 'audio.jsonl'

SEQ_EVENTS = ('start', 'stop', 'end', 'pause', 'resume', 'close')
VOLATILE = {'i', 't', 'tick', 'f', 'pos', 'len', 'pos_ms', 'len_ms'}


def _machine(key):
    sys.path.insert(0, HERE)
    import machine
    return machine.get(key)


def real_dll():
    return os.path.join(_machine('client_data'), 'release', 'Mss32.dll')


# ------------------------------------------------------------------ gen / build

def logged_names(src=None):
    """The C names mss32log.c writes out itself (D ... NAME( and SETTER(NAME, ...))."""
    text = open(src or SRC, encoding='utf-8').read()
    text = re.sub(r'/\* GENERATED-BEGIN.*?GENERATED-END \*/', '', text, flags=re.S)
    names = set(re.findall(r'^D [\w ]+?\b__stdcall (\w+)\(', text, flags=re.M))
    names |= set(re.findall(r'^SETTER\((\w+),', text, flags=re.M))
    return names


def exports(dll):
    """[(ordinal, exported name)] of a PE's export table (a small reader: no pefile needed)."""
    data = open(dll, 'rb').read()
    pe = struct.unpack_from('<I', data, 0x3C)[0]
    nsec = struct.unpack_from('<H', data, pe + 6)[0]
    opt = pe + 24
    exp_rva = struct.unpack_from('<I', data, opt + 96)[0]
    secs = []
    for k in range(nsec):
        o = opt + struct.unpack_from('<H', data, pe + 20)[0] + 40 * k
        vsize, va, rsize, raw = struct.unpack_from('<IIII', data, o + 8)
        secs.append((va, max(vsize, rsize), raw))

    def off(rva):
        for va, size, raw in secs:
            if va <= rva < va + size:
                return rva - va + raw
        raise ValueError(f'rva {rva:#x} outside the sections')
    e = off(exp_rva)
    base, nfun, nnames, afun, anames, aords = struct.unpack_from('<IIIIII', data, e + 16)
    out = []
    for k in range(nnames):
        nrva = struct.unpack_from('<I', data, off(anames) + 4 * k)[0]
        o = off(nrva)
        name = data[o:data.index(b'\0', o)].decode('ascii')
        out.append((base + struct.unpack_from('<H', data, off(aords) + 2 * k)[0], name))
    return sorted(out)


def c_form(name):
    """An exported name -> (convention, C name, argument bytes)."""
    m = re.fullmatch(r'_(\w+)@(\d+)', name)
    if m:
        return '__stdcall', m.group(1), int(m.group(2))
    m = re.fullmatch(r'@(\w+)@(\d+)', name)
    if m:
        return '__fastcall', m.group(1), int(m.group(2))
    if re.fullmatch(r'[A-Za-z_]\w*', name):
        return '__cdecl', name, 0
    raise ValueError(f'{name}: not a C export name')


def gen(dll):
    logged = logged_names()
    lines = ['/* GENERATED-BEGIN: python tools/harness/audiolog.py gen (from the game\'s Mss32.dll exports) */']
    table = []
    seen = set()
    for _, name in exports(dll):
        conv, cname, nbytes = c_form(name)
        lines.append(f'static FARPROC p_{cname};')
        table.append(f'    {{"{name}", &p_{cname}}},')
        if cname in logged:
            seen.add(cname)
            continue
        if conv == '__fastcall' and nbytes:
            raise SystemExit(f'{name}: a fastcall export with arguments: write its thunk by hand')
        args = ', '.join(f'int a{k}' for k in range(nbytes // 4)) or 'void'
        lines.append(f'D __declspec(naked) void {conv} {cname}({args}) {{ __asm jmp dword ptr [p_{cname}] }}')
    unknown = sorted(logged - seen)
    if unknown:
        raise SystemExit(f'mss32log.c writes out names the DLL does not export: {unknown}')
    lines.append('static const struct { const char *name; FARPROC *p; } Table[] = {')
    lines += table
    lines.append('};')
    lines.append('/* GENERATED-END */')
    text = open(SRC, encoding='utf-8').read()
    new = re.sub(r'/\* GENERATED-BEGIN.*?GENERATED-END \*/', lambda m: '\n'.join(lines), text, flags=re.S)
    with open(SRC, 'w', encoding='utf-8', newline='\n') as f:
        f.write(new)
    print(f'{SRC}: {len(table)} exports, {len(seen)} logged, {len(table) - len(seen)} forwarded')


def build(pool):
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import vc6
    import importer as I
    os.makedirs(OUT_DIR, exist_ok=True)
    shutil.copy(SRC, os.path.join(OUT_DIR, 'mss32log.c'))
    for f in ('mss32.dll', 'build.log', 'exports.txt'):
        if os.path.exists(os.path.join(OUT_DIR, f)):
            os.remove(os.path.join(OUT_DIR, f))

    def go():
        w = vc6.WORK + '\\mss32log'
        vc6.run_bat([f'if exist "{w}" rmdir /s /q "{w}"', f'mkdir "{w}"'])
        vc6.scp(os.path.join(OUT_DIR, 'mss32log.c'), vc6.remote(w + '\\mss32log.c'))
        vc6.run_bat([f'cd /d "{w}"',
                     'cl /nologo /c /MT /O2 /W3 mss32log.c > build.log 2>&1',
                     'link /nologo /dll /machine:ix86 /out:mss32.dll mss32log.obj kernel32.lib >> build.log 2>&1',
                     'dumpbin /exports mss32.dll > exports.txt 2>&1'])
        for f in ('mss32.dll', 'build.log', 'exports.txt'):
            try:
                vc6.scp(vc6.remote(w + '\\' + f), os.path.join(OUT_DIR, f))
            except BaseException as e:      # noqa: BLE001 - report what the VM produced
                print(f'{f}: not copied ({e})')
        vc6.ssh(f'cmd /c rmdir /s /q "{w}"', check=False)

    if pool:
        with I.pool_lease(pool):
            go()
    else:
        go()
    print(open(os.path.join(OUT_DIR, 'build.log'), encoding='latin1').read().strip()[-1500:])
    built = os.path.join(OUT_DIR, 'mss32.dll')
    if not os.path.exists(built):
        raise SystemExit('no mss32.dll built')
    mine, real = exports(built), exports(real_dll())
    names_m, names_r = {n for _, n in mine}, {n for _, n in real}
    print(f'exports: {len(mine)} built, {len(real)} real; missing {sorted(names_r - names_m)[:10]}, '
          f'extra {sorted(names_m - names_r)[:10]}')
    same_ord = sum(1 for a, b in zip(mine, real) if a == b)
    print(f'ordinals: {same_ord} of {len(real)} the same')
    if names_m != names_r:
        raise SystemExit('the export names differ from the real DLL\'s')


# ------------------------------------------------------------------ the log

def load(path):
    if os.path.isdir(path):
        path = os.path.join(path, LOG_NAME)
    events = []
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                events.append(json.loads(line))
            except ValueError:
                events.append({'ev': 'bad-line', 'text': line[:200]})   # a line cut by the client's kill
    return events


def fnv64(data):
    x = 0xcbf29ce484222325
    for b in data:
        x = ((x ^ b) * 0x100000001b3) & 0xFFFFFFFFFFFFFFFF
    return f'{x:016x}'


def _mix_entries(path):
    """[(name, offset, size)] of a MIX1 file (Renegade's mix format: header offset, names offset)."""
    with open(path, 'rb') as f:
        head = f.read(16)
        if head[:4] != b'MIX1':
            return []
        ho, no = struct.unpack_from('<II', head, 4)
        f.seek(ho)
        n = struct.unpack('<I', f.read(4))[0]
        ents = [struct.unpack('<III', f.read(12)) for _ in range(n)]
        f.seek(no)
        m = struct.unpack('<I', f.read(4))[0]
        names = []
        for _ in range(m):
            ln = f.read(1)[0]
            names.append(f.read(ln).rstrip(b'\0').decode('latin1'))
    return [(nm, off, sz) for nm, (_, off, sz) in zip(names, ents)]


def namer(events, data_dir):
    """{'hash:size': name} for the memory samples of the log, from data_dir's loose files and mixes (cached)."""
    want = {}
    for e in events:
        if e.get('ev') == 'load' and 'hash' in e:
            want.setdefault(e['size'], set()).add(e['hash'])
    try:
        cache = json.load(open(NAMES, encoding='utf-8'))
    except (OSError, ValueError):
        cache = {}
    found = {k: v for k, v in cache.items() if int(k.split(':')[1]) in want and k.split(':')[0] in want[int(k.split(':')[1])]}
    left = {(h, s) for s, hs in want.items() for h in hs if f'{h}:{s}' not in found}
    if not left or not data_dir or not os.path.isdir(data_dir):
        return found
    sizes = {s for _, s in left}
    for top, _, files in os.walk(data_dir):
        for fn in files:
            p = os.path.join(top, fn)
            low = fn.lower()
            if low.endswith(('.wav', '.mp3')):
                if os.path.getsize(p) in sizes:
                    with open(p, 'rb') as f:
                        k = f'{fnv64(f.read())}:{os.path.getsize(p)}'
                    found.setdefault(k, os.path.relpath(p, data_dir).replace('\\', '/'))
            elif low.endswith('.mix'):
                try:
                    ents = _mix_entries(p)
                except (OSError, struct.error, IndexError):
                    continue
                with open(p, 'rb') as f:
                    for nm, off, sz in ents:
                        if sz in sizes:
                            f.seek(off)
                            k = f'{fnv64(f.read(sz))}:{sz}'
                            found.setdefault(k, nm)
    cache.update(found)
    os.makedirs(os.path.dirname(NAMES), exist_ok=True)
    with open(NAMES, 'w', encoding='utf-8') as f:
        json.dump(cache, f, indent=0, sort_keys=True)
    return found


def _kind(h):
    return h[0] if isinstance(h, str) and h[:1].isalpha() else '?'


def sound(e, names):
    snd = e.get('snd', '')
    return names.get(snd, snd)


def sequence(events, names=None, kinds=None):
    """[(event, kind, sound, t)] of the start / stop events, in order; kinds: only these handle kinds ('mt')."""
    names = names or {}
    return [(e['ev'], _kind(e.get('h')), sound(e, names), e.get('t', 0))
            for e in events if e.get('ev') in SEQ_EVENTS and (not kinds or _kind(e.get('h')) in kinds)]


def counts(events, names=None, kinds=None):
    """{(event, kind, sound): how many} of the sequence: what two runs share when only the order varies."""
    out = {}
    for item in sequence(events, names, kinds):
        out[item[:3]] = out.get(item[:3], 0) + 1
    return out


def summary(events, names=None):
    names = names or {}
    kinds, sounds = {}, {}
    for e in events:
        kinds[e.get('ev')] = kinds.get(e.get('ev'), 0) + 1
        if e.get('ev') == 'load' and 'hash' in e:
            k = f'{e["hash"]}:{e["size"]}'
            sounds.setdefault(k, {'name': names.get(k, ''), 'kind': _kind(e.get('h')), 'loads': 0, 'starts': 0})
            sounds[k]['loads'] += 1
        elif e.get('ev') == 'open' and e.get('file'):
            sounds.setdefault(e['file'], {'name': e['file'], 'kind': 'm', 'loads': 0, 'starts': 0})
            sounds[e['file']]['loads'] += 1
        elif e.get('ev') == 'start' and e.get('snd'):
            sounds.setdefault(e['snd'], {'name': names.get(e['snd'], ''), 'kind': _kind(e.get('h')),
                                         'loads': 0, 'starts': 0})
            sounds[e['snd']]['starts'] += 1
    return kinds, sounds


def _close(a, b, tol):
    if isinstance(a, (int, float)) and isinstance(b, (int, float)) and not isinstance(a, bool):
        return abs(a - b) <= tol
    return a == b


def diff(ea, eb, names=None, all_fields=False, time_tol=None, float_tol=0.0, kinds=None, by_count=False):
    """None when the two logs agree, else (index, item of A or None, item of B or None, why). by_count: the
    per-sound counts of the sequence, order ignored (index is the differing key's rank, items the counts)."""
    names = names or {}
    if by_count:
        ca, cb = counts(ea, names, kinds), counts(eb, names, kinds)
        for k, key in enumerate(sorted(set(ca) | set(cb), key=lambda x: (x[2], x[1], x[0]))):
            if ca.get(key, 0) != cb.get(key, 0):
                return k, key + (ca.get(key, 0),), key + (cb.get(key, 0),), 'different count'
        return None
    if all_fields:
        ia = [e for e in ea if e.get('ev') != 'log']
        ib = [e for e in eb if e.get('ev') != 'log']
        for k, (a, b) in enumerate(zip(ia, ib)):
            keys = (set(a) | set(b)) - VOLATILE
            for key in sorted(keys):
                va, vb = a.get(key), b.get(key)
                if key == 'snd':
                    va, vb = names.get(va, va), names.get(vb, vb)
                tol = float_tol if isinstance(va, float) or isinstance(vb, float) else 0
                if not _close(va, vb, tol):
                    return k, a, b, f'{key}: {va!r} != {vb!r}'
            if time_tol is not None and abs(a.get('t', 0) - b.get('t', 0)) > time_tol:
                return k, a, b, f't: {a.get("t")} vs {b.get("t")} (more than {time_tol} ms apart)'
        n = min(len(ia), len(ib))
        if len(ia) != len(ib):
            return n, ia[n] if n < len(ia) else None, ib[n] if n < len(ib) else None, 'one log ends first'
        return None
    sa, sb = sequence(ea, names, kinds), sequence(eb, names, kinds)
    for k, (a, b) in enumerate(zip(sa, sb)):
        if a[:3] != b[:3]:
            return k, a, b, 'different event'
        if time_tol is not None and abs(a[3] - b[3]) > time_tol:
            return k, a, b, f't: {a[3]} vs {b[3]} (more than {time_tol} ms apart)'
    n = min(len(sa), len(sb))
    if len(sa) != len(sb):
        return n, sa[n] if n < len(sa) else None, sb[n] if n < len(sb) else None, 'one sequence ends first'
    return None


def _fmt(item, count=False):
    if item is None:
        return '-'
    if isinstance(item, tuple) and count:
        return f'{item[0]:<7} {item[1]} {item[2]}  x {item[3]}'
    if isinstance(item, tuple):
        return f'{item[0]:<7} {item[1]} {item[2]}  (t {item[3]} ms)'
    return json.dumps(item)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    g = sub.add_parser('gen')
    g.add_argument('--dll')
    b = sub.add_parser('build')
    b.add_argument('--pool', type=int, default=1)
    for name in ('summary', 'sequence'):
        s = sub.add_parser(name)
        s.add_argument('log')
        s.add_argument('--data')
        s.add_argument('--no-names', action='store_true')
        s.add_argument('--kinds', help='only these handle kinds: s 2D sample, t 3D sample, m stream (e.g. mt)')
    d = sub.add_parser('diff')
    d.add_argument('a')
    d.add_argument('b')
    d.add_argument('--all', action='store_true')
    d.add_argument('--time-tol', type=float)
    d.add_argument('--float-tol', type=float, default=0.0)
    d.add_argument('--data')
    d.add_argument('--no-names', action='store_true')
    d.add_argument('--kinds', help='only these handle kinds: s 2D sample, t 3D sample, m stream (e.g. mt)')
    d.add_argument('--counts', action='store_true', help='compare each sound\'s start / stop counts, not the order')
    a = ap.parse_args()
    if a.cmd == 'gen':
        return gen(a.dll or real_dll())
    if a.cmd == 'build':
        return build(a.pool)
    data = None if a.no_names else (a.data or os.path.join(_machine('client_data'), 'Data'))
    if a.cmd == 'diff':
        ea, eb = load(a.a), load(a.b)
        names = namer(ea + eb, data)
        r = diff(ea, eb, names, a.all, a.time_tol, a.float_tol, a.kinds, a.counts)
        n = len(sequence(ea, None, a.kinds)) if not a.all else len(ea)
        if r is None:
            print(f'same: {n} {"events" if a.all else "sequence items"}')
            return 0
        k, x, y, why = r
        print(f'first difference at item {k} ({why}):\n  A {_fmt(x, a.counts)}\n  B {_fmt(y, a.counts)}')
        return 1
    ev = load(a.log)
    names = namer(ev, data)
    if a.cmd == 'sequence':
        for item in sequence(ev, names, a.kinds):
            print(f'{item[0]:<7} {item[1]} {item[2]}')
        return 0
    kinds, sounds = summary(ev, names)
    print(f'{len(ev)} events: ' + ', '.join(f'{k} {v}' for k, v in sorted(kinds.items(), key=lambda x: -x[1])))
    print(f'{len(sounds)} distinct sounds:')
    for k, s in sorted(sounds.items(), key=lambda x: (-x[1]['starts'], x[0])):
        print(f'  {s["kind"]} starts {s["starts"]:>4} loads {s["loads"]:>4}  {s["name"] or k}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
