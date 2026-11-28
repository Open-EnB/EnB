// Earth & Beyond client (reconstruction): app\Client\ui_avsel.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "sclient.h"
#define __CHARVIEW_AVSEL_MEMBERS	// charview.h's and avatar.h's accessors for this file (tsk_login.h includes charview.h)
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "camera.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "dx8renderer.h"
#include "vector4.h"
#include "tutorial_manager.h"
#include "ui_avsel.h"
#include "..\shared\auth_product.h"
// gadget.cpp's gadget: gadget.h, and gd_robj.h's gadget its slot +0x08 answers with
#include "gadget.h"
#include "gd_robj.h"



































#include "gd_uimesh.h"







bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight (0, 0, 0 off: 0x55E070)  // NAME 1 [robj, a, b, c]: as ui_group.cpp's
bool Unknown_496490(RenderObjClass *robj, bool unknown);	// 0x496490 (ro_funcs.cpp, cdecl: 0x55E070 highlights the avatar under the pointer)  // NAME 1 [robj, unknown]: neutral
void Unknown_42ECF0(float *x, float *y);				// 0x42ECF0 (cdecl): the pointer's position, 0..1  // NAME 1 [x, y]: as scriptedmovie.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
// (FlashingGadgetEvent, the client's RTTI name 0xB7C068: gadget.h)






// fontmanager.h's FontManagerClass at 0xBE67C4 and its text style; fontdatabase.h's FontStringFilesClass at 0xBE67BC
// fontmanager.h (with fontdatabase.h; its 0xBE67B8 is tsk_game.h's since P-H12-3)

#include "fontmanager.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4












// local_interface_sort.cpp's sort object (8 bytes; as ui_cpit.cpp declares it)
#include "local_interface_sort.h"	// InterfaceSortScopeClass (0x541980 / 0x6E18B0)








// The control keys event the constructor registers (0x10 bytes) and what InterfaceClass +0x4C is (as ui_manu.cpp's)
#include "control_key_list.h"	// ControlKeyListClass: InterfaceClass +0x4C (Add 0x721E60, Remove 0x721EF0), keyed by kernel.h's KernelEventClass










// charview.h's CharacterViewClass (the view's slot +0x28), avatar.h's AvatarClass (the five avatars)
// and env_map_manager.h's EnvironmentMapManagerClass (the view's +0x3A4)
#include "charview.h"
#include "env_map_manager.h"





































































































