// Earth & Beyond client (reconstruction): app\client\ui_starmap.cpp, the PDA's star map window. Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include <string.h>
#include <string>
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "hanim.h"
#include "gadget.h"
#include "gd_robj.h"
#include "..\kernel\kernel.h"
#include "unknown_675120.h"
#include "fontdatabase.h"
#include "control_keys_manager.h"
#include "option.h"
#include "local_interface_sort.h"
#include "ui_dynamicwindow.h"
#include "dynamic_window_class.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\common\unknown_navlist.h"
#include "..\..\ww3d2\ww3d.h"
#include "galaxy_map_pip.h"
#include "ui_starmap.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
extern unsigned int Unknown_BE5F34;			// (gadget.cpp's: the event mask)
extern KernelEventClass Unknown_BE5EB0;		// (gadget.cpp's: the last event)

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); return((t != NULL && t->Unknown70 != 0) ? t : NULL); }
bool Unknown_496240(RenderObjClass *robj, float opacity);	// 0x496240 (ro_funcs.cpp's)
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)

enum { UISTARMAP_PTR_SIZE = sizeof(RefCountPtr<UIStarmapClass>) };	// (instantiates its friends: Create_Peek(this) is 0x7C9620, converted to the slot's RefCountPtr<InterfaceClass>)

// The window's buttons, ids 1.. (0xB9F8B8)
static const char *Unknown_B9F8B8[] = { "EXIT", "SECTOR_BTTN", "RESIZE_BTTN", "QUESTION_BTTN", "CENTER_BTTN", "XYZ_BTTN", "LABEL_BTTN" };	// NAME 1: neutral (D10)

#include "gd_uiback.h"	// BoundedUIBackGadget







// MapPipClass as this file calls it: slot +0x04 takes a RefCountPtr, slots +0x1C-+0x24 a bool, and 0x693040 (map_pip.cpp's)
class UIStarmapPipViewClass  // NAME 1: neutral (D10)
{
public:
	virtual ~UIStarmapPipViewClass();
	virtual bool Unknown_04(const RefCountPtr<InterfaceClass> &iface, const RefCountPtr<RenderObjClass> &vp, int arg);	// +0x04 0x692B30  // NAME 2 [vp]: map_pip.h's
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C(bool arg);					// +0x1C  // NAME 1 [arg]: neutral
	virtual void Unknown_20(bool arg);					// +0x20  // NAME 1 [arg]: neutral
	virtual void Unknown_24(bool arg);					// +0x24  // NAME 1 [arg]: neutral
	bool Unknown_693040();								// 0x693040 (+0x40)
};

// galaxy_map_client.cpp's map (SClient +0x12A0), as far as this file reads it
class UIStarmapGalaxyMapClass  // NAME 1: neutral (D10)
{
public:
	char Unknown00[0x7C];
	int Unknown7C;										// +0x7C
	char Unknown80[0x9C - 0x80];
	std::string Unknown_9C;								// +0x9C (the system's name)  // NAME 1: neutral (D10)
	std::string Unknown_AC;								// +0xAC (the sector's name)  // NAME 1: neutral (D10)
	std::string Unknown_BC;								// +0xBC (the starbase's name)  // NAME 1: neutral (D10)
};

// (0x58B160 is ui_dynamicwindow.h's UIDynamicWindowClass::Add_Text, cdecl)

inline void UIStarmapClass::Unknown_Show_How_To(const RefCountPtr<UIDynamicWindowClass> &window, const Vector3 &position)
{
	window->Unknown_58A580(Vector2(0.0f, 0.0f));
	Vector2 size = window->MainWindow->Get_Size();
	window->Unknown_58A580(Vector2(position.X, size.Y * 0.5f + position.Y + 10.0f));
	window->Unknown_7C(View, true);
}

