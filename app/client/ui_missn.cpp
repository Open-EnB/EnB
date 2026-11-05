// Earth & Beyond client (reconstruction): app\client\ui_missn.cpp, the character info window's missions tab (the client's
// file per its asserts, 'e:\build\beta\G\app\Client\ui_missn.cpp'). Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "texture.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\shared\unknown_debug.h"
#include "..\audio\soundlib.h"
#include "..\packets\pkmissionforfeit.h"
#include "unknown_675120.h"
#include "unknown_674260.h"		// the gadget 0x674260 makes (0x60 bytes)
#include "assetmgr.h"
#include <stdio.h>
#include "ui_missn.h"

#define InterfaceMesh Mesh		// the FAIL_IFs' name for the base's +0x84 (ui_faction.cpp's 0x58D400 l.118): Unknown5BB320Class::Mesh  // NAME 3: the FAIL_IF's
extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_cinfo.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

// intrlist.cpp's list (InterfaceClass +0x58; intrface.h declares it only under __INTRFACE_MEMBERS), as far as this file calls it
class InterfaceUnknown58Class  // NAME 1: neutral (D10)
{
public:
	void Unknown_686CC0(RenderObjClass *robj, SceneClass *scene);	// 0x686CC0
};

// ui_talkt_space.cpp's talk tree window (SClient +0x1298), as far as this file reads it
class UITalkTreeSpaceClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_talkt_space.cpp
{
};

// (the current missions are aux_roots.h's DeltaMissionClass: ID +0x120, IsForfeitable +0x5F4, IsComplete +0x67C, IsFailed +0x704, IsExpired +0x78C)







// 0x5C4170
UIMissnClass::UIMissnClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI22A", 2), client),
	DescriptionPanel(NULL),
	StandardBttnTexture(NULL),
	SelectedBttnTexture(NULL),
	UnknownB8(NULL),
	UnknownBC(0),
	UnknownC0(false),
	UnknownC4(NULL),
	CurrentMission(NO_CURRENT_MISSION)
{
	memset(CurrentMissions, 0, sizeof(CurrentMissions));
	memset(UnknownD8, 0, sizeof(UnknownD8));
	Unknown_BE67BC->Load("fa_TSI22A.ini");
}

