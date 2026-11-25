"""members.py on synthetic data: python -m unittest tools.re.test_members (or run this file)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import members  # noqa: E402


class Map(unittest.TestCase):
    def test_rows(self):
        text = [' 0001:00000000       _WinMainCRTStartup         00401000 f   LIBCMT:wincrt0.obj\n',
                ' 0001:00000100       ?f@@YAXXZ                  00401100 f i inpdsp.obj\n',
                ' 0002:00000010       ?x@@3HA                    00847010     app:x.obj\n']
        self.assertEqual(members.read_map(text), [(0x401000, '_WinMainCRTStartup', 'LIBCMT:wincrt0.obj', True),
                                                  (0x401100, '?f@@YAXXZ', 'inpdsp.obj', True)])

    def test_library_member(self):
        self.assertTrue(members.is_library_member('LIBCMT:wincrt0.obj'))
        self.assertFalse(members.is_library_member('app:x.obj'))
        self.assertFalse(members.is_library_member('inpdsp.obj'))


class Walk(unittest.TestCase):
    def test_runs_and_gaps(self):
        starts = [0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70]
        paired = {0x10: 'L:a.obj', 0x30: 'L:a.obj', 0x40: 'L:b.obj', 0x70: 'L:c.obj'}
        names = {0x50: 'crt_thing'}
        runs, gaps = members.walk(starts, paired, names, lo=0x10, hi=0x80)
        # 0x20 lies between two starts of a.obj: a.obj's; 0x50-0x60 lie between b and c: a gap
        self.assertEqual(runs, [(0x10, 0x40, 'L:a.obj', 3), (0x40, 0x50, 'L:b.obj', 1), (0x70, 0x80, 'L:c.obj', 1)])
        self.assertEqual(gaps, [(0x50, 0x70, 2, 'L:b.obj', 'L:c.obj', ['crt_thing'])])

    def test_classify(self):
        starts = [0x10, 0x20, 0x30]
        self.assertEqual(members.classify((0x10, 0x30), starts, {0x10: 'exact', 0x20: 'exact'}), 'held')
        self.assertEqual(members.classify((0x10, 0x30), starts, {0x10: 'named'}), 'part')
        self.assertEqual(members.classify((0x10, 0x40), starts, {}), 'unnamed')
        self.assertTrue(members.is_import_stub('KERNEL32:KERNEL32.dll'))

    def test_pair_unique_only(self):
        rows = [(1, 'f', 'L:a.obj', True), (2, 's', 'L:a.obj', True), (3, 's', 'L:b.obj', True)]
        self.assertEqual(members.pair([(0x100, 'f'), (0x200, 's'), (0x300, 'g')], rows), {0x100: 'L:a.obj'})


if __name__ == '__main__':
    unittest.main()
