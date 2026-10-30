"""Runs the ORIGINAL VIDTEST.EXE on the host and saves its frames: the M2 reference (M2.1, D48).

    python tools/harness/run_reference.py build [--pool 1]     # d3d8shot.c -> build/d3d8shot/d3d8.dll (VC6 VM)
    python tools/harness/run_reference.py run [--every 50] [--frames 1,30] [--max 120] [--timeout 300]
                                              [--dump 500,900] [--out DIR] [--exe PATH]
                                              [-- extra VIDTEST arguments]

`run` copies the EBCONFIG install folder to build/programs/vidtest/ref_ebconfig/ (once; the
original writes results.ini into its own folder, so it never runs in the install), puts
build/d3d8shot/d3d8.dll beside the copy's VIDTEST.EXE and starts it there with
`-resx 640 -resy 480 -depth 32 -windowed 1` unless other arguments are given. d3d8.dll is a proxy
(tools/harness/d3d8shot.c): it passes everything to the system's d3d8.dll and copies the program's
own back buffer at the chosen Present calls; no screen grab. It also logs the program's CreateFileA
calls. VIDTEST ends by itself when its tests are done (exit 1, results.ini written); after
--timeout seconds the script posts WM_CLOSE (exit -3). Output in build/programs/vidtest/ref/:
d3d8shot.log, frame_NNNNN.bmp and .png, results.ini (a copy), and run.txt (exit code, time).

--dump F,... also writes every indexed draw call of those frames (vertices, indices, transforms,
textures) to draws_NNNNN.bin (d3d8shot.c; read it with tools/harness/drawdump.py). --out DIR writes
the output there instead of build/programs/vidtest/ref/ (the reference frames stay). --exe PATH runs
another D3D8 program the same way, e.g. our build/programs/vidtest/vidtest.exe: it is copied with the
proxy into DIR/proxy_run/ and started there (its working directory; give it -nomsgbox).
"""
import argparse
import ctypes
import os
import shutil
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
INSTALL = os.path.join(os.environ.get('ENB_RETAIL_INSTALL') or r'C:\Program Files (x86)\EA GAMES\Earth & Beyond', 'EBCONFIG')
OUT = os.path.join(REPO, 'build', 'programs', 'vidtest')
DLL_DIR = os.path.join(REPO, 'build', 'd3d8shot')

DEF = 'LIBRARY d3d8\r\nEXPORTS\r\n    Direct3DCreate8=_Direct3DCreate8@4\r\n'


def build(pool):
    import vc6
    import importer as I
    os.makedirs(DLL_DIR, exist_ok=True)
    shutil.copy(os.path.join(HERE, 'd3d8shot.c'), os.path.join(DLL_DIR, 'd3d8shot.c'))
    open(os.path.join(DLL_DIR, 'd3d8shot.def'), 'w', newline='').write(DEF)

    def go():
        w = vc6.WORK + '\\d3d8shot'
        vc6.run_bat([f'if exist "{w}" rmdir /s /q "{w}"', f'mkdir "{w}"'])
        for f in ('d3d8shot.c', 'd3d8shot.def'):
            vc6.scp(os.path.join(DLL_DIR, f), vc6.remote(w + '\\' + f))
        vc6.run_bat([f'cd /d "{w}"',
                     'cl /nologo /c /MT /O2 /W3 d3d8shot.c > build.log 2>&1',
                     'link /nologo /dll /machine:ix86 /def:d3d8shot.def /out:d3d8.dll d3d8shot.obj '
                     'kernel32.lib user32.lib >> build.log 2>&1',
                     'dumpbin /exports d3d8.dll > exports.txt 2>&1'])
        for f in ('d3d8.dll', 'build.log', 'exports.txt'):
            try:
                vc6.scp(vc6.remote(w + '\\' + f), os.path.join(DLL_DIR, f))
            except Exception as e:      # noqa: BLE001 - report what the VM produced
                print(f'{f}: not copied ({e})')
        vc6.ssh(f'cmd /c rmdir /s /q "{w}"', check=False)

    if pool:
        with I.pool_lease(pool):
            go()
    else:
        go()
    print(open(os.path.join(DLL_DIR, 'build.log'), encoding='latin1').read().strip()[-1500:])
    if os.path.exists(os.path.join(DLL_DIR, 'exports.txt')):
        ex = open(os.path.join(DLL_DIR, 'exports.txt'), encoding='latin1').read()
        print('exports Direct3DCreate8:', 'Direct3DCreate8' in ex)


