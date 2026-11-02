// Earth & Beyond client (reconstruction): app\Client\ui_chat_panel.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include "ui_chat_panel.h"
#include "client.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "option.h"
#include "..\packets\pkclientchat.h"
#include "texture.h"
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
	void Unknown_77E3D0(int arg);						// 0x77E3D0 (avatar.cpp; 0x563910 passes 1 or 2)  // NAME 1 [arg]: neutral
	struct UnknownF8Struct { char Unknown00[0xD4]; char Name[0x10C - 0xD4]; };	// NAME 1: neutral (D10): +0xD4 the name 0x563910 copies
	UnknownF8Struct *Get_UnknownF8() { return(&UnknownF8); }	// NAME 1: neutral (D10): the client tests the address

	char Unknown_Pad[0xF8 - sizeof(RenderObjClass)];
	UnknownF8Struct UnknownF8;							// +0xF8
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
#include "local_interface_sort.h"	// InterfaceSortScopeClass (0x541980 / 0x6E18B0)








// The reply list at the panel's +0x1E8, as far as this file calls it
class UIChatReplyListClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	int Unknown_56EFB0();								// 0x56EFB0: the line picked (1-based), 0 for none, negative to close
	void Unknown_56EED0(std::list<std::string> &names, int arg);	// 0x56EED0 (0x567540: the reply names, +0x1EC)  // NAME 1 [names, arg]: what 0x567540 passes
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

// What the +0xF8 window's events carry (an empty type: 0x5685C0 passes one to name the handler)
class UIChatF8EventClass {};	// NAME 1: neutral (D10)

// The chat window's event handler object (0x10 bytes, constructor 0x56D0F0): the receiver and its member
class UIChatIntHandlerClass	// NAME 1: neutral (D10)
{
public:
	UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *));	// 0x56D0F0  // NAME 1 [receiver, handler]: what 0x56C180 passes
	char Unknown00[0x10];
};

// The interface event dispatcher (0x540A20 returns its function-local static at 0xBE5AF0), as far as this file calls
// it: its member templates take the sender, the receiver, the handler and a sample of the event type
class UnknownEventDispatcherClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatF8EventClass &, InterfaceClass *), const UIChatF8EventClass &type);	// 0x56CE40 (the handler removed)  // NAME 1 [sender, receiver, handler, type]: what 0x5685C0 passes
	void Unknown_4F9740(InterfaceClass *sender, UIChatIntHandlerClass *handler);	// 0x4F9740 (the handler added)  // NAME 1 [sender, handler]: what 0x56C180 passes
	void Unknown_56CEC0(UIChatIntClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *), const UIChatIntEventClass &type);	// 0x56CEC0 (the handler removed)  // NAME 1 [sender, receiver, handler, type]: what 0x56C6A0 passes
};
UnknownEventDispatcherClass *Unknown_540A20();			// 0x540A20 (intrface.cpp)  // NAME 1: neutral (D10)

// The chat window's prompts by destination (0xB7C2B8)
static const char *Unknown_B7C2B8[] = { "Broadcast:", "Local:", "To Guild:", "To Group", "To Radio:", "To ", "Secure:", "Secure:", "Secure:", "Reply:" };	// NAME 1: neutral (D10)

// The chat window's icons by kind (0xB7C2E0)
static const char *Unknown_B7C2E0[] = { NULL, "if_chat.tga", "if_chat02.tga", "if_chat03.tga" };	// NAME 1: neutral (D10)

// The object at *0xBFAEE0 (kernel.cpp's), as far as this file calls it: its slot +0x30 gives a texture by name
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual TextureClass *Get_Texture(const char *name, int a, int b, int c);	// +0x30 (0x56C180: the icon, 1, 0, 1)  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

extern int Unknown_BE5F34;								// NAME 1: neutral (D10): the key modifiers (as sclient.cpp declares it; 0x70 for slot +0x68's key 3)

