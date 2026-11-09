// Earth & Beyond client (reconstruction): app\Client\ui_shard.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <vector>
#include <list>
#include <string>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "camera.h"
#include "layer.h"
#include "assetmgr.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\g_assert.h"
#include "..\shared\avatar_desc.h"
#include "..\audio\soundlib.h"
#include "tutorial_manager.h"
#include "ui_shard.h"
#include "charview.h"
#include "avatar.h"
#include "gadget.h"
#include "gd_robj.h"
#include "textgadg.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "fontmanager.h"
#include "fontdatabase.h"
extern FontManagerClass *Unknown_BE67C4;
extern const int Unknown_B08C9C;					// 0xB08C9C (8: or'ed with textgadg.h's Unknown_B08CAC for the name's gadget)  // NAME 1: neutral (D10)			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The control keys event the constructor registers (0x10 bytes) and what InterfaceClass +0x4C is (as ui_class.cpp's)
struct UIShardKeyEventStruct	// NAME 1: neutral (D10): kernel.h's input event
{
	UIShardKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B, 0x0D)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
struct ControlKeyComboStruct; class ControlKeyListClass { public: void Add(const ControlKeyComboStruct &combo, void *handler); };	// InterfaceClass +0x4C: control_keys_manager.h's key list (Add 0x721E60), as ui_login.cpp declares it

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp)
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's
#include "..\shared\unknown_4be9d0.h"	// 0x4BE9D0: a name turned into another

static const char *Unknown_B8901C[] = { "button_shard00", "button_shard01", "button_shard02", "button_shard03", "button_shard04", "button_shard05", "skip", "name", "button_cancel", "exit" };	// NAME 1: neutral (D10)
static const char *Unknown_B89048[] = { "Galaxy00_fe02a", "Galaxy01_fe02a", "Galaxy02_fe02a", "Galaxy03_fe02a", "Galaxy04_fe02a", "Galaxy05_fe02a", "Skip_fe02a", "Name_fe02a", "Accept_fe02a", "Back_fe02a" };	// NAME 1: neutral (D10)
static const char *Unknown_B89074[] = { "Pop00_fe02a", "Pop01_fe02a", "Pop02_fe02a", "Pop03_fe02a", "Pop04_fe02a", "Pop05_fe02a" };	// NAME 1: neutral (D10)
static const char *Unknown_B89090[] = { "button_up", "button_down" };	// NAME 1: neutral (D10)
static const char *Unknown_B8909C = "Click to set Name";	// NAME 1: neutral (D10)
static char Unknown_BDA908[1];						// 0xBDA908 (.bss: an empty row's text)  // NAME 1: neutral (D10)


// The camera read through an inline accessor: the client loads it into eax and copies it to ecx (a member read
// straight into ecx does not).
static inline CameraClass *Camera_Of(CharacterViewClass *view) { return(view->Unknown068); }	// NAME 1: neutral (D10)

