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
function to the next object's, at most RANGE_GAP past its last): px.obj's PxBox * helpers right after its code
(_Construct 0x44D6A0, _Lower_bound 0x44D6D0), against byte-identical copies in other files' code (`contrib`).
Its short COMDATs (USE_CONTRIB_SHORTS): vector<PxBox *>::_Destroy 0x44D5D0 (`ret 8`) there, at the first holder's
verified start holding its bytes. (px_process.obj's vector<PxProcessClass *> helpers were the cases until other rules
came to credit them first: fresh objects of 2026-11-18.)
GAP_EDGE (D246 / EX): camera_manager.obj's short getter 0x79B4D0, right after the exact setter 0x79B4B0 in a gap that
also holds the not-exact 0x79B4F0 (short_order's count test fails), takes the next verified start after the setter's
end (`gap_edge`). USE_EX_GAP_KNOWN: boolpars.obj's destructor and Set_Callbacks between the exact deleting destructor
0x49A3C0 and the not-exact Parse, which symbols.json places at 0x49A430. USE_EX_GAP_PADDED: delta_interpolator.obj's
Get_Change 0x4A6360 (nothing calls it, so no verified start) right after the destructor 0x4A6330's padding.
CK_CALLSEQ (D246 / CK): unknown_79d250.obj's (co_data.obj's until the code moved there) ClientObjectDataAccessClass<RadarDataClass>::Get, not exact, is located at 0x79E300 by
Get_Radar_Data's call; its calls in order give the tied pair<ClientObjectClass *const, RadarDataClass *>'s constructor
0x7A0A80 (its one caller is 0x79E300) and _Hashtable_const_iterator's 0x7A0920 (`ck_callseq`). (find_or_insert
0x7A0EC0 and its resize 0x7A3B50 were the cases until other rules came to credit them first: fresh objects of 2026-11-18.)
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
                objmatch.USE_CR7_FUNCLETS = False       # CR7: a located caller's funclet stream places it too
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path])}
                finally:
                    objmatch.USE_FUNCLET_CALLS = True
                    objmatch.USE_CONTRIB_SHORTS = True
                    objmatch.USE_CR1_NAMED_TARGETS = True
                    objmatch.USE_CR7_FUNCLETS = True
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
                objmatch.USE_CR7_ADDR_REFS = False      # CR7: a located caller's references place it too
                try:
                    res = {r['symbol']: r for r in objmatch.match_objects(img, [path], (0x401000, 0x8C1B50))}
                finally:
                    objmatch.USE_PUSHED_CALLS = True
                    objmatch.USE_CONTRIB_SHORTS = True
                    objmatch.USE_CR1_NAMED_TARGETS = True
                    objmatch.USE_CR7_ADDR_REFS = True
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
    # two template destructors with one body, linker-folded at 0x53CF80: both credited by the funclet round
    # (client_lego_impl.obj's pair at 0x700BC0 was the case until ca123b2eb gave Create's lego one type)
    (['ui_ptrade.obj', 'ui_group.obj'], '??1?$RefCountPtr@VCharacterViewAvatarClass@@@@QAE@XZ', '0x53CF80',
     'funclet_tie', 'USE_FUNCLET_FOLDED'),
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
                if sym not in res:
                    self.skipTest(f'{sym} not in {names}: objects older than the case')
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
    ('px.obj', '?_Construct@std@@YAXPAPAVPxBox@@ABQAV2@@Z', '0x44D6A0'),
    ('px.obj', '?_Lower_bound@std@@YAPAPAVPxBox@@PAPAV2@0ABQAV2@PAH@Z', '0x44D6D0'),
]
CONTRIB_SHORTS = [
    ('px.obj', '?_Destroy@?$vector@PAVPxBox@@V?$allocator@PAVPxBox@@@std@@@std@@IAEXPAPAVPxBox@@0@Z', '0x44D5D0'),
]
GAP_EDGE = [
    ('camera_manager.obj', '?Unknown_79B4D0@CameraManagerClass@@QAE_NXZ', '0x79B4D0'),
    ('delta_interpolator.obj', '?Get_Change@DeltaInterpolatorClass@@QBEMXZ', '0x4A6360'),
    ('boolpars.obj', '??1BoolParserClass@@UAE@XZ', '0x49A3F0'),
    ('boolpars.obj', '?Set_Callbacks@BoolParserClass@@QAEXPAV?$hash_map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVBoolEvaluatorClass@@U?$hash@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@U?$equal_to@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVBoolEvaluatorClass@@@2@@std@@@Z', '0x49A410'),
]
CK_CALLSEQ = [
    ('unknown_79d250.obj', '??0?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@QAE@ABQAVClientObjectClass@@ABQAVRadarDataClass@@@Z', '0x7A0A80'),
    ('unknown_79d250.obj', '??0?$_Hashtable_const_iterator@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@PAVClientObjectClass@@U?$hash@PAVClientObjectClass@@@2@U?$_Select1st@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@@2@U?$equal_to@PAVClientObjectClass@@@2@V?$allocator@PAVRadarDataClass@@@2@@std@@QAE@ABU?$_Hashtable_iterator@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@PAVClientObjectClass@@U?$hash@PAVClientObjectClass@@@2@U?$_Select1st@U?$pair@QAVClientObjectClass@@PAVRadarDataClass@@@std@@@2@U?$equal_to@PAVClientObjectClass@@@2@V?$allocator@PAVRadarDataClass@@@2@@1@@Z', '0x7A0920'),
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
        # CR7's per-pool alignment reaches _Lower_bound's 0x44D6D0 from its caller too: off as well
        _check(self, CONTRIB_TIES, 'USE_CONTRIB_TIES', 'contrib', APP_REGION, also_off=('USE_CR7_POOL_ALIGN',))


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
    """VX P1: bar2d.obj's ~ShareBufferClass<Vector2> matches only 0x6BB740, whose vptr store is ShareBufferClass<Vector4>'s
    table 0xAFE6EC (its slots hold <Vector4>'s members, not <Vector2>'s): rejected. (expr_math_float.obj's
    Create_Binary<float> at 0x4CD270 was the case until it became exact at 0x4C9010 by its own bytes, the rule's
    reading confirmed: fresh objects of 2026-11-18.)"""
    def test_rejected(self):
        res = _vx_results(self, 'bar2d.obj', 'USE_VX_VTBL_CONTRA', True)
        self.assertNotEqual(res[VX_SHARE_BUFFER_V2]['status'], 'exact')
        self.assertIn('0x6BB740', res[VX_SHARE_BUFFER_V2].get('vx_rejected', []))
        res = _vx_results(self, 'bar2d.obj', 'USE_VX_VTBL_CONTRA', False)
        self.assertEqual(res[VX_SHARE_BUFFER_V2]['addresses'], ['0x6BB740'])


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
    table 0xAEDF00 (its exact constructor 0x52C980 stores it) holds 0x52CBF0 at that slot: not credited at 0x52BC30.
    Since the maker's Get_Lines was rebuilt it is exact at 0x52CBF0 by its own bytes (the rule's reading confirmed), and
    no app object exercises the rule any more (a full match of the fresh objects of 2026-11-18 sets `vx_slot` nowhere):
    with such an object the test checks that reading, with the switch on and off."""
    def test_slot(self):
        paths = [os.path.join(OBJS, n) for n in ('client_item_property_special_maker.obj',
                                                 'client_item_property_special_effect.obj')]
        if not all(os.path.exists(x) for x in paths):
            self.skipTest('objects missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        sym = '?Get_Lines@ClientItemPropertySpecialMakerClass@@'
        was = objmatch.USE_VX_SLOT_CONTRA
        objmatch.USE_VX_SLOT_CONTRA = False
        try:
            off = [r for r in objmatch.match_objects(img, paths, APP_REGION) if r['symbol'].startswith(sym)]
        finally:
            objmatch.USE_VX_SLOT_CONTRA = was
        if len(off) == 1 and off[0].get('addresses') == ['0x52CBF0']:
            self.assertEqual(off[0]['status'], 'exact')
            on = [r for r in objmatch.match_objects(img, paths, APP_REGION) if r['symbol'].startswith(sym)]
            self.assertEqual((on[0]['status'], on[0]['addresses'], on[0].get('vx_slot')), ('exact', ['0x52CBF0'], None))
            return
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
    (USE_CRT_REGISTERED_STARTS), so USE_CR2_XCU_STARTS alone changes nothing. The compiler numbers `$E` per
    translation unit (an initialiser more in a header renumbers the rest: $E99 / $E107 since f92f6a0bf), so the
    pair is the object's two highest-numbered `$E` functions."""
    def test_cases(self):
        path = os.path.join(OBJS, 'camctrl_dock.obj')
        res = _one(self, path, APP_REGION)
        atexit, init = sorted((s for s in res if s.startswith('_$E')), key=lambda s: int(s[3:]))[-2:]
        for sym, addr in ((atexit, '0x793090'), (init, '0x7930B0')):
            with self.subTest(sym=sym):
                self.assertEqual((res[sym]['status'], res[sym]['addresses']), ('exact', [addr]))
        res = _one(self, path, APP_REGION, ('USE_CR2_XCU_STARTS',))
        self.assertEqual(res[atexit]['status'], 'exact')
        res = _one(self, path, APP_REGION, ('USE_CR2_XCU_STARTS', 'USE_CRT_REGISTERED_STARTS'))
        self.assertEqual(res[atexit]['status'], 'short')


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
    """D246 (CR4): memconn.obj's guarded header static initialiser (`test` / `or` of its guard byte 0xC416FC, then
    atexit) fullmatches at one entry of its window, 0x827530 (the CRT table's entry at 0xB6CEFC), where no consecutive
    sub-run of the object's initialisers fits the client's stretch (`xcu` 'sparse'). Found by its address: the compiler
    numbers `$E` per translation unit. (delta.obj's `_$E29` at 0x4A6070 was the case until delta.obj came to list the
    stretch's other entries, f92f6a0bf: fresh objects of 2026-11-18.)"""
    def test_sparse(self):
        path = os.path.join(OBJS, 'memconn.obj')
        if not os.path.exists(path):
            self.skipTest(f'{path} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)

        def get():
            return {r['symbol']: r for r in objmatch.match_objects(img, [path], APP_REGION, callers=True)}
        res = get()
        syms = [k for k, r in res.items() if k.startswith('_$E') and r.get('addresses') == ['0x827530']]
        self.assertEqual(len(syms), 1)
        r = res[syms[0]]
        self.assertEqual((r['status'], r['addresses'], r.get('xcu')), ('exact', ['0x827530'], 'sparse'))
        objmatch.USE_CR4_XCU_SPARSE = False
        try:
            self.assertEqual(get()[syms[0]]['status'], 'multiple')
        finally:
            objmatch.USE_CR4_XCU_SPARSE = True


class ObXcuOnePerEntry(unittest.TestCase):
    """D246 (OB): permstr.obj's two .CRT$XCU initialisers (byte-identical, 178 bytes) are both found once, at 0x4ADC50,
    the CRT table's entry 76; an entry is one function pointer, so the first keeps it and the second, with no later
    entry it fullmatches, is left uncredited (`xcu_twin`). Compared by object, not by `_$E` name."""
    def test_one(self):
        path = os.path.join(OBJS, 'permstr.obj')
        if not os.path.exists(path):
            self.skipTest(f'{path} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        inits = objmatch.xcu_initialisers(objmatch.CoffObject(path))

        def at():
            res = {r['symbol']: r for r in objmatch.match_objects(img, [path], APP_REGION)}
            return [n for n in inits if res[n]['status'] == 'exact' and res[n]['addresses'] == ['0x4ADC50']], res
        got, res = at()
        self.assertEqual(got, inits[:1])
        self.assertEqual([res[n].get('xcu_twin') for n in inits[1:]], ['0x4ADC50'] * (len(inits) - 1))
        objmatch.USE_OB_XCU_ONE_PER_ENTRY = False
        try:
            self.assertEqual(len(at()[0]), 2)
        finally:
            objmatch.USE_OB_XCU_ONE_PER_ENTRY = True


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

    def test_string_located(self):
        """USE_CR6_STRING_LOCATED: attrib_dir_collective.obj's Add_Destination (not exact, no name) is the one client
        function holding its assert texts ('find != DestinationAttributes.end()', 'ValueFunction'), 0x4DC310; its
        calls to the two byte-identical pair<const string, T> constructors, in order, reach 0x4DDB20 (T =
        DestinationStruct *) and 0x4DD6F0 (T = float)."""
        p = '??0?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@'
        _check(self, [('attrib_dir_collective.obj', p + 'PAUDestinationStruct@AttribDirCollective@@@std@@QAE@ABV?$basic_'
                       'string@DU?$char_traits@D@std@@V?$allocator@D@2@@1@ABQAUDestinationStruct@AttribDirCollective@@@Z',
                       '0x4DDB20'),
                      ('attrib_dir_collective.obj', p + 'M@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator'
                       '@D@2@@1@ABM@Z', '0x4DD6F0')],
               'USE_CR6_STRING_LOCATED', 'cr6_named', APP_REGION,
               also_off=('USE_CR7_CALLEE_LOCATED',))    # CR7: its exact callees locate it as well

    def test_short(self):
        """USE_CR6_SHORTS: shiputils.obj's RefCountPtr<MeshModelClass>::operator-> (`mov eax, [ecx]; ret`, too short to
        search) is the one target of Unknown_812390's (0x812390, located by the exact functions' calls) client calls
        that holds its bytes, 0x813950."""
        _check(self, [('shiputils.obj', '??C?$RefCountPtr@VMeshModelClass@@@@QBEPAVMeshModelClass@@XZ', '0x813950')],
               'USE_CR6_SHORTS', 'cr6_named', APP_REGION, off_status='short')


_GMR_ITER = ('QAE?AU?$_Hashtable_iterator@U?$pair@$$CBHV?$RefCountPtr@VGalaxyMapRenderXClass@@@@@std@@HU?$hash@H@2@U?$'
             '_Select1st@U?$pair@$$CBHV?$RefCountPtr@VGalaxyMapRenderXClass@@@@@std@@@2@U?$equal_to@H@2@V?$allocator@V?$'
             'RefCountPtr@VGalaxyMapRenderXClass@@@@@2@@2@ABH@Z')
_GMR_TABLE_FIND = ('?find@?$hashtable@U?$pair@$$CBHV?$RefCountPtr@VGalaxyMapRenderXClass@@@@@std@@HU?$hash@H@2@U?$_Select1st'
                   '@U?$pair@$$CBHV?$RefCountPtr@VGalaxyMapRenderXClass@@@@@std@@@2@U?$equal_to@H@2@V?$allocator@V?$RefCount'
                   'Ptr@VGalaxyMapRenderXClass@@@@@2@@std@@' + _GMR_ITER)
_GMR_MAP_FIND = ('?find@?$hash_map@HV?$RefCountPtr@VGalaxyMapRenderXClass@@@@U?$hash@H@std@@U?$equal_to@H@3@V?$allocator@V?$'
                 'RefCountPtr@VGalaxyMapRenderXClass@@@@@3@@std@@' + _GMR_ITER)


class Cr7Callers(unittest.TestCase):
    """D246 (CR7): galaxy_map_render.obj's not-exact callers of its tied copies."""
    def test_callee_located(self):
        """USE_CR7_CALLEE_LOCATED: GalaxyMapRenderSectorClass::Unknown_7AB6D0 (not exact, no client name) is the one
        client function calling all its exact callees, 0x7AB6D0; its one call to hashtable<System>::find reaches the
        copy 0x7B1DE0 (of 0x7B1AB0 / 0x7B1DE0 left)."""
        sym = _GMR_TABLE_FIND.replace('X', 'System')
        _check(self, [('galaxy_map_render.obj', sym, '0x7B1DE0')], 'USE_CR7_CALLEE_LOCATED', 'cr6_named', APP_REGION)

    def test_pool_align(self):
        """USE_CR7_POOL_ALIGN: GalaxyMapRenderClass::Unknown_7A8B70 (its self name) calls Create_NEW<System>,
        Create_NEW<Sector>, hash_map<Sector>::find and two RefCountPtr<GalaxyMapRenderItemClass> conversions; the client
        function reaches one conversion copy only, so the whole alignment fails, but the Create_NEW pool aligns
        (0x7B1A50, 0x7B1C80, call order) and the find pool too (0x7B1AB0)."""
        new = '?Create_NEW@@YA?AV?$RefCountPtr@VGalaxyMapRender%sClass@@@@PAVGalaxyMapRender%sClass@@@Z'
        find = _GMR_MAP_FIND.replace('X', 'Sector')
        _check(self, [('galaxy_map_render.obj', new % ('System', 'System'), '0x7B1A50'),
                      ('galaxy_map_render.obj', new % ('Sector', 'Sector'), '0x7B1C80'),
                      ('galaxy_map_render.obj', find, '0x7B1AB0')],
               'USE_CR7_POOL_ALIGN', 'cr6_named', APP_REGION)

    def test_funclets(self):
        """USE_CR7_FUNCLETS: Unknown_7A8B70's EH prologue pushes its FuncInfo's stub; the object's unwind funclets
        destroy the System, Sector and Gate iterators in state order, the client's funclets jump to 0x7A9470,
        0x7A94A0 and 0x7A94D0 (byte-identical destructors) in the same order."""
        _check(self, [('galaxy_map_render.obj', '??1GalaxyMap%sIteratorClass@@QAE@XZ' % k, a)
                      for k, a in (('System', '0x7A9470'), ('Sector', '0x7A94A0'), ('Gate', '0x7A94D0'))],
               'USE_CR7_FUNCLETS', 'cr6_named', APP_REGION)

    def test_addr_refs(self):
        """USE_CR7_ADDR_REFS: LandClass::Build_Decorations (unknown_474530.obj, not exact) sits at 0x47A2F0 (the full
        run locates it by its exact callees, some in other objects, so this checks the stream itself); it pushes
        vector<LandDecorationVertexStruct>'s and vector<unsigned short>'s default-constructor closures and
        destructors for `eh vector constructor iterator`, the client function the thunks of 0x47CD70 / 0x47D1B0 then
        0x47CE30 / 0x47D1F0: per pool (closures, destructors) the references align in order."""
        path = os.path.join(OBJS, 'unknown_474530.obj')
        if not os.path.exists(path):
            self.skipTest(f'{path} missing')
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)
        obj = objmatch.CoffObject(path)
        ldv = '?$vector@ULandDecorationVertexStruct@@V?$allocator@ULandDecorationVertexStruct@@@std@@@std@@QAE'
        g = '?$vector@GV?$allocator@G@std@@@std@@QAE'
        f_ldv, f_g, d_ldv, d_g = '??_F' + ldv + 'XXZ', '??_F' + g + 'XXZ', '??1' + ldv + '@XZ', '??1' + g + '@XZ'
        names = {f_ldv, f_g, d_ldv, d_g}
        fn = next((sec, s, e - s) for n, sec, s, e in obj.functions() if n.startswith('?Build_Decorations@LandClass@@'))
        starts = objmatch._verified_starts(img)
        end = starts[starts.index(0x47A2F0) + 1]
        ours, theirs = objmatch._cr7_addr_refs(img, obj, *fn, 0x47A2F0, end, names, objmatch.thunks_of(img))
        cset = {f_ldv: {0x47CD70, 0x47CE30}, f_g: {0x47CD70, 0x47CE30}, d_ldv: {0x47D1B0, 0x47D1F0},
                d_g: {0x47D1B0, 0x47D1F0}}
        got = {}
        for group in objmatch._cr7_pools(cset):
            pool = set().union(*(cset[n] for n in group))
            got.update(objmatch._cr6_align([n for n in ours if n in group], [t for t in theirs if t in pool], cset)
                       or {})
        self.assertEqual(got, {f_ldv: 0x47CD70, d_ldv: 0x47D1B0, f_g: 0x47CE30, d_g: 0x47D1F0})


class NbXcuStretch(unittest.TestCase):
    """D246 (NB): USE_NB_XCU_STRETCH's test, xcu_in_stretch on the client's CRT table. unknown_root.obj's <string> guard
    `_$E32` took 0x4A7840 (CRT entry 60) by the range round in the full app run, where delta_money.obj's own `_$E32`
    sits by the table round, past delta.obj's entries 50-53, with unknown_root's PermStringInit 0x4A3B80 at entry 49:
    entry 60 lies outside unknown_root's stretch and inside delta_money's (the full run: that one credit gone, nothing
    else changed). Synthetic results, so no object is needed."""
    def test_stretch(self):
        img = objmatch.ClientImage(objmatch.DEFAULT_CLIENT)

        class Obj:
            pass
        root, delta, money = Obj(), Obj(), Obj()

        def res(obj, sym, a):
            return ({'symbol': sym, 'status': 'exact', 'addresses': [f'0x{a:06X}']}, obj, 1, 0, 16)
        results = [res(root, '_$E5', 0x4A3B80), res(root, '_$E32', 0x42B2F0), res(delta, '_$E5', 0x4A4C90),
                   res(money, '_$E5', 0x4A7000), res(money, '_$E32', 0x4A7840)]
        listed = {id(root): {'_$E5', '_$E32'}, id(delta): {'_$E5'}, id(money): {'_$E5', '_$E32'}}
        self.assertFalse(objmatch.xcu_in_stretch(img, results, root, listed, 0x4A7840))
        self.assertTrue(objmatch.xcu_in_stretch(img, results, money, listed, 0x4A7840))
        self.assertTrue(objmatch.xcu_in_stretch(img, results, root, listed, 0x4A3B80))
        self.assertTrue(objmatch.xcu_in_stretch(img, results, root, listed, 0x4A3B40))   # no table entry
        other = Obj()
        listed[id(other)] = {'_$E5'}
        self.assertTrue(objmatch.xcu_in_stretch(img, results, other, listed, 0x4A7840))  # no own entry: no bound


if __name__ == '__main__':
    unittest.main()