// The windows the panel opens (slot +0x68's key 16 'Online Manual', 0x565F80's requests), each over the window base
class Unknown5DAE90Class : public UIChatPanelWindowClass	// NAME 1: neutral (D10): 0xCC bytes
{
public:
	Unknown5DAE90Class(SClient *client, int a, const char *title, int b);	// 0x5DAE90  // NAME 1 [client, a, title, b]: what 0x563280 passes
	char Unknown9C[0xCC - 0x9C];
};
class Unknown5CE240Class : public UIChatPanelWindowClass { public: Unknown5CE240Class(SClient *client); char Unknown9C[0xC0 - 0x9C]; };	// NAME 1: neutral (D10): 0x5CE240, 0xC0 bytes (request 1)
class Unknown5CE700Class : public UIChatPanelWindowClass { public: Unknown5CE700Class(SClient *client, int a); char Unknown9C[0xB8 - 0x9C]; };	// NAME 1: neutral (D10): 0x5CE700, 0xB8 bytes (request 2)  // NAME 1 [a]: neutral
class Unknown5C9910Class : public UIChatPanelWindowClass { public: Unknown5C9910Class(SClient *client, int a, int b); char Unknown9C[0xE4 - 0x9C]; };	// NAME 1: neutral (D10): 0x5C9910, 0xE4 bytes (requests 3, 4)  // NAME 1 [a, b]: neutral
class Unknown5CEDD0Class : public UIChatPanelWindowClass { public: Unknown5CEDD0Class(SClient *client, int a, int b); char Unknown9C[0xE4 - 0x9C]; };	// NAME 1: neutral (D10): 0x5CEDD0, 0xE4 bytes (requests 5-7)  // NAME 1 [a, b]: neutral
class Unknown5CA010Class : public UIChatPanelWindowClass { public: Unknown5CA010Class(SClient *client, int a); char Unknown9C[0x1B0 - 0x9C]; };	// NAME 1: neutral (D10): 0x5CA010, 0x1B0 bytes (request 8)  // NAME 1 [a]: neutral
class Unknown5C8870Class : public UIChatPanelWindowClass { public: Unknown5C8870Class(SClient *client, int a); char Unknown9C[0xAC - 0x9C]; };	// NAME 1: neutral (D10): 0x5C8870, 0xAC bytes (requests 9, 10)  // NAME 1 [a]: neutral

class Unknown5E9410Class : public UIChatPanelWindowClass { public: Unknown5E9410Class(SClient *client, int mode, std::string channel); char Unknown9C[0xC4 - 0x9C]; };	// NAME 1: neutral (D10): 0x5E9410, 0xC4 bytes (0x566AF0's requests 1, 2, 5, 8: a mode and a channel)  // NAME 1 [mode, channel]: what 0x566AF0 passes
class Unknown5EAB40Class : public UIChatPanelWindowClass { public: Unknown5EAB40Class(SClient *client, int a); char Unknown9C[0xBC - 0x9C]; };	// NAME 1: neutral (D10): 0x5EAB40, 0xBC bytes (requests 3, 4)  // NAME 1 [a]: neutral
class Unknown5E6290Class : public UIChatPanelWindowClass { public: Unknown5E6290Class(SClient *client, int a); char Unknown9C[0xC4 - 0x9C]; };	// NAME 1: neutral (D10): 0x5E6290, 0xC4 bytes (requests 6, 7, 9)  // NAME 1 [a]: neutral
class Unknown5E4160Class : public UIChatPanelWindowClass { public: Unknown5E4160Class(SClient *client); char Unknown9C[0x11C - 0x9C]; };	// NAME 1: neutral (D10): 0x5E4160, 0x11C bytes (request 10, kept at +0xE8 too)

// pdaint.cpp's PDA (SClient +0x1284), as far as 0x565F80 calls it
class UIChatPanelPDAClass { public: void Unknown_695F40(); };	// NAME 1: neutral (D10): 0x695F40

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
	virtual void Unknown_90(TextureClass *texture);		// +0x90 (0x56C180: the chat window's icon)  // NAME 1 [texture]: neutral

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
	InterfaceSortScopeClass sort(this);
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

