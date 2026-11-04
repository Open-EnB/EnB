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
#include "..\shared\unknown_debug.h"
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
struct UIShipCustKeyEventStruct	// NAME 1: neutral (D10): kernel.h's input event
{
	UIShipCustKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
struct ControlKeyComboStruct; class ControlKeyListClass { public: void Add(const ControlKeyComboStruct &combo, void *handler); };	// InterfaceClass +0x4C: control_keys_manager.h's key list (Add 0x721E60), as ui_login.cpp declares it

// The gadget events' callback and the signal map, as ui_login.cpp declares them
class InterfaceSignalMapClass : public SignalMap		// NAME 1: neutral (D10): intrface.cpp's
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback) { connect(sender, callback); }			// 0x4F9740 (the client's 'SignalMap::connect')
	void Remove(void *sender, const SignalBaseClass::CallbackClass &callback) { disconnect(sender, callback); }	// 0x4F98D0 (the client's 'SignalMap::disconnect')
};
typedef SignalBaseClass::MemberCallback<GadgetClass, ShipCustomizeInterface, void (ShipCustomizeInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> ShipCustCallbackClass;	// NAME 1 [ShipCustCallbackClass]: neutral (D10); vtable 0xAF7970

// The decal library (0xB8 bytes; sclient.cpp's declaration, with the member this file calls)
class Unknown619280Class		// NAME 1: neutral (D10): 0x44 bytes, constructor 0x619280
{
public:
	Unknown619280Class();								// 0x619280
	~Unknown619280Class();
	char Unknown00[0x44];
};
class SClientUnknown1334Class
{
public:
	~SClientUnknown1334Class();							// 0x618A50
	void Load(const char *textures, const char *swatches, const char *secret_textures);	// 0x75BE00 (decal.cpp)  // NAME 3 [textures, swatches, secret_textures]: the INI names
	RefCountPtr<TextureClass> Unknown_61DF40(int index);	// 0x61DF40 (the insignia's texture)  // NAME 1 [index]: neutral
	std::vector<int> Unknown00;							// +0x00 (its size: the insignia count)
	std::vector<int> Unknown10;							// +0x10
	std::vector<int> Unknown20;							// +0x20
	Unknown619280Class Unknown30;						// +0x30
	Unknown619280Class Unknown74;						// +0x74
};

// The colour palette's pull gadget (0x80 bytes, constructor 0x671300) and the palette with its pull (0x10 bytes,
// constructor 0x585920), as unknown_585920.cpp declares them (ui_avatr.cpp's names)
class UIMeshClass;
class UIAvatrUnknown4A1240Class;
class UIAvatrGadget671300Class : public ButtonGadgetClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d);	// 0x671300 (mesh: the window's layer)  // NAME 1 [area, a, mesh, b, c, d]: as ui_avatr.cpp's
	virtual ~UIAvatrGadget671300Class();				// +0x00 0x6714F0 (deleting 0x6713E0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x671570 (the drag)  // NAME 1 [event, x, y, mask]: gadget.h's
	virtual void Unknown_A4();							// +0xA4 0x671650
	virtual void Unknown_A8();							// +0xA8 0x671670
	virtual void Unknown_AC();							// +0xAC 0x6716B0
	// NOT EXACT: out of line in the client (0x671630, beside 0x671300); inline here until that file lands
	void Unknown_671630() { Unknown64 = false; Unknown65 = false; Unknown66 = false; }	// 0x671630 (the frame's end: the drag's flags cleared)

	bool Unknown64;										// +0x64 (pressed)
	bool Unknown65;										// +0x65 (held)
	bool Unknown66;										// +0x66
	UIAvatrUnknown4A1240Class *Unknown68;				// +0x68
	int Unknown6C;										// +0x6C
	int Unknown70;										// +0x70
	bool Unknown74;										// +0x74
	int Unknown78;										// +0x78 (the constructor's b)
	int Unknown7C;										// +0x7C (the constructor's c)
};
class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950
	void Unknown_5859A0();								// 0x5859A0 (the pull model onto the gadget's box centre)

	UIAvatrGadget671300Class *Gadget;					// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('PULL_COLOR')  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08 (dragging)
	float Unknown0C;									// +0x0C (0.5)
};


