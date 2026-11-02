// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\rs.cpp (its asserts from client 0x458370 on); this
**	part: client 0x457BB0-0x458570, up to the render-object state's code. Interim placement (D137):
**	the object holds the render state's constructor and destructor and its statics 0xBDC094 / 0xBDC098.
**
**	The render state (RenderStateClass, declared in rs_track.h for now): a node of its list's chains,
**	its loader thread (a critical section and an event), and the class's start-up and shut-down
**	(WinMain's step 'initializing render state class'). Notes at the end.
*/



#include <windows.h>
#include <process.h>
#include <string.h>
#include "rs_track.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\shared\unknown_debug.h"

static CRITICAL_SECTION *Unknown_BDC094;			// 0xBDC094: the render states' lock  // NAME 1 [Unknown_BDC094]: neutral (D10)
static HANDLE Unknown_BDC098;						// 0xBDC098: the loader thread's event  // NAME 1 [Unknown_BDC098]: neutral (D10)

static void Unknown_458150(void *arg);				// NAME 1 [Unknown_458150, arg]: neutral (D10)

// 0x457BE0 (deleting destructor 0x457BB0)
RenderStateClass::RenderStateClass(RenderStateListClass *list, const RenderStateClass &source) :
	Next(NULL),
	List(list),
	Unknown14(NULL),
	ID(source.ID),
	LinkID(source.LinkID),
	Link(NULL),
	Unknown24(NULL),
	Flags(source.Flags & 0x6051F)
{
	Unknown24 = nstrdup(source.Unknown24);
}

// 0x457D20
RenderStateClass::~RenderStateClass()
{
	Unknown_457E90();
	if (Next != NULL) {
		delete Next;
	}
	if (Unknown14 != NULL) {
		delete Unknown14;
	}
	if (Unknown24 != NULL) {
		delete Unknown24;
	}
}

// 0x457E10: flag 4 on, linked after the list's +0x20 node
void RenderStateClass::Unknown_457E10()
{
	Flags |= 0x10;
	List->Unknown20->Link(this);
}

// 0x457E90: flag 4 off, unlinked
void RenderStateClass::Unknown_457E90()
{
	Flags &= ~0x10;
	Unlink();
}

// 0x457EE0
void RenderStateClass::Unknown_457EE0(const char *text)
{
	if (Unknown14 != NULL) {
		delete Unknown14;
	}
	Unknown14 = new char[strlen(text) + 1];
	strcpy(Unknown14, text);
}

// 0x457F60: the chain onto the list's head
void RenderStateClass::Unknown_457F60(RenderStateListClass *list)
{
	if (Next != NULL) {
		Next->Unknown_457F60(list);
	}
	Next = list->Head;
	list->Head = this;
}

// 0x458010
void RenderStateClass::Unknown_458010()
{
	if (LinkID != -1) {
		Link = List->Unknown_45A750(LinkID);
	}
}

// 0x458040: waits while the loader thread runs
void RenderStateClass::Unknown_458040()
{
	if (Unknown_Flag12()) {
		EnterCriticalSection(Unknown_BDC094);
		if (!Unknown_Flag12()) {
			LeaveCriticalSection(Unknown_BDC094);
			return;
		}
		Flags |= 0x2000;
		ResetEvent(Unknown_BDC098);
		LeaveCriticalSection(Unknown_BDC094);
		WaitForSingleObject(Unknown_BDC098, INFINITE);
	}
}

// 0x4580D0: starts the loader thread
void RenderStateClass::Unknown_4580D0()
{
	if ((Flags & 0x1200) == 0 && Unknown_BDC094 != NULL) {
		EnterCriticalSection(Unknown_BDC094);
		ResetEvent(Unknown_BDC098);
		_beginthread(Unknown_458150, 0, this);
		LeaveCriticalSection(Unknown_BDC094);
		WaitForSingleObject(Unknown_BDC098, INFINITE);
	}
}

// 0x458150: the loader thread (it ends holding the lock, as the client's does)
static void Unknown_458150(void *arg)
{
	EnterCriticalSection(Unknown_BDC094);
	RenderStateClass *state = (RenderStateClass *)arg;	// NAME 1 [state]: its type
	state->Flags |= 0x1000;
	LeaveCriticalSection(Unknown_BDC094);
	SetEvent(Unknown_BDC098);
	state->Unknown_10();
	EnterCriticalSection(Unknown_BDC094);
	if (state->Unknown_Flag13()) {
		state->Flags &= ~0x2000;
		SetEvent(Unknown_BDC098);
	}
}

// 0x4581E0: the start-up (WinMain's 'initializing render state class')
bool Unknown_4581E0()								// NAME 1 [Unknown_4581E0]: as main_client.cpp
{
	if (Unknown_BDC094 == NULL) {
		Unknown_BDC094 = new CRITICAL_SECTION;
		InitializeCriticalSection(Unknown_BDC094);
	}
	if (Unknown_BDC098 == NULL) {
		Unknown_BDC098 = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	return(Unknown_BDC098 != NULL);
}

// 0x458250: the shut-down
void Unknown_458250()								// NAME 1 [Unknown_458250]: as main_client.cpp
{
	if (Unknown_BDC094 != NULL) {
		DeleteCriticalSection(Unknown_BDC094);
		delete Unknown_BDC094;
		Unknown_BDC094 = NULL;
	}
	if (Unknown_BDC098 != NULL) {
		CloseHandle(Unknown_BDC098);
		Unknown_BDC098 = NULL;
	}
}

// 0x4582C0
void RenderStateClass::Unknown_24(int a, int b)
{
}

// 0x4582E0
void RenderStateClass::Unknown_18(RsTrackClass *owner)
{
	Unknown_457E90();
}

// 0x458330
void RenderStateClass::Unknown_1C(RsTrackClass *owner)
{
}

// 0x458350
void RenderStateClass::Unknown_30(int a, int b)
{
}

































































// 0x458370: sets a mutable flag
void RenderStateClass::Unknown_458370(int flag)
{
	G_ASSERT(((1<<flag) & RS_MUTABLE_FLAG_MASK));
	Flags |= (1 << flag) & RS_MUTABLE_FLAG_MASK;
}


// 0x4583C0: clears it
void RenderStateClass::Unknown_4583C0(int flag)
{
	G_ASSERT(((1<<flag) & RS_MUTABLE_FLAG_MASK));
	Flags &= ~((1 << flag) & RS_MUTABLE_FLAG_MASK);
}


// 0x458410: sets a thread flag
void RenderStateClass::Unknown_458410(int flag)
{
	G_ASSERT(((1<<flag) & RS_THREAD_FLAG_MASK));
	Flags |= (1 << flag) & RS_THREAD_FLAG_MASK;
}


// 0x458460: clears it
void RenderStateClass::Unknown_458460(int flag)
{
	G_ASSERT(((1<<flag) & RS_THREAD_FLAG_MASK));
	Flags &= ~((1 << flag) & RS_THREAD_FLAG_MASK);
}

// 0x4584B0
void RenderStateClass::Unknown_4584B0(RsTrackClass *owner, int arg, RenderStateClass *next)
{
	if (Flags & 0x6C) {
		Flags |= 0x80;
	}
	if (Flags & 0x2A) {
		owner->Unknown_460710(this, 50);
	}
	if (Flags & 0x01) {
		owner->Unknown8C |= 1;
	}
	if (Flags & 0x28) {
		Flags &= ~0x20;
		if (next != NULL) {
			next->Unknown_34(arg);
			owner->Unknown_460440(next, 50);
		}
	}
}

// 0x458540: Unknown24 from text
void RenderStateClass::Unknown_458540(const char *text)
{
	if (Unknown24 != NULL) {
		delete Unknown24;
	}
	if (text != NULL) {
		Unknown24 = nstrdup(text);
	} else {
		Unknown24 = NULL;
	}
}
/*
**	Reconstruction notes
**
**	client.exe: 0x457BB0-0x458570, all exact (appfile.py match 13 of 13 markers, plus the
**	compiler's): the deleting destructor 0x457BB0, the constructor 0x457BE0 (from a list and a source
**	state: the derived states' copies call it), Node<RenderStateClass *>'s destructor 0x457CA0 and
**	deleting destructor 0x457CF0 (wwlib's listnode.h), the destructor 0x457D20, 0x457E10 / 0x457E90 /
**	0x457EE0 / 0x457F60, GenericNode's deleting destructor 0x457FA0, 0x458010, the loader thread's
**	0x458040 / 0x4580D0 / 0x458150, the start-up / shut-down 0x4581E0 / 0x458250, the out-of-line
**	virtuals +0x24, +0x18, +0x1C, +0x30 (0x4582C0-0x458350), the flag setters 0x458370-0x458460 (the
**	asserts on the client's lines 257, 265, 273, 281, D25), 0x4584B0 and 0x458540. The other virtuals
**	are inline (rs_track.h), emitted after them from 0x458590; +0x04 and +0x14 are pure: the vtable
**	0xAE7CF4 holds E&B's _purecall 0x9CCD10 there. The statics are probably the class's (free here
**	until rs.h exists). 0x458150 enters the lock again before it returns and never leaves it: the
**	client's code as compiled.
**	Not here yet: the render-object state from 0x458710 (vtable 0xAE7D58: its constructors 0x458710 /
**	0x458810 / 0x4588A0, clone 0x458970, destructor 0x458A60, 0x458AE0 and its virtuals 0x458B20 /
**	0x458B70 / 0x458BC0), derived from rs_robj.cpp's class (0x45E5F0 / 0x45E880 / 0x45E9F0, 0x45EC00).
*/
