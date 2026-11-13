// Earth & Beyond client (reconstruction): app\client\ui_groupopt.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_groupopt.cpp', 0x59AD10-0x59BEDF by its asserts and its object's end).
#include <string.h>
#include "ui_groupopt.h"
#include "ui_groupfind.h"
#include "gadget.h"
#include "gd_robj.h"
#include "fontmanager.h"
#include "client.h"
#include "co.h"
#include "unknown_7e7e00.h"
#include "local_interface_sort.h"
#include "..\packets\pkaction.h"
#include "..\packets\pkoption.h"
#include "..\audio\soundlib.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h: its Type_Name(int *) for kv_pair.h, C2665 without)
#include "..\shared\g_assert.h"


// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// The buttons' models, texts and tooltips (0xB7FD34, 0xB7FD4C, 0xB7FD64)
static const char *Unknown_B7FD34[] = { "exit", "looking", "find", "leave", "disband", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7FD4C[] = { NULL, "LOOKING_opt00", "FIND_opt00", "LEAVE_opt00", "DISBAND_opt00", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7FD64[] = { "EXIT", "LOOK_FOR_GROUP", "AVAILABLE_RECRUITS", "LEAVE_GROUP", "DISBAND_GROUP", NULL };	// NAME 1: neutral (D10)
// The check buttons' models and texts (0xB7FD7C, 0xB7FD94)
static const char *Unknown_B7FD7C[] = { "invites_check", "non combat_chec", "question mark_c", "auto split_chec", "looting_check", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7FD94[] = { "INVITES_opt00", "AUTO_SPLIT_opt00", "LOOTING_opt00", "AUTO_RELEASE_LOOTING_opt00", NULL, NULL };	// NAME 1: neutral (D10)
// The 'looking' button's text by LookingForGroup (0xB7FDAC), the 'find' button's by the list's state (0xB7FDB8)
static const char *Unknown_B7FDAC[] = { "Look for new Group", "Stop Looking", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7FDB8[] = { "Available Recruits", "Close List", NULL };	// NAME 1: neutral (D10)

// 0x59AD10
UIGroupOptClass::UIGroupOptClass(SClient *client) :
	ChatPanelUIBase(client),
	Unknown0A0(false),
	Unknown0A4(0)
{
	memset(Unknown0AC, 0, sizeof(Unknown0AC));
	memset(Unknown0C0, 0, sizeof(Unknown0C0));
	FindUI = Create_NEW(new UIGroupFindClass(client));
}

// 0x59AEE0
UIGroupOptClass::~UIGroupOptClass()
{
	if (FindUI) {
		FindUI = NULL;
	}
	Unknown_5C();
}

// 0x59AFF0
void UIGroupOptClass::Unknown_54()
{
	if (Unknown_98("tsi24a", "fa_tsi24a.ini")) {
		FindUI->Unknown_54();
		FindUI->Unknown_7C(View, true);
		int i;
		for (i = 0; i < 5; i++) {
			Unknown0AC[i] = Add_Button(i + 1, Unknown_B7FD34[i], Unknown_B7FD4C[i]);
			if (Unknown_B7FD64[i] && Unknown0AC[i]) {
				GadgetClass::Set_Tooltip(Unknown0AC[i], "UI_FIND_LEAVE", Unknown_B7FD64[i]);
			}
			if (i >= 3) {
				Unknown0AC[i]->Unknown_40(false);
			}
		}
		for (i = 0; i < 5; i++) {
			Unknown0C0[i] = Add_Check_Button(i + 7, Unknown_B7FD7C[i], Unknown_B7FD94[i]);
			if (i >= 1) {
				Unknown0C0[i]->Unknown_40(false);
			}
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x59B150
void UIGroupOptClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		Unknown_59B990();
		ChatPanelUIBase::Unknown_58();
		Unknown0A4 = 0;
		Unknown_59BC40();
	}
}

// 0x59B1F0
void UIGroupOptClass::Unknown_5C()
{
	if (Active) {
		FindUI->Unknown_5C();
		InterfaceClass::Unknown_5C();
	}
}











































































// NOT EXACT: inline budget (the client calls rtti.h's cast 0x598D80 out of line in the GroupInfo find; ours inlines it, as ui_group.cpp's 0x598370; attempts.py show 0x59B220)
// 0x59B220
void UIGroupOptClass::Unknown_60()
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;
	NameClass name("GroupInfo"); DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(sclient->AuxData, name);
	if (group && group->Unknown70 > 1) {
		Unknown_59B990();
		Unknown_59BC40();
	}
	ClientObjectClass *avatar = sclient->Get_Avatar();
	if (avatar && avatar->GameID != Unknown0A4) {
		FindUI->Unknown_5C();
		Unknown0A4 = avatar->GameID;
	}
	if (!FindUI->Is_Active()) {
		((RObjGadgetClass *)Unknown0AC[2])->Unknown34->Change_Text(Unknown_B7FDB8[0], 0);
		GadgetClass::Set_Tooltip(Unknown0AC[2], "UI_FIND_LEAVE", "AVAILABLE_RECRUITS");
	}
}

// NOT EXACT: register allocation / temporaries (the client passes the check buttons' Checked through a stack byte and orders the packet temporaries' slots differently; attempts.py show 0x59B380)
// 0x59B380
bool UIGroupOptClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0A0 = !Unknown0A0;
			((ClientClass *)Client)->Send_Packet(&OptionPacketClass(((ClientClass *)Client)->AvatarID, 0, Unknown0A0));
			((RObjGadgetClass *)Unknown0AC[1])->Unknown34->Change_Text(Unknown_B7FDAC[Unknown0A0], 0);
			if (Unknown0A0) {
				GadgetClass::Set_Tooltip(Unknown0AC[1], "UI_FIND_LEAVE", "STOP_LOOKING");
			} else {
				GadgetClass::Set_Tooltip(Unknown0AC[1], "UI_FIND_LEAVE", "LOOK_FOR_GROUP");
			}
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (FindUI->Is_Active()) {
				FindUI->Unknown_5C();
			} else {
				((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 16, ((ClientClass *)Client)->AvatarID, 0));
				FindUI->Unknown_58();
				((RObjGadgetClass *)Unknown0AC[2])->Unknown34->Change_Text(Unknown_B7FDB8[1], 0);
				GadgetClass::Set_Tooltip(Unknown0AC[2], "UI_FIND_LEAVE", "CLOSE_LIST");
			}
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 14, ((ClientClass *)Client)->AvatarID, 0));
			return(true);
		case 5:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 13, ((ClientClass *)Client)->AvatarID, 0));
			return(true);
		case 7:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			((ClientClass *)Client)->Send_Packet(&OptionPacketClass(((ClientClass *)Client)->AvatarID, 1, Unknown0C0[0]->Checked));
			return(true);
		case 8:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			((ClientClass *)Client)->Send_Packet(&OptionPacketClass(((ClientClass *)Client)->AvatarID, 3, Unknown0C0[1]->Checked));
			return(true);
		case 9: {
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			bool checked = Unknown0C0[2]->Checked;
			((ClientClass *)Client)->Send_Packet(&OptionPacketClass(((ClientClass *)Client)->AvatarID, 4, checked));
			if (checked) {
				Unknown0C0[3]->Unknown_7E8030(false);
			}
			return(true);
		}
		case 10: {
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			bool checked = Unknown0C0[3]->Checked;
			((ClientClass *)Client)->Send_Packet(&OptionPacketClass(((ClientClass *)Client)->AvatarID, 5, checked));
			if (checked) {
				Unknown0C0[2]->Unknown_7E8030(false);
			}
			return(true);
		}
	}
	return(false);
}

