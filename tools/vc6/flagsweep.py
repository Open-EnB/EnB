"""Rules compiler switches out for one near miss: every /O x /Ob x /G combination, ranked.

    python flagsweep.py FILE SYMBOL ADDRESS [--jobs N] [--o /O1,/O2] [--ob /Ob0,/Ob1,/Ob2]
                        [--g /G3,/G4,/G5,/G6,/GB] [--keep]

FILE is a tree source (`app/common/tcpconn.cpp`, `ww3d2/ww3d.cpp`); SYMBOL and ADDRESS as for
objdiff.py / explain.py. The file is compiled as it is in the tree (nothing is edited, so it is not
copied: an app file needs its relative includes and its `e:\\build\\beta\\G` __FILE__ alias) with
its library's flags (importer.LIB_FLAGS: /GX, /MT and the defines stay) and each combination of
the three switch families in place of the library's own (2 x 3 x 5 = 30 compiles, `--jobs`
parallel on the VM, about 10 s each for an app file). Each object is byte-matched at ADDRESS
(objmatch's pattern) and classified by tools/re/explain.py. The table, best first:

    flags        exact  class           residue  regs   (* = the library's own switches)

VMs: ENB_VC6_HOSTS / ENB_VC6_HOST or .work/vc6_hosts.json, as vc6.py; lease one from the pool
(`pool.py lease --agent NAME --count 1`) and release it. Objects go to .work/flagsweep/<file>/<flags>/
(deleted unless --keep). Read-only on the tree; it does not take the importer lock. `--source COPY`
sweeps an untracked form instead (a parked attempt's copy, headers it overrides beside it, as
.work/c/tv.py's out-of-tree copies; FILE still gives library and flags). Not for app files: their
relative includes and __FILE__ alias need the tree location.

Evidence so far (record yours in docs/workflow.md §3.1): no switch combination has reproduced
retail where the source form was wrong. Make_Screen_Shot 0x9182C0 (2026-10-25 /G3 /G4 /G5 /G6 /GB:
no change except /G6, far off; 2026-10-26, the full grid on builder C's U form with --source: the
library's own /O2 /Ob2 /GB is best, 5 masked rows register-only, tied by /Ob1 and /G3-/G5; /Ob0 13,
/G6 112, every /O1 400 or more) and Open-BFME's
113 near misses x every /O /Ob /G /Oy combination (MSVC 7.1, `.work/audit/openbfme.md`): an
allocation residue comes from the source, not the switches. Run this once to rule switches out,
then work on the source form; never loop it.
"""
import argparse
import concurrent.futures as cf
import itertools
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import importer  # noqa: E402
import vc6  # noqa: E402
import explain  # noqa: E402
from objmatch import CoffObject, demangle  # noqa: E402

SWITCH = re.compile(r'\s/(O1|O2|Ox|Od|Ob[012]|G[3456B])(?=\s|$)')


def base_flags(path):
    return importer.flags_for([path])


def with_switches(flags, combo):
    return SWITCH.sub('', ' ' + flags).strip() + ' ' + ' '.join(combo)


def own_switches(flags):
    """The library's own switches, completed with VC6's defaults (/O1 and /O2 imply /Ob1; /GB is
    the default processor model)."""
    found = [m.group(1) for m in SWITCH.finditer(' ' + flags)]
    o = next((f'/{x}' for x in found if x in ('O1', 'O2')), '/O2')
    ob = next((f'/{x}' for x in found if x.startswith('Ob')), '/Ob1')
    g = next((f'/{x}' for x in found if x.startswith('G')), '/GB')
    return (o, ob, g)


def compile_one(path, flags, out, lib_path=None):
    if os.path.isdir(out):
        shutil.rmtree(out)
    os.makedirs(out)
    app = importer.APP_LIBS.get(importer.lib_of(lib_path or path))
    if app:
        root = importer.dest(importer.lib_of(lib_path or path))
        rc, text = vc6.compile_sources([path], importer.tree_includes(), defines=app['defines'], out=out,
                                       flags=flags, extra_sync=[d for d, _, _ in os.walk(root)] + importer.late_sync(),
                                       source_alias=(REPO, app['file_root']))
    else:
        rc, text = vc6.compile_sources([path], importer.tree_includes(), out=out, flags=flags,
                                       extra_sync=importer.late_sync())
    objs = [os.path.join(out, f) for f in os.listdir(out) if f.lower().endswith('.obj')]
    errors = [l for l in text.splitlines() if ' error ' in l]
    return (objs[0] if objs else None), errors


