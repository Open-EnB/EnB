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
