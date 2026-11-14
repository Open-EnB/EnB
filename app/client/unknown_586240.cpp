// Earth & Beyond client (reconstruction): app\client\unknown_586240.cpp, the space lights panel ('light01a').
// No string names the file (no asserts): the name is an interim one (D137). Notes at the end.
#include <string.h>
#include "unknown_586240.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "unknown_671300.h"
#include "local_interface_sort.h"
#include "control_keys_manager.h"
#include "..\shared\orbitalmanager.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "aabox.h"
#include "matrix3d.h"
#include "vector4.h"
#include "wwmath.h"

void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color);	// 0x496C00 (ro_funcs.cpp, cdecl): the model's texture colour at (u, v)  // NAME 1 [all]: as ui_shipcust.cpp's
void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (the pointer, 0..1)  // NAME 1 [x, y]: as ui_avatar_cust.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_avatar_cust.cpp's

// unknown_585920.cpp's pull: a gadget with its pull model (0x10 bytes)
class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950
	void Unknown_5859A0();								// 0x5859A0 (the pull model onto the gadget's box centre)

	Unknown_671300Class *Gadget;						// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('PULL_COLOR')  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08 (dragging)
	float Unknown0C;									// +0x0C (0.5)
};

// 0x586240
UISpaceLightsClass::UISpaceLightsClass(SClient *client) :
	ChatPanelUIBase(client),
	Pull(NULL),
	Current(-1)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Slider, 0, sizeof(Slider));
	memset(SliderValue, 0, sizeof(SliderValue));
}

// 0x586310
UISpaceLightsClass::~UISpaceLightsClass()
{
	delete Pull;
	Pull = NULL;
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// The buttons Unknown_54 makes: their models' names and their texts (0xB7EB9C / 0xB7EBBC)
static const char *Unknown_B7EB9C[7] = { "exit", "save", "light00", "light01", "light02", "light03", "light04" };	// NAME 1: neutral (D10)
static const char *Unknown_B7EBBC[7] = { NULL, "save_light01a", "key_light01a", "rev_key_light01a", "camera_light01a", "rev_camera_light01a", "ambient_light01a" };	// NAME 1: neutral (D10)

// 0x5863D0
void UISpaceLightsClass::Unknown_54()
{
	if (Unknown_98("light01a", "fa_light01a.ini")) {
		Unknown_5868D0();
		int i;
		for (i = 0; i < 7; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B7EB9C[i], Unknown_B7EBBC[i]);
		}
		for (i = 0; i < 4; i++) {
			Slider[i] = Add_Slider(i + 9, 0);
			Slider[i]->Unknown_66E460(InterfaceMesh, true, i);
			if (i == 3) {
				Slider[i]->Unknown_66FA90(4.0f, 0.01f, true);
				Slider[3]->Unknown_A4(0.5f, false);
			} else {
				Slider[i]->Unknown_66FA90(1.9f, 0.1f, true);
				Slider[i]->Unknown_A4(0.95f, false);
			}
		}
		Pull = new UIAvatrPullClass;
		RefCountPtr<RenderObjClass> palette = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("COLOR_PALETTE"));
		Unknown_496AE0(palette.Peek(), false);
		Pull->Gadget = new Unknown_671300Class(palette.Peek(), 0, 0, 0, 0);
		Pull->Gadget->Unknown2C_0A = true;
		Pull->Pull = InterfaceMesh->Get_Sub_Object_By_Name("PULL_COLOR");
		Unknown_496AE0(Pull->Pull, true);
		Pull->Unknown_5859A0();
		Pull->Gadget->Unknown_65FFA0(-1);
		Unknown64.Unknown_6870E0(Pull->Gadget, 0xE, View, true, true);
		Add_Back_Gadget(1, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	}
}

// 0x586720
void UISpaceLightsClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass scope(this);
		ChatPanelUIBase::Unknown_58();
		Unknown_586940();
		Unknown_586D90(3);
	}
}

// 0x5867B0
void UISpaceLightsClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5867D0
void UISpaceLightsClass::Unknown_60()
{
	Unknown_586B90();
	Unknown_586F60();
}

// 0x586800
bool UISpaceLightsClass::Unknown_68(int key)
{
	((ClientClass *)Client)->As_Space_Client();
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (key) {
		case 1:
			Unknown_586AA0();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			Unknown_586A70();
			return(true);
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			Unknown_586D90(key);
			return(true);
	}
	return(false);
}

