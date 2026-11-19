"""tools/re/auxroots.py generate reproduces the committed aux-data files byte for byte (2026-11-18, AB): aux_roots.h,
aux_roots.cpp, aux_roots_send.h, the group's headers (aux_roots_group.h, aux_group_ctors.h, aux_formation.h, aux_roots_types.h, aux_roots_item.h, aux_roots_hull.h,
aux_roots_player.h, aux_roots_shipinv.h, aux_roots_lego.h, aux_roots_manu.h, aux_roots_skill.h; DN), the family headers' moved constructors (aux_item.h, aux_manu.h) and
stubs/aux_roots_stubs.cpp, so the pad tools (auxpad.py, sendpad.py) and a regeneration keep the committed layout.
Needs client.exe (the retail install, ENB_RETAIL_INSTALL); skipped without it.

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
            for header, _ in auxroots.FAMILY_MOVED.values():
                shutil.copy(os.path.join(CLIENT_DIR, header), os.path.join(tmp, header))
            # the OUT_DIR path (family bodies and stubs written too), redirected into tmp
            auxroots.OUT_DIR = tmp
            auxroots.STUBS = os.path.join(tmp, 'aux_roots_stubs.cpp')
            auxroots.generate(doc, tmp)
            files = [(f, os.path.join(CLIENT_DIR, f)) for f in ('aux_roots.h', 'aux_roots.cpp', 'aux_roots_send.h',
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


if __name__ == '__main__':
    unittest.main()
