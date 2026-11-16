"""rttile.py: the tiling of an open runtime-library entry and the counts coverage.md shows
(python -m unittest tools/re/test_rttile.py). No client, no objects: byte strings and a temp file."""
import collections
import json
import os
import re
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rttile  # noqa: E402


def cands(*items):
    by = collections.defaultdict(list)
    for pat, lib, obj in items:
        by[pat[0]].append((len(pat), re.compile(re.escape(pat), re.S), lib, obj))
    for k in by:
        by[k].sort(key=lambda t: -t[0])
    return by


class Tile(unittest.TestCase):
    def test_two_functions_glued(self):
        by = cands((b'\x55\x8b\xec\x5d\xc3', 'LIBCMT', 'a.obj'), (b'\x33\xc0\xc2\x04\x00', 'LIBCMT', 'b.obj'))
        funcs, covered = rttile.tile(b'\x55\x8b\xec\x5d\xc3\x33\xc0\xc2\x04\x00', by)
        self.assertEqual(covered, 10)
        self.assertEqual([f[:2] for f in funcs], [[0, 5], [5, 5]])

    def test_untiled_bytes_are_not_covered(self):
        by = cands((b'\x55\x8b\xec\x5d\xc3', 'LIBCMT', 'a.obj'))
        funcs, covered = rttile.tile(b'\x90\x55\x8b\xec\x5d\xc3', by)
        self.assertEqual((covered, len(funcs)), (5, 1))

    def test_short_candidate_only_when_it_ends_the_entry(self):
        by = cands((b'\xc3', 'LIBCMT', 'r.obj'))
        self.assertEqual(rttile.tile(b'\xc3', by)[1], 1)      # the whole entry
        self.assertEqual(rttile.tile(b'\xc3\x90', by)[1], 0)   # a 1-byte function in the middle proves nothing


class Counts(unittest.TestCase):
    def test_named_entries_leave_the_count(self):
        d = {'0x100': {'size': 10, 'covered': 10, 'lib': 'LIBCMT'},
             '0x200': {'size': 8, 'covered': 3, 'lib': 'LIBCMT'},
             '0x300': {'size': 6, 'covered': 6, 'lib': 'd3dx8'}}
        with tempfile.TemporaryDirectory() as t:
            p = os.path.join(t, 'x.json')
            json.dump(d, open(p, 'w'))
            self.assertEqual(rttile.counts(p, {0x300}), {'LIBCMT': [2, 1, 18, 10]})
            self.assertIsNone(rttile.counts(os.path.join(t, 'none.json'), set()))


if __name__ == '__main__':
    unittest.main()
