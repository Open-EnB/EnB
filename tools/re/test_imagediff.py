"""imagediff.py's pieces that need no image: python -m unittest tools.re.test_imagediff (or run this file)."""
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import imagediff  # noqa: E402


class Ranges(unittest.TestCase):
    def test_equal(self):
        self.assertEqual(imagediff.diff_ranges(b'abcdef', b'abcdef'), [])

    def test_runs(self):
        self.assertEqual(imagediff.diff_ranges(b'abcdef', b'aXXdeY'), [(1, 3), (5, 6)])

    def test_absent_counts_as_different(self):
        # our image holds no byte at offsets 2 and 3, though the padding there happens to be equal
        self.assertEqual(imagediff.diff_ranges(b'ab\0\0ef', b'ab\0\0ef', bytes([1, 1, 0, 0, 1, 1])), [(2, 4)])

    def test_merged(self):
        self.assertEqual(imagediff.merged([(0, 4), (10, 12), (40, 41)], 16), [[0, 12], [40, 41]])
        self.assertEqual(imagediff.merged([(0, 4), (10, 12)], 1), [[0, 4], [10, 12]])


class Map(unittest.TestCase):
    def test_rows(self):
        text = (' 0001:00000000       _WinMainCRTStartup         00401000 f   LIBCMT:wincrt0.obj\n'
                ' 0001:002a5b40       _$E20                      006a6b40 f i app:damtype.obj\n'
                ' 0000:00000000       __except_list              00000000     <absolute>\n'
                ' 0003:00000010       ?x@@3HA                    00847010     app:x.obj\n')
        with tempfile.TemporaryDirectory() as d:
            p = os.path.join(d, 'c.map')
            with open(p, 'w') as f:
                f.write(text)
            rows = imagediff.read_map(p)
        self.assertEqual(rows, [(0x401000, '_WinMainCRTStartup', 'LIBCMT:wincrt0.obj'),
                                (0x6A6B40, '_$E20', 'app:damtype.obj'),
                                (0x847010, '?x@@3HA', 'app:x.obj')])

    def test_names(self):
        n = imagediff.Names([(0x1000, 'a'), (0x1010, 'b')], reach=0x20)
        self.assertEqual((n.at(0x1000), n.at(0x1014), n.at(0x0FFF), n.at(0x2000)), ('a', 'b+0x4', '', ''))


if __name__ == '__main__':
    unittest.main()
