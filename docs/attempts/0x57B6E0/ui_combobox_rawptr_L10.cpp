// Earth & Beyond client (reconstruction): app\Client\ui_combobox.cpp. Notes at the end of the file.
#define __INTRFACE_MEMBERS	// intrface.h: InterfaceClass(LineDisplayClientClass *), the base of the second constructor
#include <string.h>
#include <string>
#include <vector>
#include "sclient.h"
#include "always.h"
#include "ww3d.h"
#include "rendobj.h"
#include "hanim.h"
#include "..\..\wwlib\ref_ptr.h"

#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)
#include "unknown_675120.h"	// the gadgets (before ui_combobox.h: its text gadget is declared only with them)
#include "gd_slider.h"
#include "assetmgr.h"
#include "matrix3d.h"
#include "fontdatabase.h"
#include "..\shared\g_assert.h"
#include <stdio.h>
#include "ui_combobox.h"

template<class T> inline T Unknown_Clamp(T val, T lo, T hi) { if (val < lo) return(lo); if (val > hi) return(hi); return(val); }	// 0x57BA10 (out of line for unsigned: 0x579DC0)  // NAME 1: neutral (D10): WWMath::Clamp_Int's form, unsigned (0x57A330's top row)
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
extern UIComboBoxClass *Unknown_BE5BC4;	// NAME 1: neutral (D10): the open combo box (0x57A330 sets it while the list is down)

// (the header's inline: the text field shows the selected item unless it shows a label)
inline void UIComboBoxClass::Show_Selected() { if (!Has_Label()) TextField->Set_Text(Get_Item_Text(Selected)); }

