// Earth & Beyond client (reconstruction): app\client\ui_starbase.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_starbase.cpp', 0x633010-0x648340 by its asserts and its string order).
// So far only the starbase lights panel (the class below, 0x646170-0x648238, ST1); the file's
// own functions with their asserts (lines 368-4180) come before it in the client.
#include <stdio.h>
#include <string.h>
#include <vector>
#include "ui_chat_panel.h"
#include "sbaseview.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "..\..\ww3d2\light.h"
#include "wwmath.h"
#include "aabox.h"
#include "matrix3d.h"
#include "rendobj.h"
#include "starbase_lights.h"
#include "..\audio\soundlib.h"
#include "control_keys_manager.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4, as ui_login.cpp's

void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color);	// 0x496C00 (ro_funcs: the texture's colour at u, v)  // NAME 1 [robj, u, v, color]: as ui_avatar_cust.cpp's
void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (the pointer, 0..1)  // NAME 1 [x, y]: as ui_avatar_cust.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_avatar_cust.cpp's

// The colour pad's gadget (0x80 bytes, constructor 0x671300), as ui_avatar_cust.cpp's UIAvatarCustGadget671300Class
class UIStarbaseLightsGadget671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIStarbaseLightsGadget671300Class(RenderObjClass *area, int a, int b, int c, int d);	// 0x671410 (0x646360: the palette, 0, 0, 0, 0)  // NAME 1 [area, a, b, c, d]: neutral
	virtual bool Unknown_30(float x, float y);			// +0x30 (GadgetClass's pure slot: the pad's own)  // NAME 1 [x, y]: GadgetClass's
	void Unknown_671630();								// 0x671630 (the buttons' flags off)
	char Unknown30[0x64 - 0x30];						// +0x30
	bool Unknown64;										// +0x64 (the left button held)
	bool Unknown65;										// +0x65 (the right button held)
	char Unknown66[0x80 - 0x66];						// +0x66
};

// The pad and its pull (the panel's +0xD8), as ui_avatar_cust.cpp's UIAvatarCustGadgetHolderStruct
struct UIStarbaseLightsGadgetHolderStruct	// NAME 1: neutral (D10)
{
	UIStarbaseLightsGadgetHolderStruct();				// 0x585920 (0x10 bytes)
	void Unknown_5859A0();								// 0x5859A0 (once the gadget is in)
	GadgetClass *Unknown00;								// +0x00 (the pad: slots +0x08 / +0x64)
	RenderObjClass *Unknown04;							// +0x04 (the pull, a bone of the panel's model)
	bool Unknown08;										// +0x08 (dragged: 0x647B30)
	char Unknown09[0x10 - 0x09];						// +0x09
};

// The starbase lights panel (0x444 bytes; constructor 0x646170 over ui_chat_panel_base.cpp's 0x56D280, table
// 0xAF8610; no caller of the constructor in the client: a debug panel). Its texts are 'near_light02a',
// 'far_light02a', 'Int_light02a', 'Fill_light02a' and 'GO_light02a' in the 'fa_light02a.ini' model (0x646360).
// Declared as far as this file's functions use it (interim placement in this file, D137).
class UIStarbaseLightsClass : public UIChatPanelBaseClass	// NAME 1: neutral, after its model's 'light02a' names
{
public:
	UIStarbaseLightsClass(SClient *client);				// 0x646170  // NAME 1 [client]: the base's
	virtual ~UIStarbaseLightsClass();					// 0x646290 (deleting 0x646260)
	virtual void Unknown_54();							// +0x54 0x646360
	virtual void Unknown_58();							// +0x58 0x6466F0 (activated: the starbase, its lights, light 0)
	virtual void Unknown_5C();							// +0x5C 0x6467B0
	virtual void Unknown_60();							// +0x60 0x6467E0 (every frame: the sliders, the pad)
	virtual bool Unknown_68(int key);					// +0x68 0x646810 (the panel's keys 1-6)  // NAME 1 [key]: InterfaceClass's
	void Unknown_646940();								// 0x646940
	void Unknown_646A00();								// 0x646A00 (the lights read)
	void Unknown_646E00();								// 0x646E00 (key 2)
	void Unknown_648190();								// 0x648190 (no caller: the pad points from the lights manager)
	void Unknown_646F70(bool force);					// 0x646F70 (the sliders into the current light; its texts)  // NAME 1 [force]: neutral
	void Unknown_6474C0(int index);						// 0x6474C0 (a light chosen; mode 1: the ambient light)  // NAME 1 [index]: neutral
	void Unknown_647B30();								// 0x647B30 (the colour pad dragged)
	void Unknown_647F30(Vector3 point);					// 0x647F30 (the pull placed at a pad point, the colour read)  // NAME 1 [point]: neutral

