"""The task kernel's test (docs/kernel-map.md 6, step 1): tests/capharness/kerneltest.

    ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/kerneltest.py build [--out DIR]
    python tools/harness/kerneltest.py run [--exe EXE]

build: links kerneltest with app\\kernel\\kernel.cpp (compiled into the program) against DIR's
libraries (WWLib's profiler) into DIR/programs/kerneltest/.
run: runs it and checks its trace against the behaviour docs/kernel-map.md describes (intervals
and the sleep, focus, Pre_Run / Run / Render for the focused task only, Pending_Delete, Remove_Task,
Handle_Key, Shutdown, the asserts).
"""
import argparse
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WORK = os.path.join(REPO, '.work', 'harness', 'kerneltest')
SOURCES = [r'app\kernel\kernel.cpp']

EXPECT = [
    'instance ok',
    'focus A',
    'assert false (line 157)',
    'add twice: 0',
    'B run 1 at 1030',                    # the first cycle slept until B was due, ran nothing
    'A pre-run', 'A run 1 at 1100 (focused)', 'A render', 'B run 2 at 1100',
    'A yield? yes', 'set focus C: 1', 'focus C',
    'C yield? no', 'set focus A (C keeps it): 0',
    'C yield? yes', 'next focus: A',
    'B run 3 at 1130', 'B shutdown', 'B deleted', 'C run 1 at 1130', 'tasks left B? 0',
    'A yield? yes', 'A shutdown', 'A deleted', 'A left? 0, focus C',
    'C yield? yes',                       # 0x6A: Next_Focus finds only C again: not taken, and with a
                                          # task focused neither key is passed on
    'running 1',
    'C yield? yes', 'C shutdown', 'C deleted', 'BFAEE0 slot 0x10', 'shutdown 1',
    'running 0',
    'assert Instance == NULL (line 74)',
    'asserts 2',
]


def build(out):
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import build as B
    src = os.path.join(REPO, 'tests', 'capharness', 'kerneltest')
    stage = os.path.join(REPO, '.work', 'harness', 'kerneltest_src')
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(stage)
    for fn in os.listdir(src):
        shutil.copy(os.path.join(src, fn), stage)
    shutil.copy(os.path.join(REPO, 'app', 'kernel', 'kernel.h'), stage)
    with open(os.path.join(stage, 'app_sources.cpp'), 'w', newline='\r\n') as f:
        f.write('// app\\kernel, compiled into the program (kerneltest.py build)\n')
        for p in SOURCES:
            f.write('#include "' + os.path.relpath(os.path.join(REPO, p), stage) + '"\n')
    B.PROGRAMS['kerneltest'] = {'dir': os.path.relpath(stage, REPO), 'flags': B.I.LIB_FLAGS['app'],
                                'subsystem': 'console', 'runtime': []}
    B.link_program('kerneltest', out)
    exe = os.path.join(out, 'programs', 'kerneltest', 'kerneltest.exe')
    if not os.path.exists(exe):
        raise SystemExit('no exe: see ' + os.path.join(out, 'programs', 'kerneltest'))
    print('built', exe)


def run(exe):
    os.makedirs(WORK, exist_ok=True)
    res = os.path.join(WORK, 'kerneltest.out.txt')
    p = subprocess.run([os.path.abspath(exe), res], capture_output=True, text=True, cwd=WORK)
    if p.returncode != 0:
        print(p.stdout[-2000:], p.stderr[-2000:])
        raise SystemExit('kerneltest failed: %d' % p.returncode)
    got = [l.rstrip('\n') for l in open(res)]
    # the expected lines must appear in this order (other lines may come between)
    i = 0
    for line in got:
        if i < len(EXPECT) and line == EXPECT[i]:
            i += 1
    print('kerneltest: %d lines, %d of %d expected in order' % (len(got), i, len(EXPECT)))
    if i < len(EXPECT):
        print('  first missing: %r' % EXPECT[i])
        print('  trace: ' + ' | '.join(got))
        return False
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['build', 'run'])
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    ap.add_argument('--exe')
    a = ap.parse_args()
    if a.cmd == 'build':
        build(a.out)
        return 0
    return 0 if run(a.exe or os.path.join(a.out, 'programs', 'kerneltest', 'kerneltest.exe')) else 1


if __name__ == '__main__':
    sys.exit(main())
