// Earth & Beyond client (reconstruction): app\shared\auxdata\encode.cpp. Notes at the end of the file.
#include <string.h>
#include "encode.h"
#include "lookup.h"
#include "linearbu.h"
#include "..\unknown_debug.h"

// The pools (mempool.h): the list nodes' 0xBE3AE8 (0x4A8A60), the records' 0xBE3AD0 and 0xBE3AB8
// (0x4A9850, 0x4A98E0)
DEFINE_AUTO_POOL(DeltaListNodeClass, 256)

// (lines down to 175 hold code of the client's file not rebuilt yet, 0x4A8990-0x4A8FC0: the
// update collector, a DeltaVisitorClass with the three lists, vtable 0xAEB290; its asserts are on
// lines 103 and 169)
































































































































































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


DeltaListSetClass::DeltaListSetClass(DeltaListClass *updated, DeltaListClass *defaulted, DeltaListClass *cleared)
{
	Lists[0] = updated;
	Lists[1] = defaulted;
	Lists[2] = cleared;
}

LinearBufferClass *DeltaListSetClass::Write(LinearBufferClass *buf)
{
	int count = Lists[0]->Count;
	if (buf->Write(&count, 4) != 4) {
		return 0;
	}
	int id;
	for (DeltaListNodeClass *node = Lists[0]->Head; node != 0; node = node->Next) {
		DeltaClass *delta = node->Delta;
		id = delta->m_Unknown7C;
		if (buf->Write(&id, 4) != 4) {
			return 0;
		}
		if (!delta->Unknown_0C(*buf)) {
			return 0;
		}
	}
	id = Lists[1]->Count;
	if (buf->Write(&id, 4) != 4) {
		return 0;
	}
	for (node = Lists[1]->Head; node != 0; node = node->Next) {
		id = node->Delta->m_Unknown7C;
		if (buf->Write(&id, 4) != 4) {
			return 0;
		}
	}
	id = Lists[2]->Count;
	if (buf->Write(&id, 4) != 4) {
		return 0;
	}
	for (node = Lists[2]->Head; node != 0; node = node->Next) {
		id = node->Delta->m_Unknown7C;
		if (buf->Write(&id, 4) != 4) {
			return 0;
		}
	}
	return buf;
}

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

DEFINE_AUTO_POOL(LeafRecordClass, 128)
DEFINE_AUTO_POOL(CompoundRecordClass, 128)

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



























RecordTableClass::RecordTableClass(int count, DeltaListClass *first, DeltaListClass *second, DeltaListClass *third) :
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





// NOT EXACT: the client keeps the node in the list argument's slot, calls the first
// Children.size() out of line (0x4A58C0) in the new compound record and allocates the walk's
// records inline; VC6 here inlines all three sizes and uses other slots.
void RecordTableClass::Add(DeltaListClass *list, int id)
{
	for (DeltaListNodeClass *node = list->Head; node != 0; node = node->Next) {
		DeltaClass *delta = node->Delta;
		int index = delta->m_Unknown7C;
		G_ASSERT(Records[index] == 0);
		if (delta->As_Compound() != 0) {
			CompoundRecordClass *record = new CompoundRecordClass(delta->As_Compound(), true);
			switch (id) {
				case 0:
					record->Updated.Set_All();
					break;
				case 1:
					record->Defaulted.Set_All();
					break;
				case 2:
					record->Cleared.Set_All();
					break;
			}
			Records[index] = record;
		} else {
			Records[index] = new LeafRecordClass(delta, id == 0);
		}

		G_ASSERT(Records[index]);
		int bit = delta->m_Unknown78;
		for (DeltaClass *parent = delta->Parent; parent != 0; parent = parent->Parent) {
			int parent_index = parent->m_Unknown7C;
			CompoundRecordClass *record = (CompoundRecordClass *)Records[parent_index];
			if (record == 0) {
				record = new CompoundRecordClass((DeltaCompoundClass *)parent, false);
				Records[parent_index] = record;


				G_ASSERT(record);
			}
			BitSetClass *bits = record->Get_Bits(id);
			if (bits->Is_Set(bit)) {
				break;
			}
			bits->Set(bit);
			bit = parent->m_Unknown78;
		}
	}
}

bool LeafRecordClass::Unknown_08(LinearBufferClass *buf)
{
	if (Updated) {
		return Delta->Unknown_0C(*buf);
	}
	return true;
}

int CompoundRecordClass::Unknown_04()
{
	if (Whole) {
		return 1;
	}
	int bits = 4;
	if (Updated.Is_Any_Set()) {
		bits += Updated.NumBits;
	}
	if (Defaulted.Is_Any_Set()) {
		bits += Defaulted.NumBits;
	}
	if (Cleared.Is_Any_Set()) {
		bits += Cleared.NumBits;
	}
	return bits / 8 + (bits % 8 != 0);
}












