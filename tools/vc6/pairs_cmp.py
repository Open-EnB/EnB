"""Compare the sets of (library, file, address) pairs of two saved docs/data/imports.json copies.

The fix loop (docs/workflow.md) keeps a change only if it gains pairs and loses none, and totals
hide swaps, so the check is on the sets themselves. A pair is one client function address that a
library file reproduces byte for byte (the file's `in_tree` list in imports.json).

    cp docs/data/imports.json .work/imports.before.json
    ... change the tree, run  python tools/vc6/importer.py verify --lib LIB --pool K ...
    python tools/vc6/pairs_cmp.py .work/imports.before.json docs/data/imports.json

Prints `gained [...]` (in B, not in A) and `lost [...]` (in A, not in B). Each entry is
(lib, file, address). A pair found by D166's caller tie-break (the file's `tiebreak` list, also in
`in_tree`) is listed apart: `gained tie-break [...]`, `lost tie-break [...]`, and a lost tie-break
pair whose caller also lost its pair is named `lost tie-break with its caller (D66 vii)`; the
other lost tie-break pairs are in `lost` too (so an empty `lost` is the D66 test; `gained` leaves
tie-break pairs out). Then, for D66(v) / D117:
`lost outright [...]` : addresses of lost pairs that no file holds in B (sorted hex strings);
                        D66(v) needs this list empty.
`moves [...]`         : lost pairs whose address another file holds in B, as
                        (lib, from_file, [(lib, to_file), ...], addr).
Accepted corrections (D173): docs/data/accepted-corrections.json lists pairs the user ruled false (hand-maintained,
one entry per decision: {address, lib, file, moved_to, decision, note}). When such a pair is lost AND the same
file gains `moved_to` in the same comparison, it is printed as `corrected (accepted: D173) [...]` and left out of
`lost`, `lost outright` and `moves`; otherwise it is a loss as before.
Exit status is 0 either way; read the `lost` and `lost outright` lists.
"""
import json
import os
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
CORRECTIONS = os.path.join(ROOT, 'docs', 'data', 'accepted-corrections.json')


def load(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def pairs(data, key='in_tree'):
    """All (lib, file, address) triples of an imports.json (`in_tree`, or the `tiebreak` list)."""
    out = set()
    for lib, entry in data.items():
        for f, v in entry.get('files', {}).items():
            for a in v.get(key, []):
                out.add((lib, f, a if isinstance(a, str) else str(a)))
    return out


def load_corrections(path=CORRECTIONS):
    """The accepted corrections (a list of entries), [] when the file is missing."""
    try:
        with open(path, encoding='utf-8') as f:
            return json.load(f)['corrections']
    except FileNotFoundError:
        return []


def split_corrections(lost, gained, corrections):
    """([(entry, pair)], rest): the LOST pairs an accepted correction explains (its lib, file and address lost,
    its moved_to gained by the same lib and file in GAINED), and the other lost pairs."""
    gained_at = {(lib, f, int(a, 16)) for lib, f, a in gained}
    by_pair = {(e['lib'], e['file'], int(e['address'], 16)): e for e in corrections}
    corrected, rest = [], []
    for x in sorted(lost):
        e = by_pair.get((x[0], x[1], int(x[2], 16)))
        if e and (x[0], x[1], int(e['moved_to'], 16)) in gained_at:
            corrected.append((e, x))
        else:
            rest.append(x)
    return corrected, rest


def main(argv):
    if len(argv) != 3:
        sys.exit(__doc__)
    da, db = load(argv[1]), load(argv[2])
    ta, tb = pairs(da, 'tiebreak'), pairs(db, 'tiebreak')
    a, b = pairs(da) - ta, pairs(db) - tb          # plain pairs
    # a pair that was plain in A and is a tie-break in B (or the reverse) is neither gained nor lost
    gained_t, lost_t = tb - pairs(da), ta - pairs(db)
    print('gained', sorted(b - a - (pairs(da))))
    print('gained tie-break', sorted(gained_t))
    lost = a - pairs(db)
    # D66 (vii): the tie-break pair went with its caller. A caller is a pair of the same file in A that
    # B no longer has (any lost pair of the file: the check cannot see which function calls which)
    lost_files = {(lib, f) for lib, f, _ in lost}
    with_caller = {x for x in lost_t if (x[0], x[1]) in lost_files}
    lost |= lost_t - with_caller
    corrected, rest = split_corrections(lost, pairs(db) - pairs(da), load_corrections())
    lost = set(rest)
    for dec in sorted({e['decision'] for e, _ in corrected}):
        print(f'corrected (accepted: {dec})',
              sorted((lib, f, a, e['moved_to']) for e, (lib, f, a) in corrected if e['decision'] == dec))
    print('lost', sorted(lost))
    print('lost tie-break (counted in lost)', sorted((lost_t - with_caller) & lost))
    print('lost tie-break with its caller (D66 vii, not a loss)', sorted(with_caller))
    allb = pairs(db)
    holders = {}
    for lib, f, addr in allb:
        holders.setdefault(addr, set()).add((lib, f))
    print('lost outright', sorted({x[2] for x in lost if x[2] not in holders}))
    print('moves', sorted((lib, f, sorted(holders[addr]), addr)
                          for lib, f, addr in lost if addr in holders))


if __name__ == '__main__':
    main(sys.argv)
