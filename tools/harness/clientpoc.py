"""Runs the proof-of-concept client (D179; built by `tools/vc6/build.py link --program clientpoc`).

    python tools/harness/clientpoc.py run [--seconds N] [--fullscreen] [--shots N,M,...] [-- CLIENT ARGS...]
    python tools/harness/clientpoc.py replay [--instance NAME] [--exe PATH | --retail] --input FILE [--seconds N] [--shots N,M] [--messages] [-- CLIENT ARGS...]
    python tools/harness/dumpcmp.py A B           compares two draw dumps (d3d8shot D3D8SHOT_DUMP folders), see below
    python tools/harness/clientpoc.py log         the last run's stubs.log, digested
    python tools/harness/clientpoc.py clean --instance NAME | --all | --older HOURS
                                                  delete instances (a builder its own before reporting; the
                                                  coordinator --all at handoff); junctions unlinked, never followed

Several runs at once (builders): give each its own --instance NAME. The run then uses
C:\\EnBDevelopment\\ClientPOC\\inst\\NAME\\release beside its own inst\\NAME\\Data (so the client's ..\\Data paths
resolve): Data\\client\\ini and output are the instance's own copies, every other folder a junction to
ClientPOC\\Data's (antprox /REPLAY rewrites ini\\auth.ini's AAIUrl / LKeyUrl for its own replay server at
each start: with one shared auth.ini the first-started client sent its account login to the second's server,
failed, and crashed in the error message box). The release folder is filled from release\\ on each run (client.exe and client.map, or --exe PATH's exe and map beside it; the DLLs and data files when
they differ), with its own stubs.log, _except.txt and d3d8.dll proxy, and shots in
.work/poc/shots/NAME-HHMMSS. The PID of the client is printed ("client pid N"); kill it by that PID only.
programs/clientpoc/poc_multi.cpp makes that possible without app code changes (mutex per process, no cursor
warp). --exe runs a worktree's own link output (WT\\build\\programs\\clientpoc\\clientpoc.exe, its map beside it) whatever
release\\ holds: builders' checks use it (docs/builders.md, POC instances); it must exist.

replay: antprox /REPLAY (D181; the loopback server for one client; each instance gets its own address, so
several replays run at once) starts the client (/CLIENT:<instance exe>), ENB_POC_INPUT=1, the --input steps
are posted to the window of the client.exe whose parent is that antprox (found by parent PID, not by name),
then both are killed by PID. antprox's console output and its per-run files go in the shot folder
(antprox.out, antprox-logs\\).

--ea (with --instance, P-EA1-1): the instance lives in C:\\EnBDevelopment\\ClientPOC_EA\\inst\\NAME over that folder's
Data and DLLs, EA's final set (hash-proven against EA's manifest; only the *.th6 thumbnails the client writes are added). The
exe is unchanged: ClientPOC\\release's last build, --exe's, or --retail's copy of 249f8524. Its release\\client.exe is EA's own
82663b2f, kept as data and never run by this tool.

run: starts C:\\EnBDevelopment\\ClientPOC\\release\\client.exe with that folder as working directory (never
the game's install), ENB_POC_WINDOWED=1 unless --fullscreen, waits N seconds (default 40) and kills it by PID
if it still runs; then prints the digest. Delete stubs.log and _except.txt first (the client's exception
report, written beside the exe).

replay prints, after the run, whether the client exited by itself and when (D191): antprox.out's "Client exited"
line against the time the last input step was posted ("exited by itself at ..., +2.4 s after the last input (step
15000 click ...)"), or "still running when this run killed it". `run` reports the exit code of the process itself.

replay --retail (D190): runs a COPY of the retail client (INSTALL/release/client.exe, SHA 249f8524) copied into the
instance folder (--instance is required; the install folder is only read, D69 / D143) on the same replay input as the
POC. The d3d8shot proxy then runs with D3D8SHOT_INPUT=1: it patches the client's user32 imports (GetMessageA /
PeekMessageA, GetCursorPos, GetMessagePos, SetCursorPos, GetForegroundWindow, GetActiveWindow, GetKeyState,
GetAsyncKeyState, ClipCursor) to read the posted messages, as programs/clientpoc/poc_input.cpp does for the POC, so the
real cursor and keyboard are never used. The terms box ("I Agree") is the dialog's own input: a BM_CLICK is posted to
its button (a thread of replay); the input steps go to the main window (dialogs are skipped when looking for it). There
is no stubs.log. --shots is required (the proxy carries the hooks; `run_reference.py build` first, the proxy must be
rebuilt for them). A retail draw dump: D3D8SHOT_DUMP=N in the environment; compare dumps with
tools/harness/dumpcmp.py.

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
    T down X Y          WM_MOUSEMOVE, WM_LBUTTONDOWN (a drag's start; 'move' steps, then 'up')
    T up X Y            WM_MOUSEMOVE (button held), WM_LBUTTONUP
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
import datetime
import threading
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
EA_RELEASE = r'C:\EnBDevelopment\ClientPOC_EA\release'
RETAIL = os.path.join(os.environ.get('ENB_RETAIL_INSTALL') or r'C:\Program Files (x86)\EA GAMES\Earth & Beyond',
                      'release', 'client.exe')
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
    def __init__(self, name=None, exe=None, retail=False, ea=False):
        self.name = name
        self.source = EA_RELEASE if ea else RELEASE      # the DLLs and, beside it, the shared Data
        if ea and not os.path.isdir(EA_RELEASE):
            raise SystemExit(f'--ea: no {EA_RELEASE} (see docs/progress.md, P-EA1-1)')
        if ea and not name:
            raise SystemExit('--ea needs --instance')
        self.base = None if not name else os.path.join(os.path.dirname(self.source), 'inst', name)
        self.dir = RELEASE if not name else os.path.join(self.base, 'release')
        self.retail = retail
        self.src_exe = RETAIL if retail else (exe or EXE)
        self.exe = EXE if (not name and not exe) else os.path.join(self.dir, 'client.exe')
        self.map = os.path.join(self.dir, 'client.map')
        self.log = os.path.join(self.dir, 'stubs.log')
        self.exc = os.path.join(self.dir, '_except.txt')
        if name and not re.fullmatch(r'[A-Za-z0-9_.-]+', name):
            raise SystemExit('--instance: letters, digits, _ . - only')
        if (exe or retail) and not name:
            raise SystemExit('--exe / --retail need --instance (the release folder is not overwritten)')
        if exe and retail:
            raise SystemExit('--exe and --retail exclude each other')

    def prepare(self):
        if self.name:
            os.makedirs(self.dir, exist_ok=True)
            self._data()
            skip = {'client.exe', 'client.map', 'd3d8.dll', 'stubs.log', '_except.txt', 'd3d8shot.log',
                    'authlogin.log', 'stderr.txt'}
            for f in os.listdir(self.source):
                if f.lower() not in skip and os.path.isfile(os.path.join(self.source, f)):
                    self._copy(os.path.join(self.source, f), os.path.join(self.dir, f))
            self._copy(self.src_exe, self.exe)
            m = os.path.splitext(self.src_exe)[0] + '.map'
            if not self.retail:
                self._copy(m if os.path.exists(m) else MAP, self.map)
        for f in (self.log, self.exc):
            if os.path.exists(f):
                os.remove(f)

    # Data\client's folders the instance gets its own copy of (antprox writes ini\auth.ini); the rest are junctions
    OWN = {'ini', 'output'}

    def _data(self):
        shared = os.path.join(os.path.dirname(self.source), 'Data')
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
    """(PID, image name) of every process below pid (Toolhelp32); pid None: every process."""
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
    if pid is None:                                     # every process
        return [(p, name) for p, _, name in procs]
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


def find_window(pid, skip_dialogs=False):
    user32 = ctypes.windll.user32
    found = []
    proc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

    def cb(hwnd, _):
        owner = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(owner))
        if owner.value == pid and user32.IsWindowVisible(hwnd):
            k = ctypes.create_unicode_buffer(64)
            user32.GetClassNameW(hwnd, k, 64)
            if not (skip_dialogs and k.value == '#32770'):      # the retail client's terms box
                found.append(hwnd)
        return True
    user32.EnumWindows(proc(cb), 0)
    return found[0] if found else None


TAG = 0x5043 << 16      # programs/clientpoc/poc_input.cpp's POC_TAG: the high word of a replayed message's wParam


def post_step(hwnd, op, rest, tag=False):
    """Posts one input step. tag: wParam's high word carries TAG (poc_input.cpp strips it and logs the message as
    'replayed'; with ENB_POC_INPUT_TAG=1 it drops untagged input, a human's). Only for a POC built with that hook."""
    user32 = ctypes.windll.user32
    t = TAG if tag else 0
    post = lambda m, w, l: user32.PostMessageW(hwnd, m, w | t, l)
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
    elif op in ('down', 'up'):
        x, y = (int(v) for v in rest.split()[:2])
        lp = (y << 16) | (x & 0xFFFF)
        post(WM['move'], 0 if op == 'down' else 1, lp)
        time.sleep(0.05)
        post(WM['ldown'] if op == 'down' else WM['lup'], 1 if op == 'down' else 0, lp)
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
            post_step(hwnd, 'key', str(vk), tag)
            if shift:
                post(WM['keyup'], 0x10, 1 | (user32.MapVirtualKeyW(0x10, 0) << 16) | (3 << 30))
            time.sleep(0.03)
    else:
        raise SystemExit(f'--input: unknown step {op!r}')


