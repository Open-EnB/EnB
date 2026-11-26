"""objsizes.py on synthetic maps and ledgers: python -m unittest tools.re.test_objsizes (or run this file)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import members  # noqa: E402
import objsizes  # noqa: E402

ROWS = [(0x1000, 'a1', 'a.obj', True), (0x1040, 'a2', 'a.obj', True), (0x1100, 'b1', 'b.obj', True),
        (0x1200, 'c1', 'ww3d2:c.obj', True), (0x1210, 'end', 'ww3d2:c.obj', False)]


class ObjSizes(unittest.TestCase):
    def test_our_objects(self):
        o = objsizes.our_objects(ROWS)
        self.assertEqual(o['a.obj'][:2], [2, 0x100])
        self.assertEqual(o['b.obj'][:2], [1, 0x100])
        self.assertEqual(o['ww3d2:c.obj'][:2], [1, 0x10])     # the last row has no size

    def test_ledger_pairs_own_only(self):
        ledger = {'app': {'files': {'a': {'name': 'client/a.cpp', 'in_tree': ['0x500', '0x600'], 'shared': ['0x600']}}},
                  'ww3d2': {'files': {'c': {'name': 'c.cpp', 'in_tree': ['0x900'], 'shared': []},
                                      'z': {'name': 'z.cpp', 'in_tree': ['0x990'], 'shared': []}}}}
        self.assertEqual(objsizes.ledger_pairs(ledger, {'a.obj', 'ww3d2:c.obj'}), {0x500: 'a.obj', 0x900: 'ww3d2:c.obj'})

    def test_table_and_runs(self):
        starts = [0x429630, 0x429700, 0x429800, 0x429900, 0x429a00]
        paired = {0x429630: 'a.obj', 0x429800: 'a.obj', 0x429a00: 'b.obj'}
        runs, gaps = members.walk(starts, paired, {}, lo=objsizes.APP_LO, hi=0x429b00)
        self.assertEqual([(s, e, m) for s, e, m, _ in runs], [(0x429630, 0x429900, 'a.obj'), (0x429a00, 0x429b00, 'b.obj')])
        self.assertEqual([(g[0], g[1]) for g in gaps], [(0x429900, 0x429a00)])
        theirs = objsizes.retail_runs(runs)
        tab = objsizes.table({'a.obj': [2, 0x300, 0], 'b.obj': [1, 0x80, 0]}, theirs, {'a.obj': 2, 'b.obj': 1})
        self.assertEqual([(r[0], r[4]) for r in tab], [('b.obj', -0x80), ('a.obj', 0x300 - 0x2d0)][::-1])


    def test_lis(self):
        self.assertEqual(objsizes.lis([1, 5, 2, 3, 0, 4]), [0, 2, 3, 5])
        self.assertEqual(objsizes.lis([]), [])

    def test_growth_telescopes(self):
        base = objsizes.APP_LO
        rows = [(base + 0x00, 'f1', 'a.obj', True), (base + 0x40, 'f2', 'a.obj', True),
                (base + 0x100, 'g1', 'b.obj', True), (base + 0x180, 'h1', 'c.obj', True)]
        # retail: a's f2 is 0x20 smaller, b is 0x10 larger; h1 is the last fixed point
        syms = [(base + 0x00, 'f1'), (base + 0x40, 'f2'), (base + 0xE0, 'g1'), (base + 0x170, 'h1')]
        g = {m: (gr, ins) for m, _, _, gr, _, ins in objsizes.growth(syms, rows)}
        self.assertEqual(g['a.obj'], (0x20, 0))
        self.assertEqual(g['b.obj'], (-0x10, 0))
        self.assertEqual(sum(x[0] for x in g.values()), (base + 0x180 - (base + 0x170)))


if __name__ == '__main__':
    unittest.main()
