"""Tests for tools/vc6/linkgate.py (pipeline-plan §4 item 6b) and wt.py's link gate: the summaries' judgement
against the baseline, with the build and link calls stubbed (no VMs).

    python -m unittest discover -s tools -p "test_*.py"
"""
import argparse
import io
import json
import os
import shutil
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import linkgate  # noqa: E402
import wt  # noqa: E402

KNOWN = ['?Unknown_82ECB0@@YA?AVMatrix3D@@ABVVector3@@0@Z', '?Handle_Input@MovieClientClass@@QAEXUKernelEventClass@@@Z']


def summary(unresolved=(), exe=True, link_errors=(), cl_errors=()):
    return {'exe': exe, 'unresolved': {s: ['x.obj'] for s in unresolved}, 'link_errors': list(link_errors),
            'cl_errors': list(cl_errors)}


class Judge(unittest.TestCase):
    base = {'clienttest': KNOWN, 'udpreplay': []}

    def test_known_baseline_passes(self):
        problems, notes = linkgate.judge({'client': summary(), 'clienttest': summary(KNOWN),
                                          'udpreplay': summary()}, self.base)
        self.assertEqual(problems, [])
        self.assertIn('client: linked, no unresolved symbol, no stand-in', notes)
        self.assertIn('clienttest: linked, 2 unresolved (all in the baseline)', notes)

    def test_new_unresolved_refuses(self):
        problems, _ = linkgate.judge({'client': summary(), 'clienttest': summary(KNOWN + ['?New@@YAXXZ']),
                                      'udpreplay': summary()}, self.base)
        self.assertEqual(len(problems), 1)
        self.assertIn('?New@@YAXXZ', problems[0])

    def test_client_any_unresolved_refuses_baseline_or_not(self):
        base = dict(self.base, client=KNOWN)          # a baseline entry for the client is never read
        problems, _ = linkgate.judge({'client': summary(KNOWN[:1]), 'clienttest': summary(KNOWN),
                                      'udpreplay': summary()}, base)
        self.assertEqual(len(problems), 1)
        self.assertIn('client: 1 unresolved symbol(s) (the client links with none)', problems[0])

    def test_client_standin_object_refuses(self):
        s = summary()
        s['stub_objects'] = ['app_stubs.cpp']
        s['stub_objects_linked'] = ['app_stubs.obj']
        problems, notes = linkgate.judge({'client': s, 'clienttest': summary(KNOWN), 'udpreplay': summary()},
                                         self.base)
        self.assertEqual(len(problems), 1)
        self.assertIn('client: 2 stand-in object(s) in the link', problems[0])
        self.assertFalse(any(n.startswith('client:') for n in notes))
        t = summary(KNOWN)
        t['stub_objects_linked'] = ['app_stubs.obj']     # a test program keeps its stand-ins
        self.assertEqual(linkgate.judge({'client': summary(), 'clienttest': t, 'udpreplay': summary()},
                                        self.base)[0], [])

    def test_baseline_never_holds_the_client(self):
        tree = tempfile.mkdtemp(prefix='linkgate_base_')
        try:
            os.makedirs(os.path.join(tree, 'tools', 'vc6'))
            linkgate.write_baseline(tree, {'client': summary(['?X@@']), 'clienttest': summary(KNOWN),
                                           'udpreplay': summary()})
            self.assertEqual(linkgate.read_baseline(tree), {'clienttest': sorted(KNOWN), 'udpreplay': []})
        finally:
            shutil.rmtree(tree, ignore_errors=True)

    def test_lnk2005_refuses(self):
        dup = ('client_stubs.obj : error LNK2005: "public: void __thiscall SClient::Unknown_64(void)" '
               '(?Unknown_64@SClient@@QAEXXZ) already defined in app_sclient.obj')
        problems, _ = linkgate.judge({'client': summary(),
                                      'clienttest': summary(exe=False, link_errors=[dup, 'fatal error LNK1169: one or '
                                                                                     'more multiply defined symbols found']),
                                      'udpreplay': summary()},
                                     self.base)
        self.assertEqual(len(problems), 1)
        self.assertIn('clienttest: duplicate definitions (2 line(s))', problems[0])

    def test_fewer_unresolved_is_a_note_and_missing_summary_refuses(self):
        problems, notes = linkgate.judge({'client': None, 'clienttest': summary(KNOWN[:1]),
                                          'udpreplay': summary()}, self.base)
        self.assertEqual(len(problems), 1)
        self.assertIn('client: no link summary', problems[0])
        self.assertTrue(any('1 baseline symbol(s) now resolved' in n for n in notes))

    def test_compile_error_and_no_exe_refuse(self):
        problems, _ = linkgate.judge({'client': summary(cl_errors=['x.cpp(3) : error C2084: ...']),
                                      'clienttest': summary(exe=False, link_errors=['fatal error LNK1104: x']),
                                      'udpreplay': summary()},
                                     self.base)
        self.assertEqual(len(problems), 2)
        self.assertIn('compile error', problems[0])
        self.assertIn('clienttest: no exe written', problems[1])

    def test_touches(self):
        self.assertTrue(linkgate.touches(['docs/x.md', 'app/client/a.cpp']))
        self.assertTrue(linkgate.touches(['tests/clienttest/client_stubs.cpp']))
        self.assertTrue(linkgate.touches(['tests/udpreplay/x.cpp']))
        self.assertTrue(linkgate.touches(['programs/client/x.cpp']))
        self.assertFalse(linkgate.touches(['docs/x.md', 'tools/vc6/wt.py', 'tests/kerneltest/x.cpp']))

    def test_udpreplay_refuses(self):
        problems, _ = linkgate.judge({'client': summary(), 'clienttest': summary(KNOWN),
                                      'udpreplay': summary(['?Gone@CordCon@@QAEXXZ'], exe=False)}, self.base)
        self.assertEqual(len(problems), 1)
        self.assertIn('udpreplay: 1 unresolved symbol(s)', problems[0])


