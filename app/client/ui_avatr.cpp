// Earth & Beyond client (reconstruction): app\Client\ui_avatr.cpp. Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "refcount.h"

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral

	char Unknown04[0x2C - 0x04];						// +0x04
	unsigned int Unknown2C;								// +0x2C (0x556290 sets 0x400)
};

// The pulls' gadget (0x80 bytes, constructor 0x671300), as far as 0x556290 makes it
class UIAvatrGadget671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d);	// 0x671300 (0x556290: the area model, 0, the window's mesh, 0, 0, 0)  // NAME 1 [area, a, mesh, b, c, d]: neutral

	char Unknown30[0x80 - 0x30];						// +0x30
};

// A gadget with its pull model (0x10 bytes, constructor 0x585920; the window's +0x160 / +0x16C / +0x180)
class UIAvatrPullClass	// NAME 1: neutral (D10)
{
public:
	UIAvatrPullClass();									// 0x585920
	void Unknown_5859A0();								// 0x5859A0 (0x556290, once the gadget is in)
	void Show(bool on)		// NAME 1: neutral (D10): inline (0x556630)
	{
		Gadget->Unknown_40(on);
		Gadget->Unknown_8C(!on);
		Pull->Set_Hidden(!on);
	}

	GadgetClass *Gadget;								// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('BODY_PULL' / 'HEAD_PULL', 0x556290)  // NAME 2: what 0x556290 puts there
	char Unknown08[0x10 - 0x08];						// +0x08
};

// The +0x8C object's +0x194 (refcounted; +0x110 is 1 when the beard is off, 0x556630)
class UIAvatrUnknown194Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	char Unknown08[0x110 - 0x08];						// +0x08
	int Unknown110;										// +0x110
};

// The window's +0x8C, as far as this file reads it
class UIAvatrUnknown08CClass	// NAME 1: neutral (D10)
{
public:
	char Unknown00[0x194];								// +0x00
	UIAvatrUnknown194Class *Peek_Unknown194() { return(Unknown194); }	// NAME 1: neutral (D10)
	UIAvatrUnknown194Class *Unknown194;					// +0x194
	char Unknown198[0x1B8 - 0x198];						// +0x198
	RenderObjClass *Peek_Model() { return(Unknown1B8); }	// NAME 1: neutral (D10)
	RenderObjClass *Unknown1B8;							// +0x1B8 (the model: its texts by name)
};

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, unknown]: as eff_o.cpp's
void Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

// The object at *0xBFAEE0 (kernel.cpp's), as far as this file calls it: its slot +0x30 gives a texture by name
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual TextureClass *Get_Texture(const char *name, int a, int b, int c);	// +0x30 (the name, 0, 0, 1)  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

// The +0x94 object's +0x54, as far as 0x556B20 writes it
struct UIAvatrUnknown094Unknown054Struct	// NAME 1: neutral (D10)
{
	char Unknown000[0x214];								// +0x000
	bool Unknown214;									// +0x214 (0x556B20 sets it)
	char Unknown215[0x231 - 0x215];						// +0x215
	bool Unknown231;									// +0x231 (set on the head tab)
};

// The window's +0x94, as far as 0x556B20 calls it
class UIAvatrUnknown094Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84();
	virtual void Unknown_88();							// +0x88 (0x556B20, after +0xDC)
	virtual void Unknown_8C();							// +0x8C

	char Unknown04[0x54 - 0x04];						// +0x04
	UIAvatrUnknown094Unknown054Struct *Unknown054;		// +0x54
	char Unknown58[0xDC - 0x58];						// +0x58
	bool Unknown0DC;									// +0xDC (set on the head tab)
};