// 0x5C42E0
UIMissnClass::~UIMissnClass()
{
	REF_PTR_RELEASE(StandardBttnTexture);
	REF_PTR_RELEASE(SelectedBttnTexture);
	Unknown_BE67C4->Remove(Mesh->Peek_Mesh_Object());
	Unknown_BE67C4->Remove(DescriptionPanel);
	Unknown_BE67BC->Unload("fa_TSI22A.ini");
	REF_PTR_RELEASE(DescriptionPanel);
	REF_PTR_RELEASE(UnknownC4);
	if (UnknownB8) {
		UnknownB8->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x5C4490
void UIMissnClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (UnknownC4) {
		UnknownC4->Unknown_7C(NULL, false);
		REF_PTR_RELEASE(UnknownC4);
	}
	Unknown5BB320Class::Unknown_7C(view, arg);
	if (View) {
		LayerClass layer((SceneClass *)View->Unknown_50(), (CameraClass *)View->Unknown_54());
		UnknownC4 = new ViewUnknown34Class(View, layer);
		UnknownC4->Unknown_54();
		UnknownC4->Unknown_7C(View, false);
	}
}



















// 0x5C4610
void UIMissnClass::Unknown_54()
{
	DescriptionPanel = WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi23a");
	FAIL_IF(!DescriptionPanel) return;

	DescriptionPanel->Set_Hidden(true);
	((InterfaceUnknown58Class *)&Unknown4C[0x58 - 0x4C])->Unknown_686CC0(DescriptionPanel, NULL);	// (InterfaceClass +0x58)
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67C4->Add(DescriptionPanel);

	for (int i = 0; i < MISSIONS_PER_PAGE; i++) {
		char name[32];
		sprintf(name, "HIGHLIGHT%02d", i);
		ButtonGadgetClass *highlight = new ButtonGadgetClass(InterfaceMesh, name, 0x55, 3);
		UnknownD8[i + 1] = highlight;
		Unknown64.Unknown_6870E0(highlight, i + 1, View, 1, 0);
		Unknown_5BB4F0(highlight);
	}








	REF_PTR_SET(StandardBttnTexture, UnknownD8[1]->Unknown_66C4A0());
	G_ASSERT(StandardBttnTexture);


	SelectedBttnTexture = WW3DAssetManager::Get_Instance()->Get_Texture("I_blank_SEL.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	G_ASSERT(SelectedBttnTexture);


	RenderObjClass *forfeit = DescriptionPanel->Get_Sub_Object_By_Name("FORFEIT");
	FAIL_IF(!forfeit) return;
	ButtonGadgetClass *button = new ButtonGadgetClass(forfeit, 0x55, 5);
	UnknownD8[4] = button;
	Unknown64.Unknown_6870E0(button, 4, View, 1, 0);
	Unknown_5BB4F0(button);
	forfeit->Release_Ref();
	if (UnknownD8[4]->Unknown_60()) {
		FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
		UnknownD8[4]->Unknown_40(false);
		if (font) {
			font->Set_Hidden(true);
		}
	}

	GadgetClass *gadget = new Unknown_674260Class(Create_Peek(DescriptionPanel));
	Unknown64.Unknown_6870E0(gadget, 7, View, 1, 0);
	gadget = new Unknown_674260Class(Create_Peek(InterfaceMesh->Peek_Mesh_Object()));
	Unknown64.Unknown_6870E0(gadget, 7, View, 1, 0);

	UnknownB8 = new GadgetSlider(false);
	UnknownB8->Unknown_66E460(Create_Peek(InterfaceMesh->Peek_Object()), 0, 0);
	Unknown64.Unknown_6870E0(UnknownB8, 6, View, 1, 0);

	Unknown_10();
}

// auxdata.h's find through a wrapper whose six dead size() reads (no code) spend the inline budget: 0x5C4C90 then calls the find out of line (0x5AA9D0), as the client does
template<class C> inline C *Unknown_Find_Field_Outlined(DeltaCompoundClass *root, const NameClass &name) { root->Children.size(); root->Children.size(); root->Children.size(); root->Children.size(); root->Children.size(); root->Children.size(); return(Unknown_Find_Field<C>(root, name)); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_Set_Field(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); return((t && t->Unknown70) ? t : NULL); }	// NAME 1: neutral (D10): as ui_faction.cpp's (a field that is set, else NULL)



// 0x5C4BB0
void UIMissnClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		Unknown5BB320Class::Unknown_58();
		UnknownBC = 0;
		Unknown_5C5330(true);
	}
}

// 0x5C4C40
void UIMissnClass::Unknown_5C()
{
	if (Active) {
		if (UnknownC4 && UnknownC4->Active) {
			UnknownC4->Unknown_5C();
		}
		Unknown_5C5DB0(-1);
		Unknown5BB320Class::Unknown_5C();
	}
}

// 0x5C4C90
void UIMissnClass::Unknown_60()
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;

	DeltaPlayerClass *ad_player = sclient->AuxData;
	FAIL_IF(!ad_player) return;

	NameClass name("Missions");
	DeltaItemArrayClass<DeltaMissionClass> *missions = Unknown_Find_Field_Outlined<DeltaItemArrayClass<DeltaMissionClass> >((DeltaCompoundClass *)ad_player, name);
	if (missions && missions->Unknown70 > 1) {
		Unknown_5C5330(false);
	} else {
		for (int i = 0; i < MISSIONS_PER_PAGE; i++) {
			Unknown_5C5AC0(i, false);
		}
	}

	bool on = false;
	if (!sclient->Unknown1298 || !sclient->Unknown1298->Active || !UnknownC0) {
		if (CurrentMission != NO_CURRENT_MISSION) {
			DeltaMissionClass *mission = CurrentMissions[CurrentMission];
			if (mission && (mission->IsComplete.Value || mission->IsFailed.Value || mission->IsExpired.Value || mission->IsForfeitable.Value)) {
				on = true;
			}
		}
	}
	if (!on) {
		if (UnknownD8[4]->Unknown_60() != false) {
			FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
			UnknownD8[4]->Unknown_40(false);
			if (font) {
				font->Set_Hidden(true);
			}
		}
	} else {
		if (UnknownD8[4]->Unknown_60() != true) {
			FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
			UnknownD8[4]->Unknown_40(true);
			if (font) {
				font->Set_Hidden(false);
			}
		}
	}
	Unknown_5C6610();
	Unknown5BB320Class::Unknown_60();
}

