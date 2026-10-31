// Earth & Beyond client (reconstruction): app\Client\ui_chat_panel.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_chat_panel.h"
#include "client.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "option.h"
#include "..\shared\unknown_debug.h"
#include "control_keys_manager.h"

// fontmanager.h's FontManagerClass at 0xBE67C4, as far as this file calls it (as ui_cpit.cpp declares it)
class FontStyleClass : public RenderObjClass	// NAME 1 [FontStyleClass]: fontmanager.h's (a DynamicMeshClass there)
{
public:
	void Change_Text(const char *text, int unknown);	// 0x6CDBC0 (fontmanager.h's)  // NAME 3: the client's label
	const char *Unknown_6D87D0();						// 0x6D87D0: the text shown  // NAME 1: neutral (D10)
};
class FontManagerClass { public: FontStyleClass *Get_Style(const char *name, unsigned int id); };	// NAME 3: fontmanager.h's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// gadget.cpp's gadget, as far as the chat panel calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (the panel's buttons shown)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral

	void Unknown_65F2D0(int arg);						// 0x65F2D0 (0x564400 passes 0)  // NAME 1 [arg]: neutral
};
void Unknown_65EFB0(GadgetClass *gadget, const char *section, const char *name);	// 0x65EFB0 (cdecl, gadget.cpp): the button's text from a string section  // NAME 1 [section, name]: ui_group.cpp's

// 0x563060
void UIChatPanelClass::Unknown_5C()
{
	if (UnknownD4) UnknownD4 = NULL;
	ChatInt->Unknown_5C();
	InterfaceClass::Unknown_5C();
}

// 0x5638E0
bool UIChatPanelClass::Unknown_5638E0(int key)
{
	if (!ChatInt) return(false);
	return(ChatInt->Unknown_B8(key));
}










































































































































































































































































































































































































































































































































































































































































// 0x563DC0
void UIChatPanelClass::Unknown_563DC0(bool on)
{
	RefCountPtr<FontStyleClass> font = Create_Get(Unknown_BE67C4->Get_Style("avatar_name_TSI05B", (unsigned int)Unknown84));
	FAIL_IF(!font) return;
	if (on) {
		if (strcmp(Unknown124.c_str(), font->Unknown_6D87D0()) != 0) font->Change_Text(Unknown124.c_str(), 0);
		if (Unknown120) {
			Unknown_564150(true);
			Unknown_564400(false);
		} else {
			Unknown_564150(false);
			Unknown_564400(true);
		}
		Unknown267 = true;
	} else {
		font->Change_Text("", 0);
		Unknown110->Unknown_7E4350();
		Unknown114->Set_Hidden(true);
		Unknown_564150(false);
		Unknown_564400(false);
		UnknownA4[5]->Unknown_65F2D0(0);
		UnknownA4[5]->Unknown_40(false);
		Unknown267 = false;
	}
}













































// 0x564150
void UIChatPanelClass::Unknown_564150(bool on)
{
	RefCountPtr<RenderObjClass> big_robj = Create_Get(Unknown84->Get_Sub_Object_By_Name("FRAME_DETAIL"));
	RefCountPtr<RenderObjClass> bigbutframe = Create_Get(Unknown84->Get_Sub_Object_By_Name("RESIZE_MIN01"));
	FAIL_IF(!big_robj) return;
	FAIL_IF(!bigbutframe) return;
	big_robj->Set_Hidden(!on);
	bigbutframe->Set_Hidden(!on);
	UnknownA4[2]->Unknown_8C(!on);
	UnknownA4[2]->Unknown_40(on);
	UnknownA4[5]->Unknown_40(on);
	if (on) Unknown_65EFB0(UnknownA4[5], "UI_CHAT_PANEL", "AVATAR_THEM");
}


// 0x564400
void UIChatPanelClass::Unknown_564400(bool on)
{
	RefCountPtr<RenderObjClass> small_robj = Create_Get(Unknown84->Get_Sub_Object_By_Name("FRAME_DETAIL_MI"));
	RefCountPtr<RenderObjClass> smallbutframe = Create_Get(Unknown84->Get_Sub_Object_By_Name("RESIZE_MAX01"));
	FAIL_IF(!small_robj) return;
	FAIL_IF(!smallbutframe) return;
	small_robj->Set_Hidden(!on);
	smallbutframe->Set_Hidden(!on);
	UnknownA4[1]->Unknown_8C(!on);
	UnknownA4[1]->Unknown_40(on);
	UnknownA4[5]->Unknown_40(!on);
	if (on) UnknownA4[5]->Unknown_65F2D0(0);
}

// 0x565100
void UIChatPanelClass::Unknown_565100()
{
	Unknown_5646A0(Unknown14C);
}

// 0x565BA0
const char *UIChatPanelClass::Unknown_565BA0()
{
	return(Unknown248.c_str());
}

// 0x565BC0
const char *UIChatPanelClass::Unknown_565BC0()
{
	if (Unknown148 >= 6 && Unknown148 < 10) return(Unknown158[Unknown148 - 6].c_str());
	return(NULL);
}

// 0x565C00
const char *UIChatPanelClass::Unknown_565C00()
{
	if (Unknown150 >= 0 && Unknown150 < 5) return(Unknown188[Unknown150].c_str());
	return(NULL);
}

