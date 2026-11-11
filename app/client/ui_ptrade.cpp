// Earth & Beyond client (reconstruction): app\client\ui_ptrade.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_ptrade.cpp', 0x5E1B80-0x5E3EEF by its asserts and its callers).
// Every function 0x5E1BE0-0x5E40A1 is rebuilt, the credits button 0x5E3EF0 / 0x5E3F40 and its destructors 0x5E4010 / 0x5E4040 included (PT1, PT2).
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include "sclient.h"
#include "ui_manu.h"
#include "..\shared\money.h"
#include "client.h"
#include "co.h"
#include "gadget.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\packets\pkaction.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\lookup.h"
#include "..\shared\auxdata\delta_money.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time (0xBFAE3C - 0xBFAE40, read inline)
#include "gd_uiback.h"		// UIBackGadget (0x674260)

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// money.cpp's arithmetic and comparisons (cdecl, not rebuilt)
MoneyClass operator+(const MoneyClass &a, const MoneyClass &b);	// 0x4F0500  // NAME 1 [a, b]: neutral
bool operator>=(const MoneyClass &a, const MoneyClass &b);	// 0x4F0740  // NAME 1 [a, b]: neutral
bool operator>(const MoneyClass &a, const MoneyClass &b);	// 0x4F0700  // NAME 1 [a, b]: neutral

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// What SClient 0x74C7B0 returns, as far as 0x5E3C90 reads it (the avatar's description at +0xF8, as ui_chat_panel.cpp's)
class UIPTradeAvatarClass : public RefCountClass { public: char Unknown[0xF8 - sizeof(RefCountClass)]; AvatarDescClass Desc; };	// NAME 1: neutral (D10)

#include "avatar_pip.h"	// AvatarPipClass: PlayerPIP / OtherPIP (0x3C bytes; 0x7E31B0, 0x7E3410, 0x7E36B0, 0x7E3A20, 0x7E4350)









// 0x5E1BE0
UIPTradeClass::UIPTradeClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TRADE00A", 1), client),
	PlayerPIP(NULL),
	OtherPIP(NULL),
	State(UI_CLOSED),
	Unknown0D8(0),
	Unknown0DC(false),
	Unknown0DD(false),
	Unknown0DE(false)
{
	memset(&Unknown0B8, 0, sizeof(Unknown0B8) + sizeof(Unknown0BC));
	Unknown_BE67BC->Load("fa_trade00a.ini");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}









// 0x5E1D80
UIPTradeClass::~UIPTradeClass()
{
	G_ASSERT(State == UI_CLOSED);
	Unknown0AC = NULL;
	Unknown0B0 = NULL;
	Unknown0B4 = NULL;
	delete PlayerPIP;
	delete OtherPIP;
	PlayerPIP = NULL;
	OtherPIP = NULL;
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("fa_trade00a.ini");
}

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_manu.cpp's

