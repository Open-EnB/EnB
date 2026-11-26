"""Keep from a tier1c guesses.tsv: class renames already confirmed (suffix kind with a confirmed mapping) and
rename + one edit (suffix+*). Usage: t1c_keep.py IN OUT"""
import csv
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from mangle import class_of as cls  # noqa: E402



# kinds real at >= 5 x their null in every pass so far (gE 72 / 0.2, int 21 / 1.7, ptrcv 16 / 1.1, const 7 / 1.2)
KINDS = {'t1c-gE', 't1c-int', 't1c-ptrcv', 't1c-const'}
conf = set()
for r in csv.DictReader(open(paths.HITS), delimiter='\t'):
    if r['before'] and cls(r['before']) != cls(r['name']):
        conf.add((cls(r['before']), cls(r['name'])))
rows = list(csv.DictReader(open(sys.argv[1]), delimiter='\t'))
keep = [r for r in rows if (r['tier'] == 't1c-suffix' and (cls(r['note'][5:]), cls(r['name'])) in conf)
        or r['tier'].startswith('t1c-suffix+') or r['tier'] in KINDS]
with open(sys.argv[2], 'w', newline='') as f:
    w = csv.DictWriter(f, ['slot', 'name', 'tier', 'note'], delimiter='\t', lineterminator='\n')
    w.writeheader()
    w.writerows(keep)
print(f'kept {len(keep)} of {len(rows)}')
