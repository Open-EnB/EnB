"""Tiles the open entries of the runtime-library segments with the functions of the .lib objects.

    python tools/re/rttile.py [--libs DIR] [--out docs/data/rtlib-tiling.json]

The runtime libraries linked into client.exe (d3dx8, libcpmt, libcmt, comsupp) are Microsoft's, not the tree's,
and have no padding between functions: an open entry there is one function or several glued together (the start of
the next is not a call target). At each offset the longest function of the .lib objects (`DIR/o_*/*.obj`, the
libraries unpacked by tools/vc6/runtime_match.sh; default `.work/libs` of the main tree) whose masked pattern
(objmatch CoffObject.pattern) matches is taken; bytes no function covers are counted. An entry tiled end to end is
"explained by its .lib": the library's own code, unproven by name only because the pieces are short, repeated,
or not at a verified start (libraries.md 3 item 10).

Writes docs/data/rtlib-tiling.json, {address: {size, covered, lib, funcs: [[offset, length, lib, object], ...]}}
for every entry of the ranges that symbols.json does not name. symbols.py's coverage.md reads it and counts the
entries that are still open. Ported from .work/week-2026-11-10/lib/rtmatch.py (builder C1, run 2026-11-11b); this
is the ledger's neighbour, not part of it: it changes no (file, address) pair.
"""
import argparse
import collections
import glob
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
MAIN = os.path.normpath(os.path.join(REPO, '..', '..', '..')) if os.sep + '.work' + os.sep in REPO + os.sep else REPO
DATA = os.path.join(REPO, 'docs', 'data')
sys.path.insert(0, HERE)

# the runtime-library ranges of the library region (docs/library-matching.md 3.7; libraries.md 2 (c))
RANGES = [(0x8E3600, 0x909300),     # d3dx8
          (0xA17E50, 0xA420AC)]     # after binkmovie: libcpmt, libcmt, comsupp and what follows
NAME = 'rtlib-tiling.json'


def tile(data, byfirst):
    """Tile `data` with candidates; byfirst maps a first byte (None: a pattern starting with a wildcard) to
    [(length, regex, lib, object)] longest first. Returns (funcs [[offset, length, lib, object]], covered)."""
    size = len(data)
    p = covered = 0
    funcs = []
    while p < size:
        hit = None
        for cand in byfirst.get(data[p], []) + byfirst.get(None, []):
            length = cand[0]
            if p + length <= size and (length >= 5 or p + length == size or length == size) \
                    and cand[1].fullmatch(data, p, p + length):
                hit = cand
                break
        if hit:
            funcs.append([p, hit[0], hit[2], hit[3]])
            covered += hit[0]
            p += hit[0]
        else:
            p += 1
    return funcs, covered


def load_candidates(libs):
    from objmatch import CoffObject
    byfirst = collections.defaultdict(list)
    for d in sorted(glob.glob(os.path.join(libs, 'o_*'))):
        lib = os.path.basename(d)[2:]
        for p in sorted(glob.glob(os.path.join(d, '*.obj'))):
            try:
                obj = CoffObject(p)
                funcs = obj.functions()
            except Exception:   # noqa: BLE001 (an object that does not parse has no functions to offer)
                continue
            for name, sec, s, e in funcs:
                pat, fixed, length = obj.pattern(sec, s, e)
                if length == 0:
                    continue
                info = obj.sections[sec]
                first = obj.data[info['raw'] + s]
                key = None if pat[:1] == b'.' else first
                byfirst[key].append((length, re.compile(pat, re.S), lib, os.path.basename(p)))
    for k in byfirst:
        byfirst[k].sort(key=lambda t: -t[0])
    return byfirst


def open_entries(img, starts, named):
    """(address, bytes) of the verified starts of RANGES that no symbol names; the bytes run to the next start,
    trailing int3 fill dropped."""
    out = []
    for lo, hi in RANGES:
        ss = [s for s in starts if lo <= s < hi]
        for i, a in enumerate(ss):
            if a in named:
                continue
            end = ss[i + 1] if i + 1 < len(ss) else hi
            o = img.offset(a)
            data = img.data[o:o + end - a].rstrip(b'\xcc')
            if data:
                out.append((a, data))
    return out


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument('--libs', default=os.path.join(MAIN, '.work', 'libs'))
    ap.add_argument('--out', default=os.path.join(DATA, NAME))
    a = ap.parse_args(argv)
    from clientimage import ClientImage
    img = ClientImage()
    starts, thunks = img.function_starts()
    named = {int(r['address'], 16) for r in json.load(open(os.path.join(DATA, 'symbols.json'))) if r['kind'] == 'function'}
    byfirst = load_candidates(a.libs)
    out = {}
    for addr, data in open_entries(img, starts, named):
        funcs, covered = tile(data, byfirst)
        out['0x%X' % addr] = {'size': len(data), 'covered': covered, 'lib': funcs[0][2] if funcs else None,
                              'funcs': funcs}
    with open(a.out, 'w', newline='\n') as f:
        f.write('{\n' + ',\n'.join('%s: %s' % (json.dumps(k), json.dumps(v, separators=(',', ':')))
                                   for k, v in out.items()) + '\n}\n')
    full = sum(1 for v in out.values() if v['covered'] == v['size'])
    print('entries %d, fully tiled %d, not at all %d' % (len(out), full, sum(1 for v in out.values() if v['covered'] == 0)))
    return 0


def counts(path, named):
    """{lib: [open entries, explained by the .lib (tiled end to end), bytes, explained bytes]} from
    rtlib-tiling.json, for the entries `named` (a set of addresses) does not name since; None without the file.
    Used by symbols.py's coverage.md."""
    if not os.path.exists(path):
        return None
    out = {}
    for k, v in json.load(open(path)).items():
        if int(k, 16) in named:
            continue
        row = out.setdefault(v['lib'] or 'none', [0, 0, 0, 0])
        row[0] += 1
        row[2] += v['size']
        if v['covered'] == v['size']:
            row[1] += 1
            row[3] += v['size']
    return out


if __name__ == '__main__':
    sys.exit(main())
