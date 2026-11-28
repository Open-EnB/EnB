"""behaudit.py's checks on session 0's positive controls (run 2026-11-24-night), no VM
(python -m unittest tools/re/test_behaudit.py, or run it).

The fixtures are objects built before session 0's fixes, so they still carry the bugs; they are not tracked (build
output): .work/behaudit/fixtures/ in the main tree (or $BEHAUDIT_FIXTURES), copied on 2026-11-25 from PS's 22:39
build (.work/ps/lst/app/obj, HEAD 8bd09c18b) and PS's positive-control build (.work/ps/posctl: galaxy_map_render.obj
with the 0x7AC130 bug put back, and its /FAs listing renamed galaxy_map_render.asm). Missing fixtures skip.

  0x5DED10 petition terminal widget names 'Mission_%02d_PT_MIS' (client 'Mission%02d_PT_MIS'): lit, exact function
  0x557290 Random Appearance passes 0.0f as the bool (VC6 pushes a garbage imm8; the client pushes ebx = 0): imm
  0x7BEF50 StarmapData Create_Mesh calls Create_Peek where the client calls a Create_Get: rel call, exact function
  0x64BCA0 ui_talkt Create_Get where the client calls Create_Peek: rel call, exact function
  0x470EF0 Get_Vertex_Color 1/255 = 0x3B808081 (client 0x3B808080, one ulp): lit constant, exact function
  0x8494C0 nine fabs() > 0.0001 tolerances as doubles (client (double)0.0001f): lit constant
  0x74ABC0 /resetship dialog text: lit literal of a not-exact function
  0x466B20 WW3D::IsSortingEnabled where the client reads AreStaticSortListsEnabled: rel data (the data map needs
           every library's objects of one build: its root as $BEHAUDIT_LIBOBJS, else skipped)
  0x7AC130 galaxy_map_render passes the wrong two flags to 0x7AD090: args (with the listing)
  0x6D64C0 FontSys::Update_Text's Scale * 480.0f * Unknown2C8 / Height reassociated by VC6 (fild U; fmul Scale;
           fmul 480 where the client has fld Scale; fmul 480; fimul U): fpu (fixture fpu/fontsys.obj, run/2026-11-25's
           head a9fa08ee3, before B3's de43e31f0; the function has no marker: paired through not-exact.md)
Not reproduced: 0x56DF30 / 0x56DF70 (MemberCallback Sender_Type / Event_Type typed wrong). The buggy instances are
other template instances, so their mangled names are not the client's symbols.json names and nothing pairs them:
MemberCallback descriptors need the vtable-slot check workflow.md's trap names, not this audit.
"""
import os
import sys
import types
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import behaudit as B  # noqa: E402

MAIN = 'C:/EnBDevelopment/EnB'
FIX = os.environ.get('BEHAUDIT_FIXTURES') or next(
    (p for p in (os.path.join(REPO, '.work', 'behaudit', 'fixtures'), MAIN + '/.work/behaudit/fixtures')
     if os.path.isdir(p)), '')
LIBS = os.environ.get('BEHAUDIT_LIBOBJS')     # a full build's root (every library's obj/ folder)
CONTROLS = ['5DED10', '557290', '7BEF50', '64BCA0', '470EF0', '8494C0', '74ABC0', '466B20']


def find(rows, check, addr, kind, ours=None):
    return [r for r in rows[check] if r['addr'] == '0x' + addr and r['kind'] == kind
            and (ours is None or ours in r['ours_val'])]