// @0x7C63D0
UIStarmapClass::UIStarmapClass(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	Frame(0),
	MaxFrame(0),
	MinFrame(0),
	Maximized(true),
	StarmapPip(NULL),
	GalaxyMapPip(NULL),
	CurrentPip(NULL),
	Destination(0),
	ZoomDirection(0),
	Unknown100(0)
{
	memset(Buttons, 0, sizeof(Buttons));
	StarmapPip = new StarmapPipClass(client);
	GalaxyMapPip = new GalaxyMapPipClass(client);
	CurrentPip = StarmapPip;
	Unknown_BE67BC->Load("fa_tsi26a.ini");
	MinFrame = OptionClass::Instance->Get_Float("Starmap_MinFrame");
	MaxFrame = OptionClass::Instance->Get_Float("Starmap_MaxFrame");
	Maximized = OptionClass::Instance->Get_Bool("Starmap_Maximized");
	if (MinFrame < 0.0f || MinFrame > 59.0f) {
		MinFrame = 29.0f;
	}
	if (MaxFrame < 0.0f || MaxFrame > 59.0f) {
		MaxFrame = 59.0f;
	}
	if (MinFrame > MaxFrame) {
		float t = MinFrame;
		MinFrame = MaxFrame;
		MaxFrame = t;
	}
	Frame = MaxFrame;
}

// @0x7C6700
UIStarmapClass::~UIStarmapClass()
{
	DestinationFont = NULL;
	SectorFont = NULL;
	SectorTitle = NULL;
	SectorName = NULL;
	SystemTitle = NULL;
	SystemName = NULL;
	StarbaseTitle = NULL;
	StarbaseName = NULL;
	Unknown_BE67BC->Unload("fa_tsi26a.ini");
	delete StarmapPip;
	StarmapPip = NULL;
	FloatOptionStruct *min = OptionClass::Instance->Find_Float("Starmap_MinFrame");
	FloatOptionStruct *max = OptionClass::Instance->Find_Float("Starmap_MaxFrame");
	BoolOptionStruct *maximized = OptionClass::Instance->Find_Bool("Starmap_Maximized");
	if (min) {
		min->Set_Value(MinFrame);
	}
	if (max) {
		max->Set_Value(MaxFrame);
	}
	if (maximized) {
		maximized->Set_Value(Maximized);
	}
	OptionClass::Instance->Save();
	delete GalaxyMapPip;
	GalaxyMapPip = NULL;
}

// @0x7C6BF0
void UIStarmapClass::Unknown_54()
{
	Unknown_10();
	InterfaceMesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi26a"));

















































	FAIL_IF(!InterfaceMesh) return;
	Viewport = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("frame"));

	FAIL_IF(!Viewport) return;
	MeshAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("tsi26a.tsi26a-1"));

	FAIL_IF(!MeshAnim) return;
	Unknown_BE67C4->Add(InterfaceMesh);
	DestinationFont = Create_Peek(Unknown_BE67C4->Get_Style("DESTINATION_TSI26A", InterfaceMesh.Peek()));
	SectorFont = Create_Peek(Unknown_BE67C4->Get_Style("SYSTEM_TSI26A", InterfaceMesh.Peek()));
	SystemTitle = Create_Peek(Unknown_BE67C4->Get_Style("SYSTEM_TITLE_TSI26A", InterfaceMesh.Peek()));
	SystemName = Create_Peek(Unknown_BE67C4->Get_Style("SYSTEM_NAME_TSI26A", InterfaceMesh.Peek()));
	SectorTitle = Create_Peek(Unknown_BE67C4->Get_Style("SECTOR_TITLE_TSI26A", InterfaceMesh.Peek()));
	SectorName = Create_Peek(Unknown_BE67C4->Get_Style("SECTOR_NAME_TSI26A", InterfaceMesh.Peek()));
	StarbaseTitle = Create_Peek(Unknown_BE67C4->Get_Style("STARBASE_TITLE_TSI26A", InterfaceMesh.Peek()));
	StarbaseName = Create_Peek(Unknown_BE67C4->Get_Style("STARBASE_NAME_TSI26A", InterfaceMesh.Peek()));





	G_ASSERT(DestinationFont);
	G_ASSERT(SectorFont);
	G_ASSERT(SystemTitle);
	G_ASSERT(SystemName);
	G_ASSERT(SectorTitle);
	G_ASSERT(SectorName);
	G_ASSERT(StarbaseTitle);
	G_ASSERT(StarbaseName);
	Unknown58.Unknown_686DA0(*(RenderObjClass **)&InterfaceMesh, NULL);
	ResizeUp = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("RESIZE_UP"));
	ResizeDown = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("RESIZE_DOWN"));
	for (int i = 0; i < 7; i++) {
		RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(Unknown_B9F8B8[i]);
		Unknown_496AE0(robj, false);
		ButtonGadgetClass *button = new ButtonGadgetClass(robj, 0x55, 5);
		Buttons[i] = button;
		GadgetClass::Set_Tooltip(button, "UI_STARMAP", Unknown_B9F8B8[i]);
		robj->Release_Ref();
		Unknown64.Unknown_6870E0(button, i + 1, View, 1, 0);
	}
	Unknown64.Unknown_6870E0(new BoundedUIBackGadget(InterfaceMesh), 9, View, 1, 0);
	Unknown_7C8E00(Get_Min_Frame());
	HowToStarmap = Create_Get(new UIDynamicWindowClass(Client, "STARMAP_HOW_TO", 2));
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 0, 0x10, "Starmap Click Controls:");
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 1, 0x10, "   -Left click an object icon to select warp destination");
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 2, 0x10, "   -Right click on map to center location");
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 3, 0x10, "   -Right click an object icon to lock object to center");
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 4, 0x10, "Starmap Drag Controls:");
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 5, 0x10, "   -Left drag on map to pan map around");
	HowToStarmap->Add_Text("STARMAP_HOW_TO", 6, 0x10, "   -Right drag on map to zoom in and out");
	HowToStarmap->UnknownA8 = false;
	HowToStarmap->Unknown_7C(View, false);
	HowToGalaxyMap = Create_Get(new UIDynamicWindowClass(Client, "STARMAP_HOW_TO", 2));
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 0, 0x10, "Galaxy Map Click Controls:");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 1, 0x10, "   -Left click anywhere to zoom to a location");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 2, 0x10, "   -Right click anywhere to zoom out from a location");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 3, 0x10, "   -Right click on empty space to zoom full out");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 4, 0x10, "Galaxy Map Drag Controls:");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 5, 0x10, "   -Left drag on map to pan map around");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 6, 0x10, "   -Right drag on map to zoom in and out");
	HowToGalaxyMap->Add_Text("STARMAP_HOW_TO", 7, 0x10, "   -Left drag with CTRL button to rotate map");
	HowToGalaxyMap->UnknownA8 = false;
	HowToGalaxyMap->Unknown_7C(View, false);
}

