/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\encode.h (name assumed from encode.cpp:
**	the root's codec select 0x4A3FA0, in another object, uses RecordCodec and Get_Tree_Codec, so
**	the client declared them in a header)
**
**	The aux-data encoders and decoders of encode.cpp: the two codecs (record form, tree form), the
**	field lists an update is collected into, and the record table the encoder writes from. The
**	client names only what its asserts show (Records, record, b); the rest are neutral (D10, D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __ENCODE_H  // NAME 2 [encode.h]: encode.cpp is the client's; header name assumed (D57)
#define __ENCODE_H

#include "mempool.h"
#include "delta.h"
#include "bitset.h"

class LinearBufferClass;
class DurationClass;

// ------------------------------------------------------------------------- the field lists

// One entry of a field list (8 bytes, pooled: 0xBE3AE8, blocks of 256)
class DeltaListNodeClass : public AutoPoolClass<DeltaListNodeClass, 256>  // NAME 2: describes what it does
{
public:
	DeltaListNodeClass(DeltaClass *delta)
	{
		Next = 0;
		Delta = delta;
	}

	DeltaClass *Delta;					// +0x00  // NAME 2: describes what it does
	DeltaListNodeClass *Next;			// +0x04  // NAME 2: describes what it does
};

// A singly linked list of fields (0xC bytes), appended at the tail
class DeltaListClass  // NAME 2: describes what it does
{
public:
	DeltaListClass() : Head(0), Tail(&Head), Count(0) {}
	~DeltaListClass();													// 0x4AA710 (out of line)
	void Add(DeltaClass * const &delta);							// 0x4AA750  // NAME 2: describes what it does

	DeltaListNodeClass *Head;			// +0x00  // NAME 2: describes what it does
	DeltaListNodeClass **Tail;			// +0x04 the last node's Next (or Head)  // NAME 2: describes what it does
	int Count;								// +0x08  // NAME 2: describes what it does
};

// The three lists of an update (updated, defaulted, cleared), as the record form writes them
class DeltaListSetClass  // NAME 2: describes what it does
{
public:
	DeltaListSetClass(DeltaListClass *updated, DeltaListClass *defaulted, DeltaListClass *cleared);	// 0x4A91B0
	LinearBufferClass *Write(LinearBufferClass *buf);			// 0x4A91E0 (0 on a short write)  // NAME 2: describes what it does

	DeltaListClass *Lists[3];			// +0x00  // NAME 2: describes what it does
};

// ------------------------------------------------------------------------- the records

// One field's record (vtable 0xAEB2E4: the destructor and two pure slots)
class RecordClass  // NAME 3: after the asserts' Records (encode.cpp)
{
public:
	virtual ~RecordClass() {}
	virtual int Unknown_04() = 0;										// the bytes the tree form needs
	virtual bool Unknown_08(LinearBufferClass *buf) = 0;		// writes the tree form
};

// A leaf's record (vtable 0xAEB2C4, 0xC bytes, pooled: 0xBE3AD0, blocks of 128)
class LeafRecordClass : public RecordClass, public AutoPoolClass<LeafRecordClass, 128>  // NAME 2: describes what it does
{
public:
	LeafRecordClass(DeltaClass *delta, bool updated) : Delta(delta), Updated(updated) {}
	virtual int Unknown_04() { return 0; }										// 0x4AA0F0
	virtual bool Unknown_08(LinearBufferClass *buf);						// 0x4AA110

	DeltaClass *Delta;					// +0x04  // NAME 2: describes what it does
	bool Updated;							// +0x08 the value is written  // NAME 2: describes what it does
};

// A compound's record (vtable 0xAEB2D4, 0x30 bytes, pooled: 0xBE3AB8, blocks of 128): a bit per
// child for each state, or the whole compound in one state
class CompoundRecordClass : public RecordClass, public AutoPoolClass<CompoundRecordClass, 128>  // NAME 2: describes what it does
{
public:
	CompoundRecordClass(DeltaCompoundClass *compound, bool whole) :
		Compound(compound),
		Updated(compound->Children.size()),
		Defaulted(compound->Children.size()),
		Cleared(compound->Children.size()),
		Whole(whole)
	{
	}
	// (the implicit destructor, 0x4AA210: VC6 stores no vtable in it)
	virtual int Unknown_04();															// 0x4AA290
	virtual bool Unknown_08(LinearBufferClass *buf);						// 0x4AA320 (line 660)

	BitSetClass *Get_Bits(int list)  // NAME 2: describes what it does
	{
		switch (list) {
			case 0: return &Updated;
			case 1: return &Defaulted;
			case 2: return &Cleared;
		}
		return 0;
	}

	DeltaCompoundClass *Compound;		// +0x04  // NAME 2: describes what it does
	BitSetClass Updated;					// +0x08  // NAME 2: describes what it does
	BitSetClass Defaulted;				// +0x14  // NAME 2: describes what it does
	BitSetClass Cleared;					// +0x20  // NAME 2: describes what it does
	bool Whole;								// +0x2C the compound itself is in a list  // NAME 2: describes what it does
};

// The records of a tree's fields, indexed by their ids (DeltaClass::m_Unknown7C); size 8
class RecordTableClass  // NAME 2: describes what it does
{
public:
	RecordTableClass(int count, DeltaListClass *first, DeltaListClass *second, DeltaListClass *third);	// 0x4A9A70
	~RecordTableClass();															// 0x4A9B30
	int Unknown_Sum();															// 0x4A9BA0 (the records' Unknown_04)
	LinearBufferClass *Unknown_All(LinearBufferClass *buf);			// 0x4A9BE0 (0 when a record's Unknown_08 fails)
	void Add(DeltaListClass *list, int id);								// 0x4A9C30 (lines 568-597)  // NAME 2: describes what it does

	RecordClass **Records;		// +0x00 (the asserts' name)
	int Count;						// +0x04  // NAME 2: describes what it does
};

// ------------------------------------------------------------------------- the codecs

// The decoders of an aux-data update. The root's decode (DeltaCompoundClass::Decode, 0x4A3FA0)
// reads one byte and picks one: 0 the record form (RecordCodec), else the tree form
// (Get_Tree_Codec()). Neither codec's constructor stores a vtable for the common base, so it is
// declared novtable.
class __declspec(novtable) DeltaCodecClass  // NAME 2 [DeltaCodecClass]: describes what it does
{
public:
	virtual bool Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base) = 0;  // NAME 2: describes what it does
};

