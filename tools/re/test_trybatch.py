"""trybatch.py's pieces on fixtures, no VM (python -m unittest tools/re/test_trybatch.py, or run it); also
attempts.py's D271 progress test with trybatch's `residue N` evidence."""
import difflib
import os
import sys
import tempfile
import types
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import attempts  # noqa: E402
import trybatch as T  # noqa: E402
import explain as X  # noqa: E402

SRC = ('// head\n'
       'int A::f(int x)\n'
       '{\n'
       '\tconst char *s = "}{";  // } in a comment\n'
       "\tchar c = '}';\n"
       '\t/* { */ return x + 1;\n'
       '}\n'
       '\n'
       'int A::g()\n'
       '{\n'
       '\treturn 2;\n'
       '}\n')


def ins(mn, target=None, addr=0, raw=None, refs=()):
    return types.SimpleNamespace(mn=mn, target=target, addr=addr, raw=raw or mn, refs=list(refs), op='')


def row(text, target=None, addr=0):
    mn = text.split()[0]
    return ins(mn, target, addr, text), (f'{mn} T' if target else text)


class Edits(unittest.TestCase):
    def read(self, files):
        return lambda rel: files[rel]

    def test_hunks(self):
        files = {'app/a.cpp': SRC, 'app/a.h': 'struct A { int f(int); };\n'}
        out = T.apply_form(self.read(files), 'app/a.cpp', [('return 2;', 'return 3;'),
                                                          ('app/a.h', 'int f(int);', 'int f(int); int g();')])
        self.assertIn('return 3;', out['app/a.cpp'])
        self.assertIn('int g();', out['app/a.h'])
        with self.assertRaises(ValueError):
            T.apply_form(self.read(files), 'app/a.cpp', [('return', 'x')])   # twice in the file

    def test_crlf(self):
        files = {'a.cpp': SRC.replace('\n', '\r\n')}
        out = T.apply_form(self.read(files), 'a.cpp', [('{\n\treturn 2;\n}', '{\n\treturn 4;\n}')])
        self.assertIn('{\r\n\treturn 4;\r\n}', out['a.cpp'])

    def test_whole_function(self):
        body = '// @0x401000\nint  A::f( int x )\n{\n\treturn x - 1;\n}\n'
        out = T.replace_function(SRC, body)
        self.assertIn('int  A::f( int x )\n{\n\treturn x - 1;\n}\n\nint A::g()', out)
        self.assertNotIn('"}{"', out)
        self.assertEqual(out.count('// @0x401000'), 0)        # leading comment lines are dropped
        self.assertTrue(out.startswith('// head\n'))
        with self.assertRaises(ValueError):
            T.replace_function(SRC, 'int A::h()\n{\n}\n')

    def test_load_spec(self):
        with tempfile.TemporaryDirectory() as d:
            p = os.path.join(d, 'v.py')
            with open(p, 'w') as f:
                f.write("FILE = 'app\\\\x.cpp'\nSYMBOL = 'A::f'\nADDRESS = '0x401000'\n"
                        "V = {'one': [('app/x.cpp', 'a', 'b')]}\nHYPOTHESIS = '  two\\n words '\n")
            s = T.load_spec(p)
        self.assertEqual((s['file'], s['address'], s['hypothesis'], list(s['forms'])),
                         ('app/x.cpp', 0x401000, 'two words', ['one']))

    def test_case_colliding_form_names(self):
        with tempfile.TemporaryDirectory() as d:
            p = os.path.join(d, 'v.py')
            with open(p, 'w') as f:
                f.write("FILE = 'app/x.cpp'\nSYMBOL = 'A::f'\nADDRESS = 0x401000\n"
                        "FORMS = {'foo': [], 'Foo': []}\n")
            with self.assertRaises(SystemExit) as ctx:
                T.load_spec(p)
            msg = str(ctx.exception)
            self.assertIn("'foo'", msg)
            self.assertIn("'Foo'", msg)
            with open(p, 'w') as f:
                f.write("FILE = 'app/x.cpp'\nSYMBOL = 'A::f'\nADDRESS = 0x401000\n"
                        "FORMS = {'Base': []}\n")
            with self.assertRaises(SystemExit) as ctx:
                T.load_spec(p)
            msg = str(ctx.exception)
            self.assertIn("'base'", msg)
            self.assertIn("'Base'", msg)


