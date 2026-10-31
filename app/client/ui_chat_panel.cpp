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

// What the avatar pip's +0x2C holds (a render object), as far as 0x563C50 reads it
class AvatarPipUnknown2CClass : public RenderObjClass	// NAME 1: neutral (D10)
{
public:
	char Unknown_Pad[0x204 - sizeof(RenderObjClass)];
	int Unknown204;										// +0x204 (0x563C50: 0 hides the pip)
};

// avatar_pip.cpp's class at the panel's +0x110 (by its functions' file), as far as this file calls it
class AvatarPipClass	// NAME 3: class-from-file (D24) for app\client\avatar_pip.cpp
{
public:
	void Unknown_7E36B0(SClient *client, const RefCountPtr<RenderObjClass> &robj, int arg);	// 0x7E36B0 (slot +0x58)  // NAME 1 [client, robj, arg]: what 0x562F50 passes
	void Unknown_7E3A20(int a, unsigned id, int b);		// 0x7E3A20 (0x564030: 0, +0x118, 0)  // NAME 1 [a, id, b]: what 0x564030 passes
	void Unknown_7E4350();								// 0x7E4350

	char Unknown00[0x2C];
	RefCountPtr<AvatarPipUnknown2CClass> Unknown2C;		// +0x2C (0x563C50 tests its +0x204)
};

// The client at InterfaceClass +0x40, as this file calls it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::Unknown_18; SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// local_interface_sort.cpp's sort object (8 bytes; as ui_cpit.cpp declares it)
class LineDisplaySortClass  // NAME 1: neutral (D10)
{
public:
	LineDisplaySortClass(InterfaceClass *inter);		// 0x541980  // NAME 1 [inter]: view.cpp's
	~LineDisplaySortClass();							// 0x6E18B0
private:
	char Unknown00[8];
};

// The reply list at the panel's +0x1E8, as far as this file calls it
class UIChatReplyListClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	int Unknown_56EFB0();								// 0x56EFB0: the line picked (1-based), 0 for none, negative to close
};

extern unsigned int Unknown_BFAE3C; extern unsigned int Unknown_BFAE40;	// NAME 1: neutral (D10): the time now and at the last frame in ms (as scriptedmovie.cpp declares them)
bool Unknown_42EC60(int a, int b);						// 0x42EC60 (cdecl): *0xBDAA70's slot +0x0C (0x567760: 1, 0 and 2, 0)  // NAME 1 [a, b]: what 0x567760 passes

// The chat channels the panel offers (0xB7C530: name, option, the destination's number as text), NULL-ended
struct ChatChannelStruct	// NAME 1: neutral (D10)
{
	const char *Name;									// NAME 2: what it holds
	const char *Option;									// NAME 2: the option tested (OptionClass::Get_Bool)
	const char *Number;									// NAME 2: what 0x565390 passes to atoi
};
static ChatChannelStruct Unknown_B7C530[17] = {	// NAME 1: neutral (D10): 17 entries (0x56B700 walks to 0xB7C5FC), four filled
	{ "Broadcast", "Channel_Broadcast", "0" },
	{ "Local", "Channel_Local", "1" },
	{ "Guild", "Channel_Guild", "2" },
	{ "Group", "Channel_Group", "3" },
};

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

	char Unknown04[0x30 - 0x04];						// +0x04
	RenderObjClass *Unknown30;							// +0x30 (slot +0x58 0x562F50 hands it to 0x497300)
	RenderObjClass *Peek_Unknown30() { return(Unknown30); }	// NAME 1: neutral (D10)
};
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl): slot +0x58 hands the result to the pip  // NAME 1 [robj]: neutral
void Unknown_65EFB0(GadgetClass *gadget, const char *section, const char *name);	// 0x65EFB0 (cdecl, gadget.cpp): the button's text from a string section  // NAME 1 [section, name]: ui_group.cpp's










































































































































































































































































































































































































































































































// 0x562F50
void UIChatPanelClass::Unknown_58()
{
	LineDisplaySortClass sort(this);
	Unknown110->Unknown_7E36B0(Client->As_Space_Client(), Unknown_497300(UnknownA4[5]->Peek_Unknown30()), 0);
	ChatInt->Unknown_58();
	UIChatPanelBaseClass::Unknown_58();
	UIViewClass *view = View;
	Unknown1E8->Unknown_7C(view, 0);
}

// 0x563060
void UIChatPanelClass::Unknown_5C()
{
	if (UnknownD4) UnknownD4 = NULL;
	ChatInt->Unknown_5C();
	InterfaceClass::Unknown_5C();
}