def run(a):
    import bmp2png
    dll = os.path.join(DLL_DIR, 'd3d8.dll')
    if not os.path.exists(dll):
        raise SystemExit('build the proxy first: run_reference.py build')
    ref = os.path.join(OUT, 'ref_ebconfig')
    if not os.path.isdir(ref):
        shutil.copytree(INSTALL, ref)
        print(f'copied {INSTALL} -> {ref}')
    shutil.copy(dll, os.path.join(ref, 'd3d8.dll'))
    res = os.path.join(ref, 'RESULTS.INI')
    before = open(res, 'rb').read() if os.path.exists(res) else None
    dst = os.path.abspath(a.out) if a.out else os.path.join(OUT, 'ref')
    if os.path.isdir(dst):
        shutil.rmtree(dst)
    os.makedirs(dst)
    env = dict(os.environ, D3D8SHOT_DIR=dst, D3D8SHOT_EVERY=str(a.every), D3D8SHOT_FRAMES=a.frames,
               D3D8SHOT_MAX=str(a.max), D3D8SHOT_DUMP=a.dump)
    args = a.extra or ['-resx', '640', '-resy', '480', '-depth', '32', '-windowed', '1']
    exe = os.path.join(ref, 'VIDTEST.EXE')
    if a.exe:
        ref = os.path.join(dst, 'proxy_run')
        os.makedirs(ref)
        exe = os.path.join(ref, os.path.basename(a.exe))
        shutil.copy(a.exe, exe)
        shutil.copy(dll, os.path.join(ref, 'd3d8.dll'))
        res = os.path.join(ref, 'results.ini')
        before = None
        env['ENB_STUB_LOG'] = os.path.join(dst, 'stubs.log')
    t0 = time.time()
    p = subprocess.Popen([exe] + args, cwd=ref, env=env)
    try:
        rc = p.wait(a.timeout)
        how = 'ended by itself'
    except subprocess.TimeoutExpired:
        hwnd = ctypes.windll.user32.FindWindowA(b'VidTestWndClass', None)
        if hwnd:
            ctypes.windll.user32.PostMessageA(hwnd, 0x0010, 0, 0)    # WM_CLOSE, as a user would
        try:
            rc = p.wait(30)
            how = f'closed with WM_CLOSE after {a.timeout} s'
        except subprocess.TimeoutExpired:
            p.kill()
            rc = p.wait()
            how = 'killed'
    rc = ctypes.c_int32(rc).value
    secs = time.time() - t0
    after = open(res, 'rb').read() if os.path.exists(res) else None
    if after is not None:
        shutil.copy(res, os.path.join(dst, 'results.ini'))
    shots = sorted(f for f in os.listdir(dst) if f.endswith('.bmp'))
    for f in shots:
        bmp2png.write_png(os.path.join(dst, f[:-4] + '.png'), *bmp2png.read_bmp(os.path.join(dst, f)))
    summary = (f'{os.path.basename(exe)} {" ".join(args)}: exit code {rc}, {how}, {secs:.1f} s; {len(shots)} frames saved; '
               f'results.ini {"changed" if after != before else "unchanged"}')
    open(os.path.join(dst, 'run.txt'), 'w').write(summary + '\n')
    print(summary)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['build', 'run'])
    ap.add_argument('--pool', type=int, default=1)
    ap.add_argument('--every', type=int, default=50)
    ap.add_argument('--frames', default='1,10,30')
    ap.add_argument('--max', type=int, default=120)
    ap.add_argument('--timeout', type=float, default=300)
    ap.add_argument('--dump', default='', help='frames whose draw calls are dumped (draws_NNNNN.bin)')
    ap.add_argument('--out', help='output folder (default build/programs/vidtest/ref)')
    ap.add_argument('--exe', help='run this program instead of the original VIDTEST.EXE')
    ap.add_argument('extra', nargs='*')
    a = ap.parse_args()
    if a.command == 'build':
        build(a.pool)
    else:
        run(a)


if __name__ == '__main__':
    main()
