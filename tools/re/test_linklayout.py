"""linklayout.py's pieces that need no image: python -m unittest tools.re.test_linklayout (or run this file)."""
import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linklayout  # noqa: E402

CC = b'\xCC'


def code(n, last=b'\xC3'):
    """n bytes standing for a function: `last` (a ret), then nop fill (no int3, so all n count as its size)."""
    return last + b'\x90' * (n - 1) if n else b''


class Slot(unittest.TestCase):
    def test_rule(self):
        # retail's own figures: 368 -> 464 (0x429630), 432 -> 544, 16 -> 32, 11 -> 16, 17,792 -> 22,240
        self.assertEqual([linklayout.slot(s) for s in (368, 432, 16, 11, 17792)], [464, 544, 32, 16, 22240])


class Contributions(unittest.TestCase):
    def setUp(self):
        # three functions at 0x1000: 48 bytes + rule pad (64), 32 bytes + 16 (rule 48), then a host whose slot holds
        # a guest: 64 bytes + guest 16 bytes, filling slot(64) = 80 exactly
        self.base = 0x1000
        self.body = (code(48) + CC * 16) + (code(32) + CC * 16) + code(64) + (code(8) + CC * 8) + code(32) + CC * 16
        self.starts = [0x1000, 0x1040, 0x1070, 0x10B0, 0x10C0]

    def test_sizes(self):
        c = linklayout.contributions(self.body, self.base, self.starts, 0x1000, self.base + len(self.body))
        self.assertEqual(c, [(0x1000, 48, 16), (0x1040, 32, 16), (0x1070, 64, 0), (0x10B0, 8, 8), (0x10C0, 32, 16)])

    def test_classes(self):
        c = linklayout.contributions(self.body, self.base, self.starts, 0x1000, self.base + len(self.body))
        rows = linklayout.classify(c)
        self.assertEqual([r['cls'] for r in rows], ['rule', 'rule', 'host', 'rule'])
        self.assertEqual(rows[2]['guests'], [(0x10B0, 8, 8)])

    def test_short_and_long(self):
        rows = linklayout.classify([(0x0, 48, 0), (0x30, 32, 64)])
        self.assertEqual([(r['cls'], r['excess']) for r in rows], [('short', -16), ('long', 48)])

    def test_hidden_start(self):
        # an int3 run ending on 16 after nop fill, at a place the start list lacks
        body = code(16) + CC * 16 + code(16) + CC * 16
        self.assertEqual(linklayout.hidden_starts(body, 0x2000, 0x2000, 0x2040, [0x2000]), [0x2020])
        self.assertEqual(linklayout.hidden_starts(body, 0x2000, 0x2000, 0x2040, [0x2000, 0x2020]), [])


class FirstFit(unittest.TestCase):
    def test_fit(self):
        # a 32-byte int3 run at 0x10 (16-aligned room 32) before a guest of 32 bytes at 0x40, none before one at 0x20
        body = b'\x90' * 16 + CC * 32 + b'\x90' * 64
        out = linklayout.first_fit(body, 0, 0, [(0x10, 16), (0x40, 32), (0x40, 48)])
        self.assertEqual([k for _, _, k, _ in out], [0, 1, 0])


class Thunks(unittest.TestCase):
    def table_bytes(self, base, targets):
        b = bytearray(CC * 5)
        for t in targets:
            at = base + len(b)
            b += b'\xE9' + struct.pack('<i', t - (at + 5))
        return bytes(b) + CC * 80 + b'\x90' * 0x200

    def test_table(self):
        base = 0x401000
        body = self.table_bytes(base, [0x401100, 0x401180, 0x401100])
        t = linklayout.thunk_table(body, base)
        self.assertEqual(t, [(0x401005, 0x401100), (0x40100A, 0x401180), (0x40100F, 0x401100)])

    def test_doubles(self):
        table = [(0, 0x10), (5, 0x20), (10, 0x10), (15, 0x30), (20, 0x20), (25, 0x40), (30, 0x40)]
        names = {0x10: '??_GFoo@@UAEPAXI@Z', 0x20: '??0Bar@@QAE@H_J@Z'}
        bodies = {0x40: b'\x56\x8b\xf1\xe8\0\0\0\0\xf6\x44\x24\x08\x01\x74\x09'}
        d = linklayout.doubles(table, names, lambda va, n: bodies.get(va, b'\x90' * n)[:n])
        self.assertEqual(d, {0x10: ([0, 2], 'deleting dtor'), 0x20: ([1, 4], 'other'), 0x40: ([5, 6], 'deleting dtor')})

    def test_dtor_shapes(self):
        f = linklayout.deleting_dtor_code
        self.assertTrue(f(bytes.fromhex('8a442404568bf1a801c706d078ae00')))               # entry load, test al, 1
        self.assertTrue(f(bytes.fromhex('568bf18b4604c706c491af00') + bytes.fromhex('8a442408c6460d00a801')))
        self.assertFalse(f(bytes.fromhex('568bf18b4c24088b542410c74604000000a801')))  # a ctor: a801 alone is not enough


