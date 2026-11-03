"""Runs the proof-of-concept client (D179; built by `tools/vc6/build.py link --program clientpoc`).

    python tools/harness/clientpoc.py run [--seconds N] [--fullscreen] [--shots N,M,...] [-- CLIENT ARGS...]
    python tools/harness/clientpoc.py replay [--instance NAME] [--exe PATH] --input FILE [--seconds N] [--shots N,M] [--messages] [-- CLIENT ARGS...]
    python tools/harness/clientpoc.py log         the last run's stubs.log, digested

Several runs at once (builders): give each its own --instance NAME. The run then uses
C:\\EnBDevelopment\\ClientPOC\\inst\\NAME\\release beside its own inst\\NAME\\Data (so the client's ..\\Data paths
resolve): Data\\client\\ini and output are the instance's own copies, every other folder a junction to
ClientPOC\\Data's (antprox /REPLAY rewrites ini\\auth.ini's AAIUrl / LKeyUrl for its own replay server at
each start: with one shared auth.ini the first-started client sent its account login to the second's server,
failed, and crashed in the error message box). The release folder is filled from release\\ on each run (client.exe and client.map, or --exe PATH's exe and map beside it; the DLLs and data files when
they differ), with its own stubs.log, _except.txt and d3d8.dll proxy, and shots in
.work/poc/shots/NAME-HHMMSS. The PID of the client is printed ("client pid N"); kill it by that PID only.
programs/clientpoc/poc_multi.cpp makes that possible without app code changes (mutex per process, no cursor
warp). --exe lets a worktree's `build.py link --program clientpoc --out DIR` output run without being deployed.

replay: antprox /REPLAY (D181; the loopback server for one client; each instance gets its own address, so
several replays run at once) starts the client (/CLIENT:<instance exe>), ENB_POC_INPUT=1, the --input steps
are posted to the window of the client.exe whose parent is that antprox (found by parent PID, not by name),
then both are killed by PID. antprox's console output and its per-run files go in the shot folder
(antprox.out, antprox-logs\\).

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

--messages (run, replay): starts the client with -ENABLE_MESSAGES (replay: antprox's /CLIENTOP:, as it does
every CLIENT ARG; it adds the '-') and tools/harness/msglisten.py beside it (--pid: the client's own port), which writes the
client's debug messages (G_ASSERT / FAIL_IF and the reporters' others) to <shots dir>\\messages.log. The
client drops messages until msglisten is accepted (the kernel's first message-server run, after the sound
library starts), so the start-up asserts reach only stubs.log (poc_standins.cpp's tee); after the run every
G_ASSERT / FAIL_IF line of stubs.log is appended to messages.log under a '# stubs.log' header.
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

MSGLISTEN = os.path.join(HERE, 'msglisten.py')


def start_listener(pid, out, seconds):
    """msglisten.py for the client PID, writing out\\messages.log."""
    log = os.path.join(out, 'messages.log')
    print(f'messages: {log}', flush=True)
    return subprocess.Popen([sys.executable, MSGLISTEN, '--pid', str(pid), '--out', log, '--seconds', str(seconds)],
                            stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)


def stop_listener(listener, inst=None, out=None):
    """Waits for msglisten; then appends stubs.log's G_ASSERT / FAIL_IF lines (programs/clientpoc/poc_standins.cpp's
    tee) to messages.log: the client drops the start-up ones (no listener yet), so the log has every report."""
    if listener is None:
        return
    try:
        listener.wait(timeout=5)
    except subprocess.TimeoutExpired:
        listener.kill()
    if inst is None or out is None or not os.path.exists(inst.log):
        return
    lines = [l.rstrip() for l in open(inst.log, encoding='latin1') if l.startswith(('G_ASSERT: ', 'FAIL_IF: '))]
    with open(os.path.join(out, 'messages.log'), 'a', encoding='utf-8', newline='\n') as f:
        f.write(f'# stubs.log: {len(lines)} G_ASSERT / FAIL_IF reports, in order, the start-up ones included '
                '(the client sends a report only while a listener is connected)\n')
        for l in lines:
            f.write(l + '\n')

RELEASE = r'C:\EnBDevelopment\ClientPOC\release'
EXE = os.path.join(RELEASE, 'client.exe')
MAP = os.path.join(RELEASE, 'client.map')
LOG = os.path.join(RELEASE, 'stubs.log')
EXCEPT = os.path.join(RELEASE, '_except.txt')
ANTPROX = r'C:\EnBDevelopment\AntaresProxy\src\Proxy\Antprox\bin\Release\net10.0\win-x64\publish\antprox.exe'
ROOT = os.path.dirname(os.path.dirname(HERE))
PROXY = os.path.join(ROOT, 'build', 'd3d8shot', 'd3d8.dll')
SHOTS = os.path.join(ROOT, '.work', 'poc', 'shots')


class Inst:
    """Where a run happens: the release folder, or ClientPOC\\inst\\NAME\\release filled from it."""
    def __init__(self, name=None, exe=None):
        self.name = name
        self.base = None if not name else os.path.join(os.path.dirname(RELEASE), 'inst', name)
        self.dir = RELEASE if not name else os.path.join(self.base, 'release')
        self.src_exe = exe or EXE
        self.exe = EXE if (not name and not exe) else os.path.join(self.dir, 'client.exe')
        self.map = os.path.join(self.dir, 'client.map')
        self.log = os.path.join(self.dir, 'stubs.log')
        self.exc = os.path.join(self.dir, '_except.txt')
        if name and not re.fullmatch(r'[A-Za-z0-9_.-]+', name):
            raise SystemExit('--instance: letters, digits, _ . - only')
        if exe and not name:
            raise SystemExit('--exe needs --instance (the release folder is not overwritten)')

    def prepare(self):
        if self.name:
            os.makedirs(self.dir, exist_ok=True)
            self._data()
            skip = {'client.exe', 'client.map', 'd3d8.dll', 'stubs.log', '_except.txt', 'd3d8shot.log',
                    'authlogin.log', 'stderr.txt'}
            for f in os.listdir(RELEASE):
                if f.lower() not in skip and os.path.isfile(os.path.join(RELEASE, f)):
                    self._copy(os.path.join(RELEASE, f), os.path.join(self.dir, f))
            self._copy(self.src_exe, self.exe)
            m = os.path.splitext(self.src_exe)[0] + '.map'
            self._copy(m if os.path.exists(m) else MAP, self.map)
        for f in (self.log, self.exc):
            if os.path.exists(f):
                os.remove(f)

    # Data\client's folders the instance gets its own copy of (antprox writes ini\auth.ini); the rest are junctions
    OWN = {'ini', 'output'}

    def _data(self):
        shared = os.path.join(os.path.dirname(RELEASE), 'Data')
        mine = os.path.join(self.base, 'Data')
        for top in os.listdir(shared):
            src, dst = os.path.join(shared, top), os.path.join(mine, top)
            if top.lower() != 'client' or not os.path.isdir(src):
                self._link(src, dst)
                continue
            os.makedirs(dst, exist_ok=True)
            for f in os.listdir(src):
                a, b = os.path.join(src, f), os.path.join(dst, f)
                if f.lower() in self.OWN and os.path.isdir(a):
                    if not os.path.exists(b):
                        shutil.copytree(a, b)
                else:
                    self._link(a, b)

    @staticmethod
    def _link(src, dst):
        """A junction to the shared folder src (a copy for a file), once."""
        if os.path.lexists(dst):
            return
        if os.path.isdir(src):
            r = subprocess.run(['cmd', '/c', 'mklink', '/J', dst, src], capture_output=True, text=True)
            if r.returncode:
                raise SystemExit(f'mklink /J {dst} {src}: {r.stdout}{r.stderr}')
        else:
            shutil.copy2(src, dst)

    @staticmethod
    def _copy(src, dst):
        a, b = os.stat(src), os.stat(dst) if os.path.exists(dst) else None
        if b is None or a.st_size != b.st_size or int(a.st_mtime) != int(b.st_mtime):
            shutil.copy2(src, dst)

    def shots_dir(self, prefix=''):
        tag = (self.name + '-' if self.name else '') + prefix
        return os.path.join(SHOTS, tag + time.strftime('%H%M%S'))


def descendants(pid):
    """(PID, image name) of every process below pid (Toolhelp32)."""
    class PE(ctypes.Structure):
        _fields_ = [('dwSize', wintypes.DWORD), ('cntUsage', wintypes.DWORD), ('pid', wintypes.DWORD),
                    ('heap', ctypes.c_void_p), ('mod', wintypes.DWORD), ('threads', wintypes.DWORD),
                    ('ppid', wintypes.DWORD), ('pri', wintypes.LONG), ('flags', wintypes.DWORD),
                    ('exe', ctypes.c_char * 260)]
    k = ctypes.windll.kernel32
    k.CreateToolhelp32Snapshot.restype = wintypes.HANDLE
    snap = k.CreateToolhelp32Snapshot(2, 0)
    procs = []
    e = PE()
    e.dwSize = ctypes.sizeof(PE)
    ok = k.Process32First(snap, ctypes.byref(e))
    while ok:
        procs.append((e.pid, e.ppid, e.exe.decode('latin1').lower()))
        ok = k.Process32Next(snap, ctypes.byref(e))
    k.CloseHandle(snap)
    found, todo = [], [pid]
    while todo:
        cur = todo.pop()
        for p, pp, name in procs:
            if pp == cur and p != cur:
                found.append((p, name))
                todo.append(p)
    return found


def digest(inst=None):
    inst = inst or Inst()
    LOG, MAP, EXCEPT = inst.log, inst.map, inst.exc
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


def run(seconds, fullscreen, args, shots=None, steps=None, inst=None, messages=False):
    inst = inst or Inst()
    inst.prepare()
    RELEASE, EXE = inst.dir, inst.exe
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
        out = inst.shots_dir()
        os.makedirs(out, exist_ok=True)
        shutil.copyfile(PROXY, proxy)
        env['D3D8SHOT_DIR'] = out
        env['D3D8SHOT_FRAMES'] = shots
    listener = None
    if messages:
        args = ['-ENABLE_MESSAGES'] + list(args)
        if not shots:
            out = inst.shots_dir()
            os.makedirs(out, exist_ok=True)
    try:
        p = subprocess.Popen([EXE] + list(args), cwd=RELEASE, env=env)
        print(f'client pid {p.pid}', flush=True)
        if messages:
            listener = start_listener(p.pid, out, seconds + 5)
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
        stop_listener(listener, inst, out if messages else None)
    digest(inst)
    if shots:
        convert_shots(out)


def convert_shots(out):
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


def replay(seconds, shots, steps, inst, messages=False, args=()):
    inst.prepare()
    env = dict(os.environ)
    env['ENB_POC_INPUT'] = '1'
    env['ENB_POC_WINDOWED'] = '1'
    proxy = os.path.join(inst.dir, 'd3d8.dll')
    if os.path.exists(proxy) and inst.dir == RELEASE:
        raise SystemExit(f'{proxy} exists already: not ours, not overwritten')
    out = inst.shots_dir('replay-' if not inst.name else '')
    os.makedirs(out, exist_ok=True)
    if shots:
        shutil.copyfile(PROXY, proxy)
        env['D3D8SHOT_DIR'] = out
        env['D3D8SHOT_FRAMES'] = shots
    logdir = os.path.join(out, 'antprox-logs')
    os.makedirs(logdir, exist_ok=True)
    # antprox picks its loopback address at start: two starting together can pick the same one, so start one at a
    # time (a lock folder in %TEMP%) and let each say "Replay server ready" before the next starts
    gate = os.path.join(os.environ.get('TEMP', '.'), 'clientpoc-antprox-start.lock')
    for _ in range(300):
        try:
            os.mkdir(gate)
            break
        except OSError:
            try:
                if time.time() - os.path.getmtime(gate) > 60:
                    shutil.rmtree(gate, ignore_errors=True)
            except OSError:
                pass
            time.sleep(0.2)
    args = (['-ENABLE_MESSAGES'] if messages else []) + list(args)
    a = subprocess.Popen([ANTPROX, '/REPLAY', '/CLIENT:' + inst.exe, '/LOGDIR:' + logdir]
                         + ['/CLIENTOP:' + x.lstrip('-') for x in args], cwd=inst.dir, env=env,  # antprox adds the '-'
                         stdout=open(os.path.join(out, 'antprox.out'), 'w'), stderr=subprocess.STDOUT)
    start = time.time()
    pid = None
    listener = None
    try:
        while time.time() - start < 20 and a.poll() is None:
            if 'Replay server ready' in open(os.path.join(out, 'antprox.out'), errors='replace').read():
                break
            time.sleep(0.1)
    finally:
        shutil.rmtree(gate, ignore_errors=True)
    try:
        while time.time() - start < 30 and pid is None and a.poll() is None:
            for p, name in descendants(a.pid):
                if name == 'client.exe':
                    pid = p
            time.sleep(0.5)
        print(f'antprox pid {a.pid}, client pid {pid}', flush=True)
        if messages and pid:
            listener = start_listener(pid, out, seconds + 5)
        hwnd, t0, pending = None, None, list(steps)
        end = time.time() + seconds
        while time.time() < end:
            if pending and hwnd is None and pid:
                hwnd = find_window(pid)
                if hwnd:
                    t0 = time.time()
                    print(f'input: window 0x{hwnd:X} at {t0 - start:.1f} s')
            while pending and hwnd and (time.time() - t0) * 1000 >= pending[0][0]:
                t, op, rest = pending.pop(0)
                post_step(hwnd, op, rest)
                print(f'input: {t} ms {op} {rest}')
            time.sleep(0.05)
    finally:
        if pid:
            subprocess.run(['taskkill', '/F', '/PID', str(pid)], capture_output=True)
        subprocess.run(['taskkill', '/F', '/T', '/PID', str(a.pid)], capture_output=True)
        time.sleep(1)
        if shots and os.path.exists(proxy):
            os.remove(proxy)
        stop_listener(listener, inst, out if messages else None)
    digest(inst)
    if shots:
        convert_shots(out)
    print('antprox output:', os.path.join(out, 'antprox.out'))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['run', 'replay', 'log'])
    ap.add_argument('--seconds', type=int, default=40)
    ap.add_argument('--fullscreen', action='store_true')
    ap.add_argument('--shots', help='Present calls to save the back buffer at, e.g. 30,300,900 (d3d8shot)')
    ap.add_argument('--input', help='a file of timed window messages to post (see the docstring)')
    ap.add_argument('--instance', help='run in ClientPOC\\inst\\NAME\\release with its own logs, proxy and shots (several at once)')
    ap.add_argument('--exe', help='a client.exe to run instead of release\\client.exe (needs --instance)')
    ap.add_argument('--messages', action='store_true',
                    help='-ENABLE_MESSAGES and msglisten.py beside the client: <shots dir>\\messages.log')
    ap.add_argument('args', nargs='*')
    a = ap.parse_args()
    inst = Inst(a.instance, a.exe)
    if a.command == 'run':
        run(a.seconds, a.fullscreen, a.args, a.shots, read_input(a.input) if a.input else None, inst, a.messages)
    elif a.command == 'replay':
        if not a.input:
            raise SystemExit('replay needs --input FILE')
        replay(a.seconds, a.shots, read_input(a.input), inst, a.messages, a.args)
    else:
        digest(inst)


if __name__ == '__main__':
    main()
