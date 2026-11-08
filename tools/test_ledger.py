"""Tests for tools/vc6/ledger.py: the sharded ledger round-trips imports.json's dict.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no VMs, no network, no git.
"""
import json
import os
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import ledger  # noqa: E402

STATE = {
    'app': {'base': 'reconstructed', 'totals': {}, 'compile_errors': [], 'in_tree_total': 2, 'ambiguous_total': 1,
            'files': {'co': {'name': 'client/co.cpp', 'copy': 'reconstructed', 'expected': {}, 'merged': [],
                             'in_tree': ['0x401000', '0x401010'], 'ambiguous': ['0x402000']}},
            'headers': {'client/co.h': 'reconstructed'}},
    'wwlib': {'base': 'R', 'totals': {'R': 3, 'G': 1}, 'compile_errors': [], 'in_tree_total': 1, 'ambiguous_total': 0,
              'files': {'vector': {'name': 'Vector.cpp', 'copy': 'R', 'expected': {'R': 1}, 'merged': [],
                                   'in_tree': ['0x9C7000'], 'ambiguous': []}},
              'headers': {'Vector.h': 'R'}},
}
DESC = {('app', 'client/co.cpp'): 'the co file', ('app', 'client/co.h'): 'its header',
        ('wwlib', 'unknown.h'): 'a description with no header row'}


class Ledger(unittest.TestCase):
    def setUp(self):
        self.td = tempfile.TemporaryDirectory()
        self.src = os.path.join(self.td.name, 'imports.json')
        with open(self.src, 'w', encoding='utf-8') as f:
            json.dump(STATE, f)
        self.py = os.path.join(self.td.name, 'defs.py')
        with open(self.py, 'w', encoding='utf-8') as f:
            f.write('RECONSTRUCTED = %r\n' % DESC)
        self.root = os.path.join(self.td.name, 'imports')
        ledger.split(self.src, self.root, self.py)

    def tearDown(self):
        self.td.cleanup()

    def test_round_trip(self):
        self.assertEqual(ledger.load(self.root), STATE)
        self.assertEqual(ledger.descriptions(self.root), DESC)

    def test_layout(self):
        names = sorted(rel for rel, _ in ledger._walk(self.root))
        self.assertEqual(names, ['app/co.json', 'app/headers/client/co.h.json', 'app/library.json',
                                 'wwlib/headers/Vector.h.json', 'wwlib/headers/unknown.h.json',
                                 'wwlib/library.json', 'wwlib/vector.json'])
        lib = json.loads(ledger._text(os.path.join(self.root, 'wwlib', 'library.json')))
        self.assertNotIn('in_tree_total', lib)
        text = ledger._text(os.path.join(self.root, 'app', 'co.json'))
        self.assertTrue(text.endswith('}\n') and '\r' not in text and '\n  "0x401000",\n' in text)

    def test_counts_computed(self):
        s = json.loads(json.dumps(STATE))
        s['app']['files']['co']['in_tree'].append('0x401020')
        ledger.save(s, self.root)
        self.assertEqual(ledger.load(self.root)['app']['in_tree_total'], 3)

    def test_save_keeps_descriptions_and_drops_gone_files(self):
        s = json.loads(json.dumps(STATE))
        del s['wwlib']['files']['vector']
        ledger.save(s, self.root)
        self.assertNotIn('vector', ledger.load(self.root)['wwlib']['files'])
        self.assertEqual(ledger.descriptions(self.root), DESC)

    def test_compare(self):
        self.assertEqual(ledger.compare(self.src, self.root), 0)

    def test_describe_adds_a_reconstructed_file(self):
        ledger.describe('app', 'shared/auxdata/zz.cpp', 'desc A', self.root)
        ledger.describe('app', 'shared/auxdata/zz.h', 'desc H', self.root)
        d = ledger.descriptions(self.root)
        self.assertEqual(d[('app', 'shared/auxdata/zz.cpp')], 'desc A')
        self.assertEqual(d[('app', 'shared/auxdata/zz.h')], 'desc H')
        st = ledger.load(self.root)['app']
        self.assertEqual(st['files']['zz'], {'name': 'shared/auxdata/zz.cpp'})
        self.assertEqual(st['headers']['shared/auxdata/zz.h'], 'reconstructed')
        with self.assertRaises(SystemExit):
            ledger.describe('app', 'other/zz.cpp', 'same key', self.root)

    def test_case_collision_refused(self):
        s = json.loads(json.dumps(STATE))
        s['wwlib']['headers']['vector.h'] = 'R'
        with self.assertRaises(SystemExit):
            ledger.shard_paths(s)


if __name__ == '__main__':
    unittest.main()
