"""Tests for the first-come importer lock (D238, tools/vc6/lockq.py):

  - three waiters are served in arrival order (the holder hands the lock to the queue's head)
  - a dead waiter's ticket is skipped and deleted, so the queue cannot deadlock
  - a timeout gives up and leaves no ticket
  - old-style holders and waiters (the pre-D238 code: O_EXCL file, poll every 2 s, no ticket) still
    exclude and are excluded; a dead old-style holder is taken over
  - `vc6lock.py status` lists the holder and the queue

    python -m unittest tools.test_importer_lock      (or: python tools/test_importer_lock.py)

Real processes on a temporary lock file; never the project's .work/locks.
"""
import json
import os
import shutil
import subprocess
import sys
import tempfile
import textwrap
import time
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import lockq  # noqa: E402

# A new-code taker: acquire, log enter/leave, hold HOLD seconds, release.
NEW = textwrap.dedent('''
    import json, os, socket, sys, time
    sys.path.insert(0, {vc6!r})
    import lockq
    lock, name, wait, hold, log = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4]), sys.argv[5]
    me = {{'agent': name, 'host': socket.gethostname(), 'pid': os.getpid(), 'cmd': 'test'}}
    if not lockq.acquire(lock, me, wait, poll=0.05, log=lambda s: None):
        open(log, 'a').write(f'{{name}} timeout\\n'); sys.exit(3)
    open(log, 'a').write(f'{{name}} enter {{time.time()}}\\n')
    time.sleep(hold)
    open(log, 'a').write(f'{{name}} leave {{time.time()}}\\n')
    lockq.release(lock)
''')

# The pre-D238 vc6lock.hold_lock loop and _release_lock, as the old worktrees run them.
OLD = textwrap.dedent('''
    import json, os, socket, sys, time
    lock, name, wait, hold, log = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4]), sys.argv[5]
    me = {'agent': name, 'host': socket.gethostname(), 'pid': os.getpid(), 'cmd': 'old', 'start': time.time()}
    limit = time.time() + wait
    while True:
        try:
            fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            os.write(fd, json.dumps(me).encode()); os.close(fd); break
        except FileExistsError:
            pass
        if time.time() > limit:
            open(log, 'a').write(f'{name} timeout\\n'); sys.exit(3)
        time.sleep(2)
    open(log, 'a').write(f'{name} enter {time.time()}\\n')
    time.sleep(hold)
    open(log, 'a').write(f'{name} leave {time.time()}\\n')
    if json.load(open(lock)).get('pid') == os.getpid():
        os.remove(lock)
''')