// 0x59B8D0
void UIGroupOptClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
		FindUI->Unknown_7C(view, false);
	}
}

// 0x59B910
void UIGroupOptClass::Unknown_59B910(bool on)
{
	int i;
	for (i = 0; i < 5; i++) {
		if (Unknown0AC[i]) {
			Unknown0AC[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 5; i++) {
		if (Unknown0C0[i]) {
			Unknown0C0[i]->Unknown_40(on);
		}
	}
	if (on) {
		Unknown_59B990();
	}
}

// 0x59B990
void UIGroupOptClass::Unknown_59B990()
{
	if (Unknown_59BBC0()) {
		Unknown0AC[1]->Unknown_40(false);
		if (Unknown_59BC00()) {
			Unknown0AC[4]->Unknown_40(true);
			Unknown0C0[1]->Unknown_40(true);
			Unknown0C0[2]->Unknown_40(true);
			Unknown0C0[3]->Unknown_40(true);
		} else {
			Unknown0AC[3]->Unknown_40(true);
			Unknown0C0[1]->Unknown_40(false);
			Unknown0C0[2]->Unknown_40(false);
			Unknown0C0[3]->Unknown_40(false);
		}
		Unknown0C0[1]->Unknown_8C(false);
		Unknown0C0[2]->Unknown_8C(false);
		Unknown0C0[3]->Unknown_8C(false);
	} else {
		Unknown0AC[1]->Unknown_40(true);
		Unknown0AC[3]->Unknown_40(false);
		Unknown0AC[4]->Unknown_40(false);
		Unknown0C0[1]->Unknown_40(false);
		Unknown0C0[2]->Unknown_40(false);
		Unknown0C0[1]->Unknown_8C(true);
		Unknown0C0[2]->Unknown_8C(true);
		Unknown0C0[3]->Unknown_40(true);
		Unknown0C0[3]->Unknown_8C(false);
	}
	Unknown0C0[4]->Unknown_40(false);
	Unknown0C0[4]->Unknown_8C(true);
}























































// 0x59BBC0
bool UIGroupOptClass::Unknown_59BBC0()
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(false);
	return(sclient->Unknown_74D570());
}

// 0x59BC00
bool UIGroupOptClass::Unknown_59BC00()
{
	SClient *sclient = Client->As_Space_Client();

	FAIL_IF(!sclient) return(false);
	return(sclient->Unknown_74D690());
}


// NOT EXACT: inline budget (the client calls rtti.h's cast 0x598D80 out of line in the GroupInfo find; ours inlines it; a file-end inline_depth(1) matched it but broke the constructor and destructor; attempts.py show 0x59BC40)
// 0x59BC40
void UIGroupOptClass::Unknown_59BC40()
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;
	FAIL_IF(!sclient->AuxData) return;
	NameClass name("GroupInfo"); DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(sclient->AuxData, name);
	if (group && group->Unknown70) {
		Unknown0A0 = group->LookingForGroup.Value;
		((RObjGadgetClass *)Unknown0AC[1])->Unknown34->Change_Text(Unknown_B7FDAC[Unknown0A0], 0);
		Unknown0C0[0]->Unknown_7E8030(group->AllowGroupInvite.Value);
		Unknown0C0[1]->Unknown_7E8030(group->ForceAutoSplit.Value);
		Unknown0C0[2]->Unknown_7E8030(group->RestrictedLootingRights.Value);
		Unknown0C0[3]->Unknown_7E8030(group->AutoReleaseLootingRestrictions.Value);
	}
}


/*
** Reconstruction notes (O7, 2026-11-13). The asserts sit on the client's lines (D25): 0x59B220's FAIL_IF l.187,
** 0x59BBC0 / 0x59BC00's l.394 / 403 (0x59B990 expands both), 0x59BC40's l.413 / 414. The object runs from the
** constructor 0x59AD10 to its guarded <string> initialiser 0x59BE70 and the adjustor thunks 0x59BEC0 / 0x59BED0.
** The class name follows the file (D24). 0x59B8B0 (~OptionPacketClass), 0x59BDD0 (Create_NEW) and 0x59BE00
** (~RefCountPtr<UIGroupFindClass>) are the compiler's copies. The button tables are NULL-terminated as the
** client's data shows (0xB7FD34-0xB7FDC0).
*/
