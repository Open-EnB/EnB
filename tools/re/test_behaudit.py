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
    SClient::Unknown_743290 (0x7432F4) read with its first two entries swapped."""
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