// The record form (vtable 0xAEB29C): three lists of field ids (updated, defaulted, cleared), each a
// count and the ids, 4 bytes each, an updated field's value after its id.
class RecordCodecClass : public DeltaCodecClass  // NAME 2: describes what it does
{
public:
	virtual bool Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base);	// 0x4A8FD0
};

// The tree form (vtable 0xAEB2AC, the visitor's 0xAEB2A0; size 0x18): from the root down, each
// compound starts with 4 header bits (all children in one state, then updated / defaulted /
// cleared), followed, when not all in one state, by one bit per child for each state present.
class TreeCodecClass : public DeltaCodecClass, public DeltaVisitorClass  // NAME 2: describes what it does
{
public:
	virtual bool Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base);	// 0x4A9350 (returns Error)
	virtual bool Read_Header(bool *whole, bool *updated, bool *defaulted, bool *cleared);		// 0x4A9710  // NAME 2: describes what it does
	virtual void Visit(DeltaCompoundClass *delta);			// 0x4A9400
	virtual void Visit(DeltaClass *delta);						// 0x4A9390

	LinearBufferClass *Buffer;			// +0x08  // NAME 2: describes what it does
	const DurationClass *Base;			// +0x0C  // NAME 2: describes what it does
	bool Error;								// +0x10  // NAME 2: describes what it does
	int Mode;								// +0x14 for Visit(DeltaClass *): 0 updated, 1 defaulted, 2 cleared  // NAME 2: describes what it does
};

// Owns the tree codec (0xBE3AB0; the codec pointer is 0xBE3B00)
class TreeCodecHolderClass  // NAME 2: describes what it does
{
public:
	TreeCodecHolderClass(TreeCodecClass *&codec);		// 0x4A99E0
	~TreeCodecHolderClass();									// 0x4A9A50

	TreeCodecClass *Codec;  // NAME 2: describes what it does
};

extern RecordCodecClass RecordCodec;		// 0xBE3AB4  // NAME 2: describes what it does
DeltaCodecClass *Get_Tree_Codec();			// 0x4A9B80  // NAME 2: describes what it does

#endif //__ENCODE_H