class Regions(unittest.TestCase):
    def test_equal_counts_pair_by_index(self):
        L = [row('push 1'), row('call T', ('sym', 'x')), row('mov eax, 1'), row('call T', ('sym', 'y')), row('ret')]
        R = [row('push 1', addr=1), row('call T', ('addr', 0x500000), 2), row('mov eax, 2', addr=3),
             row('call T', ('addr', 0x500010), 4), row('ret', addr=5)]
        rs = T.region_rows(L, R, False, False, known={})
        self.assertEqual([(r['ours'], r['client'], r['res']) for r in rs], [(2, 2, 0), (2, 2, 1), (1, 1, 0)])
        self.assertEqual(rs[1]['at'], 3)

    def test_unequal_counts_align_on_known_targets(self):
        known = {'?a@@': {0x500000}, '?b@@': {0x500020}}
        L = [row('call T', ('sym', '?a@@')), row('nop'), row('call T', ('sym', '?b@@')), row('ret')]
        R = [row('call T', ('addr', 0x500000)), row('call T', ('addr', 0x500010)), row('call T', ('addr', 0x500020)),
             row('ret')]
        spans = [r[:4] for r in T.regions(L, R, known)]
        self.assertEqual(spans, [(0, 1, 0, 1), (1, 3, 1, 3), (3, 4, 3, 4)])

    def test_show_region(self):
        L = [row('mov ecx, dword ptr [esi + 4]'), row('push ecx')]
        R = [row('mov eax, dword ptr [esi + 4]', addr=0x82A5BB), row('push eax', addr=0x82A5BE)]
        out = T.show_region(L, R, False, False, (0, 2, 0, 2))
        self.assertTrue(all(x.startswith('*') for x in out))
        self.assertIn('82a5bb', out[0])


def result(res, regs, key, cls='shape', frame=0x10):
    return {'res': res, 'regs': regs, 'raw': res, 'frame': frame, 'frame_client': 0x10, 'ours': 100, 'client': 100,
            'score': T.score_of(res, 100, 100), 'class': cls, 'key': key, 'exact': False,
            'regions': [{'k': 0, 'ours': 50, 'client': 50, 'res': res, 'regs': regs, 'at': 0x401000, 'label': 'end'}]}


class Summary(unittest.TestCase):
    def test_sorted_grouped_short(self):
        spec = {'symbol': 'A::f', 'address': 0x401000, 'file': 'app/a.cpp', 'hypothesis': 'h'}
        results = {'base': result(16, 14, 'K0'), 'same_as_base': result(16, 14, 'K0'), 'worse': result(18, 18, 'K1'),
                   'best': result(2, 0, 'K2', 'register-only'), 'twin': result(2, 0, 'K2', 'register-only'),
                   'broken': {'error': 'x.cpp(3) : error C2065'}}
        for k in range(30):
            results[f'f{k}'] = result(20 + k, 20, f'Z{k}')
        order = ['base', 'same_as_base', 'worse', 'best', 'twin', 'broken'] + [f'f{k}' for k in range(30)]
        lines, best = T.summary(spec, results, order)
        self.assertEqual(best, 'best')
        self.assertLessEqual(len(lines), 32)
        self.assertTrue(lines[2].startswith('best ') and lines[2].endswith('= twin'))
        self.assertIn('identical to base: same_as_base', lines)
        self.assertTrue(any('COMPILE FAILED' in l for l in lines))
        self.assertTrue(any(l.startswith('... ') for l in lines))

    def test_score(self):
        self.assertAlmostEqual(T.score_of(2, 127, 127), 1 - 2 / 127)
        self.assertEqual(T.score_of(500, 10, 20), 0.0)

    def test_code_key(self):
        a = [row('call T', ('sym', 'x'))]
        b = [row('call T', ('sym', 'y'))]
        self.assertNotEqual(T.code_key(a), T.code_key(b))
        self.assertEqual(T.code_key(a), T.code_key([row('call T', ('sym', 'x'))]))


LISTING = '''_TEXT\tSEGMENT
_port$ = 8
_address$ = -16
?Listen@TCPListenerClass@@UAE_NG@Z PROC NEAR\t\t; TCPListenerClass::Listen, COMDAT
; 664  : {
\tsub\tesp, 16\t\t\t\t\t; 00000010H
\tmov\tax, WORD PTR _port$[esp+12]
$L123:
\tlea\teax, DWORD PTR _address$[esp+20]
\tret\t4
?Listen@TCPListenerClass@@UAE_NG@Z ENDP
'''


