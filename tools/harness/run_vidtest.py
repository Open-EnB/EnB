"""Runs our vidtest.exe unattended on the host and checks it (L2, D48).

    python tools/harness/run_vidtest.py [--exe build/programs/vidtest/vidtest.exe] [--mode frames|close]
                                        [--frames 120] [--expect 0,0,127] [-- extra vidtest arguments]

frames: vidtest renders --frames frames, saves its client area to shot.bmp and quits by itself.
close:  vidtest runs until this script posts WM_CLOSE to its window (after --seconds), as a user
        closing the window would; vidtest saves shot.bmp itself at frame 30.
The screenshot is vidtest's own (its D3D back buffer, read before the frame is presented): never a
screen grab, which would read whatever window happens to be on top.
Then: the exit code must be 0, vidtest.log must show WW3D::Init and Set_Render_Device = 0, and
the screenshot's pixels must be the clear colour (--expect, default VIDTEST's L2 dark blue
(0, 0, 0.5) = 0,0,127). The stub log (stubs.log, D44) is printed. Exit 0 when all pass.
"""
import argparse
import collections
import ctypes
import ctypes.wintypes as wt
import os
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
user32 = ctypes.windll.user32


def bmp_colours(path):
    d = open(path, 'rb').read()
    off = struct.unpack_from('<I', d, 10)[0]
    w, h = struct.unpack_from('<ii', d, 18)
    stride = ((w * 3) + 3) & ~3
    c = collections.Counter()
    for y in range(0, abs(h), 2):
        for x in range(0, w, 2):
            b, g, r = d[off + y * stride + x * 3: off + y * stride + x * 3 + 3]
            c[(r, g, b)] += 1
    return w, abs(h), c


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=os.path.join(REPO, 'build', 'programs', 'vidtest', 'vidtest.exe'))
    ap.add_argument('--mode', choices=['frames', 'close'], default='frames')
    ap.add_argument('--frames', type=int, default=120)
    ap.add_argument('--seconds', type=float, default=3.0)
    ap.add_argument('--expect', default='0,0,127')
    ap.add_argument('--timeout', type=float, default=60)
    ap.add_argument('extra', nargs='*')
    a = ap.parse_args()
    cwd = os.path.dirname(os.path.abspath(a.exe))
    for f in ('vidtest.log', 'stubs.log', 'shot.bmp'):
        if os.path.exists(os.path.join(cwd, f)):
            os.remove(os.path.join(cwd, f))
    env = dict(os.environ, ENB_STUB_LOG=os.path.join(cwd, 'stubs.log'))
    shot = os.path.join(cwd, 'shot.bmp')
    if a.mode == 'frames':
        args = [a.exe, '-frames', str(a.frames), '-shot', 'shot.bmp'] + a.extra
        p = subprocess.Popen(args, cwd=cwd, env=env)
        rc = p.wait(a.timeout)
    else:
        p = subprocess.Popen([a.exe, '-shot', 'shot.bmp', '-shotframe', '30'] + a.extra, cwd=cwd, env=env)
        hwnd = 0
        t0 = time.time()
        while not hwnd and time.time() - t0 < a.timeout:
            time.sleep(0.2)
            hwnd = user32.FindWindowA(b'VidTestWndClass', None)
        time.sleep(a.seconds)
        if hwnd:
            user32.PostMessageA(hwnd, 0x0010, 0, 0)   # WM_CLOSE
        rc = p.wait(a.timeout)
    rc = ctypes.c_int32(rc).value
    log = open(os.path.join(cwd, 'vidtest.log'), encoding='latin1').read() if os.path.exists(os.path.join(cwd, 'vidtest.log')) else ''
    print(log.rstrip())
    checks = {'exit code 0': rc == 0,
              'WW3D::Init = 0': 'WW3D::Init = 0' in log,
              'Set_Render_Device = 0': any(l.startswith('WW3D::Set_Render_Device') and l.endswith('= 0')
                                           for l in log.splitlines()),
              'WW3D::Shutdown = 0': 'WW3D::Shutdown = 0' in log}
    if os.path.exists(shot):
        w, h, c = bmp_colours(shot)
        want = tuple(int(x) for x in a.expect.split(','))
        share = c[want] / max(1, sum(c.values()))
        print(f'screenshot {w}x{h}: {share:.1%} of sampled pixels are {want}; most common {c.most_common(3)}')
        checks['clear colour'] = share > 0.95
    else:
        checks['clear colour'] = False
    stubs = os.path.join(cwd, 'stubs.log')
    print('stub log:', open(stubs).read().strip() if os.path.exists(stubs) else '(none: no stub was called)')
    for k, v in checks.items():
        print(f'  {"PASS" if v else "FAIL"}  {k}')
    print(f'exit code {rc}')
    sys.exit(0 if all(checks.values()) else 1)


if __name__ == '__main__':
    main()
