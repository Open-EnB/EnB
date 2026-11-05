// Earth & Beyond client (reconstruction): app\Client\ui_ship_cust.cpp. Notes at the end of the file.
#include <vector>
#include <map>
#include <string>
#include "sclient.h"
#include "sbaseview.h"
#include "client.h"
#include "rendobj.h"
#include "texture.h"
#include "vertmaterial.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\shipcust_desc.h"
#include "ui_shipcust.h"
#include "decal.h"
#include "gadget.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "ship_pip.h"
#include "unknown_675120.h"
#include "textgadg.h"
#include "assetmgr.h"
#include "vector2.h"
#include "aabox.h"
#include "matrix3d.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);	// 0x495FD0 (ro_funcs.cpp): the texture onto the model
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp)

// The control keys (0x10 bytes) and InterfaceClass +0x4C's list, as ui_shipcust.cpp declares them
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

// The colour palette and its pull (0x10 bytes, constructor 0x585920), as far as this file reads it (ui_shipcust.cpp's
// UIAvatrPullClass, as unknown_585920.cpp declares it)
class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	GadgetClass *Gadget;								// +0x00 (ui_avatr.cpp's UIAvatrGadget671300Class)  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('PULL_COLOR')  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08 (dragging)
	float Unknown0C;									// +0x0C (0.5)
};

// The starbase's ship customisation window (the buy-ship pages 'fa_buy_ship01_ini' ..), as far as this file's
// functions so far use it. Its first base is the starbase windows' interface base (0xAC bytes, as
// ui_avatar_cust.cpp's StarbaseAvatarCreationInterface), its second the ship being edited (ui_shipcust.h's
// Unknown620D20Class, at +0xAC: these functions call its 0x6211D0 / 0x621390 with ecx = this + 0xAC).
class StarbaseShipCustomizeInterface : public Unknown5BB320Class, public Unknown620D20Class	// NAME 3: the client's RTTI (0xB89BB0, among this file's strings: SignalBaseClass::MemberCallback<GadgetClass,StarbaseShipCustomizeInterface,...>, the FlashingGadgetEvent handler's)
{
public:
	virtual void Unknown_60();							// 0x6193A0 (every frame, then Unknown5BB320Class's 0x5BB660)
	void Unknown_61A4D0();								// 0x61A4D0 (G_ASSERT l.712)
	void Unknown_61AC20(bool on);						// 0x61AC20 (the gadgets on or off, their texts dimmed: as ui_shipcust.cpp's 0x6249D0)  // NAME 1 [on]: neutral
	void Unknown_61AD30(const char *style, bool on);	// 0x61AD30 (a style shown or hidden: as 0x624AE0)  // NAME 1 [style, on]: neutral
	void Unknown_61ADA0(int id, const char *name, const char *style, short key, short group);	// 0x61ADA0 (a button of the model's sub-object into the map: as 0x624B50)  // NAME 1 [all]: as ui_shipcust.h's
	void Unknown_61AF50();								// 0x61AF50 (the map emptied: as 0x624D00)
	void Unknown_61B0F0();								// 0x61B0F0 (the parts' colours onto the model, then 0x61F180)
	void Unknown_61B130();								// 0x61B130 (the insignia onto the model, then 0x61F180)
	void Unknown_61B510(Vector2 uv);					// 0x61B510 (the colour's palette position: as 0x6251F0)  // NAME 1 [uv]: neutral
	void Unknown_61B580();								// 0x61B580 (the pull to the colour's palette position: as ui_shipcust.cpp's 0x625260)
	void Unknown_61BA80(bool on);						// 0x61BA80 (the pull shown: as 0x625760)  // NAME 1 [on]: neutral
	void Unknown_61B170();								// 0x61B170
	void Unknown_61BAC0(int id);						// 0x61BAC0 (a button selected in its group: as ui_shipcust.cpp's 0x6257A0)  // NAME 1 [id]: neutral
	int Unknown_61BC50(short group);					// 0x61BC50 (the selected id of a group: as 0x625930)  // NAME 1 [group]: neutral
	void Unknown_61BCE0(short group);					// 0x61BCE0 (a group's buttons unselected: as 0x6259C0)  // NAME 1 [group]: neutral
	float *Unknown_61D180();							// 0x61D180 (the colour's palette position: as ui_shipcust.cpp's 0x627110)
	Vector3 *Unknown_61D1C0();							// 0x61D1C0 (the colour: as 0x627160)
	bool *Unknown_61D200();								// 0x61D200 (the setting's matte flag: as 0x6271A0)
	ShipCustomizationMetal *Unknown_61D230();			// 0x61D230 (the setting's metal: as 0x6271D0)
	void Unknown_61DAB0();								// 0x61DAB0 (the insignia buttons: as ui_shipcust.cpp's 0x627FA0)
	void Unknown_61DBF0();								// 0x61DBF0 (the insignia list's textures: as 0x6280E0)
	void Unknown_61DF80();								// 0x61DF80 (the metal models: 'METAL00' ..)
	void Unknown_61E8B0();
	void Unknown_61E8E0(int arg);						// 0x61E8E0  // NAME 1 [arg]: neutral
	void Unknown_61F0E0();								// 0x61F0E0								// 0x61E8B0 (the client's 0x74C960 for its avatar)
	void Unknown_61F180();								// 0x61F180 (after the model changes)

