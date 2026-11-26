"""rich.py on synthetic headers: python -m unittest tools.re.test_rich (or run this file)."""
import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rich  # noqa: E402

PAIRS = [(19, 8022, 14), (11, 8799, 1054), (6, 1735, 1)]


def image(pairs, pe_at=None):
    dos = bytearray(0x80)
    dos[:2] = b'MZ'
    dos[0x40:0x50] = b'stub program....'
    block = rich.encode(bytes(dos), pairs)
    pe_at = pe_at or 0x80 + len(block) + 8
    struct.pack_into('<I', dos, 0x3c, pe_at)
    data = bytes(dos) + block
    return data + b'\0' * (pe_at - len(data)) + b'PE\0\0'


class Rich(unittest.TestCase):
    def test_round_trip(self):
        d = image(PAIRS)
        start, end, key, pairs = rich.decode(d)
        self.assertEqual((start, end, pairs), (0x80, 0x80 + 16 + 8 * 3 + 8, PAIRS))
        self.assertEqual(key, rich.key_of(d[:0x80], PAIRS))

    def test_key_ignores_e_lfanew(self):
        self.assertEqual(rich.decode(image(PAIRS))[2], rich.decode(image(PAIRS, pe_at=0x100))[2])

    def test_key_depends_on_count_and_order_free(self):
        dos = image(PAIRS)[:0x80]
        self.assertNotEqual(rich.key_of(dos, PAIRS), rich.key_of(dos, [(19, 8022, 15)] + PAIRS[1:]))
        self.assertEqual(rich.key_of(dos, PAIRS), rich.key_of(dos, PAIRS[::-1]))

    def test_rol(self):
        self.assertEqual(rich.rol(0x80000001, 1), 3)
        self.assertEqual(rich.rol(5, 32), 5)

    def test_lfanew_rule(self):
        # retail: 17 pairs, key 0xDF765B66 -> (key >> 5) % 3 == 0 -> 0x128
        self.assertEqual(rich.lfanew_of(0xDF765B66, 17), 0x128)
        self.assertEqual(rich.lfanew_of(1 << 5, 17), 0x130)
        self.assertEqual(rich.lfanew_of(2 << 5, 17), 0x138)
        self.assertEqual(rich.lfanew_of(3 << 5 | 31, 17), 0x128)

    def test_diff(self):
        ours = [(19, 8022, 14), (11, 8799, 914), (37, 8755, 2)]
        self.assertEqual(rich.diff(ours, PAIRS), [(11, 8799, 1054, 914), (6, 1735, 1, 0), (37, 8755, 0, 2)])


if __name__ == '__main__':
    unittest.main()