// NOT EXACT: register (the client copies the lag light's table pointer to edx for Is_Hidden, ebx in ours; then eax / edx swapped at As_Space_Client); 0x56C0C0 is out of line through auto_inline(off), as the client
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
	bool changed = false;
	SClient *sclient = Client->As_Space_Client();
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

// NOT EXACT: inline-budget (in the inlined 0x5698F0 the client calls RefCountPtr's operator=(NULL) 0x56CC30 out of line and inlines the copy; ours the reverse; 249 instructions vs 240)
// 0x563280
bool UIChatPanelClass::Unknown_68(int key)
{
	SClient *sclient = Client->As_Space_Client();
	bool modifier = (Unknown_BE5F34 & 0x70) != 0;
	switch (key) {
		case 1:
			Unknown_564030(true);
			return(true);
		case 2:
			Unknown_564030(false);
			return(true);
		case 3:
			if (modifier) {
				if (ChatInt) {
					ChatInt->Unknown_A8();
					return(true);
				}
			} else {
				Unknown_56C0C0();
			}
			return(true);
		case 4:
			sclient->Unknown_750800();
			if (UnknownA4[4]) UnknownA4[4]->Unknown_40(false);
			return(true);
		case 5:
		case 6:
			Unknown_56BEF0();
			return(true);
		case 16:
			if (!Unknown_7CA2D0()->Unknown34) {
				Unknown_5685C0();
				UnknownF8 = Create_Get(new Unknown5DAE90Class(Client->As_Space_Client(), 6, "Online Manual", 0));
				UnknownF8->Unknown_54();
				UnknownF8->Unknown_7C(View, 1);
				UnknownEC = UnknownF8.Peek();
			}
			return(true);
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			Unknown_5698F0(key);
			return(true);
		case 15:
			SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
			Unknown_569A00();
			if (UnknownD4) UnknownD4 = NULL;
			return(true);
	}
	return(false);
}

// 0x563620
void UIChatPanelClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceSortScopeClass sort(this);
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