// The four buttons' sub-objects and their styles (0xB850B0 / 0xB850C4)
static const char *Unknown_B850B0[] = { "exit", "cancel", "confirm", "money", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B850C4[] = { NULL, "CANCEL_TRADE00", "CONFIRM_TRADE00", "MONEY_TRADE00", NULL };	// NAME 1: neutral (D10)

// The credits button (0x64 bytes, table 0xAF6578, over 0x674260's gadget): pressed, it counts only while the credits entry is closed
class UIPTradeCreditsGadgetClass : public UIBackGadget	// NAME 1: neutral (D10)
{
public:
	UIPTradeCreditsGadgetClass(const RefCountPtr<RenderObjClass> &model, RefCountPtr<UIPTradeCreditsClass> credits);	// 0x5E3F40  // NAME 1 [model, credits]: what the set-up passes
	virtual bool Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x34 0x5E3EF0 (the base's, false while the credits entry is Active)  // NAME 1 [event, x, y, mask]: the base's

	RefCountPtr<UIPTradeCreditsClass> Unknown60;		// +0x60
};





// 0x5E1FA0
void UIPTradeClass::Unknown_54()
{
	for (int id = 1; id < 5; id++) {
		RefCountPtr<RenderObjClass> robj = Create_NEW(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B850B0[id - 1]));
		Unknown_496AE0(robj.Peek(), 0);
		Unknown0BC[id - 1] = new ButtonGadgetClass(robj.Peek(), Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown0BC[id - 1], id, View, 1, 0);
		Unknown_5BB4F0(Unknown0BC[id - 1]);
		if (Unknown_B850C4[id - 1]) {
			RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B850C4[id - 1], InterfaceMesh->Peek_Mesh_Object()));
			font->Set_Sort_Level(robj->Get_Sort_Level() - 1);
			((ButtonGadgetClass *)Unknown0BC[id - 1])->Unknown_98(font.Peek());
		}
	}
	GadgetClass::Set_Tooltip(Unknown0BC[1], "UI_PTRADE", "CANCEL");
	GadgetClass::Set_Tooltip(Unknown0BC[0], "UI_PTRADE", "CANCEL");
	GadgetClass::Set_Tooltip(Unknown0BC[2], "UI_PTRADE", "CONFIRM");
	GadgetClass::Set_Tooltip(Unknown0BC[3], "UI_PTRADE", "CREDITS");
	SClient *sclient = Client->As_Space_Client();
	Unknown0AC = Create_Get(new UIInvRobjStripClass(sclient, InterfaceMesh->Peek_Object(), 6, "TRADE00A.C%02d", 15));
	Unknown0AC->Unknown_5417D0(this);
	Unknown0AC->Unknown_54();
	Unknown0B0 = Create_Get(new UIInvRobjStripClass(sclient, InterfaceMesh->Peek_Object(), 6, "TRADE00A.X%02d", 16));
	Unknown0B0->Unknown_5417D0(this);
	Unknown0B0->Unknown_54();
	Unknown0B4 = Create_Get(new UIPTradeCreditsClass(sclient, 0, 9));
	Unknown0B4->Unknown_54();
	Unknown0B4->Unknown_64A270("Enter Amount of Credits");
	GadgetClass *credits = new UIPTradeCreditsGadgetClass(Create_Peek(InterfaceMesh->Unknown_5E26D0()), Unknown0B4);
	Unknown64.Unknown_6870E0(credits, 6, View, 1, 0);
	FAIL_IF(PlayerPIP) return;
	FAIL_IF(OtherPIP) return;
	PlayerPIP = new AvatarPipClass;
	OtherPIP = new AvatarPipClass;
	Unknown_10();
}

// 0x5E26D0
RenderObjClass *UIMeshClass::Unknown_5E26D0()
{
	return(InterfaceMesh);
}


// 0x5E2F70
void UIPTradeClass::Unknown_5E2F70(bool on)
{
	Unknown0DD = on;
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("REC_CONFIRM_TRADE00", InterfaceMesh->Peek_Mesh_Object()));
	if (font) {
		if (Unknown0DD) font->Change_Text("Awaiting Trade Confirmation", 0);
		else font->Change_Text("", 0);
	}
}

// 0x5E26F0
void UIPTradeClass::Unknown_58()
{
	if (State == UI_CLOSED) {
		InterfaceSortScopeClass sort(this);
		State = UI_OPEN;
		SClient *sclient = Client->As_Space_Client();
		Unknown0DC = sclient->PDAInterface->Unknown_695780(false);
		PlayerPIP->Unknown_7E36B0(sclient, Create_NEW(InterfaceMesh->Unknown_5B8B40("AVATAR_YOU")), 0);
		OtherPIP->Unknown_7E36B0(sclient, Create_NEW(InterfaceMesh->Unknown_5B8B40("AVATAR_THEM")), 0);
		Unknown5BB320Class::Unknown_58();
		Unknown_5E3340();
		RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("PLAYER_NAME_TRADE00", InterfaceMesh->Peek_Object()));
		int id = ((ClientClass *)Client)->AvatarID; RefCountPtr<UIPTradeAvatarClass> avatar = Create_NEW((UIPTradeAvatarClass *)sclient->Unknown_74C7B0(id));
		if (avatar && font) {
			std::string name;
			avatar->Desc.Get_Full_Name(name);
			font->Change_Text(name.c_str(), 0);
		} else {
			G_ASSERT(0);
		}
		font = Create_Peek(Unknown_BE67C4->Get_Style("RECIPIENT_NAME_TRADE00", InterfaceMesh->Peek_Object()));
		if (font) font->Change_Text("", 0);
		Unknown_5E3750(true);
		Unknown_5E2F70(false);
		Unknown0E0 = MoneyClass();
		Unknown0D8 = 0; font.Peek();	// a dead read (D130): with it ~RefCountPtr<FontSys> stays out of line in the inlined 0x5E2F70, as the client has it
	}
}

// 0x5E2B30
void UIPTradeClass::Unknown_5C()
{
	if (State == UI_OPEN) {
		State = UI_CLOSED;
		Unknown0AC->Unknown_5C();
		Unknown0B0->Unknown_5C();
		Unknown0B4->Unknown_5C();
		SClient *sclient = Client->As_Space_Client();
		if (Unknown0DC) {
			sclient->PDAInterface->Unknown_695A00(0, 1);
		}
		Unknown_5E2F70(false);
		Unknown_5E3050(false);
		Unknown5BB320Class::Unknown_5C();
		PlayerPIP->Unknown_7E4350();
		OtherPIP->Unknown_7E4350();
	}
}

