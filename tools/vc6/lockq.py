"""First-come lock file with a waiting queue (D238): the importer lock's core, shared by vc6lock.py
and wt.py. No side effects on import (wt.py must not pick up vc6lock's agent and scratch folder).

The lock is the file LOCK (created with O_EXCL; JSON with agent, host, pid, cmd, start), as before
D238, so a process running the older code (which only knows the lock file and polls it every 2 s)
still excludes and is excluded. New code adds a queue: a waiter writes a ticket
LOCK.queue/<time_ns>-<pid>.json (pid, the process's start time, agent, cmd, since) and refreshes the
ticket's mtime while it waits. Arrival order is the ticket names' order. Only the head of the live
queue takes a free lock; a ticket whose process is gone (or a different process under a reused pid),
or untouched for TICKET_STALE seconds, is deleted by whoever reads the queue, so the queue cannot
deadlock on a dead waiter. A new-code holder that releases with waiters queued hands the lock
straight to the head (the lock file is replaced by one naming the head's pid and ticket, never
removed), so an old-code poller does not see it free in between; an old-code holder removes the
file, and then the head (polling every POLL s, 2 s for the old code) is the likely, not certain,
winner: first come among new-code waiters, best effort toward old ones.
"""
import json
import os
import socket
import sys
import time

POLL = 0.25                 # seconds between a waiter's looks at the lock
TICKET_STALE = 120          # a ticket whose mtime is older than this is a waiter that stopped waiting
PROGRESS = 60               # seconds between a waiter's progress lines (and on every change)


def queue_dir(lock):
    return lock + '.queue'


def pid_alive(pid):
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
    h = k.OpenProcess(0x1000, False, int(pid))  # PROCESS_QUERY_LIMITED_INFORMATION
    if not h:
        return ctypes.GetLastError() == 5  # access denied: it exists
    code = ctypes.c_ulong()
    ok = k.GetExitCodeProcess(h, ctypes.byref(code))
    k.CloseHandle(h)
    return not ok or code.value == 259  # STILL_ACTIVE


def pid_started(pid):
    """The process's creation time (FILETIME ticks on Windows, /proc start ticks elsewhere), or None."""
    try:
        if os.name != 'nt':
            with open(f'/proc/{int(pid)}/stat') as f:
                return int(f.read().rsplit(')', 1)[1].split()[19])
        import ctypes
        k = ctypes.windll.kernel32
        h = k.OpenProcess(0x1000, False, int(pid))
        if not h:
            return None
        t = [ctypes.c_ulonglong() for _ in range(4)]
        ok = k.GetProcessTimes(h, *[ctypes.byref(x) for x in t])
        k.CloseHandle(h)
        return t[0].value if ok else None
    except (OSError, ValueError, IndexError):
        return None


def _same_process(pid, start):
    if not pid_alive(pid):
        return False
    now = pid_started(pid)
    return start is None or now is None or now == start


def read_json(path):
    try:
        with open(path) as f:
            return json.load(f)
    except (OSError, ValueError):
        return None


def tickets(lock, prune=True):
    """The live tickets, first come first: [(file name, ticket dict)]. Dead or stale ones are deleted
    (prune) or left out."""
    q = queue_dir(lock)
    try:
        files = sorted(f for f in os.listdir(q) if f.endswith('.json'))
    except FileNotFoundError:
        return []
    live, now = [], time.time()
    for f in files:
        p = os.path.join(q, f)
        t = read_json(p)
        try:
            ok = (t is not None and t.get('host') == socket.gethostname()
                  and _same_process(int(t['pid']), t.get('pstart'))
                  and now - os.path.getmtime(p) <= TICKET_STALE)
        except (OSError, ValueError, KeyError, TypeError):
            ok = False
        if t is None and now - _mtime(p) < 5:
            continue  # being written (os.replace makes this rare): neither live nor dead yet
        if ok:
            live.append((f, t))
        elif prune:
            _remove(p)
    return live


def _mtime(p):
    try:
        return os.path.getmtime(p)
    except OSError:
        return 0


def _remove(p):
    try:
        os.remove(p)
    except OSError:
        pass


def add_ticket(lock, me):
    q = queue_dir(lock)
    os.makedirs(q, exist_ok=True)
    name = f'{time.time_ns():020d}-{os.getpid()}.json'
    t = dict(me, pstart=pid_started(os.getpid()), since=time.time())
    tmp = os.path.join(q, name + '.tmp')
    with open(tmp, 'w', newline='\n') as f:
        json.dump(t, f)
    os.replace(tmp, os.path.join(q, name))
    return name


def _write_lock(lock, data):
    """Create the lock file (O_EXCL): True when this call made it."""
    try:
        fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        return False
    try:
        os.write(fd, json.dumps(data).encode())
    finally:
        os.close(fd)
    return True


