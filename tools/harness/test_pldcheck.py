"""Unit tests of pldcheck.py (python -m unittest tools/harness/test_pldcheck.py)."""
import os
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pldcheck  # noqa: E402


def make(count=16, normal=None, t1e8=None, normals=None):
    """A .pld as Save_PLD 0x46ECF0 writes it: count, +0x1E4, +0x1E8, +0x1EC, +0x1F0, +0x1F4, +0x1F8, 256 normals."""
    normal = normal if normal is not None else [i % 256 for i in range(count)]
    t1e8 = t1e8 if t1e8 is not None else [1000 + i for i in range(count)]
    normals = normals if normals is not None else [float(i) for i in range(256 * 3)]
    b = struct.pack('<I', count) + bytes(normal) + struct.pack(f'<{count}H', *t1e8)
    b += struct.pack(f'<{count}H', *[(2000 + i) & 0xFFFF for i in range(count)])
    b += bytes([1] * count) + bytes([2] * count) + bytes([3] * count)
    b += struct.pack('<768f', *normals)
    return b


class ParseTest(unittest.TestCase):
    def test_layout(self):
        t = pldcheck.parse(make())
        self.assertEqual(t['count'], 16)
        self.assertEqual(list(t['normal_index_1E4']), list(range(16)))
        self.assertEqual(pldcheck.values(t['table_1E8'], 2)[:2], [1000, 1001])
        self.assertEqual(t['table_1F8'], bytes([3] * 16))
        self.assertEqual(len(t['normals_1FC']), 3072)
        self.assertEqual(t['extra'], b'')

    def test_short_file(self):
        with self.assertRaises(pldcheck.PldError):
            pldcheck.parse(make()[:40])

    def test_real_size(self):
        # a 256 x 256 map: 4 + 65536 * (1 + 2 + 2 + 1 + 1 + 1) + 3072 bytes (ClientPOC's endriago_256.pld: 527364)
        self.assertEqual(len(make(65536, normal=[0] * 65536, t1e8=[0] * 65536)), 527364)


class DiffTest(unittest.TestCase):
    def test_same(self):
        self.assertEqual(pldcheck.diff_tables(pldcheck.parse(make()), pldcheck.parse(make())), {})
        self.assertEqual(pldcheck.fmt_diff({}), 'SAME')

    def test_normal_index_differs(self):
        n = [i % 256 for i in range(16)]
        n[5], n[14] = 9, 200
        d = pldcheck.diff_tables(pldcheck.parse(make()), pldcheck.parse(make(normal=n)))
        self.assertEqual(list(d), ['normal_index_1E4'])
        count, rows, left = d['normal_index_1E4']
        self.assertEqual((count, left), (2, 0))
        self.assertEqual(rows[0], (5, 1, 1, 5, 9))           # 4 x 4 grid: index 5 is x 1, y 1
        self.assertEqual(rows[1], (14, 2, 3, 14, 200))
        self.assertIn('normal_index_1E4: 2 differ', pldcheck.fmt_diff(d))

    def test_retail_spread_left_out(self):
        # retail's own runs differ at entry 3 of table_1E8 (sp_land_pld: six edge cells of +0x1EC..+0x1F8)
        a = pldcheck.parse(make())
        t = [1000 + i for i in range(16)]
        t[3] = 7
        sp = pldcheck.spread(a, [pldcheck.parse(make()), pldcheck.parse(make(t1e8=t))])
        self.assertEqual(sp, {'table_1E8': {3}})
        t2 = list(t)
        t2[3] = 8                                            # another value at the same entry: inside the spread
        self.assertEqual(pldcheck.diff_tables(a, pldcheck.parse(make(t1e8=t2)), skip=sp), {})
        t2[4] = 9                                            # an entry where retail agrees with itself
        d = pldcheck.diff_tables(a, pldcheck.parse(make(t1e8=t2)), skip=sp)
        self.assertEqual(d['table_1E8'][0], 1)
        self.assertEqual(d['table_1E8'][2], 1)

    def test_short_table_and_normals(self):
        t = [1000 + i for i in range(16)]
        t[3] = 7
        nv = [float(i) for i in range(768)]
        nv[3 * 10 + 2] = -1.0
        d = pldcheck.diff_tables(pldcheck.parse(make()), pldcheck.parse(make(t1e8=t, normals=nv)))
        self.assertEqual(d['table_1E8'][0], 1)
        self.assertEqual(d['normals_1FC'][0], 1)
        self.assertEqual(d['normals_1FC'][1][0][0], 10)

    def test_count_differs(self):
        d = pldcheck.diff_tables(pldcheck.parse(make()), pldcheck.parse(make(count=25)))
        self.assertEqual(d, {'count': (16, 25)})


class CompareTest(unittest.TestCase):
    def test_series(self):
        with tempfile.TemporaryDirectory() as tmp:
            def put(path, data):
                os.makedirs(os.path.dirname(path), exist_ok=True)
                with open(path, 'wb') as f:
                    f.write(data)
            for k in (1, 2):
                put(os.path.join(tmp, 'retail', f'run-{k}', 'land.pld'), make())
            n = [i % 256 for i in range(16)]
            n[0] = 99
            put(os.path.join(tmp, 'ours', 'run-1', 'land.pld'), make(normal=n))
            put(os.path.join(tmp, 'same', 'run-1', 'land.pld'), make())
            r, o, s = (os.path.join(tmp, x) for x in ('retail', 'ours', 'same'))
            import io
            import contextlib
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(pldcheck.main(['compare', '--retail', r, '--ours', s]), 0)
                self.assertEqual(pldcheck.main(['compare', '--retail', r, '--ours', o]), 1)


if __name__ == '__main__':
    unittest.main()
