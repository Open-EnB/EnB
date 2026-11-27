"""Unit tests for texlife.py (the draw logs' texture_life counts): python -m unittest tools/harness/test_texlife.py"""
import json
import os
import shutil
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import texlife  # noqa: E402


def kind(w, h, fmt, created, released, levels=1, usage=0, pool=1):
    return {'w': w, 'h': h, 'levels': levels, 'format': fmt, 'usage': usage, 'pool': pool,
            'created': created, 'released': released}


class TexLife(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.dir)

    def put(self, name, frame, kinds=None):
        d = {'format': 'd3d8shot-drawlog 1', 'shot': name, 'frame': frame}
        if kinds is not None:
            c = sum(k['created'] for k in kinds)
            r = sum(k['released'] for k in kinds)
            d['texture_life'] = {'created': c, 'released': r, 'live': c - r, 'untracked': 0, 'kinds_lost': 0,
                                 'kinds': kinds}
        with open(os.path.join(self.dir, f'shot_{name}_{frame:05d}.json'), 'w') as f:
            json.dump(d, f)

    def test_moved_kinds_only_in_frame_order(self):
        self.put('b_closed', 200, [kind(128, 128, 23, 3, 0), kind(64, 64, 21, 10, 2)])
        self.put('a_open', 100, [kind(128, 128, 23, 3, 0), kind(64, 64, 21, 9, 2)])
        self.put('c_away', 300, [kind(128, 128, 23, 3, 3), kind(64, 64, 21, 10, 2)])
        rows = texlife.rows(self.dir)
        self.assertEqual([r['name'] for r in rows], ['a_open', 'b_closed', 'c_away'])
        self.assertEqual(len(rows[0]['moved']), 2)			# the first shot: every kind since the start
        self.assertEqual(rows[1]['moved'], [((64, 64, 1, 21, 0, 1), 1, 0, 8)])
        self.assertEqual(rows[2]['moved'], [((128, 128, 1, 23, 0, 1), 0, 3, 0)])
        self.assertEqual(rows[2]['live'], 8)

    def test_kind_filter_and_format(self):
        self.put('a', 1, [kind(128, 128, 23, 2, 0), kind(128, 128, 21, 5, 0), kind(256, 256, 23, 1, 0)])
        only = texlife.rows(self.dir, kinds=[texlife.parse_kind('128x128:23')])
        self.assertEqual([m[0][3] for m in only[0]['moved']], [23])
        both = texlife.rows(self.dir, kinds=[texlife.parse_kind('128x128')])
        self.assertEqual(sorted(m[0][3] for m in both[0]['moved']), [21, 23])

    def test_old_draw_log_without_counts(self):
        self.put('old', 5)
        self.assertEqual(texlife.rows(self.dir), [{'name': 'old', 'frame': 5, 'counts': False}])
        self.assertIn('no counts', '\n'.join(texlife.report(self.dir)))

    def test_parse_kind_rejects(self):
        with self.assertRaises(Exception):
            texlife.parse_kind('128')


if __name__ == '__main__':
    unittest.main()
