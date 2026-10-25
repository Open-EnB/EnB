/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	// NAME 1 [unknown_changes.h]: provisional file name, the object has no __FILE__
**	Original location: unknown (PROVISIONAL name, M1.3: the object 0x4AB580-0x4AC250 between
**	linearbu.cpp and lookup.cpp holds no assert and no __FILE__)
**
**	The change table of a root that sends its fields (UnknownSendRootClass +0xA8, made by its
**	Unknown_48 0x4A40B0): one entry per field id, linked as the tree is, that remembers what was
**	last sent of each field (its m_Unknown70 then, when) and counts, per compound, the leaves below
**	that were sent and that changed since. The update collector (encode.cpp) decides from it what
**	an update carries. The client names none of it (D10, D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_CHANGES_H
#define __UNKNOWN_CHANGES_H

#include <vector>

class DeltaClass;
class DeltaCompoundClass;

// One field's entry (0x28 bytes). A compound's counts are over the leaves below it.
class ChangeEntryClass  // NAME 2: describes what it does
{
public:
	ChangeEntryClass() :
		Parent(0),
		Next(0),
		Child(0),
		Stamp(0),
		First(0),
		Last(0),
		Size(1),
		Sent(0),
		Changed(0),
		Is_Leaf(true),
		Included(true),
		Dirty(false)
	{
	}

	ChangeEntryClass *Parent;			// +0x00  // NAME 2: describes what it does
	ChangeEntryClass *Next;				// +0x04 the next sibling  // NAME 2: describes what it does
	ChangeEntryClass *Child;			// +0x08 the first child  // NAME 2: describes what it does
	unsigned int Stamp;					// +0x0C the field's m_Unknown70 when it was last sent (or scanned)  // NAME 2: describes what it does
	unsigned int First;					// +0x10 when it was sent (a compound: the earliest of its changed children)  // NAME 2: describes what it does
	unsigned int Last;					// +0x14 when it was sent (a compound: the latest of its changed children)  // NAME 2: describes what it does
	unsigned int Size;					// +0x18 the leaves (a leaf: 1)  // NAME 2: describes what it does
	unsigned int Sent;					// +0x1C the leaves sent  // NAME 2: describes what it does
	unsigned int Changed;				// +0x20 the leaves changed since they were sent  // NAME 2: describes what it does
	bool Is_Leaf;							// +0x24  // NAME 2: describes what it does
	bool Included;							// +0x25 counted in its parent's Changed  // NAME 2: describes what it does
	bool Dirty;								// +0x26 Changed below it moved: Scan sums it again  // NAME 2: describes what it does
};

// size 0x1C
class ChangeTableClass  // NAME 2: describes what it does
{
public:
	ChangeTableClass(DeltaCompoundClass *root);							// 0x4AB910
	~ChangeTableClass();														// 0x4ABB00

	void Unsent_Parents(ChangeEntryClass *entry);						// 0x4AB5F0 (inlined in its callers)  // NAME 2: describes what it does
	void Set_Sent(int id, unsigned int stamp);							// 0x4AB660  // NAME 2: describes what it does
	void Set_Sent(DeltaClass *delta);										// 0x4AB6F0 (the field and the fields below)  // NAME 2: describes what it does
	ChangeEntryClass *Set_Entry_Sent(int id, unsigned int stamp);		// (inline) the field as sent now  // NAME 2: describes what it does
	void Set_Unsent(int id);													// 0x4AB840  // NAME 2: describes what it does
	void Set_Unsent(ChangeEntryClass *entry);							// 0x4AB8C0  // NAME 2: describes what it does
	void Scan();																	// 0x4ABB40 (fields whose m_Unknown70 moved count as changed)  // NAME 2: describes what it does
	void Exclude(int id);														// 0x4ABCD0  // NAME 2: describes what it does
	void Include_All();														// 0x4ABD20 (no caller in the client)  // NAME 2: describes what it does
	void Include(ChangeEntryClass *entry);								// 0x4ABD90 (the entry and its next siblings)  // NAME 2: describes what it does

	std::vector<ChangeEntryClass> Entries;		// +0x00 by field id (DeltaClass::m_Unknown7C)  // NAME 2: describes what it does
	ChangeEntryClass *Root;							// +0x10 the root's entry  // NAME 2: describes what it does
	unsigned int Time;								// +0x14 now (UnknownSendRootClass::Unknown_Encode sets it)  // NAME 2: describes what it does
	DeltaCompoundClass *Delta;						// +0x18 the root  // NAME 2: describes what it does
};

#endif //__UNKNOWN_CHANGES_H
