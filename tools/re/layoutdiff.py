"""Finds class-layout differences: gaps whose instructions differ only in a displacement.

For each located gap (gaps.py output) the compiled function and the client's are aligned
instruction by instruction; where two aligned instructions are the same except for one memory
displacement ([reg+X] here, [reg+Y] in the client), the pair (X -> Y) is recorded under the
function's class. A class whose functions all shift the same members the same way has a
different layout in E&B's headers (a member added, removed, resized or moved), and one header
fix closes all of them.

    python layoutdiff.py --gaps ../../docs/data/gaps-ww3d2.json --objs DIR [--min-sim 0.6]
"""
import argparse
import collections
import difflib
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage  # noqa: E402
from objdiff import client_lines, norm, obj_lines  # noqa: E402
from objmatch import CoffObject  # noqa: E402

DISP = re.compile(r'([+-]) 0x([0-9a-f]+)\]')


def unchanged_displacements(left, right):
    """Displacements ([reg+X], not esp/ebp) in aligned instructions that are identical on both sides."""
    out = []
    sm = difflib.SequenceMatcher(None, [norm(t) for _, _, t in left], [norm(t) for _, _, t in right],
                                 autojunk=False)
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op != 'equal':
            continue
        for k in range(i2 - i1):
            a = left[i1 + k][2]
            if 'esp' in a or 'ebp' in a:
                continue
            for sign, x in DISP.findall(a):
                out.append(int(x, 16) * (-1 if sign == '-' else 1))
    return out


def displacement_pairs(left, right):
    """[(x, y, text)] for aligned instruction pairs that differ only in one displacement."""
    out = []
    sm = difflib.SequenceMatcher(None, [norm(t) for _, _, t in left], [norm(t) for _, _, t in right],
                                 autojunk=False)
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op != 'replace' or i2 - i1 != j2 - j1:
            continue
        for k in range(i2 - i1):
            a, b = left[i1 + k][2], right[j1 + k][2]
            ma, mb = DISP.findall(a), DISP.findall(b)
            if len(ma) == 1 and len(mb) == 1 and DISP.sub(']', a) == DISP.sub(']', b):
                x = int(ma[0][1], 16) * (-1 if ma[0][0] == '-' else 1)
                y = int(mb[0][1], 16) * (-1 if mb[0][0] == '-' else 1)
                if x != y:
                    out.append((x, y, a))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--gaps', required=True)
    ap.add_argument('--objs', required=True)
    ap.add_argument('--min-sim', type=float, default=0.6)
    ap.add_argument('--top', type=int, default=30)
    ap.add_argument('--cls', help='one class: list every displacement, moved and unchanged')
    a = ap.parse_args()
    img = ClientImage()
    objs = {}
    per_class = collections.defaultdict(lambda: {'functions': set(), 'pairs': collections.Counter(), 'frame': 0})
    for g in json.load(open(a.gaps)):
        if not g.get('address') or (g.get('similarity') or 0) < a.min_sim:
            continue
        path = os.path.join(a.objs, g['object'])
        if path not in objs:
            objs[path] = CoffObject(path)
        obj = objs[path]
        f = [x for x in obj.functions() if x[0] == g['symbol']]
        if not f:
            continue
        name, sec, s, e = f[0]
        left = obj_lines(obj, sec, s, e)
        right = client_lines(img, int(g['address'], 16), e - s)
        cls = g['function'].split('::')[0] if '::' in g['function'] else '(free functions)'
        if a.cls and cls != a.cls:
            continue
        if a.cls:
            for x in unchanged_displacements(left, right):
                per_class[cls].setdefault('same', collections.Counter())[x] += 1
        for x, y, text in displacement_pairs(left, right):
            entry = per_class[cls]
            entry['functions'].add(g['function'])
            # [esp+..] / [ebp-..] are locals and arguments: a different frame, not a layout
            if 'esp' in text or 'ebp' in text:
                entry['frame'] += 1
            else:
                entry['pairs'][(x, y)] += 1
    if a.cls:
        e = per_class.get(a.cls)
        if not e:
            raise SystemExit('no gaps for that class')
        moved = collections.defaultdict(collections.Counter)
        for (x, y), n in e['pairs'].items():
            moved[x][y] += n
        for x in sorted(set(moved) | set(e.get('same', {}))):
            same = e.get('same', {}).get(x, 0)
            mv = ', '.join(f'-> +0x{y:X} ({y - x:+d}) x{n}' for y, n in moved[x].most_common(3)) if x in moved else ''
            print(f'+0x{x:04X}  unchanged x{same:<4d} {mv}')
        return
    rows = sorted(per_class.items(), key=lambda kv: -sum(kv[1]['pairs'].values()))
    for cls, e in rows[:a.top]:
        if not e['pairs']:
            continue
        pairs = ', '.join(f'+0x{x:X}->+0x{y:X} ({n})' for (x, y), n in e['pairs'].most_common(6))
        print(f"{cls:36s} {len(e['functions']):3d} functions  {pairs}")


if __name__ == '__main__':
    main()
