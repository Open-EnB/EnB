"""sigaudit.py's decoding and comparison without an image: python -m unittest tools.re.test_sigaudit (or run this file)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sigaudit  # noqa: E402


def d(n):
    return sigaudit.decode(n)


def fields(ours, retail):
    return {f: (o, r, k) for f, o, r, k in sigaudit.compare(d(ours), d(retail))}


class Decode(unittest.TestCase):
    def test_public_member(self):
        x = d('?Find@KVListClass@@QAEPAVKVPairClass@@PBD@Z')
        self.assertEqual((x['access'], x['kind'], x['const'], x['callconv']), ('public', 'normal', '', '__thiscall'))
        self.assertEqual((x['method'], x['cls']), ('Find', 'KVListClass'))
        self.assertEqual(sigaudit.type_text(x['ret']), 'class KVPairClass *')
        self.assertEqual([sigaudit.type_text(a) for a in x['args']], ['const char *'])

    def test_const_virtual_protected(self):
        x = d('?Get_Count@ListClass@@MBEHXZ')
        self.assertEqual((x['access'], x['kind'], x['const'], x['ret'], x['args']),
                         ('protected', 'virtual', 'const', 'H', []))

    def test_private_static(self):
        x = d('?Instance@Mgr@@CAPAV1@XZ')
        self.assertEqual((x['access'], x['kind'], x['const'], x['callconv']), ('private', 'static', None, '__cdecl'))

    def test_global_stdcall(self):
        x = d('?WndProc@@YGJPAUHWND__@@IIJ@Z')
        self.assertEqual((x['kind'], x['callconv'], x['ret']), ('global', '__stdcall', 'J'))
        self.assertEqual(sigaudit.type_text(x['args'][0]), 'struct HWND__ *')

    def test_ctor_backrefs(self):
        x = d('??0RecordTableClass@@QAE@HPAVDeltaListClass@@00@Z')
        self.assertEqual(x['ret'], '@')
        self.assertEqual(x['args'], ['H'] + ['PAVDeltaListClass@@'] * 3)

    def test_deleting_dtor(self):
        x = d('??_GFoo@@UAEPAXI@Z')
        self.assertEqual((x['method'], x['kind'], x['ret'], x['args']), ('?_G', 'virtual', 'PAX', ['I']))

    def test_ellipsis(self):
        x = d('?Printf@@YAXPBDZZ')
        self.assertTrue(x['ellipsis'])
        self.assertEqual(x['args'], ['PBD'])

    def test_enum_and_template(self):
        n = '?Get@?$DeltaTemplateAccessorClass@W4GameActionValidityBitFlag@@@@UAEABW4GameActionValidityBitFlag@@XZ'
        x = d(n)
        self.assertEqual(x['cls'], 'DeltaTemplateAccessorClass<enum(4) GameActionValidityBitFlag>')
        self.assertEqual(sigaudit.class_refs(x['ret']), [('W', 'GameActionValidityBitFlag')])

    def test_thunk_prefix_and_undecodable(self):
        self.assertEqual(d('j_??1Foo@@UAE@XZ')['method'], '?1')
        self.assertIsNone(d('_main'))
        self.assertIsNone(d('??_EAABoxRenderObjClass@@W7AEPAXI@Z'))   # an adjustor thunk


class Compare(unittest.TestCase):
    def test_access_const(self):
        f = fields('?Get_Flags@P@@QAEIXZ', '?Get_Flags@P@@ABEIXZ')
        self.assertEqual(f['access'][:2], ('public', 'private'))
        self.assertEqual(f['const'][:2], ('', 'const'))
        self.assertNotIn('name', f)

    def test_param_kinds(self):
        f = fields('?Set@P@@QAEXPAVFoo@@H@Z', '?Set@P@@QAEXABVFoo@@K@Z')
        self.assertEqual(f['param1'][2], 'ptrref+cv')     # pointer -> const reference
        self.assertEqual(f['param2'][2], 'int')
        f = fields('?Set@P@@QAEXPAVFoo@@@Z', '?Set@P@@QAEXPBVFoo@@@Z')
        self.assertEqual(f['param1'][2], 'ptrcv')
        f = fields('?Set@P@@QAEXPAVFoo@@@Z', '?Set@P@@QAEXPAUFoo@@@Z')
        self.assertEqual(f['param1'][2], 'classkey')
        f = fields('?Set@P@@QAEXPAVFoo@@@Z', '?Set@P@@QAEXPAVBar@@@Z')
        self.assertEqual(f['param1'][2], 'class')

    def test_backref_shift(self):
        # retail's first argument differs: our back-reference '0' must still compare as the spelled-out type
        f = fields('?F@P@@QAEXPAVA@@0@Z', '?F@P@@QAEXPAVB@@PAVA@@@Z')
        self.assertEqual(f['param1'][2], 'class')
        self.assertNotIn('param2', f)
        self.assertNotIn('nparams', f)

    def test_return_and_count(self):
        f = fields('?Build@Pk@@UAEPAVUnknownPacketBaseClass@@XZ', '?Build@Pk@@UAEPAVPacketClass@@H@Z')
        self.assertEqual(f['ret'][2], 'class')
        self.assertEqual(f['nparams'][:2], (0, 1))

    def test_class_rename_is_name_row(self):
        f = fields('?Draw@Old@@QAEXXZ', '?Draw@New@@QAEXXZ')
        self.assertEqual(list(f), ['name'])
        self.assertEqual(f['name'][2], 'class')
        self.assertEqual(fields('??_GFoo@@UAEPAXI@Z', '??_EFoo@@UAEPAXI@Z')['name'][2], 'gE')


class Census(unittest.TestCase):
    def test_spelled_and_enum(self):
        slots = [{'retail': '?F@P@@QAEXW4Mode@@PAUInfo@@@Z', 'ours': '?F@P@@QAEXHPAVInfo@@@Z'},
                 {'retail': '?G@P@@QAEXPAVNew@@@Z', 'ours': '?G@P@@QAEXPAVOld@@@Z'}]
        rows = {(r[0], r[1]): r for r in sigaudit.census(slots)}
        self.assertEqual(rows['enum', 'Mode'][2:4], (1, 0))
        self.assertIn(('struct-class', 'Info'), rows)
        self.assertEqual(rows['spelled', 'New'][4], 'ours Old')
        self.assertEqual(rows['spelled', 'Mode'][4], 'ours (int)')
        self.assertIn(('lacking', 'New'), rows)


if __name__ == '__main__':
    unittest.main()
