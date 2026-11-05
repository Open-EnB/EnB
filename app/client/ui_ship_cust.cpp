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
#include "ww3d.h"
#include "aabox.h"
#include "matrix3d.h"
#include "dx8renderer.h"
#include "unknown_signalbase.h"
#include "..\shared\money.h"
#include "..\packets\pkstarbasetow.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_template.h"

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

// The palette's drag gadget (0x80 bytes, constructor 0x671300), as ui_shipcust.cpp declares it
class UIMeshClass;
class UIAvatrUnknown4A1240Class;
class KernelEventClass;
class UIAvatrGadget671300Class : public ButtonGadgetClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d);	// 0x671300 (mesh: the window's layer)  // NAME 1 [area, a, mesh, b, c, d]: as ui_avatr.cpp's
	virtual ~UIAvatrGadget671300Class();				// +0x00 0x6714F0 (deleting 0x6713E0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x671570 (the drag)  // NAME 1 [event, x, y, mask]: gadget.h's
	virtual void Unknown_A4();							// +0xA4 0x671650
	virtual void Unknown_A8();							// +0xA8 0x671670
	virtual void Unknown_AC();							// +0xAC 0x6716B0
	void Unknown_671630();								// 0x671630 (the frame's end: the drag's flags cleared; unknown_585920.cpp)

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
// The colour palette and its pull (0x10 bytes, constructor 0x585920), as far as this file reads it (ui_shipcust.cpp's
// UIAvatrPullClass, as unknown_585920.cpp declares it)
class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950
	void Unknown_5859A0();								// 0x5859A0 (the pull model onto the gadget's box centre)

	GadgetClass *Gadget;								// +0x00 (ui_avatr.cpp's UIAvatrGadget671300Class)  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('PULL_COLOR')  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08 (dragging)
	float Unknown0C;									// +0x0C (0.5)
};
// The hull and wing styles' texts (0xB894C0, 0xB894CC)
static const char *Unknown_B894C0[3] = { "HULL00_buy_ship01", "HULL01_buy_ship01", "HULL02_buy_ship01" };	// NAME 1: neutral (D10)
static const char *Unknown_B894CC[3] = { "WING00_buy_ship01", "WING01_buy_ship01", "WING02_buy_ship01" };	// NAME 1: neutral (D10)
// The client at InterfaceClass +0x40, as ui_avatar_cust.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};
template<class C> inline C *UIShipCust_Find_Valid(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as ui_avatar_cust.cpp's
template<class C> inline C *UIShipCust_Find_Changed(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10): as ui_avatar_cust.cpp's
extern bool Unknown_BB9654;							// NAME 1 [Unknown_BB9654]: neutral (D10) (tsk_login.cpp's); the constructor clears it, the destructor sets it

// The pages' font styles and string files (0xB894A0)
static const char *Unknown_B894A0[][2] = { { "BUY_SHIP01", "fa_buy_ship01.ini" }, { "BUY_SHIP02", "fa_buy_ship02.ini" }, { "BUY_SHIP03", "fa_buy_ship03.ini" }, { "BUY_SHIP04", "fa_buy_ship04.ini" } };	// NAME 1: neutral (D10)

typedef SignalBaseClass::MemberCallback<GadgetClass, StarbaseShipCustomizeInterface, void (StarbaseShipCustomizeInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> StarbaseShipCustCallbackClass;	// NAME 1 [StarbaseShipCustCallbackClass]: neutral (D10)
class StarbaseShipCustNotifierClass : public SignalMap		// NAME 1: neutral (D10): as ui_shipcust.cpp's InterfaceSignalMapClass
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback) { connect(sender, callback); }			// 0x4F9740 (the client's 'SignalMap::connect')
};

// @0x6181B0
StarbaseShipCustomizeInterface::StarbaseShipCustomizeInterface(UIViewClass *view, LayerClass *layer) :
	Unknown5BB320Class(new UIMeshClass("BUY_SHIP01", 1), view, *layer),
	SBaseView((SBaseViewClass *)view->Unknown_30()),
	Unknown234(false),
	Unknown235(false),
	Unknown236(false),
	Unknown237(false),
	Unknown238(false),
	Unknown239(false),
	Unknown23C(0),
	Unknown2D4(NULL),
	Unknown2D8(NULL),
	Unknown2DC(NULL),
	Unknown2E0(NULL),
	Layer(layer),
	Page(0),
	Unknown2FC(false),
	Unknown300(NULL),
	NameGadget(NULL),
	Unknown30C(0),
	Unknown310(0),
	Unknown314(0),
	Unknown318(0),
	Unknown330(NULL),
	Unknown334(false)
{
	for (int i = 0; i < 3; i++) {
		Unknown244[i].Make_Identity();
	}
	Unknown_BB9654 = false;
}

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

