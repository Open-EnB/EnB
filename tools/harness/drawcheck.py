"""M2's draw-dump regression check (D78): our vidtest's draw calls against the original's.

    python tools/harness/drawcheck.py compare ORIG.bin OURS.bin [--quiet]   # two draws_NNNNN.bin
    python tools/harness/drawcheck.py run [--frame 900] [--orig-dir DIR] [--ours-dir DIR]
                                          [--redo-orig] [--timeout 120]

`compare` reads two dumps (tools/harness/drawdump.py) and checks, draw by draw in order, everything
but the lighting: the draw's type, counts, stride, FVF, the sizes and formats of its textures
(not their pointers), world / view / projection matrices, render and stage states (except
TEXTUREFACTOR, FOGCOLOR and AMBIENT, which follow the sun and the clock), the indices and the
vertices' positions (x y z of every vertex with D3DFVF_XYZ first). Vertex colours and normals are
left out: the sun turns with GetTickCount, so two runs differ there (docs/vidtest.md 2.4). It prints
the per-FVF/texture group table of both sides when they differ, the first differing draws with the
fields that differ, and a one-line verdict. Exit status 0 only when the two dumps are the same
sequence of draws.

`run` makes the dumps and judges them against drawcheck_baseline.json (see below): the original VIDTEST.EXE (once; reused afterwards) and our
build/programs/vidtest/vidtest.exe (`-nomsgbox`, D69) both run through run_reference.py's proxy
with `--dump FRAME` at 640x480x32 windowed, into build/programs/vidtest/drawcheck/{orig,ours}.
The judgement (`baseline`): ours is not the original yet at frame 900 (the draw order differs: ours
draws ~1,140 more tree draws and its lightmap is X8R8G8B8 where the original's is A8R8G8B8; two of our
own runs differ in 17 of 2,327 draws by the clock), so a draw-by-draw verdict would never pass. The
baseline holds the per-group tables (FVF, stride, textures: draws / prims / vertices) of both sides as
accepted on 2026-10-25; `run` prints PASS when both tables are unchanged, and otherwise the groups that
moved (REGRESSION when ours changed, CHANGED ORIGINAL when the reference did). When ours gets closer to
the original (or farther on purpose), `--update-baseline` rewrites it. `compare` stays the strict check.

Needs `build/d3d8shot/d3d8.dll` (run_reference.py build) and our vidtest.exe (tools/vc6/build.py
link). Images come only from the programs' own back buffers; no screen grabs.
"""
import argparse
import math
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import drawdump  # noqa: E402

OUT = os.path.join(REPO, 'build', 'programs', 'vidtest', 'drawcheck')
OURS_EXE = os.path.join(REPO, 'build', 'programs', 'vidtest', 'vidtest.exe')
SUN_STATES = ('TEXTUREFACTOR', 'FOGCOLOR', 'AMBIENT')
ARGS = ['-nomsgbox', '-resx', '640', '-resy', '480', '-depth', '32', '-windowed', '1']


def differences(a, b):
    """The names of the fields in which two draw records differ (lighting excluded)."""
    out = []
    for k in ('type', 'count', 'prims', 'stride', 'fvf', 'isize', 'min', 'start', 'base'):
        if a[k] != b[k]:
            out.append(f'{k} {a[k]} / {b[k]}')
    if [t[1:] for t in a['tex']] != [t[1:] for t in b['tex']]:
        out.append('texture size/format')
    for k in ('world', 'view', 'proj'):
        if any(abs(x - y) > 1e-3 * max(1.0, abs(x)) for x, y in zip(a[k], b[k])):
            out.append(k)
    if {k: v for k, v in a['states'].items() if k not in SUN_STATES} != \
            {k: v for k, v in b['states'].items() if k not in SUN_STATES}:
        out.append('render states')
    if a['stages'] != b['stages']:
        out.append('stage states')
    if a['indices'] != b['indices']:
        out.append('indices')
    if a['count'] == b['count'] and a['stride'] == b['stride'] and a['stride'] >= 12:
        pa, pb = drawdump.positions(a), drawdump.positions(b)
        if any(math.isnan(x) != math.isnan(y) or abs(x - y) > 1e-3 * max(1.0, abs(x))
               for u, v in zip(pa, pb) for x, y in zip(u, v) if not (math.isnan(x) and math.isnan(y))):
            out.append('vertex positions')
    return out


def groups(draws):
    g = {}
    for d in draws:
        k = (d['fvf'], d['stride'], d['tex'][0][1:], d['tex'][1][1:])
        e = g.setdefault(k, [0, 0, 0])
        e[0] += 1
        e[1] += d['prims']
        e[2] += d['count']
    return g


