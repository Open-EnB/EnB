"""symbols.py's thunk naming rule (a deleting destructor's data-referenced thunk is `??_E`) without an image:
python tools/re/test_symbols_thunk.py"""
import json
import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import symbols  # noqa: E402
from clientimage import ClientImage  # noqa: E402

G = '??_GFooClass@@UAEPAXI@Z'


class Rule(unittest.TestCase):
    def test_vtable_slot_is_vector(self):
        self.assertEqual(symbols.thunk_symbol(G, True), 'j_??_EFooClass@@UAEPAXI@Z')

    def test_call_only_stays_scalar(self):
        self.assertEqual(symbols.thunk_symbol(G, False), 'j_' + G)

    def test_two_thunks_one_of_each(self):
        self.assertEqual({symbols.thunk_symbol(G, True), symbols.thunk_symbol(G, False)},
                         {'j_??_EFooClass@@UAEPAXI@Z', 'j_' + G})

    def test_other_names_untouched(self):
        for n in ('?Run@FooClass@@UAEXXZ', '??1FooClass@@UAE@XZ', '??_EFooClass@@UAEPAXI@Z', '??0FooClass@@QAE@XZ'):
            self.assertEqual(symbols.thunk_symbol(n, True), 'j_' + n)
            self.assertEqual(symbols.thunk_symbol(n, False), 'j_' + n)

    def test_template_class(self):
        n = '??_G?$VectorClass@H@@UAEPAXI@Z'
        self.assertEqual(symbols.thunk_symbol(n, True), 'j_??_E?$VectorClass@H@@UAEPAXI@Z')


class DataRefs(unittest.TestCase):
    def test_aligned_dwords_in_data_sections_only(self):
        img = ClientImage.__new__(ClientImage)
        body = struct.pack('<5I', 0x401005, 0, 0x40100A, 0x401005, 7)
        img.data = b'\0' * 16 + b'\xEE' * 4 + body + struct.pack('<I', 0x40100A)
        # .text at file 0 holds a thunk address, which is a call site, not a data reference
        img.sections = {'.rdata': (0x500000, 24, 16, 24), '.data': (0x600000, 4, 40, 4)}
        img.section = lambda name: (img.sections[name][0], img.sections[name][0] + min(img.sections[name][1], img.sections[name][3]),
                                    img.sections[name][2])
        refs = img.data_refs({0x401005, 0x40100A, 0x999999})
        self.assertEqual(refs, {0x401005: [0x500004, 0x500010], 0x40100A: [0x50000C, 0x600000]})


class Named(unittest.TestCase):
    def test_reverse_rows_are_thunk_rows(self):
        rows = json.load(open(symbols.NAMED, encoding='utf-8'))
        rev = [r for r in rows if r.get('kind') == 'thunk']
        self.assertEqual(len(rev), 6)
        for r in rev:
            self.assertTrue(r['symbol'].startswith('j_??_G') and r['replaces'].startswith('j_??_E'), r)


if __name__ == '__main__':
    unittest.main()
