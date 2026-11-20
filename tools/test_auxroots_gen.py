"""tools/re/auxroots.py generate reproduces the committed aux-data files byte for byte (2026-11-18, AB): aux_roots.h,
co.cpp (its generated region and pad block, COM 2026-11-19b; aux_roots.cpp before), aux_roots_send.h, the group's headers (aux_roots_group.h, aux_group_ctors.h, aux_formation.h, aux_roots_types.h, aux_roots_item.h, aux_roots_hull.h,
aux_roots_player.h, aux_roots_shipinv.h, aux_roots_lego.h, aux_roots_manu.h, aux_roots_skill.h; DN), the family headers' moved constructors (aux_item.h, aux_manu.h) and
stubs/aux_roots_stubs.cpp, so the pad tools (auxpad.py, sendpad.py) and a regeneration keep the committed layout.
Needs client.exe (the retail install, ENB_RETAIL_INSTALL); skipped without it. RegionTest (co_region's line-count
keeping, its pad block and its refusals) needs nothing.

    python -m unittest tools.test_auxroots_gen
"""
import filecmp
import os
import shutil
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(HERE, 're'))
import auxroots  # noqa: E402

CLIENT_DIR = os.path.join(REPO, 'app', 'client')


@unittest.skipUnless(os.path.exists(auxroots.CLIENT), 'client.exe not installed')
class GenerateTest(unittest.TestCase):
    def test_generate_reproduces_committed_files(self):
        doc = auxroots.extract(auxroots.CLIENT)
        tmp = tempfile.mkdtemp(prefix='auxroots_gen_')
        out_dir, stubs = auxroots.OUT_DIR, auxroots.STUBS
        try:
            for header in [h for h, _ in auxroots.FAMILY_MOVED.values()] + [auxroots.CO_CPP]:
                shutil.copy(os.path.join(CLIENT_DIR, header), os.path.join(tmp, header))
            # the OUT_DIR path (family bodies and stubs written too), redirected into tmp
            auxroots.OUT_DIR = tmp
            auxroots.STUBS = os.path.join(tmp, 'aux_roots_stubs.cpp')
            auxroots.generate(doc, tmp)
            files = [(f, os.path.join(CLIENT_DIR, f)) for f in ('aux_roots.h', auxroots.CO_CPP, 'aux_roots_send.h',
                                                                   'aux_roots_group.h', 'aux_group_ctors.h', 'aux_formation.h',
                                                                   'aux_roots_types.h', 'aux_roots_item.h', 'aux_roots_hull.h', 'aux_roots_player.h',
                                                                   'aux_roots_shipinv.h', 'aux_roots_lego.h', 'aux_roots_manu.h', 'aux_roots_skill.h')]
            files += [(h, os.path.join(CLIENT_DIR, h)) for h, _ in auxroots.FAMILY_MOVED.values()]
            files.append(('aux_roots_stubs.cpp', os.path.join(REPO, 'stubs', 'aux_roots_stubs.cpp')))
            for name, ref in files:
                self.assertTrue(filecmp.cmp(os.path.join(tmp, name), ref, shallow=False), name + ' differs')
        finally:
            auxroots.OUT_DIR, auxroots.STUBS = out_dir, stubs
            shutil.rmtree(tmp, ignore_errors=True)


class RegionTest(unittest.TestCase):
    B, E = auxroots.REGION_BEGIN, auxroots.REGION_END
    NL = chr(10)

    def text(self, inner, after=6):
        return self.NL.join(['x();', self.B] + inner + [self.E] + [''] * after + ['y();', '/* notes */', ''])

    def test_line_count_kept_and_idempotent(self):
        t = self.text(['old'])
        body = self.NL.join(['a', '', 'b'])
        u = auxroots.co_region(t, body, '')
        self.assertEqual(u.count(self.NL), t.count(self.NL))
        self.assertIn(self.NL.join([self.B, 'a', '', 'b', self.E, '']), u)
        self.assertEqual(u.split(self.NL).index('y();'), t.split(self.NL).index('y();'))
        self.assertEqual(auxroots.co_region(u, body, ''), u)

    def test_body_shrinks_into_blank_lines(self):
        t = self.text(['1', '2', '3', '4'], after=0)
        u = auxroots.co_region(t, 'a', '')
        self.assertEqual(u.split(self.NL).index('y();'), t.split(self.NL).index('y();'))

    def test_body_too_long(self):
        with self.assertRaises(SystemExit):
            auxroots.co_region(self.text([], after=1), self.NL.join('abc'), '')

    def test_no_markers(self):
        with self.assertRaises(SystemExit):
            auxroots.co_region('x();' + self.NL, 'a', '')

    def test_pad_appended_replaced_removed(self):
        pad = auxroots.PAD % 'CO_PAD_00, CO_PAD_01'
        u = auxroots.co_region(self.text([]), 'a', pad)
        self.assertTrue(u.endswith('/* notes */' + self.NL + pad.lstrip(self.NL)))
        v = auxroots.co_region(u, 'a', auxroots.PAD % 'CO_PAD_00')
        self.assertEqual(v.count('CO_PAD_'), 1)
        self.assertEqual(v.count('enum CoSymbolPadType'), 1)
        self.assertEqual(auxroots.co_region(v, 'a', auxroots.PAD % 'CO_PAD_00, CO_PAD_01'), u)
        self.assertNotIn('CoSymbolPadType', auxroots.co_region(u, 'a', ''))

    def test_auxpad_reads_the_generated_pad(self):
        sys.path.insert(0, os.path.join(HERE, 'vc6'))
        import auxpad
        u = auxroots.co_region(self.text([]), 'a', auxroots.PAD % 'CO_PAD_00')
        self.assertEqual(auxpad.with_pad(u, 3),
                         auxroots.co_region(self.text([]), 'a', auxroots.PAD % 'CO_PAD_00, CO_PAD_01, CO_PAD_02'))


if __name__ == '__main__':
    unittest.main()
