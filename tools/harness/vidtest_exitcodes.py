"""Checks vidtest's exit codes and results.ini (M2.3, docs/vidtest.md 1.1, 1.2, 3) unattended.

    python tools/harness/vidtest_exitcodes.py [--exe build/programs/vidtest/vidtest.exe]
                                              [--dir build/programs/vidtest/exitcodes] [--case NAME]...
                                              [--ref build/programs/vidtest/ref_800/results.ini]

Each case runs vidtest in --dir (its working directory: results.ini and vidtest.log land there,
never in the install) with E&BConfig's command line form, after deleting results.ini:

  finish  the tests run to the end: exit 1 and results.ini written with VIDTEST's sections and keys,
          in VIDTEST's order (compared with --ref, the original's file from run_reference.py)
  close   WM_CLOSE posted to the window while the tests run: exit -3, no results.ini
  key     WM_KEYDOWN (space) posted instead: exit -3, no results.ini
  mode    an unsupported video mode (-resx/-resy 100000): exit -1, no results.ini. vidtest then shows
          VIDTEST's message box ("Your hardware does not support..."); this script closes it (IDOK)
          and prints its text

The window and the message box are found by the process id (never by grabbing the screen).
Exit 0 when every case passes.
"""
import argparse
import ctypes
import ctypes.wintypes as wt
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
user32 = ctypes.windll.user32

WM_CLOSE = 0x0010
WM_KEYDOWN = 0x0100
WM_COMMAND = 0x0111
VK_SPACE = 0x20
IDOK = 1

DEVICE = 'NVIDIA GeForce RTX 3060 Ti'


def windows_of(pid):
    """(hwnd, class, title) of the visible top-level windows of process pid."""
    found = []
    proto = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)

    def cb(hwnd, _):
        p = wt.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(p))
        if p.value == pid and user32.IsWindowVisible(hwnd):
            cls = ctypes.create_unicode_buffer(256)
            user32.GetClassNameW(hwnd, cls, 256)
            title = ctypes.create_unicode_buffer(256)
            user32.GetWindowTextW(hwnd, title, 256)
            found.append((hwnd, cls.value, title.value))
        return True
    user32.EnumWindows(proto(cb), 0)
    return found


def dialog_text(hwnd):
    texts = []
    proto = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)

    def cb(child, _):
        t = ctypes.create_unicode_buffer(512)
        user32.GetWindowTextW(child, t, 512)
        if t.value:
            texts.append(t.value)
        return True
    user32.EnumChildWindows(hwnd, proto(cb), 0)
    return texts


def read_ini(path):
    """[(section, [(key, value)...])...] in file order."""
    out = []
    for line in open(path, 'r', encoding='latin-1').read().splitlines():
        line = line.strip()
        if line.startswith('[') and line.endswith(']'):
            out.append((line[1:-1], []))
        elif '=' in line and out:
            k, v = line.split('=', 1)
            out[-1][1].append((k, v))
    return out


def run(exe, wd, args, action=None, delay=0.0, timeout=60.0):
    """Runs vidtest; action 'close'/'key' posts to its main window delay s after it appears, 'dialog'
    closes its message box. Returns (exit code, notes)."""
    notes = []
    p = subprocess.Popen([exe] + args, cwd=wd)
    t0 = time.time()
    done = False
    while p.poll() is None and time.time() - t0 < timeout:
        wins = windows_of(p.pid)
        if action in ('close', 'key') and not done:
            main = [w for w in wins if w[1] == 'VidTestWndClass' or w[2] == 'E&B Video Test']
            main = [w for w in main if w[1] != '#32770']
            if main:
                time.sleep(delay)
                if action == 'close':
                    user32.PostMessageW(main[0][0], WM_CLOSE, 0, 0)
                else:
                    user32.PostMessageW(main[0][0], WM_KEYDOWN, VK_SPACE, 0)
                notes.append(f'{action} posted {time.time() - t0:.2f} s after the start')
                done = True
        for hwnd, cls, title in wins:
            if cls == '#32770':
                note = f'message box "{title}": {dialog_text(hwnd)}'
                if note not in notes:
                    notes.append(note)
                user32.PostMessageW(hwnd, WM_COMMAND, IDOK, 0)
                time.sleep(0.5)
        time.sleep(0.01)
    if p.poll() is None:
        p.kill()
        notes.append('killed after the timeout')
        return None, notes
    return ctypes.c_int32(p.returncode).value, notes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=os.path.join(REPO, 'build', 'programs', 'vidtest', 'vidtest.exe'))
    ap.add_argument('--dir', default=os.path.join(REPO, 'build', 'programs', 'vidtest', 'exitcodes'))
    ap.add_argument('--ref', default=os.path.join(REPO, 'build', 'programs', 'vidtest', 'ref_800', 'results.ini'))
    ap.add_argument('--case', action='append', choices=['finish', 'close', 'key', 'mode'])
    ap.add_argument('--delay', type=float, default=0.3, help='seconds after the window appears (close, key)')
    a = ap.parse_args()
    exe = os.path.abspath(a.exe)
    wd = os.path.abspath(a.dir)
    os.makedirs(wd, exist_ok=True)
    base = ['-device', DEVICE, '-resx', '800', '-resy', '600', '-depth', '32', '-windowed', 'yes', '-filter', '1']
    cases = {
        'finish': (base, None, 1, True),
        'close': (base, 'close', -3, False),
        'key': (base, 'key', -3, False),
        'mode': (['-device', DEVICE, '-resx', '100000', '-resy', '100000', '-depth', '32', '-windowed', 'yes',
                  '-filter', '1'], 'dialog', -1, False),
    }
    ok = True
    ini = os.path.join(wd, 'results.ini')
    for name in a.case or ['finish', 'close', 'key', 'mode']:
        args, action, want, want_ini = cases[name]
        if os.path.exists(ini):
            os.remove(ini)
        code, notes = run(exe, wd, args + ['-log', f'vidtest_{name}.log'], action, a.delay)
        has_ini = os.path.exists(ini)
        good = code == want and has_ini == want_ini
        print(f'{name}: exit {code} (want {want}), results.ini {"written" if has_ini else "absent"}'
              f' (want {"written" if want_ini else "absent"}): {"PASS" if good else "FAIL"}')
        for n in notes:
            print('   ', n)
        log = os.path.join(wd, f'vidtest_{name}.log')
        if os.path.exists(log):
            for line in open(log, 'r', encoding='latin-1'):
                if line.startswith(('frames presented', 'WW3D::Set_Render_Device')):
                    print('    log:', line.strip())
        if name == 'finish' and has_ini:
            ours = read_ini(ini)
            for sec, kv in ours:
                print(f'    [{sec}] ' + ' '.join(f'{k}={v}' for k, v in kv))
            if os.path.exists(a.ref):
                ref = read_ini(a.ref)
                shape = [(s, [k for k, _ in kv]) for s, kv in ours]
                rshape = [(s, [k for k, _ in kv]) for s, kv in ref]
                same = shape == rshape
                print(f'    sections and keys in order vs {os.path.relpath(a.ref, REPO)}: '
                      f'{"same" if same else "DIFFERENT"}')
                rd = {(s, k): v for s, kv in ref for k, v in kv}
                for s, kv in ours:
                    for k, v in kv:
                        if rd.get((s, k)) != v:
                            print(f'      {s}.{k}: ours {v}, original {rd.get((s, k))}')
                good = good and same
            raw = open(ini, 'rb').read()
            if b'\n' in raw.replace(b'\r\n', b''):
                print('    line ends: not CRLF')
                good = False
        ok = ok and good
    print('all pass' if ok else 'FAILED')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