bool CompoundRecordClass::Unknown_08(LinearBufferClass *buf)
{
	if (Whole) {
		BitSetClass b(4);
		b.Set(0);
		if (Updated.Is_Any_Set()) {
			b.Set(1);
		}
		if (Defaulted.Is_Any_Set()) {
			b.Set(2);
		}
		if (Cleared.Is_Any_Set()) {
			b.Set(3);
		}
		G_ASSERT(( (b[1] && b[2]) || (b[2] && b[3]) || (b[1] && b[3] ) ) == false);
		int size = b.NumBytes;
		if (buf->Write(b.Buffer, size) != size) {
			return false;
		}
		if (b[1]) {
			return Compound->Unknown_0C(*buf);
		}
		return true;
	}
	int count = Updated.NumBits;
	int bit_count = 4;
	int updated_pos = 0;
	int defaulted_pos = 0;
	int cleared_pos = 0;
	if (Updated.Is_Any_Set()) {
		updated_pos = bit_count;
		bit_count += count;
	}
	if (Defaulted.Is_Any_Set()) {
		defaulted_pos = bit_count;
		bit_count += count;
	}
	if (Cleared.Is_Any_Set()) {
		cleared_pos = bit_count;
		bit_count += count;
	}
	BitSetClass b(bit_count);
	if (Updated.Is_Any_Set()) {
		b.Set(1);
		for (int i = 0; i < count; i++) {
			if (Updated.Is_Set(i)) {
				b.Set(updated_pos + i);
			}
		}
	}
	if (Defaulted.Is_Any_Set()) {
		b.Set(2);
		for (int i = 0; i < count; i++) {
			if (Defaulted.Is_Set(i)) {
				b.Set(defaulted_pos + i);
			}
		}
	}
	if (Cleared.Is_Any_Set()) {
		b.Set(3);
		for (int i = 0; i < count; i++) {
			if (Cleared.Is_Set(i)) {
				b.Set(cleared_pos + i);
			}
		}
	}
	int size = b.NumBytes;
	if (buf->Write(b.Buffer, size) != size) {
		return false;
	}
	return true;
}

DeltaListClass::~DeltaListClass()
{
	DeltaListNodeClass *node = Head;
	while (node != 0) {
		DeltaListNodeClass *next = node->Next;
		delete node;
		node = next;
	}
}

// NOT EXACT: the client loads Count between the two loads of Tail (`*Tail` then `->Next`);
// tried: Count first, `++Count`, the new in the assignment.
void DeltaListClass::Add(DeltaClass * const &delta)
{
	DeltaListNodeClass *node = new DeltaListNodeClass(delta);
	*Tail = node;
	Tail = &(*Tail)->Next;
	Count++;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A8FD0-0x4AA8C3. The decoders (milestones.md 2.3):
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
**	The encoder's side:
**	* DeltaListSetClass 0x4A91B0 and its record-form writer 0x4A91E0 (the decoder's format).
**	* RecordTableClass (D24; the asserts name Records), size 8: the constructor 0x4A9A70 (line 512:
**	  Records allocated and zeroed, then Add for the three lists with ids 0, 1, 2), the destructor
**	  0x4A9B30, Unknown_Sum 0x4A9BA0 and Unknown_All 0x4A9BE0 (loops over the records' slots 4
**	  and 8), Add 0x4A9C30 (lines 568, 587, 597): a record per listed field (a compound's whole,
**	  in the list's state), then up the parents a compound record each with the child's bit set,
**	  until a bit is already set.
**	* The records 0x4AA0D0-0x4AA634 (vtables 0xAEB2E4, 0xAEB2C4, 0xAEB2D4) and their pools
**	  (mempool.h's AutoPoolClass: operator new 0x4AA8E0 / 0x4AAA00, Free 0x4AA9C0 / 0x4AAAE0),
**	  the tree-form writer 0x4AA320 (line 660: the assert's bitset is `b`).
**	* DeltaListClass: Add 0x4AA750, the destructor 0x4AA710 (pool 0xBE3AE8, Free 0x4AAB20).
**	Not rebuilt yet: 0x4A8990-0x4A8FC0 (the update collector: vtable 0xAEB290, its constructor
**	0x4A8AF0 and destructor 0x4A8B40, the visits 0x4A8CA0 / 0x4A8E60 with asserts on lines 103 and
**	169, which ask the policy and a per-field change table, 0x4AB660-0x4ABCD0).
*/