	SBaseViewClass *SBaseView;								// +0x230 (its Client's EnvironmentMapManager; not InterfaceClass's View, +0x48)  // NAME 1: neutral (D10)
	char Unknown234[0x240 - 0x234];						// +0x234
	RefCountPtr<TextureClass> Unknown240;				// +0x240 (no insignia: 'BLACK.TGA', as ui_shipcust.cpp's +0x21C)
	char Unknown244[0x2E0 - 0x244];						// +0x244
	ShipPipClass *Unknown2E0;							// +0x2E0 the ship preview gadget (as ui_shipcust.cpp's +0x22C)
	LayerClass *Layer;									// +0x2E4 (the buttons are made over it)  // NAME 1: as ui_shipcust.h's
	int Page;											// +0x2E8 (1: the paint page, as ui_shipcust.cpp's +0x234)  // NAME 1: as ui_shipcust.h's
	std::map<int, ShipCustGadgetStruct> Gadgets;		// +0x2EC (as ui_shipcust.cpp's +0x238)  // NAME 1: as ui_shipcust.h's
	char Unknown2FC[0x300 - 0x2FC];						// +0x2FC
	UIAvatrPullClass *Unknown300;						// +0x300 the colour palette and its pull (as ui_shipcust.cpp's +0x24C)
	char Unknown304[0x308 - 0x304];						// +0x304
	TextGadgetClass *NameGadget;						// +0x308 (the ship's name: 'NAME_buy_ship04')  // NAME 1: as ui_shipcust.h's
	int Unknown30C;										// +0x30C the part
	int Unknown310;										// +0x310 the part's setting
	int Unknown314;										// +0x314 the insignia list's first
	int Unknown318;										// +0x318 the insignia
	char Unknown31C[0x330 - 0x31C];						// +0x31C
	SClientUnknown1334Class *Unknown330;				// +0x330 the decal library
};

// @0x6193A0
void StarbaseShipCustomizeInterface::Unknown_60()
{
	if (NameGadget) {
		bool changed = NameGadget->Changed;
		NameGadget->Changed = false;
		if (changed) {
			_snprintf(Desc.Ship_Name, 0x1A, "%s", NameGadget->Get_Text());
			Unknown_621500();
			NameGadget->Changed = true;
			Unknown_61F180();
		}
		NameGadget->Unknown_6AAD10();
	}
	Unknown_61A4D0();
	Unknown_61F0E0();
	Unknown2E0->Unknown_78();
	Unknown_61DF80();
	Unknown_61B170();
	Unknown_61E8E0(0);
	Unknown5BB320Class::Unknown_60();
}

