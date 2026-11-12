// Earth & Beyond client (reconstruction): app\Client\ui_shipcust.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "layer.h"
#include "assetmgr.h"
#include "texture.h"
#include "matrix3d.h"
#include "aabox.h"
#include "random.h"
#include "dx8renderer.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "tutorial_manager.h"
#include "ui_shipcust.h"
#include "ship_pip.h"
#include "shiputils.h"
#include "charview.h"
#include "env_map_manager.h"
#include "gadget.h"
#include "gd_robj.h"
#include "textgadg.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "unknown_signalbase.h"
extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
extern RandomClass Unknown_BDBB84;					// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's
void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), as tsk_login.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp)
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);	// 0x495FD0 (ro_funcs.cpp): the texture onto the model
void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color);	// 0x496C00 (ro_funcs.cpp, cdecl): the model's texture colour at (u, v)  // NAME 1 [all]: what the callers pass
void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (cdecl): the pointer's position, 0..1, as scriptedmovie.cpp's

// The control keys (0x10 bytes) and InterfaceClass +0x4C's list, as ui_login.cpp declares them
#include "control_key_list.h"	// ControlKeyListClass: InterfaceClass +0x4C (Add 0x721E60, Remove 0x721EF0), keyed by kernel.h's KernelEventClass










// The gadget events' callback and the signal map, as ui_login.cpp declares them
// (the signal map: unknown_signalbase.h's SignalMap, its Add / Remove)





typedef SignalBaseClass::MemberCallback<GadgetClass, ShipCustomizeInterface, void (ShipCustomizeInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> ShipCustCallbackClass;	// NAME 1 [ShipCustCallbackClass]: neutral (D10); vtable 0xAF7970

// The decal library (decal.h): its header inlines that this file calls out of line are emitted here too (COMDATs; the
// client's copies 0x618A50, 0x618C10, 0x619280, 0x61DF40 sit in ui_ship_cust.cpp's object, which emits them as well)
#include "decal.h"


















// The colour palette's pull gadget (0x80 bytes, constructor 0x671300) and the palette with its pull (0x10 bytes,
// constructor 0x585920), as unknown_585920.cpp declares them (ui_avatr.cpp's names)
class UIMeshClass;
#include "unknown_671300.h"





















class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950
	void Unknown_5859A0();								// 0x5859A0 (the pull model onto the gadget's box centre)

	Unknown_671300Class *Gadget;					// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('PULL_COLOR')  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08 (dragging)
	float Unknown0C;									// +0x0C (0.5)
};


static char Unknown_BE5E2C[0xF];					// 0xBE5E2C (0x621530's name)  // NAME 1: neutral (D10)
static const char *Unknown_B89C60[][2] = { { "FA_SHIP01", "fa_ship01.ini" }, { "FA_SHIP02", "fa_ship02.ini" }, { "FA_SHIP03", "fa_ship03.ini" }, { "FA_SHIP04", "fa_ship04.ini" } };	// NAME 1: neutral (D10)
static const char *Unknown_B89C80[] = { "HULL00_fa_ship01", "HULL01_fa_ship01", "HULL02_fa_ship01" };	// NAME 1: neutral (D10)
static const char *Unknown_B89C8C[] = { "WING00_fa_ship01", "WING01_fa_ship01", "WING02_fa_ship01" };	// NAME 1: neutral (D10)







// @0x620D20
Unknown620D20Class::Unknown620D20Class() :
	Unknown174(0),
	Unknown178(0),
	Unknown17C(0),
	Unknown180(0)
{
}



// @0x6210A0
Unknown620D20Class::~Unknown620D20Class()
{
	Unknown_621E20();
}



















// @0x6211D0
void Unknown620D20Class::Unknown_6211D0(EnvironmentMapManagerClass *environmentmapmanager)
{
	std::vector<ColorizationRegionStruct> regions;
	G_ASSERT(environmentmapmanager != NULL);/*@L174*/
	for (int i = 0; i < 4; i++) {
		ColorizationRegionStruct region;
		ShipCustSettingsClass &first = Desc.Parts[i].Settings[0];
		ColorizationHalfStruct half0;
		if (first.Metal != METAL_NONE) {
			half0.Unknown00 = first.Metal; half0.Unknown04.Set(1.0f, 1.0f, 1.0f);
		} else {
			half0.Unknown00 = first.Is_Matte ? -2 : -1; half0.Unknown04 = first.Color;
		}
		ShipCustSettingsClass &second = Desc.Parts[i].Settings[1];
		ColorizationHalfStruct half1;
		if (second.Metal != METAL_NONE) {
			half1.Unknown00 = second.Metal; half1.Unknown04.Set(1.0f, 1.0f, 1.0f);
		} else {
			half1.Unknown00 = second.Is_Matte ? -2 : -1; half1.Unknown04 = second.Color;
		}
		region.Unknown00 = half0;
		region.Unknown10 = half1;
		regions.push_back(region);
	}
	Unknown_80EFF0(Unknown160.Peek(), regions, environmentmapmanager);
}



































// NOT EXACT: functional (D64)
// @0x621390
void Unknown620D20Class::Unknown_621390(int decal, SClientUnknown1334Class *decalmaterialmanager)
{
	std::vector<DecalStruct> decals;
	G_ASSERT(decalmaterialmanager != NULL);/*@L237*/
	if (decal >= 1) {
		decal--;
	} else if (decal >= 0) {
		decal = 0x7FFFFFF;
	}
	DecalStruct one;
	one.Unknown00 = 1;
	one.Unknown04 = decal;
	one.Unknown08.Set(1.0f, 1.0f, 1.0f, 1.0f);
	decals.insert(decals.end(), 1, one);
	one.Unknown00 = 2;
	one.Unknown04 = decal;
	one.Unknown08.Set(1.0f, 1.0f, 1.0f, 1.0f);
	decals.insert(decals.end(), 1, one);
	Unknown_80E8C0(Unknown160.Peek(), decals, decalmaterialmanager);
}



// @0x621500
void Unknown620D20Class::Unknown_621500()
{
	Unknown_80FF20(Unknown160.Peek(), Desc.Ship_Name, (float *)&Desc.Ship_Name_Color);
}


// NOT EXACT: functional (D64)
// @0x621530
char *Unknown620D20Class::Unknown_621530(int race, int part, int style, int variant)
{
	int number = style + 1;
	char race_char = 'T';
	char part_char = 'H';
	switch (race) {
		case 0: race_char = 'T'; break;
		case 1: race_char = 'J'; break;
		case 2: race_char = 'P'; break;
	}


	switch (part) {
		case 0:
			part_char = 'H';
			break;
		case 1:
			part_char = 'P';
			switch (style) {
				case 0: number = 2; break;
				case 1: number = 1; break;
				case 2: number = 3; break;
			}


			break;
		case 2:
			part_char = 'W';
			break;
		case 3:
			part_char = 'E';
			number = 0;
			break;
	}
	_snprintf(Unknown_BE5E2C, sizeof(Unknown_BE5E2C), "S%c%c%d%d", race_char, part_char, number, variant + 1);
	return(Unknown_BE5E2C);
}



