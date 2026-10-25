"""Client strings that occur in a Zero Hour/Generals/Renegade source file of one library but nowhere
in our tree's copy of it: finds code the client has and our tree lost.

    python tools/re/strscan.py [LIB]        (default ww3d2; Z, G and R copies per easource.LIBRARIES)

Prints: client address, copy (zh/gen/ren), file, the string.
"""
import os
import sys
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
from clientimage import ClientImage  # noqa: E402
import easource  # noqa: E402

LIB = sys.argv[1].lower() if len(sys.argv) > 1 else 'ww3d2'
TAGS = {'R': 'ren', 'G': 'gen', 'Z': 'zh'}
ROOTS = [(TAGS[tag], os.path.join(easource.EA_ROOT, path)) for tag, path, lib in easource.LIBRARIES if lib == LIB]
ROOTS.sort(key=lambda r: ('zh', 'gen', 'ren').index(r[0]))      # Zero Hour first
T = os.path.join(REPO, LIB)


def load(d):
    out = {}
    for f in os.listdir(d):
        if f.lower().endswith(('.cpp', '.h')):
            out[f] = open(os.path.join(d, f), encoding='latin-1').read()
    return out


tree = '\n'.join(load(T).values())
ea = [(tag, load(r)) for tag, r in ROOTS]
img = ClientImage()
for va, s in sorted(img.strings(min_len=12).items()):
    key = s.replace('\r', '\\r').replace('\n', '\\n')
    if len(key) < 12 or key in tree:
        continue
    for r, files in ea:
        hit = [f for f, txt in files.items() if key in txt]
        if hit:
            print(hex(va), r, hit[0], repr(key[:80]))
            break
