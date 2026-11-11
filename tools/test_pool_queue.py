"""Tests for pool.py's lease queue and lock scope (D232, P-VM-2):

  - at the cap a lease waits in a first-come-first-served queue (--wait S; 0 exits at once as before),
    a queued process that dies loses its place, a timeout gives up and leaves no ticket
  - leases.lock is held only to load, decide and save: never while a clone is created, a VM
    started, waited for or stopped

    python -m unittest tools.test_pool_queue      (or: python tools/test_pool_queue.py)

Hyper-V, PowerShell and SSH are faked (a folder of VM records); no VM is created, started or
stopped. Windows only (pool.py's pid checks use kernel32).
"""
import contextlib
import io
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import pool  # noqa: E402

PATCHED = ('STATE_DIR', 'STATE', 'LOCK', 'QUEUE', 'POOL_DIR', 'BASE', 'POLL', 'PROGRESS', 'pool_vms',
           'safety', 'make_clone', 'start_vm', 'stop_vm', 'wait_ready', 'reset_sync_cache', 'destroy_vm')


# ---------------------------------------------------------------- the fake Hyper-V

def _retry(fn):
    for _ in range(100):
        try:
            return fn()
        except PermissionError:          # Windows: another process has the file open
            time.sleep(0.02)
    return fn()


def _vm_path(tmp, name):
    return os.path.join(tmp, 'fake', name + '.json')


def fake_put(tmp, name, state='Off', uptime=0):
    def w():
        p = _vm_path(tmp, name)
        with open(p + f'.{os.getpid()}.tmp', 'w') as f:
            json.dump({'Name': name, 'State': state, 'Uptime': uptime,
                       'Disks': [{'Path': name, 'Type': 'Differencing', 'Parent': 'base', 'Size': 0}],
                       'Snapshots': 0, 'IPs': ['10.0.0.%d' % int(name[-2:])] if state == 'Running' else []}, f)
        os.replace(p + f'.{os.getpid()}.tmp', p)
    _retry(w)


def fake_vms(tmp):
    out = []
    for f in sorted(os.listdir(os.path.join(tmp, 'fake'))):
        if f.endswith('.json'):
            out.append(_retry(lambda: json.loads(read(os.path.join(tmp, 'fake', f)))))
    return out


def install(tmp):
    """Point pool.py at tmp and fake every Hyper-V/SSH step. Slow steps sleep $POOLTEST_SLOW
    seconds and log to tmp/violations.log when this process holds leases.lock meanwhile."""
    slow = float(os.environ.get('POOLTEST_SLOW', '0'))
    os.makedirs(os.path.join(tmp, 'fake'), exist_ok=True)
    os.makedirs(os.path.join(tmp, 'vmpool'), exist_ok=True)
    os.makedirs(os.path.join(tmp, 'state'), exist_ok=True)
    pool.STATE_DIR = os.path.join(tmp, 'state')
    pool.STATE = os.path.join(pool.STATE_DIR, 'leases.json')
    pool.LOCK = os.path.join(pool.STATE_DIR, 'leases.lock')
    pool.QUEUE = os.path.join(pool.STATE_DIR, 'queue')
    pool.POOL_DIR = os.path.join(tmp, 'vmpool')
    pool.BASE = os.path.join(tmp, 'vmpool', 'base-vc6.vhdx')
    open(pool.BASE, 'a').close()
    pool.POLL, pool.PROGRESS = 0.5, 1

    def slow_step(what):
        try:
            with open(pool.LOCK) as f:
                mine = json.load(f)['pid'] == os.getpid()
        except (OSError, ValueError, KeyError):
            mine = False
        if mine:
            with open(os.path.join(tmp, 'violations.log'), 'a') as f:
                f.write(f'{os.getpid()} held leases.lock during {what}\n')
        with open(os.path.join(tmp, 'steps.log'), 'a') as f:
            f.write(f'{os.getpid()} {what}\n')
        time.sleep(slow)

    def make_clone(name):
        slow_step('create ' + name)
        fake_put(tmp, name)

    def start_vm(name):
        slow_step('start ' + name)
        fake_put(tmp, name, 'Running')

    def stop_vm(name, hard=False):
        slow_step('stop ' + name)
        fake_put(tmp, name, 'Off')

    def wait_ready(name, timeout=600):
        slow_step('wait ' + name)
        return '10.0.0.%d' % int(name[-2:]), 0.0, 'Version 12.00.8804'

    def destroy_vm(v):
        slow_step('destroy ' + v['Name'])
        os.remove(_vm_path(tmp, v['Name']))

    pool.pool_vms = lambda: fake_vms(tmp)
    pool.safety = lambda vm: None
    pool.make_clone, pool.start_vm, pool.stop_vm = make_clone, start_vm, stop_vm
    pool.wait_ready, pool.destroy_vm = wait_ready, destroy_vm
    pool.reset_sync_cache = lambda ip: None