// @0x61AC20
void StarbaseShipCustomizeInterface::Unknown_61AC20(bool on)
{
	std::map<int, ShipCustGadgetStruct>::iterator it = Gadgets.begin();
	while (it != Gadgets.end()) {
		it->second.Gadget->Unknown_40(on);
		if (it->second.Style != NULL) {
			FontSys *font = Unknown_BE67C4->Get_Style(it->second.Style, Mesh->Peek_Object()); font->Unknown_Set_2FC(on ? 1.0f : 0.33f);
		}
		it++;
	}
	if (Unknown300 != NULL) {
		Unknown300->Gadget->Unknown_40(on);
	}
	if (Unknown2E0 != NULL) {
		Unknown2E0->Unknown_40(on);
	}
}

// @0x61AD30
void StarbaseShipCustomizeInterface::Unknown_61AD30(const char *style, bool on)
{
	RenderObjClass *model = Mesh->Peek_Object(); FontSys *font = Unknown_BE67C4->Get_Style(style, model);
	if (font != NULL) { int hidden = !on; bool now = !font->Unknown300; if (now != hidden) {
		font->Set_Hidden(hidden); }
	}
}

// @0x61ADA0
void StarbaseShipCustomizeInterface::Unknown_61ADA0(int id, const char *name, const char *style, short key, short group)
{
	RenderObjClass *sub = Mesh->Peek_Object()->Get_Sub_Object_By_Name(name);
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
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIShipCustKeyEventStruct(key), (void *)id);
	}
	sub->Release_Ref();
}

// @0x61AF50
void StarbaseShipCustomizeInterface::Unknown_61AF50()
{
	while (Gadgets.begin() != Gadgets.end()) {
		Unknown64.Unknown_687150(Gadgets.begin()->second.Gadget);
		Gadgets.erase(Gadgets.begin());
	}
}

// @0x61B0F0
void StarbaseShipCustomizeInterface::Unknown_61B0F0()
{
	Unknown_6211D0(SBaseView->Client->Peek_Environment_Map_Manager());
	Unknown_61F180();
}

// @0x61B130
void StarbaseShipCustomizeInterface::Unknown_61B130()
{
	Unknown_621390(Unknown318, Unknown330);
	Unknown_61F180();
}

// @0x61B510
void StarbaseShipCustomizeInterface::Unknown_61B510(Vector2 uv)
{
	float *position = Unknown_61D180();
	position[0] = uv.X;
	position[1] = uv.Y;
}

// @0x61B580
void StarbaseShipCustomizeInterface::Unknown_61B580()
{
	AABoxClass box;
	Unknown300->Gadget->Unknown_64(box);
	float y = box.Center.Y - box.Extent.Y; float x = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f; float height = box.Extent.Y * 2.0f;
	RenderObjClass *model = Mesh->Peek_Object();
	int bone = model->Get_Sub_Object_Bone_Index(Unknown300->Pull);
	model->Capture_Bone(bone);
	Matrix3D tm = Unknown300->Pull->Get_Transform();
	tm[0][3] = width * Unknown_61D180()[0] + x - box.Center.X; tm[1][3] = height * Unknown_61D180()[1] + y - box.Center.Y;
	model->Control_Bone(bone, tm, false);
}

// @0x61BA80
void StarbaseShipCustomizeInterface::Unknown_61BA80(bool on)
{
	Unknown300->Pull->Set_Hidden(!on);
}

// @0x61BAC0
void StarbaseShipCustomizeInterface::Unknown_61BAC0(int id)
{

	Unknown_61BCE0(Gadgets[id].Group);




	Gadgets[id].Gadget->Unknown2C_0B = 1;
	Gadgets[id].Gadget->Unknown_6603A0(false, NULL);
	Gadgets[id].Selected = true;
}

// @0x61BC50
int StarbaseShipCustomizeInterface::Unknown_61BC50(short group)
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

