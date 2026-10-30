"""explain.py's classes on known near misses (python -m unittest tools/re/test_explain.py, or run it).

The objects are not tracked (never objects in the tree): Make_Screen_Shot's is builder C's U form,
`.work/c/mss_out/U/ww3d.obj`; the step-8 ones are the tree's app/common tcpconn.cpp, dbgconn.cpp,
connect.cpp and memconn.cpp as of cf646af, compiled into EXPLAIN_OBJS (default .work/tools4/objs)
by `importer.compile_and_match` (see `.work/tools4/build_objs.py`). A missing object skips its case.
When one of these functions becomes exact, drop its case (its class is then `identical`).
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import explain  # noqa: E402

OBJS = os.environ.get('EXPLAIN_OBJS', os.path.join(REPO, '.work', 'tools4', 'objs'))
CASES = [
    # object, symbol, client address, expected class
    (os.path.join(REPO, '.work', 'c', 'mss_out', 'U', 'ww3d.obj'), 'Make_Screen_Shot', '0x9182C0', 'register-only'),
    (os.path.join(REPO, 'build', 'ww3d2', 'obj', 'dx8wrapper.obj'), 'Create_Device', '0x921EF0', 'identical'),
    (os.path.join(OBJS, 'dbgconn.obj'), '?Count@RateConnectionClass', '0x824EC0', 'order-only'),
    (os.path.join(OBJS, 'connect.obj'), 'Remove_Dead', '0x822B50', 'inline-budget'),
    (os.path.join(OBJS, 'dbgconn.obj'), '??0TimedLogConnectionClass', '0x824540', 'inline-budget'),
    (os.path.join(OBJS, 'tcpconn.obj'), '?Listen@TCPListenerClass', '0x82A470', 'shape'),
    (os.path.join(OBJS, 'memconn.obj'), 'Unknown_30@MemoryConnectionClass', '0x826D00', 'shape'),
]


class Classes(unittest.TestCase):
    def test_cases(self):
        for obj, sym, addr, want in CASES:
            with self.subTest(sym=sym, addr=addr):
                if not os.path.exists(obj):
                    self.skipTest(f'{obj} missing')
                result, _ = explain.explain(obj, sym, addr)
                self.assertEqual(result['class'], want, result['reason'])


if __name__ == '__main__':
    unittest.main()
