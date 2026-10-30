"""Compares our vidtest's frames with the original VIDTEST's, frame by frame (M2.2).

    python tools/harness/vidtest_sbs.py run [-- extra vidtest arguments]
    python tools/harness/vidtest_sbs.py sbs [N ...]

run: starts build/programs/vidtest/vidtest.exe with the reference's arguments (640x480x32 windowed;
     extra arguments, e.g. `-- -resx 800 -resy 600` for a reference run at that size, override them)
     and `-shots` set to the frame numbers saved in build/programs/vidtest/ref/ (run_reference.py),
     so both programs save the same presented frames (numbered from 1, one per Present). vidtest runs
     its tests to the end and exits with 1. Frames go to build/programs/vidtest/shots/. Unless the
     extra arguments give -sunangle, it passes the original's sun angle at its frame 700 (from the
     GetTickCount stamps in ref/d3d8shot.log), since VIDTEST turns its sun with the clock. Unless they
     give -art, it passes `-art build/art` when that folder exists, else warns (D120, artdir.py).
sbs: for every frame both have (or the frames given), writes ours (left) and the original's (right)
     side by side to build/programs/vidtest/sbs/sbs_NNNNN.png, and prints the share of pixels that
     differ by more than 60 (sum of |dR|+|dG|+|dB|), sampled every other pixel.
Both programs read their back buffer, never the screen. The frame numbers line up as long as both
take the same path through the tests (docs/vidtest.md 3): the loading screen is frame 1, the fill
rate test frames 2-62, then the polygon test.
"""
import ctypes
import os
import shutil
import subprocess
import sys
import time

import artdir
import bmp2png

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
OUT = os.path.join(REPO, 'build', 'programs', 'vidtest')
EXE = os.path.join(OUT, 'vidtest.exe')
REF = os.path.join(OUT, 'ref')
SHOTS = os.path.join(OUT, 'shots')
SBS = os.path.join(OUT, 'sbs')


def ref_frames():
    return sorted(int(f[6:11]) for f in os.listdir(REF) if f.startswith('frame_') and f.endswith('.bmp'))


def ref_sun_angle():
    """The original's sun angle (degrees) at its frame 700: VIDTEST turns the sun with GetTickCount
    (a = fmod(ticks * 5e-5, 2 pi), mirrored above pi; docs/vidtest.md 2.4), and d3d8shot.log stamps
    each saved frame with GetTickCount. None without the log."""
    import math
    import re
    log = os.path.join(REF, 'd3d8shot.log')
    if not os.path.exists(log):
        return None
    ticks = {}
    for line in open(log, encoding='latin1'):
        m = re.match(r'\s*(\d+) frame (\d+): saved', line)
        if m:
            ticks[int(m.group(2))] = int(m.group(1))
    if not ticks:
        return None
    t = ticks.get(700) or ticks[max(ticks)]
    a = math.fmod(t * 5e-5, 2 * math.pi)
    if a > math.pi:
        a = 2 * math.pi - a
    return int(round(math.degrees(a)))


def run(extra):
    frames = ref_frames()
    if not frames:
        raise SystemExit('no reference frames: run_reference.py run')
    if '-sunangle' not in extra:
        # the original's sun at the time of its frames (the land's lightmap and the scenes' light
        # depend on it: 35-40% of frames 500-900 differed with a random sun, 2-4% with this; M2.4)
        angle = ref_sun_angle()
        if angle is not None:
            extra = ['-sunangle', str(angle)] + extra
            print(f'-sunangle {angle} (the original\'s sun at its frame 700, from d3d8shot.log)')
    extra = artdir.art_args(extra) + extra   # build/art unless -art is given (D120, D69)
    if os.path.isdir(SHOTS):
        shutil.rmtree(SHOTS)
    # vidtest takes the first occurrence of an argument, so the extra arguments come first
    args = [EXE, '-nomsgbox'] + extra + ['-resx', '640', '-resy', '480', '-depth', '32', '-windowed', '1',
                            '-shots', ','.join(str(n) for n in frames), '-shotdir', 'shots']
    env = dict(os.environ, ENB_STUB_LOG=os.path.join(OUT, 'stubs.log'))
    t0 = time.time()
    p = subprocess.Popen(args, cwd=OUT, env=env)
    try:
        rc = ctypes.c_int32(p.wait(300)).value
    except subprocess.TimeoutExpired:
        p.kill()
        rc = 'timeout'
    n = len([f for f in os.listdir(SHOTS) if f.endswith('.bmp')]) if os.path.isdir(SHOTS) else 0
    print(f'vidtest exit {rc} in {time.time() - t0:.1f} s; {n} of {len(frames)} frames saved in {SHOTS}')


def sbs(want):
    os.makedirs(SBS, exist_ok=True)
    for f in sorted(os.listdir(SHOTS)):
        if not f.endswith('.bmp') or not os.path.exists(os.path.join(REF, f)):
            continue
        n = int(f[6:11])
        if want and n not in want:
            continue
        w1, h1, r1 = bmp2png.read_bmp(os.path.join(SHOTS, f))
        w2, h2, r2 = bmp2png.read_bmp(os.path.join(REF, f))
        h = max(h1, h2)
        gap = bytes([255, 255, 255]) * 8
        rows = [(bytes(r1[y]) if y < h1 else bytes(w1 * 3)) + gap + (bytes(r2[y]) if y < h2 else bytes(w2 * 3))
                for y in range(h)]
        out = os.path.join(SBS, 'sbs_%05d.png' % n)
        bmp2png.write_png(out, w1 + 8 + w2, h, rows)
        share = ''
        if (w1, h1) == (w2, h2):
            big = tot = 0
            for y in range(0, h1, 2):
                a, b = r1[y], r2[y]
                for x in range(0, w1, 2):
                    tot += 1
                    if sum(abs(a[x * 3 + c] - b[x * 3 + c]) for c in range(3)) > 60:
                        big += 1
            share = f'{100.0 * big / tot:5.1f}% of the pixels differ'
        print(f'frame {n:5d}: {share}  {out}')


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ('run', 'sbs'):
        raise SystemExit(__doc__)
    rest = [a for a in sys.argv[2:] if a != '--']
    if sys.argv[1] == 'run':
        run(rest)
    else:
        sbs([int(a) for a in rest])


if __name__ == '__main__':
    main()