// The window's FlashingGadgetEvent callback: SignalBaseClass::MemberCallback (vtable 0xAEFAEC over 0xAEDB58; its
// members out of line after the galaxy list's: the destructor 0x560250, 0x560270, 0x560290, 0x5602B0, 0x560330 and
// the deleting destructor 0x560390)
#include "unknown_signalbase.h"
typedef SignalBaseClass::MemberCallback<GadgetClass, AvatarSelectionInterface, void (AvatarSelectionInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> UIAvselCallbackClass;	// NAME 1 [UIAvselCallbackClass]: neutral (D10)
typedef SignalBaseClass::CallbackClass UIAvselCallbackBaseClass;	// NAME 1 [UIAvselCallbackBaseClass]: neutral (D10)

// (the signal map: unknown_signalbase.h's SignalMap, its Add / Remove)





// (0x65ECE0: gadget.h's GadgetClass::Get_Signal_Map)

// The asset manager (*0xBFAEE0)
#include "assetmgr.h"	// WW3DAssetManager: 0xBFAEE0 is WW3DAssetManager::TheInstance (its constructor 0x91C2C0 stores it)







#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time / Get_Sync_Time (0xBFAE3C / 0xBFAE40: WW3D::SyncTime / PreviousSyncTime, written by WW3D::Sync 0x918200, read inline)

// The button models and their texts (0xB7B8E0, 0xB7B904: the five slots, delete, continue / confirm, cancel / exit)
static const char *Unknown_B7B8E0[9] = { "name00", "name01", "name02", "name03", "name04", "delete", "continue", "cancel", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7B904[9] = { "fe12_Name_00", "fe12_Name_01", "fe12_Name_02", "fe12_Name_03", "fe12_Name_04", "fe12_Delete", "fe12_confirm", "fe12_exit", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7B928[3] = { "Race: Terran", "Race: Jenquai", "Race: Progen" };	// NAME 1: neutral (D10)
static const char *Unknown_B7B934[9] = { "Class: Enforcer", "Class: Tradesman", "Class: Scout", "Class: Defender", "Class: Seeker", "Class: Explorer", "Class: Warrior", "Class: Privateer", "Class: Sentinel" };	// NAME 1: neutral (D10)
static const char *Unknown_B7B958[2] = { "CANCEL_BUTTON", "OK_BUTTON" };	// NAME 1: neutral (D10)
static const char *Unknown_B7B960[2] = { "CANCEL_BUTTON_BUDDYREF1", "OK_BUTTON_BUDDYREF1" };	// NAME 1: neutral (D10)

class LoginTaskClass;
// (LoginTaskClass's two accessors slot +0x68 calls out of line, 0x55D860 and 0x55D880 in this file's window: tsk_login.h)






// The edit box (the referral's name): textgadg.cpp's UnknownTextGadgetClass (0x80 bytes, constructor 0x6AA810); its cursor
// +0x4C is fontsys_cursor.h's FontSysCursor (Rectangle3DClass::Set_Color 0x6E2A20)
#include "textgadg.h"
#include "fontsys_cursor.h"














extern const int Unknown_B08CAC; extern const int Unknown_B08CBC; extern const int Unknown_B08CC0;	// NAME 1: neutral (D10): what 0x55F760's edit box gets (or'ed)












// 0x55C650
AvatarSelectionInterface::AvatarSelectionInterface(UIViewClass *view, LayerClass &layer) :	// a reference: retail's ...@PAVUIViewClass@@AAVLayerClass@@@Z (thunk order, IM3 / N1)
	InterfaceClass(view, (int)layer.Peek_Scene()),
	Unknown084(NULL), Unknown088(NULL), Unknown08C(0), Unknown090(-1),
	Unknown094(false), Unknown095(false), Unknown096(false), Unknown097(false), Unknown098(false),
	Unknown0EC(0), Unknown0F0(0), Unknown0F4(-1),
	Unknown0F8(false), Unknown0F9(false), Unknown0FA(false), Unknown0FB(false), Unknown0FC(false), Unknown0FD(false),
	Unknown100(-1), Layer(&layer), Unknown118(NULL), Unknown120(false)
{
	InterfaceSortScopeClass sort(this);
	memset(Unknown09C, 0, sizeof(Unknown09C));
	int i;
	for (i = 0; i < 2; i++) {
		Unknown10C[i] = NULL;
	}
	Unknown114 = NULL;
	for (i = 0; i < 5; i++) {
		Unknown0BC[i] = NULL;
		Unknown0D0[i] = NULL;
		Unknown0E4[i] = false;
	}
	Unknown084 = (CharacterViewClass *)View->Unknown_28();
	Unknown088 = (LoginTaskClass *)Unknown084->Unknown14->Unknown_08();
	Unknown084->Unknown_67A250();
	Unknown084->Unknown_67ACE0(0);
	Unknown084->Unknown_67AB20(0, "FE12A");
	RenderObjClass *model = Unknown084->ViewRObjs[0];
	Unknown084->Unknown_67AB20(1, "FE01C");
	Unknown084->Unknown_67AB20(2, "FE_ROOM");
	Unknown084->Unknown_67B7A0(1);
	Unknown084->Unknown_67A290(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -90.0f, -90.0f), 45.0f);
	Unknown084->Unknown068->Set_Clip_Planes(1.0f, 500.0f);
	Unknown_BE67BC->Load("FA_FE12A.ini");
	Unknown_BE67C4->Add(model);
	for (i = 0; i < 8; i++) {
		char name[64];
		strcpy(name, Unknown_B7B8E0[i]);
		RenderObjClass *robj = model->Get_Sub_Object_By_Name(name);
		Unknown_496AE0(robj, 0);
		GadgetClass *gadget = new ButtonGadgetClass(robj, layer, 0x55, 3);
		Unknown09C[i] = gadget;
		Unknown64.Unknown_6870E0(gadget, i + 1, View, 1, 1);
		robj->Release_Ref();
	}
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x1B), (void *)10);
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x0D), (void *)7);
	if (Unknown088->Unknown1138) {
		RenderObjClass *robj = model->Get_Sub_Object_By_Name("click_me");
		Unknown_496AE0(robj, 0);
		Unknown118 = new ButtonGadgetClass(robj, layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown118, 0x11, View, 1, 1);
		robj->Release_Ref();
	} else {
		Unknown118 = NULL;
		Unknown_55FE50(false);
	}
	Unknown_55E370(1);
	if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
		Unknown084->Unknown_67AE20(1);
	}
	Unknown0FC = true;
	Unknown_55EF90();
	Unknown084->Unknown3A4->Unknown_760770(100001, true);
	void (AvatarSelectionInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarSelectionInterface::Unknown_55F6D0;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	notifier->connect(0, new UIAvselCallbackClass(this, function));
}