def exact_at(obj_path, symbol, addr, img):
    obj = CoffObject(obj_path)
    cands = [f for f in obj.functions() if symbol in f[0] or symbol in demangle(f[0])]
    if not cands:
        return False
    name, sec, s, e = cands[0]
    pat, _, length = obj.pattern(sec, s, e)
    o = img.offset(addr)
    return re.fullmatch(pat, img.data[o:o + length], re.S) is not None


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('file')
    ap.add_argument('symbol')
    ap.add_argument('address')
    ap.add_argument('--jobs', type=int, default=4)
    ap.add_argument('--o', default='/O1,/O2')
    ap.add_argument('--ob', default='/Ob0,/Ob1,/Ob2')
    ap.add_argument('--g', default='/G3,/G4,/G5,/G6,/GB')
    ap.add_argument('--keep', action='store_true')
    ap.add_argument('--source', help='compile this copy (an untracked form, e.g. .work/c/mss_out/U/ww3d.cpp, '
                    'with its own headers beside it) instead of FILE; FILE gives the library and flags')
    a = ap.parse_args()
    path = os.path.abspath(a.file if os.path.isabs(a.file) else os.path.join(REPO, a.file))
    if not os.path.isfile(path):
        raise SystemExit(f'{a.file}: no such file')
    src = os.path.abspath(a.source) if a.source else path
    if not os.path.isfile(src):
        raise SystemExit(f'{a.source}: no such file')
    flags0 = base_flags(path)
    own = own_switches(flags0)
    combos = list(itertools.product(a.o.split(','), a.ob.split(','), a.g.split(',')))
    work = os.path.join(REPO, '.work', 'flagsweep', os.path.splitext(os.path.basename(path))[0])
    img = explain.ClientImage()
    hosts = vc6.hosts()
    print(f'{os.path.relpath(src, REPO)}: {len(combos)} combinations of {flags0!r}\'s switches on {", ".join(hosts)}',
          flush=True)

    def run(k, combo):
        vc6.use_host(hosts[k % len(hosts)])
        tag = ''.join(combo).replace('/', '')
        obj, errors = compile_one(src, with_switches(flags0, combo), os.path.join(work, tag), lib_path=path)
        if errors or not obj:
            return combo, None, errors[:2] or ['no object']
        try:
            res, _ = explain.explain(obj, a.symbol, a.address)
        except SystemExit as e:
            return combo, None, [str(e)]
        return combo, (exact_at(obj, a.symbol, int(a.address, 16), img), res), []

    # the first compile alone: vc6.sync's tar has one name per VM, so parallel first syncs collide
    rows = [run(0, combos[0])]
    with cf.ThreadPoolExecutor(max(1, a.jobs)) as ex:
        for combo, got, errors in ex.map(lambda kc: run(*kc), list(enumerate(combos))[1:]):
            rows.append((combo, got, errors))
    ok = [(c, g) for c, g, _ in rows if g]
    ok.sort(key=lambda cg: (not cg[1][0], cg[1][1]['residue'], cg[1][1]['residue_regs'], tuple(cg[0]) != own))
    print(f"{'flags':16s} {'exact':5s}  {'class':15s} {'residue':>7s} {'regs':>5s}")
    for combo, (exact, res) in ok:
        mark = '*' if tuple(combo) == own else ' '
        print(f"{' '.join(combo):15s}{mark} {'yes' if exact else 'no':5s}  {res['class']:15s} "
              f"{res['residue']:7d} {res['residue_regs']:5d}")
    for combo, got, errors in rows:
        if not got:
            print(f"{' '.join(combo):16s} failed: {errors}")
    best = ok[0] if ok else None
    if best:
        own_row = next((g for c, g in ok if tuple(c) == own), None)
        if best[1][0]:
            print(f"exact with {' '.join(best[0])}: check the library's other files before changing LIB_FLAGS")
        elif own_row and best[1][1]['residue'] >= own_row[1]['residue']:
            print('no combination beats the library\'s own switches: the residue is in the source')
        else:
            print(f"best {' '.join(best[0])} ({best[1][1]['residue']} rows) without an exact match: "
                  'a switch moves the residue but does not remove it; work on the source form')
    if not a.keep:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == '__main__':
    main()
