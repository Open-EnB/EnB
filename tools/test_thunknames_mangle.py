"""Tests for tools/re/thunknames/mangle.py: the one rule for finding and renaming a class inside an MSVC 6 decorated
name (two bugs of 2026-11-26 came from weaker copies of it; docs/workflow.md §3.6, Thunk-order names).

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no client.exe, no VMs, no network.
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 're', 'thunknames'))

import mangle  # noqa: E402


class TestMangle(unittest.TestCase):
    def test_selftest_cases(self):
        self.assertGreaterEqual(mangle._selftest(), 8)

    def test_special_members_and_type_codes(self):
        self.assertEqual(mangle.rename('??_EFoo@@UAEPAXI@Z', 'Foo', 'Bar'), '??_EBar@@UAEPAXI@Z')
        self.assertEqual(mangle.rename('?Get@C@@QAE?AVFoo@@XZ', 'Foo', 'Bar'), '?Get@C@@QAE?AVBar@@XZ')

    def test_no_substring_or_method_rename(self):
        self.assertEqual(mangle.rename('?F@XFoo@@QAEXXZ', 'Foo', 'Bar'), '?F@XFoo@@QAEXXZ')
        self.assertEqual(mangle.rename('?Foo@C@@QAEXXZ', 'Foo', 'Bar'), '?Foo@C@@QAEXXZ')

    def test_rename_all_and_class_of(self):
        m = {'Old': 'New', 'Elem': 'Element'}
        self.assertEqual(mangle.rename_all('?Add@Old@@QAEXABV?$RefCountPtr@VElem@@@@@Z', m),
                         '?Add@New@@QAEXABV?$RefCountPtr@VElement@@@@@Z')
        self.assertEqual(mangle.class_of('??1Foo@@UAE@XZ'), 'Foo')
        self.assertIsNone(mangle.class_of('_sprintf'))


if __name__ == '__main__':
    unittest.main()