// 0x55CCC0
AvatarSelectionInterface::~AvatarSelectionInterface()
{
	Unknown_5C();
	int i;
	for (i = 0; i < 8; i++) {
		if (Unknown09C[i]) {
			GadgetClass::Destroy(Unknown09C[i]);
			Unknown09C[i] = NULL;
		}
	}
	if (Unknown118) {
		GadgetClass::Destroy(Unknown118);
		Unknown118 = NULL;
	}
	for (i = 0; i < 5; i++) {
		if (Unknown0D0[i]) {
			GadgetClass::Destroy(Unknown0D0[i]);
			Unknown0D0[i] = NULL;
		}
		if (Unknown0BC[i]) {
			Unknown0BC[i]->Remove();
			Unknown0BC[i]->Release_Ref();
			Unknown0BC[i] = NULL;
		}
	}
	Unknown_BE67C4->Remove(Unknown084->Peek_Model());
	Unknown_BE67BC->Unload("FA_FE12A.ini");
	Unknown084->Unknown_67A9B0(0);
	void (AvatarSelectionInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarSelectionInterface::Unknown_55F6D0;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	UIAvselCallbackClass callback(this, function);
	notifier->disconnect(0, callback);
}

// 0x55CEA0
void AvatarSelectionInterface::Unknown_60()
{
	Unknown_55E070();
	Unknown_55D9C0();
	if (Unknown114) {
		Unknown114->Unknown_6AAD10();
	}
	float elapsed = (WW3D::Get_Frame_Time()) * 0.001f;
	if (Unknown0F0 > 0.0f && (Unknown0F0 -= elapsed) < 0.0f) {
		Unknown0F0 = 0.0f;
		if (!Unknown0F9) {
			Unknown_55ED20();
		}
	}
	if (Unknown0EC > 0.0f) {
		Unknown0EC -= elapsed;
		if (Unknown0EC < 0.0f) {
			Unknown0EC = 0.0f;
			if (Unknown0F4 > -1) {
				Unknown088->Selected = Unknown0F4;
				Unknown088->Unknown_50();
				Unknown0F4 = -1;
				Unknown0F0 = 30.0f;
				Unknown_55EF90();
				return;
			}
		} else if (Unknown0EC < 0.7f && Unknown0F4 > -1) {
			if (!Unknown0BC[Unknown0F4]->Unknown_7863E0()) {
				Unknown0BC[Unknown0F4]->Unknown_786370(true);
			}
		}
		Unknown_55EF90();
	}
}

// (cases 7 and 8 are if / else arms under one return true each, with their own copy of the FE01C block, and the function ends in return false: VC6 then loads 1 per case, holds -1 in esi and the key in ebx, and cross-jumps the copies as the client)
// 0x55D060
bool AvatarSelectionInterface::Unknown_68(int key)
{
	if (Unknown0F4 > -1 || Unknown0F0 != 0.0f) {
		Unknown_55EF90();
		return true;
	}
	if (Unknown0F8 || Unknown0F9 || Unknown0FA || Unknown0FB || Unknown0FC || Unknown0FD || Unknown095 || Unknown094) {
		return false;
	}
	if (!BuddyReferralUI) {
		int previous;
		switch (key) {
			case 17:
				Unknown_55EF90();
				Unknown_55F760();
				return false;
			case 1: case 2: case 3: case 4: case 5:
				SoundLibrary::Play_2D_Sound("ButtonClick02.wav", 2);
				if (Unknown096) {
					Unknown0BC[Unknown090]->Unknown_784A50("/normal", 0, 0);
				}
				previous = Unknown090;
				Unknown090 = key - 1;
				Unknown_55ED20();
				if (Unknown090 != previous) Unknown_55E8E0(); return true;
			case 11: case 12: case 13: case 14: case 15:
				SoundLibrary::Play_2D_Sound("ButtonClick02.wav", 2);
				if (Unknown096) {
					Unknown0BC[Unknown090]->Unknown_784A50("/normal", 0, 0);
				}
				previous = Unknown090;
				Unknown090 = key - 11;
				Unknown_55ED20();
				if (Unknown090 != previous) Unknown_55E8E0(); return true;
			case 6:
				SoundLibrary::Play_2D_Sound("Avatar_Delete_Character", 2);
				if (Unknown090 >= 0) {
					Unknown0BC[Unknown090]->Unknown_784A50("/angry please no", 1, 0);
					Unknown084->Get_Unknown034()->Unknown_588A70("YES_NO", 0, "Confirm deletion of the character?", 0x140, 0, 0);
					Unknown097 = true;
					Unknown0F9 = true;
					Unknown_55EF90();
				}
				return true;
			case 7:
				SoundLibrary::Play_2D_Sound("Login_Mouse_Click", 2);
				if (Unknown090 >= 0) {
					Unknown08C = 7;
					Unknown088->Selected = Unknown090;
					if (Unknown0E4[Unknown090]) {

						if (Unknown088->Is_Registered_Character()) {
							TutorialManagerClass::Instance->Write_Completed_File();
							Unknown088->Unknown_5C();
						} else {
							Unknown084->Unknown_5594B0(Unknown088->Get_Avatar_Descs()[Unknown088->Get_Selected()]);
							Unknown084->Set_Unknown244(Unknown088->Unknown088C[Unknown088->Selected]); Unknown084->Get_Unknown1B0(); Unknown084->Get_Unknown1B0(); Unknown084->Get_Unknown1B0();	// the copy's layer and dead reads: the budget then calls 0x55D8A0 as the client
							Unknown084->Unknown_67ACE0(1);
							Unknown084->Unknown1CC[1] = true;
							Unknown084->Unknown1CC[2] = true;
							Unknown088->Go_Next_Step();
						}
					} else {
						if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
							Unknown084->Unknown_67AE60(1);
						}
						Unknown0FD = true;
						Unknown_55EF90();
					}
				}
				return true;
			case 8:
				SoundLibrary::Play_2D_Sound("Login_Mouse_Click", 2);
				if (Unknown096) {
					if (Unknown0BC[Unknown090]) {
						Unknown0BC[Unknown090]->Unknown_784A50("/normal", 0, 0);
					}
					Unknown090 = -1;
					Unknown_8C();
				} else {
					Unknown08C = 8;
					if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
						Unknown084->Unknown_67AE60(1);
					}
					Unknown0FD = true;
					Unknown_55EF90();
				}
				return true;

			case 10:
				SoundLibrary::Play_2D_Sound("Login_Mouse_Click", 2);
				Unknown084->Unknown_67B270();
				Unknown0F9 = true;
				Unknown_55EF90();
				return true;


		}



	} else {
		SoundLibrary::Play_2D_Sound("ButtonClick02.wav", 2);
		switch (key) {
			case 1: case 8:
				Unknown11C = "";
				Unknown_55FBB0();
				Unknown_55ED20();
				return false;
			case 4:
				Unknown114->Unknown_74(true);
				return false;
			case 2:
				{
					char *text = Unknown114->Get_Text();
					Unknown11C = text;
					Unknown11C.Trim();
					delete text;
				}
				if (strcmp(Unknown11C, "") == 0) {
					Unknown084->Get_Unknown034()->Unknown_588A70("OK", 0, "Please enter a valid EA Member Name.", 0x140, 0, 0);
					return false;
				}
				if (stricmp(Unknown11C, Unknown088->Ticket_Info.EA_Customer.c_str()) == 0) {
					Unknown084->Get_Unknown034()->Unknown_588A70("OK", 0, "Sorry, you cannot refer yourself.", 0x140, 0, 0);
					return false;
				}
				Unknown_55FBB0();
				Unknown088->Send_Referrer(Unknown11C);
				Unknown120 = true;
				return false;
		}

	}	return false;
}

