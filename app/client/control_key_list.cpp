// Earth & Beyond client (reconstruction): the control keys' key list, InterfaceClass +0x4C (client 0x721C70-0x721FC0).
// File PROVISIONAL (D137): no string; after control_keys_manager.cpp's container instances (to 0x721C00). Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members typed
#include "intrface.h"
#include "..\input\inpdsp.h"
#include "..\kernel\kernel.h"

// What InterfaceKeyListClass's list holds (0x14 bytes, 0x721E60 makes it with 0x721C70): a key event and its data
struct InterfaceKeyEntryStruct  // NAME 1: neutral (D10)
{
	void Unknown_721CC0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface);	// 0x721CC0: its key pushed on the dispatcher  // NAME 1 [dispatcher, handler, iface]: what 0x721F40 passes

	KernelEventClass Event;		// +0x00
	int Data;					// +0x10 (the handler's id: Add_Keys' handler)
};

// The list node (wwlib's SLNode: next, data); InterfaceKeyListClass is an SList (head +0x04)
struct InterfaceKeyNodeStruct  // NAME 1: neutral (D10)
{
	InterfaceKeyNodeStruct *Next;
	InterfaceKeyEntryStruct *Data;
};

void InterfaceKeyEntryStruct::Unknown_721CC0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface)
{
	dispatcher->Unknown_429AF0(&Event, (InputActionClass *)handler, Data, iface);
}

// NOT EXACT: relocation (ours inlines 0x721CC0, the client calls it)
void InterfaceKeyListClass::Unknown_721F40(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface)
{
	for (InterfaceKeyNodeStruct *node = *(InterfaceKeyNodeStruct **)(Unknown00 + 4); node != NULL; node = node->Next) {
		node->Data->Unknown_721CC0(dispatcher, handler, iface);
	}
}

/*
**	Reconstruction notes
**
**	client.exe: InterfaceKeyListClass (control_keys_manager.h's ControlKeyListClass: Add 0x721E60,
**	Remove 0x721EF0), an SList of key entries (0x14 bytes). Rebuilt so far: 0x721CC0 and 0x721F40
**	(activated: every key pushed on the dispatcher). 0x721CC0 exact; NOT EXACT (D64, functional
**	for the POC): 0x721F40 (0x721CC0 inlined). ACT (run 2026-11-02).
*/
