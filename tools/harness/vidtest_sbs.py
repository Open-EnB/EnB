"""Compares our vidtest's frames with the original VIDTEST's, frame by frame (M2.2).

    python tools/harness/vidtest_sbs.py run [-- extra vidtest arguments]
    python tools/harness/vidtest_sbs.py sbs [N ...]

run: starts build/programs/vidtest/vidtest.exe with the reference's arguments (640x480x32 windowed)
     and `-shots` set to the frame numbers saved in build/programs/vidtest/ref/ (run_reference.py),
     so both programs save the same presented frames (numbered from 1, one per Present). vidtest runs
     its tests to the end and exits with 1. Frames go to build/programs/vidtest/shots/.
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


def run(extra):
    frames = ref_frames()
    if not frames:
        raise SystemExit('no reference frames: run_reference.py run')
    if os.path.isdir(SHOTS):
        shutil.rmtree(SHOTS)
    args = [EXE, '-resx', '640', '-resy', '480', '-depth', '32', '-windowed', '1',
            '-shots', ','.join(str(n) for n in frames), '-shotdir', 'shots'] + extra
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