def run_main(args):
    """pool.main() in this process: (exit message or None, stdout)."""
    old, out = sys.argv, io.StringIO()
    sys.argv = ['pool.py'] + args
    try:
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
            pool.main()
        return None, out.getvalue()
    except SystemExit as e:
        return str(e.code), out.getvalue()
    finally:
        sys.argv = old


def lease_args(agent, *more):
    return ['lease', '--agent', agent, '--holder-pid', str(os.getpid())] + list(more)


def child(tmp, agent, *more, slow=0):
    """A real lease process (its own pid, so its own ticket) against the same fake pool."""
    env = dict(os.environ, POOLTEST_SLOW=str(slow))
    with open(os.path.join(tmp, agent + '.log'), 'w') as log:
        return subprocess.Popen([sys.executable, os.path.abspath(__file__), 'child', tmp] + lease_args(agent, *more),
                                stdout=log, stderr=subprocess.STDOUT, env=env)


def read(path):
    with open(path) as f:
        return f.read()


def wait_for(cond, secs=30, what=''):
    t = time.time() + secs
    while time.time() < t:
        if cond():
            return
        time.sleep(0.1)
    raise AssertionError('timed out waiting for ' + what)


class PoolCase(unittest.TestCase):
    def setUp(self):
        self.saved = {k: getattr(pool, k) for k in PATCHED}
        self.tmp = tempfile.mkdtemp(prefix='pooltest-')
        install(self.tmp)
        self.procs = []

    def tearDown(self):
        for p in self.procs:
            if p.poll() is None:
                p.kill()
            p.wait()
        for k, v in self.saved.items():
            setattr(pool, k, v)
        shutil.rmtree(self.tmp, ignore_errors=True)

    def state(self):
        return pool.load()

    def violations(self):
        p = os.path.join(self.tmp, 'violations.log')
        return read(p) if os.path.exists(p) else ''

    def tickets(self):
        try:
            return [f for f in os.listdir(pool.QUEUE) if f.endswith('.json')]
        except FileNotFoundError:
            return []

    def log(self, agent):
        return read(os.path.join(self.tmp, agent + '.log'))


class Lease(PoolCase):
    def test_free_vm_served_at_once_lock_free_while_starting(self):
        fake_put(self.tmp, pool.PREFIX + '01')
        err, out = run_main(lease_args('a'))
        self.assertIsNone(err)
        self.assertIn('ENB_VC6_HOSTS=', out)
        s = self.state()[pool.PREFIX + '01']
        self.assertEqual((s['agent'], s.get('pending')), ('a', None))
        self.assertTrue(s['host'].endswith('@10.0.0.1'))
        self.assertEqual(self.violations(), '')
        self.assertFalse(os.path.exists(pool.LOCK))

    def test_clone_created_without_the_lock(self):
        err, out = run_main(lease_args('a', '--max', '2'))
        self.assertIsNone(err)
        n = pool.PREFIX + '01'
        self.assertIn('create ' + n, read(os.path.join(self.tmp, 'steps.log')))
        self.assertEqual(self.state()[n]['agent'], 'a')
        self.assertNotIn('busy', self.state()[n])
        self.assertEqual(self.violations(), '')

    def test_wait_0_exits_with_the_cap_message(self):
        fake_put(self.tmp, pool.PREFIX + '01', 'Running')
        run_main(lease_args('a', '--max', '1'))
        err, _ = run_main(lease_args('b', '--max', '1', '--wait', '0'))
        self.assertTrue(err.startswith('pool cap: 1 VMs exist'), err)
        self.assertEqual(self.tickets(), [])

    def test_timeout_gives_up_and_leaves_no_ticket(self):
        fake_put(self.tmp, pool.PREFIX + '01', 'Running')
        run_main(lease_args('a', '--max', '1'))
        t = time.time()
        err, _ = run_main(lease_args('b', '--max', '1', '--wait', '1'))
        self.assertIn('no VM after', err)
        self.assertLess(time.time() - t, 10)
        self.assertEqual(self.tickets(), [])

    def test_count_above_cap_refused_at_once(self):
        err, _ = run_main(lease_args('a', '--max', '1', '--count', '2'))
        self.assertIn('can never be served', err)

    def test_pending_lease_of_a_dead_process_is_orphaned(self):
        q = subprocess.Popen([sys.executable, '-c', 'pass'])
        q.wait()
        n = pool.PREFIX + '01'
        fake_put(self.tmp, n)
        who = pool.holder(os.getpid())
        who.update(lease_pid=q.pid, lease_start=None)
        pool.save({n: dict(who, agent='x', since=pool.now(), pending=True)})
        state = pool.load()
        pool.release_orphans(state, 8)
        self.assertIsNone(pool.load()[n]['agent'])
        self.assertIn('ended before', pool.load()[n]['reclaimed'])

    def test_tidy_stops_idle_vm_without_the_lock(self):
        n = pool.PREFIX + '01'
        fake_put(self.tmp, n, 'Running', uptime=3600)
        pool.tidy(10)
        self.assertEqual(fake_vms(self.tmp)[0]['State'], 'Off')
        self.assertIn('stop ' + n, read(os.path.join(self.tmp, 'steps.log')))
        self.assertEqual(self.violations(), '')
        self.assertNotIn('busy', self.state().get(n, {}))


