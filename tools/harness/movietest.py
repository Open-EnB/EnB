"""The movie task playing one of the game's Bink movies (docs/kernel-map.md 6, step 7): tests/capharness/movietest.

    ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/movietest.py build [--out DIR]
    python tools/harness/movietest.py run [--exe EXE] [--movie eb_ws_logo] [--frames N] [--shots N,N]

build: links movietest with app\\kernel\\kernel.cpp and app\\client\\tasks\\tsk_movie.cpp (compiled
into the program) and Renegade's Code\\BinkMovie (EA's GPL source, staged from EA_BINKMOVIE; not in the
tree) against DIR's libraries into DIR/programs/movietest/.
run: copies binkw32.dll and the movie from the game's install (read only; the program never runs there)
into .work/harness/movietest, runs it windowed (no message boxes), and checks its log: the movie
opened, the task took the focus, frames were drawn, the task removed itself at the movie's end (or
-frames stopped it), the screenshots were saved (from the program's own back buffer; .png beside them
when bmp2png.py can convert).
"""
import argparse
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WORK = os.path.join(REPO, '.work', 'harness', 'movietest')
SOURCES = [r'app\kernel\kernel.cpp', r'app\client\tasks\tsk_movie.cpp']
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import roots  # noqa: E402
EA_BINKMOVIE = os.path.join(roots.EA_ROOT, 'CnC_Renegade', 'Code', 'BinkMovie')
GAME = roots.RETAIL_INSTALL
BINK_DLL = os.path.join(GAME, 'release', 'binkw32.dll')
MOVIES = os.path.join(GAME, 'Data', 'client', 'mixfiles')


def build(out):
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import build as B
    src = os.path.join(REPO, 'tests', 'capharness', 'movietest')
    stage = os.path.join(REPO, '.work', 'harness', 'movietest_src')
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(stage)
    for fn in os.listdir(src):
        shutil.copy(os.path.join(src, fn), stage)
    # Renegade's player: BINKMovie.cpp/.h is included by binkmovie_adapter.cpp (renamed); the
    # subtitle files compile as they are
    # (the VM gets the stage directory only, so everything sits in it: BINKMovie.cpp as an .inl;
    # <wwlibector.h> becomes <vector.h>, the include path has wwlib itself)
    for fn in os.listdir(EA_BINKMOVIE):
        low = fn.lower()
        if low == 'binkmovie.cpp':
            dst = 'binkmovie_renegade.inl'
        elif low == 'binkmovie.h' or low.startswith('subtitle'):
            dst = low
        else:
            continue
        text = open(os.path.join(EA_BINKMOVIE, fn), encoding='latin1').read()
        text = text.replace(r'<wwlib\vector.h>', '<vector.h>')
        # the tree's WW3D2 (2004) against Renegade's: the mip count enum is global, the texture
        # accessor is Peek_D3D_Texture
        text = text.replace('TextureClass::MIP_LEVELS_1', 'MIP_LEVELS_1')
        text = text.replace('Peek_DX8_Texture()', 'Peek_D3D_Texture()')
        open(os.path.join(stage, dst), 'w', encoding='latin1', newline='').write(text)
    shutil.copy(os.path.join(REPO, 'app', 'kernel', 'kernel.h'), stage)
    with open(os.path.join(stage, 'app_sources.cpp'), 'w', newline='\r\n') as f:
        f.write('// app\\kernel and app\\client\\tasks, compiled into the program (movietest.py build)\n')
        for p in SOURCES:
            f.write('#include "' + os.path.relpath(os.path.join(REPO, p), stage) + '"\n')
    B.PROGRAMS['movietest'] = {'dir': os.path.relpath(stage, REPO), 'flags': B.I.LIB_FLAGS['ww3d2'] + ' /GX',
                               'subsystem': 'windows', 'runtime': []}
    B.link_program('movietest', out)
    exe = os.path.join(out, 'programs', 'movietest', 'movietest.exe')
    if not os.path.exists(exe):
        raise SystemExit('no exe: see ' + os.path.join(out, 'programs', 'movietest'))
    print('built', exe)


def run(exe, movie, frames, shots):
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(MOVIES, movie + '.bik')
    if not os.path.exists(src):
        src = os.path.join(MOVIES, movie + '.bik.old')		# the install's renamed copies
    if not os.path.exists(BINK_DLL) or not os.path.exists(src):
        raise SystemExit('no binkw32.dll or ' + movie + ' in the install')
    shutil.copy(BINK_DLL, WORK)
    shutil.copy(src, os.path.join(WORK, movie + '.bik'))
    shotdir = os.path.join(WORK, 'shots')
    shutil.rmtree(shotdir, ignore_errors=True)
    log = os.path.join(WORK, 'movietest.log')
    args = [os.path.abspath(exe), '-movie', movie + '.bik', '-log', log, '-shotdir', shotdir]
    if frames:
        args += ['-frames', str(frames)]
    if shots:
        args += ['-shots', shots]
    p = subprocess.run(args, cwd=WORK, timeout=600)
    text = open(log).read() if os.path.exists(log) else ''
    print(text[-3000:])
    want = ['BINKMovie::Play(%s.bik, volume 0x8000): open' % movie, 'focus: the movie task']
    want.append('frame limit' if frames else 'the movie task removed itself')
    missing = [w for w in want if w not in text]
    saved = sorted(f for f in os.listdir(shotdir)) if os.path.isdir(shotdir) else []
    if shots:
        try:
            sys.path.insert(0, HERE)
            import bmp2png
            for f in saved:
                if f.endswith('.bmp'):
                    w, h, rows = bmp2png.read_bmp(os.path.join(shotdir, f))
                    bmp2png.write_png(os.path.join(shotdir, f[:-4] + '.png'), w, h, rows)
        except Exception as e:  # noqa: BLE001
            print('png conversion skipped:', e)
    print('movietest: exit code %d, screenshots %s, missing: %s' % (p.returncode, ', '.join(saved) or 'none', missing or 'nothing'))
    return p.returncode == 0 and not missing


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['build', 'run'])
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    ap.add_argument('--exe')
    ap.add_argument('--movie', default='eb_ws_logo')
    ap.add_argument('--frames', type=int, default=0)
    ap.add_argument('--shots', default='1,30,60')
    a = ap.parse_args()
    if a.cmd == 'build':
        build(a.out)
        return 0
    exe = a.exe or os.path.join(a.out, 'programs', 'movietest', 'movietest.exe')
    return 0 if run(exe, a.movie, a.frames, a.shots) else 1


if __name__ == '__main__':
    sys.exit(main())