// NOT EXACT: inline budget (0.54: VC6 here inlines the first vector's constructor and the callback's, which the client
// calls out of line: 0x61FAB0, 0x620BC0; the frame is 4 bytes larger; the part's constructor 0x6191F0 goes out of line)
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
	Unknown2E0->Unknown_80C540(Vector2(640.0f, 480.0f), Vector3(0.0f, 0.0f, 0.0f), 0x1C);
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
	client->Send_Packet(&StarbaseRequestPacketClass(client->Unknown1138, 11, 0)); Gadgets.size(); Gadgets.size();	// dead reads (D130; not the client's): they spend VC6's inline budget, so ShipCustPartClass's constructor 0x6191F0 is called out of line as the client's
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









































































































































































































































































































































































































// NOT EXACT: functional (D64)
// @0x61A4D0
void StarbaseShipCustomizeInterface::Unknown_61A4D0()
{
	GadgetClass *gadget;
	gadget = Gadgets[0x0E].Gadget;
	if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
		Unknown2E0->Unknown0B8 = 1.0f;
	} else {
		gadget = Gadgets[0x0D].Gadget;
		if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
			Unknown2E0->Unknown0B8 = -1.0f;
		} else {
			gadget = Gadgets[0x0B].Gadget;
			if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
				Unknown2E0->Unknown0BC = 1.0f;
			} else {
				gadget = Gadgets[0x0C].Gadget;
				if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
					Unknown2E0->Unknown0BC = -1.0f;
				} else {
					Unknown2E0->Unknown0B8 = 0.0f;
					Unknown2E0->Unknown0BC = 0.0f;
				}
			}
		}
	}
	gadget = Gadgets[0x2B].Gadget;
	if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
		Unknown2E0->Unknown0C0 = -1.0f;
	} else {
		gadget = Gadgets[0x2A].Gadget;
		if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
			Unknown2E0->Unknown0C0 = 1.0f;
		} else {
			gadget = Gadgets[0x2C].Gadget;
			if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
				Unknown2E0->Unknown0C4 = -1.0f;
			} else {
				gadget = Gadgets[0x2D].Gadget;
				if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
					Unknown2E0->Unknown0C4 = 1.0f;
				} else {
					Unknown2E0->Unknown0C0 = 0.0f;
					Unknown2E0->Unknown0C4 = 0.0f;
				}
			}
		}
	}
	gadget = Gadgets[0x2E].Gadget;
	if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
		Unknown2E0->Unknown0C8 = -1.0f;
	} else {
		gadget = Gadgets[0x2F].Gadget;
		if (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget) {
			Unknown2E0->Unknown0C8 = 1.0f;
		} else {
			Unknown2E0->Unknown0C8 = 0.0f;
		}
	}
	if (Unknown237) {
		Unknown237 = false;
		int id = 0;
		for (std::map<int, StarbaseShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
			if (it->second.Group == 0 && it->second.Selected) {
				id = it->second.Id;
				break;
			}
		}
		switch (id) {
			case 1:
				Unknown_61BD80(0);
				return;
			case 2:
				Unknown_61BD80(2);
				return;
			case 3:
				Unknown_61BD80(3);
				return;
			case 4:
				Unknown_61BD80(1);
				return;
			default:
				G_ASSERT(0);
				break;
		}
	}
	if (Unknown239) {
		if (Unknown23C == 5) {
			if (Unknown318 >= 1) {
				Desc.Selected_Decal = Unknown318 - 1;
			} else if (Unknown318 < 0) {
				Desc.Selected_Decal = Unknown318;
			} else {
				Desc.Selected_Decal = 0x7FFFFFF;
			}
			Unknown_61F690();
		}
		SBaseView->Unknown_69F440();
	}
	if (SBaseView->Get_Unknown34()->Is_Active()) {
		if (!Unknown235) {
			Unknown235 = true;
			Unknown_61AC20(false);
		}
		return;
	}
	if (Unknown235) {
		Unknown235 = false;
		Unknown_61AC20(true);
		Unknown_61D260();
	}
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

