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
CONTRIB_TIES (D246 / CR8): a tied COMDAT whose one survivor lies in its holder's contribution (from its first exact own
function to the next object's, at most RANGE_GAP past its last): px_process.obj's vector<PxProcessClass *> helpers
right after its code, against byte-identical copies in other files' code (`contrib`).
Its short COMDATs (USE_CONTRIB_SHORTS): _Destroy 0x455FE0 there, at the first holder's verified start holding its bytes.
GAP_EDGE (D246 / EX): camera_manager.obj's short getter 0x79B4D0, right after the exact setter 0x79B4B0 in a gap that
also holds the not-exact 0x79B4F0 (short_order's count test fails), takes the next verified start after the setter's
end (`gap_edge`). USE_EX_GAP_KNOWN: boolpars.obj's destructor and Set_Callbacks between the exact deleting destructor
0x49A3C0 and the not-exact Parse, which symbols.json places at 0x49A430. USE_EX_GAP_PADDED: delta_interpolator.obj's
Get_Change 0x4A6360 (nothing calls it, so no verified start) right after the destructor 0x4A6330's padding.
CK_CALLSEQ (D246 / CK): co_data.obj's ClientObjectDataAccessClass<RadarDataClass>::Get, not exact, is located at 0x79E300 by
Get_Radar_Data's call; its calls in order give the tied hashtable<..RadarDataClass..>::find_or_insert 0x7A0EC0 (five
byte-identical instances), whose own calls then give its resize 0x7A3B50 (`ck_callseq`).
CK_CALLSEQ_VTBL (D246 / CK2V): sclient.obj's MemberCallback<CameraModuleClass, SClient, ..CameraModuleSignal..> vtable,
placed in the client by its slots the run knows, gives its short Sender_Type 0x7593E0 and Type 0x759400 their slot
addresses (`ck_callseq`).
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
                objmatch.USE_CONTRIB_SHORTS = False     # CR8: its contribution places it too
                objmatch.USE_CR1_NAMED_TARGETS = False  # CR1: its named targets place it too
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_FUNCLET_CALLS = True
                    objmatch.USE_CONTRIB_SHORTS = True
                    objmatch.USE_CR1_NAMED_TARGETS = True
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
                objmatch.USE_CONTRIB_SHORTS = False     # CR8: its contribution places it too
                objmatch.USE_CR1_NAMED_TARGETS = False  # CR1: its named targets place it too
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path], (0x401000, 0x8C1B50))}
                finally:
                    objmatch.USE_PUSHED_CALLS = True
                    objmatch.USE_CONTRIB_SHORTS = True
                    objmatch.USE_CR1_NAMED_TARGETS = True
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
                objmatch.USE_CR1_NAMED_TARGETS = False  # CR1: its named targets place it too
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_CALL_TIES = True
                    objmatch.USE_CR1_NAMED_TARGETS = True
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
CONTRIB_TIES = [
    ('px_process.obj', '?_Ucopy@?$vector@PAVPxProcessClass@@V?$allocator@PAVPxProcessClass@@@std@@@std@@IAEPAPAVPxProcessClass@@PBQAV3@0PAPAV3@@Z', '0x456000'),
    ('px_process.obj', '?_Construct@std@@YAXPAPAVPxProcessClass@@ABQAV2@@Z', '0x456080'),
]
CONTRIB_SHORTS = [
    ('px_process.obj', '?_Destroy@?$vector@PAVPxProcessClass@@V?$allocator@PAVPxProcessClass@@@std@@@std@@IAEXPAPAVPxProcessClass@@0@Z', '0x455FE0'),
]
GAP_EDGE = [
    ('camera_manager.obj', '?Unknown_79B4D0@CameraManagerClass@@QAE_NXZ', '0x79B4D0'),
    ('delta_interpolator.obj', '?Get_Change@DeltaInterpolatorClass@@QBEMXZ', '0x4A6360'),
    ('boolpars.obj', '??1BoolParserClass@@UAE@XZ', '0x49A3F0'),
    ('boolpars.obj', '?Set_Callbacks@BoolParserClass@@QAEXPAV?$hash_map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVBoolEvaluatorClass@@U?$hash@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@U?$equal_to@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVBoolEvaluatorClass@@@2@@std@@@Z', '0x49A410'),
]
CK_CALLSEQ = [
    ('co_data.obj', '?find_or_insert@?$hashtable@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@PAVClientObjectClass@@U?$hash@PAVClientObjectClass@@@2@U?$_Select1st@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@@2@U?$equal_to@PAVClientObjectClass@@@2@V?$allocator@PAVRadarDataClass@@@2@@std@@QAEAAU?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@2@ABU32@@Z', '0x7A0EC0'),
    ('co_data.obj', '?resize@?$hashtable@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@PAVClientObjectClass@@U?$hash@PAVClientObjectClass@@@2@U?$_Select1st@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@@2@U?$equal_to@PAVClientObjectClass@@@2@V?$allocator@PAVRadarDataClass@@@2@@std@@QAEXI@Z', '0x7A3B50'),
]
APP_REGION = (0x401000, 0x8C1B50)