// fontmanager.h's FontManagerClass at 0xBE67C4 (its header cannot be included next to sclient.h: workflow.md 3.3)
class FontSys : public RenderObjClass { public: void Change_Text(const char *text, int unknown); };	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)  // NAME 1 [text, unknown]: as loadingview.cpp's
class UIAvatrFontManagerClass { public: FontSys *Unknown_6D25D0(const char *name, RenderObjClass *object); };	// NAME 1: neutral (D10): fontmanager.h's Get_Style  // NAME 1 [name, object]: as loadingview.cpp's
extern UIAvatrFontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The window's buttons and their texts (0xB7AF84: 50 entries, the gadgets' order at +0x98)
struct UIAvatrPartStruct	// NAME 1: neutral (D10)
{
	const char *Button;		// NAME 1: neutral
	const char *Text;		// NAME 1: neutral
};
static UIAvatrPartStruct UIAvatrParts[50] = {	// NAME 1: neutral (D10)
	{ "gender_tab", "male_fe_avatar00" },
	{ "head_tab", "head_fe_avatar00" },
	{ "body_tab", "physique_fe_avatar00" },
	{ "random", "random_fe_avatar00" },
	{ "turn", NULL },
	{ "turn_left", NULL },
	{ "turn_right", NULL },
	{ "zoom_in", NULL },
	{ "zoom_out", NULL },
	{ "pan", NULL },
	{ "pan_up", NULL },
	{ "pan_down", NULL },
	{ "pan_left", NULL },
	{ "pan_right", NULL },
	{ "confirm", "confirm_fe_avatar00" },
	{ "exit", "exit_fe_avatar00" },
	{ "hair_left", "style_fe_avatar00" },			// 16: the head tab's
	{ "hair_right", "hair_title_fe_avatar00" },
	{ "beard_left", "beard_fe_avatar00" },
	{ "beard_right", "face_title_fe_avatar00" },
	{ "head_type_left", "head_type_fe_avatar00" },
	{ "head_type_right", "acc_title_fe_avatar00" },
	{ "face_left", "face_type_fe_avatar00" },
	{ "face_right", "face_type_fe_avatar00" },
	{ "tattoo_left", "details_fe_avatar00" },
	{ "tattoo_right", "details_fe_avatar00" },
	{ "eye_left", "glasses_fe_avatar00" },
	{ "eye_right", "glasses_fe_avatar00" },
	{ "ear_left", "ear_fe_avatar00" },
	{ "ear_right", "ear_fe_avatar00" },
	{ "hair_color", "hair_color_fe_avatar00" },
	{ "eye_color", "eye_color_fe_avatar00" },
	{ "skin_color", "skin_color_fe_avatar00" },
	{ "scale_in", "head_text_fe_avatar00" },
	{ "scale_out", NULL },
	{ "shirt_style_lef", "shirt_style_fe_avatar00" },	// 35: the body tab's
	{ "shirt_style_rig", "body_title_fe_avatar00" },
	{ "pant_style_left", "pant_style_fe_avatar00" },
	{ "pant_style_righ", "cloth_title_fe_avatar00" },
	{ "p_shirt_color", "p_shirt_color_fe_avatar00" },
	{ "s_shirt_color", "s_shirt_color_fe_avatar00" },
	{ "p_pant_color", "p_pant_color_fe_avatar00" },
	{ "s_pant_color", "s_pant_color_fe_avatar00" },
	{ "move_up", "height_text_fe_avatar00" },
	{ "move_down", "height_var_fe_avatar00" },
	{ "move_left", "weight_text_fe_avatar00" },
	{ "move_right", "weight_var_fe_avatar00" },
	{ "metal00", "metals_fe_avatar00" },
	{ "metal01", "body_text_fe_avatar00" },
	{ "metal02", NULL },
};

// The avatar creation window (ui_avatr.cpp's __FILE__), as far as this file's functions so far use it
class AvatarCreationInterface : public InterfaceClass	// NAME 3: the client's RTTI (0xB7B848: SignalBaseClass::MemberCallback<GadgetClass,AvatarCreationInterface,...>)
{
public:
	void Unknown_556290();								// 0x556290 (the body and head pulls made)
	void Unknown_556630(int tab, bool on);				// 0x556630 (a tab's gadgets and texts shown or hidden: 0 none, 1 head, 2 body)  // NAME 1 [tab, on]: neutral
	void Unknown_556B20(int tab);						// 0x556B20 (the tab: 0 gender, 1 head, 2 body)  // NAME 1 [tab]: neutral
	void Unknown_55A550(int a, int b, int c, int d);	// 0x55A550 (0x556B20: 1, 0, 0, 0 on the head tab, 0, 0, 0, 0 on the body tab)  // NAME 1 [a, b, c, d]: neutral
	void Unknown_55B110(int arg);						// 0x55B110 (0x556B20: 0x20, 0x2A on the body tab)  // NAME 1 [arg]: neutral
	void Unknown_55B2D0();								// 0x55B2D0 (0x556B20, the head tab)
	void Unknown_Set_Palette(const char *name);		// 0x55C350 (the texture of that name onto the model's COLOR_PALETTE: ui_avatar_cust.cpp's 0x5513E0)  // NAME 1: neutral (D10); NAME 1 [name]: neutral

	UIMeshClass *Unknown084;							// +0x84 (the window's mesh)
	int Unknown088;										// +0x88 (the tab, 0x556B20)
	UIAvatrUnknown08CClass *Unknown08C;					// +0x8C
	char Unknown90[0x94 - 0x90];						// +0x90
	UIAvatrUnknown094Class *Unknown094;					// +0x94
	GadgetClass *Unknown098[50];						// +0x98 (UIAvatrParts' gadgets)
	UIAvatrPullClass *Unknown160;						// +0x160 (the body's pull)
	char Unknown164[0x16C - 0x164];						// +0x164
	UIAvatrPullClass *Unknown16C;						// +0x16C (the head's pull)
	char Unknown170[0x180 - 0x170];						// +0x170
	UIAvatrPullClass *Unknown180;						// +0x180
};

