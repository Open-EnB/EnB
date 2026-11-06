// Earth & Beyond client (reconstruction): app\client\ui_ptrade.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_ptrade.cpp', 0x5E1B80-0x5E3EEF by its asserts and its callers).
// So far the destructor 0x5E1D80, the slots +0x5C / +0x60 / +0x7C (0x5E2B30, 0x5E2BF0, 0x5E2F00) and 0x5E3750-0x5E3EEF (PT1); not yet: the rest of 0x5E1B80-0x5E374F.
#include <stdlib.h>
#include <string>
#include "sclient.h"
#include "ui_manu.h"
#include "..\shared\money.h"
#include "client.h"
#include "co.h"
#include "gadget.h"
#include "fontmanager.h"
#include "..\packets\pkaction.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\lookup.h"
#include "..\shared\auxdata\delta_money.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time (0xBFAE3C - 0xBFAE40, read inline)

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

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// What SClient 0x74C7B0 returns, as far as 0x5E3C90 reads it (the avatar's description at +0xF8, as ui_chat_panel.cpp's)
class UIPTradeAvatarClass : public RefCountClass { public: char Unknown[0xF8 - sizeof(RefCountClass)]; AvatarDescClass Desc; };	// NAME 1: neutral (D10)

// avatar_pip.cpp's pip (0x3C bytes; +0xCC, +0xD0), as far as this file calls it (ui_chat_panel.cpp declares it the same way)
class AvatarPipClass	// NAME 3: class-from-file (D24) for app\client\avatar_pip.cpp
{
public:
	void Unknown_7E3A20(int a, unsigned id, int b);		// 0x7E3A20 (slot +0x60: 0, the avatar's or the other's id, 0)  // NAME 1 [a, id, b]: what 0x564030 passes
	void Unknown_7E4350();								// 0x7E4350 (slot +0x5C)
	~AvatarPipClass();									// 0x7E3410 (the destructor deletes both pips)
};
































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

// 0x5E2F00
void UIPTradeClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (Unknown0B4) Unknown0B4->Unknown_7C(view, false);
	if (Unknown0AC) Unknown0AC->Unknown_7C(view, arg);
	if (Unknown0B0) Unknown0B0->Unknown_7C(view, arg);
	Unknown5BB320Class::Unknown_7C(view, arg);
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
// NOT EXACT: register-only (0.72, same length: the client keeps the sclient in edi and the avatar in ebx, ours the other way)
void UIPTradeClass::Unknown_5E37F0()
{
	ClientClass *sclient = Client->As_Space_Client();
	ClientObjectClass *avatar = sclient->Get_Avatar();
	if (avatar && avatar->AuxData) {
		if (Unknown_Changed(avatar, "Inventory.PlayerTrade") || Unknown_Changed(avatar, "TradeMoney")) {
			sclient->Send_Packet(&ActionPacketClass(sclient->AvatarID, 0x17, ((ClientClass *)Client)->Unknown1134, 0));
			Unknown_5E3340();
		} else if (((ClientClass *)Client)->Find_Object(((ClientClass *)Client)->Unknown1134) && Unknown_Changed_Money(((ClientClass *)Client)->Find_Object(((ClientClass *)Client)->Unknown1134), "TradeMoney")) {
			Unknown_5E3340();
		}
	}
}

// 0x5E39F0
// NOT EXACT: inline budget (0.32: the client calls auxdata.h's Unknown_Find_Field out of line here, with the aux data read before the name; ours inlines it)
void UIPTradeClass::Unknown_5E39F0()
{
	if (Unknown0B4 && Unknown0B4->Active) {
		int result = Unknown0B4->Unknown_64A220();
		if (result != 0) {
			if (result == 2) {
				int amount = atoi(Unknown0B4->Unknown_64A240());
				MoneyClass money;
				DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
				NameClass name("Money");
				DeltaTemplateClass<MoneyClass> *t = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
				if (t && t->Unknown70) money = t->Value;
				if (money + Unknown0E0 >= MoneyClass(amount)) {
					Unknown0E0.Unknown00 = amount;
					SClient *sclient = Client->As_Space_Client();
					sclient->Send_Packet(&ActionPacketClass(sclient->AvatarID, 0x18, ((ClientClass *)Client)->Unknown1134, (int)Unknown0E0.Unknown00));
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
// NOT EXACT: shape (0.69: the client's first return releases the style in place, ours jumps to a shared tail; 450 of 466 bytes)
void UIPTradeClass::Unknown_5E3C90()
{
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("RECIPIENT_NAME_TRADE00", InterfaceMesh->Peek_Object()));
	const char *text = font->Unknown_6D87D0();
	if (text && *text && *text != ' ') return;
	SClient *sclient = Client->As_Space_Client();
	RefCountPtr<UIPTradeAvatarClass> avatar = Create_NEW((UIPTradeAvatarClass *)sclient->Unknown_74C7B0(((ClientClass *)Client)->Unknown1134));
	if (!avatar) return;
	std::string name;
	avatar->Desc.Get_Full_Name(name);
	font->Change_Text(name.c_str(), 0);
}