class Runs(unittest.TestCase):
    def test_pieces(self):
        seq = [(1, 'a'), (2, 'a'), (3, 'b'), (4, 'a'), (5, 'c'), (6, 'c')]
        runs, pieces = linklayout.object_runs(seq)
        self.assertEqual([(r['obj'], r['n']) for r in runs], [('a', 2), ('b', 1), ('a', 1), ('c', 2)])
        self.assertEqual([(p['obj'], p['start'], p['main']['start']) for p in pieces], [('a', 4, 1)])


# The order of a VC6 incremental link's thunk table, observed on probe links of 2026-11-25 (one C file of functions
# with these names, `link /debug /incremental:yes /fixed:no` on a pool VM; .work/im2/hashprobe.py)
PROBE_TWO = ('_main _aa _ab _ba _ac _bb _ad _bc _ae _bd _af _be _ag _bf _ah _bg _ai _bh _aj _bi _ak _bj _al _bk _am '
             '_bl _an _bm _ao _bn _ap _bo _aq _bp _ar _bq _as _br _at _bs _au _bt _av _bu _aw _bv _ax _bw _ay _bx _az '
             '_by _bz').split()
# the same probe with f0000..f0999: the table's first bucket, the functions defined in ascending order
PROBE_F1000_HEAD = ['_f0550', '_f0450', '_f0340', '_f0260', '_f0551', '_f0451', '_f0341', '_f0261']


class ThunkOrder(unittest.TestCase):
    def test_hash(self):
        # h = h + (c << 18) + c + ((h >> 1) ^ h) per byte, then mod 0x1ECA9D3B
        self.assertEqual(linklayout.lhash(''), 0)
        self.assertEqual(linklayout.lhash('a'), (97 << 18) + 97)
        self.assertEqual(linklayout.lhash('_main'), 340373223)

    def test_bucket(self):
        # below the split pointer the bucket takes one more bit
        h = 3 * 64 + 5
        self.assertEqual(linklayout.bucket(h, 64, 0), 5)
        self.assertEqual(linklayout.bucket(h, 64, 6), 69)

    def test_probe_order_is_bucket_order(self):
        hs = [linklayout.lhash(n) for n in PROBE_TWO]
        p = linklayout.fit_split(hs, 64)
        bs = [linklayout.bucket(h, 64, p) for h in hs]
        self.assertEqual(bs, sorted(bs))

    def test_chain_newest_first(self):
        first_seen = {n: i for i, n in enumerate(sorted(PROBE_F1000_HEAD))}
        self.assertEqual(linklayout.predict_order(PROBE_F1000_HEAD, first_seen, 128, 104), PROBE_F1000_HEAD)

    def test_name_check(self):
        # slots named in bucket order, one of them with a name that hashes elsewhere
        names = sorted(['_a%d' % i for i in range(40)], key=lambda n: linklayout.bucket(linklayout.lhash(n), 64, 0))
        names[20] = '_invented_name'
        table = [(0x1000 + 5 * i, 0x2000 + 16 * i) for i in range(len(names))]
        names_of = {t: [n] for (_, t), n in zip(table, names)}
        p, rows = linklayout.name_check(table, names_of, 64, 0)
        off = [r[2] for r in rows if not r[4]]
        self.assertLessEqual(len(off), 2)
        self.assertIn('_invented_name', off)

    def test_lnds(self):
        self.assertEqual(linklayout.lnds_members([1, 5, 2, 2, 3, 0, 4]), [0, 2, 3, 4, 6])


if __name__ == '__main__':
    unittest.main()