def click_agree(pid, seconds):
    """The retail client's terms box: BM_CLICK to its "I Agree" button (the dialog's own input, no screen access)."""
    user32 = ctypes.windll.user32
    proc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    end = time.time() + seconds
    while time.time() < end:
        hits = []

        def cb(h, _):
            owner = wintypes.DWORD()
            user32.GetWindowThreadProcessId(h, ctypes.byref(owner))
            k = ctypes.create_unicode_buffer(64)
            user32.GetClassNameW(h, k, 64)
            if owner.value == pid and k.value == '#32770' and user32.IsWindowVisible(h):
                hits.append(h)
            return True
        user32.EnumWindows(proc(cb), 0)
        for h in hits:
            b = user32.FindWindowExW(h, None, 'Button', 'I Agree')
            if b:
                time.sleep(0.5)
                user32.PostMessageW(b, 0x00F5, 0, 0)       # BM_CLICK
                print(f'clicked I Agree (dialog 0x{h:X})', flush=True)
                return
        time.sleep(0.3)
    print('terms box not seen: no "I Agree" click', flush=True)


def exit_report(antprox_out, last_input, killed):
    """D191: did the client exit by itself, and when? antprox.out's "Client exited" line (local wall clock,
    'dd/mm/yy HH:MM:SS.mmm') against the time of the last posted input step and of this run's own kill."""
    text = open(antprox_out, errors='replace').read()
    m = re.search(r'^(\d\d/\d\d/\d\d \d\d:\d\d:\d\d\.\d+) \[\s*\d+\] Client exited', text, re.M)
    if not m:
        print('client exit: no "Client exited" line in antprox.out: still running when this run killed it' if killed
              else 'client exit: no "Client exited" line in antprox.out')
        return
    at = datetime.datetime.strptime(m.group(1), '%d/%m/%y %H:%M:%S.%f')
    if killed and at >= killed - datetime.timedelta(seconds=0.5):
        print(f'client exit: still running when this run killed it (antprox logged the exit at {m.group(1)[9:]})')
        return
    if last_input:
        t, op, rest, when = last_input
        d = (at - when).total_seconds()
        print(f'client exit: exited by itself at {m.group(1)[9:]}, {d:+.1f} s after the last input '
              f'(step {t} ms {op} {rest}, posted {when.strftime("%H:%M:%S.%f")[:-3]})')
    else:
        print(f'client exit: exited by itself at {m.group(1)[9:]} (no input posted)')


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