// @0x61BCE0
void StarbaseShipCustomizeInterface::Unknown_61BCE0(short group)
{
	bool found = false;	// set, never read (D130): as ui_shipcust.cpp's 0x6259C0, its weight keeps 0x61BAC0, which inlines this loop, calling _Inc out of line as the client does
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

// @0x61D180
float *StarbaseShipCustomizeInterface::Unknown_61D180()
{
	if (Page == 1) {
		return(&Desc.Parts[Unknown30C].Settings[Unknown310].Unknown10);
	}
	return(&Desc.Unknown3C.X);
}

// @0x61D1C0
Vector3 *StarbaseShipCustomizeInterface::Unknown_61D1C0()
{
	if (Page == 1) {
		return(&Desc.Parts[Unknown30C].Settings[Unknown310].Color);
	}
	return(&Desc.Ship_Name_Color);
}

// @0x61D200
bool *StarbaseShipCustomizeInterface::Unknown_61D200()
{
	return(&Desc.Parts[Unknown30C].Settings[Unknown310].Is_Matte);
}

// @0x61D230
ShipCustomizationMetal *StarbaseShipCustomizeInterface::Unknown_61D230()
{
	return(&Desc.Parts[Unknown30C].Settings[Unknown310].Metal);
}

// @0x61DAB0
void StarbaseShipCustomizeInterface::Unknown_61DAB0()
{
	char name[0x14];
	Unknown_61ADA0(0x1C, "D00", "NONE_buy_ship03", 0, 2);
	for (int i = 1; i < 9; i++) {
		_snprintf(name, sizeof(name), "D%02d", i);
		Unknown_61ADA0(i + 0x1C, name, NULL, 0, 2);
	}
	Unknown240 = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("BLACK.TGA", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
}

// @0x61DBF0
void StarbaseShipCustomizeInterface::Unknown_61DBF0()
{
	Unknown_61BCE0(2);

	FontSys *none = Unknown_BE67C4->Get_Style("NONE_buy_ship03", Mesh->Peek_Object());
	if (none != NULL && none->Unknown300) {
		none->Set_Hidden(1);
	}
	char name[0x14];
	for (int i = 0; i < 9; i++) {
		_snprintf(name, sizeof(name), "D%02d", i);
		RenderObjClass *sub = Mesh->Peek_Object()->Get_Sub_Object_By_Name(name);
		if (sub == NULL) {
			continue;
		}
		if (Unknown314 + i == 0) {
			none = Unknown_BE67C4->Get_Style("NONE_buy_ship03", Mesh->Peek_Object());
			if (none != NULL && !none->Unknown300) {
				none->Set_Hidden(0);
			}
			Unknown_495FD0(sub, Unknown240.Peek(), 0);
		} else {
			RefCountPtr<TextureClass> texture = Unknown330->Unknown_61DF40(Unknown314 + i - 1);
			Unknown_495FD0(sub, texture.Peek(), 0);
			if (Unknown314 + i == Unknown318) {
				Unknown_61BAC0(i + 0x1C);
			}
		}
		sub->Release_Ref();
	} Gadgets.size(); Gadgets.size();	// dead reads (D130; not the client's): as ui_shipcust.cpp's 0x6280E0, they spend VC6's inline budget, so the inlined 0x61BAC0 keeps its map lookups as the client's (without them 0.47)
}

// NOT EXACT: register (0.62: the client loads the avatar id into eax, VC6 here into edx; the rest is the same)
// @0x61E8B0
void StarbaseShipCustomizeInterface::Unknown_61E8B0()
{
	SClient *client = SBaseView->Client;
	client->Unknown_74C960(client->AvatarID);
}

// Notes (CS21, 2026-11-05). The file is the client's ui_ship_cust.cpp (__FILE__ 0xB896B8, asserts at lines 712,
// 1108, 2017 and 2020: the functions holding them are not in yet, so nothing here sits on their lines). Its
// window: the class's functions from about 0x6186A0 (the destructor, called by the deleting one 0x618540) and the
// constructor 0x618CA0 to 0x61E8E0; 0x6181B0 ('BUY_SHIP01', called from sbaseview.cpp) may be its first.
// The class's members past the bases are placed only as far as these functions read them. decal.h's inlines whose
// client copies sit here: 0x61DF40 is emitted here now (0x61DBF0 calls it; ui_shipcust.cpp's 0x6280E0 emits it
// too, so verify shares it); 0x618A50, 0x618C10 and 0x619280 still come only from ui_shipcust.cpp (interim, D137)
// until their callers here are in (the destructor 0x6186A0, the constructor 0x618CA0). The map's template copies
// 0x61F760-0x620B30 (ShipCustGadgetStruct's std::map) are this object's too.
