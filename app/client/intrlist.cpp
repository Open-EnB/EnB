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

// What InterfaceUnknown64Class's list holds (0x10 bytes, 0x6870E0 fills it): a gadget, its key's data, its view
struct InterfaceUnknown64EntryStruct  // NAME 1: neutral (D10)
{
	GadgetClass *Gadget;		// +0x00 (slot +0x54 onto the view's manager; slot +0x5C its id, the key)
	int Data;					// +0x04 (non-zero: a key record pushed with it)
	UIViewClass *View;			// +0x08 (FAIL_IF '!View')  // NAME 3: the FAIL_IF's
	bool Active;				// +0x0C  // NAME 1: describes what it gates
	bool Unknown0D;				// +0x0D

	InterfaceUnknown64EntryStruct(GadgetClass *gadget, int data, UIViewClass *view, bool active, bool unknown) :
		Gadget(gadget), Data(data), View(view), Active(active), Unknown0D(unknown) {}
};
// The list node (wwlib's SLNode: next, data); InterfaceUnknown64Class is an SList (table 0xAFC8DC, head +0x04)
struct InterfaceUnknown64NodeStruct  // NAME 1: neutral (D10)
{
	InterfaceUnknown64NodeStruct *Next;
	InterfaceUnknown64EntryStruct *Data;
};

struct InterfaceUnknown58EntryStruct  // NAME 1: neutral (D10)
{
	RenderObjClass *RObj;		// +0x00
	SceneClass *Scene;			// +0x04

	InterfaceUnknown58EntryStruct(RenderObjClass *robj, SceneClass *scene) : RObj(NULL), Scene(NULL) { REF_PTR_SET(RObj, robj); REF_PTR_SET(Scene, scene); }
};
static inline void Release_Entry(InterfaceUnknown58EntryStruct *e) { if (e) { G_ASSERT(e->RObj); REF_PTR_RELEASE(e->RObj); REF_PTR_RELEASE(e->Scene); delete e; } }
InterfaceUnknown58Class::InterfaceUnknown58Class() {}
InterfaceUnknown58Class::~InterfaceUnknown58Class() { SLNode<InterfaceUnknown58EntryStruct> *next; for (SLNode<InterfaceUnknown58EntryStruct> *n = List.Head(); n; n = next) { InterfaceUnknown58EntryStruct *e = n->Data(); next = n->Next(); List.Remove(e); Release_Entry(e); } }
void InterfaceUnknown58Class::Unknown_686CC0(RenderObjClass *r, SceneClass *s) { List.Add_Head(new InterfaceUnknown58EntryStruct(r, s)); }
void InterfaceUnknown58Class::Unknown_686DA0(RenderObjClass *&r, SceneClass *s) { Unknown_686CC0(r, s); }
void InterfaceUnknown58Class::Unknown_686DD0(RenderObjClass *r) { SLNode<InterfaceUnknown58EntryStruct> *next; for (SLNode<InterfaceUnknown58EntryStruct> *n = List.Head(); n; n = next) { InterfaceUnknown58EntryStruct *e = n->Data(); next = n->Next(); if (r == e->RObj) List.Remove(e); Release_Entry(e); } }
// @0x686E90
void InterfaceUnknown58Class::Unknown_686E90() { for (SLNode<InterfaceUnknown58EntryStruct> *n = List.Head(); n; n = n->Next()) { FAIL_IF(!n->Data()->Scene) continue; n->Data()->RObj->Add(n->Data()->Scene); } }
// @0x686EE0
void InterfaceUnknown58Class::Unknown_686EE0()
{
	for (SLNode<InterfaceUnknown58EntryStruct> *node = List.Head(); node != NULL; node = node->Next()) {
		InterfaceUnknown58EntryStruct *entry = node->Data();
		entry->RObj->Remove();
	}
}
// @0x686F10
void InterfaceUnknown58Class::Unknown_686F10(SceneClass *s)
{ for (SLNode<InterfaceUnknown58EntryStruct> *node = List.Head(); node != NULL; node = node->Next()) { InterfaceUnknown58EntryStruct *entry = node->Data();
		FAIL_IF(entry->RObj->Peek_Scene()) entry->RObj->Remove(); REF_PTR_SET(entry->Scene, s); } }
// @0x686FA0
InterfaceUnknown64Class::InterfaceUnknown64Class() {}
// @0x686FD0
InterfaceUnknown64Class::~InterfaceUnknown64Class()
{
	SLNode<InterfaceUnknown64EntryStruct> *next;
	for (SLNode<InterfaceUnknown64EntryStruct> *node = List.Head(); node != NULL; node = next) {
		InterfaceUnknown64EntryStruct *entry = node->Data(); next = node->Next(); List.Remove(entry);
		if (entry != NULL) { if (!entry->Unknown0D) { GadgetClass::Destroy(entry->Gadget); entry->Gadget = NULL; } delete entry; }
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
			if (entry != NULL) { if (!entry->Unknown0D) { GadgetClass::Destroy(entry->Gadget); entry->Gadget = NULL; } delete entry; }
		}
	}
}
// @0x6871D0
void InterfaceUnknown64Class::Unknown_6871D0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *parent) { for (InterfaceUnknown64NodeStruct *node = (InterfaceUnknown64NodeStruct *)List.Head(); node != NULL; node = node->Next) { InterfaceUnknown64EntryStruct *entry = node->Data; FAIL_IF(!entry->View) continue; if (entry->Active) { entry->Gadget->Unknown_54(entry->View->Unknown2C, parent); if (entry->Data != 0) { KernelEventClass event((unsigned short)entry->Gadget->Get_Id()); dispatcher->Unknown_429AF0(&event, (InputActionClass *)handler, entry->Data, parent); } } } }

// @0x6872A0
void InterfaceUnknown64Class::Unknown_6872A0(LoginUnknown0CClass *dispatcher, InterfaceClass *parent)
{
	for (InterfaceUnknown64NodeStruct *node = (InterfaceUnknown64NodeStruct *)List.Head(); node != NULL; node = node->Next) {
		InterfaceUnknown64EntryStruct *entry = node->Data;




		FAIL_IF(!entry->View) continue;
		if (entry->Active) {
			entry->Gadget->Unknown_58(entry->View->Unknown2C);
			if (entry->Data != 0) {
				KernelEventClass event((unsigned short)entry->Gadget->Get_Id());
				dispatcher->Unknown_429D90(&event, parent);
			}
		}
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
		InterfaceUnknown64EntryStruct *entry = node->Data;
		entry->View = view;
		entry->Gadget->Unknown_28(&layer);
	}
}

/*
**	Reconstruction notes
**
**	client.exe: intrlist.cpp's InterfaceUnknown58Class (0x686B50-0x686F10) and InterfaceUnknown64Class
**	(0x686FA0-0x687360).
**	Asserts: line 39 (0x27) RObj, line 46 (0x2e) !Scene, line 58 (0x3a) RObj->Peek_Scene(), line 88 (0x58) !View, line 99 (0x63) !View.
*/