@unittest.skipUnless(FIX and os.path.isdir(os.path.join(FIX, 'lst')), 'fixtures missing: ' + str(FIX))
class PositiveControls(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rows, cls.stats, _ = B.run(('lit', 'rel', 'imm'), [os.path.join(FIX, 'lst')], libroot=LIBS or B.REPO + '/build',
                                       only={int(a, 16) for a in CONTROLS}, stale=False, quiet=True,
                                       known_path=None)

    def open_rows(self, check, addr, kind, ours=None):
        got = [r for r in find(self.rows, check, addr, kind, ours) if not r['auto']]
        self.assertTrue(got, (check, addr, kind, ours, [(r['kind'], r['ours_val'], r['auto'])
                                                         for r in self.rows[check] if r['addr'] == '0x' + addr]))
        return got

    def test_petition_names(self):
        got = self.open_rows('lit', '5DED10', 'literal', 'Mission_%02d_PT_MIS')
        self.assertIn("Mission%02d_PT_MIS", got[0]['client_val'])
        self.assertEqual(got[0]['status'], 'exact')
        self.assertEqual(len(find(self.rows, 'lit', '5DED10', 'literal')), 5)

    def test_float_as_bool(self):
        got = self.open_rows('imm', '557290', 'immreg')
        self.assertTrue(all(r['note'] == 'reg=0x0' for r in got))

    def test_create_peek_get(self):
        self.open_rows('rel', '7BEF50', 'call', 'Create_Peek')
        self.open_rows('rel', '64BCA0', 'call', 'Create_Get')

    def test_float_bytes(self):
        self.assertEqual(len(self.open_rows('lit', '470EF0', 'constant')), 3)
        self.assertEqual(len(self.open_rows('lit', '8494C0', 'constant')), 9)

    def test_not_exact_literal(self):
        got = self.open_rows('lit', '74ABC0', 'literal')
        self.assertEqual(got[0]['status'], 'notexact')

    @unittest.skipUnless(LIBS, 'no libraries\' objects for the data map')
    def test_data_map(self):
        got = self.open_rows('rel', '466B20', 'data', 'IsSortingEnabled')
        self.assertEqual(got[0]['client_val'], '0x%06X' % 0xBFAE50)


@unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'posctl', 'galaxy_map_render.asm')), 'fixtures missing')
class ArgumentControl(unittest.TestCase):
    def test_wrong_flags(self):
        rows, _, _ = B.run(('args',), [os.path.join(FIX, 'posctl')], only={0x7AC130}, stale=False, quiet=True,
                           known_path=None)
        got = [r for r in rows['args'] if not r['auto'] and '7AD090' in r['client']]
        self.assertTrue(got, rows['args'])
        self.assertEqual(got[0]['kind'], 'arg1')


@unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'lst', 'sclient.obj')), 'fixtures missing')
class SwitchControl(unittest.TestCase):
    """No session 0 case had a wrong case target, so the control is made: the client's jump table of the exact
    SpaceClientClass::Unknown_743290 (0x7432F4) read with its first two entries swapped."""
    def test_swapped_cases(self):
        ctx = B.Context([os.path.join(FIX, 'lst')], stale=False)
        ctx.paths = [p for p in ctx.paths if p.endswith('sclient.obj')]
        funcs, _ = B.pair_functions(ctx, {0x743290})
        f = funcs[0]
        self.assertTrue(f.exact)
        selfrel = B.self_relocs(f.o, f.sec, f.s, f.e)
        self.assertEqual(B.switch_rows(ctx, f, selfrel), [])
        u32 = ctx.u32
        ctx.u32 = lambda va: u32({0x7432F4: 0x7432F8, 0x7432F8: 0x7432F4}.get(va, va))
        got = B.switch_rows(ctx, f, selfrel)
        self.assertEqual(len(got), 1)
        self.assertTrue(got[0]['ours_val'].startswith('cases 0,1'), got[0])


@unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'fpu', 'fontsys.obj')), 'fixtures missing')
class FpuControl(unittest.TestCase):
    def test_reassociated_chain(self):
        rows, _, _ = B.run(('fpu',), [os.path.join(FIX, 'fpu')], only={0x6D64C0}, stale=False, quiet=True,
                           known_path=None)
        got = [r for r in rows['fpu'] if not r['auto']]
        self.assertEqual(len(got), 1, rows['fpu'])
        self.assertEqual(got[0]['client_va'], '6d664d')


