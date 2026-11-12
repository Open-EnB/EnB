// Earth & Beyond client (reconstruction): app\client\ui_combobox.h, the combo box (the client's ui_combobox.cpp per
// its __FILE__; this header's name is provisional). Moved out of ui_combobox.cpp with ui_chat_panel.h's stand-in
// UIChatComboClass, the same class (its Select_Text; P-TG4-1, 2026-11-03). Two classes (tables 0xAF097C and
// 0xAF0A58 with its two base tables 0xAF0A4C / 0xAF0A40): the text gadget of a row and of the text field, and the box.
#ifndef UI_COMBOBOX_H  // NAME 3 [ui_combobox.h]: header name by convention (D24) from the client's ui_combobox.cpp
#define UI_COMBOBOX_H

#include <string>
#include <vector>
#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"

class ButtonGadgetClass;		// unknown_675120.h's button
class FontSys;					// fontmanager.h
class GadgetSlider;				// gd_slider.h's slider
class HAnimClass;				// hanim.h
class HAnimComboClass;			// hanim.h
class LayerClass;				// layer.h
class UIComboBoxClass;
class UIComboBoxTextClass;

// The text gadget (0x7C bytes, table 0xAF097C): a line of text on a bone of the box's mesh. The box's text field
// and each row of its list. Declared where the includer has the gadgets (unknown_675120.h before this header:
// ui_chat_panel.cpp and cpacket.cpp still have their own views of GadgetClass, so this header cannot include it).
#ifdef UNKNOWN_675120_H
class UIComboBoxTextClass : public Unknown_675120Class	// NAME 1: neutral (D10): what the box makes of it
{
public:
	UIComboBoxTextClass(UIMeshClass *mesh, const char *bone, LayerClass *layer, int width);	// 0x578120  // NAME 1 [mesh, bone, layer, width]: the members they set
	UIComboBoxTextClass(UIMeshClass *mesh, const char *bone, int width);	// 0x578210 (an empty layer)  // NAME 1 [mesh, bone, width]: the members they set
	virtual ~UIComboBoxTextClass();						// +0x00 0x5782B0 (deleting 0x5781E0)
	virtual bool Unknown_30(float x, float y);			// +0x30 0x578490 (the pointer within Width of the bone, 0.9 of the text's height above and below: Hover and the text's highlight)

	void Init();										// 0x578350 (the text 'Combobox_Default' onto the bone)  // NAME 2: describes what it does
	void Set_Text(const char *text);					// 0x578670  // NAME 2: describes what it does; NAME 1 [text]: neutral
	void Set_Hover_Only(bool on);						// 0x5786A0  // NAME 1: the member it sets; NAME 1 [on]: neutral

	bool Hover;											// +0x60 (slot +0x30: the pointer is on it)  // NAME 2: what slot +0x30 sets
	bool Selected;										// +0x61 (a row: the selected item's; highlighted)  // NAME 2: what Update_Rows sets
	LayerClass *Layer;									// +0x64 (the first constructor's)  // NAME 1: its type
	FontSys *Text;										// +0x68 (0x4E8 bytes, 'Combobox_Default'; the model +0x30 too)  // NAME 2: what it shows
	UIMeshClass *Mesh;									// +0x6C (the box's mesh)  // NAME 2: what Init reads the bone from
	int Width;											// +0x70 (the row's width: slot +0x30)  // NAME 2: what slot +0x30 compares x with
	int Bone;											// +0x74 (the mesh's bone the text sits on, captured)  // NAME 2: the bone index it holds
	bool HoverOnly;										// +0x78 (a row: highlighted only under the pointer, not as the selected one)  // NAME 2: what slot +0x30 does with it
};
#endif