	// The base's +0x84 model as the font manager takes it (UIChatPanelBaseClass declares it a plain pointer)
	const RefCountPtr<RenderObjClass> &Model() { return(*(const RefCountPtr<RenderObjClass> *)&Unknown84); }	// NAME 1 [Model]: neutral

	GadgetClass *UnknownA0[6];							// +0xA0 (the buttons exit, save, light00-03: 0x646360; the constructor clears them with one memset)
	GadgetSlider *Slider[4];							// +0xB8 (near, far, intensity, fill: 0x646F70's texts)  // NAME 1: neutral
	float SliderValue[4];								// +0xC8 (the values last seen: 0x646F70 compares them)  // NAME 1: neutral
	UIStarbaseLightsGadgetHolderStruct *UnknownD8;		// +0xD8
	UIStarbaseClass *Starbase;							// +0xDC (0x640E90 / 0x640EC0 / 0x640EF0 / 0x640F30, ui_starbase.cpp's)  // NAME 1: neutral
	int UnknownE0;										// +0xE0 (0: a light; else the ambient light)
	int UnknownE4;										// +0xE4
	int UnknownE8;										// +0xE8
	int UnknownEC;										// +0xEC (1 from the constructor)
	int Current;										// +0xF0 (the light chosen; -1 from the constructor: 'Light %02d')  // NAME 1: neutral
	int UnknownF4;										// +0xF4
	float Intensity[20];								// +0xF8 ('Int %0.2f'; LightClass::Set_Intensity)  // NAME 1: the LightClass setter's
	float Fill;											// +0x148 (0.5 from the constructor; 'Fill %0.2f')  // NAME 1: its text's
	Vector3 Color[20];									// +0x14C (from the pad: Set_Diffuse)  // NAME 1: neutral
	Vector3 Position[20];								// +0x23C (the pad points)  // NAME 1: neutral
	Vector3 Atten[20];									// +0x32C (X near, Y far: Set_Far_Attenuation_Range)  // NAME 1: neutral
	Vector3 AmbientColor;								// +0x41C (to 0x640E90)  // NAME 1: neutral
	Vector3 AmbientPosition;							// +0x428  // NAME 1: neutral
	std::vector<LightClass *> Lights;					// +0x434 (the allocator byte from the constructor's argument slot)  // NAME 1: neutral
};

// 0x646170
UIStarbaseLightsClass::UIStarbaseLightsClass(SClient *client) :
	UIChatPanelBaseClass(client),
	UnknownD8(NULL),
	Starbase(NULL),
	UnknownE0(0),
	UnknownE4(0),
	UnknownE8(0),
	UnknownEC(1),
	Current(-1),
	UnknownF4(0),
	Fill(0.5f)
{
	memset(UnknownA0, 0, sizeof(UnknownA0));
	memset(Slider, 0, sizeof(Slider));
	memset(SliderValue, 0, sizeof(SliderValue));
}

// 0x646290
UIStarbaseLightsClass::~UIStarbaseLightsClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Starbase->Unknown_640F10();
	}
}

// The buttons 0x646360 makes: their models' names and their texts (0xB8B864 / 0xB8B880)
static const char *Unknown_B8B864[6] = { "exit", "save", "light00", "light01", "light02", "light03" };	// NAME 1: neutral (D10)
static const char *Unknown_B8B880[6] = { NULL, "save_light02a", "previous_light02a", "next_light02a", "diffuse_light02a", "ambient_light02a" };	// NAME 1: neutral (D10)

