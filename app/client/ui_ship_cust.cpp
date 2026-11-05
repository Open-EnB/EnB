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
#include "ui_ship_cust.h"
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
#include "dx8renderer.h"
#include "unknown_signalbase.h"
#include "..\shared\money.h"
#include "..\packets\pkstarbasetow.h"
#include "local_interface_sort.h"

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
	~UIAvatrPullClass();								// 0x585950

	GadgetClass *Gadget;								// +0x00 (ui_avatr.cpp's UIAvatrGadget671300Class)  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('PULL_COLOR')  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08 (dragging)
	float Unknown0C;									// +0x0C (0.5)
};
// The client at InterfaceClass +0x40, as ui_avatar_cust.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};
extern bool Unknown_BB9654;							// NAME 1 [Unknown_BB9654]: neutral (D10) (tsk_login.cpp's); the constructor clears it, the destructor sets it

// The pages' font styles and string files (0xB894A0)
static const char *Unknown_B894A0[][2] = { { "BUY_SHIP01", "fa_buy_ship01.ini" }, { "BUY_SHIP02", "fa_buy_ship02.ini" }, { "BUY_SHIP03", "fa_buy_ship03.ini" }, { "BUY_SHIP04", "fa_buy_ship04.ini" } };	// NAME 1: neutral (D10)

typedef SignalBaseClass::MemberCallback<GadgetClass, StarbaseShipCustomizeInterface, void (StarbaseShipCustomizeInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> StarbaseShipCustCallbackClass;	// NAME 1 [StarbaseShipCustCallbackClass]: neutral (D10)
class StarbaseShipCustNotifierClass : public SignalMap		// NAME 1: neutral (D10): as ui_shipcust.cpp's InterfaceSignalMapClass
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback) { connect(sender, callback); }			// 0x4F9740 (the client's 'SignalMap::connect')
};

// @0x6186A0
StarbaseShipCustomizeInterface::~StarbaseShipCustomizeInterface()
{
	Unknown_BB9654 = true;
	SBaseView->Unknown_69DC60();
	Unknown_5C();
	Unknown2E0->Unknown_80();
	Unknown2E0->Unknown_80C180();
	Unknown64.Unknown_687150(Unknown2E0);
	if (NameGadget) {
		Unknown64.Unknown_687150(NameGadget);
	}
	NameGadget = NULL;
	Unknown_61AF50();
	if (Unknown2FC) {
		Unknown_BE67C4->Remove(Mesh->Peek_Object());
		Unknown_BE67BC->Unload(Unknown_B894A0[Page][1]);
		Unknown2FC = false;
	}
	ColorSliderTexture = NULL;
	if (Unknown300) {
		delete Unknown300;
	}
	Unknown300 = NULL;
	Unknown240 = NULL;
	if (Unknown330) {
		delete Unknown330;
	}
	Unknown330 = NULL;
	GadgetClass::Get_Signal_Map().disconnect((GadgetClass *)NULL, this, &StarbaseShipCustomizeInterface::Unknown_61E730, FlashingGadgetEvent(NULL));
}

