"""Tests for tools/vc6/build.py's trial-link object list (--objs, read_objs) and its guard (no VMs).

    python -m unittest discover -s tools -p "test_build_objs.py"
"""
import os
import shutil
import sys
import tempfile
import unittest
from unittest import mock

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import build  # noqa: E402


class ReadObjs(unittest.TestCase):
    def setUp(self):
        self.out = tempfile.mkdtemp()
        os.makedirs(os.path.join(self.out, 'app', 'obj'))
        for n in ('b.obj', 'a.obj', 'c.obj'):
            open(os.path.join(self.out, 'app', 'obj', n), 'wb').close()
        self.list = os.path.join(self.out, 'objs.txt')

    def tearDown(self):
        shutil.rmtree(self.out)

    def write(self, text):
        open(self.list, 'w', encoding='utf-8').write(text)

    def test_order_kept_comments_and_blanks_skipped(self):
        absc = os.path.join(self.out, 'app', 'obj', 'c.obj')
        self.write(f'# retail order\napp/obj/b.obj\n\napp/obj/a.obj  # second\n{absc}\n')
        got = build.read_objs(self.list, self.out)
        self.assertEqual([os.path.basename(f) for f in got], ['b.obj', 'a.obj', 'c.obj'])
        self.assertEqual(got[0], os.path.join(self.out, 'app/obj/b.obj'))
        self.assertEqual(got[2], absc)

    def test_missing_object_refused(self):
        self.write('app/obj/a.obj\napp/obj/nope.obj\n')
        with self.assertRaises(SystemExit) as e:
            build.read_objs(self.list, self.out)
        self.assertIn(':2:', str(e.exception))

    def test_duplicate_basename_refused(self):
        # a second folder: on a case-sensitive file system app/obj/A.obj would not exist
        os.makedirs(os.path.join(self.out, 'lib', 'obj'))
        open(os.path.join(self.out, 'lib', 'obj', 'A.obj'), 'wb').close()
        self.write('app/obj/a.obj\nlib/obj/A.obj\n')
        with self.assertRaises(SystemExit) as e:
            build.read_objs(self.list, self.out)
        self.assertIn('twice', str(e.exception))


class Guard(unittest.TestCase):
    def test_objs_need_a_tag(self):
        with mock.patch.object(build, 'link_one') as lo:
            with self.assertRaises(SystemExit):
                build.link_program('client', 'out', objs=['x.obj'])
            lo.assert_not_called()

    def test_objs_reach_link_one(self):
        summ = {'cl_errors': [], 'unresolved': {}, 'exe': True}
        with mock.patch.object(build, 'link_one', return_value=summ) as lo:
            build.link_program('client', 'out', tag='objs', objs=['x.obj'])
            self.assertEqual(lo.call_args.kwargs['extra_objs'], ['x.obj'])
            self.assertEqual(lo.call_args.kwargs['trial'], 'objs')

    def test_default_link_has_no_objects(self):
        summ = {'cl_errors': [], 'unresolved': {}, 'exe': True}
        with mock.patch.object(build, 'link_one', return_value=summ) as lo:
            build.link_program('client', 'out')
            self.assertEqual(list(lo.call_args.kwargs['extra_objs']), [])
            self.assertIsNone(lo.call_args.kwargs['trial'])


if __name__ == '__main__':
    unittest.main()
