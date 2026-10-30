"""Tries every header/source pair of a library from another EA copy, keeping what helps.

    python trial_all.py LIB COPY [COPY...] [--log FILE]

For each base name that has a .h and a .cpp in the copy and whose code differs from the
repository's (comments and whitespace ignored), runs trial.py COPY LIB/name.h LIB/name.cpp
--keep-if-better (nothing lost allowed). Files reconstructed by us are skipped. Each kept swap
is committed by the caller afterwards; this script only changes the tree and the import state.
"""
import argparse
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(os.path.dirname(HERE), 're'))
import importer  # noqa: E402
from libdiff import code_lines  # noqa: E402


def candidates(lib, copy):
    src_dir = os.path.join(importer.COPY_ROOT[copy], importer.LIB_DIR[lib][copy])
    ours = {f.lower(): f for f in os.listdir(importer.dest(lib))}
    theirs = {f.lower(): f for f in os.listdir(src_dir)}
    out = []
    for low in sorted(theirs):
        if not low.endswith('.cpp'):
            continue
        base = low[:-4]
        h = base + '.h'
        if h not in theirs or low not in ours or h not in ours:
            continue
        if (lib, low) in importer.RECONSTRUCTED or (lib, h) in importer.RECONSTRUCTED:
            continue
        same = all(code_lines(os.path.join(src_dir, theirs[n])) == code_lines(os.path.join(importer.dest(lib), ours[n]))
                   for n in (low, h))
        if not same:
            out.append((theirs[h], theirs[low]))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('lib')
    ap.add_argument('copies', nargs='+')
    ap.add_argument('--log')
    a = ap.parse_args()
    log = open(a.log, 'a', newline='\n') if a.log else sys.stdout
    for copy in a.copies:
        for h, cpp in candidates(a.lib, copy):
            p = subprocess.run([sys.executable, os.path.join(HERE, 'trial.py'), copy, f'{a.lib}/{h}', f'{a.lib}/{cpp}',
                                '--keep-if-better'], capture_output=True, text=True)
            lines = [l for l in p.stdout.splitlines() if '->' in l or l.startswith(('KEPT', 'restored'))]
            log.write(f'{copy} {h} {cpp}: ' + ' | '.join(l.strip() for l in lines) + '\n')
            log.flush()


if __name__ == '__main__':
    main()