class ShortOrder(unittest.TestCase):
    def test_cases(self):
        # CR2: cr2_tiling places them too (the gap's bytes tiled)
        _check(self, SHORT_ORDER, 'USE_SHORT_ORDER', 'short_order', off_status='short', also_off=('USE_CR2_TILING',))


class GapOrder(unittest.TestCase):
    def test_cases(self):
        # with the gap round off, the edge round (USE_EDGE_ORDER) and the gap-edge round (USE_EX_GAP_EDGE) place them
        # at the same addresses
        _check(self, GAP_ORDER, 'USE_GAP_ORDER', 'gap_order', APP_REGION, also_off=('USE_EDGE_ORDER', 'USE_EX_GAP_EDGE', 'USE_CR2_TILING',
                                                                                     'USE_CR2_DATA_AGREE'))


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


class ContribTies(unittest.TestCase):
    def test_cases(self):
        _check(self, CONTRIB_TIES, 'USE_CONTRIB_TIES', 'contrib', APP_REGION)


class ContribShorts(unittest.TestCase):
    def test_cases(self):
        _check(self, CONTRIB_SHORTS, 'USE_CONTRIB_SHORTS', 'contrib', APP_REGION, off_status='short')


class GapEdge(unittest.TestCase):
    def test_cases(self):
        _check(self, GAP_EDGE, 'USE_EX_GAP_EDGE', 'gap_edge', APP_REGION, off_status='short',
               also_off=('USE_CR1_NAMED_TARGETS',))   # CR1: the named targets place it too


CR3_DBGCONN_HASH_MAP_DTOR = '??1?$hash_map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@UPacketStatsStruct@@U?$hash@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@U?$equal_to@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@UPacketStatsStruct@@@2@@std@@QAE@XZ'


