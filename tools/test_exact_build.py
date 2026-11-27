"""The exact build (build.py's client-exact, dsp.py's exact workspace) and imagediff's masked figure."""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))
sys.path.insert(0, os.path.join(HERE, 're'))
import build as B  # noqa: E402
import dsp  # noqa: E402
import imagediff  # noqa: E402


class ExactBuild(unittest.TestCase):
    def test_program(self):
        p = B.PROGRAMS['client-exact']
        self.assertTrue(p['exact'] and p['strict'])
        self.assertEqual(p['libs'], B.PROGRAMS['client']['libs'])
        self.assertIn('/incremental:yes', B.EXACT_FLAGS)
        self.assertIn('/pdb:' + B.EXACT_PDB, B.EXACT_FLAGS)

    def test_order_unique(self):
        order = B.exact_order()
        self.assertGreater(len(order), 700)
        self.assertEqual(len({s.lower() for _, s in order}), len(order))
        self.assertEqual({l for l, _ in order} - {l for l, _ in B.EXACT_PICKS}, set())
        self.assertEqual([s for l, s in order if l == 'wwlib'], ['rawfile'])

    def test_dsp_order(self):
        e = dsp.exact_project([])
        self.assertTrue(e.ordered)
        self.assertEqual(e.dir, dsp.EXACT_DIR)
        stems = [os.path.splitext(os.path.basename(s))[0].lower() for s in e.sources]
        order = [s.lower() for _, s in B.exact_order()]
        self.assertEqual(stems, [s for s in order if s in set(stems)])
        text = e.text()
        self.assertEqual(text.count('SOURCE='), len(e.sources))
        self.assertIn('/incremental:yes', text)


class Masked(unittest.TestCase):
    def test_net7_edits(self):
        self.assertEqual(len(imagediff.NET7_EDITS), 11)
        self.assertEqual(sum(n for _, n in imagediff.NET7_EDITS), 732)
        ends = sorted((va, va + n) for va, n in imagediff.NET7_EDITS)
        self.assertTrue(all(a[1] <= b[0] for a, b in zip(ends, ends[1:])))

    @unittest.skipUnless(os.path.exists(imagediff.clientimage.DEFAULT_CLIENT), 'no retail client')
    def test_stamps(self):
        c = imagediff.Image(imagediff.clientimage.DEFAULT_CLIENT)
        got = [c.data[o:o + n] for o, n in imagediff.stamp_ranges(c)]
        self.assertEqual(got, [bytes.fromhex('00ae5740'), bytes.fromhex('31a82140'), bytes.fromhex('31a82140')])


if __name__ == '__main__':
    unittest.main()