// 0x565F30
void UIChatPanelClass::Unknown_565F30(int arg)
{
	Unknown108 = arg;
	if (arg == 1) Unknown10C = true;
}

// 0x567930
void UIChatPanelClass::Unknown_567930(int channel)
{
	Unknown144 = channel;
	IntOptionStruct *option = OptionClass::Instance->Find_Int("Chat_Default_Channel");
	if (option) option->Value = channel;
}

// 0x567970
bool UIChatPanelClass::Unknown_567970()
{
	if (Unknown1E8 && Unknown1E8->Is_Active()) return(true);
	return(false);
}

// 0x5688E0
int UIChatPanelClass::Unknown_5688E0(void **list)
{
	int count = 0;
	while (list[count]) count++;
	return(count);
}

// 0x568910
void UIChatPanelClass::Unknown_568910()
{
	if (UnknownD4) UnknownD4 = NULL;
}

// 0x568950
void UIChatPanelClass::Unknown_568950()
{
	Unknown_5689D0(0);
	Unknown_5689D0(1);
	Unknown_5689D0(2);
	Unknown_5689D0(3);
	Unknown_5689D0(4);
	UnknownD4 = NULL;
}








































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// 0x5698F0
void UIChatPanelClass::Unknown_5698F0(int id)
{
	int index = id - 8;
	FAIL_IF((index < 0) || (index >= NUM_COMBO)) return;
	SoundLibraryClass::Play_2D_Sound("SR_scifielectronic42.wav", 2);
	if (UnknownD4) UnknownD4 = NULL;
	UnknownD4 = Combo[index];
	if (id == 8) UnknownD4->Unknown_57AD50(Unknown1F4.c_str());
}

// 0x56BA40
void UIChatPanelClass::Unknown_56BA40(bool on)
{
	Unknown25C = on;
	Unknown260 = 0;
	if (Unknown258) Unknown258->Set_Hidden(1);
}

// 0x56BA80
bool UIChatPanelClass::Unknown_56BA80()
{
	return(Unknown25C);
}

































































































































































































































































































// 0x56BAA0
void UIChatPanelClass::Unknown_56BAA0(bool show, const char *text)
{
	if (!UnknownA4[4]) return;
	UnknownA4[4]->Unknown_40(show);
	UnknownA4[4]->Unknown_8C(!show);
	if (text) {
		RefCountPtr<FontStyleClass> font = Create_Get(Unknown_BE67C4->Get_Style("message_TSI05B", (unsigned int)Unknown84));
		FAIL_IF(!font) return;
		font->Change_Text(text, 0);
	}
	MessageButtonShowing = show;
}



// 0x56BC10
void UIChatPanelClass::Unknown_70(ControlKeysManagerClass *manager)
{

	FAIL_IF(!manager) return;
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Help"), (void *)16, 0);
}

// 0x56BD80
void UIChatPanelClass::Unknown_74(ControlKeysManagerClass *manager)
{

	FAIL_IF(!manager) return;
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Help"), 0);
}







































// 0x56C0C0
void UIChatPanelClass::Unknown_56C0C0()
{
	unsigned int window = Unknown134;
	if (window == 0) {
		window = OptionClass::Instance->Get_Int("DefaultChatWindow");
	} else {
		window++;
		if (Unknown10D) Unknown_5685C0();
	}
	if (window >= 4) window = 1;
	Unknown_56C180(window);
	IntOptionStruct *option = OptionClass::Instance->Find_Int("DefaultChatWindow");
	if (option) {
		option->Value = Unknown134;
		OptionClass::Instance->Save();
		return;
	}
	G_ASSERT(false);
}



























































// 0x56C730
void UIChatPanelClass::Unknown_56C730(const UIChatIntEventClass &event, UIChatIntClass *chat_int)
{
	FAIL_IF(chat_int != ChatInt.Peek()) return;
	if (event.Unknown00 == 1) {
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		Unknown_565120();
		Unknown_565390(0);
	}
}



/*
** Reconstruction notes (CH1, 2026-10-31)
**
** The chat panel: SClient::Init news it (0x561DD0, 0x268 bytes) over ui_chat_panel_base.cpp's class (0x56D280, 0xA0
** bytes: InterfaceClass and five members of its own). Its table 0xAEFF1C: the destructor 0x562410, slots +0x54
** 0x5629E0, +0x58 0x562F50, +0x5C 0x563060, +0x60 0x5630B0, +0x64 0x563260 (empty), +0x68 0x563280, +0x70 / +0x74 the
** control keys, +0x7C 0x563620; +0x88 / +0x8C / +0x98 the base's, +0x90 / +0x94 / +0x9C the base's inline defaults
** (0x5623B0 / 0x5623D0 / 0x5623F0, emitted with this file's table).
**
** The asserts sit on the client's lines (D25); the functions not written yet leave their lines blank. docs/data/
** asserts.txt files the strings 0xB7CB6C-0xB7CD30 (if_chat*.tga, fa_tsi05b.ini, TextClick03.wav,
** SR_scifielectronic42.wav, the NUM_COMBO assert) under ui_buff.cpp: they are this file's (0x5629FC reads
** fa_tsi05b.ini; the NUM_COMBO assert passes this file's __FILE__).
*/
