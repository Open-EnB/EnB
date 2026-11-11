"""Tests for the 2026-11-10 review's tool changes (D223; progress.md section 4 item 9, changes 1 and 4-10):

  1  wt.py new copies build/art and build/d3d8shot into a worktree
  4  verify.py --lib a,b splits on the comma; an unknown name exits
  5  wt.py passes its --lock-wait to report.py
  6  appfile.py's markers() skips #pragma lines between a marker and its definition
  7  symbols.py with no arguments refuses
  8  explain.py takes a name that equals before names that contain
  9  sendpad.py reads its watch set from HEAD's ledger
  10 auxpad.py sweep accepts --pool

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no VM, no client.exe, no verify.
"""
import argparse
import contextlib
import io
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
for d in ('vc6', 're'):
    sys.path.insert(0, os.path.join(HERE, d))

import wt  # noqa: E402


def put(path, data=b'x'):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'wb') as f:
        f.write(data)


class Furnish(unittest.TestCase):
    """1: build/art (MSS32.DLL in it) and build/d3d8shot go into the worktree; app.lib still does not."""

    def setUp(self):
        self.td = tempfile.mkdtemp(prefix='wt_furnish_')
        self.addCleanup(shutil.rmtree, self.td, ignore_errors=True)
        self.root, self.path = os.path.join(self.td, 'main'), os.path.join(self.td, 'wt')
        os.makedirs(self.path)
        old = wt.ROOT
        wt.ROOT = self.root
        self.addCleanup(setattr, wt, 'ROOT', old)

    def test_copies_art_and_d3d8shot(self):
        b = os.path.join(self.root, 'build')
        put(os.path.join(b, 'art', 'MSS32.DLL'), b'mss')
        put(os.path.join(b, 'art', 'sub', 'A.DDS'), b'dds')
        put(os.path.join(b, 'd3d8shot', 'd3d8.dll'), b'proxy')
        put(os.path.join(b, 'wwlib', 'wwlib.lib'), b'lib')
        put(os.path.join(b, 'app', 'app.lib'), b'app')
        put(os.path.join(b, 'programs', 'vidtest', 'x.exe'))
        wt.furnish(self.path, pseudo=False)
        w = os.path.join(self.path, 'build')
        for rel, data in (('art/MSS32.DLL', b'mss'), ('art/sub/A.DDS', b'dds'), ('d3d8shot/d3d8.dll', b'proxy'),
                          ('wwlib/wwlib.lib', b'lib')):
            with open(os.path.join(w, *rel.split('/')), 'rb') as f:
                self.assertEqual(f.read(), data, rel)
        self.assertFalse(os.path.exists(os.path.join(w, 'app')))
        self.assertFalse(os.path.exists(os.path.join(w, 'programs')))

    def test_main_tree_without_them(self):
        put(os.path.join(self.root, 'build', 'wwlib', 'wwlib.lib'))
        wt.furnish(self.path, pseudo=False)
        self.assertEqual(sorted(os.listdir(os.path.join(self.path, 'build'))), ['wwlib'])

    def test_no_libs_copies_nothing(self):
        put(os.path.join(self.root, 'build', 'art', 'MSS32.DLL'))
        wt.furnish(self.path, libs=False, pseudo=False)
        self.assertFalse(os.path.exists(os.path.join(self.path, 'build')))


class VerifyLib(unittest.TestCase):
    """4: `--lib a,b` used to select nothing (verify's `l in state` filter) and exit 0."""

    @classmethod
    def setUpClass(cls):
        import verify
        cls.names = staticmethod(verify.lib_names)

    KNOWN = {'app': 1, 'wwlib': 2, 'wlib': 3}

    def test_forms(self):
        self.assertIsNone(self.names(None, self.KNOWN))
        self.assertEqual(self.names(['app'], self.KNOWN), ['app'])
        self.assertEqual(self.names(['app,wwlib'], self.KNOWN), ['app', 'wwlib'])
        self.assertEqual(self.names(['app, wwlib', 'wlib', 'app'], self.KNOWN), ['app', 'wwlib', 'wlib'])

    def test_unknown_exits(self):
        for bad in (['appp'], ['app,nope'], [','], ['']):
            with self.subTest(bad=bad), self.assertRaises(SystemExit) as e:
                self.names(bad, self.KNOWN)
            self.assertIn('verify:', str(e.exception))


