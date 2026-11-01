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

// fontmanager.h's text object, as far as this file calls it (fontmanager.h clashes with tsk_game.h through sclient.h)
class FontStyleClass	// NAME 1 [FontStyleClass]: fontmanager.h's
{
public:
	void Change_Text(const char *text, int unknown);	// 0x6CDBC0: the client's profile label 'FontSys::Change_Text'  // NAME 3: the client's label
};

// gadget.cpp's gadget, as far as this file calls it (the list's rows, the text field)
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C(); virtual void Unknown_50();
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual bool Unknown_60();							// +0x60 (the slider: 0x57A330 asks it before the top row follows the selection)
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral
	virtual void Unknown_90(); virtual void Unknown_94(); virtual void Unknown_98(); virtual void Unknown_9C(); virtual void Unknown_A0();
	virtual void Unknown_A4(float value, int arg);		// +0xA4 (the slider's position: 0x57A330 passes the top row)  // NAME 1 [value, arg]: neutral

	char Unknown04[0x5C - 0x04];						// +0x04
	unsigned char Unknown5C;							// +0x5C (bit 4: the row is off)
	char Unknown5D[0x61 - 0x5D];						// +0x5D
	bool Unknown61;										// +0x61 (a row: the selected one; the row's constructor 0x578210 clears it)
	char Unknown62[0x68 - 0x62];						// +0x62
	FontStyleClass *Unknown68;							// +0x68 (the text)
};

// gd_slider.cpp's slider (the list's scroll bar, the combo box's +0xD8), as far as this file calls it
class UIComboBoxSliderClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_66E3A0();								// 0x66E3A0
	void Unknown_66E460(const RefCountPtr<RenderObjClass> &object, int a, int b);	// 0x66E460 (0x57A330: the mesh's model, 0, 0)  // NAME 1 [object, a, b]: neutral
	void Unknown_66FA90(float range, float step, bool flag);	// 0x66FA90 (0x57A330: the items past the rows plus one, 1.0, true)  // NAME 1 [range, step, flag]: neutral
	void Unknown_66FD80(int arg);						// 0x66FD80 (0x57A330 passes 0)  // NAME 1 [arg]: neutral
};

template<class T> inline T Unknown_Clamp(T val, T lo, T hi) { if (val < lo) return(lo); if (val > hi) return(hi); return(val); }	// NAME 1: neutral (D10): WWMath::Clamp_Int's form, unsigned (0x57A330's top row)
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
class UIComboBoxClass;
extern UIComboBoxClass *Unknown_BE5BC4;	// NAME 1: neutral (D10): the open combo box (0x57A330 sets it while the list is down)

// What the combo box's +0xBC points to, as far as this file calls it
class UIComboBoxUnknownBCClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64();
	virtual void Unknown_68(int arg);					// +0x68 (0x57AC70 passes the combo box's +0xC0)  // NAME 1 [arg]: neutral
};

// The combo box (ui_combobox.cpp's __FILE__), as far as this file's functions so far use it; ui_chat_panel.h's
// UIChatComboClass is the same class (its Unknown_57AD50)
class UIComboBoxClass : public InterfaceClass	// NAME 3: class-from-file (D24)
{
public:
	void Unknown_57A330();								// 0x57A330 (the drop-down's opening / closing per frame)
	void Unknown_57AAB0();								// 0x57AAB0 (the rows from the top item)
	void Unknown_57AC70(int index);						// 0x57AC70 (select by index)  // NAME 1 [index]: neutral
	void Unknown_57AD50(const char *text);				// 0x57AD50 (select by text: the first item it begins)  // NAME 1 [text]: neutral

	const char *Unknown_Item_Text(unsigned index) { return(index < Items.size() ? Items[index].c_str() : ""); }	// (inline: 0x57AC70, 0x57AD50)  // NAME 1: neutral (D10)
	void Unknown_Show_Selected() { if (!Unknown10C) Unknown0D4->Unknown68->Change_Text(Unknown_Item_Text(Selected), 0); }	// (inline: 0x57AC70, 0x57AD50)  // NAME 1: neutral (D10)

	UIMeshClass *Unknown084;							// +0x84 (the box's mesh)
	char Unknown088[0xA0 - 0x88];						// +0x88
	HAnimComboClass *Unknown0A0;						// +0xA0 (the drop-down's animation)
	char Unknown0A4[0xB8 - 0xA4];						// +0xA4
	int Unknown0B8;										// +0xB8 (the rows)
	UIComboBoxUnknownBCClass *Unknown0BC;				// +0xBC
	int Unknown0C0;										// +0xC0
	int Unknown0C4;										// +0xC4
	int Unknown0C8;										// +0xC8 (the drop-down: 0 up, 1 opening, 2 closing, 3 down)
	float Unknown0CC;									// +0xCC (the rows shown)
	char Unknown0D0[0xD4 - 0xD0];						// +0xD0
	GadgetClass *Unknown0D4;							// +0xD4 (the text field)
	UIComboBoxSliderClass *Unknown0D8;					// +0xD8
	unsigned int Unknown0DC;							// +0xDC (the top item)
	int Selected;										// +0xE0  // NAME 2: what 0x57AC70 / 0x57AD50 set
	std::vector<std::string> Items;						// +0xE4  // NAME 2: what the rows show
	char Unknown0F4[0xF8 - 0xF4];						// +0xF4
	GadgetClass **Unknown0F8;							// +0xF8 (the rows)
	char Unknown0FC[0x10C - 0xFC];						// +0xFC
	volatile int Unknown10C;							// +0x10C (set: the text field is left alone; volatile: the client reads it after the store to Selected, 0x57AC70 / 0x57AD50)
};