// @0x7C7B40
void UIStarmapClass::Unknown_70(ControlKeysManager *manager)
{





















	FAIL_IF(!manager) return;
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map In"), (void *)0xB, 0);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map In"), (void *)0xD, 0x1000);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map Out"), (void *)0xC, 0);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map Out"), (void *)0xE, 0x1000);
}

// @0x7C7EA0
void UIStarmapClass::Unknown_74(ControlKeysManager *manager)
{
	FAIL_IF(!manager) return;
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map In"), 0);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map In"), 0x1000);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map Out"), 0);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Zoom Map Out"), 0x1000);
}

// @0x7C81F0
void UIStarmapClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	((UIStarmapPipViewClass *)StarmapPip)->Unknown_04(Create_Peek(this), Viewport, 10);
	((UIStarmapPipViewClass *)GalaxyMapPip)->Unknown_04(Create_Peek(this), Viewport, 10);
	{
		InterfaceSortScopeClass scope2(this);
		float opacity = 1.0f;
		if (CurrentPip != (MapPipClass *)GalaxyMapPip) {
			opacity = 0.8f;
		}
		Unknown_496240(Viewport.Peek(), opacity);
		CurrentPip->Unknown_0C(true);
	}
	ResizeUp->Set_Hidden(Maximized);
	ResizeDown->Set_Hidden(!Maximized);
	InterfaceClass::Unknown_58();
	Unknown_7C8E50(true);
}