// NOT EXACT: inline budget (0.48: VC6 here inlines ShipCustPartClass's constructor, the vectors' constructors and the
// callback's, which the client calls out of line: 0x6191F0, 0x61FAB0, 0x61FB60, 0x620BC0; the frame is 0x1C bytes larger)
// @0x618CA0
void StarbaseShipCustomizeInterface::Unknown_54()
{
	SBaseView->Unknown_69DC40();
	InterfaceSortScopeClass sort(this);
	Desc = ShipCustDescClass();
	Desc.Set_Race(SBaseView->Client->Unknown_74C960(SBaseView->Client->AvatarID)[4]);
	Desc.Reset_Parts();
	Desc.Set_Profession(SBaseView->Client->Unknown_74C960(SBaseView->Client->AvatarID)[5]);
	Unknown318 = (Desc.Selected_Decal == 0x7FFFFFF) ? 0 : Desc.Selected_Decal + 1;
	Unknown2E0 = new ShipPipClass(0);
	Unknown2E0->Unknown_80C220((ViewClass *)SBaseView);
	Unknown2E0->Unknown_74(0);
	Unknown2E0->Set_Last_Sort_Group(1);
	Vector3 position(0.0f, 0.0f, 0.0f);
	Vector2 size(640.0f, 480.0f);
	Unknown2E0->Unknown_80C540(size, position, 0x1C);
	Unknown64.Unknown_6870E0(Unknown2E0, 0x33, View, 1, 0);
	Unknown330 = new SClientUnknown1334Class;
	Unknown330->Load("decal_textures.ini", "decal_swatches.ini", "secret_decal_textures.ini");
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIShipCustKeyEventStruct(0x1B), (void *)0x1B);
	Unknown_10();
	Unknown_7C((UIViewClass *)SBaseView, true);
	Unknown_61BD80(Page);
	Unknown_61E8E0(1);
	TheDX8MeshRenderer.Invalidate();
	void (StarbaseShipCustomizeInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &StarbaseShipCustomizeInterface::Unknown_61E730;
	StarbaseShipCustNotifierClass *notifier = (StarbaseShipCustNotifierClass *)&GadgetClass::Get_Signal_Map();
	notifier->Add(0, new StarbaseShipCustCallbackClass(this, function));
	SClient *client = Client->As_Space_Client();
	client->Send_Packet(&StarbaseRequestPacketClass(client->Unknown1138, 11, 0));
}

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
	std::map<int, StarbaseShipCustGadgetStruct>::iterator it = Gadgets.begin();
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
	StarbaseShipCustGadgetStruct entry;
	entry.Gadget = button;
	entry.Name = name;
	entry.Style = style;
	entry.Id = id;
	entry.Key = key;
	entry.Group = group;
	entry.Selected = false;
	Gadgets.insert(std::map<int, StarbaseShipCustGadgetStruct>::value_type(id, entry));
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
	std::map<int, StarbaseShipCustGadgetStruct>::iterator it = Gadgets.begin();
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
	std::map<int, StarbaseShipCustGadgetStruct>::iterator it = Gadgets.begin();
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

// Notes (CS21, 2026-11-05; SC1, 2026-11-05). The file is the client's ui_ship_cust.cpp (__FILE__ 0xB896B8, asserts
// at lines 712, 1108, 2017 and 2020: the functions holding them are not in yet, so nothing here sits on their lines).
// Its window: the constructor 0x6181B0 ('BUY_SHIP01', called from sbaseview.cpp's 0x69F8F0; not in yet), the
// settings' constructor 0x618500 (its ??_H target), the deleting destructor 0x618540, the map's destructor 0x618570,
// the destructor 0x6186A0, then slot +0x54 0x618CA0 (InterfaceClass's after-construction call, not a constructor) and
// the class's functions to 0x61E8E0, the map's template copies 0x61F760-0x620B30 and the static initialiser 0x620CA0.
// The members follow the constructor 0x6181B0 (its stores) and the destructor's EH states (+0x240, the map, +0x304,
// the two MoneyClass at +0x320 / +0x328: 0x4A7780 in the unwind funclets).
// decal.h's inlines whose client copies sit here are emitted here: 0x618A50 / 0x618C10 (the destructor's delete of
// the decal library), 0x619280 (slot +0x54's new) and 0x61DF40 (0x61DBF0); ui_shipcust.cpp emits the same COMDATs,
// so verify shares them (D137's interim placement there is settled).
// The gadget map's element is this file's own StarbaseShipCustGadgetStruct, not ui_shipcust.h's ShipCustGadgetStruct:
// the client's two maps are separate instantiations (this file's _Nil / _Nilrefs 0xBE5E28 / 0xBE5E24 in 0x618570 and
// 0x6209D0, ui_shipcust.cpp's 0xBE5E44 in 0x629B70).
