// Earth & Beyond client (reconstruction): the control keys' key list, InterfaceClass +0x4C (client 0x721C70-0x721FC0).
// File PROVISIONAL (D137): no string; after control_keys_manager.cpp's container instances (to 0x721C00). Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members typed
#include "intrface.h"
#include "control_keys_manager.h"
#include "..\input\inpdsp.h"
#include "..\kernel\kernel.h"

// What InterfaceKeyListClass's list holds (0x14 bytes, 0x721E60 makes it with 0x721C70): a key event and its data
struct InterfaceKeyEntryStruct  // NAME 1: neutral (D10)
{
	InterfaceKeyEntryStruct(const KernelEventClass &event, int data);
	void Unknown_721CC0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface);	// 0x721CC0: its key pushed on the dispatcher  // NAME 1 [dispatcher, handler, iface]: what 0x721F40 passes
	void Unknown_721CF0(LoginUnknown0CClass *dispatcher, InterfaceClass *iface);							// 0x721CF0: its key removed from the dispatcher

	KernelEventClass Event;		// +0x00
	int Data;					// +0x10 (the handler's id: Add_Keys' handler)
};

#pragma auto_inline(off)

// 0x721C70
InterfaceKeyEntryStruct::InterfaceKeyEntryStruct(const KernelEventClass &event, int data) :
	Event()
{
	Event = event;
	Data = data;
}

// 0x721CC0
void InterfaceKeyEntryStruct::Unknown_721CC0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface)
{
	dispatcher->Unknown_429AF0(&Event, (InputActionClass *)handler, Data, iface);
}

// 0x721CF0
void InterfaceKeyEntryStruct::Unknown_721CF0(LoginUnknown0CClass *dispatcher, InterfaceClass *iface)
{
	dispatcher->Unknown_429D90(&Event, iface);
}

#pragma auto_inline(on)

// 0x721D50
ControlKeyListClass::ControlKeyListClass()
{
}

// 0x721D80
ControlKeyListClass::~ControlKeyListClass()
{
	SLNode<InterfaceKeyEntryStruct> *next;
	for (SLNode<InterfaceKeyEntryStruct> *node = List.Head(); node != NULL; node = next) {
		InterfaceKeyEntryStruct *entry = node->Data();
		next = node->Next();
		List.Remove(entry);
		delete entry;
	}
}

// NOT EXACT: shape (calls Add_Head on new entry; client has exception unwind frame)
// 0x721E60
void ControlKeyListClass::Add(const ControlKeyComboStruct &combo, void *handler)
{
	List.Add_Head(new InterfaceKeyEntryStruct(*(const KernelEventClass *)&combo, (int)handler));
}

// 0x721EF0
void ControlKeyListClass::Remove(const ControlKeyComboStruct &combo)
{
	SLNode<InterfaceKeyEntryStruct> *next;
	for (SLNode<InterfaceKeyEntryStruct> *node = List.Head(); node != NULL; node = next) {
		InterfaceKeyEntryStruct *entry = node->Data();
		next = node->Next();
		if (entry->Event.Key == combo.Key) {
			List.Remove(entry);
			delete entry;
		}
	}
}

// 0x721F40
void ControlKeyListClass::Unknown_721F40(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface)
{
	for (SLNode<InterfaceKeyEntryStruct> *node = List.Head(); node != NULL; node = node->Next()) {
		node->Data()->Unknown_721CC0(dispatcher, handler, iface);
	}
}

// 0x721F80
void ControlKeyListClass::Unknown_721F80(LoginUnknown0CClass *dispatcher, InterfaceClass *iface)
{
	for (SLNode<InterfaceKeyEntryStruct> *node = List.Head(); node != NULL; node = node->Next()) {
		node->Data()->Unknown_721CF0(dispatcher, iface);
	}
}

/*
**	Reconstruction notes
**
**	client.exe: InterfaceKeyListClass (control_keys_manager.h's ControlKeyListClass: Add 0x721E60,
**	Remove 0x721EF0), an SList of key entries (0x14 bytes).
**	0x721CC0, 0x721D50, 0x721D80, 0x721EF0, 0x721F40, 0x721F80, 0x721FC0 exact.
**	NOT EXACT (D64, functional for the POC): 0x721E60 (shape).
**	File PROVISIONAL (D137: no string; after control_keys_manager.cpp's container instances).
*/
