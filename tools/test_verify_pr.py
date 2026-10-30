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


if __name__ == '__main__':
    unittest.main()
