"""Tests for tools/re/matchaudit.py (D240's match audit) and the sweep sets sendpad.py / auxpad.py save.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no VM, no git history walk (the history test builds a small repository).
"""
import json
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 're'))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import matchaudit as ma  # noqa: E402
import sendpad  # noqa: E402


class PadTest(unittest.TestCase):
    def test_pad_rows_label_pairs_not_exact_at_every_value(self):
        sets = {'values': {'0': ['0x10', '0x20'], '1': ['0x10'], '2': ['0x10'], '3': ['0x10', '0x20']}}
        ledger = {('app', 'sclient', '0x10'), ('app', 'sclient', '0x20'), ('app', 'other', '0x30')}
        rows = ma.pad_rows(sets, 'app', 'sclient', ledger, 'SEND_SYMBOL_PAD')
        self.assertEqual([r['address'] for r in rows], ['0x20'])
        self.assertIn('exact at 2/4 values (0..3); not at 1..2', rows[0]['evidence'])

    def test_never_exact_is_no_flip(self):
        sets = {'values': {'0': ['0x10'], '1': ['0x10']}}
        rows = ma.pad_rows(sets, 'app', 'sclient', {('app', 'sclient', '0x30')}, 'P')
        self.assertEqual(rows, [])

    def test_sets_from_log_judge_the_watched_addresses_only(self):
        log = ("watch set from HEAD's ledger: 0x74D990 0x758210\n"
               '  0  1/2  0x74D990- 0x758210+  total 462\n'
               '  1  2/2  0x74D990+ 0x758210+  total 463\n'
               'sendpad: window 1; was 44; dry run, pad stays 44\n')
        sets = ma.sets_from_log(log, 'log x')
        self.assertEqual(sets['values'], {'0': ['0x758210'], '1': ['0x74D990', '0x758210']})
        ledger = {('app', 'sclient', '0x74D990'), ('app', 'sclient', '0x758210'), ('app', 'sclient', '0x111111')}
        rows = ma.pad_rows(sets, 'app', 'sclient', ledger, 'SEND_SYMBOL_PAD')
        self.assertEqual([r['address'] for r in rows], ['0x74D990'])
        self.assertTrue(rows[0]['evidence'].endswith('[log x]'))

    def test_compress(self):
        self.assertEqual(ma.compress([0, 1, 2, 5, 7, 8]), '0..2, 5, 7..8')

    def test_save_sets_writes_every_value(self):
        td = tempfile.mkdtemp(prefix='padsets_')
        self.addCleanup(shutil.rmtree, td, True)
        p = os.path.join(td, 'x', 'sets.json')
        sendpad.save_sets(p, 'app/client/sclient.cpp', {1: {'0xB', '0xA'}, 0: set()}, {0: ['e'], 1: []}, HERE)
        d = json.load(open(p))
        self.assertEqual(d['values'], {'0': [], '1': ['0xA', '0xB']})
        self.assertEqual(d['errors'], {'0': ['e']})
        self.assertEqual(d['file'], 'app/client/sclient.cpp')


class DeviceTest(unittest.TestCase):
    def test_scan_finds_each_device(self):
        text = '\n'.join([
            '#define __SIGNAL_MEMBERS',
            'class LocalView { int x; };',
            'void f() { s.size(); s.size();\t// dead reads (D130; not the client\'s)',
            '}',
            'int g = 0;\t// not the client\'s: a dead assign',
            '// VC6 symbol pad (not the client\'s code)',
            'inline int Get() { return(Other()); }\t// interim here (D137)',
            'void h();\t// 0x81D130, Unknown_81D130: addresses',
        ])
        devs = ma.scan_devices(text, {'LocalView': ['app/x.h', 'app/me.cpp']}, 'app/me.cpp')
        kinds = [(k, n) for k, n, _ in devs]
        self.assertEqual(kinds, [('guard', 1), ('class-view', 2), ('d130', 3), ('not-client', 5),
                                 ('interim-forwarder', 7)])

    def test_class_defs_skip_declarations_and_comments(self):
        text = 'class A;\n// class B { };\nstruct C : public D\n{\n};\ntemplate<class T> class E { };\n'
        self.assertEqual(sorted(ma.class_defs(text)), ['C', 'E'])

    def test_workaround_rows_cover_every_pair_of_a_device_file(self):
        state = {'app': {'files': {'a': {'name': 'client/a.cpp', 'in_tree': ['0x1', '0x2']},
                                   'b': {'name': 'client/b.cpp', 'in_tree': ['0x3']}}}}
        texts = {'app/client/a.cpp': 'x.size();\t// D130', 'app/client/b.cpp': 'int y;'}
        orig = ma.source_path
        ma.source_path = lambda lib, e: 'app/' + e['name']
        self.addCleanup(setattr, ma, 'source_path', orig)
        rows, files = ma.workaround_rows(state, {}, texts.__getitem__)
        self.assertEqual([r['address'] for r in rows], ['0x1', '0x2'])
        self.assertTrue(rows[0]['evidence'].startswith('unconfirmed: d130 x1'))
        self.assertEqual(files[0]['pairs'], 2)

    def test_exact_from_match(self):
        out = ('  EXACT    Foo::Bar                                          len   40 0x5B6060\n'
               '  exact    Foo::Baz                                          len   12 (no marker) 0x5B6100\n'
               '  none     Foo::Qux                                          len   12 0x5B6200 similarity 0.90\n')
        self.assertEqual(ma.exact_from_match(out), {'0x5B6060', '0x5B6100'})


