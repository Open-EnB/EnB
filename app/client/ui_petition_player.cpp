// Earth & Beyond client (reconstruction): app\client\ui_petition_player.cpp, the petition's players window ('hsi04a').
// No string names the file (NAME 1, provisional: D137). Notes at the end.
#include <string.h>
#include "ui_petition_player.h"
#include "ui_petition_player_entry.h"
#include "client.h"
#include "unknown_66b770.h"
#include "..\audio\soundlib.h"

bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp): a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.h's

// The buttons Unknown_54 makes (ids 1-6): their models' names, font styles and tooltips
static const char *Unknown_B840FC[7] = { "exit2", "button_01", "button_02", "button_03", "button_04", "button_05", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B84118[7] = { NULL, "BAD_NAME_hsi04a", "CHAT_HARASS_hsi04a", "GAME_HARASS_hsi04a", "SPAM_hsi04a", "CHEATING_hsi04a", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B84134[7] = { "EXIT", "NAME", "HARASS", "GAME", "SPAM", "CHEATING", NULL };	// NAME 1: neutral (D10)

// 0x5D5740
UIPetitionPlayerClass::UIPetitionPlayerClass(SClient *client) :
	UIPetitionAvatarPipClass(client)
{
	memset(Buttons, 0, sizeof(Buttons));


}

// 0x5D57E0
UIPetitionPlayerClass::~UIPetitionPlayerClass()
{
}

// 0x5D5880
void UIPetitionPlayerClass::Unknown_54()
{
	if (Unknown_98("hsi04a", "fa_hsi04a.ini")) {
		for (int i = 1; i < 7; i++) {
			GadgetClass *button = Add_Button(i, Unknown_B840FC[i - 1], Unknown_B84118[i - 1]);	// NAME 1: neutral (D10)
			Buttons[i] = button;
			GadgetClass::Set_Tooltip(button, "UI_PETITION_PLAYER", Unknown_B84134[i - 1]);
			((Unknown_66B770Class *)button)->Unknown60 = (((Unknown_66B770Class *)button)->Unknown60 & ~3) | 4;
			((Unknown_66B770Class *)button)->Unknown5C_03 = false;
			RenderObjClass *robj = ((Unknown_66B770Class *)button)->RObj;	// NAME 1: neutral (D10)
			Unknown_496110(robj, 0.33f, 0.33f, 0.99f);
		}
		Vector3 offset(0.0f, 0.0f, 0.0f);	// NAME 1: neutral (D10)
		RefCountPtr<RenderObjClass> frame = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("frame04"));	// NAME 1: neutral (D10)
		if (frame) {
			offset = frame->Get_Transform().Get_Translation();
		}
		Add_Back_Gadget(1, offset);
		UIPetitionAvatarPipClass::Unknown_54();
	}
}

// 0x5D5A50
void UIPetitionPlayerClass::Unknown_58()
{
	UIPetitionAvatarPipClass::Unknown_58();
}

// 0x5D5A70
void UIPetitionPlayerClass::Unknown_5C()
{
	UIPetitionAvatarPipClass::Unknown_5C();
}

// 0x5D5A90
void UIPetitionPlayerClass::Unknown_60()
{
	if (Page && Page->Closed) {
		Page->Unknown_7C(NULL, 0);
		if (Page->Closed == 2) {
			Unknown_5C();
			Closed = 2;
		}
		Page = NULL;
		Unknown_5D5F40();
	}
	UIPetitionAvatarPipClass::Unknown_60();
}

// 0x5D5B50
void UIPetitionPlayerClass::Unknown_64()
{
}

// 0x5D5B70
bool UIPetitionPlayerClass::Unknown_68(int key)
{
	int type;	// NAME 1: neutral (D10)
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (Page) {
				Page->Unknown_94();
			} else {
				Unknown_5C();
				Closed = 2;
			}
			return(true);
		case 2:
			if (Page) {
				return(true);
			}
			type = 3;
			break;
		case 3:
			if (Page) {
				return(true);
			}
			type = 4;
			break;
		case 4:
			if (Page) {
				return(true);
			}
			type = 5;
			break;
		case 5:
			if (Page) {
				return(true);
			}
			type = 6;
			break;
		case 6:
			if (Page) {
				return(true);
			}
			type = 7;
			break;
		default:
			return(false);
	}
	Page = Create_Get(new UIPetitionPlayerEntryClass((SClient *)((ClientClass *)Client)->As_Space_Client(), type));
	Page->Unknown_5417D0(this);
	Page->Unknown_54();
	Page->Unknown_88(InterfaceMesh, "TRANS_BONE");
	Page->Unknown_7C(View, 1);
	Unknown_5D5F00(key);
	return(true);
}

// 0x5D5EC0
void UIPetitionPlayerClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	if (Page) {
		Page->Unknown_7C(view, arg);
	}
}

// 0x5D5F00
void UIPetitionPlayerClass::Unknown_5D5F00(int key)
{
	for (int i = 2; i <= 6; i++) {
		Buttons[i]->Unknown_40(i == key);
	}
}

// 0x5D5F40
void UIPetitionPlayerClass::Unknown_5D5F40()
{
	for (int i = 2; i < 7; i++) {
		Buttons[i]->Unknown_40(true);
	}
}

/*
**	Notes.
**	Object 0x5D5740-0x5D6027: the constructor 0x5D5740, the deleting destructor 0x5D57B0, the functions in the client's
**	order, Create_Get<UIPetitionPlayerEntryClass> 0x5D5F70, ~RefCountPtr<UIPetitionPlayerEntryClass> 0x5D5FA0 (the
**	unwind's), the guarded std::ctype<unsigned short>::id initialiser (the object's <string>), then the adjustor thunks
**	0x5D6010 / 0x5D6020; ui_petition_entry.cpp's 0x5D5730 precedes it, ui_petition_player_entry.cpp's 0x5D6030 follows.
**	No string names the file: the name follows the tooltips' section 'UI_PETITION_PLAYER' and the link order. The
**	pattern of ui_petition.cpp's window.
*/