// @0x7C8440
void UIStarmapClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
	HowToStarmap->Unknown_5C();
	HowToGalaxyMap->Unknown_5C();
	Unknown_7C94C0(false);
	StarmapPip->Unknown_08();
	((MapPipClass *)GalaxyMapPip)->Unknown_08();
}

// @0x7C8490
bool UIStarmapClass::Unknown_68(int key)
{
	bool modified = (Unknown_BE5F34 & 0x70) != 0;
	switch (key) {
	case 2:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		{
			Unknown_7C8C20(Unknown_7C8BA0(0) ? 1 : 0);
		}
		return(true);
	case 1:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		{
			SClient *sclient = (SClient *)((ClientClass *)Client)->As_Space_Client();
			sclient->PDAInterface->Unknown_695A00(4, 1);
		}
		return(true);
	case 3:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		HowToStarmap->Unknown_5C();
		HowToGalaxyMap->Unknown_5C();
		if (Maximized) {
			((SClient *)((ClientClass *)Client)->As_Space_Client())->Unknown_744560(0x16);
		} else {
			((SClient *)((ClientClass *)Client)->As_Space_Client())->Unknown_744560(0x15);
		}
		Maximized = !Maximized;
		ResizeUp->Set_Hidden(Maximized);
		ResizeDown->Set_Hidden(!Maximized);
		return(true);
	case 4:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		if (HowToStarmap->Active) {
			HowToStarmap->Unknown_5C();
			return(true);
		}
		if (HowToGalaxyMap->Active) {
			HowToGalaxyMap->Unknown_5C();
			return(true);
		}
		{
			Matrix3D tm = Buttons[3]->Unknown_66C6C0();
			Vector3 position = tm.Get_Translation();
			if (CurrentPip == GalaxyMapPip) {
				Unknown_Show_How_To(HowToGalaxyMap, position);
			} else {
				Unknown_Show_How_To(HowToStarmap, position);
			}
		}
		return(true);
	case 5:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		((UIStarmapPipViewClass *)CurrentPip)->Unknown_20(modified);
		return(true);
	case 6:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		((UIStarmapPipViewClass *)CurrentPip)->Unknown_1C(modified);
		return(true);
	case 7:
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		((UIStarmapPipViewClass *)CurrentPip)->Unknown_24(modified);
		return(true);
	case 9:
		if (Unknown_BE5EB0.Key == 0x101) {
			ZoomDirection++;
		} else if (Unknown_BE5EB0.Key == 0x102) {
			ZoomDirection--;
		}
		return(true);
	case 10:
		return(true);
	case 11:
		ZoomDirection = 0xFFFF;
		return(true);
	case 12:
		ZoomDirection = -0xFFFF;
		return(true);
	case 13:
	case 14:
		ZoomDirection = 0;
		return(true);
	}
	return(false);
}

