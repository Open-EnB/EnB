"""thunkname.py without an image: python -m unittest tools.re.test_thunkname (or run this file)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linklayout  # noqa: E402
import thunkname  # noqa: E402


class Parse(unittest.TestCase):
    def test_member(self):
        n = '?Find@KVListClass@@QAEPAVKVPairClass@@PBD@Z'
        p = thunkname.parse(n)
        self.assertEqual(p['kind'], 'member')
        self.assertEqual(n[p['code']:p['conv'] + 1], 'QAE')
        self.assertEqual(n[slice(*p['ret'])], 'PAVKVPairClass@@')
        self.assertEqual([n[a:b] for a, b in p['args']], ['PBD'])
        self.assertEqual(thunkname.class_of(n), 'KVListClass@@')

    def test_ctor_backref_template(self):
        n = '??0RecordTableClass@@QAE@HPAVDeltaListClass@@00@Z'
        p = thunkname.parse(n)
        self.assertIsNone(p['ret'])
        self.assertEqual([n[a:b] for a, b in p['args']], ['H', 'PAVDeltaListClass@@', '0', '0'])
        t = '?Get@?$DeltaTemplateAccessorClass@W4GameActionValidityBitFlag@@@@UAEABW4GameActionValidityBitFlag@@XZ'
        self.assertEqual(thunkname.parse(t)['end'], 'X')
        # a template argument that names a data symbol ($1?name@@3PADA)
        d = '??1?$SimpleShdDefFactoryClass@VShdSimpleDefClass@@$00$1?ShdSimpleDefClassName@@3PADA@@UAE@XZ'
        self.assertIsNotNone(thunkname.parse(d))

    def test_not_covered(self):
        self.assertIsNone(thunkname.parse('_main'))
        self.assertIsNone(thunkname.parse('??_EAABoxRenderObjClass@@W7AEPAXI@Z'))  # an adjustor thunk


class Variants(unittest.TestCase):
    def test_kinds(self):
        v = dict((x, k) for k, x in thunkname.variants('?Get_Flags@ObjectEffectPacketClass@@QAEIXZ'))
        self.assertEqual(v['?Get_Flags@ObjectEffectPacketClass@@AAEIXZ'], 'access')
        self.assertEqual(v['?Get_Flags@ObjectEffectPacketClass@@UAEIXZ'], 'virtual')
        self.assertEqual(v['?Get_Flags@ObjectEffectPacketClass@@QBEIXZ'], 'const')
        self.assertEqual(v['?Get_Flags@ObjectEffectPacketClass@@SAIXZ'], 'static')
        self.assertEqual(v['?Get_Flags@ObjectEffectPacketClass@@QAEHXZ'], 'ret')
        self.assertNotIn('?Get_Flags@ObjectEffectPacketClass@@QAAIXZ', v)  # __cdecl member without an ellipsis

    def test_params(self):
        v = dict((x, k) for k, x in thunkname.variants('?Value_To_String@@YAPADPADHF@Z'))
        self.assertEqual(v['?Value_To_String@@YAPADPBDHF@Z'], 'ptrcv')
        self.assertEqual(v['?Value_To_String@@YAPADAADHF@Z'], 'ptrref')
        self.assertEqual(v['?Value_To_String@@YAPADPADIF@Z'], 'int')
        self.assertEqual(v['?Value_To_String@@YGPADPADHF@Z'], 'callconv')
        # an argument a later backreference may point at is left alone
        v = dict((x, k) for k, x in thunkname.variants('??0RecordTableClass@@QAE@HPAVDeltaListClass@@00@Z'))
        self.assertNotIn('??0RecordTableClass@@QAE@HPBVDeltaListClass@@00@Z', v)
        self.assertIn('??0RecordTableClass@@QAE@IPAVDeltaListClass@@00@Z', v)
        self.assertNotIn('??0RecordTableClass@@UAE@HPAVDeltaListClass@@00@Z', v)  # no virtual constructor

    def test_deleting_dtor_fixed(self):
        v = dict((x, k) for k, x in thunkname.variants('??_GGadgetSlider@@UAEPAXI@Z'))
        self.assertEqual(v['??_EGadgetSlider@@UAEPAXI@Z'], 'gE')
        self.assertEqual(v['??_GGadgetSlider@@MAEPAXI@Z'], 'access')
        self.assertFalse([x for x, k in v.items() if k in ('int', 'callconv', 'const', 'ret')])

    def test_arg_table(self):
        self.assertTrue(thunkname.arg_table_ok(['H', 'PAVA@@', '0']))
        self.assertFalse(thunkname.arg_table_ok(['PAVA@@', 'PAVA@@']))
        self.assertFalse(thunkname.arg_table_ok(['H', '0']))


class Windows(unittest.TestCase):
    def test_anchor_windows(self):
        w = thunkname.anchor_windows(range(6), [(1, 10), (5, 30)], r_near=3)
        self.assertIsNone(w[0])
        self.assertEqual(w[1], (10, 10, 10, 10))
        self.assertEqual(w[3], (10, 30, 17, 23))  # interpolated 20

    def test_check_finds_access(self):
        # a table in LINK's order for 300 names (maxp 64, no split); one slot's name in "our" symbols lacks its
        # private access (Q for A): the check finds the A variant in that slot's window
        maxp = 64
        true = sorted((f'?F{k}@C@@AAEXXZ' if k % 2 else f'?F{k}@C@@QAEXXZ' for k in range(300)),
                      key=lambda n: linklayout.bucket(linklayout.lhash(n), maxp, 0))
        table = [(0x401005 + 5 * i, 0x500000 + 0x10 * i) for i in range(len(true))]
        names_of = {t: [n] for (_, t), n in zip(table, true)}
        b = lambda n: linklayout.bucket(linklayout.lhash(n), maxp, 0)
        for i in range(2, len(true) - 2):
            wrong = true[i].replace('@@AAE', '@@QAE')
            if wrong != true[i] and not (b(true[i - 1]) <= b(wrong) <= b(true[i + 1])):
                break
        names_of[table[i][1]] = [wrong]
        p, nb, rows, nulls = thunkname.check(table, names_of, {}, maxp, 0, null=2, r_near=maxp)
        r = rows[i]
        self.assertFalse(r['on'])
        self.assertIn(('access', true[i]), r['hits'])
        self.assertTrue(all(x['on'] for j, x in enumerate(rows) if j != i))


if __name__ == '__main__':
    unittest.main()