// NOT EXACT: register (ours keeps 0 in ebp for the pushes and the stores, the client pushes immediates and never
// saves ebp; tried: one or two loop counters, named locals for the zero vector and the key combo)
// 0x646360
void UIStarbaseLightsClass::Unknown_54()
{
	if (Unknown_98("light02a", "fa_light02a.ini")) {
		Unknown_646940();
		int i;
		for (i = 0; i < 6; i++) {
			UnknownA0[i] = Unknown_56D510(i + 1, Unknown_B8B864[i], Unknown_B8B880[i]);
		}
		for (i = 0; i < 4; i++) {
			Slider[i] = Unknown_56D8D0(i + 8, 0);
			Slider[i]->Unknown_66E460(Model(), true, i);
			if (i == 2) {
				Slider[2]->Unknown_66FA90(3.0f, 0.01f, true);
				Slider[2]->Unknown_A4(0.5f, false);
			} else if (i == 3) {
				Slider[3]->Unknown_66FA90(3.0f, 0.01f, true);
				Slider[3]->Unknown_A4(0.75f, false);
			} else {
				Slider[i]->Unknown_66FA90(1000.0f, 0.1f, true);
				Slider[i]->Unknown_A4(500.0f, false);
			}
		}
		UnknownD8 = new UIStarbaseLightsGadgetHolderStruct;
		RefCountPtr<RenderObjClass> palette = Create_Get(Unknown84->Get_Sub_Object_By_Name("COLOR_PALETTE"));
		Unknown_496AE0(palette.Peek(), false);
		UnknownD8->Unknown00 = new UIStarbaseLightsGadget671300Class(palette.Peek(), 0, 0, 0, 0);
		UnknownD8->Unknown00->Unknown2C_0A = true;
		UnknownD8->Unknown04 = Unknown84->Get_Sub_Object_By_Name("PULL_COLOR");
		Unknown_496AE0(UnknownD8->Unknown04, true);
		UnknownD8->Unknown_5859A0();
		UnknownD8->Unknown00->Unknown_65FFA0(-1);
		Unknown64.Unknown_6870E0(UnknownD8->Unknown00, 0xD, View, true, false);
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)Unknown4C)->Add(ControlKeyComboStruct(0x1B, 0), (void *)1);
	}
}

// 0x6466F0
void UIStarbaseLightsClass::Unknown_58()
{
	if (!Active) {
		UIChatPanelBaseClass::Unknown_58();
		UnknownE0 = 0;
		Starbase = ((SBaseViewClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown135C)->Get_Starbase().Peek();
		Starbase->Unknown_640D80(&UnknownE4, &UnknownE8, &UnknownEC);
		Unknown_646A00();
		Unknown_6474C0(0);
	}
}

// 0x6467B0
void UIStarbaseLightsClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Starbase->Unknown_640F10();
	}
}

// 0x6467E0
void UIStarbaseLightsClass::Unknown_60()
{
	Unknown_646F70(false);
	Unknown_647B30();
}

// 0x646810
bool UIStarbaseLightsClass::Unknown_68(int key)
{
	((LineDisplayClientClass *)Client)->As_Space_Client();
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (key) {
		case 1:
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			Unknown_646E00();
			return(true);
		case 4:
			Unknown_6474C0((Current + 1) % UnknownF4);
			return(true);
		case 3:
			{
				int index = Current - 1;
				if (index < 0) {
					index = UnknownF4 - 1;
				}
				Unknown_6474C0(index);
			}
			return(true);
		case 5:
			UnknownE0 = 0;
			Unknown_6474C0(Current);
			return(true);
		case 6:
			UnknownE0 = 1;
			Unknown_6474C0(Current);
			return(true);
	}
	return(false);
}

// 0x646940
void UIStarbaseLightsClass::Unknown_646940()
{
	for (int i = 0; i < 20; i++) {
		Intensity[i] = 1.0f;
		Color[i].X = 0.5f;
		Color[i].Y = 0.5f;
		Color[i].Z = 0.5f;
		Position[i].X = 0.5f;
		Position[i].Y = 0.5f;
		Position[i].Z = 0.5f;
		Atten[i].X = 0.0f;
		Atten[i].Y = 50.0f;
		Atten[i].Z = 0.0f;
	}
	AmbientColor.Set(0.0f, 0.0f, 0.0f);
	AmbientPosition.Set(0.0f, 0.0f, 0.0f);
	Fill = 0.75f;
}