// 0x55D9C0
void AvatarSelectionInterface::Unknown_55D9C0()
{
	if (Unknown120 && Unknown088->Referrer_Result) {
		switch (Unknown088->Referrer_Result) {
			case 2:
				Unknown084->Unknown34->Unknown_588A70("OK", 0, "Sorry, you have already been referred by someone.  You cannot be referred more than once.", 0x140, 0, 0);
				break;
			case 3:
				Unknown084->Unknown34->Unknown_588A70("OK", 0, "Sorry, that account does not exist.  Please check the name and try again.", 0x140, 0, 0);
				break;
			case 4:
				Unknown084->Unknown34->Unknown_588A70("OK", 0, "A system error occurred.  Please try again later.", 0x140, 0, 0);
				break;
			case 5:
				Unknown084->Unknown34->Unknown_588A70("OK", 0, "Sorry, your account is about to expire.", 0x140, 0, 0);
				break;
			case 6:
				Unknown084->Unknown34->Unknown_588A70("OK", 0, "Sorry, you cannot refer yourself.", 0x140, 0, 0);
				break;
			case 1:
				Unknown084->Unknown34->Unknown_588A70("OK", 0, "Your referral was registered successfully.  The person who referred you has received credit for your referral.", 0x140, 0, 0);
				break;
		}
		if (!Unknown088->Unknown1138) {
			Unknown118 = NULL;
			Unknown_55FE50(false);
		}
		Unknown_55ED20();
		Unknown120 = false;
	}
	if (Unknown095 || Unknown094) {
		Unknown_55EF90();
	}
	for (int i = 0; i < 5; i++) {
		if (Unknown0BC[i]) {
			Unknown0BC[i]->Unknown_1C4();
		}
	}
	if (Unknown095 && Unknown084->Unknown_67AEA0(1)) {
		Unknown095 = false;
		Unknown096 = true;
		Unknown0BC[Unknown090]->Unknown_784A50("/happy at your service", 1, 0);
		Unknown_55ED20();
	}
	if (Unknown094 && Unknown084->Unknown_67AEA0(1)) {
		Unknown094 = false;
		Unknown_55FE50(true);
		if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-10")) {
			Unknown084->Unknown_67ADD0(1);
		}
		if (Unknown090 < 0) {
			Unknown_55ED20();
			Unknown_55E8E0();
		} else if (Unknown08C != 7 && Unknown08C != 8) {
			if (Unknown0E4[Unknown090]) {
				Unknown_88();
				Unknown_55EF90();
			} else if (Unknown090 < 5 && Unknown098) {
				Unknown08C = 7;
				if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
					Unknown084->Unknown_67AE60(1);
				}
				Unknown0FD = true;
				Unknown_55EF90();
			} else {
				Unknown090 = -1;
				Unknown08C = 0;
				Unknown_55ED20();
			}
		} else {
			if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
				Unknown084->Unknown_67AE60(1);
			}
			Unknown0FD = true;
			Unknown_55EF90();
		}
	}
	if (Unknown088->Unknown001D) {
		Unknown_55E370(false);
		Unknown088->Unknown001D = false;
		Unknown_55EF90();
		Unknown0F0 = 3.0f;
	}
	if (Unknown0FA && Unknown084->Unknown_67AEA0(0)) {
		Unknown0FA = false;
	}
	if (Unknown0FB && Unknown084->Unknown_67AEA0(0)) {
		Unknown0FB = false;
	}
	if (!Unknown0FA && Unknown0FC && Unknown084->Unknown_67AEA0(1)) {
		Unknown0FC = false;
		Unknown_55ED20();
		if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-10")) {
			Unknown084->Unknown_67ADD0(1);
		}
	}
	if (!Unknown0FB && Unknown0FD && Unknown084->Unknown_67AEA0(1)) {
		Unknown084->Unknown_67ACE0(1);
		Unknown084->Unknown1CC[1] = true;
		Unknown084->Unknown1CC[2] = true;
		if (Unknown0F4 > -1) {
			Unknown088->Selected = Unknown0F4;
			Unknown088->Unknown_50();
			Unknown0F4 = -1;
		}
		if (Unknown08C == 7) {
			Unknown088->Go_Next_Step();
		} else {
			Unknown088->Go_Back_Step();
		}
	}
	if (Unknown084->Unknown_67B2B0()) {
		Unknown0F9 = false;
		if (!Unknown095 && !Unknown094) {
			Unknown_55ED20();
		}
	}
	if (Unknown097) {
		int answer = Unknown084->Unknown34->Unknown_5890B0("YES_NO");
		if (answer) {
			Unknown084->Unknown34->Unknown_5C();
			Unknown0F9 = false;
			Unknown097 = false;
			if (answer == 1) {
				Unknown0E4[Unknown090] = false;
				Unknown0BC[Unknown090]->Unknown_784A50("you'll pay for this", 1, 0);
				Unknown0BC[Unknown090]->Unknown_7896B0(1);
				Unknown0EC = 3.2f;
				Unknown0F4 = Unknown090;
				Unknown_8C();
			} else {
				Unknown0BC[Unknown090]->Unknown_784A50("/happy yeah", 1, 0);
				Unknown_55ED20();
			}
		}
	}
	if (Unknown084->Unknown34->Active && !Unknown0F9) {
		Unknown0F9 = true;
		Unknown_55EF90();
	} else if (!Unknown084->Unknown34->Active && Unknown0F9) {
		Unknown0F9 = false;
		if (!Unknown095 && !Unknown094) {
			Unknown_55ED20();
		}
		if (Unknown08C == 7) {
			Unknown08C = 0;
		}
	}
}