// 0x5E2BF0
void UIPTradeClass::Unknown_60()
{
	Unknown_5E3C90();
	Unknown_5E39F0();
	Unknown_5E37F0();
	PlayerPIP->Unknown_7E3A20(0, ((ClientClass *)Client)->AvatarID, 0);
	OtherPIP->Unknown_7E3A20(0, ((ClientClass *)Client)->Unknown1134, 0);
	Unknown5BB320Class::Unknown_60();
	if (Unknown0D8 > 0.0f) {
		Unknown0D8 -= WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown0D8 < 0.0f) {
			Unknown0D8 = 0.0f;
			Unknown_5C();
		}
	}
}

// 0x5E2CE0
// NOT EXACT: register-only (0.84, same length: key 3's request reads the client through eax, the client's; ours through edx as keys 1 / 2)
bool UIPTradeClass::Unknown_68(int key)
{
	SClient *sclient = Client->As_Space_Client();
	switch (key) {
		case 1:
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			sclient->Send_Packet(&ActionPacketClass(sclient->AvatarID, 0x16, ((ClientClass *)Client)->Unknown1134, 0));
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			sclient->Send_Packet(&ActionPacketClass(sclient->AvatarID, 0x15, ((ClientClass *)Client)->Unknown1134, 0));
			return(true);
		case 4:
		{
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0B4->Unknown_58();
			char text[0x14];
			if (Unknown0E0 > MoneyClass()) {
				_snprintf(text, sizeof(text), "%d", Unknown0E0.Unknown00);
				Unknown0B4->Unknown_64A2B0(text);
			} else {
				Unknown0B4->Unknown_64A2B0("");
			}
			Unknown_5E3750(false);
			return(true);
		}
	}
	return(false);
}

// 0x5E2F00
void UIPTradeClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (Unknown0B4) Unknown0B4->Unknown_7C(view, false);
	if (Unknown0AC) Unknown0AC->Unknown_7C(view, arg);
	if (Unknown0B0) Unknown0B0->Unknown_7C(view, arg);
	Unknown5BB320Class::Unknown_7C(view, arg);
}

// 0x5E3050
void UIPTradeClass::Unknown_5E3050(bool on)
{
	Unknown0DE = on;
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("REC_CONFIRM_TRADE00", InterfaceMesh->Peek_Mesh_Object()));
	if (font) {
		if (Unknown0DE) font->Change_Text("Trade has been confirmed", 0);
		else font->Change_Text("", 0);
	}
}

// 0x5E3130
void UIPTradeClass::Unknown_5E3130()
{
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("REC_CONFIRM_TRADE00", InterfaceMesh->Peek_Mesh_Object()));
	if (font) font->Change_Text("Trading has successfully completed.", 0);
	Unknown0D8 = 2.0f;
	Unknown_5E3750(false);
}

























































































// 0x5E3210
void UIPTradeClass::Unknown_5E3210(const char *name, bool visible, const char *text)
{
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	FAIL_IF(!font) return;
	font->Change_Text(text, 0);
	font->Set_Hidden(!visible);
}

// The money field's lookups through two inline layers: with them the client's find 0x5466A0 stays out of line (as ui_avatar_cust.cpp's)
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)
static inline DeltaTemplateClass<MoneyClass> *Unknown_Find_Money(DeltaCompoundClass *aux, const char *field) { return(Unknown_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, field)); }	// NAME 1: neutral (D10)

// 0x5E3340
void UIPTradeClass::Unknown_5E3340()
{
	MoneyClass money;
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<MoneyClass> *t = Unknown_Find_Money(aux, "Money");
	if (t) money = t->Value;
	Unknown_5E3210("CREDITS_TRADE00", true, money.Unknown_4F00C0(false).c_str());
	Unknown_5E3210("CRED1_TRADE00", true, Unknown0E0.Unknown_4F00C0(false).c_str());
	if (((ClientClass *)Client)->Find_Object(((ClientClass *)Client)->Unknown1134)) {
		money = MoneyClass();
		DeltaCompoundClass *other = (DeltaCompoundClass *)((ClientClass *)Client)->Find_Object(((ClientClass *)Client)->Unknown1134)->AuxData;
		t = Unknown_Find_Money(other, "TradeMoney");
		if (t) money = t->Value;
		Unknown_5E3210("CRED2_TRADE00", true, money.Unknown_4F00C0(false).c_str());
	}
}