// @0x615A50
Unknown615A50InterfaceClass::Unknown615A50InterfaceClass(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	CharacterView(NULL),
	LoginTask(NULL),
	Unknown8C(0),
	ShardSelection(-1),
	Unknown94(0),
	NameGadget(NULL),
	Unknown9C(false),
	Unknown9D(false),
	Unknown9E(false),
	UnknownD0(false),
	UnknownD1(false),
	UnknownD2(false),
	UnknownD3(false),
	UnknownD4(false),
	UnknownD5(false),
	UnknownE8(false)
{
	int i;
	for (i = 0; i < 10; i++) {
		Buttons[i] = NULL;
	}
	Arrows[0] = NULL;
	Arrows[1] = NULL;
	CharacterView = (CharacterViewClass *)View->Unknown_28();
	LoginTask = (LoginTaskClass *)CharacterView->Unknown14->Unknown_08();
	InterfaceSortScopeClass sort(this);
	CharacterView->Unknown_67A250();
	CharacterView->Unknown_67AB20(0, "FE02A");
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	Camera_Of(CharacterView)->Set_Clip_Planes(1.0f, 1000.0f);
	int step = LoginTask->Previous_Step;
	if (step == 2) {
		CharacterView->Unknown_67AB20(3, "comp00a");
		CharacterView->Unknown_67C440(CharacterView->Unknown088.Race, 1);
		CharacterView->Unknown_67AB20(4, "FE_NAR_AGG");
		CharacterView->Unknown_67B4F0(0);
	}
	CharacterView->Unknown_67A6C0();
	if (CharacterView->NarratorRObj->Unknown_7863E0()) {
		CharacterView->Unknown_67A8D0(0);
	}
	Unknown_BE67BC->Load("fa_fe02a.ini");
	Unknown_BE67C4->Add(model);
	for (i = 0; i < 10; i++) {
		RenderObjClass *sub = model->Get_Sub_Object_By_Name(Unknown_B8901C[i]);
		Unknown_496AE0(sub, 0);
		Buttons[i] = new ButtonGadgetClass(sub, *layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Buttons[i], i + 1, View, 1, 1);
		sub->Release_Ref();
	}
	for (i = 0; i < 2; i++) {
		RenderObjClass *sub = model->Get_Sub_Object_By_Name(Unknown_B89090[i]);
		Unknown_496AE0(sub, 0);
		Arrows[i] = new ButtonGadgetClass(sub, *layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Arrows[i], i + 13, View, 1, 1);
		sub->Release_Ref();
	}
	NameGadget = new TextGadgetClass(this, 0x14, "Name_fe02a", model, Unknown_B08CAC | Unknown_B08C9C, 0);
	NameGadget->Set_Text(Unknown_B8909C, 0);
	NameGadget->Unknown_28(layer);
	Unknown64.Unknown_6870E0(NameGadget, 15, View, 1, 1);
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIShardKeyEventStruct(0x1B), (void *)12);
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIShardKeyEventStruct(0x0D), (void *)9);
	Unknown_6176F0();
	if (CharacterView->Unknown_67AD30(0, "FE02A.FE02A-1")) {
		CharacterView->Unknown_67AE20(0);
	}
	SoundLibraryClass::Play_2D_Sound("fontshoosh.wav", 2);
	UnknownD2 = true;
	Unknown_617540();
	LoginTaskClass *task = LoginTask;
	task->Unknown0020 = 0;
	task->Error = 0;
	Unknown9E = TutorialManagerClass::Instance->Read_Completed_File(true);
}


// @0x616090
Unknown615A50InterfaceClass::~Unknown615A50InterfaceClass()
{
	Unknown_5C();
	int i;
	for (i = 0; i < 10; i++) {
		if (Buttons[i]) {
			GadgetClass::Destroy(Buttons[i]);
			Buttons[i] = NULL;
		}
	}
	for (i = 0; i < 2; i++) {
		if (Arrows[i]) {
			GadgetClass::Destroy(Arrows[i]);
			Arrows[i] = NULL;
		}
	}
	if (NameGadget) {
		GadgetClass::Destroy(NameGadget);
		NameGadget = NULL;
	}
	Unknown_BE67C4->Remove(CharacterView->Peek_Model());
	Unknown_BE67BC->Unload("fa_fe02a.ini");
	CharacterView->Unknown_67A9B0(0);
}