static char Unknown_BE5E2C[0xF];					// 0xBE5E2C (0x621530's name)  // NAME 1: neutral (D10)
static const char *Unknown_B89C60[][2] = { { "FA_SHIP01", "fa_ship01.ini" }, { "FA_SHIP02", "fa_ship02.ini" }, { "FA_SHIP03", "fa_ship03.ini" }, { "FA_SHIP04", "fa_ship04.ini" } };	// NAME 1: neutral (D10)
static const char *Unknown_B89C80[] = { "HULL00_fa_ship01", "HULL01_fa_ship01", "HULL02_fa_ship01" };	// NAME 1: neutral (D10)
static const char *Unknown_B89C8C[] = { "WING00_fa_ship01", "WING01_fa_ship01", "WING02_fa_ship01" };	// NAME 1: neutral (D10)

char *Unknown_621530(int race, int part, int style, int variant);	// NAME 1 [all]: neutral
bool Unknown_621610(RenderObjClass *model, RenderObjClass *part, const char *format);	// NAME 1 [all]: neutral
void Unknown_6216D0(RenderObjClass *model, const char *format);	// NAME 1 [all]: neutral


// NOT EXACT: functional (D64)
// @0x620D20
Unknown620D20Class::Unknown620D20Class() :
	Unknown174(0),
	Unknown178(0),
	Unknown17C(0),
	Unknown180(0)
{
}


// NOT EXACT: functional (D64)
// @0x6210A0
Unknown620D20Class::~Unknown620D20Class()
{
	Unknown_621E20();
}


// NOT EXACT: functional (D64)
// @0x6211D0
void Unknown620D20Class::Unknown_6211D0(EnvironmentMapManagerClass *environmentmapmanager)
{
	std::vector<ColorizationRegionStruct> regions;
	G_ASSERT(environmentmapmanager != NULL);/*@L174*/
	for (int i = 0; i < 4; i++) {
		ColorizationRegionStruct region;
		ShipCustSettingsClass &first = Parts[i].Settings[0];
		region.Unknown00.Unknown00 = first.Metal;
		if (first.Metal == METAL_NONE) {
			region.Unknown00.Unknown04 = first.Color;
			region.Unknown00.Unknown00 = -1 - (first.Is_Matte ? 1 : 0);
		} else {
			region.Unknown00.Unknown04.Set(1.0f, 1.0f, 1.0f);
		}
		ShipCustSettingsClass &second = Parts[i].Settings[1];
		region.Unknown10.Unknown00 = second.Metal;
		if (second.Metal == METAL_NONE) {
			region.Unknown10.Unknown04 = second.Color;
			region.Unknown10.Unknown00 = -1 - (second.Is_Matte ? 1 : 0);
		} else {
			region.Unknown10.Unknown04.Set(1.0f, 1.0f, 1.0f);
		}
		regions.insert(regions.end(), 1, region);
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


// NOT EXACT: functional (D64)
// @0x621500
void Unknown620D20Class::Unknown_621500()
{
	Unknown_80FF20(Unknown160.Peek(), Ship_Name, (float *)&Ship_Name_Color);
}


// NOT EXACT: functional (D64)
// @0x621530
char *Unknown_621530(int race, int part, int style, int variant)
{
	int number = style + 1;
	char race_char = 'T';
	char part_char = 'H';
	if (race == 0) {
		race_char = 'T';
	} else if (race == 1) {
		race_char = 'J';
	} else if (race == 2) {
		race_char = 'P';
	}
	switch (part) {
		case 0:
			part_char = 'H';
			break;
		case 1:
			part_char = 'P';
			if (style == 0) {
				number = 2;
			} else if (style == 1) {
				number = 1;
			} else if (style == 2) {
				number = 3;
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


// NOT EXACT: functional (D64)
// @0x621610
bool Unknown_621610(RenderObjClass *model, RenderObjClass *part, const char *format)
{
	char name[0x32];
	bool any = false;
	for (int i = 1; ; i++) {
		_snprintf(name, sizeof(name), format, i);
		if (model->Get_Bone_Index(name) == 0) {
			break;
		}
		RenderObjClass *copy = part->Clone();
		model->Add_Sub_Object_To_Bone(copy, name);
		copy->Release_Ref();
		any = true;
	}
	return(any);
}


// NOT EXACT: functional (D64)
// @0x6216D0
void Unknown_6216D0(RenderObjClass *model, const char *format)
{
	if (model == NULL) {
		return;
	}
	char name[0x32];
	for (int i = 1; ; i++) {
		_snprintf(name, sizeof(name), format, i);
		int bone = model->Get_Bone_Index(name);
		if (bone == 0) {
			break;
		}
		RenderObjClass *sub = model->Get_Sub_Object_On_Bone(0, bone);
		model->Remove_Sub_Objects_From_Bone(name);
		if (sub) {
			sub->Release_Ref();
		}
	}
}


// NOT EXACT: functional (D64)
// @0x621790
void Unknown620D20Class::Unknown_621790(EnvironmentMapManagerClass *environmentmapmanager, int decal, SClientUnknown1334Class *decalmaterialmanager)
{
	Unknown_621E20();
	Unknown160 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_621530(Race, 0, Hull_Style, Unknown174)));
	Unknown164 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_621530(Race, 3, 0, Unknown180)));
	Unknown_621610(Unknown160.Peek(), Unknown164.Peek(), "~03_%02d");
	Unknown168 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_621530(Race, 2, Wing_Style, Unknown178)));
	Unknown_621610(Unknown168.Peek(), Unknown164.Peek(), "~03_%02d");
	Unknown16C = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_621530(Race, 1, Profession, Unknown17C)));
	Unknown_621610(Unknown16C.Peek(), Unknown164.Peek(), "~03_%02d");
	if (Race == 2 && Profession == 0) {
		Unknown170 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("PU43"));
		Unknown_621610(Unknown16C.Peek(), Unknown170.Peek(), "~weap_%02d");
	}
	int result = Unknown160->Add_Sub_Object_To_Bone(Unknown16C.Peek(), "~01");
	G_ASSERT(result);/*@L501*/
	result = Unknown160->Add_Sub_Object_To_Bone(Unknown168.Peek(), "~02");
	G_ASSERT(result);/*@L505*/
	Unknown_6211D0(environmentmapmanager);
	Unknown_621390(decal, decalmaterialmanager);
	Unknown_80FF20(Unknown160.Peek(), Ship_Name, (float *)&Ship_Name_Color);
	Unknown_810750(Unknown160.Peek(), 0);
}