class ReportLockWait(unittest.TestCase):
    """5: bring-over's and rebase's report.py step waits as long as their verify does."""

    def setUp(self):
        self.bo = tempfile.mkdtemp(prefix='wt_report_')
        self.addCleanup(shutil.rmtree, self.bo, ignore_errors=True)

    def test_passes_lock_wait(self):
        put(os.path.join(self.bo, 'tools', 'vc6', 'report.py'))
        self.assertEqual(wt.report_args(argparse.Namespace(lock_wait=3000), self.bo),
                         ['tools/vc6/report.py', '--lock-wait', '3000'])
        self.assertEqual(wt.report_args(argparse.Namespace(), self.bo), ['tools/vc6/report.py'])

    def test_presplit_tree_unchanged(self):
        self.assertEqual(wt.report_args(argparse.Namespace(lock_wait=3000), self.bo), [wt.IMPORTER, 'report'])

    def test_report_py_takes_the_flag(self):
        r = subprocess.run([sys.executable, os.path.join(HERE, 'vc6', 'report.py'), '--lock-wait', '5', '--help'],
                           capture_output=True, text=True)
        self.assertEqual(r.returncode, 0, r.stderr)


class Markers(unittest.TestCase):
    """6: `#pragma auto_inline(off)` under a marker read as a definition named auto_inline."""

    def markers(self, text):
        import appfile
        with tempfile.NamedTemporaryFile('w', suffix='.cpp', delete=False, encoding='latin1') as f:
            f.write(text)
        self.addCleanup(os.remove, f.name)
        return appfile.markers(f.name)

    def test_pragma_between_marker_and_definition(self):
        got, n = self.markers('// @0x401000\n#pragma auto_inline(off)\nint Foo::Bar(int a)\n{\n}\n'
                              '#pragma auto_inline(on)\n\n// @0x401020\n  # pragma optimize("", off)\nvoid Baz(void)\n{\n}\n')
        self.assertEqual(got, {'Foo::Bar': [0x401000], 'Baz': [0x401020]})
        self.assertEqual(n, 2)

    def test_plain_marker(self):
        got, n = self.markers('// @0x401000\n// a comment\n\nFoo::~Foo(void)\n{\n}\n')
        self.assertEqual(got, {'Foo::~Foo': [0x401000]})


class SymbolsRefuses(unittest.TestCase):
    """7: plain `symbols.py` regenerated symbols.json without any object's names."""

    def run_tool(self, *args):
        return subprocess.run([sys.executable, os.path.join(HERE, 're', 'symbols.py'), *args],
                              capture_output=True, text=True)

    def test_no_arguments(self):
        with tempfile.TemporaryDirectory() as out:
            for args in ((), ('--out', out)):
                r = self.run_tool(*args)
                self.assertEqual(r.returncode, 2, r.stdout + r.stderr)
                self.assertIn('--objs', r.stderr)
            self.assertEqual(os.listdir(out), [])

    def test_no_objs_goes_alone(self):
        r = self.run_tool('--no-objs', '--apply-named')
        self.assertEqual(r.returncode, 2)
        self.assertIn('--no-objs goes alone', r.stderr)


