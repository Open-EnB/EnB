// Earth & Beyond client (reconstruction): app\client\ui_avatar_dialog.cpp, the avatar dialog over the cockpit
// ('TLI01A'). The client's asserts name the file. The asserts sit on the client's lines (D25). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "ui_avatar_dialog.h"
#include "sclient.h"
#include "client.h"
#include "ui_mesh.h"
#include "avatar.h"
#include "avatar_pip.h"
#include "unknown_675120.h"
#include "gd_robj.h"
#include "gd_uiback.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "rendobj.h"
#include "..\audio\soundlib.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\unknown_font_split_string.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_manu.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's

static inline EnvironmentMapManagerClass *Unknown_Inline_Env_Maps(SClient *sclient) { return(sclient->EnvironmentMapManager); }	// NAME 1: neutral (D10)
// The buttons Unknown_54 makes (ids 1-9): their models' names and their font styles (0xB7AD2C / 0xB7AD54)
static const char *Unknown_B7AD2C[10] = { "MORE", "SKILL", "TRADE", "EXIT", "ANSWER00", "ANSWER01", "ANSWER02", "ANSWER03", "ANSWER04", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7AD54[10] = { "More_TLI01A_AV", "Skill_TLI01A_AV", "Trade_TLI01A_AV", NULL, "WRAP_Answer00_TLI01A_AV", "WRAP_Answer01_TLI01A_AV", "WRAP_Answer02_TLI01A_AV", "WRAP_Answer03_TLI01A_AV", "WRAP_Answer04_TLI01A_AV", NULL };	// NAME 1: neutral (D10)

// 0x551FB0
UIAvatarDialogClass::UIAvatarDialogClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TLI01A", true), client),
	Pip(NULL),
	State(UI_CLOSED),
	Answer(-1),
	Unknown0E0(false),
	Split(NULL),
	Text("")
{
	memset(Buttons, 0, sizeof(Buttons));
	for (int i = 0; i < NUM_ANSWERS; i++) {
		Answers[i] = "";
		Replies[i] = "";
	}
	for (int j = 0; j < 6; j++) {
		Talk[j] = NULL;
	}
	Unknown_BE67BC->Load("fc_TLI01A.ini");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}




























// 0x552310
UIAvatarDialogClass::~UIAvatarDialogClass()
{
	G_ASSERT(State == UI_CLOSED);
	delete Pip;
	Pip = NULL;
	delete Split;
	Split = NULL;
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("fc_TLI01A.ini");
}













// 0x5524F0
void UIAvatarDialogClass::Unknown_54()
{
	for (int id = 1; id < 10; id++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B7AD2C[id - 1]));
		Unknown_496AE0(robj.Peek(), 0);
		Buttons[id] = new ButtonGadgetClass(robj.Peek(), Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Buttons[id], id, View, 1, 0);
		Unknown_5BB4F0(Buttons[id]);
		if (Unknown_B7AD54[id - 1]) {
			RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B7AD54[id - 1], InterfaceMesh->Peek_Mesh_Object()));
			G_ASSERT(font);
			if (font) {
				font->Set_Sort_Level(robj->Get_Sort_Level() - 1);
				Buttons[id]->Unknown_98(font.Peek());
			}
		}
		if (id < 4) {
			Buttons[id]->Unknown_40(false);
			Buttons[id]->Unknown_8C(true);
		}
	}
	for (int j = 0; j < 6; j++) {
		char name[32];
		_snprintf(name, sizeof(name), "Talk%02d_TLI01A_AV", j);
		Talk[j] = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Object()));
	}
	Split = new FontSplitStringClass(Talk[0]->Font);
	GadgetClass *gadget = new UIBackGadget(Unknown_497300(InterfaceMesh->Peek_Mesh_Object()));
	Unknown64.Unknown_6870E0(gadget, 0xB, View, 1, 0);
	Pip = new AvatarPipClass;
	Unknown_10();
}

// NOT EXACT: register (the client keeps this in edi and the button pointer / the space client in esi, ours the other way round; tried: an early return, a 5-based loop, a button pointer, the space client declared first)
// 0x552980
void UIAvatarDialogClass::Unknown_58()
{
	if (State == UI_CLOSED) {
		InterfaceSortScopeClass scope(this);
		State = UI_OPEN;
		for (int i = 0; i < NUM_ANSWERS; i++) {
			if (Answers[i].length()) {
				Buttons[i + 5]->Unknown34->Change_Text(Answers[i].c_str(), 0);
				Buttons[i + 5]->Unknown_40(true);
				Buttons[i + 5]->RObj->Set_Hidden(false);
			} else {
				Buttons[i + 5]->Unknown34->Change_Text("", 0);
				Buttons[i + 5]->Unknown_40(false);
				Buttons[i + 5]->RObj->Set_Hidden(true);
			}
		}
		RefCountPtr<FontSys> reply = Create_Peek(Unknown_BE67C4->Get_Style("Reply_TLI01A_AV", InterfaceMesh->Peek_Object()));
		if (reply) {
			reply->Set_Hidden(false);
		}
		SClient *sclient = ((SClient *)((ClientClass *)Client)->As_Space_Client());
		Pip->Unknown_7E36B0(sclient, Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("PICTURE")), false);
		Pip->Unknown_7E3A20(Text.c_str(), 0, (CharacterViewAvatarClass *)AvatarRObj.Peek());
		Unknown0E0 = false;
		Answer = -1;
		SoundLibraryClass::Play_2D_Sound("message_1.wav", 2);
		Buttons[1]->Unknown_40(false);
		Buttons[1]->Unknown_8C(true);
		Unknown5BB320Class::Unknown_58();
	}
}