// 0x556290
void AvatarCreationInterface::Unknown_556290()
{
	Unknown160 = new UIAvatrPullClass;
	Unknown16C = new UIAvatrPullClass;
	RefCountPtr<RenderObjClass> area = Create_Get(Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("BODY_AREA"));
	Unknown_496AE0(area.Peek(), 0);
	Unknown160->Gadget = new UIAvatrGadget671300Class(area.Peek(), 0, Unknown084, 0, 0, 0);
	Unknown160->Gadget->Unknown2C |= 0x400;
	Unknown160->Unknown_5859A0();
	Unknown160->Pull = Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("BODY_PULL");
	Unknown_496AE0(Unknown160->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown160->Gadget, 0x37, View, 1, 1);
	area = Create_Get(Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("HEAD_AREA"));
	Unknown_496AE0(area.Peek(), 0);
	Unknown16C->Gadget = new UIAvatrGadget671300Class(area.Peek(), 0, Unknown084, 0, 0, 0);
	Unknown16C->Gadget->Unknown2C |= 0x400;
	Unknown16C->Unknown_5859A0();
	Unknown16C->Pull = Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("HEAD_PULL");
	Unknown_496AE0(Unknown16C->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown16C->Gadget, 0x38, View, 1, 1);
}

// 0x556630
void AvatarCreationInterface::Unknown_556630(int tab, bool on)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	int i;
	switch (tab) {
		case 0:
			if (Unknown180) {
				Unknown180->Gadget->Unknown_40(false);
				Unknown180->Gadget->Unknown_8C(true);
				Unknown180->Pull->Set_Hidden(true);
			}
			if (Unknown160) {
				Unknown160->Gadget->Unknown_40(false);
				Unknown160->Gadget->Unknown_8C(true);
				Unknown160->Pull->Set_Hidden(true);
			}
			if (Unknown16C) {
				Unknown16C->Gadget->Unknown_40(false);
				Unknown16C->Gadget->Unknown_8C(true);
				Unknown16C->Pull->Set_Hidden(true);
			}
			break;
		case 1:
			{
				for (i = 16; i < 35; i++) {
					Unknown098[i]->Unknown_40(on);
					Unknown098[i]->Unknown_8C(!on);
					if (UIAvatrParts[i].Text) {
						FontSys *text = Unknown_BE67C4->Unknown_6D25D0(UIAvatrParts[i].Text, model.Peek());
						if (text) text->Set_Hidden(!on);
					}
				}
				if (Unknown180) {
					Unknown180->Gadget->Unknown_40(on);
					Unknown180->Gadget->Unknown_8C(!on);
					Unknown180->Pull->Set_Hidden(!on);
				}
				if (Unknown160) {
					Unknown160->Gadget->Unknown_40(false);
					Unknown160->Gadget->Unknown_8C(true);
					Unknown160->Pull->Set_Hidden(true);
				}
				RefCountPtr<UIAvatrUnknown194Class> desc = Create_Peek(Unknown08C->Peek_Unknown194());
				if (desc->Unknown110 == 1) {
					Unknown098[18]->Unknown_40(false);
					Unknown098[18]->Unknown_8C(true);
					Unknown098[19]->Unknown_40(false);
					Unknown098[19]->Unknown_8C(true);
					FontSys *text = Unknown_BE67C4->Unknown_6D25D0("beard_fe_avatar00", model.Peek());
					if (text) text->Set_Hidden(true);
				}
			}
			break;
		case 2:
			for (i = 35; i < 50; i++) {
				Unknown098[i]->Unknown_40(on);
				Unknown098[i]->Unknown_8C(!on);
				if (UIAvatrParts[i].Text) {
					FontSys *text = Unknown_BE67C4->Unknown_6D25D0(UIAvatrParts[i].Text, model.Peek());
					if (text) text->Set_Hidden(!on);
				}
			}
			if (Unknown180) {
				Unknown180->Gadget->Unknown_40(on);
				Unknown180->Gadget->Unknown_8C(!on);
				Unknown180->Pull->Set_Hidden(!on);
			}
			if (Unknown160) {
				Unknown160->Gadget->Unknown_40(on);
				Unknown160->Gadget->Unknown_8C(!on);
				Unknown160->Pull->Set_Hidden(!on);
			}
			if (Unknown16C) {
				Unknown16C->Gadget->Unknown_40(false);
				Unknown16C->Gadget->Unknown_8C(true);
				Unknown16C->Pull->Set_Hidden(true);
			}
			break;
	}
}

