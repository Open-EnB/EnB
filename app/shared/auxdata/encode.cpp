// Earth & Beyond client (reconstruction): app\shared\auxdata\encode.cpp. Notes at the end of the file.
#include <string.h>
#include "delta.h"
#include "lookup.h"
#include "linearbu.h"
#include "bitset.h"
#include "..\unknown_debug.h"

// One field's record (vtable 0xAEB2E4: the destructor and two pure slots; the leaf records' vtable
// is 0xAEB2C4, the compound records' 0xAEB2D4). The header these are declared in is not known.
class RecordClass
{
public:
	virtual ~RecordClass() {}
	virtual int Unknown_04() = 0;
	virtual bool Unknown_08(LinearBufferClass *buf) = 0;
};

// The records of a tree's fields, indexed by their ids (DeltaClass::m_Unknown7C); size 8
class RecordTableClass
{
public:
	RecordTableClass(int count, DeltaClass **first, DeltaClass **second, DeltaClass **third);	// 0x4A9A70
	~RecordTableClass();															// 0x4A9B30
	int Unknown_Sum();															// 0x4A9BA0 (the records' Unknown_04)
	LinearBufferClass *Unknown_All(LinearBufferClass *buf);			// 0x4A9BE0 (0 when a record's Unknown_08 fails)
	void Add(DeltaClass **list, int id);									// 0x4A9C30 (not rebuilt yet)

	RecordClass **Records;		// +0x00 (the asserts' name)
	int Count;						// +0x04
};

// The decoders of an aux-data update (names: D10/D24, the client names none of them). The root's
// decode (0x4A3FA0) reads one byte and picks one: 0 the record form (RecordCodec), else the tree
// form (Get_Tree_Codec()). Neither class's constructor stores a vtable for the common base, so it
// is declared novtable.
class __declspec(novtable) DeltaCodecClass
{
public:
	virtual bool Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base) = 0;
};

// The record form (vtable 0xAEB29C): three lists of field ids (updated, defaulted, cleared), each a
// count and the ids, 4 bytes each, an updated field's value after its id.
class RecordCodecClass : public DeltaCodecClass
{
public:
	virtual bool Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base);	// 0x4A8FD0
};

// The tree form (vtable 0xAEB2AC, the visitor's 0xAEB2A0; size 0x18): from the root down, each
// compound starts with 4 header bits (all children in one state, then updated / defaulted /
// cleared), followed, when not all in one state, by one bit per child for each state present.
class TreeCodecClass : public DeltaCodecClass, public DeltaVisitorClass
{
public:
	virtual bool Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base);	// 0x4A9350 (returns Error)
	virtual bool Read_Header(bool *whole, bool *updated, bool *defaulted, bool *cleared);		// 0x4A9710
	virtual void Visit(DeltaCompoundClass *delta);			// 0x4A9400
	virtual void Visit(DeltaClass *delta);						// 0x4A9390

	LinearBufferClass *Buffer;			// +0x08
	const DurationClass *Base;			// +0x0C
	bool Error;								// +0x10
	int Mode;								// +0x14 for Visit(DeltaClass *): 0 updated, 1 defaulted, 2 cleared
};

// Owns the tree codec (0xBE3AB0; the codec pointer is 0xBE3B00)
class TreeCodecHolderClass
{
public:
	TreeCodecHolderClass(TreeCodecClass *&codec);		// 0x4A99E0
	~TreeCodecHolderClass();									// 0x4A9A50

	TreeCodecClass *Codec;
};

DeltaCodecClass *Get_Tree_Codec();		// 0x4A9B80

// (lines down to 170 hold code of the client's file not rebuilt yet, 0x4A8990-0x4A8FC0: its
// asserts are on lines 103 and 169)





























































