class Slots(unittest.TestCase):
    def test_listing_and_map(self):
        text = '?x@TCPListenerClass@@QAE@XZ PROC NEAR ; TCPListenerClass::x\nENDP\n' + LISTING
        table, lines = T.listing_body(text, 'TCPListenerClass::Listen')
        self.assertEqual(table, {'_port$': 8, '_address$': -16})
        self.assertEqual([l.split()[0] for l in lines], ['sub', 'mov', 'lea', 'ret'])
        L = [row('sub esp, 0x10'), row('mov ax, word ptr [esp + 0x14]'), row('lea eax, [esp + 4]'), row('ret 4')]
        R = [row('sub esp, 0x10'), row('mov ax, word ptr [esp + 0x14]'), row('lea eax, [esp + 8]'), row('ret 4')]
        sm = difflib.SequenceMatcher(None, ['a', 'b', 'c', 'd'], ['a', 'b', 'c', 'd'])
        rows = {name: c for _, name, c in T.slot_map(table, lines, L, R, sm)}
        self.assertEqual(dict(rows['_port$']), {8: 1})
        self.assertEqual(dict(rows['_address$']), {-12: 1})
        out = T.slot_lines(T.slot_map(table, lines, L, R, sm))
        self.assertTrue([l for l in out if '_address$' in l][0].endswith('*'))


REP_LISTING = '''_TEXT\tSEGMENT
_name$ = -64
?Copy@A@@QAEXPBD@Z PROC NEAR\t\t\t; A::Copy, COMDAT
\tsub\tesp, 64\t\t\t\t\t; 00000040H
\tor\tecx, -1
\txor\teax, eax
\trepne scasb
\tnot\tecx
\tlea\tedi, DWORD PTR _name$[esp+64]
\trep movsd
\tmov\tecx, eax
\trep movsb
\tlea\teax, DWORD PTR _name$[esp+64]
\tret\t4
?Copy@A@@QAEXPBD@Z ENDP
'''


class Prefixes(unittest.TestCase):
    """A4: capstone names `repne scasb` one mnemonic, the listing's first token is `repne`: a prefixed row must still
    align, else a function with inlined strlen / memcpy loses its listing (behaudit args) or its slot rows."""

    CLIENT = ['sub esp, 0x40', 'or ecx, 0xffffffff', 'xor eax, eax', 'repne scasb al, byte ptr es:[edi]',
              'not ecx', 'lea edi, [esp]', 'rep movsd dword ptr es:[edi], dword ptr [esi]', 'mov ecx, eax',
              'rep movsb byte ptr es:[edi], byte ptr [esi]', 'lea eax, [esp]', 'ret 4']

    def rows(self):
        out = []
        for text in self.CLIENT:
            mn, _, op = text.partition(' ')
            if mn in T.PREFIXES:
                more, _, op = op.partition(' ')
                mn = mn + ' ' + more
            r = ins(mn, raw=text)
            r.op = op
            out.append((r, text))
        return out

    def test_keys(self):
        self.assertEqual(T.mnemonic_key('repne scasb'), 'repne scasb')
        self.assertEqual(T.mnemonic_key('rep movsd dword ptr es:[edi], dword ptr [esi]'), 'rep movsd')
        self.assertEqual(T.mnemonic_key('mov\tecx, eax'), 'mov')
        self.assertTrue(T.same_mnemonic('rep movsd', 'rep movsd'))
        self.assertFalse(T.same_mnemonic('rep movsd', 'rep movsb'))
        self.assertFalse(T.same_mnemonic('rep stosd', 'stosd'))
        self.assertEqual(T.mnemonic_key(''), '')

    def test_listing_aligns(self):
        table, lines = T.listing_body(REP_LISTING, 'A::Copy')
        L = self.rows()
        self.assertEqual(len(lines), len(L))
        # behaudit_args's 2 % limit: no prefixed row counts as misaligned
        self.assertEqual(sum(1 for k in range(len(L)) if not T.same_mnemonic(lines[k], L[k][0].mn)), 0)
        self.assertEqual(sum(1 for k in range(len(L)) if lines[k].split()[0] != L[k][0].mn), 3)   # the old test
        n = len(L)
        sm = difflib.SequenceMatcher(None, list(range(n)), list(range(n)))
        rows = {name: c for _, name, c in T.slot_map(table, lines, L, L, sm)}
        self.assertEqual(dict(rows['_name$']), {-64: 2})


FIX = os.environ.get('BEHAUDIT_FIXTURES') or next(
    (p for p in (os.path.join(os.path.dirname(os.path.dirname(HERE)), '.work', 'behaudit', 'fixtures'),
                 'C:/EnBDevelopment/EnB/.work/behaudit/fixtures') if os.path.isdir(p)), '')