// NOT EXACT: functional (D64)
// @0x616220
void Unknown615A50InterfaceClass::Unknown_60()
{
	if (!UnknownD0 && !UnknownD1 && !UnknownD2 && !UnknownD3 && !UnknownD4 && !UnknownD5 && !UnknownE8) {
		if (Buttons[8]->Unknown_60()) {
			if (NameGadget->Get_Text_Length() == 0) {
				Unknown9C = false;
				Unknown_617660(false);
				Unknown_617310();
			}
		}
		if (!Buttons[8]->Unknown_60()) {
			if (NameGadget->Get_Text_Length() != 0 && strcmp(NameGadget->Get_Text(), Unknown_B8909C) != 0) {
				Unknown9C = true;
				Unknown_617660(true);
				Unknown_617310();
			}
		}
	}
	Unknown_616A10();
	if (NameGadget) {
		if (NameGadget->Active) {
			NameGadget->Set_Text(Unknown_4BE9D0(NameGadget->Get_Text()).c_str(), 0);
		}
		NameGadget->Unknown_6AAD10();
	}
}


// @0x6164E0
bool Unknown615A50InterfaceClass::Unknown_68(int key)
{
	if (UnknownD0 || UnknownD1 || UnknownD2 || UnknownD3 || UnknownD4 || UnknownD5 || UnknownE8) {
		return false;
	}
	SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
	switch (key) {
		case 8:
			NameGadget->Unknown_74(true);
			NameGadget->Set_Text(NULL, 0);
			Unknown9C = false;
			Unknown_617660(false);
			Unknown_617310();
			return true;

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			Unknown_6179D0(key - 1);
			return true;

		case 13:
			Unknown_617270();
			return true;

		case 14:
			Unknown_6172A0();
			return true;

		case 9:
			if (Unknown9C && ShardSelection >= 0 && ShardSelection < (int)SortedGalaxies.size() && !UnknownE8) {
				Unknown94 = 9;
				NameGadget->Unknown_74(false);
				Unknown_617540();
				char name[0x14];
				NameGadget->Unknown_6AA340(name, sizeof(name), NULL, NULL, NULL, false);
				CharacterView->Unknown088.Unknown_4C1460(name, true);
				LoginTask->Unknown_48(&CharacterView->Unknown088, &CharacterView->Unknown244, Unknown_617A10(), Unknown9D);
				UnknownE8 = true;
			}
			return true;

		case 10:
			Unknown94 = 10;
			if (CharacterView->Unknown_67AD30(0, "FE02A.FE02A-1")) {
				CharacterView->Unknown_67AE60(0);
			}
			NameGadget->Unknown_74(false);
			UnknownD3 = true;
			CharacterView->NarratorRObj->Unknown_784E00();
			CharacterView->Unknown_67A8D0(1);
			Unknown_617540();
			return true;

		case 12:
			CharacterView->Unknown_67B270();
			UnknownD1 = true;
			return true;

		case 7:
			Unknown_617AA0();
			return true;
	}
	return false;
}


static inline void Clear_Result(LoginTaskClass *task) { task->Unknown0020 = 0; task->Error = 0; }	// NAME 1: neutral (D10): the client loads the task once for both stores