def start_antprox(inst, out, env, args):
    """antprox /REPLAY starting inst's client (out\antprox.out, out\antprox-logs); (antprox, client PID or None,
    start time). Used by replay and tools/harness/e2e.py."""
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
    a = subprocess.Popen([ANTPROX, '/REPLAY', '/CLIENT:' + inst.exe, '/LOGDIR:' + logdir]
                         + ['/CLIENTOP:' + x.lstrip('-') for x in args], cwd=inst.dir, env=env,  # antprox adds the '-'
                         stdout=open(os.path.join(out, 'antprox.out'), 'w'), stderr=subprocess.STDOUT)
    start = time.time()
    pid = None
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
    except BaseException:
        subprocess.run(['taskkill', '/F', '/T', '/PID', str(a.pid)], capture_output=True)
        raise
    return a, pid, start


def replay(seconds, shots, steps, inst, messages=False, args=()):
    inst.prepare()
    env = dict(os.environ)
    env['ENB_POC_INPUT'] = '1'
    env['ENB_POC_WINDOWED'] = '1'
    if inst.retail:
        env['D3D8SHOT_INPUT'] = '1'      # d3d8shot.c Hook_Input: the retail client reads the posted messages
        if not shots:
            raise SystemExit('--retail needs --shots (the d3d8shot proxy carries the input hooks)')
    proxy = os.path.join(inst.dir, 'd3d8.dll')
    if os.path.exists(proxy) and inst.dir == RELEASE:
        raise SystemExit(f'{proxy} exists already: not ours, not overwritten')
    out = inst.shots_dir('replay-' if not inst.name else '')
    os.makedirs(out, exist_ok=True)
    if shots:
        shutil.copyfile(PROXY, proxy)
        env['D3D8SHOT_DIR'] = out
        env['D3D8SHOT_FRAMES'] = shots
    args = (['-ENABLE_MESSAGES'] if messages else []) + list(args)
    listener = None
    last_input, killed, self_exit = None, None, False
    a, pid, start = start_antprox(inst, out, env, args)
    try:
        print(f'antprox pid {a.pid}, client pid {pid}', flush=True)
        if messages and pid:
            listener = start_listener(pid, out, seconds + 5)
        if inst.retail and pid:
            threading.Thread(target=click_agree, args=(pid, 60), daemon=True).start()
        hwnd, t0, pending = None, None, list(steps)
        end = time.time() + seconds
        seen = 0
        while time.time() < end:
            if time.time() - seen > 1:
                seen = time.time()
                if a.poll() is not None or 'Client exited' in open(os.path.join(out, 'antprox.out'),
                                                                   errors='replace').read():
                    self_exit = True
                    break
            if pending and hwnd is None and pid:
                hwnd = find_window(pid, inst.retail)
                if hwnd:
                    t0 = time.time()
                    print(f'input: window 0x{hwnd:X} at {t0 - start:.1f} s')
            while pending and hwnd and (time.time() - t0) * 1000 >= pending[0][0]:
                t, op, rest = pending.pop(0)
                post_step(hwnd, op, rest)
                last_input = (t, op, rest, datetime.datetime.now())
                print(f'input: {t} ms {op} {rest}')
            time.sleep(0.05)
        if pending:
            print(f'input: {len(pending)} steps not sent')
    finally:
        # D207: a client (or antprox) that exited by itself was not killed; exit_report reads killed=None as that
        killed = None if self_exit else datetime.datetime.now()
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
    exit_report(os.path.join(out, 'antprox.out'), last_input, killed)
    print('antprox output:', os.path.join(out, 'antprox.out'))


