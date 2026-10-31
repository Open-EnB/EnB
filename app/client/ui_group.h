// Earth & Beyond client (reconstruction): app\client\ui_group.h, the group window (the client's ui_group.cpp per its
// asserts; this header's name is provisional, NAME 1). Moved out of cpacket.cpp (HN2, 2026-10-28); laid out from the
// constructor 0x593BD0 and the destructor 0x593D70 (UG1, 2026-10-30).
#ifndef UI_GROUP_H  // NAME 1 [ui_group.h]: provisional header name for ui_group.cpp
#define UI_GROUP_H

#include <vector>
#include "sclient.h"

class GadgetClass;			// gadget.cpp's (the formation buttons at +0xC0, deleted through 0x65EED0)
class UIGroupClass;

// The interface base of the group window (0xAC bytes; constructor 0x5BB320, destructor 0x5BB3B0, table 0xAF3390; its file
// is unknown, its functions follow ui_mesh.cpp's): intrface.cpp's class over a UIMeshClass and a layer
// (interim placement in ui_group.h, its only rebuilt user: D137)
class Unknown5BB320Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	Unknown5BB320Class(UIMeshClass *mesh, SClient *client);	// 0x5BB320 (the mesh at +0x84, the client to 0x540D30)  // NAME 1 [mesh, client]: what 0x593BD0 passes
	~Unknown5BB320Class();								// 0x5BB3B0 (deletes the mesh)
	virtual void Unknown_58();							// 0x5BB550
	virtual void Unknown_5C();							// 0x5BB5E0 (the group window's destructor calls it first)
	virtual void Unknown_60();							// 0x5BB660
	virtual void Unknown_7C(UIViewClass *view, int arg);	// 0x5BB470
	virtual void Unknown_88();							// +0x88 0x543750
	virtual void Unknown_8C();							// +0x8C 0x543770
	void Unknown_5BB4F0(GadgetClass *gadget);			// 0x5BB4F0  // NAME 1 [gadget]: what 0x594040 passes

	UIMeshClass *Mesh;									// +0x84 (the destructor deletes it)  // NAME 2: what it holds
	LayerClass Layer;									// +0x88 (LayerClass::LayerClass in the constructor; the gadgets are made over it)  // NAME 2: what it is
};

// What a member's +0x2C points to (a virtual destructor; 0x5DCAE0 before the delete)
class UIGroupMemberUnknown2CClass	// NAME 1: neutral (D10)
{
public:
	virtual ~UIGroupMemberUnknown2CClass();
	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10();							// +0x10 (0x5935C0, every frame)
	void Unknown_5DCAE0();								// 0x5DCAE0 (before the delete)
};

// One member of the group (0x54 bytes, in Members; destructor 0x5932E0): the asserts' Get_Interface() is its +0x00
class UIGroupMemberClass	// NAME 2: what UIGroupClass's Members holds
{
public:
	~UIGroupMemberClass();								// 0x5932E0
	UIGroupClass *Get_Interface() { return(Owner); }	// (the asserts' 'Get_Interface()')  // NAME 3: the asserts'
	void Unknown_5934A0();								// 0x5934A0: the four buttons and +0x34 released (G_ASSERT line 403)
	void Unknown_5935C0();								// 0x5935C0: the timers +0x24 / +0x20 run down (G_ASSERT line 456)
	bool Unknown_593750();								// 0x593750: the object's presence differs from +0x1C
	void Unknown_593790();								// 0x593790: +0x3C, +0x38, +0x2C and +0x30 released
	void Unknown_593830(const RefCountPtr<RenderObjClass> &object);	// 0x593830: +0x30 set  // NAME 1 [object]: neutral

	UIGroupClass *Owner;								// +0x00 (the asserts' 'Get_Interface()->Is_Active()': the group window's +0x7C)  // NAME 2: what it holds
	std::string Name;									// +0x04 (0x596090 compares it; the assert's 'Get_Member(name)')  // NAME 2 [Name]: the assert's name
	int ID;												// +0x14 (0x596040 / 0x5982A0 look members up by it; ClientClass::Find_Object)  // NAME 2: what it holds
	char Unknown18[0x1C - 0x18];						// +0x18
	bool Unknown1C;										// +0x1C (0x593750)
	bool Unknown1D;										// +0x1D (0x5935C0 clears it with +0x24)
	float Unknown20;									// +0x20 (0x5982A0: 15.0f; 0x5935C0 runs it down, then clears +0x48's flag 0x800)
	float Unknown24;									// +0x24 (0x5935C0 runs it down, then releases +0x3C / +0x38 / +0x28)
	char *Unknown28;									// +0x28 (deleted, no destructor)
	UIGroupMemberUnknown2CClass *Unknown2C;				// +0x2C
	RefCountPtr<RenderObjClass> Unknown30;				// +0x30 (0x593830 sets it)
	RefCountPtr<RenderObjClass> Unknown34;				// +0x34 (0x5934A0: slot +0x38, released)
	RefCountPtr<RenderObjClass> Unknown38;				// +0x38 (slot +0x38, released)
	GadgetClass *Unknown3C;								// +0x3C
	GadgetClass *Unknown40;								// +0x40 (0x597000: the four buttons' slot +0x40)
	GadgetClass *Unknown44;								// +0x44
	GadgetClass *Unknown48;								// +0x48 (0x5982A0: flag 0x800, 0x6603A0)
	GadgetClass *Unknown4C;								// +0x4C
	char Unknown50[0x54 - 0x50];						// +0x50
};