// @0x61AFC0
void StarbaseShipCustomizeInterface::Unknown_61AFC0()
{
	if (Unknown334) {
		SClient *client = SBaseView->Client;
		Unknown_621790(client->Peek_Environment_Map_Manager(), Unknown318, Unknown330);
		if (Unknown2E0->Unknown054.Peek() == NULL) {
			InterfaceSortScopeClass sort(this);
			Unknown2E0->Unknown_7C(Unknown160);
		} else {
			Unknown2E0->Unknown_84(Unknown160);
		}
		Unknown_61F180();
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



















































































































































































































































// @0x61B760
void StarbaseShipCustomizeInterface::Unknown_61B760()
{
	Unknown300 = new UIAvatrPullClass;
	RenderObjClass *palette = Mesh->Peek_Object()->Get_Sub_Object_By_Name("COLOR_PALETTE");
	Unknown_496AE0(palette, 0);
	if (Page == 1) {
		ColorSliderTexture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("fa_ship_color.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	} else {
		ColorSliderTexture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("fa_name_color.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	}
	G_ASSERT(ColorSliderTexture);
	Unknown_495FD0(palette, ColorSliderTexture.Peek(), 0);
	Unknown300->Gadget = new UIAvatrGadget671300Class(palette, 0, (UIMeshClass *)Layer, 0, 0, 0);
	Unknown300->Gadget->Unknown2C_0A = 1;
	Unknown300->Unknown_5859A0();
	palette->Release_Ref();
	Unknown300->Pull = Mesh->Peek_Object()->Get_Sub_Object_By_Name("PULL_COLOR");
	Unknown_496AE0(Unknown300->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown300->Gadget, 0x32, View, 1, 1);
	Unknown_61B580();
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
#pragma auto_inline(off)	// 0x61D260 calls 0x61D230 out of line where it does not read the metal itself
// @0x61D230
ShipCustomizationMetal *StarbaseShipCustomizeInterface::Unknown_61D230()
{
	return(&Desc.Parts[Unknown30C].Settings[Unknown310].Metal);
}
#pragma auto_inline(on)
// @0x61D260
void StarbaseShipCustomizeInterface::Unknown_61D260()
{
	int i;
	switch (Page) {
		case 2:
		{
			unsigned int count = Unknown330->TextureArray.size();
			bool right = (unsigned int)Unknown314 < count - 8;
			bool left = (unsigned int)Unknown314 > 0;
			if (Gadgets[0x29].Gadget->Unknown_60() != right) {
				Gadgets[0x29].Gadget->Unknown_40(right);
			}
			if (Gadgets[0x28].Gadget->Unknown_60() != left) {
				Gadgets[0x28].Gadget->Unknown_40(left);
			}
			Unknown_61BAC0(2);
			return;
		}

		case 0:
			for (i = 0; i < 3; i++) {
				Unknown_61AD30(Unknown_B894C0[i], i == Desc.Hull_Style);
			}
			for (i = 0; i < 3; i++) {
				Unknown_61AD30(Unknown_B894CC[i], i == Desc.Wing_Style);
			}
			Unknown_61BAC0(1);
			return;

		case 1:
			if (Unknown310 == 0) {
				Unknown_61BAC0(0x14);
			} else {
				Unknown_61BAC0(0x15);
			}
			if (*Unknown_61D200()) {
				Unknown_61BAC0(0x0F);
			} else {
				Unknown_61BAC0(0x10);
			}
			switch (Unknown30C) {
				case 0:
					Unknown_61BAC0(0x16);
					break;
				case 2:
					Unknown_61BAC0(0x17);
					break;
				case 1:
					Unknown_61BAC0(0x18);
					break;
				case 3:
					Unknown_61BAC0(0x19);
					break;
			}
			Unknown_61BA80(Desc.Parts[Unknown30C].Settings[Unknown310].Metal == METAL_NONE);
			if (*Unknown_61D230() != METAL_NONE) {
				Gadgets[0x10].Gadget->Unknown_40(false);
				Gadgets[0x0F].Gadget->Unknown_40(false);
			} else {
				Gadgets[0x10].Gadget->Unknown_40(true);
				Gadgets[0x0F].Gadget->Unknown_40(true);
			}
			Unknown_61BAC0(4);
			return;

		case 3:
			Unknown_61BAC0(3);
			return;
	}
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





























































































































































































































































































































































































































































































































































































































































// NOT EXACT: inline budget (0.47, 1705 bytes against 0x611: as ui_avatar_cust.cpp's 0x550A10; the client copies only
// the first model's translation, builds the second's Matrix3D through ??_H, and calls the third Rotate_Y out of line)
// @0x61DF80
void StarbaseShipCustomizeInterface::Unknown_61DF80()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	float slow = time * 0.3f;
	if (Unknown2D4 != NULL) {
		Matrix3D tm = Unknown2D4->Get_Transform();
		if (Desc.Parts[Unknown30C].Settings[Unknown310].Metal == METAL_CHROME) {
			Unknown244[0].Rotate_Y(time);
		} else {
			Unknown244[0].Rotate_Y(slow);
		}
		int bone = Mesh->Peek_Object()->Get_Bone_Index("METAL00");
		Mesh->Peek_Object()->Capture_Bone(bone);
		Mesh->Peek_Object()->Control_Bone(bone, Unknown244[0]);
		Matrix3D newtm(Unknown244[0]);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		Unknown2D4->Set_Transform(newtm);
		Gadgets[0x11].Gadget->Unknown_6608F0();
	}
	if (Unknown2D8 != NULL) {
		Matrix3D tm = Unknown2D8->Get_Transform();
		if (Desc.Parts[Unknown30C].Settings[Unknown310].Metal == METAL_BRONZE) {
			Unknown244[1].Rotate_Y(time);
		} else {
			Unknown244[1].Rotate_Y(slow);
		}
		int bone = Mesh->Peek_Object()->Get_Bone_Index("METAL01");
		Mesh->Peek_Object()->Capture_Bone(bone);
		Mesh->Peek_Object()->Control_Bone(bone, Unknown244[1]);
		Matrix3D newtm(Unknown244[1]);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		Unknown2D8->Set_Transform(newtm);
		Gadgets[0x12].Gadget->Unknown_6608F0();
	}
	if (Unknown2DC != NULL) {
		Matrix3D tm = Unknown2DC->Get_Transform();
		if (Desc.Parts[Unknown30C].Settings[Unknown310].Metal == METAL_COPPER) {
			Unknown244[2].Rotate_Y(time);
		} else {
			Unknown244[2].Rotate_Y(slow);
		}
		int bone = Mesh->Peek_Object()->Get_Bone_Index("METAL02");
		Mesh->Peek_Object()->Capture_Bone(bone);
		Mesh->Peek_Object()->Control_Bone(bone, Unknown244[2]);
		Matrix3D newtm(Unknown244[2]);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		Unknown2DC->Set_Transform(newtm);
		Gadgets[0x13].Gadget->Unknown_6608F0();
	}
}

// @0x61E730
void StarbaseShipCustomizeInterface::Unknown_61E730(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event) return;
	GadgetClass *gadget = event->Gadget;

	FAIL_IF(!gadget) return;
	if (gadget->Flash && gadget->Is_Unknown2C_02()) {
		for (std::map<int, StarbaseShipCustGadgetStruct>::iterator it = Gadgets.begin(); it != Gadgets.end(); it++) {
			if (it->second.Gadget == gadget) {
				int id = it->second.Id;
				if (id == 5 || id == 6 || id == 0x1B || id == 0x1A || id == 0x28 || id == 0x29) {
					SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
				}
				return;
			}
		}
	}
}

// @0x61E8B0
void StarbaseShipCustomizeInterface::Unknown_61E8B0()
{
	SClient *client = SBaseView->Client;
	client->Unknown_74C960(client->AvatarID);
}

// NOT EXACT: inline budget (0.42: the client calls the credits line's append out of line but inlines the cost line's,
// and keeps this in a frame slot; ui_avatar_cust.cpp's twin 0x551520 is exact in this form)
// @0x61E8E0
void StarbaseShipCustomizeInterface::Unknown_61E8E0(bool any)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<MoneyClass> *money;
	if (any) {
		money = UIShipCust_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	} else {
		money = UIShipCust_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	}
	if (money) {
		Unknown320 = money->Value;
		FontSys *text = NULL;
		switch (Page) {
			case 0: text = Unknown_BE67C4->Get_Style("credits_buy_ship01", Mesh->Peek_Object()); break;
			case 1: text = Unknown_BE67C4->Get_Style("credits_buy_ship02", Mesh->Peek_Object()); break;
			case 2: text = Unknown_BE67C4->Get_Style("credits_buy_ship03", Mesh->Peek_Object()); break;
			case 3: text = Unknown_BE67C4->Get_Style("credits_buy_ship04", Mesh->Peek_Object()); break;
		}
		if (text) {
			std::string line("Credits: ");
			line += Unknown320.Unknown_4F00C0(true);
			text->Change_Text(line.c_str(), 0);
		}
	}
	FontSys *text = NULL;
	switch (Page) {
		case 0: text = Unknown_BE67C4->Get_Style("cost_buy_ship01", Mesh->Peek_Object()); break;
		case 1: text = Unknown_BE67C4->Get_Style("cost_buy_ship02", Mesh->Peek_Object()); break;
		case 2: text = Unknown_BE67C4->Get_Style("cost_buy_ship03", Mesh->Peek_Object()); break;
		case 3: text = Unknown_BE67C4->Get_Style("cost_buy_ship04", Mesh->Peek_Object()); break;
	}
	if (text) {
		std::string line("Cost: ");
		line += Unknown328.Unknown_4F00C0(true);
		text->Change_Text(line.c_str(), 0);
	}
	FontSys *name = NULL;
	switch (Page) {
		case 0: name = Unknown_BE67C4->Get_Style("HULL_buy_ship01", Mesh->Peek_Object()); break;
		case 1: name = Unknown_BE67C4->Get_Style("Paint_buy_ship02", Mesh->Peek_Object()); break;
		case 2: name = Unknown_BE67C4->Get_Style("insignia_buy_ship03", Mesh->Peek_Object()); break;
		case 3: name = Unknown_BE67C4->Get_Style("Ship_buy_ship04", Mesh->Peek_Object()); break;
	}
	if (name) {
		name->Change_Text(Desc.Ship_Name, 0);
	}
}

// @0x61F0E0
void StarbaseShipCustomizeInterface::Unknown_61F0E0()
{
	if (View->Get_Unknown34()->Is_Active()) {
		int button = View->Get_Unknown34()->Unknown_5890B0("SHIP_CONFIRMATION");
		if (button) {
			if (button == 1) {
				Unknown23C = 5;
				Unknown239 = true;
			}
			View->Get_Unknown34()->Unknown_5C();
		}
		if (View->Get_Unknown34()->Unknown_5890B0("NEED_FUNDS")) {
			View->Get_Unknown34()->Unknown_5C();
		}
		if (View->Get_Unknown34()->Unknown_5890B0("NO_CHANGES")) {
			View->Get_Unknown34()->Unknown_5C();
		}
	}
}

// @0x61F180
void StarbaseShipCustomizeInterface::Unknown_61F180()
{
	if (Unknown334) {
		Unknown328 = Unknown338.Unknown_849AC0(&Unknown36C, &Desc);
	}
}

// NOT EXACT: functional (D64)
// @0x61F250
void StarbaseShipCustomizeInterface::Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d)
{
	Unknown174 = a;
	Unknown334 = true;
	Unknown178 = b;
	Unknown17C = c;
	Unknown180 = d;
	Unknown36C = *desc;
	Desc = *desc;
	if (Desc.Selected_Decal != 0x7FFFFFF) {
		Unknown318 = (Desc.Selected_Decal >= 0) ? Desc.Selected_Decal + 1 : Desc.Selected_Decal;
	}
	Unknown338 = *cost;
	Unknown_61AFC0();
	Unknown_6211D0(SBaseView->Client->Peek_Environment_Map_Manager());
	Unknown_61F180();
	Unknown_621390(Unknown318, Unknown330);
	Unknown_61F180();
	Unknown_621500();
	int i;
	for (i = 0; i < 3; i++) {
		Unknown_61AD30(Unknown_B894C0[i], i == Desc.Hull_Style);
	}
	for (i = 0; i < 3; i++) {
		Unknown_61AD30(Unknown_B894CC[i], i == Desc.Wing_Style);
	}
}

// @0x61F690
void StarbaseShipCustomizeInterface::Unknown_61F690()
{
	SClient *client = Client->As_Space_Client();
	int id = client->Unknown1138;
	client->Send_Packet(&RecustomizeShipDonePacketClass(id, Desc));
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