// 0x5868D0
void UISpaceLightsClass::Unknown_5868D0()
{
	for (int i = 0; i < 5; i++) {
		Intensity[i] = 0.5f;
		Color[i].X = 0.5f;
		Color[i].Y = 0.5f;
		Color[i].Z = 0.5f;
		Position[i].X = 0.5f;
		Position[i].Y = 0.5f;
		Position[i].Z = 0.5f;
		Offset[i].X = 0.95f;
		Offset[i].Y = 0.95f;
		Offset[i].Z = 0.95f;
	}
}

// 0x586940
void UISpaceLightsClass::Unknown_586940()
{
	MainViewClass *main_view = (MainViewClass *)View->As_Main_View();
	if (main_view && main_view->Orbitals->Unknown_4F37C0(NULL)) {
		for (int i = 0; i < 5; i++) {
			Vector4 light = main_view->Orbitals->Unknown_4F50D0(i);
			Color[i].X = light.X;
			Color[i].Y = light.Y;
			Color[i].Z = light.Z;
			Intensity[i] = light.W;
			Position[i] = main_view->Orbitals->Unknown_4F51A0(i);
			Offset[i] = main_view->Orbitals->Unknown_4F5140(i);
			Offset[i].X += 0.95f;
			Offset[i].Y += 0.95f;
			Offset[i].Z += 0.95f;
		}
	}
}

// 0x586A70
void UISpaceLightsClass::Unknown_586A70()
{
	MainViewClass *main_view = (MainViewClass *)View->As_Main_View();
	if (main_view) {
		main_view->Orbitals->Unknown_4F3EF0();
	}
}

// 0x586AA0
void UISpaceLightsClass::Unknown_586AA0()
{
	Unknown_5868D0();
	Unknown_586940();
	MainViewClass *main_view = (MainViewClass *)View->As_Main_View();
	if (main_view) {
		for (int i = 0; i < 5; i++) {
			main_view->Orbitals->Unknown_4F5100(i, Vector4(Color[i].X, Color[i].Y, Color[i].Z, Intensity[i]));
			main_view->Orbitals->Unknown_4F51C0(i, Position[i]);
			main_view->Orbitals->Unknown_4F5160(i, Vector3(Offset[i].X - 0.95f, Offset[i].Y - 0.95f, Offset[i].Z - 0.95f));
		}
	}
}

// 0x586B90
void UISpaceLightsClass::Unknown_586B90()
{
	bool changed = false;
	int i;
	for (i = 0; i < 4; i++) {
		Slider[i]->Unknown_66FD80();
		if (Slider[i]->Value != SliderValue[i]) {
			changed = true;
		}
	}
	MainViewClass *main_view = (MainViewClass *)View->As_Main_View();
	if (changed && main_view) {
		Offset[Current].X = Slider[0]->Get_Value();
		Offset[Current].Y = Slider[1]->Get_Value();
		Offset[Current].Z = Slider[2]->Get_Value();
		Intensity[Current] = Slider[3]->Get_Value();
		int index = Current;
		Vector4 light(Color[Current].X, Color[Current].Y, Color[Current].Z, Intensity[Current]);
		main_view->Orbitals->Unknown_4F5100(index, light);
		Vector3 offset(Offset[Current].X - 0.95f, Offset[Current].Y - 0.95f, Offset[Current].Z - 0.95f);
		main_view->Orbitals->Unknown_4F5160(index, offset);
		for (i = 0; i < 4; i++) {
			SliderValue[i] = Slider[i]->Get_Value();
		}
	}
}

// 0x586D90
void UISpaceLightsClass::Unknown_586D90(int key)
{
	int index = key - 3;
	if (Current != index) {
		Current = index;
		Slider[0]->Unknown_A4(Offset[Current].X, false);
		Slider[1]->Unknown_A4(Offset[Current].Y, false);
		Slider[2]->Unknown_A4(Offset[Current].Z, false);
		if (Current == 4) {
			Slider[3]->Unknown_66FA90(1.0f, 0.01f, true);
		} else {
			Slider[3]->Unknown_66FA90(4.0f, 0.01f, true);
		}
		Slider[3]->Unknown_A4(Intensity[Current], false);
		int i;
		for (i = 0; i < 4; i++) {
			SliderValue[i] = Slider[i]->Get_Value();
		}
		Unknown_587310(Position[Current]);
		for (i = 0; i < 5; i++) {
			Buttons[i + 2]->Unknown_6607D0();
			Buttons[i + 2]->Unknown2C_0B = false;
		}
		Buttons[Current + 2]->Unknown_6603A0(false, NULL);
		Buttons[Current + 2]->Unknown2C_0B = true;
	}
}