class TablesVerdict(unittest.TestCase):
    """The TABLES flag on a masked-exact form, copy / body apart through behaudit's tables check (A7's proposal)."""

    def test_verdict_of(self):
        self.assertEqual(T.verdict_of([]), 'same bodies')
        self.assertEqual(T.verdict_of([{'kind': 'copy', 'ours_val': 'cases 1'}]), 'copy')
        self.assertEqual(T.verdict_of([{'kind': 'copy', 'ours_val': 'cases 1'},
                                       {'kind': 'unknown', 'ours_val': 'cases 2'}]), 'unknown')
        self.assertEqual(T.verdict_of([{'kind': 'unknown', 'ours_val': 'cases 2'},
                                       {'kind': 'body', 'ours_val': 'cases 0,1'}]), 'BODY cases 0,1')

    def test_flag_text(self):
        m = {'res': 0, 'regs': 0, 'raw': 0, 'frame': 0, 'frame_client': 0, 'ours': 9, 'client': 9, 'score': 1.0,
             'class': 'exact', 'tables': [{'kind': 'jump', 'diff': 6, 'n': 12}], 'tables_verdict': 'copy'}
        self.assertTrue(T.fmt_form('f', m).endswith('TABLES jump 6/12 = copy'))
        del m['tables_verdict']
        self.assertTrue(T.fmt_form('f', m).endswith('TABLES jump 6/12'))

    @unittest.skipUnless(FIX and os.path.exists(os.path.join(FIX, 'tables', 'ui_analyze.obj'))
                         and os.path.exists(os.path.join(FIX, 'lst', 'sclient.obj')), 'fixtures missing')
    def test_fixtures(self):
        # B14's ch_d1 of 0x545190: 6 of 12 entries reordered, each reaching the same code: a copy
        obj = os.path.join(FIX, 'tables', 'ui_analyze.obj')
        m = T.measure(obj, 'Unknown_545190', 0x545190)
        self.assertTrue(m['exact'])
        self.assertEqual(m['tables_verdict'], 'copy')
        self.assertIn('= copy', T.fmt_form('ch_d1', m))
        # the exact SClient::Unknown_743290 with the client's first two jump entries swapped: another body
        ctx = T._TABLES_CTX[0]
        a, b = ctx.u32(0x7432F4), ctx.u32(0x7432F8)
        keep = ctx.img.data
        data = bytearray(keep)
        o = ctx.img.offset(0x7432F4)
        data[o:o + 8] = b.to_bytes(4, 'little') + a.to_bytes(4, 'little')
        ctx.img.data = bytes(data)
        try:
            got = T.tables_verdict(os.path.join(FIX, 'lst', 'sclient.obj'), 'Unknown_743290', 0x743290)
        finally:
            ctx.img.data = keep
        self.assertEqual(got, 'BODY cases 0,1')
        self.assertEqual(T.tables_verdict(os.path.join(FIX, 'lst', 'sclient.obj'), 'Unknown_743290', 0x743290),
                         'same bodies')


def arow(date, score, evidence='', model='opus'):
    return {'date': date, 'address': '0x401000', 'status': 'partial', 'score': score, 'evidence': evidence,
            'model': model}


class Progress(unittest.TestCase):
    def test_residue_beats_score(self):
        # the score stays at 0.98 while the residue drops: progress, not a stalled round
        rows = [arow('2026-11-19 08:00', '0.98', 'trybatch a: best x residue 315 regs 3', 'opus-medium'),
                arow('2026-11-20 08:00', '0.98', 'trybatch b: best y residue 309 regs 3', 'opus-high'),
                arow('2026-11-21 08:00', '0.98', 'trybatch c: best z residue 309; base residue 400', 'opus-high')]
        self.assertEqual(attempts.limit_reached(rows), '')
        rows.append(arow('2026-11-22 08:00', '0.99', 'trybatch d: best z residue 310', 'opus-high'))
        self.assertIn('LIMIT REACHED', attempts.limit_reached(rows))   # a higher score but no fewer rows

    def test_score_without_residue(self):
        rows = [arow('2026-11-19 08:00', '0.50'), arow('2026-11-20 08:00', '0.50'), arow('2026-11-21 08:00', '0.50')]
        self.assertIn('LIMIT REACHED', attempts.limit_reached(rows))
        self.assertEqual(attempts.residue_of(arow('x', '', 'best f residue 12 regs 3; base residue 40')), 12)
        self.assertIsNone(attempts.residue_of(arow('x', '', 'no figures')))


def xrow(addr, raw, size=2, target=None, disp_off=0):
    """A disassembled row as explain.py builds it: (Ins-like, masked text)."""
    mn = raw.split()[0]
    i = types.SimpleNamespace(addr=addr, raw=raw, mn=mn, op=raw[len(mn):].strip(), size=size, target=target,
                              refs=[], disp_off=disp_off)
    return i, (f'{mn} T' if target else raw)


