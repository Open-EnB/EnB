"""Tests for the generated VC6 workspace (tools/vc6/dsp.py, D248) and objcmp.py.

    python -m unittest discover -s tools -p "test_dsp.py"

Standard library only; no VM, no client.exe. The workspace's compile lines must stay build.py's: the
IDE build was shown to give build.py's objects (docs/build-your-own.md), and only as long as the
options agree.
"""
import os
import struct
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))
import build as B  # noqa: E402
import dsp  # noqa: E402
import libdefs as L  # noqa: E402
import objcmp  # noqa: E402
import vc6  # noqa: E402


class WorkspaceTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.projs = {p.name: p for p in dsp.projects()}

    def test_every_source_once(self):
        """Every committed library and app source is in exactly one library project or main."""
        want = []
        for lib in L.LIBS + list(L.APP_LIBS):
            want += [dsp.repo_rel(p) for p in B.sources(lib)]
        got = [s for p in self.projs.values() for s in p.sources]
        self.assertEqual(sorted(got), sorted(want))

    def test_library_lines_are_build_py(self):
        """A library project's options are build.py's: its flags, the tree's include paths (absolute), its defines."""
        for lib in L.LIBS:
            p = self.projs.get(lib)
            if not p:
                continue
            toks = p.tokens(p.flags, p.defines)
            for f in L.LIB_FLAGS.get(lib, vc6.DEFAULT_FLAGS).replace('/c', '').split():
                if not f.startswith('/D') and f in ('/MT', '/O2', '/Ob1', '/Ob2', '/GX', '/W3'):
                    self.assertIn(f, toks, lib)
            incs = [t for t in toks if t.startswith('/I ')]
            self.assertEqual(incs, [f'/I "{os.path.abspath(i)}"' for i in L.tree_includes()], lib)
            for d in vc6.DEFAULT_DEFINES:
                self.assertIn(f'/D "{d}"', toks, lib)

    def test_file_flags(self):
        """FILE_FLAGS (wwlib's rawfile.cpp: /GX) is a per-file ADD CPP."""
        p = self.projs['wwlib']
        text = p.text()
        i = text.index('SOURCE=.\\rawfile.cpp')
        self.assertIn('# ADD CPP /GX', text[i:i + 200])

    def test_app_defines(self):
        p = self.projs['client']
        self.assertIn('/D "ENABLE_WWPROFILE"', p.cpp_line(p.flags, p.defines))

    def test_main_is_the_client(self):
        main = self.projs[dsp.CLIENT_PROJECT]
        self.assertEqual(main.kind, 'exe')
        self.assertTrue(all(s.startswith('app/main/') for s in main.sources))
        self.assertIn(f'# Name "main - Win32 {dsp.CONFIG}"', main.text())
        self.assertEqual([p.name for p in self.projs.values() if p.kind == 'exe'], ['main'])   # one exe, no stand-in build
        text = main.text()
        self.assertNotIn('/force:unresolved', text)
        self.assertNotIn('stubs', text.lower())

    def test_workspace(self):
        text = dsp.dsw_text(list(self.projs.values()))
        self.assertTrue(text.startswith('Microsoft Developer Studio Workspace File, Format Version 6.00'))
        self.assertIn('Project: "main"=.\\main.dsp - Package Owner=<4>', text)
        self.assertIn('Project: "wwlib"=..\\..\\wwlib\\wwlib.dsp - Package Owner=<4>', text)


class PruneTest(unittest.TestCase):
    def test_stale_project_and_its_empty_folders_go(self):
        """A generated .dsp the generator no longer makes goes with the folders it leaves empty (the retired
        stand-in program's programs/clientpoc/clientpoc.dsp and gen/); kept, foreign and non-empty ones stay."""
        import tempfile
        with tempfile.TemporaryDirectory() as root:
            def put(rel, text):
                p = os.path.join(root, *rel.split('/'))
                os.makedirs(os.path.dirname(p), exist_ok=True)
                open(p, 'w', encoding='latin1').write(text)
            banner = '# ' + dsp.GENERATED_BANNER + '\n'
            put('app/main/main.dsp', banner)
            put('programs/clientpoc/clientpoc.dsp', banner)
            os.makedirs(os.path.join(root, 'programs', 'clientpoc', 'gen'))
            put('programs/other/keep.txt', 'x')
            put('programs/other/old.dsp', banner)
            put('wwlib/hand.dsp', 'not generated')
            gone = dsp.prune(['app/main/main.dsp'], root)
            self.assertEqual(gone, ['programs/clientpoc/clientpoc.dsp', 'programs/other/old.dsp'])
            self.assertFalse(os.path.exists(os.path.join(root, 'programs', 'clientpoc')))
            self.assertTrue(os.path.exists(os.path.join(root, 'programs', 'other', 'keep.txt')))
            self.assertTrue(os.path.exists(os.path.join(root, 'app', 'main', 'main.dsp')))
            self.assertTrue(os.path.exists(os.path.join(root, 'wwlib', 'hand.dsp')))