// NOT EXACT: register (0.78; the else branch's vtable load in eax, the client's in edx)
// 0x55E070
void AvatarSelectionInterface::Unknown_55E070()
{
	bool highlighted = false;
	int highlight = -1;
	RenderObjClass *robj;
	if (!Unknown0F8 && !Unknown0F9 && !Unknown0FA && !Unknown0FB && !Unknown0FC && !Unknown0FD && Unknown0F4 == -1 &&
		Unknown0F0 == 0.0f && !Unknown095 && !Unknown094 && !BuddyReferralUI) {
		float x, y;
		Unknown_42ECF0(&x, &y);
		for (int i = 0; i < 5; i++) {
			if (Unknown0D0[i] && Unknown0D0[i]->Unknown_30(x, y) && Unknown090 != i && y < 0.85f) {
				highlighted = true;
				for (int j = 0; j < 5; j++) {
					GadgetClass *gadget = Unknown0D0[j];
					if (j == i) {
						if (gadget) {
							robj = gadget->As_RObjGadget()->RObj;
							Unknown_496490(robj, 0);
							highlight = j;
						}
					} else if (gadget) {
						robj = gadget->As_RObjGadget()->RObj;
						Unknown_496110(robj, 0.0f, 0.0f, 0.0f);
					}
				}
			}
		}
	}
	if (!highlighted) {
		Unknown100 = -1;
		for (int i = 0; i < 5; i++) {
			if (Unknown0D0[i]) {
				robj = Unknown0D0[i]->As_RObjGadget()->RObj;
				Unknown_496110(robj, 0.0f, 0.0f, 0.0f);
			}
		}
	} else if (Unknown100 != highlight) {
		Unknown100 = highlight;
		SoundLibrary::Play_2D_Sound("Avatar_Highlight", 2);
	}
}

// 0x55E2E0
void AvatarSelectionInterface::Unknown_55E2E0(int text, const char *string, bool selected)
{
	FontSys *textptr = Unknown_BE67C4->Get_Style(Unknown_B7B904[text], Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Change_Text(string, 0);
	textptr->Unknown_6CE910(selected);
}




// 0x55E370
void AvatarSelectionInterface::Unknown_55E370(bool active)
{
	LoginCharacterClass *chars = Unknown088->Characters;
	AvatarDescriptionClass *descs = Unknown088->Unknown0350;
	G_ASSERT(chars);
	TheDX8MeshRenderer.Invalidate();
	if (!active) {
		InterfaceClass::Unknown_5C();
	}
	for (int i = 0; i < 5; i++) {
		if (!Unknown0E4[i]) {
			if (Unknown0BC[i]) {
				Unknown0BC[i]->Remove();
				Unknown0BC[i]->Release_Ref();
				Unknown0BC[i] = NULL;
			}
			if (Unknown0D0[i]) {
				Unknown64.Unknown_687150(Unknown0D0[i]);
				GadgetClass::Destroy(Unknown0D0[i]);
				Unknown0D0[i] = NULL;
			}
			if (chars[i].ID != 0) {
				Unknown_55E2E0(i, descs[i].Get_Full_Name().c_str(), i == Unknown090);
				Unknown0E4[i] = true;
				AvatarDescriptionClass *desc = new AvatarDescriptionClass(descs[i]);
				Unknown0BC[i] = new AvatarClass(desc, true, false, Unknown084->Get_Unknown3A4());
				if (Unknown0BC[i]) {
					char name[64];
					sprintf(name, "DUDE%02d", i);
					Unknown084->ViewRObjs[1]->Add_Sub_Object_To_Bone(Unknown0BC[i], name);
					Unknown0BC[i]->Unknown_77E3D0(1);
					Unknown0D0[i] = new ButtonGadgetClass(Unknown0BC[i]->Peek_Body_Obj(), *Unknown084->Get_Unknown070(), 0x55, 0);
					Unknown0D0[i]->Unknown2C_0A = true;
					Unknown0D0[i]->Set_Last_Sort_Group(true);
					Unknown64.Unknown_6870E0(Unknown0D0[i], i + 11, View, 1, 1);
				}
				delete desc;
			} else {
				Unknown_55E2E0(i, "New Character", i == Unknown090);
				Unknown0E4[i] = false;
			}
		}
	}
	if (!active) {
		InterfaceClass::Unknown_58();
	}
}

// 0x55E8E0
void AvatarSelectionInterface::Unknown_55E8E0()
{
	AvatarDescriptionClass *descs = Unknown088->Unknown0350;
	Unknown_55ED20();
	for (int i = 0; i < 5; i++) {
		if (Unknown0E4[i]) {
			if (i == Unknown090) {
				if (Unknown096) {
					Unknown_8C();
				} else {
					Unknown_88();
				}
			}
			Unknown_55E2E0(i, descs[i].Get_Full_Name().c_str(), i == Unknown090);
		} else {
			if (i == Unknown090) {
				Unknown08C = 7;
				Unknown088->Selected = Unknown090;
				if (Unknown096) {
					Unknown098 = true;
					Unknown_8C();
				} else {
					if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
						Unknown084->Unknown_67AE60(1);
					}
					Unknown0FD = true;
					Unknown_55EF90();
				}
			}
			Unknown_55E2E0(i, "New Character", i == Unknown090);
		}
	}
}