void Unknown_497240(RenderObjClass *robj, bool on);	// 0x497240 (ro_funcs.cpp, cdecl: no N-patches on the model's meshes)  // NAME 1 [robj, on]: neutral
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: what the helpers pass
bool Unknown_42EC60(int button, int arg);	// 0x42EC60 (input.cpp's, cdecl: a mouse button down)  // NAME 1 [button, arg]: neutral
void Unknown_42ECF0(float *x, float *y);	// 0x42ECF0 (input.cpp's, cdecl: the pointer's position, 0..1)  // NAME 1 [x, y]: neutral
// (what the client's RefCountPtr<HAnimClass> assignment from Create_Get comes to: the old animation released, the new one's reference kept)
inline void Set_Anim(HAnimClass *&anim, HAnimClass *get) { if (anim) anim->Release_Ref(); anim = get; }	// NAME 1: neutral (D10)
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: 0xBE67BC (fontdatabase.h's: the string files in use)

// 0x578120
UIComboBoxTextClass::UIComboBoxTextClass(UIMeshClass *mesh, const char *bone, LayerClass *layer, int width) :
	Unknown_675120Class((RenderObjClass *)NULL, *layer, 5),
	Hover(false),
	Selected(false),
	Layer(layer),
	Mesh(mesh),
	Width(width),
	HoverOnly(false)
{
	Bone = Mesh->Peek_Object()->Get_Bone_Index(bone);
	Init();
}

// 0x578210
UIComboBoxTextClass::UIComboBoxTextClass(UIMeshClass *mesh, const char *bone, int width) :
	Unknown_675120Class((RenderObjClass *)NULL, 5),
	Hover(false),
	Selected(false),
	Mesh(mesh),
	Width(width),
	HoverOnly(false)
{
	Bone = Mesh->Peek_Object()->Get_Bone_Index(bone);
	Init();
}

// 0x5782B0
UIComboBoxTextClass::~UIComboBoxTextClass()
{
	Mesh->Peek_Object()->Remove_Sub_Objects_From_Bone(Bone);
	Text->Release_Ref();
	Text = NULL;
	RObj = NULL;
}

// NOT EXACT: shape (3 rows: the client adds pos.Y onto the height left on the FPU stack, ours loads pos.Y and pops the height after)
// 0x578350
void UIComboBoxTextClass::Init()
{
	Text = new FontSys("Combobox_Default", NULL, NULL);
	RObj = Text;
	Unknown2C_0A = true;
	Mesh->Peek_Object()->Add_Sub_Object_To_Bone(Text, Bone);
	Unknown_497240(Mesh->Peek_Object(), false);
	Vector3 pos = Text->Get_Position();
	float height = Text->Get_Bounding_Box().Extent.Y;
	Text->Unknown4E4 = false;
	Text->Unknown_6D88F0((int)pos.X, (int)(height + pos.Y), (int)(Width + pos.X), (int)(pos.Y - height));
}

// 0x578490: the pointer (0..1 across the screen) in the 640 x 480 layout the text sits in
bool UIComboBoxTextClass::Unknown_30(float x, float y)
{
	if (RObj && !RObj->Is_Hidden() && RObj->Peek_Scene() && Unknown_60()) {
		Vector3 pos = Text->Get_Position();
		float height = Text->Get_Bounding_Box().Extent.Y * 0.9f;
		int px = (int)(x * 640.0f - 320.0f);
		int py = (int)-(y * 480.0f - 240.0f);
		if (px >= pos.X && px <= Width + pos.X && py >= pos.Y - height && py <= pos.Y + height) {
			Hover = true;
		} else {
			Hover = false;
		}
		if ((!Selected || HoverOnly) && !Hover && strcmp(Text->Unknown_6D87D0(), "-send to:-") != 0) {
			Text->Unknown_6CE910(false);
		} else {
			Text->Unknown_6CE910(true);
		}
		return(Hover);
	}
	return(false);
}

// 0x578670
void UIComboBoxTextClass::Set_Text(const char *text)
{
	Text->Change_Text(text, 0);
}

// 0x5786A0
void UIComboBoxTextClass::Set_Hover_Only(bool on)
{
	HoverOnly = on;
}

// NOT EXACT: inline-budget (the client calls assign for the label and stops at _Grow for the style; ours expands both further)
// 0x5786C0
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, LayerClass *layer, RenderObjClass *model, const char *name, int select_key, int toggle_key, int width, int max_rows, const char *style) :
	InterfaceClass(owner->Get_View(), (int)layer->Peek_Scene()),
	AttachBone(0),
	Model(model),
	Style(""),
	WidthAnim(NULL),
	DropAnim(NULL),
	Layer(layer),
	Width(width),
	MaxRows(max_rows),
	NumRows(0),
	Owner(owner),
	SelectKey(select_key),
	ToggleKey(toggle_key),
	State(0),
	RowsShown(0),
	Arrow(NULL),
	Slider(NULL),
	TopItem(0),
	Selected(0),
	Label(""),
	ArrowHidden(false)
{
	if (style && *style) {
		Style = style;
	}
	AttachBone = Model->Get_Bone_Index(name);
	Init(NULL);
	InterfaceClass::Unknown_58();
}

// NOT EXACT: inline-budget (the client expands the label's assign and the style's _Grow; ours calls them)
// 0x5789E0
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, SClient *client, RenderObjClass *model, const char *name, int select_key, int toggle_key, int width, int max_rows, const char *style) :
	InterfaceClass((LineDisplayClientClass *)client),
	AttachBone(0),
	Model(model),
	Style(""),
	WidthAnim(NULL),
	DropAnim(NULL),
	Width(width),
	MaxRows(max_rows),
	NumRows(0),
	Owner(owner),
	SelectKey(select_key),
	ToggleKey(toggle_key),
	State(0),
	RowsShown(0),
	Arrow(NULL),
	Slider(NULL),
	TopItem(0),
	Selected(0),
	Label(""),
	ArrowHidden(false)
{
	if (style && *style) {
		Style = style;
	}
	AttachBone = Model->Get_Bone_Index(name);
	Init(owner);
}

// NOT EXACT: source (the client's two animations are RefCountPtr<HAnimClass> members, destroyed after the body with their own EH states; raw pointers here, see ui_combobox.h and docs/attempts/0x57B6E0: that form is exact)
// 0x578D50
UIComboBoxClass::~UIComboBoxClass()
{
	InterfaceClass::Unknown_5C();
	Clear_Items();
	Model->Remove_Sub_Objects_From_Bone(AttachBone);
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), false, 0);
		Unknown64.Unknown_687150(Slider);
	}
	while (Rows.size()) {
		Unknown64.Unknown_687150(Rows.back());
		Rows.pop_back();
	}
	Unknown64.Unknown_687150(Arrow);
	Unknown64.Unknown_687150(TextField);
	REF_PTR_RELEASE(WidthAnim);
	REF_PTR_RELEASE(DropAnim);
	delete Anim;
	delete Mesh;
	Unknown_BE67BC->Unload("fc_combobox.ini");
}

