"""crtseq.py: the padding model, the comparison on made sequences, objects' kinds on the behaudit fixtures and
retail's table (client.exe)."""
import os
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import crtseq as C  # noqa: E402

FIX = os.environ.get('BEHAUDIT_FIXTURES') or next(
    (p for p in (os.path.join(os.path.dirname(os.path.dirname(HERE)), '.work', 'behaudit', 'fixtures'),
                 'C:/EnBDevelopment/EnB/.work/behaudit/fixtures') if os.path.isdir(p)), '')
LST = os.path.join(FIX, 'lst')


class Model(unittest.TestCase):
    def test_pad(self):
        self.assertEqual([C.pad(n) for n in (0, 1, 2, 6, 7, 11, 12, 17, 18, 40)], [0, 0, 1, 1, 2, 2, 3, 3, 4, 4])

    def test_compare(self):
        retail = [(0, 'CTYPE', 0x401000), (4, 'PERMSTR', 0x402000), (8, 'other', 0x403000), (12, 'Z', 0),
                  (16, 'CTYPE', 0x404000)]
        ours = [('CTYPE', 'a.obj'), ('PERMSTR', 'b.obj'), ('IOS', 'b.obj'), ('Z', 'b.obj'), ('CTYPE', 'c.obj')]
        lines, same = C.compare(retail, ours)
        self.assertTrue(same, lines)          # IOS counts as X by default
        lines, same = C.compare(retail, ours, all_kinds=True)
        self.assertFalse(same)
        self.assertIn('retail [X:0x403000] ours [IOS:b.obj]', lines[1])
        lines, same = C.compare(retail, ours[:3] + ours[4:])
        self.assertEqual(lines[1], 'delete after retail 0x403000 / ours b.obj: retail [Z] ours []')

    def test_our_kinds_padding(self):
        made = {'a.obj': ['CTYPE'], 'b.obj': ['PERMSTR', 'CTYPE'], 'c.obj': ['CTYPE', 'other']}
        keep = C.object_kinds
        C.object_kinds = lambda data: made[data]
        try:
            got, missing = C.our_kinds('B', ['app/obj/a.obj', '# note', 'app/obj/b.obj', 'app/obj/x.obj',
                                             'app/obj/c.obj'],
                                       read=lambda p: os.path.basename(p) if 'x.obj' not in p else open(p))
        finally:
            C.object_kinds = keep
        self.assertEqual([k for k, _ in got], ['CTYPE', 'PERMSTR', 'CTYPE', 'Z', 'CTYPE', 'other'])  # last Z trimmed
        self.assertEqual(missing, ['app/obj/x.obj'])


@unittest.skipUnless(os.path.exists(os.path.join(LST, 'sclient.obj')), 'fixtures missing')
class Objects(unittest.TestCase):
    def test_kinds(self):
        def kinds(n):
            return C.object_kinds(open(os.path.join(LST, n), 'rb').read())
        self.assertEqual(kinds('ui_mount.obj'), ['PERMSTR', 'CTYPE', 'other'])
        self.assertEqual(kinds('starmap_data.obj'), ['other', 'other', 'CTYPE'])
        self.assertEqual(kinds('unknown_466920.obj'), [])

    def test_retail(self):
        import clientimage
        import json
        import members
        names = {}
        for s in json.load(open(members.SYMBOLS, encoding='utf-8')):
            if s.get('kind') != 'thunk':
                names.setdefault(int(s['address'], 16), s['symbol'])
        r = C.retail_kinds(clientimage.ClientImage(), names)
        self.assertEqual(len(r), 1107)                              # IM8's count: 940 initialisers, 167 zeros
        self.assertEqual(sum(1 for _, k, _ in r if k == 'Z'), 167)
        self.assertNotEqual(r[0][1], 'Z')
        self.assertTrue(all(va < C.LIB_LO for _, _, va in r))
        # the tool end to end on a made order of two fixture objects
        d = tempfile.mkdtemp()
        order = os.path.join(d, 'order.txt')
        open(order, 'w').write(os.path.join(LST, 'ui_mount.obj') + '\n' + os.path.join(LST, 'ui_avatr.obj') + '\n')
        import contextlib
        import io
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            self.assertEqual(C.main(['--build', d, '--order', order, '--retail-kinds', os.path.join(d, 'rk.json')]), 1)
        self.assertIn('... +', out.getvalue())
        self.assertEqual(len(json.load(open(os.path.join(d, 'rk.json')))), 940)


if __name__ == '__main__':
    unittest.main()
