"""Tests for tools/re/relaudit.py's suspect selection (no client.exe needed).

    python -m unittest tools.test_relaudit
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 're'))
import relaudit  # noqa: E402


class Suspects(unittest.TestCase):
    def test_minority_functions_are_suspects(self):
        seen = {'?G@@3HA': {0xC41580: [('f', 0x7014B0), ('g', 0x731ED0), ('h', 0x729A10)],
                            0xC415A4: [('init', 0x707A20)]},
                '?H@@3HA': {0xC41600: [('a', 0x561C40)]}}
        self.assertEqual(relaudit.suspects(seen), [(0x707A20, '?G@@3HA', 0xC415A4, 0xC41580, 3)])

    def test_one_function_reading_twice_counts_once(self):
        seen = {'?G@@3HA': {1: [('f', 0x10), ('f', 0x10), ('f', 0x10)], 2: [('g', 0x20), ('h', 0x30)]}}
        self.assertEqual(relaudit.suspects(seen), [(0x10, '?G@@3HA', 1, 2, 2)])

    def test_only_filters_symbols(self):
        seen = {'?A@@3HA': {1: [('f', 0x10)], 2: [('g', 0x20), ('h', 0x30)]}}
        self.assertEqual(relaudit.suspects(seen, only='?B'), [])


if __name__ == '__main__':
    unittest.main()
