"""The client base, SClient and cpacket.cpp's handlers (D94(c), P-T7-1, P-T8-1): tests/capharness/clienttest.

    ENB_VC6_HOSTS=<user>@<ip> python tools/harness/clienttest.py build [--out DIR]
    python tools/harness/clienttest.py run [--exe EXE] [--name alex3] [--session ffff9b25] [--count 1000] [--update]

build: stages tests/capharness/clienttest with its generated sources (gen_stubs.py: pk_stubs.cpp, udpreplay's
packet constructor stand-ins without the Process_Packet lines cpacket.cpp defines; clienttest_tables.cpp, the
factory's opcodes) and one wrapper per app source (APP: client.cpp, sclient.cpp, cpacket.cpp, the connection
layer, pk.cpp and every rebuilt packet file, each its own object), and links it against DIR's libraries into
DIR/programs/clienttest/.

run: writes .work/harness/clienttest/NAME.messages.txt, the first COUNT server-to-client messages of SESSION in
the reference traces' NAME.trace.jsonl (udpreplay.py's reference: roots.EA_TRACES), runs the program there (never in
the game's folder; no registry, no network, asserts to the trace; the base-asset INIs from
tests/capharness/clienttest/data copied in) and compares its trace with
tests/capharness/clienttest/expected.txt: the first run (or --update) writes that file, later runs must equal
it line for line, and every check in the trace must pass (exit code 0).
"""
import argparse
import collections
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(REPO, 'tests', 'capharness', 'clienttest')
WORK = os.path.join(REPO, '.work', 'harness', 'clienttest')
EXPECTED = os.path.join(SRC, 'expected.txt')
DATA = os.path.join(SRC, 'data')
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import roots  # noqa: E402
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
import rootguard  # noqa: E402
TRACES = roots.EA_TRACES      # the frozen reference traces (D154)

APP = [r'app\client\client.cpp', r'app\client\sclient.cpp', r'app\client\cpacket.cpp',
       r'app\common\connect.cpp', r'app\common\dbgconn.cpp', r'app\common\circbuff.cpp',
       r'app\shared\auxdata\linearbu.cpp', r'app\packets\pk.cpp', r'app\packets\clientstub.cpp',
       r'app\common\unknown_navlist.cpp',   # WARP 155's navigation list (as udpreplay.py's CONN)
       r'app\common\lego_identifier.cpp',   # the object-to-object effects' bone path
       r'app\shared\avatar_desc.cpp']       # AVATAR_DESCRIPTION 97's member
for _sub in ('', 'gameedit'):
    _d = os.path.join(REPO, 'app', 'packets', _sub)
    if os.path.isdir(_d):
        APP += sorted(os.path.join('app', 'packets', _sub, f) for f in os.listdir(_d)
                      if f.startswith('pk') and f.endswith('.cpp') and f != 'pk.cpp')
# udpreplay's stand-ins the packet files need (the same files, copied into the stage)
UDPREPLAY_STUBS = ['avatar_desc_stubs.cpp', 'pkitemba_stubs.cpp',
                   'shipcust_stubs.cpp']   # shipcust_desc.obj's externals (pkrecustomize.cpp, pkcreatecharacter.cpp)


def build(out):
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import build as B
    stage = os.path.join(REPO, '.work', 'harness', 'clienttest_src')
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(stage)
    for fn in os.listdir(SRC):
        if fn.endswith(('.cpp', '.h')):
            shutil.copy(os.path.join(SRC, fn), stage)
    for fn in UDPREPLAY_STUBS:
        shutil.copy(os.path.join(REPO, 'tests', 'capharness', 'udpreplay', fn), stage)
    r = subprocess.run([sys.executable, os.path.join(SRC, 'gen_stubs.py'), stage], capture_output=True, text=True)
    print(r.stdout.strip())
    if r.returncode:
        raise SystemExit(r.stderr)
    # one object per app file (a single translation unit breaks on two files' statics of the same name)
    for p in APP:
        name = 'app_' + re.sub(r'\W', '_', os.path.splitext(p)[0][4:]) + '.cpp'
        with open(os.path.join(stage, name), 'w', newline='\r\n') as f:
            f.write('// %s, compiled into clienttest (clienttest.py build)\n' % p)
            f.write('#include "' + os.path.relpath(os.path.join(REPO, p), stage) + '"\n')
    B.PROGRAMS['clienttest'] = {'dir': os.path.relpath(stage, REPO), 'flags': B.I.LIB_FLAGS['app'],
                                'subsystem': 'console', 'runtime': []}
    B.link_program('clienttest', out)
    exe = os.path.join(out, 'programs', 'clienttest', 'clienttest.exe')
    log = os.path.join(out, 'programs', 'clienttest', 'link.log')
    if not os.path.exists(exe) and os.path.exists(log) and 'unresolved external symbol' in open(log, encoding='latin1').read():
        # the second pass: a logging stand-in for every symbol the first left unresolved (Init's, Shutdown's
        # and Load_INIs' callees in files not rebuilt), written by gen_stubs.py --link
        r = subprocess.run([sys.executable, os.path.join(SRC, 'gen_stubs.py'), '--link', log, stage],
                           capture_output=True, text=True)
        print(r.stdout.strip())
        if r.returncode:
            raise SystemExit(r.stderr)
        B.link_program('clienttest', out)
        shutil.copy(os.path.join(stage, 'link_stubs.cpp'), os.path.join(out, 'programs', 'clienttest'))
    if not os.path.exists(exe):
        raise SystemExit('no exe: see ' + os.path.join(out, 'programs', 'clienttest'))
    print('built', exe)


