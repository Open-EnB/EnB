// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	File PROVISIONAL (D137), as control_key_list.cpp (client 0x721C70-0x721FC0, no string).
**
**	The key list of a screen (InterfaceClass +0x4C): the key events a screen answers and the
**	handler id of each. ControlKeysManager::Add_Keys / Remove_Keys fill it from the user's key
**	mappings; the screens add their fixed keys (Escape, Return) themselves. The keys are the
**	kernel's input events (kernel.h's KernelEventClass, 16 bytes): the entry 0x721C70 copies one
**	whole and hands it to the input dispatcher (0x429AF0 / 0x429D90).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CONTROL_KEY_LIST_H  // NAME 3 [control_key_list.h]: header name assumed from control_key_list.cpp (PROVISIONAL)
#define __CONTROL_KEY_LIST_H

#include "slist.h"
#include "..\kernel\kernel.h"	// KernelEventClass

class LoginUnknown0CClass;
class RefCountClass;
class InterfaceClass;
struct InterfaceKeyEntryStruct;

class ControlKeyListClass  // NAME 1: neutral (D10): the screen's key list Add_Keys / Remove_Keys fill ('keylist')
{
public:
	ControlKeyListClass();																	// 0x721D50
	~ControlKeyListClass();																	// 0x721D80
	void Add(const KernelEventClass &event, void *handler);									// 0x721E60  // NAME 2: describes what it does
	void Remove(const KernelEventClass &event);												// 0x721EF0  // NAME 2: describes what it does
	void Unknown_721F40(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface);	// 0x721F40 (activated)  // NAME 1 [dispatcher, handler, iface]: what intrface.cpp passes
	void Unknown_721F80(LoginUnknown0CClass *dispatcher, InterfaceClass *iface);			// 0x721F80 (deactivated)  // NAME 1 [dispatcher, iface]: what intrface.cpp passes

	SList<InterfaceKeyEntryStruct> List;													// +0x00 (0x0C bytes)
};
typedef ControlKeyListClass InterfaceKeyListClass;

#endif //__CONTROL_KEY_LIST_H
