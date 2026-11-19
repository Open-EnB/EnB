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


if __name__ == '__main__':
    unittest.main()