class Cr3GapHolder(unittest.TestCase):
    """D246 (CR3): dbgconn.obj's ~hash_map<string, PacketStatsStruct> has copies 0x42B540 and 0x823EA0; 0x823EA0 lies
    between connect.obj's last exact own function and dbgconn.obj's first, and only dbgconn holds the symbol."""
    def test_gap(self):
        paths = [os.path.join(OBJS, n) for n in ('connect.obj', 'dbgconn.obj')]
        if not all(os.path.exists(p) for p in paths):
            self.skipTest('connect.obj / dbgconn.obj missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        res = {r['symbol']: r for r in objmatch.match_objects(img, paths, APP_REGION)}
        r = res[CR3_DBGCONN_HASH_MAP_DTOR]
        self.assertEqual((r['status'], r['addresses'], r.get('cr3_gap')), ('exact', ['0x823EA0'], True))
        objmatch.USE_CR3_GAP_HOLDER = False
        try:
            res = {r['symbol']: r for r in objmatch.match_objects(img, paths, APP_REGION)}
        finally:
            objmatch.USE_CR3_GAP_HOLDER = True
        self.assertEqual(res[CR3_DBGCONN_HASH_MAP_DTOR]['status'], 'multiple')


class CkCallseq(unittest.TestCase):
    def test_cases(self):
        # CR6's named callers reach the same copies from the same located caller, less strictly: off as well
        _check(self, CK_CALLSEQ, 'USE_CK_CALLSEQ', 'ck_callseq', APP_REGION, also_off=('USE_CR6_NAMED_CALLERS',))


CK_CALLSEQ_VTBL = [
    ('sclient.obj', '?Sender_Type@?$MemberCallback@VCameraModuleClass@@VSClient@@P82@AEXPAVCameraModuleSignal@@PAV1@@ZV3@@SignalBaseClass@@UBEABVtype_info@@XZ', '0x7593E0'),
    ('sclient.obj', '?Type@?$MemberCallback@VCameraModuleClass@@VSClient@@P82@AEXPAVCameraModuleSignal@@PAV1@@ZV3@@SignalBaseClass@@UBEABVtype_info@@XZ', '0x759400'),
]


CR1_NAMED_TARGETS = [
    ('selector.obj', '??0DependentSelectorClass@@QAE@XZ', '0x49FC90'),
]


class Cr1NamedTargets(unittest.TestCase):
    """D246 (CR1): selector.obj's short DependentSelectorClass constructor stores its vtable 0xAEA9C0, which the
    object's exact functions place; the one verified start storing it with the constructor's length is 0x49FC90."""
    def test_cases(self):
        _check(self, CR1_NAMED_TARGETS, 'USE_CR1_NAMED_TARGETS', 'named_targets', APP_REGION, off_status='short')


class CkCallseqVtbl(unittest.TestCase):
    def test_cases(self):
        _check(self, CK_CALLSEQ_VTBL, 'USE_CK_CALLSEQ_VTBL', 'ck_callseq', APP_REGION, off_status='short')


VX_CREATE_BINARY_FLOAT = '?Create_Binary@?$ExprMathCompareOpsClass@M@@SA?AV?$RefCountPtr@V?$ExprMathEvalClass@M@@@@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ABV2@1@Z'
VX_SHARE_BUFFER_V2 = '??1?$ShareBufferClass@VVector2@@@@UAE@XZ'


def _vx_results(test, name, switch, on):
    path = os.path.join(OBJS, name)
    if not os.path.exists(path):
        test.skipTest(f'{path} missing')
    img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
    setattr(objmatch, switch, on)
    try:
        return {r['symbol']: r for r in objmatch.match_objects(img, [path], APP_REGION)}
    finally:
        setattr(objmatch, switch, True)


class VxVtblContra(unittest.TestCase):
    """VX P1: expr_math_float.obj's Create_Binary<float> matches only 0x4CD270, whose vptr stores hold the <int>
    tables (0xAEBF88: slot 2 the thunk to Evaluate<int> 0x4CE080, not Evaluate<float> 0x4C9CB0): rejected."""
    def test_rejected(self):
        res = _vx_results(self, 'expr_math_float.obj', 'USE_VX_VTBL_CONTRA', True)
        self.assertNotEqual(res[VX_CREATE_BINARY_FLOAT]['status'], 'exact')
        self.assertIn('0x4CD270', res[VX_CREATE_BINARY_FLOAT].get('vx_rejected', []))
        res = _vx_results(self, 'expr_math_float.obj', 'USE_VX_VTBL_CONTRA', False)
        self.assertEqual(res[VX_CREATE_BINARY_FLOAT]['addresses'], ['0x4CD270'])


class VxSameTargets(unittest.TestCase):
    """VX P3: the ~ShareBufferClass<T> copies 0x483600 / 0x483670 / 0x51A7B0 store their own tables (0xAEA230, 0xAEA218,
    0xAED57C): not D54's identical copies; 0x51A610 / 0x51A7B0 (both 0xAED57C) are."""
    def test_not_identical(self):
        path = os.path.join(OBJS, 'dynamic_window_icon.obj')
        if not os.path.exists(path):
            self.skipTest(f'{path} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        start, end, raw = img.section('.text')
        text = img.data[raw:raw + (end - start)]
        obj = objmatch.CoffObject(path)
        sec, s, e = next((sec, s, e) for name, sec, s, e in obj.functions() if name == VX_SHARE_BUFFER_V2)
        length = obj.pattern(sec, s, e)[2]
        relocs = obj.relocated(sec, s, length)
        strs = {k for k, n, typ, sym, add in obj.reloc_targets(sec, s, length)
                if sym is not None and obj.literal(sym) is not None}
        self.assertFalse(objmatch.same_copies(text, start, [0x483600, 0x483670, 0x51A7B0], relocs, img, strs))
        self.assertTrue(objmatch.same_copies(text, start, [0x51A610, 0x51A7B0], relocs, img, strs))
        self.assertTrue(objmatch.same_copies(text, start, [0x483600, 0x483670, 0x51A7B0], relocs, img))   # the old reading


class VxSlotContra(unittest.TestCase):
    """VX P2: client_item_property_special_maker.obj's Get_Lines matches 0x52BC30 with the special effect's; the maker's
    table 0xAEDF00 (its exact constructor 0x52C980 stores it) holds 0x52CBF0 at that slot: not credited at 0x52BC30."""
    def test_slot(self):
        paths = [os.path.join(OBJS, n) for n in ('client_item_property_special_maker.obj',
                                                 'client_item_property_special_effect.obj')]
        if not all(os.path.exists(x) for x in paths):
            self.skipTest('objects missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        sym = '?Get_Lines@ClientItemPropertySpecialMakerClass@@'
        was = objmatch.USE_VX_SLOT_CONTRA
        for on in (True, False):
            objmatch.USE_VX_SLOT_CONTRA = on
            try:
                res = [r for r in objmatch.match_objects(img, paths, APP_REGION) if r['symbol'].startswith(sym)]
            finally:
                objmatch.USE_VX_SLOT_CONTRA = was
            self.assertEqual(len(res), 1)
            if on:
                self.assertEqual(res[0].get('vx_slot'), '0x52BC30')
                self.assertNotEqual(res[0]['status'], 'exact')
            else:
                self.assertEqual((res[0]['status'], res[0]['addresses']), ('exact', ['0x52BC30']))



class NamedAway(unittest.TestCase):
    """WA (run 2026-11-17b, USE_NAMED_AWAY): wwaudio's inlined-loop ??_E_LOGICAL_TYPE_STRUCT matches 0x9DA8C0
    (mixfile's FileOffsetStruct copy, also soundstreamhandle.obj's ??_E_ENUM_VALUE's match); symbols.json names the
    client's copy 0x4338D0 (Resize<_LOGICAL_TYPE_STRUCT> 0x9F6A80 calls it), so the 0x9DA8C0 credit is dropped."""
    def test_named_away(self):
        objs = os.path.join(MAIN, 'build', 'wwaudio', 'obj')
        paths = [os.path.join(objs, n) for n in ('WWAudio.obj', 'soundstreamhandle.obj')]
        if not all(os.path.exists(p) for p in paths):
            self.skipTest(f'{objs} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        sym = '??_E_LOGICAL_TYPE_STRUCT@WWAudioClass@@QAEPAXI@Z'
        resize = '?Resize@?$DynamicVectorClass@U_LOGICAL_TYPE_STRUCT@WWAudioClass@@@@UAE_NHPBU_LOGICAL_TYPE_STRUCT@WWAudioClass@@@Z'
        res = {(r['object'], r['symbol']): r for r in objmatch.match_objects(img, paths)}
        self.assertEqual(res[('WWAudio.obj', sym)]['status'], 'none')
        self.assertEqual(res[('WWAudio.obj', sym)]['named_away'], '0x4338D0')
        self.assertEqual(res[('WWAudio.obj', resize)]['addresses'], ['0x9F6A80'])
        objmatch.USE_NAMED_AWAY = False
        try:
            res = {(r['object'], r['symbol']): r for r in objmatch.match_objects(img, paths)}
        finally:
            objmatch.USE_NAMED_AWAY = True
        self.assertEqual((res[('WWAudio.obj', sym)]['status'], res[('WWAudio.obj', sym)]['addresses']),
                         ('exact', ['0x9DA8C0']))


class NamedAwayAlone(unittest.TestCase):
    """NA (run 2026-11-17b, USE_NAMED_AWAY_ALONE): wwsaveload's inlined-loop ??_E_ENUM_VALUE matches only 0x9DA8C0
    (mixfile's FileOffsetStruct ??_E; no other symbol of the run is exact there); D88's self name is 0x433970, so the
    lone, untie-broken credit is dropped."""
    def test_named_away_alone(self):
        p = os.path.join(MAIN, 'build', 'wwsaveload', 'obj', 'twiddler.obj')
        if not os.path.exists(p):
            self.skipTest(f'{p} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        sym = '??_E_ENUM_VALUE@EnumParameterClass@@QAEPAXI@Z'
        r = {x['symbol']: x for x in objmatch.match_objects(img, [p])}[sym]
        self.assertEqual((r['status'], r['named_away'], r.get('named_away_alone')), ('none', '0x433970', True))
        objmatch.USE_NAMED_AWAY_ALONE = False
        try:
            r = {x['symbol']: x for x in objmatch.match_objects(img, [p])}[sym]
        finally:
            objmatch.USE_NAMED_AWAY_ALONE = True
        self.assertEqual((r['status'], r['addresses']), ('exact', ['0x9DA8C0']))


W3D_OBJS = os.environ.get('OBJMATCH_W3D_OBJS', os.path.join(MAIN, 'build', 'ww3d2', 'obj'))


def _one(test, path, region, off=()):
    if not os.path.exists(path):
        test.skipTest(f'{path} missing')
    img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
    owner = lambda sw: objmatch.ClientImage if hasattr(objmatch.ClientImage, sw) else objmatch   # D27's switches too
    for sw in off:
        setattr(owner(sw), sw, False)
    try:
        return {r['symbol']: r for r in objmatch.match_objects(img, [path], region)}
    finally:
        for sw in off:
            setattr(owner(sw), sw, True)


class Cr2Tiling(unittest.TestCase):
    """D246 (CR2): ww3d.obj's forwarders between the exact Toggle_Windowed 0x917850 and Set_Device_Resolution 0x9178A0
    tile the client's bytes (jmp 0x917860 / call 0x917870 / jmp 0x917880 / call 0x917890); the verified starts hold
    only the jmps, so short_order's count test fails (`cr2_tiling`). Since SX D27's starts hold the calls too
    (USE_JMP_PAD_STARTS) and short_order places them; cr2_tiling still does without that switch."""
    CASES = [('WW3D::Get_Render_Device', '0x917860'), ('WW3D::Get_Render_Device_Desc', '0x917870'),
             ('WW3D::Get_Render_Device_Count', '0x917880'), ('WW3D::Get_Render_Device_Name', '0x917890')]

    def test_cases(self):
        path = os.path.join(W3D_OBJS, 'ww3d.obj')
        res = {r['name']: r for r in _one(self, path, None).values()}
        for name, addr in self.CASES:
            with self.subTest(name=name):
                self.assertEqual((res[name]['status'], res[name]['addresses']), ('exact', [addr]))
        res = {r['name']: r for r in _one(self, path, None, ('USE_JMP_PAD_STARTS',)).values()}
        for name, addr in self.CASES:
            with self.subTest(name=name, starts='old'):
                self.assertEqual((res[name]['status'], res[name]['addresses']), ('exact', [addr]))
                self.assertTrue(res[name].get('cr2_tiling'))
        res = {r['name']: r for r in _one(self, path, None, ('USE_CR2_TILING', 'USE_JMP_PAD_STARTS')).values()}
        self.assertEqual(res['WW3D::Get_Render_Device']['status'], 'short')


class Cr2DataAgree(unittest.TestCase):
    """D246 (CR2): unknown_7196c0.obj's two qsort comparators have the same bytes at 0x7196C0 and 0x719730 but read
    Info+0xC / Info+0x10: only 0x7196C0 = Unknown_7196C0, 0x719730 = Unknown_719730 gives Info one address
    (`cr2_data`)."""
    def test_cases(self):
        path = os.path.join(OBJS, 'unknown_7196c0.obj')
        res = _one(self, path, APP_REGION)
        for sym, addr in (('?Unknown_7196C0@@YAHPBX0@Z', '0x7196C0'), ('?Unknown_719730@@YAHPBX0@Z', '0x719730')):
            with self.subTest(sym=sym):
                self.assertEqual((res[sym]['status'], res[sym]['addresses']), ('exact', [addr]))
                self.assertTrue(res[sym].get('cr2_data'))
        res = _one(self, path, APP_REGION, ('USE_CR2_DATA_AGREE',))
        self.assertEqual(res['?Unknown_7196C0@@YAHPBX0@Z']['status'], 'multiple')


class Cr2XcuStarts(unittest.TestCase):
    """D246 (CR2): camctrl_dock.obj's atexit stub $E97 0x793090 and initialiser $E105 0x7930B0 (the CRT table's
    entry), after its last own function, are no verified starts (D27); with the table's entries and the atexit-
    registered addresses as starts the order rules place them (`gap_edge`). Since SX D27's starts hold them
    (USE_CRT_REGISTERED_STARTS), so USE_CR2_XCU_STARTS alone changes nothing."""
    def test_cases(self):
        path = os.path.join(OBJS, 'camctrl_dock.obj')
        res = _one(self, path, APP_REGION)
        for sym, addr in (('_$E97', '0x793090'), ('_$E105', '0x7930B0')):
            with self.subTest(sym=sym):
                self.assertEqual((res[sym]['status'], res[sym]['addresses']), ('exact', [addr]))
        res = _one(self, path, APP_REGION, ('USE_CR2_XCU_STARTS',))
        self.assertEqual(res['_$E97']['status'], 'exact')
        res = _one(self, path, APP_REGION, ('USE_CR2_XCU_STARTS', 'USE_CRT_REGISTERED_STARTS'))
        self.assertEqual(res['_$E97']['status'], 'short')


SLNODE_POOL ='?Allocator@?$AutoPoolClass@VGenericSLNode@@$0BAA@@@0V?$ObjectPoolClass@VGenericSLNode@@$0BAA@@@A'


class Cr2ExternData(unittest.TestCase):
    """D246 (CR2): slnode.obj's initialiser $E6 zeroes GenericSLNode's pool, which app's exact 0x42FD10 places at
    0xC1CD60 (another library's DIR32 field, passed as extern data): the CRT table entry 0x9CEDA0 is the one where
    D80 confirms it (dx8renderer's $E252 has the same bytes for its own pool), its atexit stub $E3 0x9CEDD0."""
    def test_cases(self):
        path = os.path.join(MAIN, 'build', 'wwlib', 'obj', 'slnode.obj')
        if not os.path.exists(path):
            self.skipTest(f'{path} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        ext = {objmatch.CR2_DATA_PREFIX + SLNODE_POOL: {0xC1CD60}}
        res = {r['symbol']: r for r in objmatch.match_objects(img, [path], None, extern_called=ext)}
        self.assertEqual((res['_$E6']['status'], res['_$E6']['addresses']), ('exact', ['0x9CEDA0']))
        self.assertEqual((res['_$E3']['status'], res['_$E3']['addresses']), ('exact', ['0x9CEDD0']))
        objmatch.USE_CR2_EXTERN_DATA = False
        try:
            res = {r['symbol']: r for r in objmatch.match_objects(img, [path], None, extern_called=ext)}
        finally:
            objmatch.USE_CR2_EXTERN_DATA = True
        self.assertNotEqual(res['_$E6']['status'], 'exact')


class Cr4XcuSparse(unittest.TestCase):
    """D246 (CR4): delta.obj lists two initialisers, `_$E5` (entry 50, 0x4A4C90) and the guarded header static `_$E29`;
    the client's stretch holds entries 51-52 our object does not list, so no consecutive sub-run fits; `_$E29`
    fullmatches at one entry of its window (bounded by its neighbours bitset.obj and delta_interpolator.obj), 0x4A6070
    (entry 53)."""
    def test_sparse(self):
        paths = [os.path.join(OBJS, n) for n in ('bitset.obj', 'delta.obj', 'delta_interpolator.obj')]
        if not all(os.path.exists(p) for p in paths):
            self.skipTest('bitset.obj / delta.obj / delta_interpolator.obj missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        def get():
            return {(r['object'], r['symbol']): r for r in objmatch.match_objects(img, paths, APP_REGION, callers=True)}[
                ('delta.obj', '_$E29')]
        r = get()
        self.assertEqual((r['status'], r['addresses'], r.get('xcu')), ('exact', ['0x4A6070'], 'sparse'))
        objmatch.USE_CR4_XCU_SPARSE = False
        try:
            self.assertEqual(get()['status'], 'multiple')
        finally:
            objmatch.USE_CR4_XCU_SPARSE = True


class Cr6NamedCallers(unittest.TestCase):
    """D246 (CR6): sclient.obj's Create_NEW<ChatPanelInterface> and Create_NEW<UITalkTreeSpaceClass> (21 bytes,
    byte-identical with 157 client copies) are called by SClient::Init, not exact, which symbols.json names at 0x741180
    from its vtable slot; its calls into the two symbols' copies, in order, are the client function's calls to 0x755AF0
    and 0x755B60 (`cr6_named`)."""
    def test_cases(self):
        _check(self, [('sclient.obj', '?Create_NEW@@YA?AV?$RefCountPtr@VChatPanelInterface@@@@PAVChatPanelInterface@@@Z',
                       '0x755AF0'),
                      ('sclient.obj', '?Create_NEW@@YA?AV?$RefCountPtr@VUITalkTreeSpaceClass@@@@PAVUITalkTreeSpaceClass@@@Z',
                       '0x755B60')],
               'USE_CR6_NAMED_CALLERS', 'cr6_named', APP_REGION)


if __name__ == '__main__':
    unittest.main()