// NOT EXACT: inline-budget (the client keeps the temporary vector's _Destroy 0x644410 out of line, ours inlines it)
// and the far attenuation range: the client copies the two floats with mov, as Zero Hour's light.h overload
// Get_Far_Attenuation_Range(float &, float &) gives (tested with an equivalent helper); this tree's light.h has
// only the double & one, whose fld / fstp is kept here
// 0x646A00
void UIStarbaseLightsClass::Unknown_646A00()
{
	Unknown_BE82D0->Unknown_764530();
	Lights = Starbase->Unknown_640DF0();
	UnknownF4 = Lights.size();
	int i;
	for (i = 0; i < UnknownF4; i++) {
		LightClass *light = Lights[i];
		light->Get_Ambient(&AmbientColor);
		light->Get_Diffuse(&Color[i]);
		Intensity[i] = light->Get_Intensity();
		double start;
		double end;
		light->Get_Far_Attenuation_Range(start, end);
		Atten[i].X = start;
		Atten[i].Y = end;
	}
	Fill = Unknown_BE82D0->Unknown_764EA0(UnknownE4, UnknownE8, UnknownEC);
	AmbientColor = Unknown_BE82D0->Unknown_764CF0(UnknownE4, UnknownE8, UnknownEC);
	for (i = 0; i < UnknownF4; i++) {
		Position[i] = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, i);
	}
	AmbientPosition = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, 1000);
}

// 0x646E00
void UIStarbaseLightsClass::Unknown_646E00()
{
	int i = 0;
	for (std::vector<LightClass *>::iterator it = Lights.begin(); it != Lights.end(); it++, i++) {
		Unknown_BE82D0->Unknown_764880(UnknownE4, UnknownE8, UnknownEC, i, *it);
	}
	Unknown_BE82D0->Unknown_764F60(UnknownE4, UnknownE8, UnknownEC, Fill);
	Unknown_BE82D0->Unknown_764E20(UnknownE4, UnknownE8, UnknownEC, AmbientColor);
	for (i = 0; i < UnknownF4; i++) {
		Unknown_BE82D0->Unknown_764C30(UnknownE4, UnknownE8, UnknownEC, i, Position[i]);
	}
	Unknown_BE82D0->Unknown_764C30(UnknownE4, UnknownE8, UnknownEC, 1000, AmbientPosition);
	Unknown_BE82D0->Unknown_764550();
}

// 0x646F70
void UIStarbaseLightsClass::Unknown_646F70(bool force)
{
	bool changed = false;
	int i;
	for (i = 0; i < 4; i++) {
		if (Slider[i]->Unknown_60()) {
			Slider[i]->Unknown_66FD80();
			if (Slider[i]->Value != SliderValue[i]) {
				changed = true;
			}
		}
	}
	if (changed || force) {
		Atten[Current].X = Slider[0]->Get_Value();
		Atten[Current].Y = Slider[1]->Get_Value();
		Intensity[Current] = Slider[2]->Get_Value();
		Fill = Slider[3]->Get_Value();
		if (Atten[Current].X > Atten[Current].Y) {
			Atten[Current].X = Atten[Current].Y;
			Slider[0]->Unknown_A4(Atten[Current].X, false);
		}
		LightClass *light = Lights[Current];
		light->Set_Far_Attenuation_Range(Atten[Current].X, Atten[Current].Y);
		light->Set_Intensity(WWMath::Max(0.01f, Intensity[Current]));
		Starbase->Unknown_640EC0(Fill);
		Starbase->Unknown_640F30(Current, Atten[Current].Y);
		for (i = 0; i < 4; i++) {
			if (Slider[i]->Unknown_60()) {
				SliderValue[i] = Slider[i]->Get_Value();
			}
		}
		char buffer[64];
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("near_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Near %d", (int)Slider[0]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Far %d", (int)Slider[1]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Int %0.2f", Slider[2]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Fill %0.2f", Slider[3]->Get_Value());
		text->Change_Text(buffer, 0);
	}
}

// 0x6474C0
void UIStarbaseLightsClass::Unknown_6474C0(int index)
{
	Current = index;
	Starbase->Unknown_640EF0(index);
	if (UnknownE0 == 0) {
		Slider[0]->Unknown_8C(false);
		Slider[1]->Unknown_8C(false);
		Slider[2]->Unknown_8C(false);
		Slider[0]->Unknown_40(true);
		Slider[1]->Unknown_40(true);
		Slider[2]->Unknown_40(true);
		Slider[0]->Unknown_A4(Atten[Current].X, false);
		Slider[1]->Unknown_A4(Atten[Current].Y, false);
		Starbase->Unknown_640F30(Current, Atten[Current].Y);
		Slider[2]->Unknown_A4(Intensity[Current], false);
		Slider[3]->Unknown_A4(Fill, false);
		for (int i = 0; i < 4; i++) {
			SliderValue[i] = Slider[i]->Get_Value();
		}
		Unknown_647F30(Position[Current]);
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("GO_light02a", Model());
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "Light %02d", Current);
		text->Change_Text(buffer, 0);
		Unknown_646F70(true);
	} else {
		Slider[0]->Unknown_8C(true);
		Slider[1]->Unknown_8C(true);
		Slider[2]->Unknown_8C(true);
		Slider[0]->Unknown_40(false);
		Slider[1]->Unknown_40(false);
		Slider[2]->Unknown_40(false);
		Unknown_647F30(AmbientPosition);
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("GO_light02a", Model());
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "Ambient Light", Current);
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("near_light02a", Model());
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", Model());
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", Model());
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", Model());
		text->Change_Text("", 0);
	}
}

