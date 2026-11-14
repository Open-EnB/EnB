// Earth & Beyond client (reconstruction): app\client\ui_test.cpp, the UI test window ([UITEST] in fontcode.ini /
// fontart.ini). No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_test.h"
#include "sclient.h"
#include "local_interface_sort.h"
#include "ui_mesh.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "..\..\wwlib\argv.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// 0x64F790
UITestClass::UITestClass(UIViewClass *view, const LayerClass &layer) :
	InterfaceClass(view, (int)layer.Scene),
	Windows(5),
	Files(3),
	Layer(layer)
{
}

// 0x64F990 (the deleting destructor 0x64F920)
UITestClass::~UITestClass()
{
	int count = Windows.Count();
	for (int i = 0; i < count; i++) {
		REF_PTR_RELEASE(Windows[i]);
	}
}

// 0x64FB10
void UITestClass::Unknown_54()
{
}

// 0x64FB30
void UITestClass::Unknown_58()
{
	char section[256];
	char value[256];
	char entry[256];
	InterfaceSortScopeClass sort(this);
	int i = 0;
	ArgvClass args(false, false);
	if (args.Find("-UITEST_CODE")) {
		Ini.Load("fontcode.ini");
	} else {
		Ini.Load("fontart.ini");
	}
	Unknown_BE67BC->Load(NULL);
	if (Ini.Get_String("UITEST", "Section", "", section, 255)) {
		char *name = nstrdup(section);
		sprintf(section, "UITEST\\%s", name);
		delete [] name;
		while (true) {
			sprintf(entry, "load%d", i++);
			if (!Ini.Get_String(section, entry, "", value, 255)) {
				break;
			}
			Files.Add(nstrdup(value));
			Unknown_BE67BC->Load(value);
		}
		i = 0;
		while (true) {
			sprintf(entry, "mesh%d", i);
			if (!Ini.Get_String(section, entry, "", value, 255)) {
				break;
			}
			UIMeshClass *mesh = new UIMeshClass(value, 1);
			Windows.Add(new Unknown5BB320Class(mesh, View, Layer));
			Windows[i]->Unknown_54();
			Windows[i]->Unknown_58();
			View->Unknown_6B4940(Windows[i]);
			Unknown_BE67C4->Add(mesh->InterfaceMesh);
			i++;
		}
	}
}

// 0x64FF10
void UITestClass::Unknown_5C()
{
	Ini.Clear();
	while (Windows.Count()) {
		Windows[0]->Unknown_7C(NULL, false);
		Unknown_BE67C4->Remove(Windows[0]->InterfaceMesh->InterfaceMesh);
		REF_PTR_RELEASE(Windows[0]);
	}
	while (Files.Count()) {
		Unknown_BE67BC->Unload(Files[0]);
		delete [] Files[0];
		Files.Delete(0);
	}
}

// 0x650040
void UITestClass::Unknown_60()
{
	for (int i = 0; i < Windows.Count(); i++) {
		Windows[i]->Unknown_60();
	}
}

// 0x650080
bool UITestClass::Unknown_68(int key)
{
	return(false);
}

/*
**	Notes.
**	Object 0x64F790-0x650508: ui_talkt_space.cpp's adjustor thunks 0x64F770 / 0x64F780 end the object before it;
**	ui_trade.cpp's object opens at 0x650510 with permstr.h's PermStringInitClass static (0xBE5E70; atexit 0x650540),
**	then its constructor 0x650560. No PermStringInitClass static here, so the file does not include permstr.h; the
**	guarded <string> initialiser 0x6504C0 (through sclient.h) is this object's. After the eight functions above come
**	the instances of DynamicVectorClass<Unknown5BB320Class *> (tables 0xAF8E98 / VectorClass's 0xAF8EB4, both without
**	RTTI names: this object emits them first): 0x64F950, 0x6500A0-0x650490; the adjustor thunks 0x6504F0 (+0x04) and
**	0x650500 (+0x38) of the deleting destructor close it. Unknown_58's loops are while (true) (for (;;) gave a
**	rotated loop) and its counter is declared after the sort scope (ebp's zero is set there).
**	The window's 'Section' value names a section [UITEST\<name>] whose 'load%d' keys are string files
**	(FontStringFilesClass::Load) and whose 'mesh%d' keys are models, each shown as a mesh window (0x5BB250) added to
**	the view (0x6B4940) and to the font manager (0x6D1D80). Unknown_5C's first loop never shrinks the list (the
**	client's own: the entry is set to NULL, the count kept).
*/