// NOT EXACT: source (the client's two animations are RefCountPtr<HAnimClass>, raw pointers here: see ui_combobox.h; otherwise register-only: this in esi and the scale in ebp in the client; ours this in ebp, the scale in esi and spilled)
// 0x5790C0: owner NULL (the first constructor): the gadgets on Layer; else on an empty layer
void UIComboBoxClass::Init(InterfaceClass *owner)
{
	Anim = new HAnimComboClass(2);
	Unknown_BE67BC->Load("fc_combobox.ini");
	int scale = 1;
	if (Style.size() == 0) {
		Mesh = new UIMeshClass("Combo00", 1);
		Set_Anim(WidthAnim, WW3DAssetManager::Get_Instance()->Get_HAnim("Combo00.Combo00-1"));
		Set_Anim(DropAnim, WW3DAssetManager::Get_Instance()->Get_HAnim("Combo00.Combo00-2"));
	} else {
		Mesh = new UIMeshClass(Style.c_str(), 1);
		char anim_name[0x40];
		_snprintf(anim_name, 0x40, "%s.%s-1", Style.c_str(), Style.c_str());
		Set_Anim(WidthAnim, WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name));
		_snprintf(anim_name, 0x40, "%s.%s-2", Style.c_str(), Style.c_str());
		Set_Anim(DropAnim, WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name));
		if (Style == "Combo01") {
			scale = 4;
		}
	}
	Mesh->Peek_Object()->Set_Sort_Level(4);
	Anim->Set_Motion(0, WidthAnim);
	Anim->Set_Motion(1, DropAnim);
	Anim->Set_Weight(0, 1.0f);
	Anim->Set_Weight(1, 1.0f);
	Anim->Set_Frame(0, (float)(Width * scale));
	Mesh->Peek_Object()->Set_Animation(Anim);
	RenderObjClass *drop = Mesh->Unknown_5B8B40("DROP_DOWN00");
	drop->Set_Sort_Level(4);
	drop->Set_Hidden(1);
	drop->Release_Ref();
#line 310
	G_ASSERT(AttachBone);
	Model->Add_Sub_Object_To_Bone(Mesh->Peek_Object(), AttachBone);
	Matrix3D tm = Mesh->Unknown_5B9210("TEXTFIELD01");
	int right = (int)tm.Get_X_Translation();
	for (int i = 0; i < MaxRows; i++) {
		char bone[10];
		_snprintf(bone, 10, "TBONE%02d", i + 1);
		Matrix3D bone_tm = Mesh->Unknown_5B9210(bone);
		int left = (int)bone_tm.Get_X_Translation();
		UIComboBoxTextClass *row = NULL;
		if (owner) {
			row = new UIComboBoxTextClass(Mesh, bone, right - left);
		} else {
			row = new UIComboBoxTextClass(Mesh, bone, Layer, right - left);
		}
		row->Unknown_65FFA0(-3);
		row->Unknown_40(false);
		row->Unknown_8C(true);
		Unknown64.Unknown_6870E0(row, i + 3, View, true, false);
		Rows.push_back(row);
	}
	if (owner) {
		Arrow = new ButtonGadgetClass(Mesh, "ARROWBOX00", 5, 3);
	} else {
		Arrow = new ButtonGadgetClass(Mesh, "ARROWBOX00", *Layer, 5, 3);
	}
	Arrow->Unknown_65FFA0(-2);
	Unknown_496AE0(Arrow->RObj, false);
	Unknown64.Unknown_6870E0(Arrow, 1, View, true, false);
	Matrix3D left_tm = Mesh->Unknown_5B9210("TEXTFIELD00");
	int left = (int)left_tm.Get_X_Translation();
	tm = Mesh->Unknown_5B9210("TEXTFIELD02");
	right = (int)tm.Get_X_Translation();
	if (owner) {
		TextField = new UIComboBoxTextClass(Mesh, "TEXTFIELD00", right - left);
	} else {
		TextField = new UIComboBoxTextClass(Mesh, "TEXTFIELD00", Layer, right - left);
	}
	TextField->Unknown_65FFA0(-1);
	TextField->RObj->Set_Sort_Level(5);
	Unknown64.Unknown_6870E0(TextField, 1, View, true, false);
	if (owner) {
		Slider = new GadgetSlider(false, NULL);
	} else {
		Slider = new GadgetSlider(false, Layer);
	}
	Slider->Unknown_65FFA0(-2);
	Unknown64.Unknown_6870E0(Slider, 2, View, true, false);
	Selected = 0;
}

