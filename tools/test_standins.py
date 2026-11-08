"""Tests for tools/vc6/standins.py (pipeline-plan §4 item 6a) and wt.py's gate around it, on temporary git
repositories.

    python -m unittest discover -s tools -p "test_*.py"
"""
import io
import json
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import standins  # noqa: E402
import wt  # noqa: E402

STUBS = 'stubs/app_stubs.cpp'
POC = 'programs/clientpoc/poc_world2.cpp'
CT = 'tests/clienttest/client_stubs.cpp'


def git(cwd, *args):
    r = subprocess.run(['git', *args], cwd=cwd, capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError(f'git {args}: {r.stderr}')
    return r.stdout.strip()


class Repo(unittest.TestCase):
    def setUp(self):
        self.root = tempfile.mkdtemp(prefix='standins_test_')
        git(self.root, 'init', '-q', '-b', 'master')
        for k, v in (('user.name', 't'), ('user.email', 't@t'), ('core.autocrlf', 'false'),
                     ('commit.gpgsign', 'false')):
            git(self.root, 'config', k, v)
        self.base = self.commit('base', {
            'app/client/thing.cpp': '// thing\nint Thing::Old(int a)\n{\n\treturn a;\n}\n',
            STUBS: '// stand-ins\nvoid Unrelated_Free(int)\n{\n}\n\n'
                   'char *Value_To_String(char *buf, int size, const MoneyClass &value)\n{\n\treturn buf;\n}\n',
            POC: '// POC stand-ins\nvoid Thing::Render(int when)\n{\n}\n'
                 'Thing::Thing() { }\n',
            CT: '#include "x.h"\nint SOptionsClass::Get() { return 0; }\n'})

    def tearDown(self):
        shutil.rmtree(self.root, ignore_errors=True)

    def commit(self, msg, files, remove=()):
        for rel, text in files.items():
            full = os.path.join(self.root, *rel.split('/'))
            os.makedirs(os.path.dirname(full), exist_ok=True)
            with open(full, 'w', newline='\n') as f:
                f.write(text)
        for rel in remove:
            os.remove(os.path.join(self.root, *rel.split('/')))
        git(self.root, 'add', '-A')
        git(self.root, 'commit', '-q', '-m', msg)
        return git(self.root, 'rev-parse', 'HEAD')

    def check(self, new, **kw):
        return standins.check(self.root, self.base, new, **kw)

    def test_new_definition_with_standin_refuses(self):
        new = self.commit('render', {'app/client/thing.cpp': '// thing\nint Thing::Old(int a)\n{\n\treturn a;\n}\n'
                                     '\nvoid Thing::Render(int t)\n{\n\tDraw(t);\n}\n'})
        found, _ = self.check(new)
        self.assertEqual([(f[0], f[1], f[2]) for f in found],
                         [('Thing::Render', ['app/client/thing.cpp'], [(POC, 2)])])
        self.assertIn('Thing::Render: new in app/client/thing.cpp; stand-in at ' + POC + ':2',
                      '\n'.join(standins.report(found)))

    def test_constructor_in_header_refuses(self):
        new = self.commit('ctor', {'app/client/thing.h': 'class Thing {\npublic:\n\tThing() { x = 0; }\n\tint x;\n};\n'})
        found, _ = self.check(new)
        self.assertEqual([f[0] for f in found], ['Thing::Thing'])

    def test_unrelated_standin_passes(self):
        new = self.commit('other', {'app/client/other.cpp': 'void Other::Update(float dt)\n{\n}\n'
                                    'void Unrelated_Free_2(int)\n{\n}\n'})
        self.assertEqual(self.check(new), ([], []))

    def test_other_overload_passes(self):
        new = self.commit('money', {'app/shared/money.cpp':
                                    'char *Value_To_String(char *buf, int size, MoneyClass value)\n{\n\treturn buf;\n}\n'})
        self.assertEqual(self.check(new)[0], [])
        same = self.commit('money2', {'app/shared/money2.cpp':
                                      'char *Value_To_String(char *b, int n, const MoneyClass& v)\n{\n\treturn b;\n}\n'})
        self.assertEqual([f[0] for f in self.check(same)[0]], ['Value_To_String'])

    def test_standin_deleted_in_same_change_passes(self):
        new = self.commit('render', {'app/client/thing.cpp': 'void Thing::Render(int t)\n{\n}\n',
                                     POC: '// POC stand-ins\nThing::Thing() { }\n'})
        self.assertEqual(self.check(new), ([], []))

    def test_standin_added_with_the_definition_is_a_note(self):
        new = self.commit('deliberate', {'app/client/radar.cpp': 'void Radar::Unknown_7BCA70()\n{\n}\n',
                                         CT: '#include "x.h"\nint SOptionsClass::Get() { return 0; }\n'
                                             'void Radar::Unknown_7BCA70() { }\n'})
        found, notes = self.check(new)
        self.assertEqual(found, [])
        self.assertIn('deliberate', notes[0])

    def test_coexisting_before_is_a_note(self):
        self.base = self.commit('soptions', {'app/client/soptions.cpp': 'int SOptionsClass::Get()\n{\n\treturn 1;\n}\n'})
        new = self.commit('move', {'app/client/soptions2.cpp': 'int SOptionsClass::Get()\n{\n\treturn 1;\n}\n'},
                          remove=['app/client/soptions.cpp'])
        found, notes = self.check(new)
        self.assertEqual(found, [])
        self.assertIn('both defined it before', notes[0])

    def test_keep_marker_is_a_note(self):
        self.base = self.commit('mark', {POC: '// POC stand-ins\n// standins: keep (the POC never pulls thing.obj)\n'
                                              'void Thing::Render(int when)\n{\n}\n'})
        new = self.commit('render', {'app/client/thing.cpp': 'void Thing::Render(int t)\n{\n}\n'})
        found, notes = self.check(new)
        self.assertEqual(found, [])
        self.assertIn('standins: keep', notes[0])

    def test_new_pair_named_by_symbols_refuses(self):
        syms = [{'address': '0x401000', 'name': 'SOptionsClass::Get', 'kind': 'function', 'source': 'exact'}]
        new = self.commit('symbols', {'docs/data/symbols.json': json.dumps(syms)})
        old_led = {'app': {'files': {'client/soptions.cpp': {'in_tree': []}}}}
        new_led = {'app': {'files': {'client/soptions.cpp': {'in_tree': ['0x401000']}}}}
        found, _ = self.check(new, old_ledger=old_led, new_ledger=new_led)
        self.assertEqual([(f[0], f[1]) for f in found], [('SOptionsClass::Get', ['app/client/soptions.cpp (0x401000)'])])
        self.assertEqual(self.check(new, old_ledger=new_led, new_ledger=new_led)[0], [])

    def test_wt_gate_exits_and_passes(self):
        old_root = wt.ROOT
        wt.ROOT = self.root
        try:
            new = self.commit('render', {'app/client/thing.cpp': 'void Thing::Render(int t)\n{\n}\n'})
            with self.assertRaises(SystemExit) as cm, redirect_stdout(io.StringIO()):
                wt.standin_gate(self.base, new, {}, {}, 'run/x')
            self.assertIn('Thing::Render', str(cm.exception))
            self.assertIn('run/x untouched', str(cm.exception))
            ok = self.commit('drop', {POC: '// none\n'})
            buf = io.StringIO()
            with redirect_stdout(buf):
                wt.standin_gate(self.base, ok, {}, {}, 'run/x')
            self.assertIn('no collision', buf.getvalue())
        finally:
            wt.ROOT = old_root


class Signature(unittest.TestCase):
    def test_parameters(self):
        sig = standins.signature
        self.assertEqual(sig('(char *buf, int size, const MoneyClass &value)'),
                         ('char*', 'int', 'const MoneyClass&'))
        self.assertEqual(sig('(char* b,int n,const MoneyClass & v)'), ('char*', 'int', 'const MoneyClass&'))
        self.assertEqual(sig('(MoneyClass)'), ('MoneyClass',))
        self.assertEqual(sig('(unsigned int, unsigned long x = 4)'), ('unsigned int', 'unsigned long'))
        self.assertEqual(sig('(void)'), ())
        self.assertEqual(sig('(std::vector<int, A> &v, char buf[])'), ('std::vector<int,A>&', 'char*'))


if __name__ == '__main__':
    unittest.main()