// 0x5E3750
void UIPTradeClass::Unknown_5E3750(bool on)
{
	for (int i = 0; i < 4; i++) {
		Unknown0BC[i]->Unknown_40(on);
	}
	if (on) {
		if (!Unknown0AC->Active) Unknown0AC->Unknown_58();
		if (!Unknown0B0->Active) Unknown0B0->Unknown_58();
	} else {
		if (Unknown0AC->Active) Unknown0AC->Unknown_5C();
		if (Unknown0B0->Active) Unknown0B0->Unknown_5C();
	}
}

// Whether the object's field changed (above 1: LookupClass's test, or the money field's own)
static inline bool Unknown_Changed(ClientObjectClass *object, const char *field) { NameClass name(field); return(((DeltaCompoundClass *)object->AuxData)->Get_Lookup()->Unknown_Test_70_Above_1(name)); }	// NAME 1: neutral (D10)
static inline bool Unknown_Changed_Money(ClientObjectClass *object, const char *field) { DeltaCompoundClass *aux = (DeltaCompoundClass *)object->AuxData; NameClass name(field); DeltaTemplateClass<MoneyClass> *t = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); return(t && t->Unknown70 > 1); }	// NAME 1: neutral (D10)

// 0x5E37F0
void UIPTradeClass::Unknown_5E37F0()
{
	ClientClass *sclient = Client->As_Space_Client();
	ClientObjectClass *avatar = sclient->Get_Avatar();
	if (avatar && avatar->AuxData) {
		if (Unknown_Changed(avatar, "Inventory.PlayerTrade") || Unknown_Changed(avatar, "TradeMoney")) {
			int id = ((ClientClass *)Client)->Unknown1134; sclient->Send_Packet(&ActionPacketClass(sclient->AvatarID, 0x17, id, 0));
			Unknown_5E3340();
		} else if (((ClientClass *)Client)->Find_Object(((ClientClass *)Client)->Unknown1134) && Unknown_Changed_Money(((ClientClass *)Client)->Find_Object(((ClientClass *)Client)->Unknown1134), "TradeMoney")) {
			Unknown_5E3340();
		}
	}
}


// 0x5E39F0
void UIPTradeClass::Unknown_5E39F0()
{
	if (Unknown0B4 && Unknown0B4->Active) {
		int result = Unknown0B4->Unknown_64A220();
		if (result != 0) {
			if (result == 2) {
				int amount = atoi(Unknown0B4->Unknown_64A240());
				MoneyClass money;
				DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
				DeltaTemplateClass<MoneyClass> *t = Unknown_Find_Money(aux, "Money");
				if (t) money = t->Value;
				if (money + Unknown0E0 >= MoneyClass(amount)) {
					Unknown0E0.Unknown00 = amount;
					SClient *sclient = Client->As_Space_Client();
					__int64 offered = Unknown0E0.Unknown00; int id = ((ClientClass *)Client)->Unknown1134; int avatar = sclient->AvatarID; sclient->Send_Packet(&ActionPacketClass(avatar, 0x18, id, (int)offered));
					Unknown_5E3340();
				} else {
					std::string text("Insufficient credits.");
					Client->As_Space_Client()->Unknown_74D990(0x11, (char *)text.c_str(), 0);
				}
			}
			Unknown0B4->Unknown_5C();
			Unknown_5E3750(true);
		}
	}
}


// 0x5E3C90
void UIPTradeClass::Unknown_5E3C90()
{
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("RECIPIENT_NAME_TRADE00", InterfaceMesh->Peek_Object()));
	const char *text = font->Unknown_6D87D0();
	if (text && *text && *text != ' ') return;
	SClient *sclient = Client->As_Space_Client();
	int id = ((ClientClass *)Client)->Unknown1134; RefCountPtr<UIPTradeAvatarClass> avatar = Create_NEW((UIPTradeAvatarClass *)sclient->Unknown_74C7B0(id));
	if (!avatar) return;
	std::string name;
	avatar->Desc.Get_Full_Name(name);
	font->Change_Text(name.c_str(), 0);
}


// 0x5E3EF0
bool UIPTradeCreditsGadgetClass::Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (GadgetClass::Unknown_34(event, x, y, mask)) {
		return(Unknown60->Active ? false : true);
	}
	return(false);
}

// 0x5E3F40
UIPTradeCreditsGadgetClass::UIPTradeCreditsGadgetClass(const RefCountPtr<RenderObjClass> &model, RefCountPtr<UIPTradeCreditsClass> credits) :
	UIBackGadget(model)
{
	Unknown60 = credits;
}
