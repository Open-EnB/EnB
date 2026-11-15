// Earth & Beyond client (reconstruction): app\client\intrlist.cpp (__FILE__ 0xB8FD5C). Notes at the end of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members typed
#include "intrface.h"
#include "sclient.h"
#include "gadget.h"
#include "rendobj.h"
#include "scene.h"
#include "layer.h"
#include "..\input\inpdsp.h"
#include "..\kernel\kernel.h"
#include "..\shared\g_assert.h"
// What InterfaceUnknown58Class's list holds (8 bytes): a render object and the scene it goes into
struct InterfaceUnknown58EntryStruct  // NAME 1: neutral (D10)
{
	RenderObjClass *RObj;		// +0x00 (G_ASSERT 'RObj'; FAIL_IF 'RObj->Peek_Scene()')
	SceneClass *Scene;			// +0x04 (FAIL_IF '!Scene')
	InterfaceUnknown58EntryStruct(RenderObjClass *robj, SceneClass *scene);	// 0x686740
	~InterfaceUnknown58EntryStruct();											// 0x6867C0
	void Add();																	// 0x686840: the object into its scene  // NAME 2: RenderObjClass::Add's
	void Remove();																// 0x686880  // NAME 2: RenderObjClass::Remove's
	void Set_Scene(SceneClass *scene);											// 0x6868A0  // NAME 2: what it does
};
// What InterfaceUnknown64Class's list holds (0x10 bytes, 0x6870E0 fills it): a gadget, its key's data, its view
struct InterfaceUnknown64EntryStruct  // NAME 1: neutral (D10)
{
	GadgetClass *Gadget;		// +0x00 (slot +0x54 onto the view's manager; slot +0x5C its id, the key)
	int Data;					// +0x04 (non-zero: a key record pushed with it)
	UIViewClass *View;			// +0x08 (FAIL_IF '!View')  // NAME 3: the FAIL_IF's
	bool Active;				// +0x0C  // NAME 1: describes what it gates
	bool Unknown0D;				// +0x0D
	InterfaceUnknown64EntryStruct(GadgetClass *gadget, int data, UIViewClass *view, bool active, bool unknown);	// 0x686920
	~InterfaceUnknown64EntryStruct();											// 0x686960: the gadget destroyed unless Unknown0D
	void Unknown_686990(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *parent);	// 0x686990: InterfaceUnknown64Class::Unknown_6871D0's step  // NAME 1: neutral (D10)
	void Unknown_686A60(LoginUnknown0CClass *dispatcher, InterfaceClass *parent);	// 0x686A60: Unknown_6872A0's step  // NAME 1: neutral (D10)
	void Set_View(UIViewClass *view, LayerClass *layer);						// 0x686B20: Unknown_687360's step  // NAME 2: what it does
};
InterfaceUnknown58EntryStruct::InterfaceUnknown58EntryStruct(RenderObjClass *robj, SceneClass *scene) : RObj(NULL), Scene(NULL) { REF_PTR_SET(RObj, robj); REF_PTR_SET(Scene, scene); }
InterfaceUnknown58EntryStruct::~InterfaceUnknown58EntryStruct()
{ G_ASSERT(RObj); REF_PTR_RELEASE(RObj); REF_PTR_RELEASE(Scene); }
struct InterfaceUnknown64NodeStruct  // the list node (wwlib's SLNode: next, data; the SList's table 0xAFC8DC, head +0x04)  // NAME 1: neutral (D10)
{
	InterfaceUnknown64NodeStruct *Next;
	InterfaceUnknown64EntryStruct *Data;
};
void InterfaceUnknown58EntryStruct::Add()
{ FAIL_IF(!Scene) return; RObj->Add(Scene); }
void InterfaceUnknown58EntryStruct::Remove() { RObj->Remove(); }









void InterfaceUnknown58EntryStruct::Set_Scene(SceneClass *scene)
{ FAIL_IF(RObj->Peek_Scene()) RObj->Remove(); REF_PTR_SET(Scene, scene); }

InterfaceUnknown64EntryStruct::InterfaceUnknown64EntryStruct(GadgetClass *gadget, int data, UIViewClass *view, bool active, bool unknown) :
	Gadget(gadget), Data(data), View(view), Active(active), Unknown0D(unknown) {}

InterfaceUnknown64EntryStruct::~InterfaceUnknown64EntryStruct()
{
	if (!Unknown0D) { GadgetClass::Destroy(Gadget); Gadget = NULL; }
}




















void InterfaceUnknown64EntryStruct::Unknown_686990(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *parent)
{ FAIL_IF(!View) return; if (Active) { Gadget->Unknown_54(View->Unknown2C, parent); if (Data != 0) { KernelEventClass event((unsigned short)Gadget->Get_Id()); dispatcher->Unknown_429AF0(&event, (InputActionClass *)handler, Data, parent); } } }








void InterfaceUnknown64EntryStruct::Unknown_686A60(LoginUnknown0CClass *dispatcher, InterfaceClass *parent)
{
	FAIL_IF(!View) return;
	if (Active) {
		Gadget->Unknown_58(View->Unknown2C);
		if (Data != 0) {
			KernelEventClass event((unsigned short)Gadget->Get_Id());
			dispatcher->Unknown_429D90(&event, parent);
		}
	}
}

