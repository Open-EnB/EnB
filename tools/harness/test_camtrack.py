"""Unit tests of camtrack.py (python -m unittest tools/harness/test_camtrack.py)."""
import math
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import camtrack  # noqa: E402


def view_at(x, y, z, yaw=0.0):
    """A view matrix (row vectors) of a camera at x,y,z looking along yaw degrees in the x-y plane, z up."""
    c, s = math.cos(math.radians(yaw)), math.sin(math.radians(yaw))
    look = (c, s, 0.0)
    right = (s, -c, 0.0)
    up = (0.0, 0.0, 1.0)
    # columns: right, up, -look
    cols = (right, up, (-look[0], -look[1], -look[2]))
    r = [[cols[j][i] for j in range(3)] for i in range(3)]
    p = (x, y, z)
    t = [-sum(p[i] * r[i][j] for i in range(3)) for j in range(3)]
    return [r[0][0], r[0][1], r[0][2], 0, r[1][0], r[1][1], r[1][2], 0, r[2][0], r[2][1], r[2][2], 0,
            t[0], t[1], t[2], 1]


ORTHO = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -640, 1]


def line(t, frame, *views):
    return f'{int(t * 1000)} {t:.3f} {frame} {len(views)} ' + ' '.join(f'{v:.5f}' for vw in views for v in vw)


def track(points):
    """[(t, frame, pose)] from [(t, x, y, z, yaw)]."""
    return [camtrack.parse_track_line(line(t, i, ORTHO, view_at(x, y, z, yaw))) for i, (t, x, y, z, yaw)
            in enumerate(points)]


class PoseTest(unittest.TestCase):
    def test_pose_round_trip(self):
        pos, yaw, pitch = camtrack.pose(view_at(10, -20, 5, 30))
        for a, b in zip(pos, (10, -20, 5)):
            self.assertAlmostEqual(a, b, places=4)
        self.assertAlmostEqual(yaw, 30, places=4)
        self.assertAlmostEqual(pitch, 0, places=4)

    def test_real_views_in_order(self):
        t, fr, ps = camtrack.parse_track_line(line(1.5, 7, ORTHO, view_at(1, 2, 3, 90), view_at(9, 9, 9)))
        self.assertEqual((t, fr, len(ps)), (1.5, 7, 2))
        self.assertAlmostEqual(ps[0][0][0], 1, places=4)
        self.assertAlmostEqual(ps[0][1], 90, places=4)
        self.assertAlmostEqual(ps[1][0][2], 9, places=4)

    def test_only_ortho_is_no_3d_view(self):
        self.assertEqual(camtrack.parse_track_line(line(1.0, 1, ORTHO))[2], [])

    def test_malformed_line(self):
        self.assertIsNone(camtrack.parse_track_line('1 2.0 3 2 1 2 3'))


class AnchorTest(unittest.TestCase):
    EV = [(1.0, 'antprox', 'x = received ACTION'), (2.0, 'antprox', 'x = sent CAMERA_CONTROL'),
          (3.0, 'antprox', 'x = received ACTION'), (4.0, 'proxy', 'x = received ACTION')]

    def test_spec(self):
        self.assertEqual(camtrack.parse_spec('antprox:received ACTION$#3@-2,5'),
                         ('antprox', 'received ACTION$', 3, -2.0, 5.0))
        self.assertEqual(camtrack.parse_spec('any:FOO'), ('any', 'FOO', 1, -1.0, 8.0))

    def test_kth_match_of_a_source(self):
        self.assertEqual(camtrack.find_anchor(self.EV, 'antprox:received ACTION$#2'), 3.0)
        self.assertEqual(camtrack.find_anchor(self.EV, 'any:received ACTION$#3'), 4.0)
        self.assertIsNone(camtrack.find_anchor(self.EV, 'antprox:received ACTION$#3'))

    def test_scenario_specs_parse(self):
        for name, rows in camtrack.SCENARIOS.items():
            for n, spec in rows:
                src, rx, k, lo, hi = camtrack.parse_spec(spec)
                self.assertIn(src, ('antprox', 'proxy', 'msg', 'any'), name)
                self.assertLess(lo, hi)


