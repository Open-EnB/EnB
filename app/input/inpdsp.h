/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\input\inpdsp.h (name assumed from inpdsp.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\input\inpdsp.cpp')
**
**	The input dispatcher (0x18 bytes): an SGI hash_map (100 buckets, D87) from an input event's
**	first dword (key and flags) to a stack of records; the top record's action takes the event.
**	LoginTaskClass +0x0C and SClient +0x1270 own one; the interface list (intrlist.cpp) pushes and
**	removes records. tsk_login.h keeps an opaque declaration of the same class (char[0x18]) so
**	that its users compile as before (P-H18-1).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INPDSP_H  // NAME 3 [inpdsp.h]: header name assumed from the client's inpdsp.cpp
#define __INPDSP_H

#include <hash_map.h>
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\refcount.h"
#include "..\kernel\kernel.h"

// What a record calls with its data when its key's event arrives (slot +0x08 after RefCountClass's two)
class InputActionClass : public RefCountClass	// NAME 1 [InputActionClass]: neutral (D10): the record's 'Action' (inpdsp.cpp's assert)
{
public:
	virtual bool Unknown_08(int data) = 0;			// +0x08: true when it took the event  // NAME 1 [Unknown_08]: neutral (D10)
};

// One entry of a key's stack (0x10 bytes, allocated by 0x429AF0)
struct InputRecordStruct	// NAME 1 [InputRecordStruct]: neutral (D10): the client's asserts call one 'rec'
{
	InputRecordStruct(void *owner, int data, InputActionClass *action) : Owner(owner), Data(data), Action(action), Next(NULL) { action->Add_Ref(); }
	~InputRecordStruct() { REF_PTR_RELEASE(Action); Next = NULL; }

	void *Owner;						// +0x00: who pushed it (0x429D90 / 0x429E50 remove by it)  // NAME 2: what it holds
	int Data;							// +0x04: passed to the action  // NAME 2: what it holds
	InputActionClass *Action;			// +0x08: the client's 'rec->Action' (counted reference)
	InputRecordStruct *Next;			// +0x0C: the record below it  // NAME 2: what it holds
};

class LoginUnknown0CClass	// NAME 1: neutral (D10): what LoginTaskClass +0x0C points to (inpdsp.cpp)
{
public:
	LoginUnknown0CClass();																// 0x429630 (0x18 bytes)
	~LoginUnknown0CClass();																// 0x429800
	bool Unknown_429A20(KernelEventClass *event);										// 0x429A20: the top record's action takes the event  // NAME 1 [Unknown_429A20]: neutral (D10)
	bool Unknown_429AF0(KernelEventClass *event, InputActionClass *action, int data, void *owner);	// 0x429AF0: pushes a record (false: the owner has one)  // NAME 1 [Unknown_429AF0]: neutral (D10)
	bool Unknown_429D90(KernelEventClass *event, void *owner);							// 0x429D90: removes the owner's record of a key  // NAME 1 [Unknown_429D90]: neutral (D10)
	int Unknown_429E50(void *owner);													// 0x429E50: removes the owner's records of every key  // NAME 1 [Unknown_429E50]: neutral (D10)
	bool Unknown_429F60(InputRecordStruct *stack, unsigned int key, void *owner);		// 0x429F60: removes the owner's record from a stack  // NAME 1 [Unknown_429F60]: neutral (D10)

	unsigned int Unknown_42A170(KernelEventClass *event) const { return (event->Flags << 16) | event->Key; }	// 0x42A170 (out of line, no caller): the key of an event  // NAME 1 [Unknown_42A170]: neutral (D10)

	void Set_Stack(unsigned int key, InputRecordStruct *stack) { Stacks[key] = stack; }	// inline only: 0x429AF0's push onto a stack  // NAME 2: describes what it does

	std::hash_map<unsigned int, InputRecordStruct *> Stacks;							// +0x00: 100 buckets  // NAME 2: what it holds
};

bool Unknown_429D00();				// 0x429D00: false when HKLM\...\Earth and Beyond's DisableExceptions is a non-zero DWORD  // NAME 1 [Unknown_429D00]: neutral (D10)

#endif //__INPDSP_H
