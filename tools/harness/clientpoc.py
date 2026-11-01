"""Runs the proof-of-concept client (D179; built by `tools/vc6/build.py link --program clientpoc`).

    python tools/harness/clientpoc.py run [--seconds N] [--fullscreen] [-- CLIENT ARGS...]
    python tools/harness/clientpoc.py log         the last run's stubs.log, digested

run: starts C:\\EnBDevelopment\\ClientPOC\\release\\client.exe with that folder as working directory (never
the game's install), ENB_POC_WINDOWED=1 unless --fullscreen, waits N seconds (default 40) and kills it by PID
if it still runs; then prints the digest. Delete stubs.log and _except.txt first (the client's exception
report, written beside the exe).

The digest: stubs.log's lines in order, repeats folded ("x12"), each CRASH: line (poc_window.cpp's
vectored handler) with its addresses named from client.map (tools/harness/symbolize.py), and the first
lines of _except.txt. Nothing is run against a server (antprox stays off: it writes the registry).
"""
import argparse
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import symbolize  # noqa: E402

RELEASE = r'C:\EnBDevelopment\ClientPOC\release'
EXE = os.path.join(RELEASE, 'client.exe')
MAP = os.path.join(RELEASE, 'client.map')
LOG = os.path.join(RELEASE, 'stubs.log')
EXCEPT = os.path.join(RELEASE, '_except.txt')


def digest():
    if not os.path.exists(LOG):
        print('no stubs.log')
        return
    syms = symbolize.load(MAP) if os.path.exists(MAP) else []
    lines = [l.rstrip('\n') for l in open(LOG, encoding='latin1')]
    out, last, count = [], None, 0
    for line in lines + [None]:
        if line == last:
            count += 1
            continue
        if last is not None:
            out.append(last + (f'   x{count}' if count > 1 else ''))
        last, count = line, 1
    named = False
    for line in out:
        print(line)
        if line.startswith('CRASH:') and syms and not named:     # the first: later ones come from the report's own probing
            named = True
            addrs = re.findall(r'\b([0-9A-F]{8})\b', line)
            for a in addrs[:1] + addrs[2:12]:
                v = int(a, 16)
                if 0x401000 <= v < 0x1000000:
                    print(f'    {v:08X} {symbolize.name_of(syms, v)}')
    if os.path.exists(EXCEPT):
        print('_except.txt:')
        for line in open(EXCEPT, encoding='latin1').read().splitlines()[:4]:
            print('   ', line)


def run(seconds, fullscreen, args):
    for f in (LOG, EXCEPT):
        if os.path.exists(f):
            os.remove(f)
    env = dict(os.environ)
    if not fullscreen:
        env['ENB_POC_WINDOWED'] = '1'
    p = subprocess.Popen([EXE] + list(args), cwd=RELEASE, env=env)
    end = time.time() + seconds
    while time.time() < end and p.poll() is None:
        time.sleep(0.5)
    if p.poll() is None:
        print(f'still running after {seconds} s: killing PID {p.pid}')
        subprocess.run(['taskkill', '/F', '/PID', str(p.pid)], capture_output=True)
        p.wait()
    else:
        print(f'exited with code {p.returncode} (0x{p.returncode & 0xFFFFFFFF:08X})')
    digest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['run', 'log'])
    ap.add_argument('--seconds', type=int, default=40)
    ap.add_argument('--fullscreen', action='store_true')
    ap.add_argument('args', nargs='*')
    a = ap.parse_args()
    if a.command == 'run':
        run(a.seconds, a.fullscreen, a.args)
    else:
        digest()


if __name__ == '__main__':
    main()