// @0x616A10
void Unknown615A50InterfaceClass::Unknown_616A10()
{
	if (UnknownD2 && CharacterView->Unknown_67AEA0(0)) {
		UnknownD2 = false;
		Unknown_617310();
		CharacterView->NarratorRObj->Unknown_784C90("ChooseShard", false);
	}
	if (CharacterView->NarratorRObj->Unknown_784EA0("AllGood")) {
		if (CharacterView->Unknown_67AD30(0, "FE02A.FE02A-2")) {
			CharacterView->Unknown_67AE20(0);
		}
		SoundLibraryClass::Play_2D_Sound("fontshoosh.wav", 2);
		UnknownD3 = true;
		CharacterView->Unknown_67A8D0(1);
	}
	if (UnknownD3 && CharacterView->Unknown_67AEA0(0)) {
		UnknownD3 = false;
		if (Unknown94 == 10) {
			if (LoginTask->Previous_Step == 2) {
				LoginTask->Set_Step(2, false);
				CharacterView->Unknown_67A670();
			} else {
				LoginTask->Go_Back_Step();
				CharacterView->Unknown_67A670();
			}
		} else if (Unknown94 == 9) {
			LoginTask->Go_Next_Step();
			TutorialManagerClass::Instance->Write_Completed_File();
		}
	}
	if (LoginTask->Unknown0020 != 0) {
		if (LoginTask->Unknown0020 != 1) {
			NameGadget->Set_Text(Unknown_B8909C, 0);
			Unknown_617660(false);
			Unknown9C = false;
			switch (LoginTask->Error) {
				case 8: CharacterView->NarratorRObj->Unknown_784C90("NameTaken", true); break;
				case 9: CharacterView->NarratorRObj->Unknown_784C90("NameWithNumbers", true); break;
				case 10: CharacterView->NarratorRObj->Unknown_784C90("NameTooShort", true); break;
				case 11: CharacterView->NarratorRObj->Unknown_784C90("NameNoVowels", true); break;
				case 12: CharacterView->NarratorRObj->Unknown_784C90("NameRepeatCharacters", true); break;
				case 13: CharacterView->NarratorRObj->Unknown_784C90("NameRestricted", true); break;
			}
			Clear_Result(LoginTask);
		} else {
			if (LoginTask->Unknown001D) {
				Clear_Result(LoginTask);
				CharacterView->NarratorRObj->Unknown_784C90("AllGood", true);
			}
		}
	}
	if (CharacterView->Unknown_67B2B0()) {
		UnknownE8 = false;
	}
	if (CharacterView->Unknown34->Active && !UnknownD1) {
		UnknownD1 = true;
		Unknown_617540();
	} else if (!CharacterView->Unknown34->Active && UnknownD1) {
		UnknownD1 = false;
		Unknown_617310();
		UnknownE8 = false;
	}
}


// @0x616DA0
void Unknown615A50InterfaceClass::Unknown_616DA0()
{
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	char name[32];
	char text[32];
	int row;
	if ((int)SortedGalaxies.size() <= Unknown8C) {
		for (row = 0; row < 6; row++) {
			sprintf(name, "Galaxy%02d_fe02a", row);
			Unknown_BE67C4->Unknown_6D2500(name, Unknown_BDA908, model, 0);
			sprintf(name, "Pop%02d_fe02a", row);
			Unknown_BE67C4->Unknown_6D2500(name, Unknown_BDA908, model, 0);
			Buttons[row]->Unknown_6607D0();
			Buttons[row]->Unknown2C_0B = 0;
		}
		Unknown_617190();
		return;
	}
	row = 0;
	for (int index = Unknown8C; index < (int)SortedGalaxies.size() && row < 6; row++, index++) {
		sprintf(name, "Galaxy%02d_fe02a", row);
		Unknown_BE67C4->Unknown_6D2500(name, SortedGalaxies[index].Name, model, 0);
		sprintf(name, "Pop%02d_fe02a", row);
		float capacity = (float)(unsigned int)SortedGalaxies[index].Unknown58 * 0.9f;
		if (capacity == 0.0f) {
			capacity = 1.0f;
		}
		float full = ((float)(unsigned int)SortedGalaxies[index].Unknown54 / capacity) * 100.0f;
		if (full >= 100.0f) {
			_snprintf(text, sizeof(text), "FULL");
		} else {
			_snprintf(text, sizeof(text), "%.0f%% full", (double)full);
		}
		Unknown_BE67C4->Unknown_6D2500(name, text, model, 0);
		if (ShardSelection == row + Unknown8C) {
			Buttons[row]->Unknown_6603A0(false, NULL);
			Buttons[row]->Unknown2C_0B = 1;
		} else {
			Buttons[row]->Unknown_6607D0();
			Buttons[row]->Unknown2C_0B = 0;
		}
	}
	if (row < 6) {
		int count = 6 - row;
		do {
			sprintf(name, "Galaxy%02d_fe02a", row);
			Unknown_BE67C4->Unknown_6D2500(name, Unknown_BDA908, model, 0);
			sprintf(name, "Pop%02d_fe02a", row);
			Unknown_BE67C4->Unknown_6D2500(name, Unknown_BDA908, model, 0);
			Buttons[row]->Unknown_6607D0();
			Buttons[row]->Unknown2C_0B = 0;
			row++;
		} while (--count != 0);
	}
	Unknown_617190();
}


