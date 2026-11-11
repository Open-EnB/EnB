// Earth & Beyond client (reconstruction): app\client\unknown_5b7ab0.cpp, the menu window of the view at SClient +0x1358
// (interim placement, D137: the client's object has no __FILE__ string; file name provisional, NAME 1). Notes at the end of the file.
#include "sclient.h"
#include "ui_mesh.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "unknown_5b7ab0.h"

// 0x5B7AB0
Unknown5B7AB0Class::Unknown5B7AB0Class(UIViewClass *view, const LayerClass &layer) :
	Unknown5BB320Class(NULL, view, layer)
{
	InterfaceMesh = new UIMeshClass("mu01", 2);
}

// 0x5B7BA0
Unknown5B7AB0Class::~Unknown5B7AB0Class()
{
	if (InterfaceMesh) {
		delete InterfaceMesh;
		InterfaceMesh = NULL;
	}
}

// 0x5B7C40
void Unknown5B7AB0Class::Unknown_54()
{
	ButtonGadgetClass *button = new ButtonGadgetClass(InterfaceMesh, "MU01A.OPTION1", Layer, 5, 3);
	Unknown64.Unknown_6870E0(button, 1, View, 1, 0);
	Unknown_5BB4F0(button);
	button = new ButtonGadgetClass(InterfaceMesh, "MU01A.OPTION2", Layer, 5, 3);
	Unknown64.Unknown_6870E0(button, 2, View, 1, 0);
	Unknown_5BB4F0(button);
	button = new ButtonGadgetClass(InterfaceMesh, "MU01A.OPTION3", Layer, 5, 3);
	Unknown64.Unknown_6870E0(button, 3, View, 1, 0);
	Unknown_5BB4F0(button);
	button = new ButtonGadgetClass(InterfaceMesh, "MU01A.OPTION4", Layer, 5, 3);
	Unknown64.Unknown_6870E0(button, 4, View, 1, 0);
	Unknown_5BB4F0(button);
	button = new ButtonGadgetClass(InterfaceMesh, "MU01A.OPTION5", Layer, 5, 3);
	Unknown64.Unknown_6870E0(button, 5, View, 1, 0);
	Unknown_5BB4F0(button);
	Unknown_10();
}

// 0x5B7EE0
void Unknown5B7AB0Class::Unknown_58()
{
	InterfaceSortScopeClass sort(this);
	Unknown5BB320Class::Unknown_58();
	InterfaceMesh->Unknown_5B8DF0(0, "mu01a.mu01-idl", true, 0, 0, 0, -1);
	InterfaceMesh->Unknown_5B8EE0(0, 1, "mu01a.mu01-on", false, 1, 0, 0, -1);
}

// 0x5B7FA0
void Unknown5B7AB0Class::Unknown_5C()
{
	Unknown5BB320Class::Unknown_5C();
}

// 0x5B7FC0
bool Unknown5B7AB0Class::Unknown_68(int key)
{
	switch (key) {
		case 1:
			InterfaceMesh->Unknown_5B8EE0(0, 2, "mu01a.mu01-o1", false, 1, 0, 0, -1);
			break;
		case 2:
			InterfaceMesh->Unknown_5B8EE0(0, 2, "mu01a.mu01-o2", false, 1, 0, 0, -1);
			break;
		case 3:
			InterfaceMesh->Unknown_5B8EE0(0, 2, "mu01a.mu01-o3", false, 1, 0, 0, -1);
			break;
		case 4:
			InterfaceMesh->Unknown_5B8EE0(0, 2, "mu01a.mu01-o4", false, 1, 0, 0, -1);
			break;
		case 5:
			InterfaceMesh->Unknown_5B8EE0(0, 2, "mu01a.mu01-o5", false, 1, 0, 0, -1);
			break;
		default:
			return(false);
	}
	InterfaceMesh->Unknown_5B8EE0(0, 1, "mu01a.mu01-on", false, 1, 0, 0, -1);
	return(false);
}

// 0x5B80B0
void Unknown5B7AB0Class::Unknown_60()
{
	Unknown5BB320Class::Unknown_60();
}


/*
**	Notes (reconstruction).
**
**	The object is 0x5B7AB0-0x5B8120 (ui_mesh.cpp's 0x5B8120 follows): the constructor 0x5B7AB0, the deleting
**	destructor 0x5B7B70, the destructor 0x5B7BA0, slots +0x54 0x5B7C40, +0x58 0x5B7EE0, +0x5C 0x5B7FA0, +0x68 0x5B7FC0,
**	+0x60 0x5B80B0, the guarded initialiser of std::ctype<unsigned short>::id 0x5B80D0 (every <string> includer's) and
**	the deleting destructor's adjustor thunks 0x5B8100 (-4) / 0x5B8110 (-0x38). The constructor passes the base a NULL
**	mesh and makes mu01 in its body (the tables are stored before the new). Its only maker is the view at SClient
**	+0x1358 (mainview.cpp's 0x514E70); what the menu is for is not known yet (MU01A.OPTION1..5).
*/