def running_from(folder):
    """PIDs of client.exe processes whose image lies under folder (QueryFullProcessImageNameW)."""
    k = ctypes.windll.kernel32
    k.OpenProcess.restype = wintypes.HANDLE
    pids = [p for p, name in descendants(None) if name == 'client.exe']
    hit, root = [], os.path.normcase(os.path.abspath(folder)) + os.sep
    for p in pids:
        h = k.OpenProcess(0x1000, False, p)              # PROCESS_QUERY_LIMITED_INFORMATION
        if not h:
            continue
        buf, n = ctypes.create_unicode_buffer(1024), wintypes.DWORD(1024)
        if k.QueryFullProcessImageNameW(h, 0, buf, ctypes.byref(n)) and \
                os.path.normcase(buf.value).startswith(root):
            hit.append(p)
        k.CloseHandle(h)
    return hit


def _is_link(p):
    """A junction or symlink (os.walk would descend into a junction)."""
    return os.path.islink(p) or bool(getattr(os.lstat(p), 'st_reparse_tag', 0))


def _walk_own(base):
    """os.walk over base that never enters a junction; yields (dirpath, files, links)."""
    for dirpath, dirnames, files in os.walk(base):
        links = [d for d in dirnames if _is_link(os.path.join(dirpath, d))]
        dirnames[:] = [d for d in dirnames if d not in links]
        yield dirpath, files, links