// The combo box (0x118 bytes, table 0xAF0A58): an interface of its own on its owner's view
class UIComboBoxClass : public InterfaceClass	// NAME 3: class-from-file (D24)
{
public:
	UIComboBoxClass(InterfaceClass *owner, LayerClass *layer, RenderObjClass *model, const char *name, int select_key, int toggle_key, int width, int max_rows, const char *style);	// 0x5786C0 (ui_registration.cpp's 0x5F2F90)  // NAME 1 [owner, layer, model, name, select_key, toggle_key, width, max_rows, style]: the members they set
	UIComboBoxClass(InterfaceClass *owner, SClient *client, RenderObjClass *model, const char *name, int select_key, int toggle_key, int width, int max_rows, const char *style);	// 0x5789E0 (ui_pda_cargo.cpp's filter: 'filter', 0x15, 0x14, 0x19, 0xC, 'Combo01')  // NAME 1 [owner, client, model, name, select_key, toggle_key, width, max_rows, style]: the members they set
	virtual ~UIComboBoxClass();							// +0x00 0x578D50 (deleting 0x5789B0)
	virtual void Unknown_58();							// +0x58 0x579D80 (the base's)
	virtual void Unknown_5C();							// +0x5C 0x579DA0 (the base's)
	virtual void Unknown_60();							// +0x60 0x579DC0 (every frame: the drop-down, and a click outside the box when the arrow is hidden)
	virtual bool Unknown_68(int key);					// +0x68 0x57A060 (1 the arrow, 2 the slider, 3.. a row)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x57A030 (the base's)  // NAME 1 [view, arg]: InterfaceClass's

	void Init(InterfaceClass *owner);					// 0x5790C0 (the mesh, the animations, the rows, the arrow, the text field, the slider; owner NULL: the gadgets on Layer)  // NAME 2: describes what it does; NAME 1 [owner]: what the second constructor passes
	void Unknown_57A310();								// 0x57A310 (empty, no caller)
	void Update_Drop_Down();							// 0x57A330 (the drop-down's opening / closing per frame)  // NAME 2: describes what it does
	void Update_Rows();									// 0x57AAB0 (the rows' text from the top item)  // NAME 2: describes what it does
	void Select(int index);								// 0x57AC70 (select by index)  // NAME 2: describes what it does; NAME 1 [index]: neutral
	void Select_Text(const char *text);					// 0x57AD50 (select by text: the first item it begins)  // NAME 2: describes what it does; NAME 1 [text]: neutral
	void Toggle_Drop_Down();							// 0x57AE70 (any other open box closed; up: opens, down: closes)  // NAME 2: describes what it does
	void Add_Item(const char *text);					// 0x57B000 (ui_registration.cpp's fillers)  // NAME 2: describes what it does; NAME 1 [text]: neutral
	void Add_Items(const char **items);					// 0x57B1D0 (ui_registration.cpp: a table up to its NULL)  // NAME 2: describes what it does; NAME 1 [items]: neutral
	void Clear_Items();									// 0x57B210 (ui_registration.cpp 0x5F9460: the rows emptied before a refill)  // NAME 2: describes what it does
	void Scroll_To_Selected();							// 0x57B2F0 (the top item from the selection when the slider is on, else 0)  // NAME 2: describes what it does
	void Enable(bool on);								// 0x57B3C0 (ui_registration.cpp 0x5FB9F0: the arrow's and the text field's slot +0x40, the drop-down closed)  // NAME 2: describes what it does; NAME 1 [on]: neutral
	void Close_Drop_Down();								// 0x57B440 (ui_chat_panel.cpp, before each combo's slot +0x7C)  // NAME 2: describes what it does
	void Set_Label(const char *text);					// 0x57B480 (ui_pda_cargo.cpp passes 'Filter': the text field shows it instead of the selected item)  // NAME 2: describes what it does; NAME 1 [text]: neutral
	bool Has_Label();									// 0x57B550 (no caller)  // NAME 2: describes what it does
	void Hide_Arrow(bool on);							// 0x57B570 (0x5689D0): ArrowHidden and the arrow button's +0x40 / +0x8C  // NAME 2: describes what it does; NAME 1 [on]: neutral
	void Set_Hover_Only(int first, int last);			// 0x57B5C0 (0x5689D0: 0 and the item count): the rows first..last-1  // NAME 1: the rows' member it sets; NAME 1 [first, last]: neutral
	bool Is_Pointer_Over();								// 0x57B610 (the text field or, when down, a row)  // NAME 2: describes what it does