// NOT EXACT: order (the address of Color[Current] after 1.0f - v in the client), D89; the pseudocode-faithful form
// 0x586F60
void UISpaceLightsClass::Unknown_586F60()
{
	if (Pull->Gadget->Unknown64) {
		Pull->Unknown08 = true;
	}
	if ((Pull->Gadget->Unknown64 || Pull->Gadget->Unknown65) && Pull->Unknown08) {
		AABoxClass box;
		Pull->Gadget->Unknown_64(box);
		float y;
		float x;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		x = WWMath::Min(x, box.Center.X + box.Extent.X);
		x = WWMath::Max(x, box.Center.X - box.Extent.X);
		y = WWMath::Min(y, box.Center.Y + box.Extent.Y);
		y = WWMath::Max(y, box.Center.Y - box.Extent.Y);
		int bone = InterfaceMesh->Get_Sub_Object_Bone_Index(Pull->Pull);
		InterfaceMesh->Capture_Bone(bone);
		Matrix3D tm = Pull->Pull->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
		tm[2][3] = 0.0f;
		InterfaceMesh->Control_Bone(bone, tm, false);
		Vector2 point;
		point.X = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		point.Y = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		RObjGadgetClass *area = Pull->Gadget->As_RObjGadget();
		Unknown_496C00(area->RObj, point.X, 1.0f - point.Y, &Color[Current]);
		Position[Current].Set(point.X, point.Y, 0.0f);
		MainViewClass *main_view = (MainViewClass *)View->As_Main_View();
		if (main_view) {
			int index = Current;
			Vector4 light(Color[Current].X, Color[Current].Y, Color[Current].Z, Intensity[Current]);
			main_view->Orbitals->Unknown_4F5100(index, light);
			main_view->Orbitals->Unknown_4F51C0(index, Position[Current]);
		}
	} else {
		Pull->Unknown08 = false;
	}
	Pull->Gadget->Unknown_671630();
}

// 0x587310
void UISpaceLightsClass::Unknown_587310(Vector3 point)
{
	AABoxClass box;
	Pull->Gadget->Unknown_64(box);
	float top = box.Center.Y - box.Extent.Y;
	float left = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f;
	float height = box.Extent.Y * 2.0f;
	int bone = InterfaceMesh->Get_Sub_Object_Bone_Index(Pull->Pull);
	InterfaceMesh->Capture_Bone(bone);
	Matrix3D tm = Pull->Pull->Get_Transform();
	tm.Set_X_Translation((point.X * width) + left - box.Center.X);
	tm.Set_Y_Translation((point.Y * height) + top - box.Center.Y);
	tm[2][3] = 0.0f;
	InterfaceMesh->Control_Bone(bone, tm, false);
	RObjGadgetClass *area = Pull->Gadget->As_RObjGadget();
	Unknown_496C00(area->RObj, point.X, 1.0f - point.Y, &Color[Current]);
	Position[Current].Set(point.X, point.Y, 0.0f);
}

/*
**	Reconstruction notes (unknown_586240.cpp, client 0x586240-0x587530)
**
**	The object: the constructor 0x586240 opens it (unknown_585920.cpp's pull helpers and 0x5860B0 before it carry no
**	object boundary of their own: interim placements, D137); its guarded <string> id initialiser 0x5874F0 and the two
**	this-adjusting deleting-destructor thunks 0x587520 / 0x587530 close it; 0x587540 is ViewUnknown34MessageStruct's
**	inline constructor, the message box's object. No string names the file and no caller of the constructor exists
**	(a debug panel, as ui_starbase.cpp's lights panel 0x646170, whose code this one shares almost line for line).
**
**	Its data: the buttons' model names 0xB7EB9C and texts 0xB7EBBC; the five lights are OrbitalManager's [LIGHTS]
**	KEY, RKEY, CAMERA, RCAMERA, AMBIENT (the texts 'key_light01a' .. 'ambient_light01a'), read and written through
**	the main view's OrbitalManager (+0x140; OrbitalManager 0x4F37C0 / 0x4F50D0 / 0x4F5100 / 0x4F5140 / 0x4F5160 /
**	0x4F51A0 / 0x4F51C0, 'save': 0x4F3EF0 writes light_sector%04d.ini). The offsets are kept 0.95 up so the sliders
**	(0 .. 1.9) can go below zero.
**
**	Forms that matter: 0x586940 copies the Vector4 into a local and assigns its members one by one; 0x586B90 holds
**	the light's index in a local for both OrbitalManager calls, with named Vector4 / Vector3 locals (temporaries
**	share one slot, the client's frame has both); 0x586D90's slider range is an if / else (a ternary makes a float
**	temporary). 0x586F60 (NOT EXACT, an order-only residue: the address of Color[Current] is computed after
**	1.0f - v in the client) clamps with WWMath::Min / Max, y declared before x.
*/