// @0x7C8980
void UIStarmapClass::Unknown_60()
{
	Unknown_7C8E50(false);
	if (CurrentPip == GalaxyMapPip) {
		GalaxyMapPip->Unknown_53F9E0();
	}
	Unknown_7C91A0();
	float target;
	if (Maximized) {
		if (Frame == MaxFrame) {
			goto done;
		}
		target = MaxFrame;
	} else {
		if (Frame == MinFrame) {
			goto done;
		}
		target = MinFrame;
	}
	{
		float step = WW3D::Get_Frame_Time() * 0.06666667f;
		if (Frame < target) {
			Frame += step;
			if (Frame >= target) {
				Frame = target;
			}
		} else {
			Frame -= step;
			if (Frame <= target) {
				Frame = target;
			}
		}
		Unknown_7C8E00(Get_Frame());
	}
done:
	if (ZoomDirection) {
		float time = WW3D::Get_Frame_Time();
		if (ZoomDirection > 0) {
			ZoomDirection--;
			if (CurrentPip == StarmapPip) {
				StarmapPip->Unknown_7C1120(time * 0.001);
			} else {
				GalaxyMapPip->Unknown_53FCA0(time * 0.001);
			}
		} else {
			ZoomDirection++;
			if (CurrentPip == StarmapPip) {
				StarmapPip->Unknown_7C1120(-time * 0.001);
			} else {
				GalaxyMapPip->Unknown_53FCA0(-time * 0.001);
			}
		}
	}
}

// @0x7C8BA0
bool UIStarmapClass::Unknown_7C8BA0(int mode)
{
	switch (mode) {
	case 0:
		return(CurrentPip == StarmapPip);
	case 1:
		return(CurrentPip == GalaxyMapPip);
	}
















































































	G_ASSERT(false);
	return(false);
}

// @0x7C8C20
void UIStarmapClass::Unknown_7C8C20(int mode)
{
	if (Unknown_7C8BA0(mode)) {
		return;
	}
	HowToStarmap->Unknown_5C();
	HowToGalaxyMap->Unknown_5C();
	bool active = CurrentPip ? ((UIStarmapPipViewClass *)CurrentPip)->Unknown_693040() : false;
	if (active) {
		Unknown_7C94C0(false);
	}
	const char *section;
	if (CurrentPip == GalaxyMapPip) {
		CurrentPip = StarmapPip;
		section = "UI_STARMAP";
	} else {
		CurrentPip = GalaxyMapPip;
		section = "UI_GALAXYMAP";
	}
	for (int i = 0; i < 7; i++) {
		GadgetClass::Set_Tooltip(Buttons[i], section, Unknown_B9F8B8[i]);
	}
	if (active) {
		Unknown_7C94C0(true);
		CurrentPip->Unknown_10(Viewport);
	}
	Unknown_7C8E50(true);
}

// @0x7C8D90
bool UIStarmapClass::Unknown_7C8D90()
{
	return(CurrentPip->Unknown_693020());
}

// @0x7C8DB0
StarmapPipClass *UIStarmapClass::Unknown_7C8DB0(bool on)
{
	if (on && (!Active || !((UIStarmapPipViewClass *)StarmapPip)->Unknown_693040())) {
		return(NULL);
	}
	return(StarmapPip);
}

// @0x7C8E00
void UIStarmapClass::Unknown_7C8E00(float frame)
{
	Frame = frame;
	InterfaceMesh->Set_Animation(MeshAnim.Peek(), frame, 0);
	CurrentPip->Unknown_10(Viewport);
}