def messages(name, session, count, path):
    """The first COUNT server-to-client messages of SESSION (opcode, payload hex per line)."""
    trace = os.path.join(TRACES, name + '.trace.jsonl')
    n = 0
    with open(path, 'w', newline='\n') as f:
        for line in open(trace, encoding='utf-8'):
            e = json.loads(line)
            if e['channel'].split(':', 1)[1] != session or e['dir'] != 'ServerToClient':
                continue
            f.write('%d %s\n' % (int(e['op'], 16), e.get('data') or ''))
            n += 1
            if n == count:
                break
    return n


def run(exe, name, session, count, update):
    os.makedirs(WORK, exist_ok=True)
    msgs = os.path.join(WORK, '%s.messages.txt' % name)
    n = messages(name, session, count, msgs)
    # clienttest's own basset.ini / cbasset.ini (co.cpp's model loader, Set_Model's radius, the mount points read
    # them from the working directory): tests/capharness/clienttest/data, never the game's (D69)
    for fn in os.listdir(DATA):
        shutil.copy(os.path.join(DATA, fn), WORK)
    # Miles: loadingview.cpp (linked for SClient's calls) imports AIL_set_preference / AIL_serve, so the exe needs
    # Mss32.dll: the copy in build/art (artdir.py's), into the working directory (the DLL search finds it there)
    dll = os.path.join(REPO, 'build', 'art', 'MSS32.DLL')
    if os.path.exists(dll) and not os.path.exists(os.path.join(WORK, 'MSS32.DLL')):
        shutil.copy(dll, WORK)
    res = os.path.join(WORK, 'clienttest.out.txt')
    if os.path.exists(res):
        os.remove(res)		# a run that never starts (a missing DLL) must not compare the last run's trace
    p = subprocess.run([os.path.abspath(exe), res, msgs], capture_output=True, text=True, cwd=WORK, timeout=300)
    print(p.stdout.strip()[-1000:])
    if not os.path.exists(res):
        raise SystemExit('clienttest wrote no trace (exit %d)' % p.returncode)
    got = [l.rstrip('\n') for l in open(res)]
    fails = [l for l in got if l.startswith('check ') and l.endswith(': FAIL')]
    driven = collections.Counter(re.match(r'msg \d+ (\S+)', l).group(1) for l in got
                                 if re.match(r'msg \d+ \S+ \(\d+\) size \d+: cpacket', l))
    stubs = sum(1 for l in got if l.startswith('stub '))
    print('clienttest: %s session %s, %d messages; %d trace lines, %d stub calls; checks %d, failed %d; exit %d'
          % (name, session, n, len(got), stubs, sum(1 for l in got if l.startswith('check ')), len(fails), p.returncode))
    print('  handlers driven: ' + ', '.join('%s x%d' % kv for kv in sorted(driven.items())))
    for l in fails[:10]:
        print('  ' + l)
    ok = p.returncode == 0 and not fails
    if (update or not os.path.exists(EXPECTED)) and not ok:
        print('  no expectation written: the run failed')
        return False
    if update or not os.path.exists(EXPECTED):
        with open(EXPECTED, 'w', newline='\n') as f:
            f.write('\n'.join(got) + '\n')
        print('  expectation written: %s (%d lines)' % (EXPECTED, len(got)))
        return ok
    want = [l.rstrip('\r\n') for l in open(EXPECTED)]
    if got == want:
        print('  equal to expected.txt (%d lines)' % len(want))
        return ok
    for i in range(max(len(got), len(want))):
        a = got[i] if i < len(got) else '<end>'
        b = want[i] if i < len(want) else '<end>'
        if a != b:
            print('  differs from expected.txt at line %d:\n    got  %s\n    want %s' % (i + 1, a, b))
            break
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['build', 'run'])
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    ap.add_argument('--exe')
    ap.add_argument('--name', default='alex3')
    ap.add_argument('--session', default='ffff9b25')
    ap.add_argument('--count', type=int, default=1000)
    ap.add_argument('--update', action='store_true', help='write expected.txt from this run')
    ap.add_argument('--root', help=rootguard.HELP)
    a = ap.parse_args()
    rootguard.check(__file__, a.root)   # D112(4): build writes build/, run .work/harness/ (--update the tree)
    if a.cmd == 'build':
        build(a.out)
        return 0
    exe = a.exe or os.path.join(a.out, 'programs', 'clienttest', 'clienttest.exe')
    return 0 if run(exe, a.name, a.session, a.count, a.update) else 1


if __name__ == '__main__':
    sys.exit(main())