// NOT EXACT: functional (D64)
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
	(ShipCustDescClass &)Ship = CharacterView->Unknown244;
	if (unknown == 0) {
		if (!unknown2) {
			Ship.Set_Race(CharacterView->Unknown088.Race);
			Ship.Reset_Parts();
			Ship.Set_Profession(CharacterView->Unknown088.Profession);
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
	Unknown264 = (Ship.Selected_Decal == 0x7FFFFFF) ? 0 : Ship.Selected_Decal + 1;
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
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIShipCustKeyEventStruct(0x1B), (void *)0x1C);
	Ship.Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
	if (Unknown22C->Unknown054.Peek() == NULL) {
		Unknown22C->Unknown_7C(Ship.Unknown160);
	} else {
		Unknown22C->Unknown_84(Ship.Unknown160);
	}
	Unknown_625A60(Page);
	Ship.Unknown_6211D0(CharacterView->Unknown3A4);
	Ship.Unknown_621390(Unknown264, Unknown268);
	Unknown_80FF20(Ship.Unknown160.Peek(), Ship.Ship_Name, (float *)&Ship.Ship_Name_Color);
	TheDX8MeshRenderer.Invalidate();
	void (ShipCustomizeInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ShipCustomizeInterface::Unknown_628900;
	InterfaceSignalMapClass *notifier = (InterfaceSignalMapClass *)&GadgetClass::Get_Signal_Map();
	notifier->Add(0, new ShipCustCallbackClass(this, function));
}


// NOT EXACT: functional (D64)
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
	while (Gadgets.begin() != Gadgets.end()) {
		Unknown64.Unknown_687150(Gadgets.begin()->second.Gadget);
		Gadgets.erase(Gadgets.begin());
	}
	if (Unknown248) {
		Unknown_BE67C4->Remove(CharacterView->ViewRObjs[0]);
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
	void (ShipCustomizeInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ShipCustomizeInterface::Unknown_628900;
	InterfaceSignalMapClass *notifier = (InterfaceSignalMapClass *)&GadgetClass::Get_Signal_Map();
	ShipCustCallbackClass callback(this, function);
	notifier->Remove(0, callback);
}


// NOT EXACT: functional (D64)
// @0x622FC0
void ShipCustomizeInterface::Unknown_60()
{
	if (NameGadget) {
		bool changed = NameGadget->Changed;
		NameGadget->Changed = false;
		if (changed) {
			_snprintf(Ship.Ship_Name, 0x1A, "%s", NameGadget->Get_Text());
			Unknown_80FF20(Ship.Unknown160.Peek(), Ship.Ship_Name, (float *)&Ship.Ship_Name_Color);
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
				if (--Ship.Hull_Style < 0) {
					Ship.Hull_Style = 2;
				}
			} else if (key == 8) {
				if (++Ship.Hull_Style > 2) {
					Ship.Hull_Style = 0;
				}
			} else if (key == 9) {
				if (--Ship.Wing_Style < 0) {
					Ship.Wing_Style = 2;
				}
			} else {
				if (++Ship.Wing_Style > 2) {
					Ship.Wing_Style = 0;
				}
			}
			Unknown_6277A0();
			Ship.Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
			if (Unknown22C->Unknown054.Peek() == NULL) {
				Unknown22C->Unknown_7C(Ship.Unknown160);
			} else {
				Unknown22C->Unknown_84(Ship.Unknown160);
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
			Ship.Unknown_6211D0(CharacterView->Unknown3A4);
			Ship.Unknown_621390(Unknown264, Unknown268);
			Unknown_6277A0();
			return(false);

		case 0x12:
		case 0x13:
		case 0x14:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			*Unknown_6271D0() = (ShipCustomizationMetal)(key - 0x12);
			Ship.Unknown_6211D0(CharacterView->Unknown3A4);
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
			Ship.Unknown_621390(Unknown264, Unknown268);
			return(false);

		case 0x29:
			if (Unknown260 != 0) {
				Unknown260--;
			}
			Unknown_6280E0();
			Unknown_6277A0();
			return(false);

		case 0x2A:
			if ((unsigned int)Unknown260 < Unknown268->Unknown00.size() - 9) {
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


// NOT EXACT: functional (D64)
// @0x624270
void ShipCustomizeInterface::Unknown_624270()
{
	GadgetClass *gadget;
	gadget = Gadgets[0x0F].Gadget;
	if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
		Unknown22C->Unknown0B8 = 1.0f;
	} else {
		gadget = Gadgets[0x0E].Gadget;
		if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
			Unknown22C->Unknown0B8 = -1.0f;
		} else {
			gadget = Gadgets[0x0C].Gadget;
			if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
				Unknown22C->Unknown0BC = 1.0f;
			} else {
				gadget = Gadgets[0x0D].Gadget;
				if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
					Unknown22C->Unknown0BC = -1.0f;
				} else {
					Unknown22C->Unknown0B8 = 0.0f;
					Unknown22C->Unknown0BC = 0.0f;
				}
			}
		}
	}
	gadget = Gadgets[0x2C].Gadget;
	if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
		Unknown22C->Unknown0C0 = -1.0f;
	} else {
		gadget = Gadgets[0x2B].Gadget;
		if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
			Unknown22C->Unknown0C0 = 1.0f;
		} else {
			gadget = Gadgets[0x2D].Gadget;
			if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
				Unknown22C->Unknown0C4 = -1.0f;
			} else {
				gadget = Gadgets[0x2E].Gadget;
				if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
					Unknown22C->Unknown0C4 = 1.0f;
				} else {
					Unknown22C->Unknown0C0 = 0.0f;
					Unknown22C->Unknown0C4 = 0.0f;
				}
			}
		}
	}
	gadget = Gadgets[0x2F].Gadget;
	if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
		Unknown22C->Unknown0C8 = -1.0f;
	} else {
		gadget = Gadgets[0x30].Gadget;
		if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
			Unknown22C->Unknown0C8 = 1.0f;
		} else {
			Unknown22C->Unknown0C8 = 0.0f;
		}
	}
	if (Unknown213) {
		Unknown213 = false;
		int id = 0;
		for (std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
			if (it->second.Group == 0 && it->second.Selected) {
				id = it->second.Id;
				break;
			}
		}
		switch (id) {
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
			Ship.Selected_Decal = (Unknown264 == 0) ? 0x7FFFFFF : Unknown264 - 1;
			CharacterView->Unknown244 = Ship;
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
}


// NOT EXACT: functional (D64)
// @0x6249D0
void ShipCustomizeInterface::Unknown_6249D0(bool on)
{
	for (std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
		it->second.Gadget->Unknown_40(on);
		if (it->second.Style != NULL) {
			Unknown_BE67C4->Get_Style(it->second.Style, CharacterView->ViewRObjs[0])->Unknown_Set_2FC(on ? 1.0f : 0.33f);
		}
	}
	if (Unknown24C != NULL) {
		Unknown24C->Gadget->Unknown_40(on);
	}
	if (Unknown22C != NULL) {
		Unknown22C->Unknown_40(on);
	}
}


// NOT EXACT: functional (D64)
// @0x624AE0
void ShipCustomizeInterface::Unknown_624AE0(const char *style, bool on)
{
	FontSys *font = Unknown_BE67C4->Get_Style(style, CharacterView->ViewRObjs[0]);
	if (font != NULL && (!font->Unknown300) != (!on)) {
		font->Set_Hidden(!on);
	}
}


// NOT EXACT: functional (D64)
// @0x624B50
void ShipCustomizeInterface::Unknown_624B50(int id, const char *name, const char *style, short key, short group)
{
	RenderObjClass *sub = CharacterView->ViewRObjs[0]->Get_Sub_Object_By_Name(name);
	Unknown_496AE0(sub, 0);
	GadgetClass *button = new ButtonGadgetClass(sub, *Layer, 5, 3);
	ShipCustGadgetStruct entry;
	entry.Gadget = button;
	entry.Id = id;
	entry.Name = name;
	entry.Style = style;
	entry.Key = key;
	entry.Group = group;
	entry.Selected = false;
	Gadgets.insert(std::map<int, ShipCustGadgetStruct>::value_type(id, entry));
	Unknown64.Unknown_6870E0(button, id, View, 1, 0);
	if (key != 0) {
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIShipCustKeyEventStruct(key), (void *)id);
	}
	sub->Release_Ref();
}


