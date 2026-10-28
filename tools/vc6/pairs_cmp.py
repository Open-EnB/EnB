"""Compare the sets of (library, file, address) pairs of two saved docs/data/imports.json copies.

The fix loop (docs/workflow.md) keeps a change only if it gains pairs and loses none, and totals
hide swaps, so the check is on the sets themselves. A pair is one client function address that a
library file reproduces byte for byte (the file's `in_tree` list in imports.json).

    cp docs/data/imports.json .work/imports.before.json
    ... change the tree, run  python tools/vc6/importer.py verify --lib LIB --pool K ...
    python tools/vc6/pairs_cmp.py .work/imports.before.json docs/data/imports.json

Prints `gained [...]` (in B, not in A) and `lost [...]` (in A, not in B). Each entry is
(lib, file, address). Then, for D66(v) / D117:
`lost outright [...]` : addresses of lost pairs that no file holds in B (sorted hex strings);
                        D66(v) needs this list empty.
`moves [...]`         : lost pairs whose address another file holds in B, as
                        (lib, from_file, [(lib, to_file), ...], addr).
Exit status is 0 either way; read the `lost` and `lost outright` lists.
"""
import json
import sys


def pairs(path):
    """All (lib, file, address) triples in an imports.json."""
    data = json.load(open(path))
    out = set()
    for lib, entry in data.items():
        for f, v in entry.get('files', {}).items():
            for a in v.get('in_tree', []):
                out.add((lib, f, a if isinstance(a, str) else str(a)))
    return out


def main(argv):
    if len(argv) != 3:
        sys.exit(__doc__)
    a, b = pairs(argv[1]), pairs(argv[2])
    print('gained', sorted(b - a))
    lost = a - b
    print('lost', sorted(lost))
    holders = {}
    for lib, f, addr in b:
        holders.setdefault(addr, set()).add((lib, f))
    print('lost outright', sorted({x[2] for x in lost if x[2] not in holders}))
    print('moves', sorted((lib, f, sorted(holders[addr]), addr)
                          for lib, f, addr in lost if addr in holders))


if __name__ == '__main__':
    main(sys.argv)