void InterfaceUnknown64EntryStruct::Set_View(UIViewClass *view, LayerClass *layer)
{
	View = view;
	Gadget->Unknown_28(layer);
}

InterfaceUnknown58Class::InterfaceUnknown58Class() {}
InterfaceUnknown58Class::~InterfaceUnknown58Class() { SLNode<InterfaceUnknown58EntryStruct> *next; for (SLNode<InterfaceUnknown58EntryStruct> *n = List.Head(); n; n = next) { InterfaceUnknown58EntryStruct *e = n->Data(); next = n->Next(); List.Remove(e); delete e; } }
void InterfaceUnknown58Class::Unknown_686CC0(RenderObjClass *r, SceneClass *s) { List.Add_Head(new InterfaceUnknown58EntryStruct(r, s)); }
void InterfaceUnknown58Class::Unknown_686DA0(RenderObjClass *&r, SceneClass *s) { Unknown_686CC0(r, s); }
void InterfaceUnknown58Class::Unknown_686DD0(RenderObjClass *r) { SLNode<InterfaceUnknown58EntryStruct> *next; for (SLNode<InterfaceUnknown58EntryStruct> *n = List.Head(); n; n = next) { InterfaceUnknown58EntryStruct *e = n->Data(); next = n->Next(); if (r == e->RObj) List.Remove(e); delete e; } }
// @0x686E90
void InterfaceUnknown58Class::Unknown_686E90() { for (SLNode<InterfaceUnknown58EntryStruct> *n = List.Head(); n; n = n->Next()) { n->Data()->Add(); } }
// @0x686EE0
void InterfaceUnknown58Class::Unknown_686EE0()
{
	for (SLNode<InterfaceUnknown58EntryStruct> *node = List.Head(); node != NULL; node = node->Next()) {
		node->Data()->Remove();
	}
}
// @0x686F10
void InterfaceUnknown58Class::Unknown_686F10(SceneClass *s)
{ for (SLNode<InterfaceUnknown58EntryStruct> *node = List.Head(); node != NULL; node = node->Next()) { node->Data()->Set_Scene(s); } }
// @0x686FA0
InterfaceUnknown64Class::InterfaceUnknown64Class() {}
// @0x686FD0
InterfaceUnknown64Class::~InterfaceUnknown64Class()
{
	SLNode<InterfaceUnknown64EntryStruct> *next;
	for (SLNode<InterfaceUnknown64EntryStruct> *node = List.Head(); node != NULL; node = next) {
		InterfaceUnknown64EntryStruct *entry = node->Data(); next = node->Next(); List.Remove(entry);
		delete entry;
	}
}
// @0x6870E0
void InterfaceUnknown64Class::Unknown_6870E0(GadgetClass *gadget, int id, UIViewClass *view, bool active, bool unknown)
{
	List.Add_Head(new InterfaceUnknown64EntryStruct(gadget, id, view, active, unknown));
}
// @0x687150
void InterfaceUnknown64Class::Unknown_687150(GadgetClass *gadget)
{
	SLNode<InterfaceUnknown64EntryStruct> *next;
	for (SLNode<InterfaceUnknown64EntryStruct> *node = List.Head(); node != NULL; node = next) {
		InterfaceUnknown64EntryStruct *entry = node->Data(); next = node->Next();
		if (gadget == entry->Gadget) {
			List.Remove(entry);
			delete entry;
		}
	}
}
// @0x6871D0
void InterfaceUnknown64Class::Unknown_6871D0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *parent) { for (InterfaceUnknown64NodeStruct *node = (InterfaceUnknown64NodeStruct *)List.Head(); node != NULL; node = node->Next) { node->Data->Unknown_686990(dispatcher, handler, parent); } }
// @0x6872A0
void InterfaceUnknown64Class::Unknown_6872A0(LoginUnknown0CClass *dispatcher, InterfaceClass *parent)
{
	for (InterfaceUnknown64NodeStruct *node = (InterfaceUnknown64NodeStruct *)List.Head(); node != NULL; node = node->Next) {
		node->Data->Unknown_686A60(dispatcher, parent);
	}
}
// @0x687360
void InterfaceUnknown64Class::Unknown_687360(UIViewClass *view)
{
	LayerClass layer;
	if (view != NULL) {
		layer.Set_Scene((SceneClass *)view->Unknown_50());
		layer.Set_Camera((CameraClass *)view->Unknown_54());
	}
	for (InterfaceUnknown64NodeStruct *node = (InterfaceUnknown64NodeStruct *)List.Head(); node != NULL; node = node->Next) {
		node->Data->Set_View(view, &layer);
	}
}

/*
**	Reconstruction notes
**
**	client.exe: intrlist.cpp's two list entries (0x686740-0x686B20: their constructors, destructors and
**	steps, out of line with no callers, the lists' loops inline them), InterfaceUnknown58Class
**	(0x686B50-0x686F10) and InterfaceUnknown64Class (0x686FA0-0x687360). The entries' methods hold the
**	asserts: line 39 (0x27) RObj (the first entry's destructor), line 46 (0x2e) !Scene (Add), line 58 (0x3a)
**	RObj->Peek_Scene() (Set_Scene), line 88 (0x58) and line 99 (0x63) !View (the second entry's two steps).
**	0x6865E0 (a float clamp, called from 0x6863E0 / 0x686010 / 0x515980) is the object before.
*/
