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

// A current mission (CurrentMissions[]), as far as this file reads it
class UnknownMissionClass	// NAME 1: neutral (D10)
{
public:
	char Unknown000[0x120];								// +0x000
	int Unknown120;										// +0x120 (the mission's id for the forfeit / dismissal packets)
};

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

	int id = CurrentMissions[CurrentMission]->Unknown120;
	int playerid = ((ClientClass *)Client)->Unknown1138;
	if (UnknownC0) {
		((ClientClass *)Client)->Send_Packet(&MissionForfeitPacketClass(playerid, id));
		SoundLibraryClass::Play_2D_Sound("Mission_Forfeited", 2);
	} else {
		((ClientClass *)Client)->Send_Packet(&MissionDismissalPacketClass(playerid, id));
	}
	Unknown_5C5DB0(-1);
}

/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x5C4170-~0x5C735F: the constructor 0x5C4170 first (its strings 'TSI22A' 0xB826BC and
**	'fa_TSI22A.ini' 0xB826AC come before the __FILE__ string 0xB82708), after ui_mfdm.cpp's RefCountPtr COMDATs
**	(0x5C4030-0x5C40EF); the adjustor thunks 0x5C7340 (-4) / 0x5C7350 (-0x38) of the deleting destructor 0x5C42B0
**	close it before ui_mount.cpp. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's second tab.
*/
