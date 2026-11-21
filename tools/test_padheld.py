"""Tests for D279 (2026-11-21): pairs_cmp.py's pad-held class.

  A lost pair that docs/data/pad-held.json lists (exact only inside a window of its file's VC6 symbol pad) is
  reported under `pad_held`, not under `lost` / `lost_outright`: no landing sweeps or sets a pad, the handoff does.
  Any other lost pair of the same file is still a loss. The committed list parses and names co / sclient only.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only.
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))
import pairs_cmp  # noqa: E402


def led(files):
    return {'app': {'files': {f: {'in_tree': list(a)} for f, a in files.items()}}}


class PadHeld(unittest.TestCase):
    HELD = {('app', 'sclient', 0x758210), ('app', 'co', 0x6FB1E0)}

    def cmp(self, before, after):
        return pairs_cmp.compare(led(before), led(after), corrections=[], pad_held=self.HELD)

    def test_listed_pair_is_no_loss(self):
        r = self.cmp({'sclient': ['0x758210', '0x7405E0']}, {'sclient': ['0x7405E0']})
        self.assertEqual(r['pad_held'], [('app', 'sclient', '0x758210')])
        self.assertEqual(r['lost'], [])
        self.assertEqual(r['lost_outright'], [])

    def test_other_pair_of_the_file_is_still_lost(self):
        r = self.cmp({'sclient': ['0x758210', '0x7405E0']}, {'sclient': []})
        self.assertEqual(r['pad_held'], [('app', 'sclient', '0x758210')])
        self.assertEqual(r['lost_outright'], ['0x7405E0'])

    def test_same_address_in_an_unlisted_file_is_lost(self):
        r = self.cmp({'other': ['0x758210']}, {'other': []})
        self.assertEqual(r['pad_held'], [])
        self.assertEqual(r['lost_outright'], ['0x758210'])

    def test_extension_is_ignored(self):
        held, rest = pairs_cmp.split_pad_held([('app', 'co.cpp', '0x6FB1E0'), ('app', 'co.cpp', '0x6F5F60')], self.HELD)
        self.assertEqual((held, rest), ([('app', 'co.cpp', '0x6FB1E0')], [('app', 'co.cpp', '0x6F5F60')]))

    def test_committed_list(self):
        ph = pairs_cmp.load_pad_held()
        self.assertTrue(ph)
        self.assertEqual({(lib, f) for lib, f, _ in ph}, {('app', 'co'), ('app', 'sclient')})


if __name__ == '__main__':
    unittest.main()