def describe(h):
    return f"{h.get('agent')} (pid {h.get('pid')}, {h.get('cmd')}) since " + \
        time.strftime('%H:%M', time.localtime(h.get('start', 0) or 0))


def _takeover(lock, h, stale, log):
    """Remove a lock whose process is gone (same host) or older than `stale` seconds; True if removed."""
    dead = h.get('host') == socket.gethostname() and not pid_alive(h.get('pid', 0) or 0)
    old = stale is not None and time.time() - (h.get('start', 0) or 0) > stale
    if not (dead or old):
        return False
    log(f"taking over from {describe(h)}: {'process gone' if dead else 'stale'}")
    _remove(lock)
    return True


def acquire(lock, me, wait, stale=None, log=None, poll=None):
    """Take `lock` for `me` (dict: agent, host, pid, cmd), first come first served, waiting up to `wait`
    seconds. Returns True, or False after the wait (no ticket left behind)."""
    log = log or (lambda s: print(s, flush=True))
    poll = POLL if poll is None else poll
    os.makedirs(os.path.dirname(lock), exist_ok=True)
    t0 = time.time()
    limit = t0 + wait
    ticket = add_ticket(lock, me)
    tpath = os.path.join(queue_dir(lock), ticket)
    said, said_at = None, 0
    try:
        while True:
            try:
                os.utime(tpath)
            except OSError:  # deleted by hand or pruned while we slept: queue again at the end
                ticket = add_ticket(lock, me)
                tpath = os.path.join(queue_dir(lock), ticket)
            h = read_json(lock)
            if h is not None and h.get('pid') == os.getpid() and h.get('ticket') == ticket:
                return True  # handed over by the releasing holder
            q = tickets(lock)
            names = [f for f, _ in q]
            ahead = names.index(ticket) if ticket in names else len(names)
            if h is not None and _takeover(lock, h, stale, log):
                continue
            if ahead == 0 and _write_lock(lock, dict(me, start=time.time())):
                return True
            if h is not None:
                why = f'held by {describe(h)}; queue position {ahead + 1} of {len(names)}'
            elif os.path.exists(lock):
                why = None  # being written or removed: look again
            else:
                why = f'free, {ahead} waiter(s) ahead'
            if why and (why != said or time.time() - said_at >= PROGRESS):
                log(f'importer lock {why}, waiting ({time.time() - t0:.0f} s)')
                said, said_at = why, time.time()
            if time.time() > limit:
                break
            time.sleep(poll)
        # Giving up: leave the queue first, then make sure no release handed us the lock meanwhile.
        _remove(tpath)
        time.sleep(min(0.5, poll * 2))
        h = read_json(lock)
        return bool(h and h.get('pid') == os.getpid() and h.get('ticket') == ticket)
    finally:
        _remove(tpath)


def release(lock, me_pid=None, log=None):
    """Give the lock back if this process holds it: to the head of the queue when one waits (the file
    is replaced, never removed, so no old-code poller sees it free), else by removing the file."""
    me_pid = os.getpid() if me_pid is None else me_pid
    h = read_json(lock)
    if not h or h.get('pid') != me_pid:
        return False
    for name, t in tickets(lock):
        if t.get('pid') == me_pid:
            continue
        new = {k: t.get(k) for k in ('agent', 'host', 'pid', 'cmd')}
        new.update(start=time.time(), ticket=name, handed_by=h.get('agent'))
        tmp = f'{lock}.{me_pid}.tmp'
        with open(tmp, 'w', newline='\n') as f:
            json.dump(new, f)
        for _ in range(20):  # a reader with the file open blocks the replace on Windows: retry briefly
            try:
                os.replace(tmp, lock)
                return True
            except PermissionError:
                time.sleep(0.05)
        _remove(tmp)
        break  # could not hand over: fall back to a plain release
    _remove(lock)
    return True


def status(lock):
    """Lines: the holder and the queue, in order."""
    out = []
    h = read_json(lock)
    if h:
        alive = h.get('host') != socket.gethostname() or pid_alive(h.get('pid', 0) or 0)
        out.append(f"holder: {describe(h)}{'' if alive else ' [process gone]'}"
                   + (f", handed over by {h['handed_by']}" if h.get('handed_by') else ''))
    else:
        out.append('holder: none' + (' (file being written)' if os.path.exists(lock) else ''))
    q = tickets(lock, prune=False)
    for i, (name, t) in enumerate(q, 1):
        out.append(f"queue {i}: {t.get('agent')} (pid {t.get('pid')}, {t.get('cmd')}) waiting since "
                   + time.strftime('%H:%M:%S', time.localtime(t.get('since', 0))))
    if not q:
        out.append('queue: empty (old-code waiters poll without a ticket and are not listed)')
    return out


if __name__ == '__main__':
    sys.exit('use: python tools/vc6/vc6lock.py status')
