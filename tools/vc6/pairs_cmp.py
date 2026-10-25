"""Compare the sets of (library, file, address) pairs of two saved docs/data/imports.json copies.

The fix loop (docs/workflow.md) keeps a change only if it gains pairs and loses none, and totals
hide swaps, so the check is on the sets themselves. A pair is one client function address that a
library file reproduces byte for byte (the file's `in_tree` list in imports.json).

    cp docs/data/imports.json .work/imports.before.json
    ... change the tree, run  python tools/vc6/importer.py verify --lib LIB --pool K ...
    python tools/vc6/pairs_cmp.py .work/imports.before.json docs/data/imports.json

Prints `gained [...]` (in B, not in A) and `lost [...]` (in A, not in B). Each entry is
(lib, file, address). Exit status is 0 either way; read the `lost` list.
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
    print('lost', sorted(a - b))


if __name__ == '__main__':
    main(sys.argv)