// @0x621610
bool Unknown620D20Class::Unknown_621610(RenderObjClass *model, RenderObjClass *part, const char *format)
{
	char name[0x32];
	bool any = false; bool more = true;
	for (int i = 1; more; i++) {
		_snprintf(name, sizeof(name), format, i);
		if (model->Get_Bone_Index(name) != 0) {
			RenderObjClass *copy = part->Clone();
			model->Add_Sub_Object_To_Bone(copy, name);
			copy->Release_Ref();
			any = true;
		} else more = false;
	}

	return(any);
}



// @0x6216D0
void Unknown620D20Class::Unknown_6216D0(RenderObjClass *model, const char *format)
{
	if (model == NULL) {
		return;
	}
	char name[0x32];
	bool more = true;
	for (int i = 1; more; i++) {
		_snprintf(name, sizeof(name), format, i);
		int bone = model->Get_Bone_Index(name);
		if (bone != 0) {
			RenderObjClass *sub = model->Get_Sub_Object_On_Bone(0, bone);
			model->Remove_Sub_Objects_From_Bone(name);
			if (sub) {
				sub->Release_Ref();
			}
		} else more = false;
	}
}











































































































































// @0x621790
void Unknown620D20Class::Unknown_621790(EnvironmentMapManagerClass *environmentmapmanager, int decal, SClientUnknown1334Class *decalmaterialmanager)
{
	Unknown_621E20();
	int race = Desc.Race; char *name = Unknown_621530(race, 0, Desc.Hull_Style, Unknown174); Unknown160 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	name = Unknown_621530(race, 3, 0, Unknown180); Unknown164 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	Unknown_621610(Unknown160.Peek(), Unknown164.Peek(), "~03_%02d");
	name = Unknown_621530(race, 2, Desc.Wing_Style, Unknown178); Unknown168 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	Unknown_621610(Unknown168.Peek(), Unknown164.Peek(), "~03_%02d");
	int profession = Desc.Profession; name = Unknown_621530(race, 1, profession, Unknown17C); Unknown16C = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	Unknown_621610(Unknown16C.Peek(), Unknown164.Peek(), "~03_%02d");
	if (race == 2 && Desc.Profession == 0) {
		Unknown170 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("PU43"));
		Unknown_621610(Unknown16C.Peek(), Unknown170.Peek(), "~weap_%02d");
	}
	int result = Unknown160->Add_Sub_Object_To_Bone(Unknown16C.Peek(), "~01");
	G_ASSERT(result);/*@L501*/


	result = Unknown160->Add_Sub_Object_To_Bone(Unknown168.Peek(), "~02");
	G_ASSERT(result);/*@L505*/
	Unknown_6211D0(environmentmapmanager);
	Unknown_621390(decal, decalmaterialmanager);
	Unknown_80FF20(Unknown160.Peek(), Desc.Ship_Name, (float *)&Desc.Ship_Name_Color);
	Set_Engine_Glow(Unknown160.Peek(), 0);
}

#pragma auto_inline(off)	// the client calls 0x621E20 from the dtor 0x6210A0 and 0x621790

// @0x621E20
void Unknown620D20Class::Unknown_621E20()
{
	Unknown_6216D0(Unknown160.Peek(), "~03_%02d");
	Unknown_6216D0(Unknown168.Peek(), "~03_%02d");
	Unknown_6216D0(Unknown16C.Peek(), "~03_%02d");
	if (Unknown170) {
		Unknown_6216D0(Unknown16C.Peek(), "~weap_%02d");
	}
	Unknown168 = NULL;
	Unknown16C = NULL;
	Unknown164 = NULL;
	Unknown160 = NULL;
	Unknown170 = NULL;
}
#pragma auto_inline(on)