// 0x5C4EA0
bool UIMissnClass::Unknown_68(int key)
{
	if (key >= 1 && key <= MISSIONS_PER_PAGE) {
		int index = key - 1;
		if (index == CurrentMission) {
			Unknown_5C5DB0(-1);
		} else {
			SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
			Unknown_5C5DB0(index);
		}
		return(true);
	}
	switch (key) {
		case 4:
			SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
			if (UnknownC0 && UnknownC4) {
				UnknownC4->Unknown_588A70("UI_MISSN", true, NULL, 0, false, false);
				return(true);
			}
			Unknown_5C66D0();
			return(true);
		case 6:
			{
				int value = UnknownB8->Value;
				if (UnknownBC != value) {
					UnknownBC = value;
					Unknown_5C5330(true);
				}
			}
			break;
		case 7:
			return(true);
	}
	return(false);
}






// 0x5C4FC0's sort: the newest mission first (StartTime +0x56C)
struct MissionStartTimeGreater	// NAME 2: describes what it does
{
	bool operator()(DeltaMissionClass *a, DeltaMissionClass *b) const { return(a->StartTime.Value > b->StartTime.Value); }
};

// 0x5C4FC0
void UIMissnClass::Unknown_5C4FC0(std::vector<DeltaMissionClass *> &missions)
{
	missions.clear(); missions.size(); missions.size();	// (two dead reads: they spend the inline budget so copy / _Destroy stay out of line, as the client's)

	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;

	DeltaPlayerClass *ad_player = sclient->AuxData;
	FAIL_IF(!ad_player) return;

	DeltaItemArrayClass<DeltaMissionClass> *ad_missions = Unknown_Find_Set_Field<DeltaItemArrayClass<DeltaMissionClass> >((DeltaCompoundClass *)ad_player, "Missions");
	FAIL_IF(!ad_missions) return;

	missions.reserve(12);
	for (int i = 0; i < 12; i++) {
		DeltaMissionClass *delta_item = (DeltaMissionClass *)ad_missions->Children[i];

		FAIL_IF(!delta_item) continue;
		DeltaMissionClass *mission = delta_item; const char *name = mission->Name.Get();
		if (name && *name) {
			missions.push_back(mission);
		}
	}
	std::sort(missions.begin(), missions.end(), MissionStartTimeGreater());
}


// 0x5C5330
void UIMissnClass::Unknown_5C5330(bool arg)
{
	std::vector<DeltaMissionClass *> missions;
	Unknown_5C4FC0(missions);
	int count = missions.size();
	while (UnknownBC && UnknownBC + MISSIONS_PER_PAGE > count) {
		UnknownBC--;
		arg = true;
	}
	int index = UnknownBC;
	for (int i = 0; i < MISSIONS_PER_PAGE; i++) {
		DeltaMissionClass *mission = (index < count) ? missions[index] : NULL;
		index++;
		bool update = false;
		if (mission == CurrentMissions[i]) {
			if (arg || (CurrentMissions[i] && CurrentMissions[i]->Unknown70 > 1)) update = true;
		} else {
			CurrentMissions[i] = mission;
			update = true;
		}
		if (update) {
			Unknown_5C5500(i);
			if (i == CurrentMission) {
				Unknown_5C5DB0(CurrentMission);
			}
		} else {
			Unknown_5C5AC0(i, true);
		}
	}
	UnknownB8->Unknown_66FA90((float)count, 3.0f, true);
	GadgetSlider *slider = UnknownB8;
	int page = UnknownBC;
	if ((int)slider->Value != page) {
		slider->Unknown_A4((float)page, false);
	}
}



