class LockFile(PoolCase):
    def test_exit_deletes_the_lock_while_a_reader_has_it_open(self):
        """Windows refuses deleting a file another handle has open: the lock must not stay behind."""
        import threading
        lk = pool.Lock().__enter__()
        fh = open(pool.LOCK)                       # a waiter reading the holder's pid
        threading.Timer(0.5, fh.close).start()
        lk.__exit__(None, None, None)
        self.assertFalse(os.path.exists(pool.LOCK))


class Concurrent(PoolCase):
    def test_lock_free_during_a_slow_start(self):
        """A lease whose VM takes 4 s to come up: another process takes leases.lock meanwhile."""
        n = pool.PREFIX + '01'
        fake_put(self.tmp, n)
        p = child(self.tmp, 'slow', slow=4)
        self.procs.append(p)
        wait_for(lambda: self.state().get(n, {}).get('pending'), what='the pending lease')
        t = time.time()
        with pool.Lock():
            took = time.time() - t
        self.assertLess(took, 1.5)
        self.assertIsNone(p.poll(), 'the lease finished before the lock was tried')
        self.assertEqual(p.wait(30), 0, self.log('slow'))
        self.assertEqual(self.violations(), '')

    def test_fifo_queue_and_dead_waiter_dropped(self):
        """One VM, cap 1, held by a; w1..w4 queue in order; w2 dies; releases serve w1, w3, w4."""
        n = pool.PREFIX + '01'
        fake_put(self.tmp, n, 'Running')
        self.assertIsNone(run_main(lease_args('a', '--max', '1'))[0])
        ws = {}
        for i, w in enumerate(('w1', 'w2', 'w3', 'w4'), 1):
            ws[w] = child(self.tmp, w, '--max', '1', '--wait', '120')
            self.procs.append(ws[w])
            try:
                wait_for(lambda: len(self.tickets()) == i, what=f'{w} to queue')
            except AssertionError:
                raise AssertionError(f'{w} did not queue; tickets {self.tickets()}; its log: ' + self.log(w))
        ws['w2'].kill()
        ws['w2'].wait()
        for prev, nxt, rest in (('a', 'w1', ('w3', 'w4')), ('w1', 'w3', ('w4',)), ('w3', 'w4', ())):
            time.sleep(1)                       # the queue keeps polling; nobody may jump it
            for w in ('w1', 'w3', 'w4'):
                if w == nxt or w in rest:
                    self.assertIsNone(ws[w].poll(), f'{w} served before {prev} released')
            self.assertEqual(self.state()[n]['agent'], prev)
            self.assertIsNone(run_main(['release', '--agent', prev])[0])
            self.assertEqual(ws[nxt].wait(30), 0, self.log(nxt))
            self.assertIn('ENB_VC6_HOSTS=', self.log(nxt))
            self.assertEqual(self.state()[n]['agent'], nxt)
            for w in rest:
                self.assertIsNone(ws[w].poll(), f'{w} served out of order (with {nxt})')
        self.assertIn('queue position', self.log('w4'))
        self.assertEqual(self.tickets(), [])
        self.assertEqual(self.violations(), '')


if __name__ == '__main__':
    if len(sys.argv) > 2 and sys.argv[1] == 'child':
        install(sys.argv[2])
        sys.argv = ['pool.py'] + sys.argv[3:]
        pool.main()
    else:
        unittest.main()
