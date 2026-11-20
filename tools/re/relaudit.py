"""Relocation audit of exact functions: byte matching masks relocated fields, so a function can be credited exact
while it references other globals than the client's (PAD, run 2026-11-19b).

    python tools/re/relaudit.py [OBJ ...] [--objs DIR] [--only SUBSTR] [--all-symbols] [--log PATH]

For every function of the objects that objmatch credits exact (tools/re/trybatch.py's exact_set: the crediting of
`appfile.py match`), each 32-bit absolute relocation (DIR32) is read back from client.exe at the credited address:
our symbol -> the client's address. A symbol read at two or more client addresses is a disagreement; the address
most functions agree on is taken as the symbol's (majority), and every function reading another one is listed as
a suspect: `0xFUNC  symbol  client 0xVAL  (majority 0xVAL of N functions)`. External symbols only (statics and
compiler labels are per object), string literals and floating constants left out (the client keeps a copy per
object), unless --all-symbols. Default objects: build/app/obj/*.obj (~7 min for the app). A `const int` read at
several addresses (Unknown_B08C90..CC0) is the client's per-object copy of a const (internal linkage) where ours
is one extern: a declaration difference, not a false credit.

Cases found on run/2026-11-19b (build/app/obj of 2026-11-18; rebuild first, a stale object reports its old form):
the Delta numbering initialisers co 0x707A20, radar_system 0x7BDEA0, ship_stats_display 0x6A3CA0 (their stores in
another class order than the client's: the client numbers DeltaItemArrayClass<DeltaAttachmentClass> first; fixed by
auxroots.py's SPLIT_SWAP, PAD, but co's last three: the manufacturing classes and Faction), co 0x6FD7A0, 0x722770,
0x7228C0 and 0x685E80 reading WW3DAssetManager::TheInstance where the client reads 0xBDADC0 (another asset manager
pointer), ShaderClass::_PresetOpaqueShader read 4 bytes low at 0x492470 / 0x4941A0, GadgetClass's type descriptor
at 0x56DF30, and the const ints Unknown_B08C90..CC0 (per-object copies in the client). Prints a summary line; the
detail goes to --log (default .work/log/relaudit.log).
"""
import argparse
import collections
import glob
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
DIR32 = 6
# string literals and floating constants: the client keeps a copy per object (no cross-object pooling), so one
# literal read at several client addresses is no disagreement
LITERALS = ('??_C@', '__real@')


def audit(paths, all_symbols=False, img=None):
    """{symbol: {client address: [(our function symbol, client function address), ...]}}"""
    import trybatch
    from objmatch import CoffObject, _reloc_targets
    from clientimage import ClientImage
    img = img or ClientImage()
    seen = collections.defaultdict(lambda: collections.defaultdict(list))
    for p in paths:
        o = CoffObject(p)
        fn = {f[0]: f for f in o.functions()}
        for s, a in trybatch.exact_set(p, img):
            f = fn.get(s)
            if not f:
                continue
            for off, n, typ, sym, addend in _reloc_targets(o, f[1], f[2], f[3] - f[2]):
                if typ != DIR32 or not sym or n != 4 or (not all_symbols and (
                        sym.get('class') != 2 or sym['name'].startswith(LITERALS))):
                    continue
                v = struct.unpack_from('<I', img.data, img.offset(a + off))[0] - struct.unpack('<I', addend)[0]
                seen[sym['name']][v & 0xFFFFFFFF].append((s, a))
    return seen


def suspects(seen, only=None):
    """[(function address, symbol, client value, majority value, majority count)] sorted by function."""
    out = []
    for sym, vals in seen.items():
        if len(vals) < 2 or (only and only not in sym):
            continue
        count = {v: len({a for _, a in fs}) for v, fs in vals.items()}
        major = max(count, key=lambda v: (count[v], -v))
        for v, fs in vals.items():
            if v != major:
                for a in sorted({a for _, a in fs}):
                    out.append((a, sym, v, major, count[major]))
    return sorted(out)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('objs', nargs='*')
    ap.add_argument('--objs', dest='objdir', default=os.path.join(REPO, 'build', 'app', 'obj'))
    ap.add_argument('--only', help='symbols containing this text only')
    ap.add_argument('--all-symbols', action='store_true', help='statics and compiler labels too')
    ap.add_argument('--log', default=os.path.join(REPO, '.work', 'log', 'relaudit.log'))
    a = ap.parse_args(argv)
    paths = [os.path.abspath(p) for p in a.objs] or sorted(glob.glob(os.path.join(a.objdir, '*.obj')))
    sus = suspects(audit(paths, a.all_symbols), a.only)
    os.makedirs(os.path.dirname(os.path.abspath(a.log)), exist_ok=True)
    with open(a.log, 'w', encoding='utf-8') as f:
        for fa, sym, v, major, n in sus:
            f.write('0x%06X  %s  client 0x%06X  (majority 0x%06X of %d functions)\n' % (fa, sym, v, major, n))
    funcs = sorted({x[0] for x in sus})
    print('relaudit: %d objects, %d suspect functions (%s%s)  (log %s)' % (
        len(paths), len(funcs), ', '.join('0x%06X' % x for x in funcs[:12]), ', ...' if len(funcs) > 12 else '', a.log))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
