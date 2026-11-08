"""The importer lock, the agent name and its scratch folder, and pool_lease (split from importer.py).

One verify (or compile_and_match user) at a time: .work/locks/importer.lock (wt.py reads the same
file and format). The agent (--agent, ENB_AGENT, else the git worktree name) owns the lock and its
scratch folder WORK.
"""
import json
import os
import re
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))


def _worktree_agent():
    """The git worktree's directory name when cwd is inside a worktree other than the main tree."""
    import subprocess
    try:
        top, common = [subprocess.run(['git', 'rev-parse', a], capture_output=True, text=True, timeout=20).stdout.strip()
                       for a in ('--show-toplevel', '--git-common-dir')]
        if top and common:
            main_top = os.path.dirname(os.path.abspath(common))
            if os.path.normcase(os.path.abspath(top)) != os.path.normcase(main_top):
                return os.path.basename(os.path.abspath(top))
    except Exception:
        pass
    return None


def _scratch(agent):
    """Per-agent scratch <repo>/.work/<agent>/tmp/enb_import; without an agent, %TEMP%/enb_import."""
    if not agent:
        return os.path.join(os.environ.get('TEMP', '.'), 'enb_import')
    return os.path.join(REPO, '.work', re.sub(r'[^A-Za-z0-9._-]', '_', agent), 'tmp', 'enb_import')


def set_agent(name):
    """--agent NAME: the lock owner and scratch folder; children inherit ENB_AGENT."""
    global WORK
    os.environ['ENB_AGENT'] = name
    WORK = _scratch(name)
    os.makedirs(WORK, exist_ok=True)


_AGENT = os.environ.get('ENB_AGENT') or _worktree_agent()
if _AGENT:
    os.environ['ENB_AGENT'] = _AGENT
WORK = _scratch(_AGENT)
os.makedirs(WORK, exist_ok=True)

# One importer at a time: every run that writes WORK or imports.json (verify, and the
# tryv2/tryvariants/trydiff scripts through compile_and_match and ledger.save) holds
# .work/locks/importer.lock from its first write until the process exits. A second run waits up
# to --lock-wait seconds (ENB_LOCK_WAIT, default 600), then exits non-zero. A lock whose process
# is gone, or older than LOCK_STALE (the longest refine/assemble), is taken over.
LOCK = os.path.join(REPO, '.work', 'locks', 'importer.lock')
LOCK_STALE = 4 * 3600
LOCK_WAIT = int(os.environ.get('ENB_LOCK_WAIT', '600'))
_lock_held = False
_lock_guard = threading.Lock()


def _pid_alive(pid):
    if os.name != 'nt':
        try:
            os.kill(pid, 0)
        except ProcessLookupError:
            return False
        except PermissionError:
            pass
        return True
    import ctypes  # os.kill(pid, 0) would terminate the process on Windows
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, pid)  # PROCESS_QUERY_LIMITED_INFORMATION
    if not h:
        return ctypes.GetLastError() == 5  # access denied: it exists
    code = ctypes.c_ulong()
    ok = k.GetExitCodeProcess(h, ctypes.byref(code))
    k.CloseHandle(h)
    return not ok or code.value == 259  # STILL_ACTIVE


def _release_lock():
    try:
        if json.load(open(LOCK)).get('pid') == os.getpid():
            os.remove(LOCK)
    except (OSError, ValueError):
        pass


def hold_lock(wait=None):
    """Take the importer lock for the rest of this process (re-entrant, thread-safe)."""
    global _lock_held
    with _lock_guard:
        if _lock_held:
            return
        import atexit
        import socket
        os.makedirs(os.path.dirname(LOCK), exist_ok=True)
        me = {'agent': os.environ.get('ENB_AGENT') or f"{os.environ.get('USERNAME', '?')}@{socket.gethostname()}",
              'host': socket.gethostname(), 'pid': os.getpid(), 'cmd': ' '.join(sys.argv), 'start': time.time()}
        limit = time.time() + (LOCK_WAIT if wait is None else wait)
        said = None
        while True:
            try:
                fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                os.write(fd, json.dumps(me).encode())
                os.close(fd)
                break
            except FileExistsError:
                pass
            try:
                h = json.load(open(LOCK))
            except (OSError, ValueError):  # being written or removed: look again
                h = None
            if h is not None:
                who = f"{h.get('agent')} (pid {h.get('pid')}, {h.get('cmd')}) since " + \
                      time.strftime('%H:%M', time.localtime(h.get('start', 0)))
                dead = h.get('host') == me['host'] and not _pid_alive(h.get('pid', 0))
                if dead or time.time() - h.get('start', 0) > LOCK_STALE:
                    print(f"importer lock: taking over from {who}: {'process gone' if dead else 'stale'}", flush=True)
                    try:
                        os.remove(LOCK)
                    except OSError:
                        pass
                    continue
                if said != who:
                    print(f'importer in use by {who}, waiting', flush=True)
                    said = who
            if time.time() > limit:
                raise SystemExit(f'importer lock: still held after waiting; giving up ({LOCK})')
            time.sleep(2)
        _lock_held = True
        atexit.register(_release_lock)



def release_lock():
    """Give the lock back before the process exits (auxpad.py's per-variant lock)."""
    global _lock_held
    with _lock_guard:
        _release_lock()
        _lock_held = False


import contextlib  # noqa: E402


@contextlib.contextmanager
def pool_lease(count, holder_pid=None):
    """Lease `count` VMs from pool.py as agent importer-<pid>, point vc6.hosts() at them through
    ENB_VC6_HOSTS for the block, and release the lease afterwards (stopping idle VMs is pool.py's
    job). pool.py runs as a child process: it never takes the importer lock, so the lock this
    process holds cannot deadlock it."""
    import subprocess
    pool = [sys.executable, os.path.join(HERE, 'pool.py')]
    agent = f'importer-{os.getpid()}'
    cmd = pool + ['lease', '--agent', agent, '--count', str(count)]
    if holder_pid:
        cmd += ['--holder-pid', str(holder_pid)]
    old = os.environ.get('ENB_VC6_HOSTS')
    try:
        out = subprocess.run(cmd, stdout=subprocess.PIPE, text=True)
        line = [l for l in out.stdout.splitlines() if l.startswith('ENB_VC6_HOSTS=')]
        if out.returncode or not line:
            raise SystemExit(f'pool.py lease failed (exit {out.returncode}): {out.stdout.strip()}')
        os.environ['ENB_VC6_HOSTS'] = line[-1].split('=', 1)[1]
        print(f'verify on pool VMs ({agent}): {os.environ["ENB_VC6_HOSTS"]}', flush=True)
        yield os.environ['ENB_VC6_HOSTS']
    finally:
        if old is None:
            os.environ.pop('ENB_VC6_HOSTS', None)
        else:
            os.environ['ENB_VC6_HOSTS'] = old
        rel = subprocess.run(pool + ['release', '--agent', agent], stdout=subprocess.PIPE, text=True)
        print(f'pool: {rel.stdout.strip()}', flush=True)

