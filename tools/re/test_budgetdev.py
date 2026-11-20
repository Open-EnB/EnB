"""budgetdev.py's scan on fixtures, no VM (python -m unittest tools/re/test_budgetdev.py, or run it)."""
import os
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import budgetdev as B  # noqa: E402

SRC = '\n'.join([
    '#include "x.h"',                                                       # 1
    'class LocalClass { public: int Get() const { return(X); } int X; };',  # 2
    '',                                                                     # 3
    '// 0x401000 (the dead size() after the call spends the inline budget; D272)',   # 4
    'void A::f(int x)',                                                     # 5
    '{',                                                                    # 6
    '\tconst char *s = "};{";\t// } in a comment',                        # 7
    '\tList.size(); G_ASSERT(x != 0);',                                    # 8
    '\tfor (int i = 0; i < List.size(); ++i) Do(i);',                      # 9
    '\tstd::char_traits<char>::length; switch (x) { case 1: Name.c_str(); break; }',   # 10
    '\tout << Name.c_str();',                                              # 11
    '\ta == b;',                                                           # 12
    '}',                                                                    # 13
    '',                                                                     # 14
    '#pragma auto_inline(off)',                                             # 15
    '// 0x401200',                                                          # 16
    'int Unknown_401200(int y)',                                            # 17
    '{',                                                                    # 18
    '\treturn(y + 1);',                                                     # 19
    '}',                                                                    # 20
    '#pragma auto_inline(on)',                                              # 21
    '',                                                                     # 22
    'static inline void Helper(int &v) { v = 0; }',                         # 23
    '',                                                                     # 24
    '// 0x401300',                                                          # 25
    'int B::g()',                                                           # 26
    '{',                                                                    # 27
    '\tint v; Helper(v); return(Unknown_401200(v));',                       # 28
    '}',                                                                    # 29
    'S &S::operator=(const S &o) { X = o.X; return(*this); }',              # 30
]) + '\n'


class ScanTest(unittest.TestCase):
    def test_functions_and_names(self):
        fs = B.functions(SRC)
        self.assertEqual([(f['name'], f['header_line'], f['end_line']) for f in fs],
                         [('LocalClass::Get' if False else 'Get', 2, 2), ('A::f', 5, 13), ('Unknown_401200', 17, 20),
                          ('Helper', 23, 23), ('B::g', 26, 29), ('S::operator=', 30, 30)])
        self.assertEqual(fs[0]['scope'], [])

    def test_dead_kind(self):
        self.assertEqual(B.dead_kind('List.size()'), 'accessor size')
        self.assertEqual(B.dead_kind('std::char_traits<char>::length'), 'char_traits length')
        self.assertEqual(B.dead_kind('Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this)'), 'find')
        self.assertEqual(B.dead_kind('a == b'), 'compare')
        for s in ('G_ASSERT(x != 0)', 'out << Name.c_str()', 'x = List.size()',
                  'return List.size()', 'Buttons[i]->Set(i == key)', 'Find_Samples(index, lower, upper)'):
            self.assertIsNone(B.dead_kind(s), s)

    def test_scan_file_rows(self):
        td = tempfile.mkdtemp(prefix='budgetdev_')
        p = os.path.join(td, 'a.cpp')
        with open(p, 'w', newline='\n') as f:
            f.write(SRC)
        known = {0x401000, 0x401200, 0x401300}
        rows = B.scan_file(p, 'app/client/a.cpp', known, {0x401000, 0x401300}, {0x401000: 100})
        got = sorted((r['kind'], r['address'], r['lines'][0]) for r in rows)
        self.assertIn(('dead-read', '0x401000', 8), got)
        self.assertIn(('auto-inline', '0x401200', 15), got)
        self.assertIn(('inline-helper', None, 23), got)
        self.assertIn(('copy-operator', None, 30), got)
        dead = next(r for r in rows if r['kind'] == 'dead-read')
        self.assertEqual(dead['lines'], [8, 10, 12])                 # not the for header, the assert or the stream
        self.assertTrue(dead['exact'])
        self.assertEqual(dead['size'], 100)
        self.assertIn('comment', dead['found'])                      # the comment above names the device: merged
        ai = next(r for r in rows if r['kind'] == 'auto-inline')
        self.assertEqual(ai['held'], ['0x401300'])                   # its same-file caller
        helper = next(r for r in rows if r['kind'] == 'inline-helper')
        self.assertEqual(helper['held'], ['0x401300'])
        # the device removed keeps every line (no __LINE__ shift) and only the device's text goes
        text, forms = B.forms_of_text(SRC, rows)
        self.assertEqual(len(forms), 3)                              # the dead reads, the pragma pair, `all`
        self.assertIn('all', forms)
        for name, (keys, new) in forms.items():
            self.assertEqual(new.count('\n'), SRC.count('\n'), name)
        new = forms['all'][1].splitlines()
        self.assertEqual(new[7].strip(), 'G_ASSERT(x != 0);')
        self.assertEqual(new[14], '')
        self.assertIn('for (int i = 0; i < List.size(); ++i)', new[8])

    def test_address_prefers_the_marker_line(self):
        lines = ['// 0x402000 (also the view\'s 0x401000)', 'void Unknown_401000()', '{', '}']
        f = {'header_line': 2, 'body_line': 3, 'name': 'Unknown_401000'}
        self.assertEqual(B.address_of(f, lines, {0x401000, 0x402000})[0], 0x402000)
        lines[0] = '// what it does (0x403000 calls it)'
        self.assertEqual(B.address_of(f, lines, {0x401000, 0x403000})[0], 0x401000)


if __name__ == '__main__':
    unittest.main()
