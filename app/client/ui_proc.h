// Earth & Beyond client (reconstruction): app\client\ui_proc.h, the mesh's animation processes (the client's ui_proc.cpp
// per its asserts; this header's name is provisional, NAME 1). Laid out from what ui_mesh.cpp inlines (0x5B95E0, 0x5B9A70,
// 0x5B9BC0, 0x5B9DF0, 0x5BA0B0), the destructors it emits (0x5B97E0, 0x5B9D10) and ui_proc.cpp (UM1, UP1, 2026-11-07).
#ifndef UI_PROC_H  // NAME 1 [ui_proc.h]: provisional header name for ui_proc.cpp
#define UI_PROC_H

#include "listnode.h"
#include "nstrdup.h"

class UIMeshClass;
class UIProc5DCD20Class;
class HAnimClass;
class HAnimComboDataClass;

// One process on a mesh (table 0xAF3278, 0x20 bytes): it sits in the mesh's list through Node (+0x04, a DataNode
// pointing back at it, table 0xAF3274), on channel Slot (-1: the mesh's main list) under Id
class UIProcClass	// NAME 3: class-from-file (D24) for ui_proc.cpp
{
public:
	UIProcClass(UIMeshClass *uimesh) : UIMesh(uimesh) { Node.Set(this); Slot = -1; Id = -1; }	// NAME 3 [uimesh]: the FAIL_IFs' '!uimesh' (0x5DCD20 l.36)
	virtual ~UIProcClass() {}							// 0x5B97E0 (deleting 0x5B97B0)
	virtual bool Unknown_04() { Unknown_5DCCE0(1, -1); return(true); }	// +0x04 0x5B9790: started
	virtual bool Unknown_08(float time) { return(true); }	// +0x08 0x5B96B0: false when done (0x5B9410 passes the frame's seconds)  // NAME 1 [time]: what 0x5B9410 passes
	virtual void Unknown_0C() { Unknown_5DCCE0(2, -1); }	// +0x0C 0x5B96D0: ended
	virtual UIProc5DCD20Class *Unknown_10() { return(NULL); }	// +0x10 0x5B96F0: the animation process (0x5DCEB0) or NULL
	void Unknown_5DCCE0(int event, int arg);			// 0x5DCCE0: { this, event, arg } to the mesh's observers (0x49EAB0)  // NAME 1 [event, arg]: neutral

	DataNode<UIProcClass *> Node;						// +0x04  // NAME 1: what it is
	UIMeshClass *UIMesh;								// +0x14  // NAME 2: the FAIL_IFs' 'uimesh'
	int Slot;											// +0x18: the mesh's channel (-1: none)  // NAME 1: neutral
	int Id;												// +0x1C (0x5B9550 / 0x5B98B0 remove by it)  // NAME 1: neutral
};

// What a process sends the mesh's observers (0x5DCCE0's local)
struct UIProcEventStruct	// NAME 1: neutral (D10)
{
	UIProcClass *Proc;									// +0x00  // NAME 1: neutral
	int Event;											// +0x04: 0 frame, 1 started, 2 ended, 3 channel freed  // NAME 1: neutral
	int Arg;											// +0x08: the frame for 0, else -1  // NAME 1: neutral
};

// The animation process (0x30 bytes, table 0xAF5D94; constructors 0x5DCD20 by name and 0x5DCF00 by animation, the
// destructor 0x5DD2E0): an animation played on the mesh's combo from StartFrame to EndFrame
class UIProc5DCD20Class : public UIProcClass	// NAME 1: neutral (D10)
{
public:
	UIProc5DCD20Class(UIMeshClass *uimesh, const char *anim_name, bool cycle, bool delete_self, bool reverse, int start_frame, int end_frame);	// 0x5DCD20  // NAME 3 [uimesh, anim_name]: the FAIL_IFs'; NAME 2 [cycle, delete_self, start_frame, end_frame]: the members'; NAME 1 [reverse]: neutral
	UIProc5DCD20Class(UIMeshClass *uimesh, HAnimClass *morph_anim, bool cycle, bool delete_self, bool reverse, int start_frame, int end_frame);	// 0x5DCF00  // NAME 3 [uimesh, morph_anim]: the FAIL_IFs'; NAME 2 [cycle, delete_self, start_frame, end_frame]: the members'; NAME 1 [reverse]: neutral
	virtual ~UIProc5DCD20Class();						// 0x5DD2E0 (deleting 0x5DCED0)
	virtual bool Unknown_08(float time);				// +0x08 0x5DD3C0: the next frame; false when done  // NAME 1 [time]: neutral
	virtual void Unknown_0C();							// +0x0C 0x5DD720: off the combo, ended
	virtual UIProc5DCD20Class *Unknown_10() { return(this); }	// +0x10 0x5DCEB0
	void Unknown_5DD030(HAnimClass *anim);				// 0x5DD030: the combo data for anim, the frames clamped  // NAME 1 [anim]: neutral

