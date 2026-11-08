"""Tests for tools/vc6/wt.py's batch form: the add-only union, the loss rule, pairs_cmp.compare.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no VMs, no network.
"""
import json
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import pairs_cmp  # noqa: E402
import wt  # noqa: E402


def rec(**files):
    return {'app': {'files': {f: {'in_tree': v} for f, v in files.items()}}}


class AddOnlyUnion(unittest.TestCase):
    BASE = "ROWS = [\n    'a',\n    'z',\n]\n"

    def test_both_add(self):
        t, why = wt.add_only_union(self.BASE, "ROWS = [\n    'a',\n    'b',\n    'z',\n]\n",
                                   "ROWS = [\n    'a',\n    'c',\n    'z',\n]\n")
        self.assertIsNone(why)
        self.assertEqual(t, "ROWS = [\n    'a',\n    'b',\n    'c',\n    'z',\n]\n")

    def test_same_line_added_once(self):
        t, why = wt.add_only_union(self.BASE, "ROWS = [\n    'a',\n    'b',\n    'c',\n    'z',\n]\n",
                                   "ROWS = [\n    'a',\n    'b',\n    'd',\n    'z',\n]\n")
        self.assertIsNone(why)
        self.assertEqual(t.count("'b'"), 1)

    def test_both_change_a_line_refused(self):
        t, why = wt.add_only_union(self.BASE, "ROWS = [\n    'A',\n    'z',\n]\n", "ROWS = [\n    'B',\n    'z',\n]\n")
        self.assertIsNone(t)
        self.assertIn('change existing', why)

    def test_union_must_compile(self):
        t, why = wt.add_only_union("x = 1\n", "x = 1\nif x:\n", "x = 1\ny = (\n")
        self.assertIsNone(t)
        self.assertIn('compile', why)


class LossRule(unittest.TestCase):
    def setUp(self):
        wt_corr = wt.corrections_at
        wt.corrections_at = lambda r: []
        self.addCleanup(setattr, wt, 'corrections_at', wt_corr)

    def judge(self, a, b):
        return wt.judge_losses(a, b, 'T', 'old', 'test')

    def test_move_lands(self):
        r = self.judge(rec(f1=['0x1'], f2=[]), rec(f1=[], f2=['0x1']))
        self.assertEqual(r['lost_outright'], [])
        self.assertEqual([m[3] for m in r['moves']], ['0x1'])

    def test_outright_refuses_and_names(self):
        with self.assertRaises(SystemExit) as c:
            self.judge(rec(f1=['0x1', '0x2'], f2=['0x2']), rec(f1=['0x2'], f2=['0x2']))
        self.assertIn('0x1', str(c.exception))
        self.assertIn('T untouched', str(c.exception))

    def test_compare_lost_is_outright_plus_held(self):
        r = pairs_cmp.compare(rec(f1=['0x1', '0x2'], f2=[]), rec(f1=[], f2=['0x2'], f3=['0x3']), [])
        self.assertEqual(r['lost_outright'], ['0x1'])
        self.assertEqual(len(r['lost']), 2)
        self.assertEqual(r['gained'], [('app', 'f2', '0x2'), ('app', 'f3', '0x3')])


class LedgerShards(unittest.TestCase):
    def test_generated(self):
        self.assertTrue(wt.generated('docs/data/imports/app/co.json'))
        self.assertTrue(wt.generated('docs/imports.md'))
        self.assertFalse(wt.generated('app/client/co.cpp'))

    def test_shard_union_takes_target_pairs(self):
        b = json.dumps({'in_tree': ['0x1'], 'description': 'd'})
        o = json.dumps({'in_tree': ['0x2'], 'description': 'd'})
        t = json.dumps({'in_tree': ['0x3'], 'description': 'd'})
        self.assertEqual(wt.shard_union(b, o, t), (None, None))

    def test_shard_union_keeps_branch_description(self):
        b = json.dumps({'in_tree': ['0x1'], 'description': 'd'})
        o = json.dumps({'in_tree': ['0x2'], 'description': 'd'})
        t = json.dumps({'in_tree': ['0x3'], 'description': 'new'})
        text, why = wt.shard_union(b, o, t)
        self.assertIsNone(why)
        self.assertEqual(json.loads(text), {'in_tree': ['0x2'], 'description': 'new'})

    def test_shard_union_both_descriptions_refuse(self):
        b, o, t = (json.dumps({'description': x}) for x in ('d', 'o', 't'))
        self.assertIsNotNone(wt.shard_union(b, o, t)[1])


if __name__ == '__main__':
    unittest.main()
