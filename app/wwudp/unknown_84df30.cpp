// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	CordCon's channel (CordChannelClass) and its pool: the client's object before cordcon.obj
**	(client 0x84DF30-0x84F640). The file's name is not in the client (no assert, no string): provisional
**	(interim, D137). Notes at the end.
*/

#include "cordcon.h"
#include <time.h>
// The channels come from a pool of 1024 (0xBF40D8; ObjectPoolClass::Allocate_Object_Memory 0x8563E0
// with blocks of 4 + 1024 x 0x78 bytes).
DEFINE_AUTO_POOL(CordChannelClass, 1024)

// Reset's body, inlined into the constructor and the destructor as the client does (Reset itself is kept
// out of line below, where the CordCon constructor and openc call it).
static inline void Reset_Channel(CordChannelClass *c)	// NAME 2: Reset's body
{
	c->Mode = 0;
	c->IsClosed = true;
	c->LastWrite = (unsigned int)time(0);
	c->Priority = 0;
	c->Written = 0;
	c->Delivered = 0;
	c->Expected = 0;
	c->NextSequenced = 0;
	c->LastSeq = 0;
	c->Queue.clear();
	c->Incoming.Clear();
	c->Sent.clear();
	c->ToAck.clear();
	c->Missing.clear();
}

CordChannelClass::CordChannelClass()	// 0x84DFC0
{
	NextSeq = 0;
	Index = -1;
	Reset_Channel(this);
}

// 0x84E170: the members' constructors, then the copy.
CordChannelClass::CordChannelClass(const CordChannelClass &other)
{
	Copy(other);
}

// 0x84E230
CordChannelClass &CordChannelClass::operator=(const CordChannelClass &other)
{
	Copy(other);
	return *this;
}

// 0x84E260, NOT EXACT (functional, 0.88: inline budget): member by member in the client's order (the
// arrays through wlib's operator=, Incoming through Clear / getPointer / Add, the vectors by clear /
// push_back, their erase / insert out of line as the client); the client calls Sent's
// size() out of line, ours inlines it.
void CordChannelClass::Copy(const CordChannelClass &other)
{
	if (this == &other) {
		return;
	}
	CordChannelClass &from = (CordChannelClass &)other;
	Mode = from.Mode;
	LastWrite = from.LastWrite;
	NextSeq = from.NextSeq;
	Priority = from.Priority;
	Written = from.Written;
	Delivered = from.Delivered;
	Expected = from.Expected;
	NextSequenced = from.NextSequenced;
	LastSeq = from.LastSeq;
	ToAck = from.ToAck;
	Missing = from.Missing;
	IsClosed = from.IsClosed;
	CordMessageClass *p; int j; Incoming.Clear();
	for (j = 0; j < from.Incoming.Entries; j++) { from.Incoming.getPointer(&p, j); Incoming.Add_Tail(*p); }
	unsigned int i;
	Queue.clear();
	for (i = 0; i < from.Queue.size(); i++) {
		Queue.push_back(from.Queue[i]);
	}
	Sent.clear();
	for (i = 0; i < from.Sent.size(); i++) {
		Sent.push_back(from.Sent[i]);
	}
	Index = from.Index;
}

// 0x84E570: Reset only, as the client (the records' buffers are not freed: CordMessageClass's
// destructor leaves them; Free 0x85BEC0 is called where a record is dropped). Out of line, as
// ~CordCon 0x84F8C0 calls it.
#pragma auto_inline(off)
CordChannelClass::~CordChannelClass()
{
	Reset_Channel(this);
}
#pragma auto_inline(on)

// 0x84E770: out of line in CordCon's constructor and openc, as the client calls it (CordChannelClass's
// constructor and destructor inline Reset_Channel).
#pragma auto_inline(off)
void CordChannelClass::Reset()
{
	Mode = 0;
	IsClosed = true;
	LastWrite = (unsigned int)time(0);
	Priority = 0;
	Written = 0;
	Delivered = 0;
	Expected = 0;
	NextSequenced = 0;
	LastSeq = 0;
	Queue.clear();
	Incoming.Clear();
	Sent.clear();
	ToAck.clear();
	Missing.clear();
}
#pragma auto_inline(on)

/*
** Notes.
**
** Why a file of its own (the client's evidence, image builder IM9, binary.md §5.7):
**  - the client's CRT table (.CRT$XCU) lists the pool's initialiser 0x84DF30 and <string>'s guard 0x856870
**    with no zero slot after them; the incremental link pads each object's contribution by S / 5 bytes
**    (rounded up to 4), so an 8-byte contribution (one object with both) gets one zero (nav_list_builder.obj's
**    allocator and guard, the same code forms, keep theirs at 0xB6CD10). Two 4-byte contributions get none:
**    two objects, the pool's and cordcon.cpp's (which ends with the guard);
**  - the template instantiations this code needs (vector<CordMessageClass> 0x84E8A0-0x84EDB0, CordListClass's
**    destructor 0x84EDF0, ArrayList<unsigned>, std::copy / fill / copy_backward, CordMessageClass's
**    operator= 0x84F150, CordListClass::Add 0x84F220, ArrayList::get / add 0x84F440 / 0x84F480) sit between
**    Reset 0x84E770 and CordCon's constructor 0x84F640: VC6 emits a translation unit's instantiations after
**    its own functions (one cordcon.obj put them after CordCon's code, 0x855D20 in the all trial), so the
**    client's object ends at 0x84F640;
**  - the name sorts between bandwidthaccounting and cordcon in the client's folder order (binary.md §5.6).
*/
