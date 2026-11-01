"""Tests for tools/vc6/verify_pr.py: the gate (what may be verified without the maintainer reading it first)
and the D66 split of lost pairs.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no VMs, no network.
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import verify_pr  # noqa: E402
import verify_watch  # noqa: E402
import pairs_cmp  # noqa: E402


class Gate(unittest.TestCase):
    def test_sources_docs_and_data_pass(self):
        self.assertEqual(verify_pr.needs_approval([
            'app/client/co.cpp', 'app/client/co.h', 'wwlib/vector.inl', 'libs/wwonline/wlib/wstring.cpp',
            'docs/workflow.md', 'README.md', 'docs/data/imports.json', 'tests/clienttest/data/basset.ini']), [])

    def test_anything_that_runs_on_the_host_is_gated(self):
        paths = ['tools/vc6/importer.py', 'tools/re/helper.cpp', '.github/workflows/checks.yml',
                 'tools/vc6/setup/New-BuildVM.ps1', 'tools/vc6/mss32.def', '.gitattributes', 'tests/x/gen.py',
                 'tools/hooks/pre-push', 'Makefile']
        self.assertEqual(verify_pr.needs_approval(paths), paths)


class Pairs(unittest.TestCase):
    def test_moves_and_outright(self):
        lost = [('app', 'a', '0x10'), ('app', 'a', '0x20')]
        verified = {('app', 'b', '0x10'), ('app', 'c', '0x30')}
        moves, outright = verify_pr.moves_and_outright(lost, verified)
        self.assertEqual(moves, [('app', 'a', [('app', 'b')], '0x10')])
        self.assertEqual(outright, ['0x20'])

    def test_accept_d66_names_the_head_and_needs_a_maintainer(self):
        m = lambda body, assoc='OWNER': {'body': body, 'author_association': assoc, 'user': {'login': 'x'}}  # noqa: E731
        self.assertIsNotNone(verify_pr.accepted_by([m('looked at it\n/accept-d66 abc1234')], 'abc1234ff'))
        self.assertIsNone(verify_pr.accepted_by([m('/accept-d66 0000000')], 'abc1234ff'))
        self.assertIsNone(verify_pr.accepted_by([m('/accept-d66 abc12')], 'abc1234ff'))
        self.assertIsNone(verify_pr.accepted_by([m('/accept-d66 abc1234', 'CONTRIBUTOR')], 'abc1234ff'))
        self.assertIsNone(verify_pr.accepted_by([m('see `/accept-d66 abc1234`')], 'abc1234ff'))
        self.assertIsNone(verify_pr.accepted_by([m(verify_pr.MARK + '\n/accept-d66 abc1234')], 'abc1234ff'))


class Corrections(unittest.TestCase):
    """Accepted corrections (D173, docs/data/accepted-corrections.json) in pairs_cmp.py and verify_pr.py."""
    E = {'address': '0x723AB0', 'lib': 'app', 'file': 'aux_roots', 'moved_to': '0x4B2AA0', 'decision': 'D173',
         'note': ''}

    def test_corrected_only_when_moved_to_is_gained_in_the_same_file(self):
        lost = [('app', 'aux_roots', '0x723AB0'), ('app', 'co', '0x10')]
        got, rest = pairs_cmp.split_corrections(lost, [('app', 'aux_roots', '0x4b2aa0')], [self.E])
        self.assertEqual([x for _, x in got], [('app', 'aux_roots', '0x723AB0')])
        self.assertEqual(rest, [('app', 'co', '0x10')])
        for gained in ([], [('app', 'other', '0x4B2AA0')], [('wwlib', 'aux_roots', '0x4B2AA0')]):
            got, rest = pairs_cmp.split_corrections(lost, gained, [self.E])
            self.assertEqual((got, rest), ([], sorted(lost)), gained)

    def test_moved_to_null_is_a_decided_loss(self):
        d = dict(self.E, address='0x77AA10', lib='wlib', file='configfile', moved_to=None, decision='D175')
        lost = [('wlib', 'configfile', '0x77aa10'), ('wlib', 'xtime', '0x77AA10')]
        got, rest = pairs_cmp.split_corrections(lost, [], [d])
        self.assertEqual([x for _, x in got], [('wlib', 'configfile', '0x77aa10')])
        self.assertEqual(rest, [('wlib', 'xtime', '0x77AA10')])
        res = {'verdict': 'success', 'summary': '+11 pairs, 0 lost, 1 corrected (accepted: D175)', 'merge': 'm' * 9,
               'base': 'b' * 9, 'when': 'now', 'corrected': [('wlib', 'configfile', '0x77AA10', None, 'D175')]}
        self.assertIn('wlib configfile 0x77AA10 (decided loss) (accepted: D175)',
                      verify_pr.report({'head': 'h' * 9}, res))

    def test_the_shipped_list_names_d175s_four(self):
        got = {(e['lib'], e['file'], e['address']) for e in pairs_cmp.load_corrections()
               if e['decision'] == 'D175' and e['moved_to'] is None}
        self.assertEqual(got, {('wlib', 'configfile', a) for a in ('0x77AA10', '0x77B030', '0x77B690', '0x850820')})

    def test_the_shipped_list_names_d173(self):
        entries = pairs_cmp.load_corrections()
        self.assertTrue(all(set(e) >= {'address', 'lib', 'file', 'moved_to', 'decision', 'note'} for e in entries))
        self.assertIn(('app', 'aux_roots', '0x723AB0', '0x4B2AA0', 'D173'),
                      [(e['lib'], e['file'], e['address'], e['moved_to'], e['decision']) for e in entries])
        self.assertEqual(pairs_cmp.load_corrections(os.path.join(HERE, 'no-such-file.json')), [])

    def test_pairs_cmp_prints_it_apart(self):
        import contextlib
        import io
        import json
        import tempfile
        d = tempfile.mkdtemp()
        a = {'app': {'files': {'aux_roots': {'in_tree': ['0x723AB0', '0x1']}}}}
        b = {'app': {'files': {'aux_roots': {'in_tree': ['0x4B2AA0', '0x1']}}}}
        for name, data in (('a.json', a), ('b.json', b)):
            with open(os.path.join(d, name), 'w', encoding='utf-8') as f:
                json.dump(data, f)
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            pairs_cmp.main(['pairs_cmp.py', os.path.join(d, 'a.json'), os.path.join(d, 'b.json')])
        lines = dict(l.split(' [', 1) for l in out.getvalue().splitlines() if ' [' in l)
        self.assertEqual(lines['corrected (accepted: D173)'], "('app', 'aux_roots', '0x723AB0', '0x4B2AA0')]")
        self.assertEqual(lines['lost'], ']')
        self.assertEqual(lines['lost outright'], ']')

    def test_landable_once_the_correction_leaves_lost(self):
        rec = {'head': 'h', 'base': 'm', 'verdict': 'success', 'lost': [],
               'corrected': [('app', 'aux_roots', '0x723AB0', '0x4B2AA0', 'D173')]}
        self.assertIsNone(verify_watch.landable(rec, 'h', 'm'))
        res = {'verdict': 'success', 'summary': '+1 pairs, 0 lost, 1 corrected (accepted: D173)', 'merge': 'm' * 9,
               'base': 'b' * 9, 'when': 'now', 'corrected': rec['corrected']}
        text = verify_pr.report({'head': 'h' * 9}, res)
        self.assertIn('app aux_roots 0x723AB0 -> 0x4B2AA0 (accepted: D173)', text)


class Landing(unittest.TestCase):
    REC = {'head': 'abc1234ff', 'base': 'm1', 'verdict': 'success', 'lost': []}

    def test_landable(self):
        self.assertIsNone(verify_watch.landable(self.REC, 'abc1234ff', 'm1'))
        self.assertIn('older master', verify_watch.landable(self.REC, 'abc1234ff', 'm2'))
        self.assertIn('head', verify_watch.landable(self.REC, 'def', 'm1'))
        self.assertIn('loses', verify_watch.landable(dict(self.REC, lost=[('app', 'a', '0x1')]), 'abc1234ff', 'm1'))
        self.assertIsNone(verify_watch.landable(dict(self.REC, lost=[('app', 'a', '0x1')], accepted={'by': 'x'}),
                                                'abc1234ff', 'm1'))

    def test_commands_name_the_head(self):
        c = [{'body': '/land abc1234\nthanks'}, {'body': '/verify-approved 0000000'}]
        self.assertTrue(verify_watch.names_head(c, verify_watch.LAND, 'abc1234ff'))
        self.assertFalse(verify_watch.names_head(c, verify_watch.APPROVE, 'abc1234ff'))
        self.assertFalse(verify_watch.names_head([{'body': '/land abc12'}], verify_watch.LAND, 'abc1234ff'))

    def test_hold_and_unhold(self):
        self.assertTrue(verify_watch.held([{'body': '/hold'}]))
        self.assertFalse(verify_watch.held([{'body': '/hold'}, {'body': '/unhold'}]))
        self.assertFalse(verify_watch.held([]))

    def test_what_lands_by_itself(self):
        ok = lambda p: any(rx.search(p) for rx in verify_watch.BOT_FILES)  # noqa: E731
        self.assertTrue(ok('app/client/co.cpp') and ok('docs/data/imports.json') and ok('wwlib/vector.h'))
        self.assertFalse(ok('docs/workflow.md') or ok('tools/vc6/importer.py') or ok('.github/x.h'))


if __name__ == '__main__':
    unittest.main()