static void Unknown_Strip_Tags(std::string &text);	// 0x5C69D0 (defined after 0x5C66D0): the text without its <...> tags  // NAME 1: neutral (D10)

// 0x5C5500
void UIMissnClass::Unknown_5C5500(unsigned int cur_mission_idx)
{

	FAIL_IF(cur_mission_idx >= MISSIONS_PER_PAGE) return;
	char num_name[64];
	sprintf(num_name, "%d_TSI22A", cur_mission_idx + 1);
	FontSys *mission_num_font = Unknown_BE67C4->Get_Style(num_name, InterfaceMesh->Peek_Mesh_Object());

	FAIL_IF(!mission_num_font) return;
	char title_name[64];
	sprintf(title_name, "WRAP_Title%02d_TSI22A", cur_mission_idx);
	ManagedFontClass *title_grp = Unknown_BE67C4->Find(InterfaceMesh->Peek_Mesh_Object());

	FAIL_IF(!title_grp) return;
	char desc_name[64];
	sprintf(desc_name, "WRAP_DESC%02d_TSI22A", cur_mission_idx);
	ManagedFontClass *textgrp = Unknown_BE67C4->Find(InterfaceMesh->Peek_Mesh_Object());

	FAIL_IF(!textgrp) return;

	DeltaMissionClass *mission = CurrentMissions[cur_mission_idx];
	const char *name = mission ? mission->Name.Get() : NULL;
	if (!name || !*name) {
		CurrentMissions[cur_mission_idx] = NULL;
		Unknown_5C5AC0(cur_mission_idx, true);
		mission_num_font->Change_Text(" ", 0);
		textgrp->Unknown_6D0A70(desc_name, " ");
		title_grp->Unknown_6D0A70(title_name, " ");
		return;
	}

	Unknown_5C5AC0(cur_mission_idx, true);
	int stage = mission->Stage.Value;
	int stage_idx = stage - 1;

	mission_num_font->Unknown_6CE420(0, "%d", UnknownBC + 1 + cur_mission_idx);
	title_grp->Unknown_6D0A70(title_name, name);

	std::string text;

	if (mission->IsComplete.Value) {
		text = "Mission Complete!";
	} else if (mission->IsFailed.Value) {
		text = "Mission Failed!";
	} else if (mission->IsExpired.Value) {
		text = "Mission Expired!";
	} else {
		char buf[32];

		if (mission->StageCount.Value) {
			sprintf(buf, "(%d/%d) ", stage, mission->StageCount.Value);
		} else {
			sprintf(buf, "(%d) ", stage);
		}
		text = buf;

		G_ASSERT(stage_idx >= 0);
		if (stage_idx >= 0) {
			text += ((DeltaMissionStageClass *)mission->Stages.Children[stage_idx])->Text.Get();
		}
	}
	Unknown_Strip_Tags(text);
	textgrp->Unknown_6D0A70(desc_name, text.c_str());
}