class HistoryTest(unittest.TestCase):
    P1, P2 = ('app', 'a', '0x1'), ('app', 'a', '0x2')

    def test_flips_count_lost_and_regained(self):
        f = frozenset
        changes = [('r1', [(('app', 'a'), f(), f({self.P1, self.P2}))]),
                   ('r2', [(('app', 'a'), f({self.P1, self.P2}), f({self.P2}))]),
                   ('r3', [(('app', 'a'), f({self.P2}), f({self.P1, self.P2}))]),
                   ('r4', [(('app', 'a'), f({self.P2}), f({self.P1, self.P2}))])]   # a merge re-applying r3
        t = ma.flips(changes)
        self.assertEqual(t[self.P1], [('r1', '+'), ('r2', '-'), ('r3', '+')])
        rows = ma.unstable_rows(t, {self.P1, self.P2}, short=lambda r: r)
        self.assertEqual([(r['address'], r['flips']) for r in rows], [('0x1', 2)])

    def test_invalid_files_are_no_loss(self):
        f = frozenset
        changes = [('r1', [(None, f(), f({self.P1}))]),
                   ('r2', [(None, f({self.P1}), None)]),          # broken JSON: skipped
                   ('r3', [(None, None, f({self.P1}))]),          # repaired: before = the running state
                   ('r4', [(None, f({self.P1}), f()), (('app', 'a'), f(), f({self.P1}))])]   # the split
        t = ma.flips(changes)
        self.assertEqual(t[self.P1], [('r1', '+')])

    def test_history_on_a_small_repository(self):
        td = tempfile.mkdtemp(prefix='mahist_')
        self.addCleanup(shutil.rmtree, td, True)

        def git(*a):
            subprocess.run(['git', '-C', td] + list(a), check=True, capture_output=True)

        def commit(addrs, msg):
            d = os.path.join(td, 'docs', 'data', 'imports', 'app')
            os.makedirs(d, exist_ok=True)
            with open(os.path.join(d, 'a.json'), 'w') as fh:
                json.dump({'name': 'a.cpp', 'in_tree': addrs}, fh)
            git('add', '-A')
            git('-c', 'user.name=t', '-c', 'user.email=t@t', 'commit', '-q', '-m', msg)

        git('init', '-q')
        commit(['0x1', '0x2'], 'one')
        commit(['0x2'], 'two')
        commit(['0x1', '0x2'], 'three')
        orig = ma.ROOT
        ma.ROOT = td
        self.addCleanup(setattr, ma, 'ROOT', orig)
        revs = ma.linear_revisions('HEAD')
        diffs = ma.ledger_diffs(revs, ma.first_parents('HEAD'))
        t = ma.flips(ma.history_changes(diffs))
        self.assertEqual([s for _, s in t[('app', 'a', '0x1')]], ['+', '-', '+'])
        self.assertEqual([s for _, s in t[('app', 'a', '0x2')]], ['+'])


class TableTest(unittest.TestCase):
    def test_combine_confirm_and_share(self):
        cur = {('app', 'a', '0x1'), ('app', 'a', '0x2'), ('app', 'b', '0x3'), ('app', 'c', '0x4')}
        pad = {'rows': [{'lib': 'app', 'file': 'c', 'address': '0x4', 'class': 'pad-held', 'evidence': 'p'}]}
        work = {'rows': [{'lib': 'app', 'file': 'a', 'address': a, 'class': 'workaround-held', 'evidence': 'u'}
                         for a in ('0x1', '0x2')]
                + [{'lib': 'app', 'file': 'b', 'address': '0x3', 'class': 'workaround-held', 'evidence': 'u'}]}
        conf = {'app/a': {'device': 'D130', 'lost': ['0x2'], 'note': ''}}
        hist = {'rows': [{'lib': 'app', 'file': 'c', 'address': '0x4', 'class': 'unstable', 'evidence': 'h'},
                         {'lib': 'app', 'file': 'gone', 'address': '0x9', 'class': 'unstable', 'evidence': 'h'}]}
        rows, s = ma.combine(cur, pad, work, conf, hist)
        self.assertEqual([(r['file'], r['address'], r['class']) for r in rows],
                         [('c', '0x4', 'pad-held'), ('a', '0x2', 'workaround-held'), ('b', '0x3', 'workaround-held'),
                          ('c', '0x4', 'unstable')])
        self.assertTrue(rows[1]['evidence'].startswith('confirmed by build'))
        self.assertEqual(s['classes']['workaround-held'], {'count': 2, 'share': 0.5})
        self.assertEqual(s['not_stable'], {'count': 3, 'share': 0.75})
        md = ma.render(rows, s, {})
        self.assertIn('| contradicted | 0 | 0.0% | not built |', md)


if __name__ == '__main__':
    unittest.main()
