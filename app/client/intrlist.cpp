// Earth & Beyond client (reconstruction): app\client\intrlist.cpp (__FILE__ 0xB8FD5C). Notes at the end of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members typed
#include "intrface.h"
#include "sclient.h"
#include "gadget.h"
#include "..\input\inpdsp.h"
#include "..\kernel\kernel.h"
#include "..\shared\unknown_debug.h"

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

// 0x686FA0
InterfaceUnknown64Class::InterfaceUnknown64Class()
{
}

// 0x686FD0
InterfaceUnknown64Class::~InterfaceUnknown64Class()
{
	SLNode<InterfaceUnknown64EntryStruct> *next;
	for (SLNode<InterfaceUnknown64EntryStruct> *node = List.Head(); node != NULL; node = next) {
		InterfaceUnknown64EntryStruct *entry = node->Data();
		next = node->Next();
		List.Remove(entry);
		if (entry != NULL) {
			if (!entry->Unknown0D) {
				GadgetClass::Destroy(entry->Gadget);
				entry->Gadget = NULL;
			}
			delete entry;
		}
	}
}

// 0x6870E0 (Add_Head: slot +0x08 of the SList, table 0xAFC8DC)
void InterfaceUnknown64Class::Unknown_6870E0(GadgetClass *gadget, int id, UIViewClass *view, bool active, bool unknown)
{
	List.Add_Head(new InterfaceUnknown64EntryStruct(gadget, id, view, active, unknown));
}


























void InterfaceUnknown64Class::Unknown_6871D0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *parent)
{
	for (InterfaceUnknown64NodeStruct *node = (InterfaceUnknown64NodeStruct *)List.Head(); node != NULL; node = node->Next) {
		InterfaceUnknown64EntryStruct *entry = node->Data;
		FAIL_IF(!entry->View) continue;
		if (entry->Active) {
			entry->Gadget->Unknown_54(entry->View->Unknown2C, parent);
			if (entry->Data != 0) {
				KernelEventClass event((unsigned short)entry->Gadget->Get_Id());
				dispatcher->Unknown_429AF0(&event, (InputActionClass *)handler, entry->Data, parent);
			}
		}
	}
}

/*
**	Reconstruction notes
**
**	client.exe: intrlist.cpp's InterfaceUnknown58Class (0x686B50-0x686F10) and InterfaceUnknown64Class
**	(0x686FA0-0x687360). Rebuilt so far: 0x6871D0 (activated: each active entry's gadget onto its
**	view's manager, its key pushed on the dispatcher), FAIL_IF '!View' on the client's line 88 (D25).
**	0x6871D0 exact (the list's head read as the raw node struct).
**	0x686FA0 / 0x686FD0 exact (MSG2): intrface.h types +0x00 as SList<InterfaceUnknown64EntryStruct>; the destructor
**	takes every entry out (Remove), destroys its gadget unless +0x0D, deletes it.
**	0x6870E0 exact (MSG2): the entry made with an inline constructor and put at the list's head through
**	SList<InterfaceUnknown64EntryStruct>'s Add_Head (slot +0x08; 0x687150 removes through slot +0x1C, Remove);
**	its last two arguments are bools (stored as bytes: intrface.h retyped from int).
**	File placement from the FAIL_IF's __FILE__; ACT (run 2026-11-02).
*/