// NOT EXACT: functional (D64)
// @0x624D00
void ShipCustomizeInterface::Unknown_624D00()
{
	while (Gadgets.begin() != Gadgets.end()) {
		Unknown64.Unknown_687150(Gadgets.begin()->second.Gadget);
		Gadgets.erase(Gadgets.begin());
	}
}


// NOT EXACT: functional (D64)
// @0x624D70
void ShipCustomizeInterface::Unknown_624D70()
{
	Ship.Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
	if (Unknown22C->Unknown054.Peek() == NULL) {
		Unknown22C->Unknown_7C(Ship.Unknown160);
		return;
	}
	Unknown22C->Unknown_84(Ship.Unknown160);
}


// NOT EXACT: functional (D64)
// @0x624DF0
void ShipCustomizeInterface::Unknown_624DF0()
{
	Ship.Unknown_6211D0(CharacterView->Unknown3A4);
}


// NOT EXACT: functional (D64)
// @0x624E20
void ShipCustomizeInterface::Unknown_624E20()
{
	Ship.Unknown_621390(Unknown264, Unknown268);
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
		Vector3 *color = (Page == 1) ? &Ship.Parts[Unknown258].Settings[Unknown25C].Color : &Ship.Ship_Name_Color;
		Unknown_496C00(((RObjGadgetClass *)Unknown24C->Gadget->Unknown_08())->RObj, u, 1.0f - v, color);
		Unknown_6251F0(u, v);
		if (Page == 1) {
			Ship.Unknown_6211D0(CharacterView->Unknown3A4);
		} else {
			Unknown_80FF20(Ship.Unknown160.Peek(), Ship.Ship_Name, (float *)&Ship.Ship_Name_Color);
		}
	}
	Unknown24C->Gadget->Unknown_671630();
}


