"""rsrc.py's pieces on synthetic data: python -m unittest tools.re.test_rsrc (or run this file)."""
import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rsrc  # noqa: E402


def tree(rva):
    """A .rsrc with one resource: type 'TYPELIB', id 1, language 1033, 6 bytes of data, at section RVA rva."""
    d = bytearray(0x80)
    # root: 1 named entry -> type directory at 0x18
    struct.pack_into('<IIHHHH', d, 0, 0, 0, 0, 0, 1, 0)
    struct.pack_into('<II', d, 16, 0x80000000 | 0x60, 0x80000000 | 0x18)
    struct.pack_into('<IIHHHH', d, 0x18, 0, 0, 0, 0, 0, 1)          # names: id 1 -> language dir 0x30
    struct.pack_into('<II', d, 0x28, 1, 0x80000000 | 0x30)
    struct.pack_into('<IIHHHH', d, 0x30, 0, 0, 0, 0, 0, 1)          # languages: 1033 -> data entry 0x48
    struct.pack_into('<II', d, 0x40, 1033, 0x48)
    struct.pack_into('<IIII', d, 0x48, rva + 0x70, 6, 0, 0)
    name = 'TYPELIB'.encode('utf-16-le')
    struct.pack_into('<H', d, 0x60, 7)
    d[0x62:0x62 + len(name)] = name
    d[0x70:0x76] = b'abcdef'
    return bytes(d)


class Tree(unittest.TestCase):
    def test_parse(self):
        dirs, res = rsrc.parse(tree(0x5000), 0x5000)
        self.assertEqual(len(dirs), 3)
        self.assertEqual(res, [{'type': 'TYPELIB', 'name': 1, 'lang': 1033, 'data_rva': 0x5070, 'size': 6,
                                'codepage': 0, 'entry': 0x48}])

    def test_rebased(self):
        moved = rsrc.rebased(tree(0x5000), 0x5000, 0x9000)
        self.assertEqual(moved, tree(0x9000))
        self.assertEqual(rsrc.equal_bytes(tree(0x9000), tree(0x5000), 0x80), 0x80 - 1)   # one byte of the RVA


class Res(unittest.TestCase):
    def test_entry_layout(self):
        e = rsrc.res_entry(5, 104, 1033, b'abcde')
        size, hsize = struct.unpack_from('<II', e, 0)
        self.assertEqual((size, hsize), (5, 32))
        self.assertEqual(e[8:16], b'\xff\xff\x05\x00\xff\xff\x68\x00')
        self.assertEqual(struct.unpack_from('<IHHII', e, 16), (0, 0x1030, 1033, 0, 0))
        self.assertEqual(len(e), 32 + 8)                    # data padded to a dword

    def test_named_type_aligned(self):
        e = rsrc.res_entry('TYPELIB', 1, 1033, b'x')
        hsize = struct.unpack_from('<I', e, 4)[0]
        self.assertEqual(hsize % 4, 0)
        self.assertEqual(e[8:8 + 16], 'TYPELIB'.encode('utf-16-le') + b'\0\0')

    def test_round_trip(self):
        items = [(16, 1, 1033, b'v' * 7), ('TYPELIB', 1, 1033, b't' * 4), (6, 1, 1033, b's' * 9)]
        data = rsrc.to_res(items)
        self.assertEqual(data[:32], bytes.fromhex('00000000200000 00ffff0000ffff0000'.replace(' ', ''))
                         + b'\0' * 16)
        self.assertEqual(rsrc.read_res(data), items)


if __name__ == '__main__':
    unittest.main()