// @0x617190
void Unknown615A50InterfaceClass::Unknown_617190()
{
	if (Unknown8C == 0) {
		Arrows[0]->Unknown_40(false);
		((RObjGadgetClass *)Arrows[0])->Unknown_8C(true);
	} else {
		Arrows[0]->Unknown_40(true);
		((RObjGadgetClass *)Arrows[0])->Unknown_8C(false);
	}
	if (Unknown8C >= (int)SortedGalaxies.size() - 6) {
		Arrows[1]->Unknown_40(false);
		((RObjGadgetClass *)Arrows[1])->Unknown_8C(true);
	} else {
		Arrows[1]->Unknown_40(true);
		((RObjGadgetClass *)Arrows[1])->Unknown_8C(false);
	}
}


// @0x617270
void Unknown615A50InterfaceClass::Unknown_617270()
{
	if (Unknown8C != 0) {
		Unknown8C--;
		Unknown_616DA0();
	}
}


// @0x6172A0
void Unknown615A50InterfaceClass::Unknown_6172A0()
{
	if (Unknown8C < (int)SortedGalaxies.size() - 6) {
		Unknown8C++;
		Unknown_616DA0();
	}
}


// @0x617310
void Unknown615A50InterfaceClass::Unknown_617310()
{
	int i;
	for (i = 0; i < 10; i++) {
		if ((i != 6 || Unknown9E) && Buttons[i] != NULL) {
			Buttons[i]->Unknown_40(true);
			Unknown_BE67C4->Get_Style(Unknown_B89048[i], CharacterView->Peek_Model())->Unknown_Set_2FC(1.0f);
		}
	}
	for (i = 0; i < 6; i++) {
		FontSys *pop = Unknown_BE67C4->Get_Style(Unknown_B89074[i], CharacterView->Peek_Model());
		if (Unknown8C + i >= (int)SortedGalaxies.size()) {
			Buttons[i]->Unknown_40(false);
			pop->Set_Hidden(1);
		} else {
			pop->Set_Hidden(0);
		}
	}
	NameGadget->Text->Unknown_Set_2FC(1.0f);
	for (i = 0; i < 2; i++) {
		if (Arrows[i]) {
			Arrows[i]->Unknown_40(true);
		}
	}
	if (Unknown9C && ShardSelection >= 0 && ShardSelection < (int)SortedGalaxies.size()) {
		return;
	}
	Buttons[8]->Unknown_40(false);
	Unknown_BE67C4->Get_Style("Accept_fe02a", CharacterView->Peek_Model())->Unknown_Set_2FC(0.33f);
}


// @0x617540
void Unknown615A50InterfaceClass::Unknown_617540()
{
	int i;
	for (i = 0; i < 10; i++) {
		if (Buttons[i] != NULL) {
			Buttons[i]->Unknown_40(false);
			Unknown_BE67C4->Get_Style(Unknown_B89048[i], CharacterView->Peek_Model())->Unknown_Set_2FC(0.33f);
		}
	}
	for (i = 0; i < 6; i++) {
		Unknown_BE67C4->Get_Style(Unknown_B89074[i], CharacterView->Peek_Model())->Unknown_Set_2FC(0.33f);
	}
	NameGadget->Text->Unknown_Set_2FC(0.33f);
	for (i = 0; i < 2; i++) {
		if (Arrows[i]) {
			Arrows[i]->Unknown_40(false);
		}
	}
}