// 0x647B30
void UIStarbaseLightsClass::Unknown_647B30()
{
	if (((UIStarbaseLightsGadget671300Class *)UnknownD8->Unknown00)->Unknown64) {
		UnknownD8->Unknown08 = true;
	}
	if ((((UIStarbaseLightsGadget671300Class *)UnknownD8->Unknown00)->Unknown64 || ((UIStarbaseLightsGadget671300Class *)UnknownD8->Unknown00)->Unknown65) && UnknownD8->Unknown08) {
		AABoxClass box;
		UnknownD8->Unknown00->Unknown_64(box);
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		x = WWMath::Min(x, box.Center.X + box.Extent.X);
		x = WWMath::Max(x, box.Center.X - box.Extent.X);
		y = WWMath::Min(y, box.Center.Y + box.Extent.Y);
		y = WWMath::Max(y, box.Center.Y - box.Extent.Y);
		int bone = Unknown84->Get_Sub_Object_Bone_Index(UnknownD8->Unknown04);
		Unknown84->Capture_Bone(bone);
		Matrix3D tm = UnknownD8->Unknown04->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
		tm[2][3] = 0.0f;
		Unknown84->Control_Bone(bone, tm, false);
		Vector2 point;
		point.X = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		point.Y = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		if (UnknownE0 == 0) {
			GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
			Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &Color[Current]);
			Position[Current].Set(point.X, point.Y, 0.0f);
			Lights[Current]->Set_Diffuse(Color[Current]);
		} else {
			GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
			Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &AmbientColor);
			AmbientPosition.Set(point.X, point.Y, 0.0f);
			Starbase->Unknown_640E90(AmbientColor);
		}
	} else {
		UnknownD8->Unknown08 = false;
	}
	((UIStarbaseLightsGadget671300Class *)UnknownD8->Unknown00)->Unknown_671630();
}

// 0x647F30
void UIStarbaseLightsClass::Unknown_647F30(Vector3 point)
{
	AABoxClass box;
	UnknownD8->Unknown00->Unknown_64(box);
	float top = box.Center.Y - box.Extent.Y;
	float left = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f;
	float height = box.Extent.Y * 2.0f;
	int bone = Unknown84->Get_Sub_Object_Bone_Index(UnknownD8->Unknown04);
	Unknown84->Capture_Bone(bone);
	Matrix3D tm = UnknownD8->Unknown04->Get_Transform();
	tm.Set_X_Translation((point.X * width) + left - box.Center.X);
	tm.Set_Y_Translation((point.Y * height) + top - box.Center.Y);
	tm[2][3] = 0.0f;
	Unknown84->Control_Bone(bone, tm, false);
	if (UnknownE0 == 0) {
		GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
		Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &Color[Current]);
		Position[Current].Set(point.X, point.Y, 0.0f);
	} else {
		GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
		Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &AmbientColor);
		AmbientPosition.Set(point.X, point.Y, 0.0f);
	}
}

// 0x648190
void UIStarbaseLightsClass::Unknown_648190()
{
	for (int i = 0; i < UnknownF4; i++) {
		Position[i] = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, i);
	}
	AmbientPosition = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, 1000);
}
