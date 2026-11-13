"""Tests for the pad sweeps' --hold and --first-window (sendpad.py, auxpad.py sweep; 2026-11-12):

  --hold          the importer lock is taken once, at the first value, and held to the end of the sweep; it is
                  given back on every exit (an exception or Ctrl-C in a variant included) and the pool lease too
  --first-window  values from the current pad outward (cur, cur+1, cur-1, ...); the sweep stops at the first run
                  of 2 or more full values whose neighbours are known, and picks what the full sweep picks when
                  that run is the longest (otherwise the nearest run, by design)
  wt.py bring-over's retune passes both when the scratch tree's sendpad.py has them

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; the importer (lock, pool lease, compile) is a fake: no VM, no real lock.
"""
import argparse
import contextlib
import io
import os
import shutil
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
for d in ('vc6', 're'):
    sys.path.insert(0, os.path.join(HERE, d))
sys.path.append(HERE)

import auxpad  # noqa: E402
import sendpad  # noqa: E402
import test_padretune  # noqa: E402


class FakeImporter:
    """vc6lock's lock semantics (re-entrant hold_lock, release_lock gives it back) with counters; a pad value n is
    'full' when table[n] is true. fail_at: raise that exception at that value."""

    def __init__(self, table, fail_at=None, exc=RuntimeError):
        self.table, self.fail_at, self.exc = table, fail_at, exc
        self.held, self.takes, self.releases, self.leases, self.leased = False, 0, 0, 0, False
        self.tried, self.held_at = [], []

    def set_agent(self, name):
        pass

    def hold_lock(self, wait=None):
        if not self.held:
            self.held, self.takes = True, self.takes + 1

    def release_lock(self):
        if self.held:
            self.releases += 1
        self.held = False

    @contextlib.contextmanager
    def pool_lease(self, count, holder_pid=None):
        self.leases, self.leased = self.leases + 1, True
        try:
            yield
        finally:
            self.leased = False

    def compile_and_match(self, files, tag):
        n = int(tag.rsplit('-', 1)[1])
        self.tried.append(n)
        self.held_at.append(self.held)
        if n == self.fail_at:
            raise self.exc('variant failed')
        return ({'f': set(sendpad.WATCH) | set(auxpad.WATCH)} if self.table.get(n) else {'f': set()}), ''


def table(*ranges):
    """{n: True} for every n in the (lo, hi) ranges."""
    return {n: True for lo, hi in ranges for n in range(lo, hi + 1)}


class Base(unittest.TestCase):
    def setUp(self):
        self.td = tempfile.mkdtemp(prefix='padhold_')
        self.addCleanup(shutil.rmtree, self.td, ignore_errors=True)
        self.saved = sys.modules.get('importer')
        self.addCleanup(self.restore)
        self.h = os.path.join(self.td, 'aux_roots_send.h')
        self.cpp = os.path.join(self.td, 'aux_roots.cpp')
        for mod, name, path in ((sendpad, 'H', self.h), (auxpad, 'CPP', self.cpp)):
            self.addCleanup(setattr, mod, name, getattr(mod, name))
            setattr(mod, name, path)
        with open(self.cpp, 'wb') as f:
            f.write(b'int x;\r\n// VC6 symbol pad (not the client\'s code)\r\nenum AuxRootsSymbolPadType { };\r\nint y;\r\n')

    def restore(self):
        if self.saved is None:
            sys.modules.pop('importer', None)
        else:
            sys.modules['importer'] = self.saved

    def fake(self, tab, **kw):
        sys.modules['importer'] = imp = FakeImporter(tab, **kw)
        return imp

    def set_cur(self, cur):
        with open(self.h, 'wb') as f:
            f.write(sendpad.with_pad('int a;\nenum AuxRootsSendSymbolPadType { };\nint b;\n', cur).encode('latin-1'))
        self.orig = open(self.h, 'rb').read()

    def args(self, **kw):
        a = dict(range=(0, 63), pool=1, holder_pid=None, lock_wait=5, agent=None, hold=False, first_window=False,
                 sets=os.path.join(self.td, 'sets.json'))
        a.update(kw)
        return argparse.Namespace(**a)

    def sendpad(self, tab, cur, **kw):
        self.set_cur(cur)
        imp = self.fake(tab)
        r = sendpad.run_sweep(self.args(**kw), io.StringIO(), list(sendpad.WATCH))
        self.assertEqual(open(self.h, 'rb').read(), self.orig)     # header restored
        return r, imp