class CompareTest(unittest.TestCase):
    EV = [(10.0, 'antprox', '= received ACTION')]

    def move(self, held, dt=0.0, frames=0.03):
        """Held at (0,0,0) until the anchor, a 1 s move to (100,0,0), then held at HELD."""
        pts = []
        t = 8.0
        while t < 17.0:
            u = t - 10.0 - dt
            if u < 0:
                p = (0, 0, 0)
            elif u < 1:
                p = (100 * u, 0, 0)
            else:
                p = held
            pts.append((t, p[0], p[1], p[2], 0.0))
            t += frames
        return track(pts)

    def runs(self, ours_held, ours_dt=0.0):
        return [('R1', True, self.move((100, 0, 0)), self.EV), ('R2', True, self.move((100, 0, 0), 0.01, 0.033), self.EV),
                ('O1', False, self.move(ours_held, ours_dt, 0.031), self.EV)]

    def test_same(self):
        r = camtrack.compare_anchor(self.runs((100, 0, 0)), 'antprox:received ACTION$@-1,6')
        self.assertEqual(r['verdict'], 'SAME', r)
        self.assertEqual(r['runs']['O1']['views'][0]['verdict'], 'SAME')

    def test_spread_is_local(self):
        # retail varies by 50 units before the anchor (an arrival place), not where it holds: ours off by 0.5 there
        # is still a difference
        def mv(y0, held):
            pts, t = [], 8.0
            while t < 17.0:
                p = (0, y0, 0) if t < 10.0 else held
                pts.append((t, p[0], p[1], p[2], 0.0))
                t += 0.03
            return track(pts)
        runs = [('R1', True, mv(0, (100, 0, 0)), self.EV), ('R2', True, mv(50, (100, 0, 0)), self.EV),
                ('O1', False, mv(20, (100, 0.5, 0)), self.EV)]
        r = camtrack.compare_anchor(runs, 'antprox:received ACTION$@-1,6')
        self.assertEqual(r['verdict'], 'DIFF')
        self.assertGreater(r['first_difference']['t'], 0)

    def test_held_place_off_by_half_a_unit_is_a_difference(self):
        r = camtrack.compare_anchor(self.runs((100, 0.5, 0)), 'antprox:received ACTION$@-1,6')
        self.assertEqual(r['verdict'], 'DIFF')
        fd = r['first_difference']
        self.assertEqual(fd['field'], 'y')
        self.assertAlmostEqual(fd['ours'], 0.5, places=3)

    def test_plateaus(self):
        pl = camtrack.plateaus(camtrack.window(self.move((100, 0, 0)), 10.0, -1, 6, 0))
        self.assertEqual(len(pl), 2)
        self.assertAlmostEqual(pl[1][2][0][0], 100, places=3)

    def test_missing_view_on_ours(self):
        ours = [(t, fr, []) for t, fr, ps in self.move((100, 0, 0))]
        runs = self.runs((100, 0, 0))[:2] + [('O1', False, ours, self.EV)]
        r = camtrack.compare_anchor(runs, 'antprox:received ACTION$@-1,6')
        self.assertEqual(r['verdict'], 'DIFF')

    def test_no_anchor_in_retail(self):
        r = camtrack.compare_anchor([('R1', True, self.move((100, 0, 0)), [])], 'antprox:received ACTION$')
        self.assertEqual(r['verdict'], 'NO RETAIL')


class ShipTest(unittest.TestCase):
    def log(self, *places):
        w = lambda p: [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, p[0], p[1], p[2], 1]
        return {'views': [ORTHO, view_at(0, 0, 0)], 'draws': [{'i': i, 'world': w(p)} for i, p in enumerate(places)]}

    def test_moving_centre_skips_still_and_far_draws(self):
        still = (100.0, 0.0, 0.0)
        log = self.log(still, (500, 10, 0), (502, 12, 0), (504, 14, 0), (90000, 0, 0))
        ref = self.log(still, (900, 10, 0))
        self.assertEqual(camtrack.moving_centre(log, ref), (502, 12, 0))

    def test_nothing_moved(self):
        log = self.log((1, 2, 3))
        self.assertIsNone(camtrack.moving_centre(log, log))


if __name__ == '__main__':
    unittest.main()