// NOT EXACT: functional (D64)
// @0x622180
ShipCustomizeInterface::ShipCustomizeInterface(UIViewClass *view, LayerClass *layer, int unknown, int unknown2) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	CharacterView(NULL),
	LoginTask(NULL),
	Unknown210(false),
	Unknown211(false),
	Unknown212(false),
	Unknown213(false),
	Unknown214(false),
	Unknown215(false),
	Unknown218(0),
	Unknown22C(NULL),
	Layer(layer),
	Page(0),
	Unknown248(false),
	Unknown24C(NULL),
	NameGadget(NULL),
	Unknown258(0),
	Unknown25C(0),
	Unknown260(0),
	Unknown264(0),
	Unknown268(NULL)
{
	Unknown220[0] = NULL;
	Unknown220[1] = NULL;
	Unknown220[2] = NULL;
	CharacterView = (CharacterViewClass *)View->Unknown_28();
	LoginTask = (LoginTaskClass *)CharacterView->Unknown14->Unknown_08();
	InterfaceSortScopeClass sort(this);
	Desc = CharacterView->Unknown244;
	if (unknown == 0) {
		if (!unknown2) {
			Desc.Set_Race(CharacterView->Unknown088.Race);
			Desc.Reset_Parts();
			Desc.Set_Profession(CharacterView->Unknown088.Profession);
		}
		CharacterView->Unknown_67C7B0();
		if (!TutorialManagerClass::Instance->Is_Completed("ShipCreationGeometry")) {
			CharacterView->Unknown_67C920(std::string("1492_00_300se.mp3"));
			CharacterView->Unknown_67C920(std::string("1492_00_301se.mp3"));
			CharacterView->Unknown_67C920(std::string("1492_00_302se.mp3"));
			TutorialManagerClass::Instance->Set_Completed("ShipCreationGeometry");
		}
	}
	CharacterView->Unknown3A4->Unknown_760770(100005, true);
	Unknown264 = (Desc.Selected_Decal == 0x7FFFFFF) ? 0 : Desc.Selected_Decal + 1;
	Unknown22C = new ShipPipClass(0);
	Unknown22C->Unknown_80C220((ViewClass *)CharacterView);
	Unknown22C->Unknown_74(0);
	Unknown22C->Unknown_65FFC0(1, NULL);
	Vector3 position(0.0f, 0.0f, 0.0f);
	Vector2 size(640.0f, 480.0f);
	Unknown22C->Unknown_80C540(size, position, 0x1C);
	Unknown64.Unknown_6870E0(Unknown22C, 0x34, View, 1, 0);
	CharacterView->Unknown_67A8D0(true);
	Unknown268 = new SClientUnknown1334Class;
	Unknown268->Load("decal_textures.ini", "decal_swatches.ini", "secret_decal_textures.ini");
	((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)0x1C);
	Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
	if (Unknown22C->Unknown054.Peek() == NULL) {
		Unknown22C->Unknown_7C(Unknown160);
	} else {
		Unknown22C->Unknown_84(Unknown160);
	}
	Unknown_625A60(Page);
	Unknown_6211D0(CharacterView->Unknown3A4);
	Unknown_621390(Unknown264, Unknown268);
	Unknown_621500();
	TheDX8MeshRenderer.Invalidate();
	void (ShipCustomizeInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ShipCustomizeInterface::Unknown_628900;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	notifier->Add(0, new ShipCustCallbackClass(this, function));
}



// @0x622B90
ShipCustomizeInterface::~ShipCustomizeInterface()
{
	Unknown_5C();
	InterfaceClass::Unknown_5C();
	Unknown22C->Unknown_80();
	Unknown22C->Unknown_80C180();
	Unknown64.Unknown_687150(Unknown22C);
	if (NameGadget) {
		Unknown64.Unknown_687150(NameGadget);
	}
	NameGadget = NULL;
	Unknown_624D00();



	if (Unknown248) {
		Unknown_BE67C4->Remove(CharacterView->Peek_Model());
		Unknown_BE67BC->Unload(Unknown_B89C60[Page][1]);
		Unknown248 = false;
	}
	ColorSliderTexture = NULL;
	if (Unknown24C) {
		delete Unknown24C;
	}
	Unknown24C = NULL;
	Unknown21C = NULL;
	if (Unknown268) {
		delete Unknown268;
	}
	Unknown268 = NULL;
	CharacterView->Unknown_67A9B0(0);
	GadgetClass::Get_Signal_Map().disconnect((GadgetClass *)NULL, this, &ShipCustomizeInterface::Unknown_628900, FlashingGadgetEvent(NULL));



}



// @0x622FC0
void ShipCustomizeInterface::Unknown_60()
{
	if (NameGadget) {
		bool changed = NameGadget->Changed;
		NameGadget->Changed = false;
		if (changed) {
			_snprintf(Desc.Ship_Name, 0x1A, "%s", NameGadget->Get_Text());
			Unknown_621500();
			NameGadget->Changed = true;
		}
		NameGadget->Unknown_6AAD10();
	}
	Unknown_624270();
	Unknown22C->Unknown_78();
	Unknown_628420();
	Unknown_624E50();
}


// NOT EXACT: functional (D64)
// @0x623080
bool ShipCustomizeInterface::Unknown_68(int key)
{
	if (Unknown211) {
		return(false);
	}
	switch (key) {
		case 1:
		case 2:
		case 3:
		case 4:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown_6257A0(key);
			Gadgets[key].Selected = true;
			Unknown218 = 5;
			Unknown213 = true;
			CharacterView->Unknown_67C7B0();
			return(false);

		case 5:
		case 6:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			Unknown218 = (key == 5) ? 5 : 0;
			Unknown215 = true;
			if (NameGadget) {
				NameGadget->Unknown_74(false);
			}
			CharacterView->Unknown_67C7B0();
			return(true);

		case 7:
		case 8:
		case 9:
		case 10:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			if (key == 7) {
				if (--Desc.Hull_Style < 0) {
					Desc.Hull_Style = 2;
				}
			} else if (key == 8) {
				if (++Desc.Hull_Style > 2) {
					Desc.Hull_Style = 0;
				}
			} else if (key == 9) {
				if (--Desc.Wing_Style < 0) {
					Desc.Wing_Style = 2;
				}
			} else {
				if (++Desc.Wing_Style > 2) {
					Desc.Wing_Style = 0;
				}
			}
			Unknown_6277A0();
			Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
			if (Unknown22C->Unknown054.Peek() == NULL) {
				Unknown22C->Unknown_7C(Unknown160);
			} else {
				Unknown22C->Unknown_84(Unknown160);
			}
			return(false);

		case 0x0B:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown_627200();
			return(false);

		case 0x10:
		case 0x11:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			*Unknown_6271A0() = (key == 0x10);
			Unknown_6257A0(key);
			Unknown_6211D0(CharacterView->Unknown3A4);
			Unknown_621390(Unknown264, Unknown268);
			Unknown_6277A0();
			return(false);

		case 0x12:
		case 0x13:
		case 0x14:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			*Unknown_6271D0() = (ShipCustomizationMetal)(key - 0x12);
			Unknown_6211D0(CharacterView->Unknown3A4);
			Unknown_6277A0();
			return(false);

		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:
		case 0x1A:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			if (key == 0x15) {
				Unknown25C = 0;
			} else if (key == 0x16) {
				Unknown25C = 1;
			} else if (key == 0x17) {
				Unknown258 = 0;
			} else if (key == 0x18) {
				Unknown258 = 2;
			} else if (key == 0x19) {
				Unknown258 = 1;
			} else {
				Unknown258 = 3;
			}
			Unknown_6257A0(key);
			Unknown_625260();
			Unknown_6277A0();
			return(false);

		case 0x1B:
		{
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			FontSys *style = Unknown_BE67C4->Get_Style("NAME_fa_ship04", CharacterView->ViewRObjs[0]);
			if (style != NULL && style->Unknown300) {
				style->Set_Hidden(1);
			}
			char *text = NameGadget->Get_Text();
			NameGadget->Set_Text((text != NULL && *text != 0) ? text : NULL, 0);
			NameGadget->Unknown_74(true);
			delete text;
			return(false);
		}

		case 0x1C:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			Unknown211 = false;
			CharacterView->Unknown_67B270();
			CharacterView->Unknown_67C7B0();
			return(true);

		case 0x1D:
		case 0x1E:
		case 0x1F:
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown264 = Unknown260 - 0x1D + key;
			Unknown_6280E0();
			Unknown_621390(Unknown264, Unknown268);
			return(false);

		case 0x29:
			if (Unknown260 != 0) {
				Unknown260--;
			}
			Unknown_6280E0();
			Unknown_6277A0();
			return(false);

		case 0x2A:
			if ((unsigned int)Unknown260 < Unknown268->TextureArray.size() - 9) {
				Unknown260++;
			}
			Unknown_6280E0();
			Unknown_6277A0();
			return(false);

		case 0x31:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown22C->Unknown_88();
			return(false);

		case 0x32:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown22C->Unknown_8C();
			return(false);

		case 0x33:
			if (Page != 1) {
				return(false);
			}
			*Unknown_6271D0() = METAL_NONE;
			Unknown_6277A0();
			return(false);
	}
	return(false);
}






























































































































































































































































































static const float Unknown_AF794C = 1.0f;	// NAME 1 [Unknown_AF794C]: neutral (D10); three .rdata 1.0f 0xAF794C / 0xAF7950 / 0xAF7954 read only by 0x624270 (fld, fchs for the negatives): named statics, one per axis, as ui_ship_cust.cpp's 0x61A4D0
static const float Unknown_AF7950 = 1.0f;	// NAME 1 [Unknown_AF7950]: neutral (D10)
static const float Unknown_AF7954 = 1.0f;	// NAME 1 [Unknown_AF7954]: neutral (D10)
static void Set_Float(float &dst, float value) { dst = value; }	// NAME 1 [Set_Float]: neutral (D10)
static bool Is_Pressed(GadgetClass *gadget) { return Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget; }	// NAME 1 [Is_Pressed]: neutral (D10)
static void Copy_Desc(ShipCustDescClass &dst, const ShipCustDescClass &src) { dst = src; }	// NAME 1 [Copy_Desc]: neutral (D10)
// NOT EXACT: functional (D64)
// @0x624270
void ShipCustomizeInterface::Unknown_624270()
{
	GadgetClass *gadget;
	gadget = Gadgets[0x0F].Gadget;
	if (Is_Pressed(gadget)) {
		Set_Float(Unknown22C->Unknown0B8, Unknown_AF794C);
	} else {
		gadget = Gadgets[0x0E].Gadget;
		if (Is_Pressed(gadget)) {
			Set_Float(Unknown22C->Unknown0B8, -Unknown_AF794C);
		} else {
			gadget = Gadgets[0x0C].Gadget;
			if (Is_Pressed(gadget)) {
				Set_Float(Unknown22C->Unknown0BC, Unknown_AF794C);
			} else {
				gadget = Gadgets[0x0D].Gadget;
				if (Is_Pressed(gadget)) {
					Set_Float(Unknown22C->Unknown0BC, -Unknown_AF794C);
				} else {
					Unknown22C->Unknown0B8 = 0.0f;
					Unknown22C->Unknown0BC = 0.0f;
				}
			}
		}
	}
	gadget = Gadgets[0x2C].Gadget;
	if (Is_Pressed(gadget)) {
		Set_Float(Unknown22C->Unknown0C0, -Unknown_AF7950);
	} else {
		gadget = Gadgets[0x2B].Gadget;
		if (Is_Pressed(gadget)) {
			Set_Float(Unknown22C->Unknown0C0, Unknown_AF7950);
		} else {
			gadget = Gadgets[0x2D].Gadget;
			if (Is_Pressed(gadget)) {
				Set_Float(Unknown22C->Unknown0C4, -Unknown_AF7950);
			} else {
				gadget = Gadgets[0x2E].Gadget;
				if (Is_Pressed(gadget)) {
					Set_Float(Unknown22C->Unknown0C4, Unknown_AF7950);
				} else {
					Unknown22C->Unknown0C0 = 0.0f;
					Unknown22C->Unknown0C4 = 0.0f;
				}
			}
		}
	}
	gadget = Gadgets[0x2F].Gadget;
	if (Is_Pressed(gadget)) {
		Set_Float(Unknown22C->Unknown0C8, -Unknown_AF7954);
	} else {
		gadget = Gadgets[0x30].Gadget;
		if (Is_Pressed(gadget)) {
			Set_Float(Unknown22C->Unknown0C8, Unknown_AF7954);
		} else {
			Unknown22C->Unknown0C8 = 0.0f;
		}
	}
	if (Unknown213) {
		Unknown213 = false;
		switch (Unknown_625930(0)) {
			case 1:
				Unknown_625A60(0);
				return;
			case 2:
				Unknown_625A60(2);
				return;
			case 3:
				Unknown_625A60(3);
				return;
			case 4:
				Unknown_625A60(1);
				return;
			default:
				G_ASSERT(0);/*@L1223*/
				break;
		}
	}
	if (Unknown215) {
		if (Unknown218 == 5) {
			Desc.Selected_Decal = (Unknown264 == 0) ? 0x7FFFFFF : Unknown264 - 1;
			Copy_Desc(CharacterView->Unknown244, Desc);
			LoginTask->Go_Next_Step();
		} else {
			LoginTask->Go_Back_Step();
		}
	}
	if (CharacterView->Unknown_67B2B0()) {
		Unknown_6249D0(true);
		Unknown_6277A0();
	}
	if (CharacterView->Unknown34->Active) {
		if (!Unknown211) {
			Unknown211 = true;
			Unknown_6249D0(false);
		}
		return;
	}
	if (Unknown211) {
		Unknown211 = false;
		Unknown_6249D0(true);
		Unknown_6277A0();
	}
	Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); Gadgets.size(); }	// dead reads (D130; not the client's): with the while loop and the file-static inlines they spend VC6's inline budget as the client does: map::insert out of line at the first four lookups, the step's _Inc 0x6296C0 out of line with the group-0 lookup through the inlined 0x625930 (its extra inline layer puts _Tree::begin 0x628F00 out of line too); 13-15 reads, 10-12 leave three lookups deep