// 0x5C5AC0
void UIMissnClass::Unknown_5C5AC0(unsigned int cur_mission_idx, bool arg)
{
	FAIL_IF(cur_mission_idx >= MISSIONS_PER_PAGE) return;
	DeltaMissionClass *mission = CurrentMissions[cur_mission_idx];
	if (!mission && !arg) return;

	char name[64];
	sprintf(name, "Time%02d_TSI22A", cur_mission_idx);
	FontSys *time_font = Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object());



	FAIL_IF(!time_font) return;

	int time = 0;
	if (mission) {
		int stage_idx = mission->Stage.Value - 1;

		G_ASSERT(stage_idx >= 0);
		if (stage_idx >= 0 && ((DeltaMissionStageClass *)mission->Stages.Children[stage_idx])->IsTimed.Value) {
			time = mission->StageExpirationTime.Value.As_Int();
		} else if (mission->IsTimed.Value) {
			time = mission->ExpirationTime.Value.As_Int();
		}
	}
	time -= ((ClientClass *)Client)->Unknown10BC;
	if (time > 0) {
		int secs = time / 1000;
		int minutes = secs / 60;
		if (minutes) {
			time_font->Unknown_6CE420(0, "%d:%02d", minutes / 60, minutes % 60);
		} else {
			time_font->Unknown_6CE420(0, "%d secs", secs);
		}
	} else {
		time_font->Unknown_6CE420(0, " ");
	}
}




// 0x5C5DB0
// NOT EXACT: register (similarity 0.58: the client keeps this in ebp, spilled, and the mission in ebx, spilled to new_mission's slot; edi is pushed only around the text; the dead reads at the end bring the appends' inline / out-of-line mix to the client's: 9 append calls, the loop's inline)
void UIMissnClass::Unknown_5C5DB0(int new_mission)
{
	if (UnknownC4 && UnknownC4->Active) {
		UnknownC4->Unknown_5C();
	}
	if (UnknownD8[4]->Unknown_60()) {
		FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
		UnknownD8[4]->Unknown_40(false);
		if (font) {
			font->Set_Hidden(true);
		}
	}
	if (new_mission != NO_CURRENT_MISSION) {
		FAIL_IF(new_mission >= MISSIONS_PER_PAGE) return;
		FAIL_IF(new_mission < 0) return;
	}
	if (CurrentMission != NO_CURRENT_MISSION) {
		UnknownD8[CurrentMission + 1]->Unknown_90(StandardBttnTexture);
	}
	CurrentMission = new_mission;



	FAIL_IF(!Get_Client()) return;
	SClient *sclient = Get_Client()->As_Space_Client();
	if (CurrentMission == NO_CURRENT_MISSION) {
		DescriptionPanel->Set_Hidden(true);
		sclient->Unknown_744560(9);
		return;
	}
	DeltaMissionClass *mission = CurrentMissions[CurrentMission];
	if (!mission) {
		Unknown_5C5DB0(NO_CURRENT_MISSION);
		return;
	}
	if (!DescriptionPanel->Is_Hidden()) {
		sclient->Unknown_744560(9);
	}
	sclient->Unknown_744560(8);
	DescriptionPanel->Set_Hidden(false);
	UnknownD8[CurrentMission + 1]->Unknown_90(SelectedBttnTexture);

	std::string text;
	const char *reward = mission->Reward.Get();
	if (reward && *reward) {
		text += "Reward: ";
		text += reward;
		text += "\n\n";
	}
	const char *failure = mission->FailureConsequence.Get();
	if (failure && *failure) {
		text += "Failure: ";
		text += failure;
		text += "\n\n";
	}
	const char *summary = mission->Summary.Get();
	if (summary && *summary) {
		text += summary;
		text += "\n\n";
	}
	text += "Mission Steps:\n";
	int count = mission->StageCount.Value;
	for (int i = 0; i < count; i++) {
		const char *step = (i < 20) ? ((DeltaMissionStageClass *)mission->Stages.Children[i])->Text.Get() : "Error: Designer exceeded MAX_STAGES";
		if (!step || !*step) break;
		char num[8];
		sprintf(num, "%d. ", i + 1);
		text += num;
		text += step;
		text += "\n";
	}
	ManagedFontClass *grp = Unknown_BE67C4->Find(DescriptionPanel);
	if (grp) {
		Unknown_Strip_Tags(text);
		grp->Unknown_6D0A70("WRAP_DESC_DETAIL_TSI22A", text.c_str());
		grp->Unknown_6D0A70("WRAP_Title_Detail_TSI22A", mission->Name.Get());
	}

	UnknownC0 = false;
	const char *label = NULL;
	if (mission->IsComplete.Value) {
		label = "Clear";
	} else if (mission->IsFailed.Value) {
		label = "Clear";
	} else if (mission->IsExpired.Value) {
		label = "Clear";
	} else if (mission->IsForfeitable.Value) {
		UnknownC0 = true;
		label = "Forfeit";
	}
	FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
	if (label) {
		if (font) {
			font->Change_Text(label, 0);
		}
		if (!sclient->Unknown1298 || !sclient->Unknown1298->Active) {
			if (UnknownD8[4]->Unknown_60() != true) {
				FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
				UnknownD8[4]->Unknown_40(true);
				if (font) {
					font->Set_Hidden(false);
				}
			}
		}
	} else {
		if (UnknownD8[4]->Unknown_60()) {
			FontSys *font = Unknown_BE67C4->Get_Style("Forfeit_TSI22A", DescriptionPanel);
			UnknownD8[4]->Unknown_40(false);
			if (font) {
				font->Set_Hidden(true);
			}
		}
	}
	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();	text.size();
}




