	HAnimComboDataClass *AnimComboData;					// +0x20  // NAME 3: the FAIL_IFs' '!AnimComboData'
	bool Cycle;											// +0x24  // NAME 3: the FAIL_IF's 'Cycle && DeleteSelf'
	bool DeleteSelf;									// +0x25  // NAME 3: the FAIL_IF's
	bool Unknown26;										// +0x26: played backwards from EndFrame
	int StartFrame;										// +0x28  // NAME 3: the FAIL_IF's 'StartFrame > EndFrame'
	int EndFrame;										// +0x2C  // NAME 3: the FAIL_IF's
};

// table 0xAF325C (0x24 bytes, made by 0x5B95E0)
class UIProc5DD7E0Class : public UIProcClass	// NAME 1: neutral (D10)
{
public:
	UIProc5DD7E0Class(UIMeshClass *uimesh, int arg) : UIProcClass(uimesh), Unknown20(arg) {}	// NAME 1 [arg]: neutral
	virtual bool Unknown_04();							// +0x04 0x5DD7E0
	int Unknown20;										// +0x20
};

// table 0xAF3290 (0x28 bytes, made by 0x5B9A70)
class UIProc5DD770Class : public UIProcClass	// NAME 1: neutral (D10)
{
public:
	UIProc5DD770Class(UIMeshClass *uimesh, int arg, bool on) : UIProcClass(uimesh), Unknown20(arg), Unknown24(on) {}	// NAME 1 [arg, on]: neutral
	virtual bool Unknown_04();							// +0x04 0x5DD770 (0x5B9A20 on the mesh's gadgets)
	int Unknown20;										// +0x20
	bool Unknown24;										// +0x24
};

// table 0xAF32A8 (0x28 bytes, made by 0x5B9BC0)
class UIProc5DD830Class : public UIProcClass	// NAME 1: neutral (D10)
{
public:
	UIProc5DD830Class(UIMeshClass *uimesh, const char *name, int arg) : UIProcClass(uimesh), Unknown24(arg) { Name = nstrdup(name); }	// NAME 1 [name, arg]: neutral
	virtual ~UIProc5DD830Class() { delete [] Name; }	// 0x5B9D10 (deleting 0x5B9CE0)
	virtual bool Unknown_04();							// +0x04 0x5DD830
	char *Name;											// +0x20  // NAME 1: neutral
	int Unknown24;										// +0x24
};

// table 0xAF32C0 (0x28 bytes, made by 0x5B9DF0)
class UIProc5DD8A0Class : public UIProcClass	// NAME 1: neutral (D10)
{
public:
	UIProc5DD8A0Class(UIMeshClass *uimesh, const char *name, bool on) : UIProcClass(uimesh), Unknown24(on) { Name = nstrdup(name); }	// NAME 1 [name, on]: neutral
	virtual ~UIProc5DD8A0Class() { delete [] Name; }	// 0x5B9F40 (deleting 0x5B9F10)
	virtual bool Unknown_04();							// +0x04 0x5DD8A0
	char *Name;											// +0x20  // NAME 1: neutral
	bool Unknown24;										// +0x24
};

// table 0xAF32D8 (0x24 bytes, made by 0x5BA0B0)
class UIProc5DD910Class : public UIProcClass	// NAME 1: neutral (D10)
{
public:
	UIProc5DD910Class(UIMeshClass *uimesh, const char *name) : UIProcClass(uimesh) { Name = nstrdup(name); }	// NAME 1 [name]: neutral
	virtual ~UIProc5DD910Class() { delete [] Name; }	// 0x5BA200 (deleting 0x5BA1D0)
	virtual bool Unknown_04();							// +0x04 0x5DD910
	char *Name;											// +0x20  // NAME 1: neutral
};

#endif