// 0x55EBD0
void AvatarSelectionInterface::Unknown_88()
{
	if (Unknown090 >= 0 && Unknown0E4[Unknown090]) {
		Unknown_55FE50(false);
		char name[32];
		sprintf(name, "FE01C.FE01C-%d", Unknown090 + 5);
		if (Unknown084->Unknown_67AD30(1, name)) {
			Unknown084->Unknown_67AE20(1);
			Unknown095 = true;
			Unknown_55EF90();
			Unknown0F9 = true;
			Unknown084->Unknown_67AD30(0, "FE12A.FE12A-1");
			Unknown084->Unknown_67AE20(0);
		}
		Unknown_55F140();
	}
}

// 0x55ECB0
void AvatarSelectionInterface::Unknown_8C()
{
	if (Unknown084->Unknown1D4[1].Anim) {
		Unknown084->Unknown_67AE60(1);
		Unknown094 = true;
		Unknown096 = false;
		Unknown_55EF90();
		Unknown0F9 = true;
		Unknown084->Unknown_67AE60(0);
	}
}

// 0x55ED20
void AvatarSelectionInterface::Unknown_55ED20()
{
	int i;
	FontSys *text;
	if (BuddyReferralUI) {
		for (i = 0; i < 2; i++) {
			if (Unknown10C[i]) {
				Unknown10C[i]->Unknown_40(true);
				text = Unknown_BE67C4->Get_Style(Unknown_B7B960[i], BuddyReferralUI.Peek());
				if (text) {
					text->Unknown_Set_2FC(1.0f);
				}
			}
		}
		return;
	}
	for (i = 0; i < 8; i++) {
		if (Unknown09C[i] && !Unknown09C[i]->Is_Enabled()) {
			Unknown09C[i]->Unknown_40(true);
			text = Unknown_BE67C4->Get_Style(Unknown_B7B904[i], Unknown084->Peek_Model());
			if (text) {
				text->Unknown_Set_2FC(1.0f);
			}
		}
	}
	if (Unknown118 && !Unknown118->Is_Enabled()) {
		Unknown118->Unknown_40(true);
		text = Unknown_BE67C4->Get_Style("fe12_ReferralButton", Unknown084->Peek_Model());
		if (text) {
			text->Unknown_Set_2FC(1.0f);
		}
		text = Unknown_BE67C4->Get_Style("fe12_ReferralText", Unknown084->Peek_Model());
		if (text) {
			text->Unknown_Set_2FC(1.0f);
		}
	}
	if (Unknown090 < 0 || !Unknown0E4[Unknown090]) {
		Unknown09C[5]->Unknown_40(false);
		text = Unknown_BE67C4->Get_Style(Unknown_B7B904[5], Unknown084->Peek_Model());
		if (text) {
			text->Unknown_Set_2FC(0.33f);
		}
	}
	if (Unknown090 < 0) {
		Unknown09C[6]->Unknown_40(false);
		text = Unknown_BE67C4->Get_Style(Unknown_B7B904[6], Unknown084->Peek_Model());
		if (text) {
			text->Unknown_Set_2FC(0.33f);
		}
	}
	Unknown0F9 = false;
}

// 0x55EF90
void AvatarSelectionInterface::Unknown_55EF90()
{
	int i;
	FontSys *text;
	if (BuddyReferralUI) {
		for (i = 0; i < 2; i++) {
			if (Unknown10C[i]) {
				Unknown10C[i]->Unknown_40(false);
				text = Unknown_BE67C4->Get_Style(Unknown_B7B960[i], BuddyReferralUI.Peek());
				if (text) {
					text->Unknown_Set_2FC(0.33f);
				}
			}
		}
		return;
	}
	for (i = 0; i < 8; i++) {
		if (Unknown09C[i] && Unknown09C[i]->Is_Enabled()) {
			Unknown09C[i]->Unknown_40(false);
			text = Unknown_BE67C4->Get_Style(Unknown_B7B904[i], Unknown084->Peek_Model());
			if (text) {
				text->Unknown_Set_2FC(0.33f);
			}
		}
	}
	if (Unknown118 && Unknown118->Is_Enabled()) {
		Unknown118->Unknown_40(false);
		text = Unknown_BE67C4->Get_Style("fe12_ReferralButton", Unknown084->Peek_Model());
		if (text) {
			text->Unknown_Set_2FC(0.33f);
		}
		text = Unknown_BE67C4->Get_Style("fe12_ReferralText", Unknown084->Peek_Model());
		if (text) {
			text->Unknown_Set_2FC(0.33f);
		}
	}
}





















































