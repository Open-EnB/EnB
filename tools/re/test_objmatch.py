"""objmatch.py's funclet evidence for short functions (D246 / C21) and for tied non-short bodies (D246 / C35;
python -m unittest tools/re/test_objmatch.py).

The object is not tracked: build/app/obj/attrib_dir_collective.obj in the main tree (or OBJMATCH_OBJS), a
missing object skips the case. AttribModClass's destructor 0x4DBA50 (`mov [ecx], vtable; ret`, too short to
search) is reached only by a `jmp` in an unwind funclet of an exact function of the object; the same state's
funclet in the object jumps to ??1AttribModClass, so short_known credits it with kind `funclet`.
TIES: a destructor with byte-identical copies at many client addresses (D166's tie) is the copy a matching
unwind funclet of an exact function of the object jumps to under its symbol (`funclet_tie`).
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import objmatch  # noqa: E402

MAIN = os.path.normpath(os.path.join(REPO, '..', '..', '..')) if os.sep + '.work' + os.sep in REPO else REPO
OBJS = os.environ.get('OBJMATCH_OBJS', os.path.join(MAIN, 'build', 'app', 'obj'))
CASES = [
    # object, symbol, client address
    ('attrib_dir_collective.obj', '??1AttribModClass@@UAE@XZ', '0x4DBA50'),
]


class FuncletEvidence(unittest.TestCase):
    def test_cases(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        for name, sym, addr in CASES:
            path = os.path.join(OBJS, name)
            with self.subTest(sym=sym):
                if not os.path.exists(path):
                    self.skipTest(f'{path} missing')
                res ={r['symbol']: r for r in objmatch.match_objects(img, [path])}
                self.assertEqual(res[sym]['status'], 'exact')
                self.assertEqual(res[sym]['addresses'], [addr])
                self.assertEqual(res[sym].get('short_known'), 'funclet')
                # the rule's switch: without it the function stays short
                objmatch.USE_FUNCLET_CALLS = False
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_FUNCLET_CALLS = True
                self.assertEqual(res[sym]['status'], 'short')


TIES = [
    # object, symbol, client address (the tie's copies)
    ('dynamic_window_class.obj', '??1?$RefCountPtr@VDynamicWindowElementClass@@@@QAE@XZ', '0x517030'),
]


class FuncletTies(unittest.TestCase):
    def test_ties(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        for name, sym, addr in TIES:
            path = os.path.join(OBJS, name)
            with self.subTest(sym=sym):
                if not os.path.exists(path):
                    self.skipTest(f'{path} missing')
                res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                self.assertEqual(res[sym]['status'], 'exact')
                self.assertEqual(res[sym]['addresses'], [addr])
                self.assertTrue(res[sym].get('funclet_tie'))
                # the rule's switch: without it the copies stay tied
                objmatch.USE_FUNCLET_TIES = False
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_FUNCLET_TIES = True
                self.assertEqual(res[sym]['status'], 'multiple')


if __name__ == '__main__':
    unittest.main()