// NOT EXACT: inline-budget (ours expands 0x56C0C0 at its call; the client calls it)
// 0x5630B0
void UIChatPanelClass::Unknown_60()
{
	Unknown_566AF0();
	Unknown_565F80();
	Unknown_567760();
	Unknown_563C50();
	if (ChatInt->Unknown98) Unknown_56C0C0();
	for (int i = 0; i < NUM_COMBO; i++) {
		Combo[i]->Unknown_60();
	}
	if (!Unknown227) {
		Unknown_56B700();
		Unknown227 = true;
	}
	if (Unknown25C) {
		Unknown260 += (float)(Unknown_BFAE3C - Unknown_BFAE40) * 0.001f;
		if (Unknown260 > 0.5f) {
			Unknown260 -= 0.5f;
			RenderObjClass *light = Unknown258;
			if (light) light->Set_Hidden(!light->Is_Hidden());
		}
	}
	SClient *sclient = Client->As_Space_Client();
	bool changed = false;
	if (Unknown265 != sclient->Unknown_74D570()) {
		Unknown265 = sclient->Unknown_74D570();
		changed = true;
	}
	if (Unknown266 != sclient->Unknown_728300()) {
		Unknown266 = sclient->Unknown_728300();
		changed = true;
	}
	if (changed) Unknown_5689D0(0);
}

// 0x563620
void UIChatPanelClass::Unknown_7C(UIViewClass *view, int arg)
{
	if (View != view) {
		LineDisplaySortClass sort(this);
		InterfaceClass::Unknown_7C(view, arg);
		if (UnknownF0) UnknownF0->Unknown_7C(view, 0);
		if (ChatInt) ChatInt->Unknown_7C(view, 1);
		if (UnknownF8) UnknownF8->Unknown_7C(view, 0);
		if (Unknown104) Unknown104->Unknown_7C(view, 0);
		if (Unknown1E8) Unknown1E8->Unknown_7C(view, 0);
		Unknown_5689D0(4);
		for (int i = 0; i < NUM_COMBO; i++) {
			Combo[i]->Unknown_57B440();
			Combo[i]->Unknown_7C(view, 1);
		}
	}
}

// 0x5638E0
bool UIChatPanelClass::Unknown_5638E0(int key)
{
	if (!ChatInt) return(false);
	return(ChatInt->Unknown_B8(key));
}

// 0x563C50
void UIChatPanelClass::Unknown_563C50()
{
	SClient *sclient = Client->As_Space_Client();
	RefCountPtr<AvatarPipUnknown2CClass> object = Unknown110->Unknown2C;
	if (Unknown118) {
		if (!sclient->Find_Object(Unknown118) || (object && !object->Unknown204)) {
			Unknown110->Unknown_7E4350();
			Unknown114->Set_Hidden(1);
			if (Unknown118 == Unknown11C) {
				Unknown118 = 0;
				Unknown11C = 0;
			} else {
				Unknown118 = Unknown11C;
			}
		}
	}
	if (Unknown118 != 0) {
		if (Unknown120) Unknown110->Unknown_7E3A20(0, Unknown118, 0);
		if (!Unknown267) Unknown_563DC0(true);
	} else {
		if (Unknown267) Unknown_563DC0(false);
	}
}

