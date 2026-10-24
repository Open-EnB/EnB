// Earth & Beyond client (reconstruction): app\shared\auxdata\encode.cpp. Notes at the end of the file.
#include <string.h>
#include "delta.h"
#include "..\unknown_debug.h"

class LinearBufferClass;

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

// (the lines down to the constructor hold code of the client's file not rebuilt yet: its assert
// is on line 512)

























































































































































































































































































































































































































































































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
**	client.exe 0x4A9A70-0x4A9C1A: RecordTableClass (D24; the asserts name Records), size 8. Rebuilt:
**	the constructor 0x4A9A70 (line 512: Records allocated and zeroed, then Add for the three lists
**	with ids 0, 1, 2), the destructor 0x4A9B30, Unknown_Sum 0x4A9BA0 and Unknown_All 0x4A9BE0 (loops
**	over the records' slots 4 and 8). Not rebuilt yet: the rest of the file (0x4A8990-0x4A9A50: the
**	tree-form and record-form codecs (the reference decoder's notes: 0x4A9400, 0x4A9710, 0x4A8FD0), Add
**	0x4A9C30 (lines 568-597: one record per field id; leaf records 0xC bytes with their own
**	operator new 0x4AA8E0, compound ones with a bitset, 0x4AAA00), the records' methods
**	0x4AA0D0-0x4AA320 (line 660), 0x4AA710-0x4AA8B0).
*/
