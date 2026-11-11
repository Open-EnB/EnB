// Earth & Beyond client (reconstruction): app\Client\ui_combobox.cpp. Notes at the end of the file.
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
#include "ui_combobox.h"

template<class T> inline T Unknown_Clamp(T val, T lo, T hi) { if (val < lo) return(lo); if (val > hi) return(hi); return(val); }	// 0x57BA10 (out of line for unsigned: 0x579DC0)  // NAME 1: neutral (D10): WWMath::Clamp_Int's form, unsigned (0x57A330's top row)
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
extern UIComboBoxClass *Unknown_BE5BC4;	// NAME 1: neutral (D10): the open combo box (0x57A330 sets it while the list is down)

// (the header's inline: the text field shows the selected item unless +0x10C says leave it)
inline void UIComboBoxClass::Show_Selected() { if (Label.size() == 0) TextField->Text->Change_Text(Get_Item_Text(Selected), 0); }

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

// NOT EXACT: inline-budget (the client expands the item's string constructor, _Tidy and _Grow, where ours calls assign)
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

// 0x57B440
void UIComboBoxClass::Close_Drop_Down()
{
	if (State == 1 || State == 3 || State == 2) {
		State = 2;
		RowsShown = -1.0f;
		Update_Drop_Down();
	}
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
**	The item list and drop-down helpers 0x57B000, 0x57B1D0, 0x57B210, 0x57B440, 0x57B570, 0x57B5C0 (CW1, 2026-11-08):
**	0x57B000 caps the rows +0xB8 at +0xB4 and shows the first item; 0x57B210 empties the list.
*/