// 0x563DC0
void UIChatPanelClass::Unknown_563DC0(bool on)
{
	RefCountPtr<FontStyleClass> font = Create_Get(Unknown_BE67C4->Get_Style("avatar_name_TSI05B", (unsigned int)Peek_Unknown84()));
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

// 0x564030
void UIChatPanelClass::Unknown_564030(bool on)
{
	Unknown120 = on;
	if (on) {
		LineDisplaySortClass sort(this);
		Unknown110->Unknown_7E3A20(0, Unknown118, 0);
		Unknown114->Set_Hidden(0);
		Unknown_564150(true);
		Unknown_564400(false);
	} else {
		Unknown110->Unknown_7E4350();
		Unknown114->Set_Hidden(1);
		Unknown_564150(false);
		Unknown_564400(true);
	}
	SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
	SoundLibraryClass::Play_2D_Sound("vidscreen_1.wav", 2);
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

// 0x564F50
void UIChatPanelClass::Unknown_564F50()
{
	Unknown204 = Unknown1F4;
	Unknown_5646A0(Unknown144);
}

// 0x565100
void UIChatPanelClass::Unknown_565100()
{
	Unknown_5646A0(Unknown14C);
}

// 0x565120
void UIChatPanelClass::Unknown_565120()
{
	SClient *sclient = Client->As_Space_Client();
	Unknown228.clear();
	for (int i = 0; Unknown_B7C530[i].Name; i++) {
		if (OptionClass::Instance->Get_Bool(Unknown_B7C530[i].Option)) {
			if (strcmp(Unknown_B7C530[i].Name, "Guild") || sclient->Unknown_728300()) {
				if (strcmp(Unknown_B7C530[i].Name, "Group") || sclient->Unknown_74D570()) {
					Unknown228.push_back(std::string(Unknown_B7C530[i].Name));
				}
			}
		}
	}
	if (Unknown214.length()) Unknown228.push_back(Unknown214);
}

// 0x5658B0
void UIChatPanelClass::Unknown_5658B0(const char *name)
{
	for (int i = 0; i < 3; i++) {
		if (strcmp(Unknown158[i].c_str(), name) == 0) {
			Unknown14C = i + 6;
			Unknown_5646A0(i + 6);
			return;
		}
	}
	for (i = 2; i > 0; i--) {
		Unknown158[i] = Unknown158[i - 1];
	}
	Unknown158[0] = name;
	Unknown14C = 6;
	Unknown_5646A0(6);
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

// 0x565C40
void UIChatPanelClass::Unknown_565C40(int index)
{
	Unknown150 = index;
	Unknown148 = 9;
	Unknown248 = Unknown_565C00();
	if (ChatInt) {
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "To %s:", Unknown_565C00());
		ChatInt->Unknown_B4(buffer);
		SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	}
}

// NOT EXACT: inline-budget (the client calls _Grow out of line in the last assign, ours inlines it; ebx / esi swapped in the shift loop)
// 0x565D80
void UIChatPanelClass::Unknown_565D80(const char *name)
{
	Unknown1D8 = name;
	for (int i = 0; i < 5; i++) {
		if (strcmp(Unknown188[i].c_str(), name) == 0) {
			Unknown150 = i;
			return;
		}
	}
	for (i = 4; i > 0; i--) {
		Unknown188[i] = Unknown188[i - 1];
	}
	Unknown188[0] = name;
	if (Unknown1E8->Is_Active()) {
		Unknown1E8->Unknown_5C();
		Unknown_567540(true);
	}
}

// 0x565F30
void UIChatPanelClass::Unknown_565F30(int arg)
{
	Unknown108 = arg;
	if (arg == 1) Unknown10C = true;
}

// NOT EXACT: inline-budget (the client calls string::assign(const char *, size_t) out of line for +0x248, ours inlines it with _Grow; frame 0x40 vs 0x44; calling 0x565C40 instead does not change it)
// 0x567760
void UIChatPanelClass::Unknown_567760()
{
	if (Unknown1E8->Is_Active()) {
		int index = ((UIChatReplyListClass *)Unknown1E8)->Unknown_56EFB0();
		if (index != 0) {
			if (index > 0) {
				index--;
				Unknown148 = 9;
				Unknown150 = index;
				Unknown248 = Unknown_565C00();
				if (ChatInt) {
					char buffer[64];
					_snprintf(buffer, sizeof(buffer), "To %s:", Unknown_565C00());
					ChatInt->Unknown_B4(buffer);
					SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				}
				Client->As_Space_Client()->Unknown_744E70(true);
			}
			Unknown1E8->Unknown_5C();
			Unknown1EC = 0;
		}
		if (!Unknown_42EC60(1, 0) && !Unknown_42EC60(2, 0)) {
			Unknown1F0 = true;
			return;
		}
		if (Unknown1F0) {
			Unknown1F0 = false;
			Unknown1E8->Unknown_5C();
			Unknown1EC = 0;
		}
	}
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

// 0x5680B0
void UIChatPanelClass::Unknown_5680B0(const std::string &name)
{
	if (Unknown214 == name) return;
	Unknown214 = name;
	Unknown224 = false;
	Unknown_5689D0(0);
}

// 0x5682C0
std::string UIChatPanelClass::Unknown_5682C0()
{
	return(Unknown214);
}

// 0x568440
std::string UIChatPanelClass::Unknown_568440()
{
	return(Unknown204);
}

// 0x568690
void UIChatPanelClass::Unknown_568690()
{
	if (Unknown104) {
		Unknown104->Unknown_7C(NULL, 0);
		Unknown104 = NULL;
		UnknownEC = NULL;
	}
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
		RefCountPtr<FontStyleClass> font = Create_Get(Unknown_BE67C4->Get_Style("message_TSI05B", (unsigned int)Peek_Unknown84()));
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


// 0x56BEF0
void UIChatPanelClass::Unknown_56BEF0()
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;
	if (sclient->Unknown_744B60()) return;
	ClientObjectClass *co = NULL;
	if (sclient->MainView) {
		co = sclient->MainView->Unknown_5C();
		if (!co) return;
		if (!co->Unknown_6F5A40()) return;
	}
	std::string name("");
	co->Get_Name(name);
	Unknown_5658B0(name.c_str());
	sclient->Unknown_744E70(true);
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
