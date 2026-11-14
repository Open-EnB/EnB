// Earth & Beyond client (reconstruction): app\client\ui_petition.cpp, the petition window ('hsi01a'). No string
// names the file (NAME 1, provisional: D137). Notes at the end.
#include <string.h>
#include "ui_petition.h"
#include "ui_petition_player.h"
#include "ui_petition_stuck.h"
#include "ui_popup_browser.h"
#include "client.h"
#include "unknown_66b770.h"
#include "..\audio\soundlib.h"

bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp): a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.h's

// The buttons' colours (initialisers 0x5D3320 / 0x5D3350 / 0x5D3380)
static Vector3 Unknown_BE5D58(0.0f, 0.66f, 0.0f);		// NAME 1: neutral (D10)
static Vector3 Unknown_BE5D48(0.99f, 0.99f, 0.33f);	// NAME 1: neutral (D10)
static Vector3 Unknown_BE5D38(0.66f, 0.0f, 0.0f);		// NAME 1: neutral (D10)

// The buttons Unknown_54 makes (ids 1-7): their models' names (also their tooltips), font styles and colours
static const char *Unknown_B83CD0[8] = { "EXIT2", "KNOWLEDGE", "BUG", "BILLING", "GAMEPLAY", "STUCK", "PLAYER", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B83CF0[8] = { NULL, "KNOWLEDGE_hsi01a", "BUG_hsi01a", "BILLING_hsi01a", "GAMEPLAY_hsi01a", "STUCK_hsi01a", "PLAYER_hsi01a", NULL };	// NAME 1: neutral (D10)
static const Vector3 *Unknown_B83D10[8] = { NULL, &Unknown_BE5D58, &Unknown_BE5D48, &Unknown_BE5D48, &Unknown_BE5D38, &Unknown_BE5D38, &Unknown_BE5D38, NULL };	// NAME 1: neutral (D10)

// 0x5D33B0
UIPetitionClass::UIPetitionClass(SClient *client) :
	UIPetitionAvatarPipClass(client)
{
	memset(Buttons, 0, sizeof(Buttons));


}

// 0x5D3450
UIPetitionClass::~UIPetitionClass()
{
}

// 0x5D34F0
void UIPetitionClass::Unknown_54()
{
	if (Unknown_98("hsi01a", "fa_hsi01a.ini")) {
		for (int i = 1; i < 8; i++) {
			GadgetClass *button = Add_Button(i, Unknown_B83CD0[i - 1], Unknown_B83CF0[i - 1]);	// NAME 1: neutral (D10)
			Buttons[i] = button;
			GadgetClass::Set_Tooltip(button, "UI_PETITION", Unknown_B83CD0[i - 1]);
			((Unknown_66B770Class *)button)->Unknown60 = (((Unknown_66B770Class *)button)->Unknown60 & ~3) | 4;
			((Unknown_66B770Class *)button)->Unknown5C_03 = false;
			const Vector3 *color = Unknown_B83D10[i - 1];	// NAME 1: neutral (D10)
			RenderObjClass *robj = ((Unknown_66B770Class *)button)->RObj;	// NAME 1: neutral (D10)
			if (color) {
				Unknown_496110(robj, color->X, color->Y, color->Z);
			}
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

// 0x5D36D0
void UIPetitionClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	if (Page) {
		Page->Unknown_7C(view, arg);
	}
}

// 0x5D3710
void UIPetitionClass::Unknown_60()
{
	Unknown_5D3F80();
	UIPetitionAvatarPipClass::Unknown_60();
}

// 0x5D3800
void UIPetitionClass::Unknown_64()
{
	Unknown_5D3F80();
}

// 0x5D38F0
bool UIPetitionClass::Unknown_68(int key)
{
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
			if (!Page) {
				Unknown_5C();
				Page = Create_Get(new UIPopupBrowserClass((SClient *)((ClientClass *)Client)->As_Space_Client(), 0, "Knowledgebase", NULL));
				Page->Unknown_54();
				Page->Unknown_7C(View, 1);
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			}
			break;
		case 3:
			if (!Page) {
				Unknown_5C();
				Page = Create_Get(new UIPopupBrowserClass((SClient *)((ClientClass *)Client)->As_Space_Client(), 2, "Bug Submission", NULL));
				Page->Unknown_54();
				Page->Unknown_7C(View, 1);
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			}
			break;
		case 4:
			if (!Page) {
				Unknown_5C();
				Page = Create_Get(new UIPopupBrowserClass((SClient *)((ClientClass *)Client)->As_Space_Client(), 1, "Tech Support", NULL));
				Page->Unknown_54();
				Page->Unknown_7C(View, 1);
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			}
			break;
		case 5:
			if (Buttons[5]) {
				GadgetClass::Set_Tooltip(Buttons[5], "UI_PETITION", "GAMEPLAY2");
			}
		case 6:
			if (Page) {
				return(true);
			}
			Page = Create_Get(new UIPetitionStuckClass((SClient *)((ClientClass *)Client)->As_Space_Client(), (key == 6) ? 2 : 1));
			Page->Unknown_5417D0(this);
			Page->Unknown_54();
			Page->Unknown_88(InterfaceMesh, "TRANS_BONE");
			Page->Unknown_7C(View, 1);
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5D4070(key);
			return(true);
		case 7:
			if (Page) {
				return(true);
			}
			Unknown_5C();
			Page = Create_Get(new UIPetitionPlayerClass((SClient *)((ClientClass *)Client)->As_Space_Client()));
			Page->Unknown_5417D0(this);
			Page->Unknown_54();
			Page->Unknown_7C(View, 1);
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			break;
		default:
			return(false);
	}
	return(true);
}

// 0x5D3F80
void UIPetitionClass::Unknown_5D3F80()
{
	if (Page && Page->Closed) {
		Page->Unknown_7C(NULL, 0);
		if (Page->Closed == 2) {
			Unknown_5C();
			Closed = 2;
		} else if (!Active) {
			Unknown_58();
		}
		Page = NULL;
		Unknown_5D40B0();
	}
}

// 0x5D4070
void UIPetitionClass::Unknown_5D4070(int key)
{
	for (int i = 2; i <= 7; i++) {
		Buttons[i]->Unknown_40(i == key);
	}
}

// 0x5D40B0
void UIPetitionClass::Unknown_5D40B0()
{
	for (int i = 2; i < 8; i++) {
		Buttons[i]->Unknown_40(true);
	}
	if (Buttons[5]) {
		GadgetClass::Set_Tooltip(Buttons[5], "UI_PETITION", "GAMEPLAY");
	}
}

/*
**	Notes.
**	Object 0x5D3320-0x5D4147: the colours' initialisers 0x5D3320 / 0x5D3350 / 0x5D3380 (the previous object's
**	adjustor thunk 0x5D3310 precedes them), the constructor 0x5D33B0, the deleting destructor
**	0x5D3420, the functions in the client's order, the guarded std::ctype<unsigned short>::id initialiser 0x5D40F0
**	(the object's <string>), then the adjustor thunks 0x5D4130 / 0x5D4140 of the second and third tables;
**	ui_petition_avatar_pip.cpp's 0x5D4150 follows. No string names the file: the name follows the tooltips' section
**	'UI_PETITION' and the link order (ui_petition.cpp sorts before ui_petition_avatar_pip.cpp). The pattern of
**	ui_petition_entry.cpp's window and ui_option_quit.cpp's buttons.
*/