// NOT EXACT: functional (D64)
// @0x6251F0
void ShipCustomizeInterface::Unknown_6251F0(float u, float v)
{
	float *position = Unknown_627110();
	position[0] = u;
	position[1] = v;
}


// NOT EXACT: functional (D64)
// @0x625260
void ShipCustomizeInterface::Unknown_625260()
{
	AABoxClass box;
	Unknown24C->Gadget->Unknown_64(box);
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	int bone = model->Get_Sub_Object_Bone_Index(Unknown24C->Pull);
	model->Capture_Bone(bone);
	Matrix3D tm = Unknown24C->Pull->Get_Transform();
	float *position = Unknown_627110();
	tm[0][3] = (box.Extent.X + box.Extent.X) * position[0] - box.Extent.X;
	tm[1][3] = (box.Extent.Y + box.Extent.Y) * position[1] - box.Extent.Y;
	model->Control_Bone(bone, tm, false);
}


// NOT EXACT: functional (D64)
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
	Unknown24C->Gadget = new UIAvatrGadget671300Class(palette, 0, (UIMeshClass *)Layer, 0, 0, 0);
	Unknown24C->Gadget->Unknown2C_0A = 1;
	Unknown24C->Unknown_5859A0();
	palette->Release_Ref();
	Unknown24C->Pull = CharacterView->ViewRObjs[0]->Get_Sub_Object_By_Name("PULL_COLOR");
	Unknown_496AE0(Unknown24C->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown24C->Gadget, 0x33, View, 1, 1);
	Unknown_625260();
}