// NOT EXACT: register (0.78; the profession index: the client adds race * 2 to the profession, then race)
// 0x55F140
void AvatarSelectionInterface::Unknown_55F140()
{
	LoginCharacterClass *chars = Unknown088->Characters;
	if (Unknown090 == -1 || !Unknown0E4[Unknown090]) {
		return;
	}
	FontSys *textptr = Unknown_BE67C4->Get_Style("fe12_Race_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	int race = Unknown0BC[Unknown090]->Desc.Race;
	textptr->Change_Text(Unknown_B7B928[race], 0);


	textptr = Unknown_BE67C4->Get_Style("fe12_Prof_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Change_Text(Unknown_B7B934[Unknown0BC[Unknown090]->Desc.Profession + race * 3], 0);



	textptr = Unknown_BE67C4->Get_Style("fe12_Level_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	char level[64];
	sprintf(level, "Level: %d", chars[Unknown090].Level_Combat + chars[Unknown090].Level_Explore + chars[Unknown090].Level_Trade);
	textptr->Change_Text(level, 0);

	ManagedFontClass *font = Unknown_BE67C4->Find(Unknown084->Peek_Model());
	if (font) {
		const char *sector = chars[Unknown090].Sector_Name;
		if (strlen(sector)) {
			char location[0x78];
			_snprintf(location, sizeof(location), "Location: %s", sector);
			font->Unknown_6D0A70("WRAP_fe12_Location_Title", location);
		} else {
			font->Unknown_6D0A70("WRAP_fe12_Location_Title", "");
		}
	}









	std::list<LoginGalaxyClass> galaxies = Unknown088->Galaxies;
	galaxies.size();	// dead reads: they spend the inline budget the client spends here, so the FAIL_IF's ~list
	galaxies.size();	// inlines erase(first, last) and calls it++ 0x5601B0 / erase(it) 0x560160 out of line
	textptr = Unknown_BE67C4->Get_Style("fe12_Server_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Change_Text("", 0);
	for (std::list<LoginGalaxyClass>::iterator it = galaxies.begin(); it != galaxies.end(); it++) {
		if ((*it).ID == chars[Unknown090].Galaxy_ID && strlen((*it).Name)) {
			float capacity = (unsigned int)(*it).Unknown58 * 0.9f;
			if (capacity == 0.0f) {
				capacity = 1.0f;
			}
			float full = (unsigned int)(*it).Unknown54 / capacity * 100.0f;
			char galaxy[0x68];
			if (full >= 100.0f) {
				_snprintf(galaxy, sizeof(galaxy), "Galaxy: %s (FULL)", (*it).Name);
			} else {
				_snprintf(galaxy, sizeof(galaxy), "Galaxy: %s (%.0f%% full)", (*it).Name, full);
			}
			textptr->Change_Text(galaxy, 0);
		}
	}
}











// 0x55F680
void AvatarSelectionInterface::Unknown_55F680()
{
	if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
		Unknown084->Unknown_67AE60(1);
	}
	Unknown0FD = true;
	Unknown_55EF90();
}

// 0x55F6D0
void AvatarSelectionInterface::Unknown_55F6D0(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event)
		return;

	GadgetClass *gadget = event->Gadget;
	FAIL_IF(!gadget)
		return;
	if (gadget->Flash && gadget->Is_Unknown2C_02()) {
		SoundLibrary::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
	}
}












// 0x55F760
void AvatarSelectionInterface::Unknown_55F760()
{
	Unknown_5C();
	Unknown11C = "";
	RenderObjClass *model = Unknown084->Peek_Model();
	BuddyReferralUI = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("BUDDYREF1"));
	G_ASSERT(BuddyReferralUI);
	model->Add_Sub_Object(BuddyReferralUI.Peek());
	Unknown_BE67BC->Load("fc_buddyref1.ini");
	Unknown_BE67C4->Add(BuddyReferralUI);
	for (int i = 0; i < 2; i++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(BuddyReferralUI->Get_Sub_Object_By_Name(Unknown_B7B958[i]));
		Unknown_496AE0(robj.Peek(), 0);
		Unknown10C[i] = new ButtonGadgetClass(robj.Peek(), *Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown10C[i], i + 1, View, 1, 0);
	}
	Unknown114 = new UnknownTextGadgetClass(this, 20, "ENTRY_NAME_BUDDYREF1", BuddyReferralUI.Peek(), Unknown_B08CAC | Unknown_B08CBC | Unknown_B08CC0, 0);
	Unknown114->Unknown_28(Layer);
	Unknown114->Cursor->Set_Color(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	Unknown114->Set_Text("", false);
	Unknown114->Unknown5C = false;
	Unknown114->Unknown3B = true;
	Unknown114->Unknown_74(true);
	Unknown64.Unknown_6870E0(Unknown114, 4, View, 1, 0);
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x0D), (void *)2);
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	InterfaceClass::Unknown_58();
	Unknown_58();
}



















