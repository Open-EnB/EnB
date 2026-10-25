"""zhall.py LIB DONOR [FILE...]: for every function of the tree's LIB files whose DONOR definition
differs, splice the donor's definition in, compile the file alone, and print client addresses gained
and lost (vs the file as it is). Tree files are restored after every try.

DONOR is an EA copy tag (Z Zero Hour, G Generals, R Renegade). Compiles on a build VM, so lease one
first (tools/vc6/pool.py lease) and export ENB_VC6_HOSTS. Writes into the tree while it runs: do
not run it beside another builder on the same library. Output tags: GAIN (gains, loses nothing),
MIX (gains and loses), - (no gain), ERR (compile errors). FROM=file.cpp resumes at that file.

    python tools/vc6/zhall.py ww3d2 Z [FILE.cpp ...]
"""
import os
import sys
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import importer  # noqa: E402
import time


def put(p, b):
    for i in range(60):
        try:
            open(p, 'wb').write(b)
            return
        except PermissionError:
            time.sleep(1)
    raise SystemExit('could not restore ' + p)
from easource import function_spans  # noqa: E402

LIB, DONOR = sys.argv[1], sys.argv[2]
only = [f.lower() for f in sys.argv[3:]]
ZD = os.path.join(importer.COPY_ROOT[DONOR], importer.LIB_DIR[LIB][DONOR])
d = importer.dest(LIB)
zfiles = {f.lower(): os.path.join(ZD, f) for f in os.listdir(ZD) if f.lower().endswith('.cpp')}
for f in sorted(os.listdir(d)):
    if not f.lower().endswith('.cpp') or f.lower() not in zfiles:
        continue
    if only and f.lower() not in only:
        continue
    if os.environ.get('FROM') and f.lower() < os.environ['FROM']:
        continue
    tp = os.path.join(d, f)
    raw = open(tp, 'rb').read()
    text = raw.decode('latin-1')
    nl = '\r\n' if '\r\n' in text else '\n'
    t = text.replace('\r\n', '\n')
    z = open(zfiles[f.lower()], encoding='latin-1', newline='').read().replace('\r\n', '\n')
    try:
        ts, zs = function_spans(t), function_spans(z)
    except Exception as e:
        print('SKIP', f, e, flush=True)
        continue
    names = []
    for n in dict.fromkeys(s[0] for s in ts if s[0]):
        mine = [t[s[1]:s[3] + 1] for s in ts if s[0] == n]
        theirs = [z[s[1]:s[3] + 1] for s in zs if s[0] == n]
        if theirs and len(mine) == len(theirs) and mine != theirs:
            names.append(n)
    if not names:
        continue
    found, _ = importer.compile_and_match([tp], 'zhall')
    base = set().union(*found.values())
    for n in names:
        new, done = importer.splice(t, z, [n])
        if not done:
            continue
        try:
            put(tp, new.replace('\n', nl).encode('latin-1'))
            found, log = importer.compile_and_match([tp], 'zhall')
        finally:
            put(tp, raw)
        errs = [l for l in log.splitlines() if ' error ' in l]
        got = set().union(*found.values())
        plus, minus = sorted(got - base), sorted(base - got)
        tag = 'GAIN' if plus and not minus else ('MIX' if plus else '-')
        if errs:
            tag = 'ERR'
        print(f'{tag:4s} {f} {n} errors={len(errs)} +{plus} -{minus[:6]}{"..." if len(minus) > 6 else ""}', flush=True)
