// Earth & Beyond client (reconstruction): app\client\ui_groupfind.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_groupfind.cpp', 0x598DF0-0x59ACBF by its asserts and its object's end).
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <string>
#include <vector>
#include <list>
#include "ui_groupfind.h"
#include "gadget.h"
#include "gd_slider.h"
#include "gd_robj.h"
#include "fontmanager.h"
#include "client.h"
#include "co.h"
#include "..\input\input.h"
#include "..\packets\pkfindmember.h"
#include "target_find.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\nstrdup.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

extern FontManagerClass *Unknown_BE67C4;				// NAME 1 [Unknown_BE67C4]: 0xBE67C4

#define NUMBER_PLAYER_RACES		3	// NAME 3: the FAIL_IF 'race >= NUMBER_PLAYER_RACES' (compares with 3)
#define MAX_PLAYER_PROFESSIONS	3	// NAME 3: the FAIL_IF 'prof >= MAX_PLAYER_PROFESSIONS' (compares with 3)

// The buttons' models and tooltips (0xB7FB40)
static const char *Unknown_B7FB40[] = { "exit", "name", "rc", "level", NULL };	// NAME 1: neutral (D10)

// The races' professions (0xB7FB54)
static const char *Unknown_B7FB54[NUMBER_PLAYER_RACES][MAX_PLAYER_PROFESSIONS] = {	// NAME 1: neutral (D10)
	{ "Ter/Enf", "Ter/Tra", "Ter/Sct" },
	{ "Jen/Def", "Jen/Skr", "Jen/Exp" },
	{ "Pro/War", "Pro/Pvt", "Pro/Sen" },
};

// 0x598DF0
UIGroupFindClass::UIGroupFindClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	PlayersInfo(0),
	Unknown148(0)
{
	memset(Unknown0A8, 0, sizeof(Unknown0A8));
	memset(Unknown0B8, 0, sizeof(Unknown0B8));
}


// 0x599080
UIGroupFindClass::~UIGroupFindClass()
{
	Unknown_5C();
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x599260
void UIGroupFindClass::Unknown_54()
{
	if (Unknown_98("tsi25a", "fa_tsi25a.ini")) {
		int i;
		for (i = 0; i < 4; i++) {
			Unknown0A8[i] = Add_Button(i + 1, Unknown_B7FB40[i], NULL);
			GadgetClass::Set_Tooltip(Unknown0A8[i], "UI_GROUPFIND", Unknown_B7FB40[i]);
			if (i > 0) {
				Unknown0A8[i]->Unknown2C_0A = true;
			}
		}
		char name[0x20];
		for (i = 0; i < 8; i++) {
			_snprintf(name, sizeof(name), "name%02d", i + 1);
			Unknown0B8[i] = Add_Button(i + 6, name, NULL);
			GadgetClass::Set_Tooltip(Unknown0B8[i], "UI_FIND_LEAVE", "CLICK_TARGET");
			Unknown0B8[i]->Unknown2C_0A = true;
			_snprintf(name, sizeof(name), "name%02d_opt01", i + 1);
			Unknown0D8[i].Name = Create_Peek(Unknown_BE67C4->Get_Style(name, Interface_Mesh()));
			_snprintf(name, sizeof(name), "rc%02d_opt01", i + 1);
			Unknown0D8[i].Class = Create_Peek(Unknown_BE67C4->Get_Style(name, Interface_Mesh()));
			_snprintf(name, sizeof(name), "level%02d_opt01", i + 1);
			Unknown0D8[i].Level = Create_Peek(Unknown_BE67C4->Get_Style(name, Interface_Mesh()));
		}
		Slider = Add_Slider(0xF, 0);
		Slider->Unknown_66E460(InterfaceMesh, 0, 0);
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
	}
}


// 0x599660
void UIGroupFindClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		ChatPanelUIBase::Unknown_58();
		Unknown_599D00();
		Unknown_599E60();
		Top = -1;
		Unknown_599B10(0);
		if (Unknown0A8[0]) {
			((RObjGadgetClass *)Unknown0A8[0])->Unknown_66C360();
		}
	}
}