// 0x5C6610
void UIMissnClass::Unknown_5C6610()
{
	if (UnknownC4 && UnknownC4->Active) {
		SClient *sclient = Client->As_Space_Client();
		FAIL_IF(!sclient) return;

		if (sclient->Unknown1298 && sclient->Unknown1298->Active) {
			UnknownC4->Unknown_5C();
			return;
		}
		switch (UnknownC4->Unknown_5890B0("UI_MISSN")) {
			case 0:
				break;
			case 1:
				UnknownC4->Unknown_5C();
				Unknown_5C66D0();
				break;
			default:
				UnknownC4->Unknown_5C();
				break;
		}
	}
}


// 0x5C66D0
// NOT EXACT: register (the client reloads CurrentMissions[CurrentMission] for the packets' arguments, hoisted above the branch; this keeps the FAIL_IF's pointer)
void UIMissnClass::Unknown_5C66D0()
{
	FAIL_IF(NO_CURRENT_MISSION == CurrentMission) return;
	FAIL_IF(!CurrentMissions[CurrentMission]) return;
	FAIL_IF(!Client) return;

	int id = CurrentMissions[CurrentMission]->ID.Value;
	int playerid = ((ClientClass *)Client)->Unknown1138;
	if (UnknownC0) {
		((ClientClass *)Client)->Send_Packet(&MissionForfeitPacketClass(playerid, id));
		SoundLibraryClass::Play_2D_Sound("Mission_Forfeited", 2);
	} else {
		((ClientClass *)Client)->Send_Packet(&MissionDismissalPacketClass(playerid, id));
	}
	Unknown_5C5DB0(-1);
}

#pragma auto_inline(off)		// (0x5C5500 calls it out of line)
// 0x5C69D0
static void Unknown_Strip_Tags(std::string &text)
{
	int length = text.length();
	char *buffer = new char[length + 1];
	int count = 0;
	bool stripped = false;
	bool in_tag = false;
	const char *s = text.c_str();
	for (int i = 0; i < length; i++) {
		char c = *s;
		if (in_tag) {
			if (c == '>') {
				in_tag = false;
				stripped = true;
			}
		} else if (c == '<') {
			in_tag = true;
		} else {
			buffer[count++] = c;
		}
		s++;
	}
	if (stripped) {
		buffer[count] = '\0';
		text = buffer;
	}
	delete [] buffer;
}
#pragma auto_inline(on)


/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x5C4170-~0x5C735F: the constructor 0x5C4170 first (its strings 'TSI22A' 0xB826BC and
**	'fa_TSI22A.ini' 0xB826AC come before the __FILE__ string 0xB82708), after ui_mfdm.cpp's RefCountPtr COMDATs
**	(0x5C4030-0x5C40EF); the adjustor thunks 0x5C7340 (-4) / 0x5C7350 (-0x38) of the deleting destructor 0x5C42B0
**	close it before ui_mount.cpp. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's second tab.
*/
