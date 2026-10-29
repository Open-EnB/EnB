#include "sclient.h"
#include "tasks\tsk_login.h"
#include "..\shared\unknown_debug.h"
#include <vector>

// local_interface_sort.cpp's sort object (8 bytes) over one interface object (0x541980; the main view's 0x512A00
// makes it from the view)
class LineDisplaySortClass  // NAME 1: neutral (D10)
{
public:
	LineDisplaySortClass(UIInterfaceBaseClass *inter);	// 0x541980  // NAME 1 [inter]: what 0x6B4860 passes
	~LineDisplaySortClass();							// 0x6E18B0

private:
	char Unknown00[8];
};

// the interface objects' flag at +0x7C (intrface.cpp's member; UIInterfaceBaseClass declares no members)
struct ViewInterfaceFlagStruct	// NAME 1: neutral (D10)
{
	char Unknown00[0x7C];
	bool Unknown7C;
};

// (RefCountPtr<UIInterfaceBaseClass> instantiated here, so that its friend Create_Get is found)
enum { VIEW_INTERFACE_PTR_SIZE = sizeof(RefCountPtr<UIInterfaceBaseClass>) };	// NAME 1: neutral (D10)

// 0x6B4480: the input dispatcher (0x18 bytes) to +0x04
ViewClass::ViewClass()
{
	Unknown04 = new LoginUnknown0CClass;
}




// 0x6B45D0 (deleting 0x6B45A0)
ViewClass::~ViewClass()
{
	G_ASSERT(!InterfaceObjects.Head());
	delete Unknown04;
	Unknown04 = NULL;
}






















// 0x6B46F0: slot +0x10: every interface object's +0x7C with 0, 0 (it takes itself off the list, 0x6B4990)
void ViewClass::Unknown_10()
{
	std::vector<RefCountPtr<UIInterfaceBaseClass> > objects;
	SLNode<UIInterfaceBaseClass> *node;
	for (node = InterfaceObjects.Head(); node != NULL; node = node->Next()) {
		objects.push_back(Create_Get(node->Data()));
	}
	for (std::vector<RefCountPtr<UIInterfaceBaseClass> >::iterator i = objects.begin(); i != objects.end(); i++) {
		(*i)->Unknown_7C(NULL, 0);
	}
	node = InterfaceObjects.Head();
	G_ASSERT(!node);
}

// 0x6B4860: slot +0x14: every interface object's +0x64, or its +0x60 under a sort object when its +0x7C is set
void ViewClass::Unknown_14()
{
	for (SLNode<UIInterfaceBaseClass> *node = InterfaceObjects.Head(); node != NULL; node = node->Next()) {
		UIInterfaceBaseClass *inter = node->Data();
		if (((ViewInterfaceFlagStruct *)inter)->Unknown7C) {
			LineDisplaySortClass sort(inter);
			inter->Unknown_60();
		} else {
			inter->Unknown_64();
		}
	}
}









// 0x6B4940: onto the list, referenced
void ViewClass::Unknown_6B4940(UIInterfaceBaseClass *inter)
{
	FAIL_IF(!inter) {return;}
	InterfaceObjects.Add_Tail(inter);
	inter->Add_Ref();
}
// 0x6B4990: off the list, released
bool ViewClass::Unknown_6B4990(UIInterfaceBaseClass *inter)
{
	FAIL_IF(!inter) {return(false);}
	if (InterfaceObjects.Remove(inter)) {
		inter->Release_Ref();
		return(true);
	}
	return(false);
}

/*
**	Notes (LD4, 2026-10-29)
**
**	view.cpp is ViewClass (sclient.h), UIViewClass's base: client 0x6B4480-0x6B4AD0 and the SList<> instances after
**	it (appfile.py show app\client\view.cpp); its asserts' __FILE__ 'e:\build\beta\G\app\client\view.cpp' (lines 40,
**	78, 106, 113). +0x04 is the input dispatcher (inpdsp.cpp, 0x18 bytes), +0x08 the interface objects, a WWLib
**	SList (table 0xAFDFF0) of intrface.cpp's objects (UIInterfaceBaseClass here; ui_line_display.h's InterfaceClass).
**	Not rebuilt: 0x6B4900 (every object's +0x6C with an argument; UIUnknownBaseClass's Unknown_6C takes none and
**	its subclasses override it so).
*/