// 0x55FBB0
void AvatarSelectionInterface::Unknown_55FBB0()
{
	Unknown_5C();
	Unknown64.Unknown_687150(Unknown114);
	Unknown114 = NULL;
	for (int i = 0; i < 2; i++) {
		Unknown64.Unknown_687150(Unknown10C[i]);
		Unknown10C[i] = NULL;
	}
	Unknown_BE67C4->Remove(BuddyReferralUI.Peek());
	Unknown_BE67BC->Unload("fc_buddyref1.ini");
	RenderObjClass *model = Unknown084->Peek_Model();
	G_ASSERT(BuddyReferralUI);
	model->Remove_Sub_Object(BuddyReferralUI.Peek());
	BuddyReferralUI = NULL;
	((ControlKeyListClass *)Unknown4C)->Remove(KernelEventClass(0x0D));
	((ControlKeyListClass *)Unknown4C)->Remove(KernelEventClass(0x1B));
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x0D), (void *)7);
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x1B), (void *)10);
	InterfaceClass::Unknown_58();
	Unknown_58();
}









// 0x55FDC0
void AvatarSelectionInterface::Unknown_55FDC0(bool on)
{
	for (int i = 0; i < 2; i++) {
		if (Unknown10C[i]) {
			Unknown10C[i]->Unknown_40(on);
			FontSys *text = Unknown_BE67C4->Get_Style(Unknown_B7B960[i], BuddyReferralUI.Peek());
			if (text) {
				text->Unknown_Set_2FC(on ? 1.0f : 0.33f);
			}
		}
	}
}

// 0x55FE50
void AvatarSelectionInterface::Unknown_55FE50(bool on)
{
	int hidden = !on;
	if (!Unknown088->Unknown1138) hidden = true;
	RefCountPtr<RenderObjClass> robj = Create_Get(Unknown084->Peek_Model()->Get_Sub_Object_By_Name("click_me"));
	G_ASSERT(robj);
	robj->Set_Hidden(hidden);

	FontSys *textptr = Unknown_BE67C4->Get_Style("fe12_ReferralText", Unknown084->Peek_Model());
	G_ASSERT(textptr);
	textptr->Set_Hidden(hidden);

	textptr = Unknown_BE67C4->Get_Style("fe12_ReferralButton", Unknown084->Peek_Model());
	G_ASSERT(textptr);
	textptr->Set_Hidden(hidden);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avsel.cpp (its __FILE__, 0xB7BEA0: 15 asserts, lines
**	769-1262)
**
**	The character select window, AvatarSelectionInterface (the client's RTTI: the type name of its
**	FlashingGadgetEvent callback, 0xB7C068, sits in this file's literals), charview.cpp's step 2 (0x679160).
**	Its base is intrface.cpp's InterfaceClass (the constructor calls 0x540A90 itself and puts no mesh at
**	+0x84, so not sclient.h's Unknown5BB320Class). The file's code starts at the constructor 0x55C650:
**	0x55C490-0x55C640 before it are ui_avatr.cpp's (0x55C630 / 0x55C640 the adjustor thunks into
**	AvatarCreationInterface's deleting destructor).
**
**	So far (LG2, claim in run PR #139): the constructor 0x55C650 (the 2-entry +0x10C cleared in a loop,
**	the camera's two vectors as temporaries, the key events' +4 / +8 floats), the destructor 0x55CCC0
**	(0x55CC90 its deleting one), slot +0x60 0x55CEA0 (every frame), +0x88 0x55EBD0, +0x8C 0x55ECB0,
**	0x55E2E0 (a text of Unknown_B7B904 changed: FAIL_IF line 769; inlined in its callers), 0x55F680, the
**	FlashingGadgetEvent callback 0x55F6D0 (the gadget's +0x2C bit 2 read through a bool getter of a
**	bitfield: the client shifts and tests), 0x55FDC0 and 0x55FE50 (the referral button; the hidden flag
**	set to !on, then to true without the task's +0x1138) exact; 0x55E8E0 (the five names, 0x55E2E0
**	inlined), 0x55ED20 / 0x55EF90 (the buttons lit / dimmed), 0x55F760 (the referral dialog opened:
**	G_ASSERT line 1160; its edit box is textgadg.cpp's) and 0x55FBB0 (closed: line 1215) exact. NOT
**	EXACT (register): 0x55E070 (the avatar under the pointer highlighted, 0.78). 0x55E370 (the
**	slots' avatars made, G_ASSERT 'chars' line 782) exact through inline accessors (the view's +0x3A4 and
**	+0x70, the avatar's +0x2BC): read directly, the arguments land in other registers. 0x55D9C0 (every frame: the referral's
**	result boxes, the emotes, the delete question's answer) exact; slot +0x68 0x55D060 (the buttons
**	1-17 and the referral dialog's) exact: it assigns the selected character's ship descriptor
**	through charview.h's setter, so ShipCustDescClass's implicit operator= is called out of line
**	(0x55D8A0) as the client does (three dead reads hold the budget); 0x55F140 (the selected character's
**	texts: race, profession, level, location, galaxy; FAIL_IFs 1047 / 1053 / 1059 / 1088; the
**	galaxies' list copied) NOT EXACT (register, 0.78). The class is declared in ui_avsel.h (a
**	provisional name: with it in this file the functions before line 769 do not fit above it), the
**	other types only as far as these functions use them; the character view, its avatars and the env maps are
**	charview.h's, avatar.h's and env_map_manager.h's (S1, run 2026-11-14).
*/
