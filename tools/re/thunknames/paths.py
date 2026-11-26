"""Where the name-recovery tools read and write (docs/binary.md §5.6, README.md here).

Tracked inputs: the state (docs/data/thunk-names.tsv), the accepted hits (docs/data/thunk-name-hits.tsv), the
model proposals (tools/re/thunknames/proposals/). Scratch outputs: .work/names/ (untracked). The retail client and the
EA tree come from the environment or the Windows workstation's defaults:
  ENB_CLIENT_EXE  retail client.exe (SHA-256 249f8524...51c4); default clientimage.DEFAULT_CLIENT
  ENB_EA          the EA source tree (CnC_Renegade/, CnC_Generals_Zero_Hour/); default C:\\EnBDevelopment\\EA
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RE_DIR = os.path.dirname(HERE)
REPO = os.path.dirname(os.path.dirname(RE_DIR))
for _p in (RE_DIR, HERE):
    if _p not in sys.path:
        sys.path.insert(0, _p)

from clientimage import DEFAULT_CLIENT  # noqa: E402

DATA = os.path.join(REPO, 'docs', 'data')
STATE = os.path.join(DATA, 'thunk-names.tsv')
HITS = os.path.join(DATA, 'thunk-name-hits.tsv')
RENAMES = os.path.join(DATA, 'class-renames.tsv')
SYMBOLS = os.path.join(DATA, 'symbols.json')
ASSERTS = os.path.join(DATA, 'asserts.txt')
PROPOSALS = os.path.join(HERE, 'proposals')
WORK = os.path.join(REPO, '.work', 'names')
PSEUDO = os.path.join(REPO, '.work', 'pseudo')
CLIENT_EXE = os.environ.get('ENB_CLIENT_EXE') or DEFAULT_CLIENT
EA_ROOT = os.environ.get('ENB_EA') or r'C:\EnBDevelopment\EA'


def work(*parts):
    """A path under .work/names/, its directory created."""
    p = os.path.join(WORK, *parts)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    return p


def workdir(*parts):
    """A directory under .work/names/, created."""
    p = os.path.join(WORK, *parts)
    os.makedirs(p, exist_ok=True)
    return p