// 0x579D80
void UIComboBoxClass::Unknown_58()
{
	InterfaceClass::Unknown_58();
}

// 0x579DA0
void UIComboBoxClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
}

// NOT EXACT: inline-budget (the client calls Unknown_Clamp 0x57BA10 and the items' size() 0x47D640 in the expanded Scroll_To_Selected; ours expands them)
// 0x579DC0
void UIComboBoxClass::Unknown_60()
{
	Update_Drop_Down();
	if (ArrowHidden && State == 3) {
		if (!Is_Pointer_Over()) {
			if (Unknown_42EC60(1, 0) || Unknown_42EC60(2, 0)) {
				Toggle_Drop_Down();
			}
		}
	}
}

// 0x57A030
void UIComboBoxClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
}

// NOT EXACT: inline-budget (the client calls Scroll_To_Selected 0x57B2F0 and the items' size(); ours expands them)
// 0x57A060
bool UIComboBoxClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			Toggle_Drop_Down();
			break;
		case 2:
			TopItem = (int)Slider->Get_Value();
			Update_Rows();
			break;
		default:
			if (key >= 3) {
				Select(TopItem + key - 3);
				Toggle_Drop_Down();
			}
			break;
	}
	return(false);
}

// 0x57A310
void UIComboBoxClass::Unknown_57A310()
{
}

// 0x57AAB0
void UIComboBoxClass::Update_Rows()
{
	for (int i = 0; i < NumRows; i++) {
		if (i <= RowsShown && RowsShown != 0.0f && i + TopItem < Items.size()) {
			if (Rows[i]->Unknown5C_02) {
				Rows[i]->Unknown_40(true);
				Rows[i]->Unknown_8C(false);
			}
			Rows[i]->Text->Change_Text(Items[i + TopItem].c_str(), 0);
			if (i + TopItem == Selected) {
				Rows[i]->Selected = true;
			} else {
				Rows[i]->Selected = false;
			}
		} else {
			if (!(Rows[i]->Unknown5C_02)) {
				Rows[i]->Unknown_40(false);
				Rows[i]->Unknown_8C(true);
			}
			Rows[i]->Text->Change_Text(" ", 0);
		}
	}
}

// 0x57AC70
void UIComboBoxClass::Select(int index)
{
	if (index >= (int)Items.size()) {
		index = Items.size() - 1;
	}
	Selected = index;
	Show_Selected();
	Owner->Unknown_68(SelectKey);
}

// 0x57AD50
void UIComboBoxClass::Select_Text(const char *text)
{
	for (unsigned i = 0; i < Items.size(); i++) {
		if (strncmp(text, Items[i].c_str(), strlen(text)) == 0) {
			Selected = i;
			Show_Selected();
			return;
		}
	}
	Selected = 0;
	Show_Selected();
}