class Gate(unittest.TestCase):
    """gate() in a scratch tree with the build and the links stubbed: the runner writes the summaries."""

    def setUp(self):
        self.tree = tempfile.mkdtemp(prefix='linkgate_test_')
        os.makedirs(os.path.join(self.tree, 'tools', 'vc6'))
        with open(os.path.join(self.tree, *linkgate.BASELINE.split('/')), 'w') as f:
            json.dump({'clienttest': KNOWN, 'udpreplay': []}, f)
        self.calls = []

    def tearDown(self):
        shutil.rmtree(self.tree, ignore_errors=True)

    def runner(self, results):
        def go(args, cwd, logfile):
            tag = os.path.basename(logfile).split('.')[-2]
            self.calls.append((tag, args[1:]))
            if tag in results:
                d = os.path.dirname(linkgate.summary_path(self.tree, tag))
                os.makedirs(d, exist_ok=True)
                with open(linkgate.summary_path(self.tree, tag), 'w') as f:
                    json.dump(results[tag], f)
            return 0
        return go

    def test_gate_passes_and_refuses(self):
        with redirect_stdout(io.StringIO()):
            problems, _ = linkgate.gate(self.tree, ['ww3d2'], 0, None,
                                        self.runner({'client': summary(), 'clienttest': summary(KNOWN),
                                                     'udpreplay': summary()}))
        self.assertEqual(problems, [])
        self.assertEqual([c[0] for c in self.calls], ['build', 'client', 'clienttest', 'udpreplay'])
        self.assertEqual(self.calls[3][1], ['tools/harness/udpreplay.py', 'build'])
        self.assertIn('--no-deploy', self.calls[1][1])
        self.assertEqual(self.calls[1][1][:4], ['tools/vc6/build.py', 'link', '--program', 'client'])
        self.assertEqual(self.calls[0][1][-4:], ['--lib', 'app', '--lib', 'ww3d2'])
        # a stale summary from the previous run never counts: this run writes none for clienttest or udpreplay
        with redirect_stdout(io.StringIO()):
            problems, _ = linkgate.gate(self.tree, [], 0, None, self.runner({'client': summary(['?X@@'])}))
        self.assertEqual(len(problems), 3)

    def test_wt_link_gate(self):
        a = argparse.Namespace(no_link=False, pool=0)
        old_git, old_gate = wt.git, linkgate.gate
        seen = []
        try:
            wt.git = lambda *args, **kw: 'docs/a.md\ntools/vc6/wt.py\n'
            linkgate.gate = lambda *x, **k: seen.append(x) or ([], {})
            buf = io.StringIO()
            with redirect_stdout(buf):
                wt.link_gate(a, self.tree, 'A', 'B', 'run/x')
                a.no_link = True
                wt.link_gate(a, self.tree, 'A', 'B', 'run/x')
            self.assertIn('skipped, no path under', buf.getvalue())
            self.assertIn('WARNING: --no-link', buf.getvalue())
            self.assertEqual(seen, [])
            a.no_link = False
            wt.git = lambda *args, **kw: 'app/client/a.cpp\n' if args[0] == 'diff' else ''
            linkgate.gate = lambda *x, **k: (['clienttest: duplicate definitions (1 line(s)): LNK2005'], {})
            with self.assertRaises(SystemExit) as cm, redirect_stdout(io.StringIO()):
                wt.link_gate(a, self.tree, 'A', 'B', 'run/x')
            self.assertIn('LNK2005', str(cm.exception))
            self.assertIn('run/x untouched', str(cm.exception))
        finally:
            wt.git, linkgate.gate = old_git, old_gate


if __name__ == '__main__':
    unittest.main()