bool RecordCodecClass::Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base)
{
	LookupClass *lookup = root->Get_Lookup();
	int updated = 0;
	if (buf.Read(&updated, 4) != 4) {
		return false;
	}
	for (int i = 0; i < updated; i++) {
		int id = 0;
		if (buf.Read(&id, 4) != 4) {
			return false;
		}
		if (!lookup->Get_Delta(id)->Unknown_10(buf, base)) {
			return false;
		}
	}
	int defaulted = 0;
	if (buf.Read(&defaulted, 4) != 4) {
		return false;
	}
	for (i = 0; i < defaulted; i++) {
		int id = 0;
		if (buf.Read(&id, 4) != 4) {
			return false;
		}
		lookup->Get_Delta(id)->Unknown_18();
	}
	int cleared = 0;
	if (buf.Read(&cleared, 4) != 4) {
		return false;
	}
	for (i = 0; i < cleared; i++) {
		int id = 0;
		if (buf.Read(&id, 4) != 4) {
			return false;
		}
		lookup->Get_Delta(id)->Unknown_1C();
	}
	return true;
}

RecordCodecClass RecordCodec;		// 0xBE3AB4 (constructed by 0x4A9190)

// (0x4A91B0 and the record-form encoder 0x4A91E0, not rebuilt yet)

bool TreeCodecClass::Decode(DeltaCompoundClass *root, LinearBufferClass &buf, const DurationClass &base)
{
	Buffer = &buf;
	Base = &base;
	Error = false;
	root->Accept(*this);
	return Error;
}

void TreeCodecClass::Visit(DeltaClass *delta)
{
	switch (Mode) {
		case 0:
			Error = !delta->Unknown_10(*Buffer, *Base);
			break;
		case 1:
			delta->Unknown_18();
			break;
		case 2:
			delta->Unknown_1C();
			break;
	}
}









































































































































// NOT EXACT: the body matches the client's instruction for instruction, but VC6 gives the early
// paths (header not read, whole compound in one state) their own epilogues and pushes ebp only
// for the per-child path, where the client jumps to one shared epilogue; the client also places the
// Mode = 0 / Mode = 1 blocks after the function's end. Tried: returns, if/else chains, a goto.
void TreeCodecClass::Visit(DeltaCompoundClass *delta)
{
	int count = delta->Children.size();
	bool whole;
	bool updated;
	bool defaulted;
	bool cleared;
	if (!Read_Header(&whole, &updated, &defaulted, &cleared)) {
		Error = true;
	} else if (whole) {
		if (updated) {
			Error = !delta->Unknown_10(*Buffer, *Base);
		} else if (defaulted) {
			delta->Unknown_18();
		} else if (cleared) {
			delta->Unknown_1C();
		} else {
			G_ASSERT(false);
		}
	} else {
	int updated_pos = 0;
	int defaulted_pos = 0;
	int cleared_pos = 0;
	int bit_count = 4;
	if (updated) {
		updated_pos = bit_count;
		bit_count += count;
	}
	if (defaulted) {
		defaulted_pos = bit_count;
		bit_count += count;
	}
	if (cleared) {
		cleared_pos = bit_count;
		bit_count += count;
	}
	BitSetClass bits(bit_count);
	int size = bits.NumBytes;
	if (Buffer->Read(bits.Buffer, size) != size) {
		Error = true;
	} else {
		for (int i = 0; i < count; i++) {
			bool is_updated = updated && bits.Is_Set(updated_pos + i);
			bool is_defaulted = defaulted && bits.Is_Set(defaulted_pos + i);
			bool is_cleared = cleared && bits.Is_Set(cleared_pos + i);
			if (is_updated) {
				Mode = 0;
			} else if (is_defaulted) {
				Mode = 1;
			} else if (is_cleared) {
				Mode = 2;
			}
			if (is_updated || is_defaulted || is_cleared) {
				delta->Children[i]->Accept(*this);
			}
			if (Error) {
				break;
			}
		}
	}
	}
}

bool TreeCodecClass::Read_Header(bool *whole, bool *updated, bool *defaulted, bool *cleared)
{
	int pos = Buffer->Get_Pos();
	BitSetClass bits(4);
	int size = bits.NumBytes;
	if (Buffer->Read(bits.Buffer, size) != size) {
		return false;
	}
	*whole = bits.Is_Set(0);
	*updated = bits.Is_Set(1);
	*defaulted = bits.Is_Set(2);
	*cleared = bits.Is_Set(3);
	if (!*whole) {
		Buffer->Seek(pos);
	}
	return true;
}