// 0x57AAB0
void UIComboBoxClass::Unknown_57AAB0()
{
	for (int i = 0; i < Unknown0B8; i++) {
		if (i <= Unknown0CC && Unknown0CC != 0.0f && i + Unknown0DC < Items.size()) {
			if (Unknown0F8[i]->Unknown5C & 4) {
				Unknown0F8[i]->Unknown_40(true);
				Unknown0F8[i]->Unknown_8C(false);
			}
			Unknown0F8[i]->Unknown68->Change_Text(Items[i + Unknown0DC].c_str(), 0);
			if (i + Unknown0DC == Selected) {
				Unknown0F8[i]->Unknown61 = true;
			} else {
				Unknown0F8[i]->Unknown61 = false;
			}
		} else {
			if (!(Unknown0F8[i]->Unknown5C & 4)) {
				Unknown0F8[i]->Unknown_40(false);
				Unknown0F8[i]->Unknown_8C(true);
			}
			Unknown0F8[i]->Unknown68->Change_Text(" ", 0);
		}
	}
}

// 0x57AC70
void UIComboBoxClass::Unknown_57AC70(int index)
{
	if (index >= (int)Items.size()) {
		index = Items.size() - 1;
	}
	Selected = index;
	Unknown_Show_Selected();
	Unknown0BC->Unknown_68(Unknown0C0);
}

// 0x57AD50
void UIComboBoxClass::Unknown_57AD50(const char *text)
{
	for (unsigned i = 0; i < Items.size(); i++) {
		if (strncmp(text, Items[i].c_str(), strlen(text)) == 0) {
			Selected = i;
			Unknown_Show_Selected();
			return;
		}
	}
	Selected = 0;
	Unknown_Show_Selected();
}

// 0x57A330
void UIComboBoxClass::Unknown_57A330()
{
	if (Unknown0C8 == 3 || Unknown0C8 == 0) {
		return;
	}
	float seconds = WW3D::Get_Frame_Time() * 0.001f;
	float step = seconds * 50.0f;
	RenderObjClass *drop = Unknown084->Unknown_5B8B40("DROP_DOWN00");
	switch (Unknown0C8) {
		case 2:
			Unknown0CC -= step;
			if (Unknown0CC < 0.0f) {
				Unknown0CC = 0;
				Unknown0C8 = 0;
				drop->Set_Hidden(1);
				Unknown0F8[0]->Unknown_40(false);
				Unknown0F8[0]->Unknown_8C(true);
				Unknown_BE5BC4 = NULL;
				if ((int)Items.size() > Unknown0B8) {
					Unknown_5C();
					Unknown0D8->Unknown_66E3A0();
					Unknown0D8->Unknown_40(false);
					Unknown_58();
				}
			}
			Unknown_57AAB0();
			Unknown0A0->Set_Frame(1, Unknown0CC);
			Unknown084->Peek_Object()->Update_Sub_Object_Transforms();
			if ((int)Items.size() > Unknown0B8 && Unknown0D8->Unknown_60()) {
				Unknown0D8->Unknown_66FD80(0);
			}
			if (Unknown0C8 == 0) {
				Unknown0BC->Unknown_68(Unknown0C4);
			}
			break;
		case 1:
			if (drop->Is_Hidden()) {
				Unknown_BE5BC4 = this;
				Unknown0BC->Unknown_68(Unknown0C4);
				Unknown_5C();
				drop->Set_Hidden(0);
				if ((int)Items.size() > Unknown0B8) {
					Unknown0D8->Unknown_66E460(Unknown_497300(Unknown084->Peek_Object()), 0, 0);
					Unknown0D8->Unknown_66FA90((float)(Items.size() - Unknown0B8 + 1), 1.0f, true);
					Unknown0D8->Unknown_40(true);
					Unknown0D8->Unknown_66FD80(0);
					Unknown0D8->Unknown_A4((float)Unknown0DC, 0);
				}
				if ((int)Items.size() > Unknown0B8 && Unknown0D8->Unknown_60()) {
					Unknown0DC = min((unsigned)Selected, Items.size() - Unknown0B8);
					Unknown0DC = Unknown_Clamp(Unknown0DC, 0u, Items.size() - Unknown0B8);
				} else {
					Unknown0DC = 0;
				}
				Unknown_58();
			}
			Unknown0CC += step;
			if (Unknown0CC >= (float)Unknown0B8) {
				Unknown0CC = (float)Unknown0B8;
				Unknown0C8 = 3;
				Unknown_5C();
				if ((int)Items.size() > Unknown0B8 && Unknown0D8->Unknown_60()) {
					Unknown0DC = min((unsigned)Selected, Items.size() - Unknown0B8);
					Unknown0DC = Unknown_Clamp(Unknown0DC, 0u, Items.size() - Unknown0B8);
				} else {
					Unknown0DC = 0;
				}
				if ((int)Items.size() > Unknown0B8) {
					Unknown0D8->Unknown_66E3A0();
					Unknown0D8->Unknown_66E460(Unknown_497300(Unknown084->Peek_Object()), 0, 0);
					Unknown0D8->Unknown_66FA90((float)(Items.size() - Unknown0B8 + 1), 1.0f, true);
					Unknown0D8->Unknown_40(true);
					Unknown0D8->Unknown_A4((float)Unknown0DC, 0);
				}
				Unknown_58();
			}
			Unknown_57AAB0();
			Unknown0A0->Set_Frame(1, Unknown0CC);
			Unknown084->Peek_Object()->Update_Sub_Object_Transforms();
			if ((int)Items.size() > Unknown0B8 && Unknown0D8->Unknown_60()) {
				Unknown0D8->Unknown_66FD80(0);
			}
			break;
	}
	drop->Release_Ref();
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
*/
