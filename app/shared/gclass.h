// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\gclass.h (name assumed from gclass.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\shared\gclass.cpp'; the client holds no string of this header)
**
**	GClass (the client's RTTI .?AVGClass@@: SignalMap::emit<GClass, TargetChangeEvent>): the base of
**	the UI's interfaces (intrface.h's InterfaceClass) and of whatever can target or be targeted. Two
**	one-pointer bases (tables 0xAEAB48, 3 slots, and 0xAEAB3C, 2 slots), table 0xAEAAD8 (21 slots) and
**	0xAEAACC, 0x38 bytes: +0x08 the flags (1: in the dirty list), +0x0C, +0x10 the SignalMap its target
**	changes are emitted on. A target is held through a node (ContextDataNode<GClass *, GClass *>: the
**	target as its value, the targeting object as its context) in the target's targeting list.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GCLASS_H  // NAME 3 [gclass.h]: header name assumed from the client's gclass.cpp
#define __GCLASS_H

#include "..\..\wwlib\listnode.h"
#include "..\client\unknown_signalbase.h"

class GClass;

typedef ContextDataNode<GClass *, GClass *> GClassTargetNode;  // NAME 1: neutral (D10): the target as the value, the targeting GClass as the context
typedef List<GClassTargetNode *> GClassTargetingList;  // NAME 1: neutral (D10): the asserts' Get_Targeting_List()

// The first base (table 0xAEAB48: the deleting destructor 0x4A19E0, 0x4A19A0, 0x4A19C0)
class UnknownGadgetBase00Class  // NAME 1: neutral (D10): the inline base's first table (0xAEAB48)
{
public:
	virtual ~UnknownGadgetBase00Class() {}												// 0x4A1980
	virtual GenericNode *Unknown_04() { return(NULL); }									// +0x04 0x4A19A0 (GClass's +0x10 adds it to the list 0x4A17B0 walks)
	virtual void Unknown_08() {}														// +0x08 0x4A19C0 (0x4A17B0 calls it for every node's value)
};

// The second base (table 0xAEAB3C; its +0x04 is 0x4A1930 in every derived table, intrface.cpp's 0xAEEF8C included)
class UnknownGadgetBase04Class  // NAME 1: neutral (D10): the inline base's second table (0xAEAB3C)
{
public:
	virtual ~UnknownGadgetBase04Class() {}												// 0x4A1910
	virtual bool Unknown_4A1930(int unknown1, int unknown2, int unknown3) { return(false); }	// +0x04 0x4A1930 (ret 0x0C)  // NAME 1: neutral (D10): its address; NAME 1 [unknown1, unknown2, unknown3]: neutral
};

// What GClass's SignalMap carries when the target node's target changes (0x0C bytes)
class TargetChangeEvent  // NAME 5: the client's RTTI .?AVTargetChangeEvent@@
{
public:
	TargetChangeEvent(GClass *object, GClass *old_target, GClass *new_target) : Object(object), OldTarget(old_target), NewTarget(new_target) {}	// NAME 1 [object, old_target, new_target]: what they hold

	GClass *Object;																		// +0x00  // NAME 1: neutral (D10)
	GClass *OldTarget;																	// +0x04  // NAME 1: neutral (D10)
	GClass *NewTarget;																	// +0x08  // NAME 1: neutral (D10)
};

class GClass : public UnknownGadgetBase00Class, public UnknownGadgetBase04Class  // NAME 5: the client's RTTI .?AVGClass@@ (constructor 0x4A1880)
{
public:
	GClass();																			// 0x4A1880
	GClass(const GClass &that);															// 0x4A1B40 (the SignalMap a new one)  // NAME 1 [that]: neutral
	virtual ~GClass();																	// 0x4A1BD0
	virtual const char *Unknown_0C() { return(NULL); }									// +0x0C 0x4A1A10 (+0x14 copies it)
	virtual void Unknown_10();															// +0x10 0x4A1800 (+0x04's node into the list 0x4A17B0 walks)
	virtual bool Unknown_14(int unknown1, int unknown2, char *buffer, int size);		// +0x14 0x4A1D80 (+0x0C into buffer)  // NAME 1 [unknown1, unknown2, buffer, size]: neutral
	virtual void Unknown_18();															// +0x18 0x4A2010 (once into the dirty list, flag 1)
	virtual int Unknown_1C() { return(0); }												// +0x1C 0x4A1A30
	virtual int Unknown_20() { return(0); }												// +0x20 0x4A1A50
	virtual int Unknown_24() { return(0); }												// +0x24 0x4A1A70
	virtual int Unknown_28() { return(0); }												// +0x28 0x4A1A90
	virtual int Unknown_2C() { return(0); }												// +0x2C 0x4A1AB0
	virtual bool Unknown_30(GClass *target, GClassTargetNode *node);					// +0x30 0x4A1C90 (node or the target node onto target: TargetChangeEvent)  // NAME 1 [target, node]: what it does with them
	virtual void Unknown_34();															// +0x34 0x4A1C60 (+0x38 of the target node)
	virtual void Unknown_38(GClassTargetNode *node);					// +0x38 0x4A1E20 (node off its target: TargetChangeEvent)  // NAME 1 [node]: what it does with it
	virtual void Unknown_3C(GClass *target);											// +0x3C 0x4A1DD0 (+0x34 when the target node holds target)  // NAME 1 [target]: what it compares
	virtual void Unknown_40();															// +0x40 0x4A1EC0 (every targeting node off this)
	virtual void Unknown_44(GClassTargetNode *node);									// +0x44 0x4A1F30 (node onto this, G_ASSERT line 325)  // NAME 1 [node]: what it does with it
	virtual GClass *Unknown_48();														// +0x48 0x4A1FD0 (the target node's target, G_ASSERT line 344)
	virtual GClassTargetNode *Get_Target_Node() { return(NULL); }						// +0x4C 0x4A1AD0  // NAME 3: the client's assert 'Get_Target_Node()'
	virtual GClassTargetingList *Get_Targeting_List() { return(NULL); }				// +0x50 0x4A1AF0  // NAME 3: the client's assert 'Get_Targeting_List() != NULL'

	static void Unknown_4A17B0();														// 0x4A17B0 (+0x08 of every node's value in the list +0x10 fills)  // NAME 1: neutral (D10): its address
	static void Unknown_4A20A0();														// 0x4A20A0 (empties the dirty list)  // NAME 1: neutral (D10): its address

	unsigned char Flags;																// +0x08 (1: in the dirty list, +0x18)  // NAME 1: neutral (D10)
	int Unknown0C;																		// +0x0C (the copy constructor copies it)
	SignalMap Signals;																	// +0x10 (0x4F9350 / 0x4F9530: TargetChangeEvent)  // NAME 1: neutral (D10)
};

#endif // __GCLASS_H