def exact_func(dirname, objname, addr):
    ctx = B.Context([os.path.join(FIX, dirname)], stale=False)
    ctx.paths = [p for p in ctx.paths if p.endswith(objname)]
    funcs, _ = B.pair_functions(ctx, {addr})
    return ctx, funcs[0]


def patched_image(ctx, edits):
    """The client image with some bytes changed ({va: bytes}), for made controls."""
    data = bytearray(ctx.img.data)
    for va, b in edits.items():
        o = ctx.img.offset(va)
        data[o:o + len(b)] = b
    ctx.img.data = bytes(data)


@unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'lst', 'sclient.obj')), 'fixtures missing')
class TablesControl(unittest.TestCase):
    """tables (A7): an exact function's case values sent to another body. Made controls on the client's tables: the
    exact SpaceClientClass::Unknown_743290's jump table 0x7432F4 with its first two entries swapped (bodies that differ), and
    the exact starmap_data Unknown_7BF4C0's byte index table (a two-level switch) with two indexes swapped."""

    def test_clean(self):
        ctx, f = exact_func('lst', 'sclient.obj', 0x743290)
        self.assertTrue(f.exact)
        self.assertEqual(B.tables_rows(ctx, f), [])

    def test_swapped_jump_entries(self):
        ctx, f = exact_func('lst', 'sclient.obj', 0x743290)
        a, b = ctx.u32(0x7432F4), ctx.u32(0x7432F8)
        patched_image(ctx, {0x7432F4: b.to_bytes(4, 'little'), 0x7432F8: a.to_bytes(4, 'little')})
        got = B.tables_rows(ctx, f)
        self.assertEqual(len(got), 1, got)
        self.assertEqual(got[0]['kind'], 'body')
        self.assertEqual(got[0]['ours_val'], 'cases 0,1')
        self.assertEqual(got[0]['auto'], '')

    def test_byte_index(self):
        ctx, f = exact_func('lst', 'starmap_data.obj', 0x7BF4C0)
        self.assertEqual(B.tables_rows(ctx, f), [])
        sw = B.our_switches(f, B.self_relocs(f.o, f.sec, f.s, f.e))
        t, idx, jt, bt = sw[0]
        self.assertIsNotNone(bt)
        # two values whose bodies differ: swap their indexes in the client's byte table
        v, w = next((v, w) for v in range(len(bt)) for w in range(v + 1, len(bt)) if jt[bt[v]] != jt[bt[w]])
        base = f.addr + idx
        patched_image(ctx, {base + v: bytes([bt[w]]), base + w: bytes([bt[v]])})
        got = B.tables_rows(ctx, f)
        self.assertEqual(len(got), 1, got)
        self.assertEqual(got[0]['kind'], 'body')
        self.assertIn('byte index', got[0]['note'])

    def test_same_offset_walk(self):
        """Every case body compared with itself is the same code, fields at the same offset compared too."""
        ctx, f = exact_func('lst', 'sclient.obj', 0x743290)
        oc = B.OurCode(f)
        keep = B.DEFER_SAME
        B.DEFER_SAME = False
        try:
            for t, idx, jt, bt in B.our_switches(f, B.self_relocs(f.o, f.sec, f.s, f.e)):
                for a in set(jt):
                    self.assertNotEqual(B.same_body(ctx, oc, a, f.addr + a)[0], False, hex(a))
        finally:
            B.DEFER_SAME = keep


@unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'tables', 'ui_analyze.obj')), 'fixtures missing')
class TablesReordered(unittest.TestCase):
    """B14's masked-exact form ch_d1 of UIAnalyzeClass::Unknown_545190 (trybatch v545190a, run 2026-11-26): 6 of its
    12 jump-table entries reach other offsets than the client's, its message bodies laid out in another order with
    their strings permuted to match, so each value still reaches the client's message: `copy`, codegen
    (fixture tables/ui_analyze.obj, from .work/wt/B14/.work/trybatch/v545190a-081318/ch_d1)."""

    def test_copy(self):
        rows, _, _ = B.run(('tables',), [os.path.join(FIX, 'tables')], only={0x545190}, stale=False, quiet=True,
                           known_path=None)
        got = rows['tables']
        self.assertEqual(len(got), 1, got)
        self.assertEqual(got[0]['kind'], 'copy')
        self.assertEqual(got[0]['auto'], 'codegen')
        self.assertTrue(got[0]['note'].startswith('6 of 12 values reach another offset: 6 same code'), got[0])

    def test_permuted_strings_matter(self):
        """The same form with the client's table read as ours (each value at our offset): the bodies there push
        other messages, so the walk must say they differ (the strings decide, not the masked bytes)."""
        ctx, f = exact_func('tables', 'ui_analyze.obj', 0x545190)
        sw = B.our_switches(f, B.self_relocs(f.o, f.sec, f.s, f.e))
        t, idx, jt, bt = sw[0]
        jc = [ctx.u32(f.addr + t + 4 * i) - f.addr for i in range(len(jt))]
        oc = B.OurCode(f)
        moved = [(a, b) for a, b in zip(jt, jc) if a != b]
        self.assertEqual(len(moved), 6)
        for a, b in moved:
            self.assertIs(B.same_body(ctx, oc, a, f.addr + b)[0], True)
            self.assertIs(B.same_body(ctx, oc, a, f.addr + a)[0], True)     # same offset: the rel check's
        keep = B.DEFER_SAME
        B.DEFER_SAME = False
        try:
            self.assertTrue(all(B.same_body(ctx, oc, a, f.addr + a)[0] is False for a, _ in moved))
        finally:
            B.DEFER_SAME = keep


@unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'lst', 'sclient.obj')), 'fixtures missing')
class LedgerPlacement(unittest.TestCase):
    """The ledger fallback every per-function check shares (A7's tables placement, AT1): exact functions the pairing
    leaves out, as when symbols.json does not name them yet, placed at their own object's ledger addresses."""

    @classmethod
    def setUpClass(cls):
        cls.ctx = B.Context([os.path.join(FIX, 'lst')], stale=False)
        cls.ctx.paths = [p for p in cls.ctx.paths if p.endswith('sclient.obj')]
        cls.funcs, _ = B.pair_functions(cls.ctx)
        cls.exact = [f for f in cls.funcs if f.exact and not getattr(f, 'tied', None)]

    def test_places_left_out(self):
        gone = {(f.name, f.addr) for f in self.exact}
        rest = [f for f in self.funcs if (f.name, f.addr) not in gone]
        files = {'sclient': {'in_tree': ['0x%06X' % a for _, a in gone] + ['0x401000']}}
        placed = B.ledger_funcs(self.ctx, rest, files=files)
        got = {(f.name, f.addr) for f in placed}
        self.assertTrue(got <= gone, got - gone)
        self.assertGreater(len(got), len(gone) // 3, (len(got), len(gone)))   # short or twin copies stay out
        self.assertTrue(all(f.exact and f.ledger for f in placed))
        f = next(f for f in placed if f.addr == 0x743290)
        self.assertTrue(f.switch)
        sw = B.ledger_funcs(self.ctx, rest, files=files, switch_only=True)
        self.assertIn(0x743290, {g.addr for g in sw})
        self.assertTrue(all(g.switch for g in sw))
        self.assertLess(len(sw), len(placed))
        # the summary line counts each check's gain
        rows = {'lit': [{'addr': '0x743290'}, {'addr': '0x401000'}], 'tables': [], 'args': []}
        line = B.ledger_line(placed, rows)
        self.assertIn('lit %d functions 1 rows' % len(placed), line)
        self.assertIn('tables %d functions 0 rows' % len(sw), line)
        self.assertIn('args 0 functions', line)

    def test_data_map_unique_names_only(self):
        import collections
        objs = {'a.obj': [('?f@@YAXXZ', 1, 0, 9), ('$E15', 1, 9, 20), ('?s@@YAXXZ', 1, 20, 30)],
                'b.obj': [('$E15', 1, 0, 9), ('?s@@YAXXZ', 1, 9, 20)]}
        ctx = types.SimpleNamespace(paths=list(objs), stale=set(), exact_syms=collections.defaultdict(set),
                                    obj=lambda p: types.SimpleNamespace(functions=lambda: objs[p]))
        placed = [types.SimpleNamespace(name=n, addr=a) for n, a in
                  (('?f@@YAXXZ', 0x401000), ('$E15', 0x402000), ('?s@@YAXXZ', 0x403000))]
        self.assertEqual(B.add_to_data_map(ctx, placed), 1)
        self.assertEqual(dict(ctx.exact_syms), {'?f@@YAXXZ': {0x401000}})

    def test_not_in_ledger(self):
        rest = [f for f in self.funcs if f.addr != 0x743290]
        self.assertEqual([], [f for f in B.ledger_funcs(self.ctx, rest, files={'sclient': {'in_tree': ['0x401000']}})
                              if f.addr == 0x743290])
        self.assertEqual(B.ledger_funcs(self.ctx, rest, files={}), [])
        # already paired: the pairing's own functions are never placed again
        self.assertEqual(B.ledger_funcs(self.ctx, self.funcs, files={'sclient': {'in_tree': ['0x743290']}}), [])


class Pieces(unittest.TestCase):
    def r(self, **k):
        base = dict.fromkeys(B.COLS, '')
        base.update(k)
        return base

    def test_auto_class(self):
        self.assertEqual(B.auto_class(self.r(kind='imm', note='shifted')), 'noise')
        self.assertEqual(B.auto_class(self.r(kind='immreg', note='reg=same')), 'codegen')
        self.assertEqual(B.auto_class(self.r(kind='imm', note='esp')), 'codegen')
        self.assertEqual(B.auto_class(self.r(kind='immreg', note='reg=0x0')), '')
        self.assertEqual(B.auto_class(self.r(kind='lit-extra')), 'noise')
        self.assertEqual(B.auto_class(self.r(kind='literal')), '')

    def test_known(self):
        known = [['lit', '0x5DED10', 'literal', '*', '*', 'real-fixed', 'P2'],
                 ['imm', '*', 'imm', '0x1*', '0x2', 'codegen', 'x']]
        self.assertEqual(B.known_of(self.r(check='lit', addr='0x5DED10', kind='literal', ours_val='a'), known),
                         'real-fixed')
        self.assertEqual(B.known_of(self.r(check='imm', addr='0x1', kind='imm', ours_val='0x10', client_val='0x2'),
                                    known), 'codegen')
        self.assertEqual(B.known_of(self.r(check='imm', addr='0x1', kind='imm', ours_val='0x20', client_val='0x2'),
                                    known), '')

    def test_reg_value(self):
        def rr(raw, imm=None):
            return {'raw': raw, 'mn': raw.split()[0], 'imm': imm}
        rows = [rr('xor ebx, ebx'), rr('mov eax, 5', 5), rr('push ebx')]
        self.assertEqual(B.reg_value(rows, 2, 'ebx'), 0)
        self.assertEqual(B.reg_value(rows, 2, 'eax'), 5)
        self.assertIsNone(B.reg_value(rows + [rr('call 0x10'), rr('push eax')], 4, 'eax'))

    def test_shape(self):
        self.assertEqual(B.shape('push 0x20'), B.shape('push 1'))
        self.assertNotEqual(B.shape('push 0x20'), B.shape('push ebx'))

    def test_empty_literal(self):
        sym = {'name': '??_C@_00A@?$AA@', 'section': 1, 'value': 0}
        self.assertEqual(B.our_literal(types.SimpleNamespace(literal=lambda s: None), sym), b'\0')


if __name__ == '__main__':
    unittest.main()
