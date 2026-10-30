"""The task kernel's test (docs/kernel-map.md 6, step 1): tests/capharness/kerneltest.

    ENB_VC6_HOSTS=<user>@<ip> python tools/harness/kerneltest.py build [--out DIR]
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
SOURCES = [r'app\kernel\kernel.cpp', r'app\client\tasks\tsk_movie.cpp', r'app\client\tasks\tsk_smovie.cpp',
           r'app\client\tasks\tsk_menu.cpp']
# the game's install (read only): binkw32.dll and a movie are copied to the work directory
GAME = (os.environ.get('ENB_RETAIL_INSTALL') or r'C:\Program Files (x86)\EA GAMES\Earth & Beyond')
BINK_DLL = os.path.join(GAME, 'release', 'binkw32.dll')
BINK_FILE = os.path.join(GAME, 'Data', 'client', 'mixfiles', 'eb_ws_logo.bik.old')

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
    # the start-up tasks (taskstest.cpp, kernel-map.md 6, step 7): the intro movie, the scripted
    # movie, MenuTask, in WinMain's order
    'bink init',
    'fail_if !movie_name (line 87)', 'movies 2',
    'bink play ..//data//client//mixfiles//eb_sizzle.bik (same) volume 0x8000',
    'set focus movie: 0',                  # already focused: Set_Focus asks it to yield first: no
    'client 1 made (task ok, kernel ok)', 'client 2 made (task ok, kernel ok)',
    'client 3 made (task ok, kernel ok)',   # moviescript2.txt and 3 exist, 4 doesn't
    'load assets cbasset.ini', 'smovie init: 1, clients 3',
    'focus movie', 'movie interval 500',     # no Render yet: the device check not passed
    'yield movie early: 0',
    'bink stop at the end', 'bink play second.bik (same) volume 0x4000',
    'movie 1 done: index 1, some frames rendered, renders bracketed',
    'key before 3 s: movie kept',
    'bink stop before the end',
    'client 1 loads moviescript1.txt (some background runs before)',   # Remove_Task's Next_Focus
    'bink shutdown',                       # the movie task deleted after its Pre_Run (Pending_Delete)
    'client 1 run 1 (focused)', 'client 1 render',
    'key after 3 s: movie removed, focus smovie',
    'client 1 key 0x41', 'client 1 run 2 (focused)', 'client 1 deleted',
    'client 2 loads moviescript2.txt (no background runs before)',
    'client 2 run 2 (focused)', 'client 2 deleted',
    'client 3 loads moviescript3.txt (no background runs before)',
    'client 3 run 2 (focused)', 'client 3 deleted',
    'log: MenuTask: accepting focus.', 'free assets',
    'login task mode 1', 'log: MenuTask: Starting Login Task.', 'step: starting login task',
    'log: MenuTask: yielding focus.', 'login accepts focus', 'step end: ok',
    'smovie removed after 5 cycles',
    'menu removed, login added, focus login',
    'log: MenuTask: killing kernel.', 'running after 0x51: 0',
    'tasks done', 'asserts 2',
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
    args = [os.path.abspath(exe), res]
    if os.path.exists(BINK_DLL) and os.path.exists(BINK_FILE):
        shutil.copy(BINK_DLL, WORK)
        shutil.copy(BINK_FILE, os.path.join(WORK, 'eb_ws_logo.bik'))
        args.append('eb_ws_logo.bik')
    p = subprocess.run(args, capture_output=True, text=True, cwd=WORK, timeout=300)
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