// 0x599830
UIGroupFindPlayerInfoClass::~UIGroupFindPlayerInfoClass()
{
	if (Name) {
		delete [] Name;
	}
	if (Class) {
		delete [] Class;
	}
	Name = NULL;
	Class = NULL;
	Level = 0;
	ID = 0;
}


// 0x599880
void UIGroupFindClass::Unknown_5C()
{
	if (Active) {
		Unknown_599D00();
		Unknown_599E60();
		InterfaceClass::Unknown_5C();







	}
}

// 0x5999F0
void UIGroupFindClass::Unknown_60()
{
	Unknown_59A1D0();
	Unknown_599B80();
}

// 0x599A30
bool UIGroupFindClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			return(true);
		case 2:
		case 3:
		case 4:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_599F30(key - 2);
			return(true);
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_59A290(key - 6);
			return(true);
	}
	return(false);
}

// 0x599B10
void UIGroupFindClass::Unknown_599B10(int count)
{
	Slider->Unknown_66FA90((float)count, 8.0f, true);
	Unknown_599B80();
}

// 0x599B80
void UIGroupFindClass::Unknown_599B80()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_599BC0();
	}
}

// 0x599BC0
void UIGroupFindClass::Unknown_599BC0()
{
	for (int i = 0; i < 8; i++) {
		unsigned int index = Top + i;
		if (index < PlayersInfo.size()) {
			Unknown0D8[i].Name->Change_Text(PlayersInfo[index]->Name, 0);
			Unknown0D8[i].Class->Change_Text(PlayersInfo[index]->Class, 0);
			char level[0x10];
			_snprintf(level, sizeof(level), "%d", PlayersInfo[index]->Level);
			Unknown0D8[i].Level->Change_Text(level, 0);
			Unknown0B8[i]->Unknown_40(true);
		} else {
			Unknown0D8[i].Name->Change_Text("", 0);
			Unknown0D8[i].Class->Change_Text("", 0);
			Unknown0D8[i].Level->Change_Text("", 0);
			Unknown0B8[i]->Unknown_40(false);
		}
	}
}

// 0x599D00
void UIGroupFindClass::Unknown_599D00()
{
	std::vector<UIGroupFindPlayerInfoClass *>::iterator it;
	for (it = PlayersInfo.begin(); it != PlayersInfo.end(); it++) {
		delete *it;
	}
	PlayersInfo.clear();
}

// 0x599E60
void UIGroupFindClass::Unknown_599E60()
{
	std::list<UIGroupFindPlayerInfoClass *>::iterator it;
	for (it = Unknown148.begin(); it != Unknown148.end(); ++it) {
		delete *it;
	}
	Unknown148.clear();
}

// (0x59A3D0 is defined below, after 0x59A290, at its client position)


// 0x599F30
void UIGroupFindClass::Unknown_599F30(int sort)
{
	Unknown_599D00();

	for (std::list<UIGroupFindPlayerInfoClass *>::iterator node = Unknown148.begin(); node != Unknown148.end(); node++) {
		UIGroupFindPlayerInfoClass *info = *node;
		if (info->ID == ((ClientClass *)Client)->AvatarID) continue;
		std::vector<UIGroupFindPlayerInfoClass *>::iterator pos;
		for (pos = PlayersInfo.begin(); pos != PlayersInfo.end(); pos++) {
			UIGroupFindPlayerInfoClass *other = *pos;
			int result;
			if (sort == 0) {
				result = wcscoll((const wchar_t *)info->Name, (const wchar_t *)other->Name);
				if (result < 0) break;
			} else if (sort == 1) {
				result = wcscoll((const wchar_t *)info->Class, (const wchar_t *)other->Class);
				if (!result) { if (other->Level > info->Level) break; if (other->Level < info->Level) continue; result = wcscoll((const wchar_t *)info->Name, (const wchar_t *)other->Name); }
				if (result < 0) break;
			} else {
				if (other->Level > info->Level) break; if (other->Level < info->Level) continue;
				result = wcscoll((const wchar_t *)info->Class, (const wchar_t *)other->Class); if (!result) result = wcscoll((const wchar_t *)info->Name, (const wchar_t *)other->Name);
				if (result < 0) break;
			}
		}
		UIGroupFindPlayerInfoClass *copy = new UIGroupFindPlayerInfoClass;
		copy->Name = nstrdup(info->Name);
		copy->Class = nstrdup(info->Class);
		copy->Level = info->Level;
		copy->ID = info->ID;
		PlayersInfo.insert(pos, copy);
	}

	Top = -1;
	Unknown_599B10(PlayersInfo.size());
}

