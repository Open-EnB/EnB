"""objmatch.py's funclet evidence for short functions (D246 / C21), for tied non-short bodies (D246 / C35) and beyond (CR2)
and pushed-address evidence for short functions (D246 / CR3) and for tied non-short bodies, with the object's own calls' evidence (D246 / CR4;
python -m unittest tools/re/test_objmatch.py).

The object is not tracked: build/app/obj/attrib_dir_collective.obj in the main tree (or OBJMATCH_OBJS), a
missing object skips the case. AttribModClass's destructor 0x4DBA50 (`mov [ecx], vtable; ret`, too short to
search) is reached only by a `jmp` in an unwind funclet of an exact function of the object; the same state's
funclet in the object jumps to ??1AttribModClass, so short_known credits it with kind `funclet`.
TIES: a destructor with byte-identical copies at many client addresses (D166's tie) is the copy a matching
unwind funclet of an exact function of the object jumps to under its symbol (`funclet_tie`).
SHORT_ORDER (D246 / CR1): camera_manager.obj's short getters (`fld [ecx+x]; ret`, too short to search), each the only
own function between two exact own functions in object order, take the one client function between those two
addresses that has their bytes and length (`short_order`).
GAP_ORDER (D246 / CR1): the same alignment when the gap also holds tied own functions: starbase_defs.obj's
String_To_Value overloads (byte-identical copies, D166's tie) between their exact neighbours (`gap_order`).
EDGE_ORDER (D246 / CR6): an own function before an object's first exact own function in object order, tied with a
byte-identical copy in another object's code, is the client function right below that exact one (`edge_order`):
scriptedmovieline.obj's and ui_symbolchat.obj's constructors (copies at 0x56E170 / 0x69AC20).
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



PUSHED = [
    # object, symbol, client address: a short function an exact function of the object pushes (D246 / CR3)
    ('rs_list.obj', '??0WeightInfoStruct@NamedPivotMapClass@@QAE@XZ', '0x45D820'),   # eh vector ctor iterator's arg
    ('cordcon.obj', '?flush@@YAAAVostream@@AAV1@@Z', '0x850850'),                     # an ostream manipulator
]


class PushedEvidence(unittest.TestCase):
    def test_pushed(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        for name, sym, addr in PUSHED:
            path = os.path.join(OBJS, name)
            with self.subTest(sym=sym):
                if not os.path.exists(path):
                    self.skipTest(f'{path} missing')
                res = {r['symbol']: r for r in objmatch.match_objects(img, [path], (0x401000, 0x8C1B50))}
                self.assertEqual(res[sym]['status'], 'exact')
                self.assertEqual(res[sym]['addresses'], [addr])
                self.assertEqual(res[sym].get('short_known'), 'pushed')
                # the rule's switch: without it the function stays short
                objmatch.USE_PUSHED_CALLS = False
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path], (0x401000, 0x8C1B50))}
                finally:
                    objmatch.USE_PUSHED_CALLS = True
                self.assertEqual(res[sym]['status'], 'short')

PUSHED_TIES = [
    # object, symbol, client address: a tied body that exact functions push at one address (D246 / CR4)
    ('ambientsoundmanager.obj',
     '??1?$vector@PAURandomSoundStruct@AmbientSoundManagerClass@@V?$allocator@PAURandomSoundStruct@AmbientSoundManagerClass@@@std@@@std@@QAE@XZ',
     '0x4362E0'),   # an atexit destructor's argument
]


class PushedTies(unittest.TestCase):
    def test_ties(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        for name, sym, addr in PUSHED_TIES:
            path = os.path.join(OBJS, name)
            with self.subTest(sym=sym):
                if not os.path.exists(path):
                    self.skipTest(f'{path} missing')
                res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                self.assertEqual(res[sym]['status'], 'exact')
                self.assertEqual(res[sym]['addresses'], [addr])
                self.assertTrue(res[sym].get('pushed_tie'))
                # the rule's switch: without it the copies stay tied
                objmatch.USE_PUSHED_TIES = False
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_PUSHED_TIES = True
                self.assertEqual(res[sym]['status'], 'multiple')


CALL_TIES = [
    # object, symbol, client address: a tied body the object's own exact functions call at one address (D246 / CR4)
    ('control_keys_manager.obj',
     '?reserve@?$vector@PAU?$_Hashtable_node@U?$pair@$$CBIPAUControlKeyDefStruct@@@std@@@std@@V?$_Bucket_allocator@PAU?$_Hashtable_node@U?$pair@$$CBIPAUControlKeyDefStruct@@@std@@@std@@@2@@std@@QAEXI@Z',
     '0x71F2A0'),   # vector<_Hashtable_node *>::reserve, called by the object's hashtable
    # CR5 / CR7: the funclet round's form, a tie the funclets leave (Create_NEW<CameraClass>, 112 copies)
    ('avatar_pip.obj', '?Create_NEW@@YA?AV?$RefCountPtr@VCameraClass@@@@PAVCameraClass@@@Z', '0x5C4030'),
    # CR1 / CR7: the one address every exact function's calls reach under the symbol
    ('ui_attrb_skills.obj', '?Create_Get@@YA?AV?$RefCountPtr@VRenderObjClass@@@@PAVRenderObjClass@@@Z', '0x474080'),
]


class CallTies(unittest.TestCase):
    def test_ties(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        for name, sym, addr in CALL_TIES:
            path = os.path.join(OBJS, name)
            with self.subTest(sym=sym):
                if not os.path.exists(path):
                    self.skipTest(f'{path} missing')
                res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                self.assertEqual(res[sym]['status'], 'exact')
                self.assertEqual(res[sym]['addresses'], [addr])
                self.assertTrue(res[sym].get('call_tie'))
                # the rule's switch: without it the copies stay tied
                objmatch.USE_CALL_TIES = False
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_CALL_TIES = True
                self.assertEqual(res[sym]['status'], 'multiple')
# CR2 (run 2026-11-16): object list, symbol, client address, the result key it sets, the switch that gives it
BEYOND = [
    # two template destructors with one body, linker-folded at 0x700BC0: both credited by the funclet round
    (['client_lego_impl.obj'], '??1?$RefCountPtr@VLegoClass@@@@QAE@XZ', '0x700BC0', 'funclet_tie',
     'USE_FUNCLET_FOLDED'),
    # a parent not exact: its FuncInfo located by its whole unwind map (or its named address)
    (['lookup_table.obj'], '??1LookupTableClass@@UAE@XZ', '0x4EC6F0', 'short_known', 'USE_FUNCLET_NOTEXACT'),
    # CR5: a not-exact parent whose unwind map matches several client FuncInfos, the one placed between its exact
    # neighbours in object order used
    (['extrap.obj'], '??1Unknown710030Class@@UAE@XZ', '0x7109B0', 'short_known', 'USE_FUNCLET_PLACED'),
]


class FuncletBeyond(unittest.TestCase):
    def test_beyond(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        for names, sym, addr, key, switch in BEYOND:
            paths = [os.path.join(OBJS, n) for n in names]
            with self.subTest(sym=sym):
                if not all(os.path.exists(p) for p in paths):
                    self.skipTest(f'{paths} missing')
                res = {r['symbol']: r for r in objmatch.match_objects(img, paths)}
                self.assertEqual(res[sym]['status'], 'exact')
                self.assertEqual(res[sym]['addresses'], [addr])
                self.assertTrue(res[sym].get(key))
                setattr(objmatch, switch, False)
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, paths)}
                finally:
                    setattr(objmatch, switch, True)
                # CR7: a later round (CR4's own-call tie) may still credit it, but not by this evidence
                self.assertFalse(res[sym]['status'] == 'exact' and res[sym]['addresses'] == [addr]
                                 and res[sym].get(key))


class FuncletNamed(unittest.TestCase):
    def test_reach(self):
        # ~StringClass reached at 0x431C80 and at 0x67F660, which symbols.json names FontSoundStruct's destructor:
        # the named one is dropped; a single reach stays (a linker fold is named with one of its symbols only)
        sym = '??1StringClass@@QAE@XZ'
        self.assertEqual(objmatch.funclet_reach({sym: {0x431C80, 0x67F660}}), {sym: {0x431C80}})
        self.assertEqual(objmatch.funclet_reach({sym: {0x67F660}}), {sym: {0x67F660}})
        objmatch.USE_FUNCLET_NAMED = False
        try:
            self.assertEqual(objmatch.funclet_reach({sym: {0x431C80, 0x67F660}}), {sym: {0x431C80, 0x67F660}})
        finally:
            objmatch.USE_FUNCLET_NAMED = True


def _check(test, cases, switch, flag, region=None, off_status='multiple', also_off=()):
    img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
    for name, sym, addr in cases:
        path = os.path.join(OBJS, name)
        with test.subTest(sym=sym):
            if not os.path.exists(path):
                test.skipTest(f'{path} missing')
            res = {r['symbol']: r for r in objmatch.match_objects(img, [path], region)}
            test.assertEqual(res[sym]['status'], 'exact')
            test.assertEqual(res[sym]['addresses'], [addr])
            test.assertTrue(res[sym].get(flag))
            # the rule's switch: without it the function stays unresolved
            for sw in (switch,) + tuple(also_off):
                setattr(objmatch, sw, False)
            try:
                res = {r['symbol']: r for r in objmatch.match_objects(img, [path], region)}
            finally:
                for sw in (switch,) + tuple(also_off):
                    setattr(objmatch, sw, True)
            test.assertEqual(res[sym]['status'], off_status)


SHORT_ORDER = [
    ('camera_manager.obj', '?Get_Heading@CameraManagerClass@@QAEMXZ', '0x79B390'),
    ('camera_manager.obj', '?Get_Distance@CameraManagerClass@@QAEMXZ', '0x79B3D0'),
]
GAP_ORDER = [
    ('starbase_defs.obj', '?String_To_Value@@YA_NPBDPAW4StarbaseFacilityEnum@@@Z', '0x84A750'),
    ('starbase_defs.obj', '?String_To_Value@@YA_NPBDPAW4StarbaseArtEnum@@@Z', '0x84A930'),
]
EDGE_ORDER = [
    ('scriptedmovieline.obj', '??0ScriptedMovieLineClass@@QAE@XZ', '0x7FFC50'),
    ('ui_symbolchat.obj', '??0UISymbolChatOptionStruct@@QAE@XZ', '0x64A330'),
]
APP_REGION = (0x401000, 0x8C1B50)


class ShortOrder(unittest.TestCase):
    def test_cases(self):
        _check(self, SHORT_ORDER, 'USE_SHORT_ORDER', 'short_order', off_status='short')


class GapOrder(unittest.TestCase):
    def test_cases(self):
        # with the gap round off, the edge round (USE_EDGE_ORDER) places them at the same addresses
        _check(self, GAP_ORDER, 'USE_GAP_ORDER', 'gap_order', APP_REGION, also_off=('USE_EDGE_ORDER',))


class EdgeOrder(unittest.TestCase):
    def test_cases(self):
        _check(self, EDGE_ORDER, 'USE_EDGE_ORDER', 'edge_order', APP_REGION)


class XcuSubrun(unittest.TestCase):
    """C15: soundlib's first window holds sectorsound's guard initialiser 0x43D2F0 too (5 table entries, 4
    initialisers); the one consecutive sub-run that fits is soundlib's. Synthetic fits: the guard entry fits no
    soundlib initialiser."""
    RUN = [0x43D2F0, 0x43D320, 0x43E780, 0x43E8B0, 0x441880]

    def test_subrun(self):
        fits = lambda sub: sub[0] != 0x43D2F0
        self.assertEqual(objmatch.xcu_subrun(self.RUN, 4, fits), self.RUN[1:])
        self.assertIsNone(objmatch.xcu_subrun(self.RUN, 4, lambda sub: True))     # two fit: none credited
        self.assertIsNone(objmatch.xcu_subrun(self.RUN, 6, lambda sub: True))
        objmatch.USE_XCU_SUBRUN = False
        try:
            self.assertIsNone(objmatch.xcu_subrun(self.RUN, 4, fits))
            self.assertEqual(objmatch.xcu_subrun(self.RUN[1:], 4, fits), self.RUN[1:])
        finally:
            objmatch.USE_XCU_SUBRUN = True

if __name__ == '__main__':
    unittest.main()