	const char *Get_Item_Text(unsigned index) { return(index < Items.size() ? Items[index].c_str() : ""); }	// (inline: 0x57AC70, 0x57AD50)  // NAME 2: describes what it does
	void Show_Selected();	// (inline: 0x57AC70, 0x57AD50; ui_combobox.cpp defines it: it needs the text gadget)  // NAME 2: describes what it does

	UIMeshClass *Mesh;									// +0x84 ('Combo00', or the style's)  // NAME 2: its type and use
	int AttachBone;										// +0x88 (the model's bone the mesh sits on: G_ASSERT 'AttachBone' l.310)  // NAME 3: the assert's
	RenderObjClass *Model;								// +0x8C (the owner's model)  // NAME 1: what the constructors are given
	std::string Style;									// +0x90 ('Combo01': the mesh's and the animations' name; empty: 'Combo00')  // NAME 1: neutral
	HAnimComboClass *Anim;								// +0xA0 (the two animations: 0 the width, 1 the drop-down's rows)  // NAME 2: its type and use
	HAnimClass *WidthAnim;								// +0xA4 ('<style>.<style>_1': its frame Width, four times that for 'Combo01'; RefCountPtr<HAnimClass> in the client, as DropAnim: raw until 0x57B6E0 is decided, docs/attempts/0x57B6E0)  // NAME 2: what Init sets its frame from
	HAnimClass *DropAnim;								// +0xA8 ('<style>.<style>_2': its frame RowsShown)  // NAME 2: what Update_Drop_Down sets its frame from
	LayerClass *Layer;									// +0xAC (the first constructor's; the second leaves it)  // NAME 1: its type
	int Width;											// +0xB0 (the width animation's frame)  // NAME 2: what Init does with it
	int MaxRows;										// +0xB4 (the row gadgets; Add_Item caps NumRows at it)  // NAME 2: what Init and Add_Item do with it
	int NumRows;										// +0xB8 (the rows in use)  // NAME 2: what Add_Item counts
	InterfaceClass *Owner;								// +0xBC (its slot +0x68 gets SelectKey / ToggleKey)  // NAME 1: what the constructors are given
	int SelectKey;										// +0xC0 (to the owner when the selection changes)  // NAME 2: when the owner gets it
	int ToggleKey;										// +0xC4 (to the owner when the drop-down opens and when it has closed)  // NAME 2: when the owner gets it
	int State;											// +0xC8 (the drop-down: 0 up, 1 opening, 2 closing, 3 down)  // NAME 2: what Update_Drop_Down switches on
	float RowsShown;									// +0xCC (the rows shown: the drop-down animation's frame)  // NAME 2: what Update_Drop_Down animates
	ButtonGadgetClass *Arrow;							// +0xD0 (the arrow button, 'ARROWBOX00': key 1)  // NAME 2: its sub-object's name
	UIComboBoxTextClass *TextField;						// +0xD4 (the text field, 'TEXTFIELD00': key 1)  // NAME 2: its bone's name
	GadgetSlider *Slider;								// +0xD8 (the list's scroll bar: key 2)  // NAME 2: its type
	unsigned int TopItem;								// +0xDC (the item in the first row)  // NAME 2: what Update_Rows reads
	int Selected;										// +0xE0  // NAME 2: what Select / Select_Text set
	std::vector<std::string> Items;						// +0xE4  // NAME 2: what the rows show
	std::vector<UIComboBoxTextClass *> Rows;			// +0xF4 (MaxRows of them, 'TBONE_%02d': keys 3..)  // NAME 2: what they are
	std::string Label;									// +0x104 (Set_Label; not empty: the text field is left alone)  // NAME 2: what Set_Label shows
	bool ArrowHidden;									// +0x114 (Hide_Arrow; slot +0x60 then closes on a click elsewhere)  // NAME 2: what Hide_Arrow sets
};

#endif