// 0x59A1D0
void UIGroupFindClass::Unknown_59A1D0()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 8; i++) {
		if (Unknown0B8[i]->Unknown_30(x, y)) {
			Unknown0D8[i].Name->Unknown_6CE910(true);
			Unknown0D8[i].Class->Unknown_6CE910(true);
			Unknown0D8[i].Level->Unknown_6CE910(true);
		} else {
			Unknown0D8[i].Name->Unknown_6CE910(false);
			Unknown0D8[i].Class->Unknown_6CE910(false);
			Unknown0D8[i].Level->Unknown_6CE910(false);
		}
	}
}








































// 0x59A290
void UIGroupFindClass::Unknown_59A290(int row)
{
	int index = Top + row;
	FAIL_IF(index >= (int)PlayersInfo.size()) return;
	FAIL_IF(index < 0) return;
	unsigned id = PlayersInfo[index]->ID;
	if (id) {
		SClient *sclient = Client->As_Space_Client();



		G_ASSERT(sclient);
		TargetFindClass *targeting = (TargetFindClass *)sclient->Targeting;

		FAIL_IF(!targeting) return;
		ClientObjectClass *co = sclient->Find_Object(id);

		FAIL_IF(!co) return;
		targeting->Select_And_Set_Filter(co);
	}
}

// 0x59A3D0
int UIGroupFindClass::Unknown_59A3D0(int a, int b)
{
	if (a < b) return(-1);
	return(b < a);
}

















// 0x59A400
void UIGroupFindClass::Unknown_59A400(FindMemberPacketClass *packet)
{
	SClient *sclient;
	if (Client && (sclient = Client->As_Space_Client()) != NULL) {
		Unknown_599E60();




		for (std::list<FindMemberStruct *>::iterator it = packet->PlayerList.begin(); it != packet->PlayerList.end(); it++) {
			FindMemberStruct *player = *it;
			std::string name("Anonymous");
			ClientObjectClass *co = sclient->Find_Object(player->Unknown00);
			if (co) {
				co->Get_Name(name);
			}
			UIGroupFindPlayerInfoClass *info = new UIGroupFindPlayerInfoClass;
			info->ID = player->Unknown00;
			info->Name = nstrdup(name.c_str());
			int race = player->Unknown08;
			FAIL_IF(race >= NUMBER_PLAYER_RACES) race = 0;
			int prof = player->Unknown0C;

			FAIL_IF(prof >= MAX_PLAYER_PROFESSIONS) prof = 0;
			info->Class = nstrdup(Unknown_B7FB54[race][prof]);
			info->Level = player->Unknown04;
			Unknown148.push_back(info);
		}
		Unknown_599F30(2);
	}
}


/*
** Reconstruction notes (O7, 2026-11-13). The asserts sit on the client's lines (D25): 0x59A290's FAIL_IFs l.347 /
** 348 / 358 / 361 and G_ASSERT 'sclient' l.355, 0x59A400's FAIL_IFs l.410 / 413. The object ends with its guarded
** <string> initialiser 0x59AC70 and the adjustor thunks 0x59ACA0 / 0x59ACB0; the next object (ui_groupopt.cpp)
** starts at 0x59AD10. The class name follows the file (D24) and its tooltips' 'UI_GROUPFIND'. The template
** instances 0x59A720-0x59AC40 (the vector's and list's destructors, clear, insert, erase; std::_Construct) are the
** compiler's. 0x59A3D0 is a member that never uses this (ret 8) and has no caller; 0x599F30 compares the levels itself.
*/