// NOT EXACT: inline-budget (the second branch's name copy: the client inlines assign with _Grow out of line, ours expands _Grow; frame 0xC vs 8)
// 0x563910
void UIChatPanelClass::Unknown_563910(unsigned id, bool flag)
{
	SClient *sclient = Client->As_Space_Client();
	if (sclient->Unknown_744B60()) return;
	InterfaceSortScopeClass sort(this);
	ClientObjectClass *target = sclient->MainView->Unknown_5C();
	ClientObjectClass *co = sclient->Find_Object(id);
	if (!co) {
		Unknown118 = 0;
		Unknown11C = 0;
	} else if (target == co || flag) {
		if (Unknown118 != id) {
			Unknown118 = id;
			Unknown110->Unknown_7E4350();
			Unknown110->Unknown_7E3A20(0, Unknown118, 0);
		}
		Unknown11C = id;
		RefCountPtr<AvatarPipUnknown2CClass> avatar = Unknown110->Unknown2C;
		if (avatar) {
			avatar->Unknown_77E3D0(1);
			if (avatar->Get_UnknownF8()) Unknown124 = avatar->Get_UnknownF8()->Name;
		}
		Unknown_564030(Unknown120);
		Unknown267 = false;
	} else {
		if (Unknown118 != id) {
			Unknown118 = id;
			if (Unknown11C) Unknown110->Unknown_7E4350();
			Unknown110->Unknown_7E3A20(0, Unknown118, 0);
			RefCountPtr<AvatarPipUnknown2CClass> avatar = Unknown110->Unknown2C;
			if (avatar) {
				avatar->Unknown_77E3D0(2);
				if (avatar->Get_UnknownF8()) Unknown124 = avatar->Get_UnknownF8()->Name;
			}
			Unknown_564030(Unknown120);
		}
	}
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
		InterfaceSortScopeClass sort(this);
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

// NOT EXACT: inline-budget (the client keeps string::compare and assign out of line in the two searches and the label copy; 396 instructions vs 328)
// 0x565390
void UIChatPanelClass::Unknown_565390(bool back)
{
	int count = Unknown228.size();
	if (count == 0) return;
	int index = 0;
	for (int i = 0; i < count; i++) {
		if (Unknown228[i] == Unknown238) {
			index = i;
			break;
		}
	}
	if (back) {
		if (index > 0) {
			index--;
		} else {
			index = count - 1;
		}
	} else {
		index++;
		if (index >= count) index = 0;
	}
	Unknown238 = Unknown228[index];
	int channel = 0;
	for (ChatChannelStruct *entry = Unknown_B7C530; entry->Name; entry++, channel++) {
		if (std::string(entry->Name) == Unknown238) {
			Unknown148 = atoi(Unknown_B7C530[channel].Number);
			goto found;
		}
	}
	if (Unknown214 == Unknown238) Unknown148 = 4;
found:
	if (ChatInt) {
		char buffer[64];
		if (Unknown148 < 6) {
			if (Unknown148 == 5) {
				Unknown204 = Unknown238;
				_snprintf(buffer, sizeof(buffer), "%s:", Unknown238.c_str());
				ChatInt->Unknown_B4(buffer);
			} else if (Unknown148 == 4) {
				_snprintf(buffer, sizeof(buffer), "Private %s:", Unknown238.c_str());
				ChatInt->Unknown_B4(buffer);
			} else {
				ChatInt->Unknown_B4(Unknown_B7C2B8[Unknown148]);
			}
		}
		int kind = Unknown148;
		if (kind < 6) {
			Unknown144 = kind;
			IntOptionStruct *option = OptionClass::Instance->Find_Int("Chat_Default_Channel");
			if (option) option->Value = kind;
			if (Unknown148 == 4) {
				Unknown1F4 = buffer;
			} else {
				Unknown1F4 = Unknown238;
			}
			StringOptionStruct *label = OptionClass::Instance->Find_String("Channel_Label");
			if (label) label->Value = Unknown1F4.c_str();
			Unknown_5689D0(0);
		}
	}
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
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

// 0x565F80
void UIChatPanelClass::Unknown_565F80()
{
	if (Unknown104 && Unknown104->Unknown98) {
		Unknown104->Unknown_7C(NULL, 0);
		Unknown104 = NULL;
	}
	if (Unknown104) return;
	if (!Unknown108) return;
	switch (Unknown108) {
		case 1:
			Unknown104 = Create_Get(new Unknown5CE240Class(Client->As_Space_Client()));
			break;
		case 2:
			Unknown104 = Create_Get(new Unknown5CE700Class(Client->As_Space_Client(), 1));
			break;
		case 3:
			Unknown104 = Create_Get(new Unknown5C9910Class(Client->As_Space_Client(), 0, 0));
			break;
		case 4:
			Unknown104 = Create_Get(new Unknown5C9910Class(Client->As_Space_Client(), 1, 0));
			break;
		case 5:
			if (Unknown10C) {
				Unknown104 = Create_Get(new Unknown5CEDD0Class(Client->As_Space_Client(), 0, 0));
			} else {
				Unknown104 = Create_Get(new Unknown5CEDD0Class(Client->As_Space_Client(), 0, 1));
			}
			break;
		case 6:
			Unknown104 = Create_Get(new Unknown5CEDD0Class(Client->As_Space_Client(), 1, 0));
			break;
		case 7:
			Unknown104 = Create_Get(new Unknown5CEDD0Class(Client->As_Space_Client(), 2, 0));
			break;
		case 8:
			Unknown104 = Create_Get(new Unknown5CA010Class(Client->As_Space_Client(), 0));
			break;
		case 9:
			Unknown104 = Create_Get(new Unknown5C8870Class(Client->As_Space_Client(), 0));
			break;
		case 10:
			Unknown104 = Create_Get(new Unknown5C8870Class(Client->As_Space_Client(), 1));
			break;
		case 11:
			if (!Unknown_7CA2D0()->Unknown34) {
				Unknown104 = Create_Get(new Unknown5DAE90Class(Client->As_Space_Client(), 5, "Credits", 0));
				UnknownEC = Unknown104.Peek();
			}
			break;
		case 12:
			Unknown_56C7E0();
			Unknown108 = 0;
			return;
	}
	if (Unknown104) {
		Unknown104->Unknown_54();
		Unknown104->Unknown_7C(View, 1);
		SClient *sclient = Client->As_Space_Client();
		if (sclient->PDAInterface) ((UIChatPanelPDAClass *)sclient->PDAInterface)->Unknown_695F40();
	}
	Unknown108 = 0;
}

// 0x566AF0
void UIChatPanelClass::Unknown_566AF0()
{
	if (UnknownF8 && UnknownF8->Unknown98) Unknown_5685C0();
	if (UnknownF8) return;
	if (!UnknownFC) return;
	switch (UnknownFC) {
		case 1:
			UnknownF8 = Create_Get(new Unknown5E9410Class(Client->As_Space_Client(), 0, Unknown214));
			break;
		case 2:
			UnknownF8 = Create_Get(new Unknown5E9410Class(Client->As_Space_Client(), 1, ""));
			break;
		case 3:
			UnknownF8 = Create_Get(new Unknown5EAB40Class(Client->As_Space_Client(), 0));
			break;
		case 4:
			UnknownF8 = Create_Get(new Unknown5EAB40Class(Client->As_Space_Client(), 1));
			break;
		case 5:
			UnknownF8 = Create_Get(new Unknown5E9410Class(Client->As_Space_Client(), 2, ""));
			break;
		case 6:
			UnknownF8 = Create_Get(new Unknown5E6290Class(Client->As_Space_Client(), 0));
			break;
		case 7:
			UnknownF8 = Create_Get(new Unknown5E6290Class(Client->As_Space_Client(), 1));
			break;
		case 8:
			UnknownF8 = Create_Get(new Unknown5E9410Class(Client->As_Space_Client(), 3, Unknown_5682C0()));
			break;
		case 9:
			UnknownF8 = Create_Get(new Unknown5E6290Class(Client->As_Space_Client(), 2));
			break;
		case 10:
			UnknownF8 = Create_Get(new Unknown5E4160Class(Client->As_Space_Client()));
			UnknownE8 = UnknownF8.Peek();
			break;
	}
	if (UnknownF8) {
		UnknownF8->Unknown_54();
		UnknownF8->Unknown_7C(View, 1);
	}
	UnknownFC = 0;
}

// NOT EXACT: inline-budget (the client calls string::compare and list::_Buynode out of line; 183 instructions vs 141)
// 0x567540
bool UIChatPanelClass::Unknown_567540(int arg)
{
	if (!Unknown1E8->Is_Active()) {
		Unknown1EC = arg;
		if (arg) {
			int count = 0;
			Unknown1F0 = false;
			std::list<std::string> names;
			for (int i = 0; i < 5; i++) {
				if (Unknown188[i] == "") break;
				names.push_back(Unknown188[i]);
				count++;
			}
			if (count) {
				((UIChatReplyListClass *)Unknown1E8)->Unknown_56EED0(names, Unknown1EC);
				Unknown1E8->Unknown_58();
				return(true);
			}
			Client->As_Space_Client()->Unknown_74D990(17, "Unable to reply - no one to reply to.", 0);
		}
	}
	return(false);
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

// NOT EXACT: inline-budget (the client calls string::compare out of line in the inlined 0x5680B0 and keeps more of the string helpers out of line; 296 instructions vs 287)
// 0x5679A0
void UIChatPanelClass::Unknown_5679A0(int event, const std::string &channel, const std::string &name, bool flag)
{
	switch (event) {
		case 7:
			if (strcmp(Unknown214.c_str(), channel.c_str()) != 0) Unknown224 = false;
			if (flag) {
				SoundLibraryClass::Play_2D_Sound("success_1.wav", 2);
				Unknown225 = false;
			}
			break;
		case 8:
			if (strcmp(Unknown214.c_str(), channel.c_str()) == 0 && flag) {
				Unknown_5680B0(std::string(""));
				Unknown224 = false;
				SoundLibraryClass::Play_2D_Sound("success_1.wav", 2);
			}
			break;
		case 9:
			Unknown_5680B0(channel);
			UnknownFC = 8;
			SoundLibraryClass::Play_2D_Sound("success_1.wav", 2);
			Unknown224 = true;
			Unknown225 = false;
			break;
		case 23:
			if (strcmp(Unknown214.c_str(), channel.c_str()) == 0 && flag) Unknown224 = true;
			break;
		case 24:
			if (strcmp(Unknown214.c_str(), channel.c_str()) == 0 && flag) Unknown224 = false;
			break;
	}
}

// NOT EXACT: inline-budget (the client builds the two empty strings differently: case 2 expands the constructor with _Grow / _Eos, case 10 calls assign; 185 instructions vs 169)
// 0x567DF0
void UIChatPanelClass::Unknown_567DF0(int code)
{
	switch (code) {
		case 12:
			UnknownFC = 6;
			Unknown225 = false;
			return;
		case 13:
			UnknownFC = 7;
			break;
		case 2:
			UnknownFC = 4;
			Unknown_5680B0(std::string(""));
			break;
		case 10:
			UnknownFC = 3;
			Unknown_5680B0(std::string(""));
			break;
		default:
			return;
	}
	SoundLibraryClass::Play_2D_Sound("cost.wav", 2);
	Unknown225 = false;
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

#pragma auto_inline(off)	// the client calls 0x5685C0 out of line (0x56C0C0, slot +0x68)
// 0x5685C0
void UIChatPanelClass::Unknown_5685C0()
{
	if (UnknownF8) {
		if (Unknown10D) {
			Unknown_540A20()->Unknown_56CE40(UnknownF8.Peek(), this, &UIChatPanelClass::Unknown_56C7B0, UIChatF8EventClass());
			Unknown10D = false;
		}
		UnknownF8->Unknown_7C(NULL, 0);
		UnknownF8 = NULL;
		UnknownD8 = NULL;
		UnknownDC = NULL;
		UnknownE0 = NULL;
		UnknownE4 = NULL;
		UnknownE8 = NULL;
		UnknownEC = NULL;
	}
}
#pragma auto_inline(on)

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

// NOT EXACT: inline-budget (the client expands the channel-name temporary's constructor with _Grow and calls _Tidy, and keeps the options pointer and this on the stack; 173 instructions vs 178)
// 0x56B700
void UIChatPanelClass::Unknown_56B700()
{
	OptionClass *options = OptionClass::Instance;
	if (options) {
		for (ChatChannelStruct *channel = Unknown_B7C530; channel < &Unknown_B7C530[17]; channel++) {
			if (channel->Name && atoi(channel->Number) == 5 && options->Get_Bool(channel->Option)) {
				ClientChatRequestPacketClass packet(6, ((ClientClass *)Client)->Unknown1138);
				packet.Unknown14 = std::string(channel->Name);
				packet.Unknown24 = std::string(" ");
				((ClientClass *)Client)->Send_Packet(&packet);
			}
		}
		if (options->Get_Bool("RestrictedStatus")) {
			((ClientClass *)Client)->Send_Packet(&ClientChatRequestPacketClass(0x1C, ((ClientClass *)Client)->Unknown1138));
		} else {
			((ClientClass *)Client)->Send_Packet(&ClientChatRequestPacketClass(0x1D, ((ClientClass *)Client)->Unknown1138));
		}
	}
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



















#pragma auto_inline(off)	// the client calls 0x56C0C0 out of line (slot +0x60, slot +0x68)
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
#pragma auto_inline(on)


// NOT EXACT: frame (0x18 vs the client's 0x1C: the client keeps &ChatInt in edi and calls RefCountPtr's operator=(NULL) 0x56CD50 out of line; 349 instructions vs 346)
// 0x56C180
void UIChatPanelClass::Unknown_56C180(int window)
{
	bool active = Active;
	if (ChatInt) {
		Unknown_540A20()->Unknown_56CEC0(ChatInt.Peek(), this, &UIChatPanelClass::Unknown_56C730, UIChatIntEventClass());
		ChatInt->Unknown_7C(NULL, 0);
		ChatInt = NULL;
	}
	Unknown134 = window;
	switch (window) {
		case 1:
			ChatInt = Create_Get(new UIChatIntClass(Client->As_Space_Client(), Unknown13C, Unknown140, 0));
			break;
		case 2:
			ChatInt = Create_Get(new UIChatIntClass(Client->As_Space_Client(), Unknown13C, Unknown140, 1));
			break;
		case 3:
			ChatInt = Create_Get(new UIChatIntClass(Client->As_Space_Client(), Unknown13C, Unknown140, 2));
			break;
		default:
			G_ASSERT(false);
			Unknown134 = 0;
			break;
	}
	if (ChatInt) {
		UnknownEventDispatcherClass *dispatcher = Unknown_540A20();
		dispatcher->Unknown_4F9740(ChatInt.Peek(), new UIChatIntHandlerClass(this, &UIChatPanelClass::Unknown_56C730));
		ChatInt->Unknown_5417D0(this);
		ChatInt->Unknown_54();
		ChatInt->Unknown_7C(View, active);
		Unknown_5646A0(Unknown148);
	}



	const char *icon_name = Unknown_B7C2E0[window];
	FAIL_IF(!icon_name) return;

	RefCountPtr<TextureClass> texture = Create_Get(Unknown_BFAEE0->Get_Texture(icon_name, 1, 0, 1));
	FAIL_IF(!texture) return;
	UnknownA4[3]->Unknown_90(texture.Peek());
}

// 0x56C6A0
void UIChatPanelClass::Unknown_56C6A0()
{
	if (ChatInt) {
		Unknown_540A20()->Unknown_56CEC0(ChatInt.Peek(), this, &UIChatPanelClass::Unknown_56C730, UIChatIntEventClass());
		ChatInt->Unknown_7C(NULL, 0);
		ChatInt = NULL;
	}
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

// 0x56C7B0
void UIChatPanelClass::Unknown_56C7B0(const UIChatF8EventClass &event, InterfaceClass *sender)
{
	if (sender == UnknownF8.Peek() && ChatInt) ChatInt->Unknown_571430();
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
** The asserts sit on the client's lines (D25); the functions not written yet (the constructor, destructor, slot
** +0x54, 0x5646A0, 0x5689D0, 0x569A00, 0x56C7E0) leave their lines blank. docs/data/asserts.txt files the strings
** 0xB7CB6C-0xB7CD30 (if_chat*.tga, fa_tsi05b.ini, TextClick03.wav, SR_scifielectronic42.wav, the NUM_COMBO assert)
** under ui_buff.cpp: they are this file's (0x5629FC reads fa_tsi05b.ini; the NUM_COMBO assert passes this file's
** __FILE__).
**
** Handlers: the chat window's and the +0xF8 window's events are routed through intrface.cpp's dispatcher (0x540A20
** returns a function-local static); its member templates (0x56CE40 / 0x56CEC0 remove, 0x4F9740 adds a 0x10-byte
** handler object, 0x56D0F0) are declared here as plain members, which compile to the same calls. The handler's last
** argument is a sample of the event type: a zeroed UIChatIntEventClass, an empty UIChatF8EventClass.
**
** Levers that made functions exact: inline accessors for the font owner (+0x84) and the gadget's +0x30 (0x563DC0,
** 0x56BAA0, 0x562F50), a local for View (0x562F50), the branch order that gives the client's EH states (0x563910,
** 0x566AF0's tail test), the table walked by index (0x565120), #pragma auto_inline(off) on 0x5685C0 and 0x56C0C0
** (the client calls them out of line everywhere), and case 5's test written on +0x10C itself (0x565F80).
*/