// @0x617660
void Unknown615A50InterfaceClass::Unknown_617660(bool named)
{
	TextureClass *texture;
	if (named) {
		texture = WW3DAssetManager::Get_Instance()->Get_Texture("co0102nw_gd.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
	} else {
		texture = WW3DAssetManager::Get_Instance()->Get_Texture("co0102nw.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
	}
	if (texture) {
		Unknown_495FD0(((RObjGadgetClass *)Buttons[7])->RObj, texture, 0);
		texture->Release_Ref();
	}
}


// The four dead size() reads spend VC6's inline budget so that clear()'s erase and insert(at, 1, x) stay out of
// line (0x617F70, 0x617B70) as in the client (workflow.md 3.1: 4 or 5 reads match, 2-3 or 6 do not).
// @0x6176F0
void Unknown615A50InterfaceClass::Unknown_6176F0()
{
	SortedGalaxies.clear();
	SortedGalaxies.size();
	SortedGalaxies.size();
	SortedGalaxies.size();
	SortedGalaxies.size();
	std::list<LoginGalaxyClass> galaxies(LoginTask->Galaxies);
	for (std::list<LoginGalaxyClass>::iterator it = galaxies.begin(); it != galaxies.end(); it++) {
		LoginGalaxyClass galaxy = *it;
		std::vector<LoginGalaxyClass>::iterator at = SortedGalaxies.begin();
		while (at != SortedGalaxies.end() && (unsigned int)at->Unknown54 <= (unsigned int)galaxy.Unknown54) {
			at++;
		}
		LoginGalaxyClass entry(galaxy);
		if (entry.Unknown5C != 0) {
			SortedGalaxies.insert(at, 1, entry);
		}
	}
	Unknown_616DA0();
}


// @0x6179D0
void Unknown615A50InterfaceClass::Unknown_6179D0(int row)
{
	ShardSelection = Unknown8C + row;
	Unknown_616DA0();
	Unknown_617310();
}





































































































































































// @0x617A10
int Unknown615A50InterfaceClass::Unknown_617A10()
{
	FAIL_IF(ShardSelection < 0 || ShardSelection >= (int)SortedGalaxies.size()) return -1;
	return SortedGalaxies[ShardSelection].ID;
}


// @0x617AA0
void Unknown615A50InterfaceClass::Unknown_617AA0()
{
	Unknown9D = !Unknown9D;
	FontSys *skip = Unknown_BE67C4->Get_Style("Skip_fe02a", CharacterView->Peek_Model());
	if (skip) {
		if (Unknown9D) {
			skip->Change_Text("Skipping Beginner Tutorial", 0);
			return;
		}
		skip->Change_Text("Playing with Beginner Tutorial", 0);
	}
}


/*
**	Notes
**
**	The galaxy select window of character creation (the character view's mode 5, LoginTaskClass step 5), client
**	0x615A50-0x617AF0 (then the vector<LoginGalaxyClass> members it instantiates, 0x617B20-0x618050). __FILE__
**	'e:\build\beta\G\app\Client\ui_shard.cpp' (0xB89390) from the FAIL_IF of 0x617A10 (line 707), inlined in the
**	Accept of slot +0x68. Functional (D64) for the POC client: written from the saved pseudocode and the
**	disassembly; the FAIL_IF sits on the client's line 707 through the blank lines above 0x617A10 (D25).
**
**	The Accept (event 9; its button is the model's 'button_cancel') names the character view's avatar (+0x88,
**	AvatarDescClass::Unknown_4C1460 with the text gadget's 0x14 characters) and calls LoginTaskClass slot +0x48
**	(0x7689E0) with the view's avatar, its ship (+0x244), the picked galaxy's ID and the tutorial skip: the
**	GlobalCreateCharacterPacketClass (opcode 114) on the global connection. The answer comes back through the
**	task's +0x20 (1: approved, then +0x1D) or its error +0x1090 (8-13: the narrator's 'NameTaken' ...).
**
**	Event ids: rows 1-6, skip 7, name 8, Accept 9, back 10 ('exit'), quit 12 (escape), up 13, down 14; the
**	name's text gadget 15; enter is 9.
*/
