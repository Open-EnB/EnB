"""Compares two d3d8shot draw dumps draw by draw (D190): the same frame of the POC and of the retail client.

    python tools/harness/dumpcmp.py A B [--frame N]

A and B are shot folders (clientpoc.py replay ... with D3D8SHOT_DUMP=N in the environment) holding draws_NNNNN.bin and
d3d8shot.log (the material and the enabled lights of each draw, Log_Lights). Prints the draw counts, then for each draw
that differs the fields that differ: prims, stride, textures (size, format), render states (A vs B), stage states,
material, lights (light numbers ignored, sorted).
"""
import argparse
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import drawdump as D  # noqa: E402


def load(folder, frame):
    pat = f'draws_{frame:05d}.bin' if frame else 'draws_*.bin'
    files = sorted(glob.glob(os.path.join(folder, pat)))
    if not files:
        raise SystemExit(f'{folder}: no {pat}')
    mats = {}
    for l in open(os.path.join(folder, 'd3d8shot.log'), encoding='latin1'):
        m = re.search(r'draw (\d+): (material.*)$', l)
        if m:
            mats[int(m.group(1))] = re.sub(r'L(\d) ', 'L ', m.group(2).strip())
    out = []
    for i, r in enumerate(D.read(files[0])):
        tex = tuple((x[1], x[2], x[3]) if x[0] else None for x in r['tex'])
        parts = mats.get(i, '').split(';')
        lights = tuple(sorted(p.strip() for p in parts[1:] if p.strip()))
        out.append((r['prims'], r['stride'], tex, tuple(sorted(r['states'].items())),
                    tuple(tuple(sorted(s.items())) for s in r['stages']), parts[0], lights))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('a')
    ap.add_argument('b')
    ap.add_argument('--frame', type=int, default=0, help='the dump file draws_NNNNN.bin (default: the first)')
    a = ap.parse_args()
    x, y = load(a.a, a.frame), load(a.b, a.frame)
    print(f'draws: {len(x)} vs {len(y)}')
    diff = 0
    for i in range(max(len(x), len(y))):
        p = x[i] if i < len(x) else None
        q = y[i] if i < len(y) else None
        if p == q:
            continue
        diff += 1
        if p is None or q is None:
            print(i, 'missing in', 'A' if p is None else 'B')
            continue
        for k, name in enumerate(['prims', 'stride', 'tex', 'states', 'stages', 'material', 'lights']):
            if p[k] == q[k]:
                continue
            if name == 'states':
                dp, dq = dict(p[k]), dict(q[k])
                print(i, name, {s: (dp.get(s), dq.get(s)) for s in dp if dp.get(s) != dq.get(s)})
            elif name == 'stages':
                print(i, name, [(dict(p[k][n]), dict(q[k][n])) for n in range(2) if p[k][n] != q[k][n]])
            else:
                print(i, name, p[k], '|', q[k])
    print(f'{diff} draws differ')


if __name__ == '__main__':
    main()