// NOT EXACT: inline budget (0.50, 972 bytes against 0x379: the client calls the palette helper's two ~RefCountPtr out of line, 0x4740B0, in both inlined copies, and keeps 1 in ebx; ours inlines them and keeps 0; attempts.py show 0x556B20)
// 0x556B20
void AvatarCreationInterface::Unknown_556B20(int tab)
{
	if (Unknown088 == tab && (tab == 1 || tab == 2)) {
		return;
	}
	Unknown088 = tab;
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<RenderObjClass> frame = Create_Get(model->Get_Sub_Object_By_Name("FRAME_COLOR"));
	switch (tab) {
		case 0:
			Unknown_556630(1, false);
			Unknown_556630(2, false);
			Unknown_556630(0, true);
			if (frame) frame->Set_Hidden(true);
			Unknown_55B110(0x20);
			Unknown094->Unknown0DC = false;
			Unknown094->Unknown_88();
			Unknown094->Unknown_8C();
			Unknown094->Unknown054->Unknown231 = false;
			Unknown094->Unknown054->Unknown214 = true;
			break;
		case 1:
			Unknown_556630(0, false);
			Unknown_556630(2, false);
			Unknown_556630(1, true);
			Unknown_55B2D0();
			Unknown_55A550(1, 0, 0, 0);
			if (frame) frame->Set_Hidden(false);
			Unknown_55B110(0x20);
			Unknown_Set_Palette("fa_avatar_color.tga");
			Unknown094->Unknown0DC = true;
			Unknown094->Unknown_88();
			Unknown094->Unknown_8C();
			Unknown094->Unknown054->Unknown231 = true;
			Unknown094->Unknown054->Unknown214 = true;
			break;
		case 2:
			Unknown_556630(0, false);
			Unknown_556630(1, false);
			Unknown_556630(2, true);
			Unknown_55A550(0, 0, 0, 0);
			if (frame) frame->Set_Hidden(false);
			Unknown_55B110(0x2A);
			Unknown_Set_Palette("fa_avatar_color.tga");
			Unknown094->Unknown0DC = false;
			Unknown094->Unknown_88();
			Unknown094->Unknown_8C();
			Unknown094->Unknown054->Unknown231 = false;
			Unknown094->Unknown054->Unknown214 = true;
			break;
	}
}

// 0x55C350 (after the FAIL_IFs of 0x55C280, lines 2207 / 2211, in the client; no caller: 0x556B20 inlines it)
void AvatarCreationInterface::Unknown_Set_Palette(const char *name)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<RenderObjClass> palette = Create_Get(model->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	TextureClass *texture = Unknown_BFAEE0->Get_Texture(name, 0, 0, 1);
	if (texture) {
		Unknown_495FD0(palette.Peek(), texture, 0);
		texture->Release_Ref();
	}
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avatr.cpp (its __FILE__, 0xB7B80C: 0x55C280's FAIL_IFs on lines 2207 / 2211)
**
**	The avatar creation window, AvatarCreationInterface (the client's RTTI: the type name of its
**	FlashingGadgetEvent callback, 0xB7B848, sits in this file's literals). The file's evidence: its
**	code runs from 0x553AF0 (after ui_avatar_dialog.cpp, whose last asserts are 0x5537A0's) past the
**	FAIL_IFs of 0x55C280; 0x553AF0 loads 'fa_fe_avatar00.ini', and the literals of the file's own
**	(the '*_fe_avatar00' names, 'BODY_PULL' 0xB7B76C, 'AvatarCreationHead' ...) lie between
**	ui_avatar_dialog.cpp's __FILE__ (0xB7AEBC) and this file's (0xB7B80C). It is ui_avatar_cust.cpp's
**	sibling: the literals both use ('COLOR_PALETTE', 'HEAD_PULL', 'FRAME_COLOR', 'fa_avatar_color.tga')
**	are merged into ui_avatar_cust.cpp's, which links first.
**
**	So far (TK6, claim #88): 0x556290 (the body and head pulls made) and 0x556630 (a tab's gadgets and
**	texts shown or hidden) exact, 0x556B20 (the
**	tab chosen; NOT EXACT) and the palette helper 0x55C350 it inlines (exact; defined at the end here
**	for now, it belongs after the FAIL_IFs of line 2207). The gadgets at +0x98 follow UIAvatrParts
**	(0xB7AF84, 50 entries: 16 shared, 19 for the head tab, 15 for the body tab). The rest of the file
**	is not rebuilt; the class is declared only as far as these use it.
*/