def coff(sections, symbols):
    """A minimal COFF object: sections [(name, data, [(offset, symbol index, type)])], symbols [names]."""
    nsec = len(sections)
    hdr_end = 20 + 40 * nsec
    blobs, heads = b'', []
    off = hdr_end
    for name, data, rels in sections:
        rel = b''.join(struct.pack('<IIH', o, s, t) for o, s, t in rels)
        heads.append((name, len(data), off, off + len(data), len(rels)))
        blobs += data + rel
        off += len(data) + len(rel)
    symtab = off
    syms = b''.join(n.encode().ljust(8, b'\0') + b'\0' * 10 for n in symbols)
    out = struct.pack('<HHIIIHH', 0x14C, nsec, 0, symtab, len(symbols), 0, 0)
    for name, size, ptr, prel, nrel in heads:
        out += name.encode().ljust(8, b'\0') + struct.pack('<IIIIIIHHI', 0, 0, size, ptr, prel, 0, nrel, 0, 0)
    return out + blobs + syms + struct.pack('<I', 4)


class BuildClientTest(unittest.TestCase):
    def test_sdk_header_hash_ignores_line_ends(self):
        """docs/toolchain.md §2's header hashes are of CRLF files; GitHub's archives have LF."""
        import tempfile
        sys.path.insert(0, HERE)
        import build_client
        d = tempfile.mkdtemp()
        lf, crlf, lib = (os.path.join(d, n) for n in ('a.h', 'b.h', 'c.lib'))
        open(lf, 'wb').write(b'int a;\nint b;\n')
        open(crlf, 'wb').write(b'int a;\r\nint b;\r\n')
        open(lib, 'wb').write(b'x\ny\n')
        self.assertEqual(build_client.sha256(lf), build_client.sha256(crlf))
        import hashlib
        self.assertEqual(build_client.sha256(lib), hashlib.sha256(b'x\ny\n').hexdigest())   # binaries as they are

    def test_tracked_without_git(self):
        """build.py's file list walks the tree when there is no git checkout (the VM's copy)."""
        real = B.REPO
        try:
            import tempfile
            d = tempfile.mkdtemp()
            os.makedirs(os.path.join(d, 'wwdebug', 'client_release'))
            open(os.path.join(d, 'wwdebug', 'wwdebug.cpp'), 'w').close()
            open(os.path.join(d, 'wwdebug', 'client_release', 'x.obj'), 'w').close()
            B.REPO = d
            self.assertEqual(B.tracked('wwdebug'), ['wwdebug/wwdebug.cpp'])
        finally:
            B.REPO = real


class ObjcmpTest(unittest.TestCase):
    def test_label_numbers_and_index_ignored(self):
        a = coff([('.text', b'\xe8\0\0\0\0', [(1, 1, 20)])], ['.file', '$L100'])
        b = coff([('.text', b'\xe8\0\0\0\0', [(1, 2, 20)])], ['.file', 'pad', '$L104'])
        self.assertIs(objcmp.same(a, b), True)

    def test_other_target_differs(self):
        a = coff([('.text', b'\xe8\0\0\0\0', [(1, 0, 20)])], ['?f@@YAXXZ'])
        b = coff([('.text', b'\xe8\0\0\0\0', [(1, 0, 20)])], ['?g@@YAXXZ'])
        self.assertIs(objcmp.same(a, b), False)

    def test_comdat_order(self):
        s1, s2 = ('.text', b'\x90', []), ('.text', b'\xc3', [])
        self.assertEqual(objcmp.same(coff([s1, s2], []), coff([s2, s1], [])), 'order')


if __name__ == '__main__':
    unittest.main()