class Tables(unittest.TestCase):
    """1. (NX1, SR3) a switch's jump and byte tables end the code; their entries are compared apart."""

    def client(self):
        # 0x1000: code, a two-level switch, ret; the tables at 0x1010 (jump, 3 entries) and 0x101C (byte, 4)
        R = [xrow(0x1000, 'mov cl, byte ptr [eax + 0x101c]', 6), xrow(0x1006, 'jmp dword ptr [ecx*4 + 0x1010]', 7),
             xrow(0x100D, 'ret', 1), xrow(0x100E, 'nop', 1), xrow(0x100F, 'nop', 1),
             xrow(0x1010, 'adc al, 0x10', 2), xrow(0x1012, 'add byte ptr [eax], al', 2),
             xrow(0x101C, 'add byte ptr [ecx], al', 2), xrow(0x101E, 'add byte ptr [edx], al', 2)]
        data = {0x1010: [0x100D, 0x100E, 0x100D], 0x101C: [0, 1, 1, 2]}

        def entries(t, kind, size, last):
            return data[t]
        return R, entries

    def test_refs_and_split(self):
        R, entries = self.client()
        where = lambda i: int(i.raw.split('+ ')[1].rstrip(']'), 16) if '+ 0x10' in i.raw else None   # noqa: E731
        refs = X.table_refs(R, where)
        self.assertEqual(refs, [(0x1010, 'jump'), (0x101C, 'byte')])
        code, tables = X.split_tables(R, refs, 0x1020, entries)
        self.assertEqual([i.addr for i, _ in code], [0x1000, 0x1006, 0x100D, 0x100E, 0x100F])
        self.assertEqual(tables[0], {'kind': 'jump', 'at': 0x1010, 'entries': [2, 3, 2]})   # row indices
        self.assertEqual(tables[1]['entries'], [0, 1, 1, 2])

    def test_compare_by_rank(self):
        # ours' code is longer (other row indices) but the cases share bodies the same way: equal
        ours = [{'kind': 'jump', 'at': 0x40, 'entries': [7, 9, 7]}, {'kind': 'byte', 'at': 0x50, 'entries': [0, 1, 1, 2]}]
        client = [{'kind': 'jump', 'at': 0x1010, 'entries': [2, 3, 2]},
                  {'kind': 'byte', 'at': 0x101C, 'entries': [0, 1, 2, 2]}]
        got = X.compare_tables(ours, client)
        self.assertEqual([(t['kind'], t['n'], t['diff']) for t in got], [('jump', 3, 0), ('byte', 4, 1)])
        self.assertEqual(got[0]['at'], 0x1010)
        self.assertEqual(X.compare_tables([], client)[0]['diff'], 3)     # a table on one side only

    def test_no_tables(self):
        rows = [xrow(0, 'ret', 1)]
        self.assertEqual(X.split_tables(rows, [], 1, None), (rows, []))

    def test_summary_prints_tables_apart(self):
        spec = {'symbol': 'A::f', 'address': 0x401000, 'file': 'app/a.cpp', 'hypothesis': 'h'}
        r = result(0, 0, 'K0')
        r['tables'] = [{'kind': 'jump', 'at': 0x401100, 'ours': 7, 'client': 7, 'n': 7, 'diff': 0},
                       {'kind': 'byte', 'at': 0x40111C, 'ours': 19, 'client': 19, 'n': 19, 'diff': 2}]
        lines, best = T.summary(spec, {'base': r}, ['base'])
        self.assertIn('  switch jump table @0x401100 entries 7/7 equal', lines)
        self.assertIn('  switch byte table @0x40111C entries 19/19 2 different', lines)
        self.assertIn('TABLES byte 2/19', T.fmt_form('base', r))


class Branches(unittest.TestCase):
    """2. (SR2) a short jump made near, or a jump to another row, under masked-equal text."""

    @staticmethod
    def rows(size, to_row, base=0, client=False):
        """test; je (size bytes) -> row to_row; inc; ret; xor; ret, at consecutive addresses from base."""
        spec = [('test eax, eax', 2), ('je T', size), ('inc eax', 2), ('ret', 1), ('xor eax, eax', 2), ('ret', 1)]
        addrs, at = [], base
        for _, n in spec:
            addrs.append(at)
            at += n
        out = []
        for k, (raw, n) in enumerate(spec):
            target = (('addr' if client else 'local'), addrs[to_row]) if raw.startswith('je') else None
            out.append(xrow(addrs[k], raw, n, target))
        return out

    def measure(self, L, R):
        sm = difflib.SequenceMatcher(None, [t for _, t in L], [t for _, t in R], autojunk=False)
        return X.branch_shape(L, R, sm)

    def test_short_vs_near(self):
        b = self.measure(self.rows(6, 4), self.rows(2, 4, 0x500000, True))
        self.assertEqual((b['size'], b['target'], b['bytes']), (1, 0, (14, 10)))
        self.assertTrue(X.branch_differs(b))

    def test_other_target_and_same(self):
        b = self.measure(self.rows(2, 4), self.rows(2, 3, 0x500000, True))
        self.assertEqual((b['size'], b['target']), (0, 1))
        self.assertFalse(X.branch_differs(self.measure(self.rows(2, 4), self.rows(2, 4, 0x500000, True))))

    def test_flag_in_summary(self):
        r = result(0, 0, 'K0', 'branch')
        r['branch'] = {'bytes': (40, 44), 'size': 1, 'target': 0}
        self.assertIn('BRANCH(1s/0t 40/44b)', T.fmt_form('f', r))


