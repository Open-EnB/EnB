"""Build and run tests/capharness/auxdata_smoke (M1.2: the aux-data round trip on a hand-built root).

    python tools/vc6/build.py [--out DIR]          (the libraries; then mkmss32.py lib --out DIR)
    ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/auxdata_smoke.py [--out DIR]

Uses build.py's link step (the program is added to its PROGRAMS in memory), links against
DIR's libraries and stubs/, runs the exe on the host and exits with its code: 0 when both forms
(tree, record) decode into a second tree with the expected values and re-encode byte for byte.
"""
import argparse
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
import build as B  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    a = ap.parse_args()
    B.PROGRAMS['auxdata_smoke'] = {'dir': os.path.join('tests', 'capharness', 'auxdata_smoke'),
                                   'flags': B.I.LIB_FLAGS['ww3d2'] + ' /GX', 'subsystem': 'console', 'runtime': []}
    B.link_program('auxdata_smoke', a.out)
    exe = os.path.join(a.out, 'programs', 'auxdata_smoke', 'auxdata_smoke.exe')
    if not os.path.exists(exe):
        raise SystemExit('no exe')
    r = subprocess.run([os.path.abspath(exe)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=60)
    print(r.stdout)
    raise SystemExit(r.returncode)


if __name__ == '__main__':
    main()