class LockQueue(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.mkdtemp(prefix='lockq-')
        self.addCleanup(shutil.rmtree, self.tmp, True)
        self.lock = os.path.join(self.tmp, 'locks', 'importer.lock')
        os.makedirs(os.path.dirname(self.lock))
        self.log = os.path.join(self.tmp, 'log.txt')
        for n, src in (('new.py', NEW.format(vc6=os.path.join(HERE, 'vc6'))), ('old.py', OLD)):
            with open(os.path.join(self.tmp, n), 'w') as f:
                f.write(src)
        self.procs = []
        self.addCleanup(self.kill_all)

    def kill_all(self):
        for p in self.procs:
            if p.poll() is None:
                p.kill()
                p.wait()

    def spawn(self, kind, name, wait=30, hold=0.3):
        p = subprocess.Popen([sys.executable, os.path.join(self.tmp, kind + '.py'), self.lock, name,
                              str(wait), str(hold), self.log])
        self.procs.append(p)
        return p

    def queued(self, n, timeout=15):
        """Wait until n live tickets are in the queue."""
        end = time.time() + timeout
        while time.time() < end:
            if len(lockq.tickets(self.lock, prune=False)) >= n:
                return
            time.sleep(0.05)
        self.fail(f'{n} tickets never queued')

    def hold_here(self):
        me = {'agent': 'test-holder', 'host': __import__('socket').gethostname(), 'pid': os.getpid(), 'cmd': 'test'}
        self.assertTrue(lockq.acquire(self.lock, me, 1, poll=0.05, log=lambda s: None))

    def wait_all(self, timeout=60):
        for p in self.procs:
            p.wait(timeout)

    def events(self):
        with open(self.log) as f:
            return [l.split() for l in f.read().splitlines()]

    def assert_exclusive(self, ev):
        spans, open_ = [], {}
        for e in ev:
            if e[1] == 'enter':
                open_[e[0]] = float(e[2])
            elif e[1] == 'leave':
                spans.append((open_.pop(e[0]), float(e[2]), e[0]))
        spans.sort()
        for (a0, a1, an), (b0, b1, bn) in zip(spans, spans[1:]):
            self.assertLessEqual(a1, b0, f'{an} and {bn} held the lock together')

    def test_three_waiters_in_arrival_order(self):
        self.hold_here()
        for i, n in enumerate(('W1', 'W2', 'W3'), 1):
            self.spawn('new', n)
            self.queued(i)
        self.assertEqual([t['agent'] for _, t in lockq.tickets(self.lock)], ['W1', 'W2', 'W3'])
        lockq.release(self.lock)
        self.wait_all()
        ev = self.events()
        self.assertEqual([e[0] for e in ev if e[1] == 'enter'], ['W1', 'W2', 'W3'])
        self.assert_exclusive(ev)
        self.assertFalse(os.path.exists(self.lock))
        self.assertEqual(lockq.tickets(self.lock), [])

    def test_dead_waiter_skipped(self):
        self.hold_here()
        dead = self.spawn('new', 'D')
        self.queued(1)
        self.spawn('new', 'W2')
        self.queued(2)
        dead.kill()
        dead.wait()
        self.assertEqual([t['agent'] for _, t in lockq.tickets(self.lock)], ['W2'])  # pruned
        lockq.release(self.lock)
        self.procs[1].wait(30)
        self.assertEqual([e[0] for e in self.events() if e[1] == 'enter'], ['W2'])
        self.assertEqual(os.listdir(lockq.queue_dir(self.lock)), [])

    def test_timeout_leaves_no_ticket(self):
        self.hold_here()
        p = self.spawn('new', 'T', wait=1)
        self.assertEqual(p.wait(30), 3)
        self.assertEqual(self.events(), [['T', 'timeout']])
        self.assertEqual(os.listdir(lockq.queue_dir(self.lock)), [])
        self.assertEqual(lockq.read_json(self.lock)['pid'], os.getpid())  # still ours
        lockq.release(self.lock)

    def test_old_holder_then_new_waiter(self):
        old = self.spawn('old', 'OLD', hold=1.5)
        end = time.time() + 15
        while not os.path.exists(self.log) and time.time() < end:
            time.sleep(0.05)
        self.spawn('new', 'N1')
        self.queued(1)
        self.wait_all()
        ev = self.events()
        self.assertEqual([e[0] for e in ev if e[1] == 'enter'], ['OLD', 'N1'])
        self.assert_exclusive(ev)
        self.assertEqual(old.returncode, 0)

    def test_new_release_hands_over_past_old_waiter(self):
        """An old-style waiter polling a new-code holder's lock never sees it free while a ticket waits."""
        self.hold_here()
        self.spawn('old', 'OLD', wait=60)
        time.sleep(0.5)                       # the old waiter is polling
        self.spawn('new', 'N1', hold=2.5)     # long enough that OLD polls during N1's hold
        self.queued(1)
        lockq.release(self.lock)
        self.wait_all()
        ev = self.events()
        self.assertEqual([e[0] for e in ev if e[1] == 'enter'], ['N1', 'OLD'])
        self.assert_exclusive(ev)

    def test_mixed_crowd_exclusive(self):
        self.hold_here()
        for n in ('A', 'B', 'C'):
            self.spawn('new', n, hold=0.2)
        for n in ('X', 'Y'):
            self.spawn('old', n, hold=0.2, wait=60)
        self.queued(3)
        lockq.release(self.lock)
        self.wait_all(120)
        ev = self.events()
        self.assertEqual(sorted(e[0] for e in ev if e[1] == 'enter'), ['A', 'B', 'C', 'X', 'Y'])
        self.assert_exclusive(ev)

    def test_dead_old_holder_taken_over(self):
        p = subprocess.Popen([sys.executable, '-c', 'pass'])
        p.wait()
        with open(self.lock, 'w') as f:
            json.dump({'agent': 'gone', 'host': __import__('socket').gethostname(), 'pid': p.pid,
                       'cmd': 'old', 'start': time.time()}, f)
        self.spawn('new', 'N1').wait(30)
        self.assertEqual([e[0] for e in self.events() if e[1] == 'enter'], ['N1'])

    def test_status_lists_holder_and_queue(self):
        self.hold_here()
        self.spawn('new', 'Q1')
        self.queued(1)
        out = lockq.status(self.lock)
        self.assertIn('test-holder', out[0])
        self.assertTrue(out[1].startswith('queue 1: Q1'))
        lockq.release(self.lock)
        self.wait_all()


if __name__ == '__main__':
    unittest.main()