SRC_MARKED = ('// NOT EXACT: order-only (the shadowed vertex; attempts.csv 0x46CAE0). more text\n'
              'void LandClass::Compute_Lighting(int a)\n{\n}\n'
              '// 0x570CD0  // NOT EXACT: the inlined 0x571A70\'s stores\n'
              'void ChatWindowInterface::Unknown_A4(void)\n{\n}\n'
              '\tPacket_00B3Class(RawPacketClass *raw);\t// 0x867250\n')


class Resolve(unittest.TestCase):
    """3. (SCAN) a stale or annotated SYMBOL, a header FILE, a base-only run."""

    NAMES = ['?Value_To_String@@YAPADPADHW4VerbEnum@@@Z', '??0UDPConnectionClass@@QAE@PBDG_N@Z',
             '??0UDPConnectionClass@@QAE@PAVCordCon@@PAVCordMgr@@@Z', '_$E28', '_$E29',
             '?Get@?$ClientObjectDataAccessClass@VRadarDataClass@@@@SAPAVRadarDataClass@@PAVClientObjectClass@@_N@Z',
             '?Get@?$ClientObjectDataAccessClass@VGutterDataClass@@@@SAPAVGutterDataClass@@PAVClientObjectClass@@_N@Z',
             '?Compute_Lighting@LandClass@@QAEXH@Z']
    INDEX = {
        '??0UDPConnectionClass@@QAE@PBDG_N@Z': ('public: __thiscall UDPConnectionClass::UDPConnectionClass(char const *,unsigned short,bool)', ['udpconn']),
        '??0UDPConnectionClass@@QAE@PAVCordCon@@PAVCordMgr@@@Z': ('public: __thiscall UDPConnectionClass::UDPConnectionClass(class CordCon *,class CordMgr *)', ['udpconn']),
        '?Get@?$ClientObjectDataAccessClass@VRadarDataClass@@@@SAPAVRadarDataClass@@PAVClientObjectClass@@_N@Z':
            ('public: static class RadarDataClass * __cdecl ClientObjectDataAccessClass<class RadarDataClass>::Get(class ClientObjectClass *,bool)', ['co_data']),
        '?Value_To_String@@YAPADPADHW4VerbEnum@@@Z': ('char * __cdecl Value_To_String(char *,int,enum VerbEnum)', ['unknown_verb'])}

    def cands(self, symbol, texts=()):
        return T.candidates(self.NAMES, symbol, 0x123456, texts, self.INDEX)

    def test_signatures(self):
        self.assertEqual(T.norm_sig('public: void __thiscall A<class B>::f(char *,int)'), ('A<B>::f', '(char*,int)'))
        self.assertEqual(self.cands('Value_To_String(char *, int, VerbEnum)')[0], [self.NAMES[0]])
        self.assertEqual(self.cands('UDPConnectionClass::UDPConnectionClass(CordCon *, CordMgr *)')[0], [self.NAMES[2]])
        self.assertEqual(self.cands('ClientObjectDataAccessClass<RadarDataClass>::Get')[0], [self.NAMES[5]])

    def test_annotation_and_markers(self):
        got, how = self.cands('_$E (PermStringInit)')
        self.assertEqual((got, how), (['_$E28', '_$E29'], 'name without its arguments'))
        got, how = self.cands('LandClass::Old_Name', [SRC_MARKED.replace('0x46CAE0', '0x123456')])
        self.assertEqual(got, ['?Compute_Lighting@LandClass@@QAEXH@Z'])
        self.assertIn("source's marker", how)

    def test_own_mentions(self):
        self.assertEqual(T.own_mentions(SRC_MARKED, 0x46CAE0), ['LandClass::Compute_Lighting'])
        self.assertEqual(T.own_mentions(SRC_MARKED, 0x571A70), [])          # mentioned, but not its own marker
        self.assertEqual(T.own_mentions(SRC_MARKED, 0x867250), ['Packet_00B3Class'])

    def test_class_marker(self):
        # TB3: a template member's marker inside its class body (co_data.h's Get, 0x79E300): the bare name is every
        # instance's, the class qualifies it and the comment's word picks the instance
        src = ('template <class T>\nclass ClientObjectDataAccessClass\n{\npublic:\n'
               '\tstatic T *Get(ClientObjectClass *co, bool create)  // 0x123456 (radar) ...\n\t{\n\t}\n};\n')
        self.assertEqual(T.untemplate('A<B<int> >::f'), 'A::f')
        self.assertEqual(T.class_mentions(src, 0x123456), [('ClientObjectDataAccessClass::Get', ['radar'])])
        got, how = self.cands('', [src])
        self.assertEqual(got, [self.NAMES[5]])
        self.assertIn('in its class', how)

    def test_empty_symbol_by_code(self):
        # TB3: SYMBOL '' with no name or marker at the address (a header static's _$E): the nearest-length
        # functions ordered by residue
        import objmatch
        funcs = [('_$E28', 1, 0, 22), ('_$E29', 1, 30, 52), ('_$E30', 1, 60, 80), ('?big@@YAXXZ', 1, 90, 400)]
        res = {'_$E28': 1, '_$E29': 0, '_$E30': 5}     # of 10 rows: 0.9, 1.0 and 0.5 (below 0.8: no answer)
        old = (objmatch.CoffObject, T.client_length, T.X.explain)
        try:
            objmatch.CoffObject = lambda p: type('O', (), {'functions': lambda self: funcs})()
            T.client_length = lambda addr: 22
            T.X.explain = lambda o, n, a: ({'residue': res[n], 'ours': 10, 'client': 10}, None)
            self.assertEqual(T.by_code('x.obj', 0x448BE0), ['_$E29', '_$E28'])
        finally:
            objmatch.CoffObject, T.client_length, T.X.explain = old

    def test_base_needs_no_forms(self):
        with tempfile.TemporaryDirectory() as d:
            p = os.path.join(d, 'v.py')
            with open(p, 'w') as f:
                f.write("FILE = 'app/x.h'\nSYMBOL = ''\nADDRESS = 0x401000\nUNIT = 'app\\\\x.cpp'\n")
            with self.assertRaises(SystemExit):
                T.load_spec(p)
            s = T.load_spec(p, need_forms=False)
        self.assertEqual((s['forms'], s['unit']), ({}, 'app/x.cpp'))
        self.assertEqual(T.compile_unit(s, None), 'app/x.cpp')
        self.assertEqual(T.compile_unit(dict(s, file='app/x.cpp', unit=None), None), 'app/x.cpp')