// 0x57A330
void UIComboBoxClass::Update_Drop_Down()
{
	if (State == 3 || State == 0) {
		return;
	}
	float seconds = WW3D::Get_Frame_Time() * 0.001f;
	float step = seconds * 50.0f;
	RenderObjClass *drop = Mesh->Unknown_5B8B40("DROP_DOWN00");
	switch (State) {
		case 2:
			RowsShown -= step;
			if (RowsShown < 0.0f) {
				RowsShown = 0;
				State = 0;
				drop->Set_Hidden(1);
				Rows[0]->Unknown_40(false);
				Rows[0]->Unknown_8C(true);
				Unknown_BE5BC4 = NULL;
				if ((int)Items.size() > NumRows) {
					Unknown_5C();
					Slider->Unknown_66E3A0();
					Slider->Unknown_40(false);
					Unknown_58();
				}
			}
			Update_Rows();
			Anim->Set_Frame(1, RowsShown);
			Mesh->Peek_Object()->Update_Sub_Object_Transforms();
			if ((int)Items.size() > NumRows && Slider->Unknown_60()) {
				Slider->Unknown_66FD80(false);
			}
			if (State == 0) {
				Owner->Unknown_68(ToggleKey);
			}
			break;
		case 1:
			if (drop->Is_Hidden()) {
				Unknown_BE5BC4 = this;
				Owner->Unknown_68(ToggleKey);
				Unknown_5C();
				drop->Set_Hidden(0);
				if ((int)Items.size() > NumRows) {
					Slider->Unknown_66E460(Unknown_497300(Mesh->Peek_Object()), false, 0);
					Slider->Unknown_66FA90((float)(Items.size() - NumRows + 1), 1.0f, true);
					Slider->Unknown_40(true);
					Slider->Unknown_66FD80(false);
					Slider->Unknown_A4((float)TopItem, 0);
				}
				if ((int)Items.size() > NumRows && Slider->Unknown_60()) {
					TopItem = min((unsigned)Selected, Items.size() - NumRows);
					TopItem = Unknown_Clamp(TopItem, 0u, Items.size() - NumRows);
				} else {
					TopItem = 0;
				}
				Unknown_58();
			}
			RowsShown += step;
			if (RowsShown >= (float)NumRows) {
				RowsShown = (float)NumRows;
				State = 3;
				Unknown_5C();
				if ((int)Items.size() > NumRows && Slider->Unknown_60()) {
					TopItem = min((unsigned)Selected, Items.size() - NumRows);
					TopItem = Unknown_Clamp(TopItem, 0u, Items.size() - NumRows);
				} else {
					TopItem = 0;
				}
				if ((int)Items.size() > NumRows) {
					Slider->Unknown_66E3A0();
					Slider->Unknown_66E460(Unknown_497300(Mesh->Peek_Object()), false, 0);
					Slider->Unknown_66FA90((float)(Items.size() - NumRows + 1), 1.0f, true);
					Slider->Unknown_40(true);
					Slider->Unknown_A4((float)TopItem, 0);
				}
				Unknown_58();
			}
			Update_Rows();
			Anim->Set_Frame(1, RowsShown);
			Mesh->Peek_Object()->Update_Sub_Object_Transforms();
			if ((int)Items.size() > NumRows && Slider->Unknown_60()) {
				Slider->Unknown_66FD80(false);
			}
			break;
	}
	drop->Release_Ref();
}

// 0x57AE70
void UIComboBoxClass::Toggle_Drop_Down()
{
	if (Unknown_BE5BC4 && Unknown_BE5BC4 != this) {
		Unknown_BE5BC4->Close_Drop_Down();
		Unknown_BE5BC4 = NULL;
	}
	switch (State) {
		case 0:
			if (Items.size() == 0) {
				return;
			}
			Scroll_To_Selected();
			Update_Rows();
		case 2:
			State = 1;
			break;
		case 1:
		case 3:
			State = 2;
			break;
	}
}

// NOT EXACT: inline-budget (the client calls the items' insert; with the whole file in the object ours expands it)
// 0x57B000
void UIComboBoxClass::Add_Item(const char *text)
{
	NumRows++;
	NumRows = min(MaxRows, NumRows);
	Items.insert(Items.end(), 1, std::string(text));
	if (Items.size() == 1) {
		Selected = 0;
		Show_Selected();
	}
}

// 0x57B1D0
void UIComboBoxClass::Add_Items(const char **items)
{
	while (*items) {
		Add_Item(*items++);
	}
}

// 0x57B210
void UIComboBoxClass::Clear_Items()
{
	Items.clear();
	TopItem = 0;
	Selected = 0;
	Update_Rows();
	NumRows = 0;
}

// 0x57B2F0
void UIComboBoxClass::Scroll_To_Selected()
{
	if ((int)Items.size() > NumRows && Slider->Unknown_60()) {
		TopItem = min((unsigned)Selected, Items.size() - NumRows);
		TopItem = Unknown_Clamp(TopItem, 0u, Items.size() - NumRows);
	} else {
		TopItem = 0;
	}
}