class ExplainPick(unittest.TestCase):
    """8: 'CordCon::pump' also selected pumpClose (and --n 0 took whichever the object defined first)."""

    FUNCS = [('?pumpClose@CordCon@@QAEXXZ', 1, 0, 8), ('?pump@CordCon@@QAEXXZ', 1, 8, 16),
             ('?pump@Other@@QAEXXZ', 1, 16, 24), ('??0CordCon@@QAE@XZ', 1, 24, 32), ('_plain_c', 1, 32, 40)]

    def names(self, symbol):
        import explain
        return [f[0] for f in explain.pick(self.FUNCS, symbol)]

    def test_equal_before_containing(self):
        self.assertEqual(self.names('CordCon::pump'), ['?pump@CordCon@@QAEXXZ'])
        self.assertEqual(self.names('?pump@CordCon@@QAEXXZ'), ['?pump@CordCon@@QAEXXZ'])
        self.assertEqual(self.names('pump'), ['?pump@CordCon@@QAEXXZ', '?pump@Other@@QAEXXZ'])
        self.assertEqual(self.names('pumpClose'), ['?pumpClose@CordCon@@QAEXXZ'])
        self.assertEqual(self.names('CordCon::CordCon'), ['??0CordCon@@QAE@XZ'])
        self.assertEqual(self.names('plain_c'), ['_plain_c'])

    def test_containing_when_none_equals(self):
        self.assertEqual(self.names('?pump@CordCon'), ['?pump@CordCon@@QAEXXZ'])
        self.assertEqual(self.names('CordCon::pu'), ['?pumpClose@CordCon@@QAEXXZ', '?pump@CordCon@@QAEXXZ'])
        self.assertEqual(self.names('nothing'), [])

    def test_n_out_of_range_lists_candidates(self):
        import explain

        class Obj:
            def __init__(self, path):
                pass

            def functions(self):
                return ExplainPick.FUNCS
        old = explain.CoffObject
        explain.CoffObject = Obj
        self.addCleanup(setattr, explain, 'CoffObject', old)
        with self.assertRaises(SystemExit) as e:
            explain.explain('x.obj', 'pump', '0x401000', n=2)
        self.assertIn('2 candidate(s)', str(e.exception))
        self.assertIn('?pump@Other@@QAEXXZ', str(e.exception))


class SendpadWatch(unittest.TestCase):
    """9: after the verify that lost sclient's pairs the working ledger no longer holds them: the sweep then watched
    only what was left and called any pad a window."""

    def test_reads_head(self):
        import ledger
        import sendpad
        asked = []

        def load(src=None, repo=None):
            asked.append(src)
            held = sendpad.WATCH[:3] if src == 'HEAD' else sendpad.WATCH[:1]
            return {'app': {'files': {'sclient': {'in_tree': held}}}}
        old = ledger.load
        ledger.load = load
        self.addCleanup(setattr, ledger, 'load', old)
        self.assertEqual(sendpad.watch_list(), sendpad.WATCH[:3])
        self.assertEqual(asked, ['HEAD'])

    def test_no_entry_watches_all(self):
        import ledger
        import sendpad
        old = ledger.load
        ledger.load = lambda src=None, repo=None: {'app': {'files': {}}}
        self.addCleanup(setattr, ledger, 'load', old)
        self.assertEqual(sendpad.watch_list(), sendpad.WATCH)


class AuxpadPool(unittest.TestCase):
    """10: the docstring listed `--pool`; the parser rejected it."""

    def test_pool_accepted(self):
        import auxpad
        p = auxpad.parser()
        self.assertEqual(p.parse_args(['sweep', '--pool']).pool, 1)
        self.assertEqual(p.parse_args(['sweep', '--pool', '1', '--from', '3']).lo, 3)
        self.assertIsNone(p.parse_args(['sweep']).pool)
        self.assertEqual(p.parse_args(['sweep', '--host', '10.0.0.1']).host, '10.0.0.1')
        for bad in (['sweep', '--pool', '2'], ['sweep', '--pool', '--host', '10.0.0.1']):
            with self.subTest(bad=bad), contextlib.redirect_stderr(io.StringIO()), self.assertRaises(SystemExit):
                p.parse_args(bad)


if __name__ == '__main__':
    unittest.main()