// What +0xE8 points to (0x34 bytes; constructor 0x592910 over the window; its inline destructor calls 0x592BB0)
class UIGroupUnknownE8Class	// NAME 1: neutral (D10)
{
public:
	~UIGroupUnknownE8Class() { Unknown_592BB0(); }		// inline (the group window's destructor expands it)
	void Unknown_592BB0();								// 0x592BB0
	RefCountPtr<RenderObjClass> Unknown_5944A0();		// 0x5944A0 (0x594040, 0x598370): Unknown04

	UIGroupClass *Unknown00;							// +0x00 (the constructor's argument)
	RefCountPtr<RenderObjClass> Unknown04;				// +0x04 (0x5944A0 returns it)
	RefCountPtr<RenderObjClass> Unknown08[6];			// +0x08 (the eh vector destructor iterator with 0x4740B0)
	char Unknown20[0x34 - 0x20];						// +0x20
};

// ui_group.cpp's group window (the main view's +0x1D8; 0xF4 bytes, table 0xAF1C34)
class UIGroupClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_group.cpp
{
public:
	UIGroupClass(SClient *client);						// 0x593BD0 (0xF4 bytes; the main view's 0x50C690)  // NAME 1 [client]: what 0x50C690 passes
	virtual ~UIGroupClass();							// 0x593D70 (slot 0x593D40)
	virtual void Unknown_54();							// 0x594040 (0x50C690 calls it after construction)
	virtual void Unknown_60();							// 0x5944D0
	virtual bool Unknown_68(int key);					// 0x5946D0
	virtual void Unknown_70(ControlKeysManagerClass *manager);	// 0x5985A0 (FAIL_IF '!manager')  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_74(ControlKeysManagerClass *manager);	// 0x598720 (FAIL_IF '!manager')  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_7C(UIViewClass *view, int arg);	// 0x595480 (0x50C690: the view, 0)  // NAME 1 [view, arg]: what 0x50C690 passes
	void Unknown_5954C0();								// 0x5954C0 (the destructor expands it: the NAMEnn_TSI03A styles hidden)
	int Unknown_596040(int id);					// 0x596040: the index of the member with that ID, else -1  // NAME 1 [id]: what it compares
	void Unknown_597000(bool on);						// 0x597000: each member's four gadgets' slot +0x40  // NAME 1 [on]: neutral
	void Unknown_5975D0();								// 0x5975D0: the leave / disband question ('GROUP_LEAVE')
	bool Unknown_5979A0();								// 0x5979A0: GroupInfo.FormationName is set
	bool Unknown_597A60();								// 0x597A60: GroupInfo.Formation is not 0
	void Unknown_597080(const char *text, const char *name, int index);	// 0x597080 (CLIENT_CHAT_EVENT 165: a 'group' message '<S>n name`text')  // NAME 1 [text, name, index]: the message's parts
	void Unknown_597820(unsigned id);					// 0x597820 (a ClientGroupEvent of true)  // NAME 1 [id]: what Find_Object gets
	void Unknown_5978A0(unsigned id);					// 0x5978A0 (the same)  // NAME 1 [id]: what Find_Object gets
	void Unknown_597920(unsigned id, bool value);		// 0x597920: a ClientGroupEvent for the object (FAIL_IF '!client', line 1594)  // NAME 1 [id, value]: neutral
	bool Unknown_597B00();								// 0x597B00: GroupInfo's +0x120 (FAIL_IFs '!client', '!sclient')
	void Unknown_597B70(bool value);					// 0x597B70: SClient +0x13C0  // NAME 1 [value]: neutral
	void Unknown_597D10();								// 0x597D10 (FAIL_IF '!SymbolChatUI' on +0xAC)
	void Unknown_597E40(int arg);						// 0x597E40  // NAME 1 [arg]: what the callers pass (0)
	void Unknown_598370();								// 0x598370
	void Unknown_5981B0();								// 0x5981B0 (cpacket.cpp: CTA_RESPONSE 189's request type 14, when the player doesn't lead the group)
	void Unknown_598200(bool on);						// 0x598200 (CTA_RESPONSE 189's type 15 passes true)  // NAME 1 [on]: what the handler passes
	void Unknown_5982A0(int avatar_id);					// 0x5982A0 (CTA_RESPONSE 189's type 17)  // NAME 1 [avatar_id]: the packet's AvatarID

	RefCountPtr<InterfaceClass> SymbolChatUI;			// +0xAC (FAIL_IF '!SymbolChatUI'; 0x594040 makes it, 0xAC bytes, 0x64A480)  // NAME 3: the FAIL_IF's
	std::vector<UIGroupMemberClass *> Members;			// +0xB0 (constructed with a count of 0)  // NAME 3 [Members]: the asserts'
	GadgetClass *Unknown0C0[9];							// +0xC0 the formation buttons (0x594040)
	RefCountPtr<RenderObjClass> Unknown0E4;				// +0xE4 (0x597D10 hides it: slot +0x184)
	UIGroupUnknownE8Class *Unknown0E8;					// +0xE8 (0x594040 news it; the destructor deletes it)
	float Unknown0EC;									// +0xEC (0x598200: 15.0f or 0)
	bool Unknown0F0;									// +0xF0
	bool Unknown0F1;									// +0xF1 (0x5981B0 / 0x598200 / 0x5982A0 / 0x5985A0 do nothing while it is set)
};

#endif // UI_GROUP_H
