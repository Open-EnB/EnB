// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	// NAME 1 [unknown_root.h]: provisional file name, the object has no __FILE__
**	Original location: unknown (PROVISIONAL name, M1.3: the object 0x4A3990-0x4A4780 between
**	strconv_lib.cpp and bitset.cpp holds no assert and no __FILE__; renaming it later is fine)
**
**	The aux-data root: the DeltaCompoundClass every root tree (Ship, Player, ... in co.cpp) derives
**	from. The client names it nowhere (D24, D10). Its constructor 0x4A3BD0 inlines
**	DeltaCompoundClass's and keeps one int argument at +0x9C (the CREATE factory 0x6F6000 passes
**	its second argument through).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_ROOT_H
#define __UNKNOWN_ROOT_H

#include "delta.h"

class LinearBufferClass;
class PolicyClass;
class PolicyStateClass;
class ChangeTableClass;

// vtable 0xAEAB88 (DeltaCompoundClass's 0xAEABE4 with its own destructor), size 0xA0
// NAME 1: neutral; the root of every aux-data tree (vtable 0xAEAB88). Hints: C# AuxRoot (the reference game model; a schema object, not this class), Net-7 has none (its roots derive from AuxBase directly)
class UnknownRootClass : public DeltaCompoundClass
{
public:
	UnknownRootClass(int unknown, const char *name);		// 0x4A3BD0
	virtual ~UnknownRootClass();								// +0x04 0x4A3D30 (deleting 0x4A3D00)

	// 0x4A3D50: an update of what changed since the last (DeltaCollectorClass, encode.cpp), in the
	// record form or the tree form, whichever is smaller; 0 when there is nothing to send
	LinearBufferClass *Encode(PolicyClass *policy, ChangeTableClass *changes);  // NAME 2: describes what it does

	int m_Unknown9C;					// +0x9C the constructor's int (the factory's second argument)
};

// vtable 0xAEAC40, size 0xAC: a root that sends its fields (the Player root 0x752510 derives from
// it). Unknown_48 gives it a policy (everything INCLUDE) and a change table, and Encode_Changes
// writes an update from them.
// NAME 1: neutral, after UnknownRootClass; it adds the encoder's side (policy, change table)
class UnknownSendRootClass : public UnknownRootClass
{
public:
	UnknownSendRootClass(int unknown, const char *name);	// 0x4A3FF0 (name isn't used: the client's root is named "Hull")
	virtual ~UnknownSendRootClass();							// +0x04 0x4A4590 (deleting 0x4A4080)
	virtual void Unknown_48();										// +0x48 0x4A40B0
	// 0x4A4660 (no caller in the client): sets the change table's time, rescans it when a field
	// moved, and encodes when something changed
	LinearBufferClass *Encode_Changes(unsigned int time);  // NAME 2: describes what it does

	PolicyClass *Policy;					// +0xA0  // NAME 2: describes what it does
	PolicyStateClass *m_UnknownA4;	// +0xA4 the policy's state (0x4AF190's, 0x14 bytes)
	ChangeTableClass *Changes;			// +0xA8  // NAME 2: describes what it does
};

// The aux-data clock 0x4A3B40: milliseconds since its first call (timeGetTime).
unsigned int Unknown_Get_Ticks();

#endif //__UNKNOWN_ROOT_H
