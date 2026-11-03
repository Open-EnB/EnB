"""Runs the proof-of-concept client (D179; built by `tools/vc6/build.py link --program clientpoc`).

    python tools/harness/clientpoc.py run [--seconds N] [--fullscreen] [--shots N,M,...] [-- CLIENT ARGS...]
    python tools/harness/clientpoc.py log         the last run's stubs.log, digested

run: starts C:\\EnBDevelopment\\ClientPOC\\release\\client.exe with that folder as working directory (never
the game's install), ENB_POC_WINDOWED=1 unless --fullscreen, waits N seconds (default 40) and kills it by PID
if it still runs; then prints the digest. Delete stubs.log and _except.txt first (the client's exception
report, written beside the exe).

--shots N,M: saves the client's own back buffer at those Present calls (1 = the first frame presented;
never a screen grab, D69) through the d3d8shot proxy (build/d3d8shot/d3d8.dll, built by
`run_reference.py build`): the proxy is copied beside the exe for the run and deleted after it, so later
runs load the system's d3d8.dll. The frames go to .work/poc/shots/<HHMMSS>/frame_NNNNN.png (converted
by bmp2png.py, the .bmp deleted) with the proxy's d3d8shot.log (the Present count, the files opened).

The digest: stubs.log's lines in order, repeats folded ("x12"), each CRASH: line (poc_window.cpp's
vectored handler) with its addresses named from client.map (tools/harness/symbolize.py), and the first
lines of _except.txt. Nothing is run against a server (antprox stays off: it writes the registry).

--input FILE: posts window messages to the client's top-level window (found by its PID) at the times the
file gives; never the real cursor or keyboard (PostMessage only: nothing reaches other programs). It sets
ENB_POC_INPUT=1: programs/clientpoc/poc_input.cpp then gives the client a virtual cursor and keyboard (the
client reads GetCursorPos / GetMessagePos, not the message's coordinates, and takes input only while its
window is in the foreground). One step
a line, '#' comments; T is milliseconds after the window first appears, X Y client-area pixels:
    T move X Y          WM_MOUSEMOVE
    T click X Y         WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_LBUTTONUP (50 ms apart)
    T rclick X Y        the same with the right button
    T key VK            WM_KEYDOWN / WM_KEYUP (VK decimal or 0x hex, e.g. 0x0D Enter, 0x09 Tab)
    T text STRING       WM_KEYDOWN / WM_KEYUP per character (VkKeyScan; Shift around capitals)
Each step is logged to stdout with the time it was sent.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import time
import ctypes
from ctypes import wintypes

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import symbolize  # noqa: E402
import bmp2png  # noqa: E402

RELEASE = r'C:\EnBDevelopment\ClientPOC\release'
EXE = os.path.join(RELEASE, 'client.exe')
MAP = os.path.join(RELEASE, 'client.map')
LOG = os.path.join(RELEASE, 'stubs.log')
EXCEPT = os.path.join(RELEASE, '_except.txt')
ROOT = os.path.dirname(os.path.dirname(HERE))
PROXY = os.path.join(ROOT, 'build', 'd3d8shot', 'd3d8.dll')
SHOTS = os.path.join(ROOT, '.work', 'poc', 'shots')


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


WM = {'move': 0x200, 'ldown': 0x201, 'lup': 0x202, 'rdown': 0x204, 'rup': 0x205,
      'keydown': 0x100, 'keyup': 0x101, 'char': 0x102}


def read_input(path):
    steps = []
    for line in open(path, encoding='utf-8'):
        line = line.rstrip('\n')
        if not line.strip() or line.lstrip().startswith('#'):
            continue
        t, op, rest = (line.split(None, 2) + ['', ''])[:3]
        steps.append((int(t), op, rest))
    return sorted(steps, key=lambda s: s[0])


def find_window(pid):
    user32 = ctypes.windll.user32
    found = []
    proc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

    def cb(hwnd, _):
        owner = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(owner))
        if owner.value == pid and user32.IsWindowVisible(hwnd):
            found.append(hwnd)
        return True
    user32.EnumWindows(proc(cb), 0)
    return found[0] if found else None


def post_step(hwnd, op, rest):
    user32 = ctypes.windll.user32
    post = lambda m, w, l: user32.PostMessageW(hwnd, m, w, l)
    if op in ('move', 'click', 'rclick'):
        x, y = (int(v) for v in rest.split()[:2])
        lp = (y << 16) | (x & 0xFFFF)
        post(WM['move'], 0, lp)
        if op != 'move':
            down, up, mk = (('ldown', 'lup', 1) if op == 'click' else ('rdown', 'rup', 2))
            time.sleep(0.05)
            post(WM[down], mk, lp)
            time.sleep(0.05)
            post(WM[up], 0, lp)
    elif op == 'key':
        vk = int(rest.split()[0], 0)
        scan = user32.MapVirtualKeyW(vk, 0)
        post(WM['keydown'], vk, 1 | (scan << 16))
        time.sleep(0.05)
        post(WM['keyup'], vk, 1 | (scan << 16) | (3 << 30))
    elif op == 'text':
        for ch in rest:
            vs = user32.VkKeyScanW(ord(ch))
            vk, shift = vs & 0xFF, bool(vs & 0x100)
            if shift:
                post(WM['keydown'], 0x10, 1 | (user32.MapVirtualKeyW(0x10, 0) << 16))
            post_step(hwnd, 'key', str(vk))
            if shift:
                post(WM['keyup'], 0x10, 1 | (user32.MapVirtualKeyW(0x10, 0) << 16) | (3 << 30))
            time.sleep(0.03)
    else:
        raise SystemExit(f'--input: unknown step {op!r}')


def run(seconds, fullscreen, args, shots=None, steps=None):
    for f in (LOG, EXCEPT):
        if os.path.exists(f):
            os.remove(f)
    env = dict(os.environ)
    if not fullscreen:
        env['ENB_POC_WINDOWED'] = '1'
    if steps:
        env['ENB_POC_INPUT'] = '1'
    proxy = os.path.join(RELEASE, 'd3d8.dll')
    if shots:
        if not os.path.exists(PROXY):
            raise SystemExit(f'{PROXY} missing: python tools/harness/run_reference.py build')
        if os.path.exists(proxy):
            raise SystemExit(f'{proxy} exists already: not ours, not overwritten')
        out = os.path.join(SHOTS, time.strftime('%H%M%S'))
        os.makedirs(out, exist_ok=True)
        shutil.copyfile(PROXY, proxy)
        env['D3D8SHOT_DIR'] = out
        env['D3D8SHOT_FRAMES'] = shots
    try:
        p = subprocess.Popen([EXE] + list(args), cwd=RELEASE, env=env)
        start = time.time()
        end = start + seconds
        hwnd, t0, pending = None, None, list(steps or [])
        while time.time() < end and p.poll() is None:
            if pending and hwnd is None:
                hwnd = find_window(p.pid)
                if hwnd:
                    t0 = time.time()
                    print(f'input: window 0x{hwnd:X} at {t0 - start:.1f} s')
            while pending and hwnd and (time.time() - t0) * 1000 >= pending[0][0]:
                t, op, rest = pending.pop(0)
                post_step(hwnd, op, rest)
                print(f'input: {t} ms {op} {rest} (sent at {(time.time() - t0) * 1000:.0f} ms)')
            time.sleep(0.05 if pending else 0.5)
        if pending:
            print(f'input: {len(pending)} steps not sent')
        if p.poll() is None:
            print(f'still running after {seconds} s: killing PID {p.pid}')
            subprocess.run(['taskkill', '/F', '/PID', str(p.pid)], capture_output=True)
            p.wait()
        else:
            print(f'exited with code {p.returncode} (0x{p.returncode & 0xFFFFFFFF:08X})')
    finally:
        if shots and os.path.exists(proxy):
            os.remove(proxy)
    digest()
    if shots:
        for name in sorted(os.listdir(out)):
            if name.endswith('.bmp'):
                bmp = os.path.join(out, name)
                png = bmp[:-4] + '.png'
                w, h, rows = bmp2png.read_bmp(bmp)
                bmp2png.write_png(png, w, h, rows)
                os.remove(bmp)
                print('shot:', png)
        log = os.path.join(out, 'd3d8shot.log')
        if os.path.exists(log):
            presents = [l for l in open(log, encoding='latin1') if 'frame' in l]
            print(f'd3d8shot.log: {len(presents)} frame lines; the last: {presents[-1].strip() if presents else "-"}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['run', 'log'])
    ap.add_argument('--seconds', type=int, default=40)
    ap.add_argument('--fullscreen', action='store_true')
    ap.add_argument('--shots', help='Present calls to save the back buffer at, e.g. 30,300,900 (d3d8shot)')
    ap.add_argument('--input', help='a file of timed window messages to post (see the docstring)')
    ap.add_argument('args', nargs='*')
    a = ap.parse_args()
    if a.command == 'run':
        run(a.seconds, a.fullscreen, a.args, a.shots, read_input(a.input) if a.input else None)
    else:
        digest()


if __name__ == '__main__':
    main()