// @0x7C8E50
bool UIStarmapClass::Unknown_7C8E50(bool force)
{





















































	FAIL_IF(!SectorFont) return(false);
	if (((UIStarmapPipViewClass *)StarmapPip)->Unknown_693040()) {

		FAIL_IF(!Client) return(false);
		SClient *sclient = (SClient *)((ClientClass *)Client)->As_Space_Client();

		FAIL_IF(!sclient) return(false);
		UIStarmapGalaxyMapClass *galaxy_map = (UIStarmapGalaxyMapClass *)sclient->GalaxyMap;

		FAIL_IF(!galaxy_map) return(false);
		if (galaxy_map->Unknown7C != Unknown100) {
			Unknown100 = galaxy_map->Unknown7C;
			StarmapPip->Unknown_14();
		} else if (!force) {
			return(false);
		}
		bool on = sclient->Unknown_744B60();
		SectorFont->Set_Hidden(on);
		DestinationFont->Set_Hidden(on);
		SystemTitle->Set_Hidden(!on);
		SystemName->Set_Hidden(!on);
		SectorTitle->Set_Hidden(!on);
		SectorName->Set_Hidden(!on);
		StarbaseTitle->Set_Hidden(!on);
		StarbaseName->Set_Hidden(!on);
		if (on) {
			SystemName->Change_Text(galaxy_map->Unknown_9C.c_str(), 0);
			SectorName->Change_Text(galaxy_map->Unknown_AC.c_str(), 0);
			StarbaseName->Change_Text(galaxy_map->Unknown_BC.c_str(), 0);
			StarmapPip->Unknown_7C2D30();
		} else {
			SectorFont->Change_Text(galaxy_map->Unknown_AC.c_str(), 0);
		}
		return(true);
	} else if (force) {
		SectorFont->Set_Hidden(true);
		SystemTitle->Set_Hidden(true);
		SystemName->Set_Hidden(true);
		SectorTitle->Set_Hidden(true);
		SectorName->Set_Hidden(true);
		StarbaseTitle->Set_Hidden(true);
		StarbaseName->Set_Hidden(true);
	}
	return(false);
}

// @0x7C91A0
bool UIStarmapClass::Unknown_7C91A0()
{
	if (((UIStarmapPipViewClass *)StarmapPip)->Unknown_693040()) {
		unsigned long id = 0;
		NavigationListClass *path = StarmapPip->Unknown_7C0EE0();
		if (path) {
			id = path->Get(-1);
		}
		if (id != Destination) {
			ClientObjectClass *object = ((ClientClass *)Client)->Find_Object(id);
			if (object) {
				NameClass field_name("Name");
				DeltaStringClass *name = Unknown_Find_Valid<DeltaStringClass>((DeltaCompoundClass *)object->AuxData, field_name);
				std::string text("Dest: ");
				text += name ? name->Get() : "";
				DestinationFont->Change_Text(text.c_str(), 0);
			} else {
				DestinationFont->Change_Text("", 0);
			}
			Destination = id;
			return(true);
		}
	} else if (Destination) {
		DestinationFont->Change_Text("", 0);
		Destination = 0;
		return(true);
	}
	return(false);
}

// @0x7C94C0
void UIStarmapClass::Unknown_7C94C0(bool on)
{
	InterfaceSortScopeClass scope(this);
	if (on) {
		float opacity = 1.0f;
		if (CurrentPip != (MapPipClass *)GalaxyMapPip) {
			opacity = 0.8f;
		}
		Unknown_496240(Viewport.Peek(), opacity);
	}
	CurrentPip->Unknown_0C(on);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\ui_starmap.cpp (the asserts' __FILE__ 0xB9FDAC).
**
**	Placement: the FAIL_IFs at lines 211-239, 301, 311, 581 and 694-703 name the file; 0x7C91A0 and 0x7C94C0 have no
**	assert but 0x7C8980 and 0x7C8C20 call them (the only callers), and their data are this window's (+0xAC, +0xCC, +0xA8).
**	The object runs 0x7C63D0-0x7C96E0: after the two come the COMDATs Create_Get 0x7C9590, Clear 0x7C95C0,
**	operator-> 0x7C9600, Create_Peek 0x7C9620 and the vtable's this-adjusters 0x7C96D0 / 0x7C96E0. The PermStringInitClass
**	at 0xBE87E4 (0x7C6380, atexit 0x7C63B0) comes first in it, after targeting.cpp's <string> initialiser 0x7C6350.
**
**	Forms worth keeping: VC6 hoists the common `fld Frame` and the pushed float out of the if / else arms in 0x7C8980;
**	the x87 load before the inlined 0x7C8E00 comes from an accessor returning float (Get_Frame), not from the member.
*/