// 0x57B3C0
void UIComboBoxClass::Enable(bool on)
{
	if (Arrow) {
		Arrow->Unknown_40(on);
	}
	if (TextField) {
		TextField->Unknown_40(on);
	}
	Close_Drop_Down();
}

// 0x57B440
void UIComboBoxClass::Close_Drop_Down()
{
	if (State == 1 || State == 3 || State == 2) {
		State = 2;
		RowsShown = -1.0f;
		Update_Drop_Down();
	}
}

// 0x57B480
void UIComboBoxClass::Set_Label(const char *text)
{
	Label = text;
	TextField->Set_Text(Label.c_str());
	GadgetClass::Set_Tooltip(TextField, "UI_COMBO_BOX", text);
}

// 0x57B550
bool UIComboBoxClass::Has_Label()
{
	return(Label.size() ? true : false);
}

// 0x57B570
void UIComboBoxClass::Hide_Arrow(bool on)
{
	ArrowHidden = on;
	Arrow->Unknown_40(!on);
	Arrow->Unknown_8C(on);
}

// 0x57B5C0
void UIComboBoxClass::Set_Hover_Only(int first, int last)
{
	first = (first > 0) ? first : 0;
	int end = (last < NumRows) ? last : NumRows;
	for (int i = first; i < end; i++) {
		Rows[i]->HoverOnly = true;
	}
}

// 0x57B610
bool UIComboBoxClass::Is_Pointer_Over()
{
	float x, y;
	Unknown_42ECF0(&x, &y);
	if (TextField->Unknown_30(x, y)) {
		return(true);
	}
	if (State == 3) {
		for (int i = 0; i < NumRows; i++) {
			if (Rows[i]->Unknown_30(x, y)) {
				return(true);
			}
		}
	}
	return(false);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_combobox.cpp (its __FILE__, 0x5790C0's assert 'AttachBone' on line 310)
**
**	The combo box: so far the drop-down's frame 0x57A330, 0x57AAB0, 0x57AC70 and 0x57AD50 (TG4, claim #61). The file's evidence:
**	the three sit after the assert's 0x5790C0 and before ui_cpit.cpp's first function (0x57D6C0); 0x57AAB0
**	is called by 0x578D50, 0x579DC0, 0x57A060, 0x57A330, 0x57AE70 and 0x57B210 (all in the stretch), and
**	0x57A330 uses the literal 'DROP_DOWN00' at 0xB7E45C with 0x5790C0. 0x57AC70 / 0x57AD50 are the
**	box's public selection (ui_chat_panel.cpp, 0x5F3770 and others call them). The rows are the gadget
**	subclass 0x578210 builds (table 0xAF097C). 0x57A330's top-row clamps: always.h's min, then an upper
**	clamp in WWMath::Clamp_Int's form on unsigned (std::_cpp_min and a ternary both lose the client's ja).
**	The rest of the object (L10, 2026-11-11), 0x578120-0x57BAC0: the text gadget UIComboBoxTextClass (table 0xAF097C:
**	the constructors 0x578120 / 0x578210, which Init 0x5790C0 expands in its news; slot +0x30 0x578490) before the
**	box's constructors 0x5786C0 / 0x5789E0, destructor 0x578D50, Init, the slots +0x58..+0x7C and the public members;
**	then the templates (Create_Get<HAnimClass> 0x57B6B0, the rows' vector 0x57B710-0x57BA40, Unknown_Clamp<unsigned>
**	0x57BA10) and the two base tables' adjustor thunks 0x57BAB0 / 0x57BAC0. The functions without a caller in the
**	client (0x578670, 0x5786A0, 0x57A310, 0x57B550, 0x57B610) are members /Ob2 expanded at every call.
**	The item list and drop-down helpers 0x57B000, 0x57B1D0, 0x57B210, 0x57B440, 0x57B570, 0x57B5C0 (CW1, 2026-11-08):
**	0x57B000 caps the rows +0xB8 at +0xB4 and shows the first item; 0x57B210 empties the list.
*/
