"""Turn uncommitted tree edits into an addpatch.py spec: one (lib, file, old, new, why) per diff hunk.

    python tools/vc6/mkspec.py OUT_SPEC.py REASONS.py

REASONS.py defines R = {'lib/file': 'why', ...}. Hunks are applied in file order; each hunk's
`old` is widened with unchanged context lines (never into a neighbouring hunk) until it occurs
exactly once in the file as patched so far; a hunk that can't be made unique is merged with the
next one.
"""
import difflib
import pprint
import runpy
import subprocess
import sys

out, reasons = sys.argv[1], runpy.run_path(sys.argv[2])['R']
P = []
NL = '\n'


def text(lines, last):
    return ''.join(lines)  # lines keep their own newline


for path, why in reasons.items():
    lib, name = path.split('/')
    head = subprocess.run(['git', 'show', 'HEAD:' + path], capture_output=True, check=True).stdout.decode('latin1')
    head = head.replace('\r\n', '\n')
    work = open(path, encoding='latin1', newline='').read().replace('\r\n', '\n')
    a, b = head.splitlines(True), work.splitlines(True)
    ops = [list(o[1:]) for o in difflib.SequenceMatcher(None, a, b, autojunk=False).get_opcodes() if o[0] != 'equal']
    cur = head
    k = 0
    prev_end = 0
    while k < len(ops):
        i1, i2, j1, j2 = ops[k]
        next_start = ops[k + 1][0] if k + 1 < len(ops) else len(a)
        found = None
        for ctx in range(0, 40):
            lo, hi = max(prev_end, i1 - ctx), min(next_start, i2 + ctx)
            last = hi == len(a)
            old = text(a[lo:hi], last)
            new = text(a[lo:i1] + b[j1:j2] + a[i2:hi], last)
            if old and (NL + cur).count(NL + old) == 1 and cur.count(old) == 1:
                found = (old, new, hi)
                break
            if lo == prev_end and hi == next_start:
                break
        if found is None:
            assert k + 1 < len(ops), (path, i1)
            n = ops.pop(k + 1)
            ops[k] = [i1, n[1], j1, n[3]]
            continue
        old, new, hi = found
        cur = (NL + cur).replace(NL + old, NL + new)[1:]  # matches start at a line start
        P.append((lib, name, old, new, why))
        prev_end = hi
        k += 1
    assert cur == work, path
with open(out, 'w', encoding='utf-8', newline='\n') as f:
    f.write('P = ' + pprint.pformat(P, width=120) + '\n')
print(len(P), 'patches')
