// Earth & Beyond client (reconstruction): app\client\unknown_5bb250.cpp, the mesh windows' interface base (interim
// placement, D137: the client's object has no __FILE__ string; file name provisional, NAME 1). Notes at the end of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30)
#include "sclient.h"
#include "local_interface_sort.h"
#include "unknown_5bb250.h"

// 0x5BB250
Unknown5BB320Class::Unknown5BB320Class(UIMeshClass *mesh, UIViewClass *view, const LayerClass &layer) :
	InterfaceClass(view, (int)layer.Peek_Scene()),
	InterfaceMesh(mesh),
	Layer(layer)
{
}

// 0x5BB320
Unknown5BB320Class::Unknown5BB320Class(UIMeshClass *mesh, SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	InterfaceMesh(mesh)
{
}

// 0x5BB3B0 (the deleting destructor 0x5BB2F0)
Unknown5BB320Class::~Unknown5BB320Class()
{
	delete InterfaceMesh;
}

// 0x5BB470
void Unknown5BB320Class::Unknown_7C(UIViewClass *view, bool arg)
{
	if (view) {
		Layer.Set_Scene((SceneClass *)view->Unknown_50());
		Layer.Set_Camera((CameraClass *)view->Unknown_54());
	} else {
		Layer.Set_Scene(NULL);
		Layer.Set_Camera(NULL);
	}
	InterfaceClass::Unknown_7C(view, arg);
}

// 0x5BB4F0
void Unknown5BB320Class::Unknown_5BB4F0(GadgetClass *gadget)
{
	InterfaceMesh->Unknown_5B99C0(gadget);
}

// 0x5BB520
void Unknown5BB320Class::Unknown_5BB520(GadgetClass *gadget)
{
	InterfaceMesh->Unknown_5B99F0(gadget);
}

// 0x5BB550
void Unknown5BB320Class::Unknown_58()
{
	InterfaceSortScopeClass sort(this);

	InterfaceMesh->Unknown_5B9290((SimpleSceneClass *)Layer.Peek_Scene());
	InterfaceClass::Unknown_58();
}

// 0x5BB5E0
void Unknown5BB320Class::Unknown_5C()
{
	InterfaceSortScopeClass sort(this);

	InterfaceMesh->Unknown_5B92B0();
	InterfaceClass::Unknown_5C();
}

// 0x5BB660
void Unknown5BB320Class::Unknown_60()
{
	InterfaceMesh->Unknown_5B9410();
}

/*
**	Notes (L3, run 2026-11-10b)
**
**	The client's object 0x5BB250-0x5BB6D0: linked between ui_mesh.obj (to 0x5BB1F0) and ui_mfd.obj (its PermStringInitClass
**	static's initialiser is 0x5BB6D0), so its file sorts between 'ui_mesh.cpp' and 'ui_mfd.cpp'; no __FILE__ string, no
**	assert, no literal. In address order: the constructor over a view and a layer 0x5BB250, the deleting destructor
**	0x5BB2F0, the constructor over a client 0x5BB320, the destructor 0x5BB3B0, slot +0x7C 0x5BB470, the two gadget calls
**	0x5BB4F0 / 0x5BB520 (nothing calls the second), slots +0x58 0x5BB550, +0x5C 0x5BB5E0, +0x60 0x5BB660, the guarded
**	initialiser of std::ctype<unsigned short>::id 0x5BB680 (every <string> includer's), and the deleting destructor's
**	adjustor thunks 0x5BB6B0 (-4) / 0x5BB6C0 (-0x38). Slots +0x88 / +0x8C are the header's inline empties: the client
**	emits them in ui_analyze.obj (0x543750 / 0x543770), the first object in link order that makes a table with them.
**	The mesh is stored before the layer is constructed in 0x5BB250: a member declared before Layer, initialised in the list.
*/