class Hold(Base):
    def test_default_takes_the_lock_per_value(self):
        _, imp = self.sendpad(table((10, 20)), 15, range=(0, 9))
        self.assertEqual((imp.takes, imp.releases, imp.held), (10, 10, False))
        self.assertEqual(imp.tried, list(range(10)))                 # ascending, unchanged

    def test_hold_takes_it_once(self):
        _, imp = self.sendpad(table((10, 20)), 15, hold=True)
        self.assertEqual((imp.takes, imp.releases, imp.held, imp.leased), (1, 1, False, False))
        self.assertTrue(all(imp.held_at))
        self.assertFalse(sendpad.HOLD)

    def test_hold_released_on_exception_and_ctrl_c(self):
        for exc in (RuntimeError, KeyboardInterrupt):
            for fw in (False, True):
                self.set_cur(15)
                imp = self.fake(table((10, 20)), fail_at=17, exc=exc)
                with self.assertRaises(exc):
                    sendpad.run_sweep(self.args(hold=True, first_window=fw), io.StringIO(), list(sendpad.WATCH))
                self.assertEqual((imp.takes, imp.releases, imp.held, imp.leased), (1, 1, False, False))
                self.assertFalse(sendpad.HOLD)
                self.assertEqual(open(self.h, 'rb').read(), self.orig)

    def test_auxpad_hold(self):
        cpp = open(self.cpp, 'rb').read()
        for hold, takes in ((False, 8), (True, 1)):
            imp = self.fake(table((2, 5)))
            a = auxpad.parser().parse_args(['sweep', '--from', '0', '--to', '7'] + (['--hold'] if hold else []))
            with contextlib.redirect_stdout(io.StringIO()) as out:
                self.assertEqual(auxpad.held_sweep(a), 0)
            self.assertIn('all six exact: 2..5', out.getvalue())
            self.assertEqual((imp.takes, imp.releases, imp.held, imp.leased), (takes, takes, False, False))
            self.assertEqual(open(self.cpp, 'rb').read(), cpp)
        imp = self.fake(table((2, 5)), fail_at=3, exc=KeyboardInterrupt)
        a = auxpad.parser().parse_args(['sweep', '--to', '7', '--hold'])
        with contextlib.redirect_stdout(io.StringIO()), self.assertRaises(KeyboardInterrupt):
            auxpad.held_sweep(a)
        self.assertEqual((imp.takes, imp.releases, imp.held, imp.leased), (1, 1, False, False))
        self.assertFalse(auxpad.HOLD)
        self.assertEqual(open(self.cpp, 'rb').read(), cpp)


