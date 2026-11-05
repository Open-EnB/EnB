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
#include "ui_missn.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// 0x5C4170
UIMissnClass::UIMissnClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI22A", 2), client),
	UnknownAC(NULL),
	StandardBttnTexture(NULL),
	SelectedBttnTexture(NULL),
	UnknownB8(NULL),
	UnknownBC(0),
	UnknownC0(false),
	DescriptionPanel(NULL),
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
	Unknown_BE67C4->Remove(UnknownAC);
	Unknown_BE67BC->Unload("fa_TSI22A.ini");
	REF_PTR_RELEASE(UnknownAC);
	REF_PTR_RELEASE(DescriptionPanel);
	if (UnknownB8) {
		UnknownB8->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x5C4490
void UIMissnClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (DescriptionPanel) {
		DescriptionPanel->Unknown_7C(NULL, false);
		REF_PTR_RELEASE(DescriptionPanel);
	}
	Unknown5BB320Class::Unknown_7C(view, arg);
	if (View) {
		LayerClass layer((SceneClass *)View->Unknown_50(), (CameraClass *)View->Unknown_54());
		DescriptionPanel = new ViewUnknown34Class(View, layer);
		DescriptionPanel->Unknown_54();
		DescriptionPanel->Unknown_7C(View, false);
	}
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
		if (DescriptionPanel && DescriptionPanel->Active) {
			DescriptionPanel->Unknown_5C();
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
			if (UnknownC0 && DescriptionPanel) {
				DescriptionPanel->Unknown_588A70("UI_MISSN", true, NULL, 0, false, false);
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

/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x5C4170-~0x5C735F: the constructor 0x5C4170 first (its strings 'TSI22A' 0xB826BC and
**	'fa_TSI22A.ini' 0xB826AC come before the __FILE__ string 0xB82708), after ui_mfdm.cpp's RefCountPtr COMDATs
**	(0x5C4030-0x5C40EF); the adjustor thunks 0x5C7340 (-4) / 0x5C7350 (-0x38) of the deleting destructor 0x5C42B0
**	close it before ui_mount.cpp. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's second tab.
*/