// @0x6249D0
void ShipCustomizeInterface::Unknown_6249D0(bool on)
{
	std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin();
	while (it != Gadgets.end()) {
		it->second.Gadget->Unknown_40(on);
		if (it->second.Style != NULL) {
			FontSys *font = Unknown_BE67C4->Get_Style(it->second.Style, CharacterView->Peek_Model()); font->Unknown_Set_2FC(on ? 1.0f : 0.33f);
		}
		it++;
	}
	if (Unknown24C != NULL) {
		Unknown24C->Gadget->Unknown_40(on);
	}
	if (Unknown22C != NULL) {
		Unknown22C->Unknown_40(on);
	}
}


// @0x624AE0
void ShipCustomizeInterface::Unknown_624AE0(const char *style, bool on)
{
	RenderObjClass *model = CharacterView->ViewRObjs[0]; FontSys *font = Unknown_BE67C4->Get_Style(style, model);
	if (font != NULL) { int hidden = !on; bool now = !font->Unknown300; if (now != hidden) {
		font->Set_Hidden(hidden); }
	}
}



// @0x624B50
void ShipCustomizeInterface::Unknown_624B50(int id, const char *name, const char *style, short key, short group)
{
	RenderObjClass *sub = CharacterView->ViewRObjs[0]->Get_Sub_Object_By_Name(name);
	Unknown_496AE0(sub, 0);
	GadgetClass *button = new ButtonGadgetClass(sub, *Layer, 5, 3);
	ShipCustGadgetStruct entry;
	entry.Gadget = button;
	entry.Name = name;
	entry.Style = style;
	entry.Id = id;
	entry.Key = key;
	entry.Group = group;
	entry.Selected = false;
	Gadgets.insert(std::map<int, ShipCustGadgetStruct>::value_type(id, entry));
	Unknown64.Unknown_6870E0(button, id, View, 1, 0);
	if (key != 0) {
		((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(key), (void *)id);
	}
	sub->Release_Ref();
}



// @0x624D00
void ShipCustomizeInterface::Unknown_624D00()
{
	while (Gadgets.begin() != Gadgets.end()) {
		Unknown64.Unknown_687150(Gadgets.begin()->second.Gadget);
		Gadgets.erase(Gadgets.begin());
	}
}



// @0x624D70
void ShipCustomizeInterface::Unknown_624D70()
{
	Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
	if (Unknown22C->Unknown054.Peek() == NULL) {
		Unknown22C->Unknown_7C(Unknown160);
		return;
	}
	Unknown22C->Unknown_84(Unknown160);
}



// @0x624DF0
void ShipCustomizeInterface::Unknown_624DF0()
{
	Unknown_6211D0(CharacterView->Unknown3A4);
}



// @0x624E20
void ShipCustomizeInterface::Unknown_624E20()
{
	Unknown_621390(Unknown264, Unknown268);
}


// NOT EXACT: functional (D64)
// @0x624E50
void ShipCustomizeInterface::Unknown_624E50()
{
	if (Page != 1 && Page != 3) {
		return;
	}
	if (Unknown24C->Gadget->Unknown64) {
		Unknown24C->Unknown08 = true;
	}
	if ((!Unknown24C->Gadget->Unknown64 && !Unknown24C->Gadget->Unknown65) || !Unknown24C->Unknown08) {
		Unknown24C->Unknown08 = false;
	} else {
		AABoxClass box;
		Unknown24C->Gadget->Unknown_64(box);
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		if (x >= box.Center.X + box.Extent.X) {
			x = box.Center.X + box.Extent.X;
		}
		if (x <= box.Center.X - box.Extent.X) {
			x = box.Center.X - box.Extent.X;
		}
		if (y >= box.Center.Y + box.Extent.Y) {
			y = box.Center.Y + box.Extent.Y;
		}
		if (y <= box.Center.Y - box.Extent.Y) {
			y = box.Center.Y - box.Extent.Y;
		}
		RenderObjClass *model = CharacterView->ViewRObjs[0];
		int bone = model->Get_Sub_Object_Bone_Index(Unknown24C->Pull);
		model->Capture_Bone(bone);
		Matrix3D tm = Unknown24C->Pull->Get_Transform();
		tm[0][3] = x - box.Center.X;
		tm[1][3] = y - box.Center.Y;
		model->Control_Bone(bone, tm, false);
		float u = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		float v = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		Vector3 *color = (Page == 1) ? &Desc.Parts[Unknown258].Settings[Unknown25C].Color : &Desc.Ship_Name_Color;
		Unknown_496C00(((RObjGadgetClass *)Unknown24C->Gadget->As_RObjGadget())->RObj, u, 1.0f - v, color);
		Unknown_6251F0(Vector2(u, v));
		if (Page == 1) {
			Unknown_6211D0(CharacterView->Unknown3A4);
		} else {
			Unknown_621500();
		}
	}
	Unknown24C->Gadget->Unknown_671630();
}


#pragma auto_inline(off)	// every client caller calls 0x6251F0 out of line
// @0x6251F0
void ShipCustomizeInterface::Unknown_6251F0(Vector2 uv)
{
	float *position = Unknown_627110();
	position[0] = uv.X;
	position[1] = uv.Y;
}
#pragma auto_inline(on)


// @0x625260
void ShipCustomizeInterface::Unknown_625260()
{
	AABoxClass box;
	Unknown24C->Gadget->Unknown_64(box);
	float y = box.Center.Y - box.Extent.Y; float x = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f; float height = box.Extent.Y * 2.0f;
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	int bone = model->Get_Sub_Object_Bone_Index(Unknown24C->Pull);
	model->Capture_Bone(bone);
	Matrix3D tm = Unknown24C->Pull->Get_Transform();
	tm[0][3] = width * Unknown_627110()[0] + x - box.Center.X; tm[1][3] = height * Unknown_627110()[1] + y - box.Center.Y;
	model->Control_Bone(bone, tm, false);
}

































































































































































// @0x625440
void ShipCustomizeInterface::Unknown_625440()
{
	Unknown24C = new UIAvatrPullClass;
	RenderObjClass *palette = CharacterView->ViewRObjs[0]->Get_Sub_Object_By_Name("COLOR_PALETTE");
	Unknown_496AE0(palette, 0);
	if (Page == 1) {
		ColorSliderTexture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("fa_ship_color.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	} else {
		ColorSliderTexture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("fa_name_color.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	}
	G_ASSERT(ColorSliderTexture);/*@L1600*/
	Unknown_495FD0(palette, ColorSliderTexture.Peek(), 0);
	Unknown24C->Gadget = new Unknown_671300Class(palette, 0, *Layer, 0, 0, 0);
	Unknown24C->Gadget->Unknown2C_0A = 1;
	Unknown24C->Unknown_5859A0();
	palette->Release_Ref();
	Unknown24C->Pull = CharacterView->ViewRObjs[0]->Get_Sub_Object_By_Name("PULL_COLOR");
	Unknown_496AE0(Unknown24C->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown24C->Gadget, 0x33, View, 1, 1);
	Unknown_625260();
}



// @0x625760
void ShipCustomizeInterface::Unknown_625760(bool on)
{
	Unknown24C->Pull->Set_Hidden(!on);
}



// @0x6257A0
void ShipCustomizeInterface::Unknown_6257A0(int id)
{

	Unknown_6259C0(Gadgets[id].Group);




	Gadgets[id].Gadget->Unknown2C_0B = 1;
	Gadgets[id].Gadget->Unknown_6603A0(false, NULL);
	Gadgets[id].Selected = true;
}



// @0x625930
int ShipCustomizeInterface::Unknown_625930(short group)
{
	std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin();
	while (it != Gadgets.end()) {
		if (it->second.Group == group && it->second.Selected) {
			return(it->second.Id);
		}
		it++;
	}
	return(0);
}


// @0x6259C0
void ShipCustomizeInterface::Unknown_6259C0(short group)
{
	bool found = false;	// set, never read (D130): its weight keeps 0x6257A0, which inlines this loop, calling _Inc out of line as the client does
	std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin();
	while (it != Gadgets.end()) {
		if (it->second.Group == group) {
			it->second.Gadget->Unknown2C_0B = 0;
			it->second.Gadget->Unknown_6607D0();
			it->second.Selected = false;
			found = true;
		}
		it++;
	}
}

// NOT EXACT: functional (D64)
// @0x625A60
void ShipCustomizeInterface::Unknown_625A60(int page)
{
	InterfaceSortScopeClass sort(this);
	Unknown_5C();
	InterfaceClass::Unknown_5C();
	Unknown_624D00();
	if (Unknown248) {
		Unknown_BE67C4->Remove(CharacterView->ViewRObjs[0]);
		Unknown_BE67BC->Unload(Unknown_B89C60[Page][1]);
		Unknown248 = false;
	}
	if (Page == 1) {
		Unknown220[0] = NULL;
		Unknown220[1] = NULL;
		Unknown220[2] = NULL;
		Unknown64.Unknown_687150(Unknown24C->Gadget);
		if (Unknown24C) {
			delete Unknown24C;
		}
		Unknown24C = NULL;
	} else if (Page == 2) {
		Unknown21C = NULL;
	} else if (Page == 3) {
		Unknown64.Unknown_687150(NameGadget);
		NameGadget = NULL;
		Unknown64.Unknown_687150(Unknown24C->Gadget);
		if (Unknown24C) {
			delete Unknown24C;
		}
		Unknown24C = NULL;
	}
	CharacterView->Unknown_67A9B0(0);
	CharacterView->Unknown_67AB20(0, Unknown_B89C60[page][0]);
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	Unknown_BE67BC->Load(Unknown_B89C60[page][1]);
	Unknown_BE67C4->Add(model);
	Page = page;
	Unknown248 = true;
	Unknown_624B50(1, "HULL_TAB", "hull_tab_fa_ship", 0, 0);
	Unknown_624B50(2, "INSIGNIA_TAB", "insignia_tab_fa_ship", 0, 0);
	Unknown_624B50(3, "NAME_TAB", "name_tab_fa_ship", 0, 0);
	Unknown_624B50(4, "PAINT_TAB", "paint_tab_fa_ship", 0, 0);
	const char *tutorial = NULL;
	switch (Page) {
		case 0:
			Unknown_624B50(5, "CONFIRM", "confirm_fa_ship01", 0x0D, 0);
			Unknown_624B50(6, "EXIT", "exit_fa_ship01", 0, 0);
			Unknown_624B50(7, "HULL_LEFT", NULL, 0, 0);
			Unknown_624B50(8, "HULL_RIGHT", NULL, 0, 0);
			Unknown_624B50(9, "WING_LEFT", NULL, 0, 0);
			Unknown_624B50(10, "WING_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x0B, "RANDOM", "RANDOM_fa_ship01", 0, 0);
			Unknown_624B50(0x0C, "TURN_UP", NULL, 0, 0);
			Unknown_624B50(0x0D, "TURN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x0E, "TURN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x0F, "TURN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2B, "PAN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x2C, "PAN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2D, "PAN_UP", NULL, 0, 0);
			Unknown_624B50(0x2E, "PAN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x2F, "ZOOM_IN", NULL, 0, 0);
			Unknown_624B50(0x30, "ZOOM_OUT", NULL, 0, 0);
			Unknown_624B50(0x31, "PAN", NULL, 0, 0);
			Unknown_624B50(0x32, "TURN", NULL, 0, 0);
			Unknown_6257A0(1);
			Gadgets[1].Selected = true;
			Unknown_6277A0();
			break;

		case 1:
			Unknown_624B50(5, "CONFIRM", "confirm_fa_ship02", 0x0D, 0);
			Unknown_624B50(6, "EXIT", "exit_fa_ship02", 0, 0);
			Unknown_624B50(0x11, "GLOSSY", "glossy_fa_ship02", 0, 4);
			Unknown_624B50(0x10, "FLAT", "flat_fa_ship02", 0, 4);
			Unknown_624B50(0x0B, "RANDOM", "RANDOM_fa_ship02", 0, 0);
			Unknown_624B50(0x12, "METAL00", NULL, 0, 0);
			Unknown_624B50(0x13, "METAL01", NULL, 0, 0);
			Unknown_624B50(0x14, "METAL02", NULL, 0, 0);
			Unknown220[0] = ((RObjGadgetClass *)Gadgets[0x12].Gadget)->RObj;
			Unknown220[1] = ((RObjGadgetClass *)Gadgets[0x13].Gadget)->RObj;
			Unknown220[2] = ((RObjGadgetClass *)Gadgets[0x14].Gadget)->RObj;
			Unknown_624B50(0x15, "PRIMARY", "primary_fa_ship02", 0, 3);
			Unknown_624B50(0x16, "SECONDARY", "secondary_fa_ship02", 0, 3);
			Unknown_624B50(0x17, "HULL", "hull_fa_ship02", 0, 1);
			Unknown_624B50(0x18, "WING", "wing_fa_ship02", 0, 1);
			Unknown_624B50(0x19, "PROFESSION", "profession_fa_ship02", 0, 1);
			Unknown_624B50(0x1A, "ENGINE", "engine_fa_ship02", 0, 1);
			Unknown_624B50(0x0C, "TURN_UP", NULL, 0, 0);
			Unknown_624B50(0x0D, "TURN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x0E, "TURN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x0F, "TURN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2B, "PAN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x2C, "PAN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2D, "PAN_UP", NULL, 0, 0);
			Unknown_624B50(0x2E, "PAN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x2F, "ZOOM_IN", NULL, 0, 0);
			Unknown_624B50(0x30, "ZOOM_OUT", NULL, 0, 0);
			Unknown_624B50(0x31, "PAN", NULL, 0, 0);
			Unknown_624B50(0x32, "TURN", NULL, 0, 0);
			Unknown_6257A0(4);
			Unknown_625440();
			Unknown_625260();
			Unknown_6277A0();
			CharacterView->Unknown_67C7B0();
			if (TutorialManagerClass::Instance->Is_Completed("ShipCreationColor")) {
				break;
			}
			CharacterView->Unknown_67C920(std::string("1492_00_306se.mp3"));
			CharacterView->Unknown_67C920(std::string("1492_00_307se.mp3"));
			CharacterView->Unknown_67C920(std::string("1492_00_308se.mp3"));
			tutorial = "ShipCreationColor";
			break;

		case 2:
			Unknown_624B50(5, "CONFIRM", "confirm_fa_ship03", 0x0D, 0);
			Unknown_624B50(6, "EXIT", "exit_fa_ship03", 0, 0);
			Unknown_624B50(0x0B, "RANDOM", "RANDOM_fa_ship03", 0, 0);
			Unknown_624B50(0x0C, "TURN_UP", NULL, 0, 0);
			Unknown_624B50(0x0D, "TURN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x0E, "TURN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x0F, "TURN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x29, "LEFT", NULL, 0, 0);
			Unknown_624B50(0x2A, "RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2B, "PAN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x2C, "PAN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2D, "PAN_UP", NULL, 0, 0);
			Unknown_624B50(0x2E, "PAN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x2F, "ZOOM_IN", NULL, 0, 0);
			Unknown_624B50(0x30, "ZOOM_OUT", NULL, 0, 0);
			Unknown_624B50(0x31, "PAN", NULL, 0, 0);
			Unknown_624B50(0x32, "TURN", NULL, 0, 0);
			Unknown_6257A0(2);
			Unknown_627FA0();
			{
				int count = Unknown268->TextureArray.size();
				int first = Unknown264;
				if (first < 0) {
					first = 0;
				} else if (first > count - 9) {
					first = count - 9;
				}
				Unknown260 = first;
			}
			Unknown_6280E0();
			Unknown_6277A0();
			CharacterView->Unknown_67C7B0();
			if (TutorialManagerClass::Instance->Is_Completed("ShipCreationDecal")) {
				break;
			}
			CharacterView->Unknown_67C920(std::string("1492_00_311se.mp3"));
			tutorial = "ShipCreationDecal";
			break;

		case 3:
			Unknown_624B50(5, "CONFIRM", "confirm_fa_ship04", 0x0D, 0);
			Unknown_624B50(6, "EXIT", "exit_fa_ship04", 0, 0);
			Unknown_624B50(0x0B, "RANDOM", "RANDOM_fa_ship04", 0, 0);
			Unknown_624B50(0x1B, "NAME", "NAME_fa_ship04", 0, 0);
			Unknown_624B50(0x0C, "TURN_UP", NULL, 0, 0);
			Unknown_624B50(0x0D, "TURN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x0E, "TURN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x0F, "TURN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2B, "PAN_LEFT", NULL, 0, 0);
			Unknown_624B50(0x2C, "PAN_RIGHT", NULL, 0, 0);
			Unknown_624B50(0x2D, "PAN_UP", NULL, 0, 0);
			Unknown_624B50(0x2E, "PAN_DOWN", NULL, 0, 0);
			Unknown_624B50(0x2F, "ZOOM_IN", NULL, 0, 0);
			Unknown_624B50(0x30, "ZOOM_OUT", NULL, 0, 0);
			Unknown_624B50(0x31, "PAN", NULL, 0, 0);
			Unknown_624B50(0x32, "TURN", NULL, 0, 0);
			Unknown_6257A0(3);
			NameGadget = new TextGadgetClass(this, 0x19, "NAME_fa_ship04", CharacterView->ViewRObjs[0], Unknown_B08CB8 | Unknown_B08CC4, 0);
			NameGadget->Unknown78 = 10.0f;
			NameGadget->Unknown_28(Layer);
			if (strlen(Desc.Ship_Name) != 0) {
				FontSys *style = Unknown_BE67C4->Get_Style("NAME_fa_ship04", CharacterView->ViewRObjs[0]);
				if (style != NULL && style->Unknown300) {
					style->Set_Hidden(1);
				}
				NameGadget->Set_Text(Desc.Ship_Name, 0);
			}
			Unknown64.Unknown_6870E0(NameGadget, 0x36, View, 1, 0);
			Unknown_625440();
			CharacterView->Unknown_67C7B0();
			if (TutorialManagerClass::Instance->Is_Completed("ShipCreationName")) {
				break;
			}
			CharacterView->Unknown_67C920(std::string("1492_00_312se.mp3"));
			tutorial = "ShipCreationName";
			break;
	}
	if (tutorial != NULL) {
		TutorialManagerClass::Instance->Set_Completed(tutorial);
	}
	InterfaceClass::Unknown_58();
	Unknown_58();
	TheDX8MeshRenderer.Invalidate();
}



// @0x627110
float *ShipCustomizeInterface::Unknown_627110()
{
	if (Page == 1) {
		return(&Desc.Parts[Unknown258].Settings[Unknown25C].Unknown10);
	}
	return(&Desc.Unknown3C.X);
}



// @0x627160
Vector3 *ShipCustomizeInterface::Unknown_627160()
{
	if (Page == 1) {
		return(&Desc.Parts[Unknown258].Settings[Unknown25C].Color);
	}
	return(&Desc.Ship_Name_Color);
}



// @0x6271A0
bool *ShipCustomizeInterface::Unknown_6271A0()
{
	return(&Desc.Parts[Unknown258].Settings[Unknown25C].Is_Matte);
}



// @0x6271D0
ShipCustomizationMetal *ShipCustomizeInterface::Unknown_6271D0()
{
	return(&Desc.Parts[Unknown258].Settings[Unknown25C].Metal);
}


// NOT EXACT: functional (D64)
// @0x627200
void ShipCustomizeInterface::Unknown_627200()
{
	switch (Page) {
		case 0:
			Desc.Hull_Style = Unknown_BDBB84(0, 2);
			Desc.Wing_Style = Unknown_BDBB84(0, 2);
			Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
			if (Unknown22C->Unknown054.Peek() == NULL) {
				Unknown22C->Unknown_7C(Unknown160);
			} else {
				Unknown22C->Unknown_84(Unknown160);
			}
			Unknown_6277A0();
			return;

		case 1:
		{
			int part = Unknown258;
			int setting = Unknown25C;
			for (int s = 0; s < 2; s++) {
				Unknown25C = s;
				int metal_or_paint = Unknown_BDBB84(0, 1);
				int matte = Unknown_BDBB84(0, 1);
				int metal = Unknown_BDBB84(0, 2);
				Vector2 uv;

				Vector3 color;
				if (metal_or_paint == 0) {
					uv.X = (float)Unknown_BDBB84(0, 100) * 0.01f;
					uv.Y = (float)Unknown_BDBB84(0, 100) * 0.01f;
					RenderObjClass *robj = ((RObjGadgetClass *)Unknown24C->Gadget->As_RObjGadget())->RObj; Unknown_496C00(robj, uv.X, 1.0f - uv.Y, &color);
				}
				for (int p = 0; p < 4; p++) {
					Unknown258 = p;
					if (metal_or_paint != 0) {
						*Unknown_6271D0() = (ShipCustomizationMetal)metal;
					} else {
						Unknown_627730(color);
						Unknown_6251F0(uv);
						*Unknown_6271D0() = METAL_NONE;
						*Unknown_6271A0() = (matte != 0);
					}
				}
			}
			Unknown258 = part;
			Unknown25C = setting;
			Unknown_6211D0(CharacterView->Unknown3A4);
			Unknown_625260();
			Unknown_6277A0();
			return;
		}

		case 2:
		{
			int count = Unknown268->TextureArray.size();
			Unknown264 = Unknown_BDBB84(1, count);
			count = Unknown268->TextureArray.size();
			if (Unknown264 < count - 10) {
				Unknown260 = Unknown264;
			} else {
				Unknown260 = count - 10;
			}
			Unknown_6280E0();
			Unknown_621390(Unknown264, Unknown268);
			Unknown_6277A0();
			return;
		}

		case 3:
		{
			Vector2 *position = &Desc.Unknown3C;
			position->X = (float)Unknown_BDBB84(0, 100) * 0.01f; position->Y = (float)Unknown_BDBB84(0, 100) * 0.01f;
			Vector3 color;
			RenderObjClass *robj = ((RObjGadgetClass *)Unknown24C->Gadget->As_RObjGadget())->RObj; Unknown_496C00(robj, Desc.Unknown3C.X, 1.0f - Desc.Unknown3C.Y, &color);
			Unknown_627730(color);
			Unknown_6251F0(Desc.Unknown3C);
			Unknown_625260();
			Unknown_621500();
			Unknown_6277A0();
			return;
		}
	}
	Unknown_6277A0();
}


#pragma auto_inline(off)	// every client caller calls 0x627730 out of line
// @0x627730
void ShipCustomizeInterface::Unknown_627730(Vector3 color)
{
	Vector3 *setting = Unknown_627160();
	setting->X = color.X;
	setting->Y = color.Y;
	setting->Z = color.Z;
}
#pragma auto_inline(on)


// @0x6277A0
void ShipCustomizeInterface::Unknown_6277A0()
{
	int i;
	switch (Page) {
		case 2:
		{
			unsigned int count = Unknown268->TextureArray.size();
			bool right = (unsigned int)Unknown260 < count - 9;
			bool left = (unsigned int)Unknown260 > 0;
			if (Gadgets[0x2A].Gadget->Unknown_60() != right) {
				Gadgets[0x2A].Gadget->Unknown_40(right);
			}
			if (Gadgets[0x29].Gadget->Unknown_60() != left) {
				Gadgets[0x29].Gadget->Unknown_40(left);
			}
			Unknown_6257A0(2);
			return;
		}

		case 0:
			for (i = 0; i < 3; i++) {
				Unknown_624AE0(Unknown_B89C80[i], i == Desc.Hull_Style);
			}
			for (i = 0; i < 3; i++) {
				Unknown_624AE0(Unknown_B89C8C[i], i == Desc.Wing_Style);
			}
			Unknown_6257A0(1);
			return;

		case 1:
			if (Unknown25C == 0) {
				Unknown_6257A0(0x15);
			} else {
				Unknown_6257A0(0x16);
			}
			if (*Unknown_6271A0()) {
				Unknown_6257A0(0x10);
			} else {
				Unknown_6257A0(0x11);
			}
			switch (Unknown258) {
				case 0:
					Unknown_6257A0(0x17);
					break;
				case 2:
					Unknown_6257A0(0x18);
					break;
				case 1:
					Unknown_6257A0(0x19);
					break;
				case 3:
					Unknown_6257A0(0x1A);
					break;
			}
			Unknown_625760(*Unknown_6271D0() == METAL_NONE);
			if (*Unknown_6271D0() != METAL_NONE) {
				Gadgets[0x11].Gadget->Unknown_40(false);
				Gadgets[0x10].Gadget->Unknown_40(false);
			} else {
				Gadgets[0x11].Gadget->Unknown_40(true);
				Gadgets[0x10].Gadget->Unknown_40(true);
			}
			Unknown_6257A0(4);
			return;

		case 3:
			Unknown_6257A0(3);
			return;
	}
}



// @0x627FA0
void ShipCustomizeInterface::Unknown_627FA0()
{
	char name[0x14];
	Unknown_624B50(0x1D, "D00", "NONE_fa_ship03", 0, 2);
	for (int i = 1; i < 10; i++) {
		_snprintf(name, sizeof(name), "D%02d", i);
		Unknown_624B50(i + 0x1D, name, NULL, 0, 2);
	}
	Unknown21C = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("BLACK.TGA", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
}


// @0x6280E0
void ShipCustomizeInterface::Unknown_6280E0()
{
	Unknown_6259C0(2);

	FontSys *none = Unknown_BE67C4->Get_Style("NONE_fa_ship03", CharacterView->Peek_Model());
	if (none != NULL && none->Unknown300) {
		none->Set_Hidden(1);
	}
	char name[0x14];
	for (int i = 0; i < 10; i++) {
		_snprintf(name, sizeof(name), "D%02d", i);
		RenderObjClass *sub = CharacterView->Peek_Model()->Get_Sub_Object_By_Name(name);
		if (sub == NULL) {
			continue;
		}
		if (Unknown260 + i == 0) {

			none = Unknown_BE67C4->Get_Style("NONE_fa_ship03", CharacterView->Peek_Model());
			if (none != NULL && !none->Unknown300) {
				none->Set_Hidden(0);
			}
			Unknown_495FD0(sub, Unknown21C.Peek(), 0);
		} else {
			RefCountPtr<TextureClass> texture = Unknown268->Unknown_61DF40(Unknown260 + i - 1);
			Unknown_495FD0(sub, texture.Peek(), 0);
			if (Unknown260 + i == Unknown264) {
				Unknown_6257A0(i + 0x1D);
			}
		}
		sub->Release_Ref();
	} Gadgets.size(); Gadgets.size();	// dead reads (D130; not the client's): they spend VC6's inline budget, so the inlined 0x6257A0 keeps its second Gadgets[] inline and calls the rest out of line as the client's (with one, not exact)
}



// @0x628420
void ShipCustomizeInterface::Unknown_628420()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	float slow = time * 0.3f;
	float fast = time + time;
	if (Unknown220[0] != NULL) {
		Matrix3D tm = Unknown220[0]->Get_Transform(); if (*Unknown_6271D0() == METAL_CHROME) { tm.Rotate_Y(fast); } else { tm.Rotate_Y(slow); }
		Unknown220[0]->Set_Transform(tm); Gadgets[0x12].Gadget->Unknown_6608F0();
	}
	if (Unknown220[1] != NULL) {
		Matrix3D tm = Unknown220[1]->Get_Transform(); if (*Unknown_6271D0() == METAL_BRONZE) { tm.Rotate_Y(fast); } else { tm.Rotate_Y(slow); }
		Unknown220[1]->Set_Transform(tm); Gadgets[0x13].Gadget->Unknown_6608F0();
	}
	if (Unknown220[2] != NULL) {
		Matrix3D tm = Unknown220[2]->Get_Transform(); if (*Unknown_6271D0() == METAL_COPPER) { tm.Rotate_Y(fast); } else { tm.Rotate_Y(slow); }
		Unknown220[2]->Set_Transform(tm); Gadgets[0x14].Gadget->Unknown_6608F0();
	}
}


// @0x6288C0
bool ShipCustomizeInterface::Unknown_78(void *arg)
{
	if (((int *)arg)[3] == 6 && ((int *)arg)[4] == 0x11) {
		Unknown_Debug_Print("BAD SHIP NAME");
	}
	return(true);
}
















































































































































































































































































































































































// @0x628900
void ShipCustomizeInterface::Unknown_628900(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event) return;/*@L2531*/
	GadgetClass *gadget = event->Gadget;

	FAIL_IF(!gadget) return;/*@L2534*/
	if (gadget->Flash && gadget->Is_Unknown2C_02()) {
		for (std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
			if (it->second.Gadget == gadget) {
				int id = it->second.Id;
				if (id == 5 || id == 6 || id == 0x1C || id == 0x1B || id == 0x29 || id == 0x2A || id == 0x0B) {
					SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
				}
				return;
			}
		}
	}
}


/*
**	Notes
**
**	The ship customisation window of character creation (the character view's mode 6, LoginTaskClass step 6),
**	client 0x620D20-0x62A7C0 (the std::map<int, ShipCustGadgetStruct> members it instantiates from 0x628A70).
**	__FILE__ 'e:\build\beta\G\app\Client\ui_shipcust.cpp' (0xB89DEC) from the asserts of 0x6211D0 (l.174),
**	0x621390 (l.237), 0x621790 (l.501, 505), 0x624270 (l.1223), 0x625440 (l.1600) and 0x628900 (l.2531, 2534).
**	Functional (D64) for the POC client: written from the saved pseudocode and the disassembly.
**
**	Pages (0x625A60): 0 FA_SHIP01 hull and wing styles, 1 FA_SHIP02 paint (parts, primary / secondary, glossy /
**	flat, metals, the colour palette), 2 FA_SHIP03 insignia (the decal library's textures, ten at a time), 3
**	FA_SHIP04 the ship's name and its colour. Tabs (group 0): 1 hull, 2 insignia, 3 name, 4 paint. CONFIRM (5,
**	enter) copies the description to the character view's +0x244 and calls LoginTaskClass::Go_Next_Step (step 6
**	to 5, ui_shard.cpp's galaxy select); EXIT (6) Go_Back_Step.
**
**	The ship preview gadget (0x80B2E0, ship_pip.cpp) and shiputils.cpp's 0x80EFF0 / 0x80E8C0 / 0x80FF20 /
**	0x810750 are declared in the block near the top until those files land.
*/
