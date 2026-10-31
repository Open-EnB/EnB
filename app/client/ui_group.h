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
	UIGroupMemberClass(UIGroupClass *owner);			// 0x593200 (Add_Member 0x596130)  // NAME 1 [owner]: what it holds
	~UIGroupMemberClass();								// 0x5932E0
	UIGroupClass *Get_Interface() { return(Owner); }	// (the asserts' 'Get_Interface()')  // NAME 3: the asserts'
	void Unknown_5934A0();								// 0x5934A0: the four buttons and +0x34 released (G_ASSERT line 403)
	void Unknown_5935C0();								// 0x5935C0: the timers +0x24 / +0x20 run down (G_ASSERT line 456)
	bool Unknown_593750();								// 0x593750: the object's presence differs from +0x1C
	void Unknown_593790();								// 0x593790: +0x3C, +0x38, +0x2C and +0x30 released
	void Unknown_5938A0(const char *text, float time, int index);	// 0x5938A0 (G_ASSERT line 563; 0x597080 passes the symbol text, 5.0f and the member's index)  // NAME 1 [text, time, index]: what 0x597080 passes
	void Unknown_593830(const RefCountPtr<RenderObjClass> &object);	// 0x593830: +0x30 set  // NAME 1 [object]: neutral

	UIGroupClass *Owner;								// +0x00 (the asserts' 'Get_Interface()->Is_Active()': the group window's +0x7C)  // NAME 2: what it holds
	std::string Name;									// +0x04 (0x596090 compares it; the assert's 'Get_Member(name)')  // NAME 2 [Name]: the assert's name
	int ID;												// +0x14 (0x596040 / 0x5982A0 look members up by it; ClientClass::Find_Object)  // NAME 2: what it holds
	int Unknown18;										// +0x18 (the constructor's -1; Add_Member's third argument)
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
	void Unknown_592A80();								// 0x592A80 (the window's slot +0x60, every frame)
	void Unknown_592C50();								// 0x592C50 (0x598370)

	UIGroupClass *Unknown00;							// +0x00 (the constructor's argument)
	RefCountPtr<RenderObjClass> Unknown04;				// +0x04 (0x5944A0 returns it)
	RefCountPtr<RenderObjClass> Unknown08[6];			// +0x08 (the eh vector destructor iterator with 0x4740B0)
	char Unknown20[0x34 - 0x20];						// +0x20
};

// The symbol chat interface the group window holds at +0xAC (0xAC bytes, constructor 0x64A480 over the sclient; its file
// is unknown), as far as ui_group.cpp calls it
class UISymbolChatClass : public InterfaceClass	// NAME 2: the FAIL_IF's '!SymbolChatUI'
{
public:
	int Unknown_64B2F0(char *text, bool *leader);		// 0x64B2F0: the symbol picked, as text, and whether the leader sent it (0x596DB0)  // NAME 1 [text, leader]: what 0x596DB0 hands on
};

#define MAX_VISIBLE_GROUP_MEMBERS	5	// the client's FAIL_IF 'Members.size() >= MAX_VISIBLE_GROUP_MEMBERS' compares with 5 (0x596130)

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
	std::vector<UIGroupMemberClass *>::iterator Get_Member(const std::string &name);	// 0x596090 (the asserts' 'Get_Member(name) != Members.end()')  // NAME 3: the asserts'
	void Unknown_5966A0();								// 0x5966A0: 0x595530 when a member's presence changed, then every member's timers
	void Unknown_596DB0();								// 0x596DB0: a picked symbol sent as a group chat line
	void Unknown_595530();								// 0x595530
	void Unknown_596770(bool arg);						// 0x596770 (slot +0x60 passes false)  // NAME 1 [arg]: neutral
	void Unknown_597650();								// 0x597650 (slot +0x60, every frame)
	void Unknown_596E20(const char *text, bool leader);	// 0x596E20: a group chat line '<S>n owner`text' (FAIL_IFs '!avatar' 1388, '!d_name' 1391)  // NAME 1 [text, leader]: what the format takes
	void Unknown_597BF0();								// 0x597BF0: the formation buttons shown, GRP_DISBAND / GRP_LEAVE
	void Add_Member(const std::string &name, int id, int unknown);	// 0x596130 (FAIL_IFs 'Get_Member(name) != Members.end()' 1165, 'Members.size() >= MAX_VISIBLE_GROUP_MEMBERS' 1168)  // NAME 2: what it does; NAME 1 [unknown]: neutral
	void Remove_Member(const std::string &name);			// 0x596390 (Get_Member, then 0x596560)  // NAME 2: what it does
	std::vector<UIGroupMemberClass *>::iterator Remove_Member(const std::vector<UIGroupMemberClass *>::iterator &it);	// 0x596560 (FAIL_IF 'it == Members.end()' 1198)  // NAME 2: what it does; NAME 3 [it]: the FAIL_IF's
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

	RefCountPtr<UISymbolChatClass> SymbolChatUI;			// +0xAC (FAIL_IF '!SymbolChatUI'; 0x594040 makes it, 0xAC bytes, 0x64A480)  // NAME 3: the FAIL_IF's
	std::vector<UIGroupMemberClass *> Members;			// +0xB0 (constructed with a count of 0)  // NAME 3 [Members]: the asserts'
	GadgetClass *Unknown0C0[9];							// +0xC0 the formation buttons (0x594040)
	RefCountPtr<RenderObjClass> Unknown0E4;				// +0xE4 (0x597D10 hides it: slot +0x184)
	UIGroupUnknownE8Class *Unknown0E8;					// +0xE8 (0x594040 news it; the destructor deletes it)
	float Unknown0EC;									// +0xEC (0x598200: 15.0f or 0)
	bool Unknown0F0;									// +0xF0
	bool Unknown0F1;									// +0xF1 (0x5981B0 / 0x598200 / 0x5982A0 / 0x5985A0 do nothing while it is set)
};

#endif // UI_GROUP_H
