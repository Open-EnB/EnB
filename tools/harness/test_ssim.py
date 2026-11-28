"""Unit tests of ssim.py and audio3d.py (python -m unittest tools/harness/test_ssim.py)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import audio3d  # noqa: E402
import ssim  # noqa: E402


def img(w, h, f):
    return [[float(f(x, y)) for x in range(w)] for y in range(h)]


class SsimTest(unittest.TestCase):
    def test_identical(self):
        a = img(32, 16, lambda x, y: (x * 7 + y * 3) % 255)
        m, worst, _, n = ssim.ssim_rows(a, a)
        self.assertAlmostEqual(m, 1.0)
        self.assertAlmostEqual(worst, 1.0)
        self.assertEqual(n, 8)

    def test_worse_for_bigger_change(self):
        a = img(32, 32, lambda x, y: (x * 7 + y * 3) % 255)
        b = img(32, 32, lambda x, y: (x * 7 + y * 3) % 255 + (5 if x < 8 else 0))
        c = img(32, 32, lambda x, y: 255 - (x * 7 + y * 3) % 255)
        sb, sc = ssim.ssim_rows(a, b)[0], ssim.ssim_rows(a, c)[0]
        self.assertLess(sb, 1.0)
        self.assertLess(sc, sb)

    def test_skip(self):
        a = img(16, 8, lambda x, y: x * 10)
        b = img(16, 8, lambda x, y: x * 10 if x < 8 else 0)
        self.assertLess(ssim.ssim_rows(a, b)[0], 1.0)
        self.assertAlmostEqual(ssim.ssim_rows(a, b, skip=lambda cx, cy: cx == 1)[0], 1.0)

    def test_judge_leave_one_out(self):
        import random
        rnd = random.Random(1)
        base = img(64, 32, lambda x, y: (x * 7 + y * 3) % 200)

        def run(still_change=False, moving=10.0):
            im = [list(row) for row in base]
            for y in range(24, 32):                 # the bottom row of windows moves in every run (noise)
                for x in range(64):
                    im[y][x] += rnd.uniform(-moving, moving)
            if still_change:                        # one still window changed
                for y in range(8):
                    for x in range(16, 24):
                        im[y][x] = 255.0
            return im
        names = {'r1': run(), 'r2': run(), 'r3': run(), 'same': run(), 'diff': run(True), 'wild': run(moving=60)}
        for k, v in names.items():
            ssim._cache[(os.path.abspath(k), ssim.STEP)] = (64, 32, v)
        shot = ssim.Shot({k: k for k in names})
        refs = ['r1', 'r2', 'r3']
        same, diff, wild = (ssim.judge(shot, k, refs) for k in ('same', 'diff', 'wild'))
        self.assertEqual(same['verdict'], 'SAME')
        self.assertEqual(same['low']['still'], 0)
        self.assertEqual(diff['verdict'], 'DIFF')
        self.assertEqual(diff['low']['still'], 1)
        self.assertEqual(diff['box'], (32, 0, 48, 16))
        self.assertEqual(wild['verdict'], 'DIFF')       # the moving windows far outside retail's spread
        self.assertEqual(wild['low']['still'], 0)
        self.assertGreater(wild['low']['all'], wild['noise']['all'])
        self.assertEqual(ssim.judge(shot, 'same', ['r1', 'r2'])['verdict'], 'NO NOISE')


def ev(e, h, f=1, **kw):
    return dict(ev=e, h=h, f=f, t=f * 10, **kw)


class Audio3dTest(unittest.TestCase):
    def test_position_before_and_after_start(self):
        events = [ev('position', '-', x=10, y=0, z=5), ev('orientation', '-', fx=0, fy=0, fz=1),
                  ev('load', 't1', snd='a'), ev('volume', 't1', snd='a', v=28),
                  ev('distances', 't1', snd='a', min=500, max=4000), ev('position', 't1', snd='a', x=1, y=2, z=3),
                  ev('start', 't1', snd='a'),
                  ev('load', 't2', snd='b'), ev('start', 't2', f=2, snd='b'), ev('position', 't2', f=2, snd='b', x=4, y=5, z=6),
                  ev('load', 't3', snd='c'), ev('start', 't3', f=3, snd='c'), ev('position', 't3', f=4, snd='c', x=7, y=8, z=9),
                  ev('start', 's1', snd='ui')]
        rows = audio3d.spatial(events)
        self.assertEqual(len(rows), 3)
        self.assertEqual(rows[0], ['a', 10, [1.0, 2.0, 3.0], 28, [500, 4000], [10.0, 0.0, 5.0], [0.0, 0.0, 1.0]])
        self.assertEqual(rows[1][2], [4.0, 5.0, 6.0])     # placed after the start, in the same frame
        self.assertIsNone(rows[2][2])                       # placed only in a later frame: not the start's place

    def test_compare_by_sound(self):
        def row(p, v=28, snd='a'):
            return [snd, 0, p, v, [500, 4000], [0, 0, 0], [0, 0, 1]]
        bs = audio3d.by_sound
        stored = {'r1': bs([row([0, 0, 0], 20), row([5, 0, 0], 30)]), 'r2': bs([row([5, 0, 1], 25), row([0, 0, 2], 35)]),
                  'r3': bs([row([0, 0, 1], 22), row([9, 9, 9], 28, 'b')])}
        verdicts = lambda res: [(r['sound'], r['verdict']) for r in res]   # noqa: E731
        same = audio3d.compare_sounds(bs([row([5, 0, 0.5], 30), row([0, 0, 1], 21)]), stored)
        self.assertEqual(verdicts(same), [('a', 'SAME')])
        far = audio3d.compare_sounds(bs([row([0, 0, 9])]), stored)
        self.assertEqual(verdicts(far)[0], ('a', 'DIFF'))
        vol = audio3d.compare_sounds(bs([row([0, 0, 1], v=40)]), stored)
        self.assertEqual(verdicts(vol)[0], ('a', 'DIFF'))
        self.assertEqual(verdicts(audio3d.compare_sounds(bs([row([0, 0, 0], snd='c')]), stored)),
                         [('a', 'RETAIL-ONLY'), ('c', 'OURS-ONLY')])


if __name__ == '__main__':
    unittest.main()