class Leases(unittest.TestCase):
    """4. (D273) as many VMs as the pool gives, each yielded as soon as it is ready."""

    def test_until_refused(self):
        calls = []

        def fake(cmd, **kw):
            calls.append(cmd)
            n = len(calls)
            if n <= 3:
                return types.SimpleNamespace(returncode=0, stdout=f'ENB_VC6_HOSTS=u@10.0.0.{n}\n',
                                             stderr=f'enb-vc6-0{n}: 10.0.0.{n} ready\n')
            return types.SimpleNamespace(returncode=3, stdout='', stderr='pool at its memory reserve\n')
        real = T.subprocess.run
        T.subprocess.run = fake
        try:
            got = list(T.lease_iter(8, 'tb', lambda s: None, parallel=1))
            self.assertEqual(got, [('enb-vc6-01', 'u@10.0.0.1'), ('enb-vc6-02', 'u@10.0.0.2'), ('enb-vc6-03', 'u@10.0.0.3')])
            self.assertEqual(len(calls), 4)            # the refusal ends it (no retry past the first VM)
            calls.clear()
            got = list(T.lease_iter(8, 'tb', lambda s: None, more=lambda: False))
            self.assertEqual(len(got), 1)              # nothing left to compile: no second VM
        finally:
            T.subprocess.run = real

    def test_parallel_rounds(self):
        # TB3: after the first VM, up to PARALLEL requests at once (a lease takes ~37 s); a round all refused ends it
        import threading
        lock, calls = threading.Lock(), []

        def fake(cmd, **kw):
            with lock:
                calls.append(cmd)
                n = len(calls)
            if n <= 3:
                return types.SimpleNamespace(returncode=0, stdout=f'ENB_VC6_HOSTS=u@10.0.0.{n}\n',
                                             stderr=f'enb-vc6-0{n}: 10.0.0.{n} ready\n')
            return types.SimpleNamespace(returncode=3, stdout='', stderr='pool cap\n')
        real = T.subprocess.run
        T.subprocess.run = fake
        try:
            got = list(T.lease_iter(8, 'tb', lambda s: None, parallel=4))
        finally:
            T.subprocess.run = real
        self.assertEqual(sorted(n for n, _ in got), ['enb-vc6-01', 'enb-vc6-02', 'enb-vc6-03'])
        self.assertEqual(len(calls), 9)              # 1, a round of 4 (2 served), a round of 4 refused
        self.assertEqual([c[c.index('--wait') + 1] for c in calls].count('900'), 1)

    def test_poll_while_forms_wait(self):
        # TB3: with poll, a refusal after the first VM waits and retries while forms wait; a VM freed meanwhile joins
        answers = [0, 3, 3, 0, 3]                    # ok, refused, refused, ok (another agent released one), refused
        calls, slept, left = [], [], [3]

        def fake(cmd, **kw):
            calls.append(cmd)
            rc = answers[len(calls) - 1]
            n = len(calls)
            if rc == 0:
                return types.SimpleNamespace(returncode=0, stdout=f'ENB_VC6_HOSTS=u@10.0.0.{n}\n',
                                             stderr=f'enb-vc6-0{n}: 10.0.0.{n} ready\n')
            return types.SimpleNamespace(returncode=3, stdout='', stderr='pool cap\n')

        def sleep(s):
            slept.append(s)
            left[0] -= 1                             # forms compile meanwhile; none left after the third wait

        real = T.subprocess.run
        T.subprocess.run = fake
        try:
            got = list(T.lease_iter(8, 'tb', lambda s: None, more=lambda: left[0] > 0, poll=30, sleep=sleep,
                                    parallel=1))
        finally:
            T.subprocess.run = real
        self.assertEqual([n for n, _ in got], ['enb-vc6-01', 'enb-vc6-04'])
        self.assertEqual(slept, [30, 30, 30])        # three refusals, then the queue was empty: no more leases
        self.assertEqual(len(calls), 5)