def remove_instance(base):
    """Delete an instance folder: its junctions into ClientPOC\\Data are unlinked first (os.rmdir on the junction
    itself), never followed, so the shared data stays; then the rest (its own copies, logs, exe) is deleted."""
    for dirpath, _, links in list(_walk_own(base)):
        for d in links:
            os.rmdir(os.path.join(dirpath, d))           # the link only
    shutil.rmtree(base)


def clean(name=None, older=None):
    """Delete ClientPOC\\inst\\NAME and ClientPOC_EA\\inst\\NAME (or every instance, or those unused for `older`
    hours), skipping any whose client still runs. Shots and antprox logs stay in .work/poc/shots."""
    tops = [os.path.join(os.path.dirname(r), 'inst') for r in (RELEASE, EA_RELEASE)]
    if name:
        found = [os.path.join(t, name) for t in tops if os.path.isdir(os.path.join(t, name))]
        if not found:
            print(f'{name}: no such instance')
    else:
        found = [os.path.join(t, n) for t in tops if os.path.isdir(t) for n in sorted(os.listdir(t))]
    freed = 0
    for base in found:
        n = os.path.basename(base)
        if older is not None and time.time() - os.stat(os.path.join(base, 'release')
                                                         if os.path.isdir(os.path.join(base, 'release'))
                                                         else base).st_mtime < older * 3600:
            continue
        busy = running_from(base)
        if busy:
            print(f'{n}: client.exe still running (pid {", ".join(map(str, busy))}); kept')
            continue
        size = sum(os.path.getsize(os.path.join(d, f)) for d, fs, _ in _walk_own(base) for f in fs)
        remove_instance(base)
        freed += size
        print(f'{n}: removed')
    print(f'clean: {freed / 1048576:.0f} MB freed')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['run', 'replay', 'log', 'clean'])
    ap.add_argument('--all', action='store_true', help='clean: every instance')
    ap.add_argument('--older', type=float, metavar='HOURS', help='clean: instances unused for HOURS')
    ap.add_argument('--seconds', type=int, default=40)
    ap.add_argument('--fullscreen', action='store_true')
    ap.add_argument('--shots', help='Present calls to save the back buffer at, e.g. 30,300,900 (d3d8shot)')
    ap.add_argument('--input', help='a file of timed window messages to post (see the docstring)')
    ap.add_argument('--instance', help='run in ClientPOC\\inst\\NAME\\release with its own logs, proxy and shots (several at once)')
    ap.add_argument('--exe', help='an exe to run instead of release\\client.exe, e.g. a worktree\'s '
                    'build\\programs\\clientpoc\\clientpoc.exe (needs --instance)')
    ap.add_argument('--retail', action='store_true',
                    help='replay: run a copy of the retail client under the d3d8shot input hooks (D190; needs --instance, --shots)')
    ap.add_argument('--ea', action='store_true',
                    help='run over EA\'s final data, ClientPOC_EA (needs --instance; the exe is still ClientPOC\\release\'s, --exe\'s or --retail\'s)')
    ap.add_argument('--messages', action='store_true',
                    help='-ENABLE_MESSAGES and msglisten.py beside the client: <shots dir>\\messages.log')
    ap.add_argument('args', nargs='*')
    a = ap.parse_args()
    if a.command == 'clean':
        if not (a.instance or a.all or a.older is not None):
            raise SystemExit('clean needs --instance NAME, --all or --older HOURS')
        return clean(a.instance, a.older)
    if a.exe is not None and not os.path.isfile(a.exe):
        raise SystemExit(f'--exe {a.exe!r}: no such file (an empty --exe would run release\\client.exe instead)')
    inst = Inst(a.instance, a.exe, a.retail, a.ea)
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