// (two record allocators' statics, 0x4A9850-0x4A9957, not rebuilt yet)

TreeCodecClass *TreeCodec;										// 0xBE3B00
static TreeCodecHolderClass TreeCodecHolder(TreeCodec);		// 0xBE3AB0 (0x4A9970)

TreeCodecHolderClass::TreeCodecHolderClass(TreeCodecClass *&codec)
{
	Codec = codec = new TreeCodecClass;
}

TreeCodecHolderClass::~TreeCodecHolderClass()
{
	delete Codec;
}




























RecordTableClass::RecordTableClass(int count, DeltaClass **first, DeltaClass **second, DeltaClass **third) :
	Records(0),
	Count(0)
{
	Records = new RecordClass *[count];
	G_ASSERT(Records);
	Count = count;
	memset(Records, 0, count * sizeof(RecordClass *));
	Add(first, 0);
	Add(second, 1);
	Add(third, 2);
}

RecordTableClass::~RecordTableClass()
{
	for (int i = 0; i < Count; i++) {
		if (Records[i] != 0) {
			delete Records[i];
		}
	}
	delete [] Records;
}

DeltaCodecClass *Get_Tree_Codec()
{
	return TreeCodec;
}

int RecordTableClass::Unknown_Sum()
{
	int total = 0;
	for (int i = 0; i < Count; i++) {
		if (Records[i] != 0) {
			total += Records[i]->Unknown_04();
		}
	}
	return total;
}

LinearBufferClass *RecordTableClass::Unknown_All(LinearBufferClass *buf)
{
	for (int i = 0; i < Count; i++) {
		if (Records[i] != 0 && !Records[i]->Unknown_08(buf)) {
			return 0;
		}
	}
	return buf;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A8FD0-0x4A9C1A. The decoders (milestones.md 2.3):
**	* RecordCodecClass::Decode 0x4A8FD0 (vtable 0xAEB29C, the global 0xBE3AB4 made by 0x4A9190):
**	  updated ids (each field's Unknown_10 reads its value), defaulted ids (Unknown_18), cleared
**	  ids (Unknown_1C), the ids resolved through the root's LookupClass.
**	* TreeCodecClass 0x4A9350-0x4A9800 (vtables 0xAEB2AC and, for the DeltaVisitorClass base,
**	  0xAEB2A0; 0x18 bytes, made by TreeCodecHolderClass 0x4A99E0 at start-up, 0x4A9970). Decode
**	  sets Buffer/Base, clears Error and has the root Accept the codec; it returns Error (the record
**	  form returns success: the client's caller 0x4A3FA0 passes either on). Visit(compound) 0x4A9400
**	  reads the 4 header bits (Read_Header 0x4A9710 peeks at them and seeks back unless "whole"),
**	  then either applies one state to the whole compound or reads a bitset of 4 + count bits per
**	  state present and sets Mode before each child's Accept; Visit(leaf) 0x4A9390 applies Mode.
**	* Get_Tree_Codec 0x4A9B80.
**	RecordTableClass (D24; the asserts name Records), size 8: the constructor 0x4A9A70 (line 512:
**	Records allocated and zeroed, then Add for the three lists with ids 0, 1, 2), the destructor
**	0x4A9B30, Unknown_Sum 0x4A9BA0 and Unknown_All 0x4A9BE0 (loops over the records' slots 4 and 8).
**	Not rebuilt yet: 0x4A8990-0x4A8FC0 (the encoders; asserts on lines 103 and 169), 0x4A91B0,
**	the record-form encoder 0x4A91E0, the record allocators' statics 0x4A9850-0x4A9957, Add
**	0x4A9C30 (lines 568-597: one record per field id; leaf records 0xC bytes with their own
**	operator new 0x4AA8E0, compound ones with a bitset, 0x4AAA00), the records' methods
**	0x4AA0D0-0x4AA320 (line 660), 0x4AA710-0x4AA8B0.
*/
