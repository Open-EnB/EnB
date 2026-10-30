"""mark.py SRC OUT NAME=ADDR...: copy SRC to OUT, replacing the blank line just above each function
definition whose line starts with NAME (e.g. 'StarLayerClass::Render') by '// @ADDR' (lines kept).
Used to hand a reconstructed file to tools/re/appfile.py with its client addresses marked without
moving any line (workflow.md: the assert line numbers of a file stay).

    python tools/re/mark.py app/x/y.cpp .work/m/y.cpp StarLayerClass::Render=A1B2C3
"""
import sys, os
src, out = sys.argv[1], sys.argv[2]
pairs = [a.split('=') for a in sys.argv[3:]]
lines = open(src, encoding='utf-8', newline='').read().replace('\r\n', '\n').split('\n')
for name, addr in pairs:
    hits = [i for i, l in enumerate(lines) if name in l and not l.startswith((' ', '\t', '/', '*'))
            and '(' in l and not l.rstrip().endswith(';')]
    if not hits:
        raise SystemExit('not found: ' + name)
    i = hits[0]
    j = i - 1
    while lines[j].strip() != '':
        j -= 1
    lines[j] = '// @' + addr
os.makedirs(os.path.dirname(out) or '.', exist_ok=True)
open(out, 'w', encoding='utf-8', newline='').write('\n'.join(lines))
