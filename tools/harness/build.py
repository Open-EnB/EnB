"""Build tests/capharness/capharness.exe with VC6 on a build VM and copy it back.

    python tools/vc6/pool.py lease --agent NAME       (prints ENB_VC6_HOSTS=<user>@ip)
    ENB_VC6_HOSTS=<user>@ip python tools/harness/build.py [--out DIR]

The sources are sent with vc6.sync (the same mechanism as importer.py verify), then compiled
and linked in one `cl` call in the VM (LIBCMT: /MT, as the client). The exe lands in
--out (default .work/harness/bin). No client or library code is linked yet (M1.0 skeleton).
"""
import argparse
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
import vc6  # noqa: E402

SRC_DIR = os.path.join(REPO, 'tests', 'capharness')
SOURCES = ['capharness.cpp']
FLAGS = '/nologo /MT /W3 /O2'
LIBS = []


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'harness', 'bin'))
    a = ap.parse_args()
    full, rel = vc6.under_root(SRC_DIR)
    vc6.sync([SRC_DIR])
    vm_dir = vc6.ROOT + '\\' + rel
    exe = vc6.WORK + '\\capharness.exe'
    p = vc6.run_bat([
        f'cd /d "{vm_dir}"',
        f'del "{exe}" 2>nul',
        f'cl {FLAGS} /Fo"{vc6.WORK}\\\\" /Fe"{exe}" ' + ' '.join(SOURCES) + ' ' + ' '.join(LIBS),
    ])
    text = p.stdout + p.stderr
    print(text.strip())
    if p.returncode != 0:
        raise SystemExit(f'build failed ({p.returncode})')
    os.makedirs(a.out, exist_ok=True)
    local = os.path.join(a.out, 'capharness.exe')
    vc6.scp(vc6.remote(exe), local)
    print('built', local)


if __name__ == '__main__':
    main()
