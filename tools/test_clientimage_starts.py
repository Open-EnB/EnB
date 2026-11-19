"""Tests for clientimage.py's function starts (D267's CRT-registered starts, N1's P2, run 2026-11-18b):

  crt_registered_starts  no incremental-thunk table slot is a start: the CRT reaches 8 initialisers through their
                         thunk (0x4046BF, 0x4046C4, 0x406384, 0x40A66E, 0x40D189, 0x40FF88, 0x410E79, 0x413EA3);
                         the thunks' targets are starts already
  function_starts        nothing below the first thunk target (the table), so app_inventory.py takes every start
                         below its APP_END without a filter of its own

    python -m unittest discover -s tools -p "test_*.py"

Reads client.exe (ClientImage's default path); skipped when it is not there.
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 're'))

THUNK_SLOTS = (0x4046BF, 0x4046C4, 0x406384, 0x40A66E, 0x40D189, 0x40FF88, 0x410E79, 0x413EA3)


def _image():
    try:
        from clientimage import ClientImage
        return ClientImage()
    except (OSError, FileNotFoundError) as e:
        raise unittest.SkipTest(f'client.exe not readable: {e}')


class CrtRegisteredStarts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.img = _image()
        cls.thunks = cls.img.incremental_thunks()
        cls.crt = cls.img.crt_registered_starts()

    def test_no_thunk_slot(self):
        self.assertFalse(self.crt & set(self.thunks))

    def test_known_slots_dropped_targets_kept(self):
        starts, _ = self.img.function_starts()
        starts = set(starts)
        for slot in THUNK_SLOTS:
            self.assertIn(slot, self.thunks)
            self.assertNotIn(slot, self.crt)
            self.assertNotIn(slot, starts)
            self.assertIn(self.thunks[slot], starts)

    def test_nothing_below_first_target(self):
        starts, thunks = self.img.function_starts()
        self.assertEqual(min(starts), min(thunks.values()))


if __name__ == '__main__':
    unittest.main()