class FirstWindow(Base):
    def test_outward_order(self):
        self.assertEqual(sendpad.outward(0, 63, 49)[:7], [49, 50, 48, 51, 47, 52, 46])
        self.assertEqual(sorted(sendpad.outward(0, 63, 49)), list(range(64)))
        self.assertEqual(sendpad.outward(0, 5, 0), [0, 1, 2, 3, 4, 5])
        self.assertEqual(sendpad.outward(0, 5, 5), [5, 4, 3, 2, 1, 0])
        self.assertEqual(sendpad.outward(10, 12, 99), [12, 11, 10])         # clamped
        self.assertEqual(sendpad.outward(10, 12, None), [10, 11, 12])

    def compare(self, tab, cur, equal=True):
        full, _ = self.sendpad(tab, cur)
        fast, imp = self.sendpad(tab, cur, hold=True, first_window=True)
        if equal:
            self.assertEqual(full[:2], fast[:2], (tab, cur))
        return full, fast, imp

    def test_window_around_cur_stops_early(self):
        full, fast, imp = self.compare(table((46, 53)), 49)
        self.assertEqual(fast[1], 49)
        self.assertEqual(sorted(imp.tried), list(range(45, 55)))           # 10 values, not 64
        self.assertEqual(imp.tried[0], 49)

    def test_window_beside_cur(self):
        full, fast, imp = self.compare(table((20, 24)), 30)
        self.assertEqual((fast[1], fast[2]), (22, [(20, 24)]))
        self.assertLess(len(imp.tried), 64)

    def test_window_at_the_edges(self):
        _, fast, imp = self.compare(table((0, 4)), 3)
        self.assertEqual(fast[1], 2)
        self.assertEqual(sorted(imp.tried), list(range(0, 7)))              # 0 is reached after 6; no neighbour below 0
        _, fast, imp = self.compare(table((59, 63)), 62)
        self.assertEqual(fast[1], 61)
        self.assertEqual(sorted(imp.tried), list(range(58, 64)))

    def test_two_windows_nearest_longest(self):
        _, fast, imp = self.compare(table((40, 50), (10, 12)), 45)
        self.assertEqual(fast[1], 45)
        self.assertLess(len(imp.tried), 64)

    def test_two_windows_far_one_longer(self):
        # by design the nearest settled run wins; the full sweep picks the longest
        full, fast, _ = self.compare(table((30, 33), (50, 60)), 31, equal=False)
        self.assertEqual((full[1], fast[1]), (55, 31))

    def test_single_values_sweep_everything(self):
        for tab in (table((5, 5), (30, 30), (62, 62)), table((7, 7))):
            _, fast, imp = self.compare(tab, 30)
            self.assertEqual(sorted(imp.tried), list(range(64)))

    def test_two_wide_window_settles(self):
        # P-L18-4: a 2-wide window with both neighbours failed stops the sweep (it used to try all 64 values)
        _, fast, imp = self.compare(table((46, 47)), 49)
        self.assertEqual((fast[1], fast[2]), (46, [(46, 47)]))
        self.assertEqual(sorted(imp.tried), list(range(45, 54)))           # 49, 50, 48, ... 45: 9 values
        _, fast, imp = self.compare(table((0, 1)), 30)                      # at the edge: no neighbour below 0
        self.assertEqual(fast[1], 0)
        self.assertLess(len(imp.tried), 64)
        # the nearest 2-wide run wins over a longer far one, as for 3-wide runs (the full sweep picks the longest)
        full, fast, _ = self.compare(table((5, 6), (30, 31), (62, 63)), 30, equal=False)
        self.assertEqual((full[1], fast[1]), (5, 30))

    def test_no_window(self):
        _, fast, imp = self.compare({}, 49)
        self.assertIsNone(fast[1])
        self.assertEqual(fast[2], [])
        self.assertEqual(sorted(imp.tried), list(range(64)))

    def test_range_and_no_current_pad(self):
        _, fast, imp = self.compare(table((12, 16)), 49, )
        self.assertEqual(fast[1], 14)
        full, _ = self.sendpad(table((12, 16)), 0, range=(10, 20))
        fast, imp = self.sendpad(table((12, 16)), 0, range=(10, 20), first_window=True)
        self.assertEqual((full[1], fast[1], imp.tried[0]), (14, 14, 10))


class WtPasses(test_padretune.Retune):
    def test_retune_passes_hold_and_first_window(self):
        test_padretune.commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n',
                                                      'tools/vc6/sendpad.py': '# --first-window --watch-from\n'})
        test_padretune.commit(self.td, test_padretune.RECORD, {'docs/data/imports/x.json': '{}\n'})
        self.assertTrue(self.retune())
        sweep = [c for c in self.calls if any(str(x).endswith('sendpad.py') for x in c) and '--includes' not in c]
        self.assertEqual(sweep[0][-4:], ['--hold', '--first-window', '--watch-from', self.base])

    def test_old_sendpad_gets_neither(self):
        test_padretune.commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n'})
        self.assertTrue(self.retune())
        flat = [x for c in self.calls for x in c]
        self.assertNotIn('--hold', flat)
        self.assertNotIn('--first-window', flat)


if __name__ == '__main__':
    unittest.main()
