"""Propagate every confirmed class rename to every off name: qualifiers and parameter / return types alike
(mangle.rename_all: each spelled-out `Old@`). Off names do not set windows, so this never moves an anchor; it
makes later guesses start from the confirmed spelling (docs/binary.md §5.6).

Confirmed renames come from the accepted hits (docs/data/thunk-name-hits.tsv: `before` -> `name` with another
class qualifier). A rename is NOT propagated when our class may be a split of two retail classes:
  - our class maps to two or more retail classes in the hits, or
  - some anchor (a retail-exact name) still spells our class name.

Usage: python tools/re/thunknames/propagate.py --state F [--write OUT]   (without --write: guesses.tsv for gapguess)
"""
import argparse
import collections
import csv
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402
from gapguess import State  # noqa: E402
from mangle import class_of, contains, rename_all  # noqa: E402


def confirmed_renames(st, hits=paths.HITS, log=print):
    pairs = collections.defaultdict(collections.Counter)
    for r in csv.DictReader(open(hits), delimiter='\t'):
        a, b = class_of(r.get('before') or ''), class_of(r['name'])
        if a and b and a != b:
            pairs[a][b] += 1
    ren, split = {}, []
    anchors = [st.names[i] for i in st.onset]
    for a, bs in pairs.items():
        if len(bs) > 1:
            split.append((a, 'maps to ' + ', '.join(f'{b} ({n})' for b, n in bs.most_common())))
            continue
        b = next(iter(bs))
        keep = [n for n in anchors if contains(n, a)]
        if keep:
            split.append((a, f'-> {b}, but {len(keep)} anchors still spell {a} (e.g. {keep[0]})'))
            continue
        ren[a] = b
    for a, why in sorted(split):
        log(f'  not propagated (possible split): {a} {why}')
    return ren, split


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--hits', default=paths.HITS)
    ap.add_argument('--write', help='write the propagated state here (else guesses for gapguess.py)')
    a = ap.parse_args()
    st = State(a.state)
    ren, split = confirmed_renames(st, a.hits)
    changed = 0
    rows = []
    for i, nm in enumerate(st.names):
        if not nm or i in st.onset or not st.window(i):
            continue
        new = rename_all(nm, ren)
        if new != nm:
            changed += 1
            rows.append((i, nm, new))
    print(f'renames {len(ren)} (not propagated {len(split)}); names changed {changed}')
    if a.write:
        for i, nm, new in rows:
            if st.orig[i] is None:
                st.orig[i] = nm
            st.names[i] = new
            st.how[i] = (st.how[i] + '; ' if st.how[i] else '') + 'prop'
        st.recompute()
        os.makedirs(os.path.dirname(os.path.abspath(a.write)), exist_ok=True)
        st.write(a.write)
        print(f'wrote {a.write}: {st.summary()}')
    else:
        out = paths.work('propagate', 'guesses.tsv')
        with open(out, 'w', newline='') as f:
            w = csv.writer(f, delimiter='\t', lineterminator='\n')
            w.writerow(['slot', 'name', 'tier', 'note'])
            for i, nm, new in rows:
                w.writerow([f'0x{st.table[i][0]:X}', new, 'prop', f'ours {nm}'])
        print(f'wrote {out}')


if __name__ == '__main__':
    main()