def compare(orig_path, ours_path, quiet=False):
    """True when the two dumps are the same sequence of draws."""
    a, b = drawdump.read(orig_path), drawdump.read(ours_path)
    ga, gb = groups(a), groups(b)
    bad = []
    for i, (x, y) in enumerate(zip(a, b)):
        diff = differences(x, y)
        if diff:
            bad.append((i, diff))
    same = len(a) == len(b) and not bad and ga == gb
    if not quiet and not same:
        print('group (fvf stride tex0 tex1): original draws/prims/verts | ours')
        for k in sorted(set(ga) | set(gb), key=str):
            mark = '  ' if ga.get(k) == gb.get(k) else '!!'
            print(f'{mark} fvf {k[0]:04X} stride {k[1]:2d} tex0 {k[2]} tex1 {k[3]}: {ga.get(k)} | {gb.get(k)}')
        for i, diff in bad[:10]:
            print(f'draw {i}: ' + '; '.join(diff))
    print(f'draws: original {len(a)}, ours {len(b)}; {len(bad)} of {min(len(a), len(b))} paired draws differ '
          f'(lighting excluded); groups {"equal" if ga == gb else "DIFFER"}: {"SAME" if same else "DIFFERENT"}')
    return same


BASELINE = os.path.join(HERE, 'drawcheck_baseline.json')


def table(draws):
    return {f'fvf {k[0]:04X} stride {k[1]} tex0 {k[2]} tex1 {k[3]}': v for k, v in groups(draws).items()}


def judge(orig_path, ours_path, frame, update=False):
    """Compares the group tables with the baseline; True on PASS."""
    import json
    now = {'frame': frame, 'orig': table(drawdump.read(orig_path)), 'ours': table(drawdump.read(ours_path))}
    if update or not os.path.exists(BASELINE):
        json.dump(now, open(BASELINE, 'w'), indent=1, sort_keys=True)
        print(f'baseline written: {BASELINE}')
        return True
    base = json.load(open(BASELINE))
    if base['frame'] != frame:
        raise SystemExit(f'the baseline is for frame {base["frame"]}, not {frame}')
    ok = True
    for side, label in (('ours', 'REGRESSION (ours changed)'), ('orig', 'CHANGED ORIGINAL (the reference differs)')):
        for k in sorted(set(base[side]) | set(now[side])):
            if base[side].get(k) != now[side].get(k):
                ok = False
                print(f'{label}: {k}: baseline {base[side].get(k)}, now {now[side].get(k)}')
    known = [k for k in sorted(set(base['orig']) | set(base['ours'])) if base['orig'].get(k) != base['ours'].get(k)]
    tot = lambda t: [sum(v[i] for v in t.values()) for i in range(3)]       # noqa: E731
    print(f'frame {frame}: original {tot(now["orig"])} draws/prims/verts in {len(now["orig"])} groups, ours '
          f'{tot(now["ours"])} in {len(now["ours"])}; {len(known)} groups known to differ from the original '
          f'(baseline): {"PASS" if ok else "FAIL"}')
    return ok


def run_one(exe, out, frame, timeout):
    cmd = [sys.executable, os.path.join(HERE, 'run_reference.py'), '--every', '0', '--frames', str(frame),
           '--dump', str(frame), '--out', out, '--timeout', str(timeout)]
    if exe:
        cmd += ['--exe', exe]
    cmd += ['run', '--'] + ARGS
    if not exe:
        cmd.remove('-nomsgbox')      # the original has no such option; it shows no dialog on a clean run
    print(' '.join(cmd))
    subprocess.run(cmd, check=True)
    return os.path.join(out, f'draws_{frame:05d}.bin')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['compare', 'run'])
    ap.add_argument('files', nargs='*')
    ap.add_argument('--frame', type=int, default=900)
    ap.add_argument('--orig-dir', default=os.path.join(OUT, 'orig'))
    ap.add_argument('--ours-dir', default=os.path.join(OUT, 'ours'))
    ap.add_argument('--redo-orig', action='store_true', help='dump the original again')
    ap.add_argument('--timeout', type=float, default=120)
    ap.add_argument('--quiet', action='store_true')
    ap.add_argument('--update-baseline', action='store_true', help='run: accept this run as drawcheck_baseline.json')
    a = ap.parse_args()
    if a.command == 'compare':
        if len(a.files) != 2:
            raise SystemExit(__doc__)
        sys.exit(0 if compare(a.files[0], a.files[1], a.quiet) else 1)
    orig = os.path.join(a.orig_dir, f'draws_{a.frame:05d}.bin')
    if a.redo_orig or not os.path.exists(orig):
        orig = run_one(None, a.orig_dir, a.frame, a.timeout)
    ours = run_one(OURS_EXE, a.ours_dir, a.frame, a.timeout)
    sys.exit(0 if judge(orig, ours, a.frame, a.update_baseline) else 1)


if __name__ == '__main__':
    main()