// NOT EXACT: functional (D64)
// @0x625760
void ShipCustomizeInterface::Unknown_625760(bool on)
{
	Unknown24C->Pull->Set_Hidden(!on);
}


// NOT EXACT: functional (D64)
// @0x6257A0
void ShipCustomizeInterface::Unknown_6257A0(int id)
{
	short group = Gadgets[id].Group;
	for (std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
		if (it->second.Group == group) {
			it->second.Gadget->Unknown2C_0B = 0;
			it->second.Gadget->Unknown_6607D0();
			it->second.Selected = false;
		}
	}
	Gadgets[id].Gadget->Unknown2C_0B = 1;
	Gadgets[id].Gadget->Unknown_6603A0(false, NULL);
	Gadgets[id].Selected = true;
}


// NOT EXACT: functional (D64)
// @0x625930
int ShipCustomizeInterface::Unknown_625930(short group)
{
	for (std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
		if (it->second.Group == group && it->second.Selected) {
			return(it->second.Id);
		}
	}
	return(0);
}


// NOT EXACT: functional (D64)
// @0x6259C0
void ShipCustomizeInterface::Unknown_6259C0(short group)
{
	for (std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
		if (it->second.Group == group) {
			it->second.Gadget->Unknown2C_0B = 0;
			it->second.Gadget->Unknown_6607D0();
			it->second.Selected = false;
		}
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
				int count = Unknown268->Unknown00.size();
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
			if (strlen(Ship.Ship_Name) != 0) {
				FontSys *style = Unknown_BE67C4->Get_Style("NAME_fa_ship04", CharacterView->ViewRObjs[0]);
				if (style != NULL && style->Unknown300) {
					style->Set_Hidden(1);
				}
				NameGadget->Set_Text(Ship.Ship_Name, 0);
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


// NOT EXACT: functional (D64)
// @0x627110
float *ShipCustomizeInterface::Unknown_627110()
{
	if (Page == 1) {
		return(&Ship.Parts[Unknown258].Settings[Unknown25C].Unknown10);
	}
	return(&Ship.Unknown3C.X);
}


// NOT EXACT: functional (D64)
// @0x627160
Vector3 *ShipCustomizeInterface::Unknown_627160()
{
	if (Page == 1) {
		return(&Ship.Parts[Unknown258].Settings[Unknown25C].Color);
	}
	return(&Ship.Ship_Name_Color);
}


// NOT EXACT: functional (D64)
// @0x6271A0
bool *ShipCustomizeInterface::Unknown_6271A0()
{
	return(&Ship.Parts[Unknown258].Settings[Unknown25C].Is_Matte);
}


// NOT EXACT: functional (D64)
// @0x6271D0
ShipCustomizationMetal *ShipCustomizeInterface::Unknown_6271D0()
{
	return(&Ship.Parts[Unknown258].Settings[Unknown25C].Metal);
}


// NOT EXACT: functional (D64)
// @0x627200
void ShipCustomizeInterface::Unknown_627200()
{
	switch (Page) {
		case 0:
			Ship.Hull_Style = Unknown_BDBB84(0, 2);
			Ship.Wing_Style = Unknown_BDBB84(0, 2);
			Ship.Unknown_621790(CharacterView->Unknown3A4, Unknown264, Unknown268);
			if (Unknown22C->Unknown054.Peek() != NULL) {
				Unknown22C->Unknown_84(Ship.Unknown160);
			} else {
				Unknown22C->Unknown_7C(Ship.Unknown160);
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
				float u = 0.0f;
				float v = 0.0f;
				Vector3 color(0.0f, 0.0f, 0.0f);
				if (metal_or_paint == 0) {
					u = (float)Unknown_BDBB84(0, 100) * 0.01f;
					v = (float)Unknown_BDBB84(0, 100) * 0.01f;
					Unknown_496C00(((RObjGadgetClass *)Unknown24C->Gadget->Unknown_08())->RObj, u, 1.0f - v, &color);
				}
				for (int p = 0; p < 4; p++) {
					Unknown258 = p;
					if (metal_or_paint == 0) {
						Unknown_627730(color.X, color.Y, color.Z);
						Unknown_6251F0(u, v);
						*Unknown_6271D0() = METAL_NONE;
						*Unknown_6271A0() = (matte != 0);
					} else {
						Ship.Parts[p].Settings[s].Metal = (ShipCustomizationMetal)metal;
					}
				}
			}
			Unknown258 = part;
			Unknown25C = setting;
			Ship.Unknown_6211D0(CharacterView->Unknown3A4);
			Unknown_625260();
			Unknown_6277A0();
			return;
		}

		case 2:
		{
			int count = Unknown268->Unknown00.size();
			Unknown264 = Unknown_BDBB84(1, count);
			count = Unknown268->Unknown00.size();
			if (Unknown264 < count - 10) {
				Unknown260 = Unknown264;
			} else {
				Unknown260 = count - 10;
			}
			Unknown_6280E0();
			Ship.Unknown_621390(Unknown264, Unknown268);
			Unknown_6277A0();
			return;
		}

		case 3:
		{
			Ship.Unknown3C.X = (float)Unknown_BDBB84(0, 100) * 0.01f;
			Ship.Unknown3C.Y = (float)Unknown_BDBB84(0, 100) * 0.01f;
			Vector3 color(0.0f, 0.0f, 0.0f);
			Unknown_496C00(((RObjGadgetClass *)Unknown24C->Gadget->Unknown_08())->RObj, Ship.Unknown3C.X, 1.0f - Ship.Unknown3C.Y, &color);
			Unknown_627730(color.X, color.Y, color.Z);
			Unknown_6251F0(Ship.Unknown3C.X, Ship.Unknown3C.Y);
			Unknown_625260();
			Unknown_80FF20(Ship.Unknown160.Peek(), Ship.Ship_Name, (float *)&Ship.Ship_Name_Color);
			Unknown_6277A0();
			return;
		}
	}
	Unknown_6277A0();
}


// NOT EXACT: functional (D64)
// @0x627730
void ShipCustomizeInterface::Unknown_627730(float r, float g, float b)
{
	Vector3 *color = Unknown_627160();
	color->X = r;
	color->Y = g;
	color->Z = b;
}


// NOT EXACT: functional (D64)
// @0x6277A0
void ShipCustomizeInterface::Unknown_6277A0()
{
	int i;
	switch (Page) {
		case 0:
			for (i = 0; i < 3; i++) {
				Unknown_624AE0(Unknown_B89C80[i], i == Ship.Hull_Style);
			}
			for (i = 0; i < 3; i++) {
				Unknown_624AE0(Unknown_B89C8C[i], i == Ship.Wing_Style);
			}
			Unknown_6257A0(1);
			return;

		case 1:
			Unknown_6257A0((Unknown25C == 0) ? 0x15 : 0x16);
			Unknown_6257A0(*Unknown_6271A0() ? 0x10 : 0x11);
			switch (Unknown258) {
				case 0:
					Unknown_6257A0(0x17);
					break;
				case 1:
					Unknown_6257A0(0x19);
					break;
				case 2:
					Unknown_6257A0(0x18);
					break;
				case 3:
					Unknown_6257A0(0x1A);
					break;
			}
			Unknown24C->Pull->Set_Hidden(*Unknown_6271D0() != METAL_NONE);
			if (*Unknown_6271D0() == METAL_NONE) {
				Gadgets[0x11].Gadget->Unknown_40(true);
				Gadgets[0x10].Gadget->Unknown_40(true);
			} else {
				Gadgets[0x11].Gadget->Unknown_40(false);
				Gadgets[0x10].Gadget->Unknown_40(false);
			}
			Unknown_6257A0(4);
			return;

		case 2:
		{
			unsigned int count = Unknown268->Unknown00.size();
			bool right = (unsigned int)Unknown260 < count - 9;
			bool left = Unknown260 != 0;
			if (Gadgets[0x2A].Gadget->Unknown_60() != right) {
				Gadgets[0x2A].Gadget->Unknown_40(right);
			}
			if (Gadgets[0x29].Gadget->Unknown_60() != left) {
				Gadgets[0x29].Gadget->Unknown_40(left);
			}
			Unknown_6257A0(2);
			return;
		}

		case 3:
			Unknown_6257A0(3);
			return;
	}
}


// NOT EXACT: functional (D64)
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


// NOT EXACT: functional (D64)
// @0x6280E0
void ShipCustomizeInterface::Unknown_6280E0()
{
	Unknown_6259C0(2);
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	FontSys *none = Unknown_BE67C4->Get_Style("NONE_fa_ship03", model);
	if (none != NULL && none->Unknown300) {
		none->Set_Hidden(1);
	}
	char name[0x14];
	for (int i = 0; i < 10; i++) {
		_snprintf(name, sizeof(name), "D%02d", i);
		RenderObjClass *sub = model->Get_Sub_Object_By_Name(name);
		if (sub == NULL) {
			continue;
		}
		int index = Unknown260 + i;
		if (index == 0) {
			none = Unknown_BE67C4->Get_Style("NONE_fa_ship03", model);
			if (none != NULL && !none->Unknown300) {
				none->Set_Hidden(0);
			}
			Unknown_495FD0(sub, Unknown21C.Peek(), 0);
		} else {
			RefCountPtr<TextureClass> texture = Unknown268->Unknown_61DF40(index - 1);
			Unknown_495FD0(sub, texture.Peek(), 0);
			if (Unknown260 + i == Unknown264) {
				Unknown_6257A0(i + 0x1D);
			}
		}
		sub->Release_Ref();
	}
}


// NOT EXACT: functional (D64)
// @0x628420
void ShipCustomizeInterface::Unknown_628420()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	float slow = time * 0.3f;
	float fast = time + time;
	for (int i = 0; i < 3; i++) {
		RenderObjClass *model = Unknown220[i];
		if (model == NULL) {
			continue;
		}
		Matrix3D tm = model->Get_Transform();
		tm.Rotate_Y((*Unknown_6271D0() == (ShipCustomizationMetal)i) ? fast : slow);
		model->Set_Transform(tm);
		Gadgets[0x12 + i].Gadget->Unknown_6608F0();
	}
}


// NOT EXACT: functional (D64)
// @0x6288C0
bool ShipCustomizeInterface::Unknown_78(void *arg)
{
	if (((int *)arg)[3] == 6 && ((int *)arg)[4] == 0x11) {
		Unknown_Debug_Print("BAD SHIP NAME");
	}
	return(true);
}


// NOT EXACT: functional (D64)
// @0x628900
void ShipCustomizeInterface::Unknown_628900(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event) return;/*@L2531*/
	GadgetClass *gadget = event->Gadget;


	FAIL_IF(!gadget) return;
	if (gadget->Flash && gadget->Unknown2C_02) {
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