class FileCheck(unittest.TestCase):
    """5. (LV2) every exact function of the file per form, against the base's."""

    def test_lost_and_gained(self):
        sets = {'base.obj': {('?a@@', 0x401000), ('?b@@', 0x401100)},
                'f.obj': {('?a@@', 0x401000), ('?c@@', 0x401200)}, 'g.obj': {('?a@@', 0x401000), ('?b@@', 0x401100)}}
        results = {'base': {'res': 1}, 'f': {'res': 0}, 'g': {'res': 1}}
        logged = []
        import clientimage
        orig = clientimage.ClientImage
        clientimage.ClientImage = lambda: None
        try:
            T.file_check({'base': 'base.obj', 'f': 'f.obj', 'g': 'g.obj'}, results, logged.append,
                         exact=lambda obj, img, said: sets[obj])
        finally:
            clientimage.ClientImage = orig
        self.assertEqual((results['f']['file'], results['g']['file']), ((1, 1), (0, 0)))
        self.assertIn('FILE -1/+1', T.fmt_form('f', dict(result(0, 0, 'K'), file=(1, 1))))
        self.assertTrue(any('lost' in l and '0x401100' in l for l in logged))

    def test_not_grouped_with_base(self):
        spec = {'symbol': 'A::f', 'address': 0x401000, 'file': 'app/a.cpp', 'hypothesis': 'h'}
        results = {'base': dict(result(5, 5, 'K0'), file=(0, 0)), 'same': dict(result(5, 5, 'K0'), file=(0, 0)),
                   'breaks': dict(result(5, 5, 'K0'), file=(1, 0))}
        T.regroup(results)
        lines, _ = T.summary(spec, results, ['base', 'same', 'breaks'])
        self.assertIn('identical to base: same', lines)
        self.assertTrue(any(l.startswith('breaks') and 'FILE -1/+0' in l for l in lines))


class Asserts(unittest.TestCase):
    """6. (AP) a form that changes the stringified text of an assert condition."""

    SRC = ('void A::f(int count)\n{\n\tFAIL_IF(count  <  0) return;\n\tG_ASSERT(Ptr != NULL);\n'
           '#define assert(x) ((void)0)\n\tint n = count;\n}\n')

    def test_conditions(self):
        self.assertEqual(T.assert_conditions(self.SRC), {'count < 0': 1, 'Ptr != NULL': 1})

    def test_renames(self):
        read = lambda rel: self.SRC     # noqa: E731
        self.assertEqual(T.assert_renames(read, 'a.cpp', [('int count', 'int n_items'), ('count  <', 'n_items <')]),
                         ['count < 0'])
        self.assertEqual(T.assert_renames(read, 'a.cpp', [('int n = count;', 'int m = count;')]), [])
        self.assertEqual(T.assert_renames(read, 'a.cpp', []), [])
        self.assertIn('ASSERT-TEXT count < 0', T.fmt_form('f', dict(result(0, 0, 'K'), asserts=['count < 0'])))


if __name__ == '__main__':
    unittest.main()
