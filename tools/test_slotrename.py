"""Tests for tools/re/slotrename.py (P-L12-1, D239): the type-aware slot rename, on a temporary tree.

    python -m unittest discover -s tools -p "test_*.py"
"""
import io
import os
import shutil
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 're'))

import slotrename  # noqa: E402

BASE_H = """\
class LayerClass;
class GadgetClass
{
public:
	virtual int Unknown_08() { return(0); }	// +0x08 (a mention of Unknown_08 in a comment stays)
	virtual void Unknown_2C();
};
class RObjGadgetClass : public GadgetClass
{
public:
	virtual int Unknown_08();
};
class ButtonClass : public RObjGadgetClass
{
};
class OtherClass
{
public:
	virtual int Unknown_08();
	OtherClass *Next;
};
"""

USE_CPP = """\
#include "base.h"
int RObjGadgetClass::Unknown_08() { return(GadgetClass::Unknown_08()); }
void GadgetClass::Unknown_2C() { Unknown_08(); this->Unknown_08(); }
int OtherClass::Unknown_08() { return(Next->Unknown_08()); }
GadgetClass *Find_Gadget(int id);
void Use(GadgetClass *gadget, OtherClass *other, ButtonClass &button)
{
	gadget->Unknown_08();
	other->Unknown_08();
	button.Unknown_08();
	Find_Gadget(1)->Unknown_08();
	((RObjGadgetClass *)other)->Unknown_08();
	GadgetClass *list[4];
	list[2]->Unknown_08();
	const char *text = "Unknown_08";
	mystery()->Unknown_08();
}
"""


class SlotRename(unittest.TestCase):
    def setUp(self):
        self.root = tempfile.mkdtemp(prefix='slotrename_test_')
        os.makedirs(os.path.join(self.root, 'app', 'client'))
        for name, text in (('base.h', BASE_H), ('use.cpp', USE_CPP)):
            with open(os.path.join(self.root, 'app', 'client', name), 'w', newline='') as f:
                f.write(text)

    def tearDown(self):
        shutil.rmtree(self.root, ignore_errors=True)

    def run_tool(self, *extra):
        out = io.StringIO()
        with redirect_stdout(out):
            rc = slotrename.main(['--class', 'GadgetClass', '--slot', 'Unknown_08=Get_RObj', '--tree', self.root] + list(extra))
        return rc, out.getvalue()

    def read(self, name):
        with open(os.path.join(self.root, 'app', 'client', name), newline='') as f:
            return f.read()

    def test_dry_run_changes_nothing(self):
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn('dry run', out)
        self.assertEqual(self.read('use.cpp'), USE_CPP)

    def test_renames_by_the_object_class(self):
        rc, out = self.run_tool('--write')
        self.assertEqual(rc, 0)
        h = self.read('base.h')
        self.assertIn('virtual int Get_RObj() { return(0); }\t// +0x08 (a mention of Unknown_08 in a comment stays)', h)
        self.assertIn('class RObjGadgetClass : public GadgetClass\n{\npublic:\n\tvirtual int Get_RObj();', h)
        self.assertIn('class OtherClass\n{\npublic:\n\tvirtual int Unknown_08();', h)
        c = self.read('use.cpp')
        self.assertIn('int RObjGadgetClass::Get_RObj() { return(GadgetClass::Get_RObj()); }', c)
        self.assertIn('void GadgetClass::Unknown_2C() { Get_RObj(); this->Get_RObj(); }', c)
        self.assertIn('int OtherClass::Unknown_08() { return(Next->Unknown_08()); }', c)
        self.assertIn('\tgadget->Get_RObj();\n\tother->Unknown_08();\n\tbutton.Get_RObj();\n', c)
        self.assertIn('Find_Gadget(1)->Get_RObj();', c)
        self.assertIn('((RObjGadgetClass *)other)->Get_RObj();', c)
        self.assertIn('list[2]->Get_RObj();', c)
        self.assertIn('"Unknown_08"', c)
        self.assertIn('mystery()->Unknown_08();', c)
        self.assertIn('UNRESOLVED app/client/use.cpp:16 Unknown_08', out)

    def test_strict_fails_on_unresolved(self):
        rc, _ = self.run_tool('--strict')
        self.assertEqual(rc, 1)

    def test_unknown_class(self):
        out = io.StringIO()
        with redirect_stdout(out):
            rc = slotrename.main(['--class', 'NoSuchClass', '--slot', 'A=B', '--tree', self.root])
        self.assertEqual(rc, 2)


if __name__ == '__main__':
    unittest.main()