// 0x552C20
void UIAvatarDialogClass::Unknown_5C()
{
	if (State == UI_OPEN) {
		State = UI_CLOSED;
		Unknown5BB320Class::Unknown_5C();
		Pip->Unknown_7E4350();
	}
}

// 0x552C60
void UIAvatarDialogClass::Unknown_60()
{
	Pip->Unknown_7E3A20(NULL, 0, (CharacterViewAvatarClass *)AvatarRObj.Peek());
	if (Unknown0E0 && Pip->Unknown02C->Unknown220) {
		Unknown_5C();
	}
	Unknown5BB320Class::Unknown_60();
}

// 0x552CD0
bool UIAvatarDialogClass::Unknown_68(int key)
{
	SClient *sclient = ((SClient *)((ClientClass *)Client)->As_Space_Client());
	switch (key) {
		case 1:
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			return(true);
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_552D90(key - 5);
			return(true);
	}
	return(false);
}

// 0x552D90
void UIAvatarDialogClass::Unknown_552D90(int answer)
{
	Answer = answer;
	for (int i = 0; i < NUM_ANSWERS; i++) {
		Buttons[i + 5]->Unknown34->Change_Text("", 0);
		Buttons[i + 5]->Unknown_40(false);
		Buttons[i + 5]->RObj->Set_Hidden(true);
	}
	RefCountPtr<FontSys> reply = Create_Peek(Unknown_BE67C4->Get_Style("Reply_TLI01A_AV", InterfaceMesh->Peek_Object()));
	if (reply) {
		reply->Set_Hidden(true);
	}
	if (!Replies[Answer].empty()) {
		Unknown_553370(Replies[Answer].c_str());
		Pip->Unknown_7E3A20(Text.c_str(), 0, (CharacterViewAvatarClass *)AvatarRObj.Peek());
		Unknown0E0 = true;
		Buttons[1]->Unknown_40(true);
		Buttons[1]->Unknown_8C(false);
	} else {
		Unknown_5C();
	}
}




























































// NOT EXACT: register (one load: the client reads the new avatar into ecx for the store to AvatarRObj, ours reuses edi; the template operator= and the inline env-map read fixed the rest)
// 0x552FC0
void UIAvatarDialogClass::Unknown_552FC0(AvatarDescClass *desc)
{
	AvatarRObj = Create_NEW(new CharacterViewAvatarClass(desc, 1, 0, Unknown_Inline_Env_Maps((SClient *)((ClientClass *)Client)->As_Space_Client())));
	FAIL_IF(!AvatarRObj) return;



	int level = Buttons[4]->RObj->Get_Sort_Level() - 1;
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("NAME_TLI01A_AV", InterfaceMesh->Peek_Object()));
	FAIL_IF(!font) return;
	std::string name;
	((CharacterViewAvatarClass *)AvatarRObj.Peek())->Desc.Get_Full_Name(name);
	font->Set_Sort_Level(level);
	font->Change_Text(name.c_str(), 0);
	font = Create_Peek(Unknown_BE67C4->Get_Style("Reply_TLI01A_AV", InterfaceMesh->Peek_Object()));



	FAIL_IF(!font) return;
	font->Set_Sort_Level(level);
}

// 0x553370
void UIAvatarDialogClass::Unknown_553370(std::string text)
{
	Text = text;
	RefCountPtr<RenderObjClass> talk = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("TALK00"));
	if (talk) {
		const AABoxClass &box = talk->Get_Bounding_Box();
		float width = (box.Extent.X + box.Extent.X) * 0.95f;
		Split->Split(Text.c_str(), width, 0);
	}
	for (int i = 0; i < 6; i++) {
		const char *line = Split->Get(i);
		if (line) {
			Talk[i]->Change_Text(line, 0);
		} else {
			Talk[i]->Change_Text(NULL, 0);
		}
	}
}






// 0x553500
void UIAvatarDialogClass::Unknown_553500(std::vector<std::string> lines)
{
	int num = lines.size();
	FAIL_IF(num > NUM_ANSWERS) num = NUM_ANSWERS;
	for (int i = 0; i < num; i++) {
		Answers[i] = lines[i];
	}
	for (; num < NUM_ANSWERS; num++) {
		Answers[num] = "";
	}
}







// 0x5537A0
void UIAvatarDialogClass::Unknown_5537A0(std::vector<std::string> lines)
{
	int num = lines.size();
	FAIL_IF(num > NUM_ANSWERS) num = NUM_ANSWERS;
	for (int i = 0; i < num; i++) {
		Replies[i] = lines[i];
	}
	for (; num < NUM_ANSWERS; num++) {
		Replies[num] = "";
	}
}

// 0x553A40
int UIAvatarDialogClass::Unknown_553A40()
{
	return(Answer);
}

/*
**	Notes.
**	Object 0x551FB0-0x553AEF: the constructor 0x551FB0 (StarbaseAvatarCreationInterface's adjustor thunk 0x551FA0
**	precedes it), the deleting destructor 0x5522B0, std::string's default constructor 0x5522E0 (out of line: the
**	answer arrays' vector constructor iterator), the slots and functions in the client's order, then the out-of-line
**	std::string::c_str 0x553A60 (0x552D90's), RefCountPtr<FontSys>'s default constructor 0x553A80 (the Talk array's
**	iterator) and the adjustor thunks 0x553AD0 / 0x553AE0 of the second and third tables. The ui_analyze.cpp pattern
**	(Unknown5BB320Class, the buttons over the model's sub-objects, State); the PIP as ui_petition_avatar_pip.cpp's.
**	The incapacitance packet (ui_cpit.cpp's 0x584690) gives the avatar, its text, the answers and the replies.
*/
