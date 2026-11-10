// Earth & Beyond client (reconstruction): app\client\ui_pda_cargo.h, the PDA's cargo window (the client's ui_pda_cargo.cpp
// per its asserts, 'e:\build\beta\G\app\Client\ui_pda_cargo.cpp' at 0xB839B4; this header's name is provisional, NAME 1,
// D137). Laid out from the constructor 0x5CF9B0, the destructor 0x5CFCE0 and the set-up 0x5CFE40 (PW1, 2026-11-09).
#ifndef UI_PDA_CARGO_H  // NAME 1 [ui_pda_cargo.h]: provisional header name for ui_pda_cargo.cpp
#define UI_PDA_CARGO_H

#include <string>
#include "ui_inv.h"		// UIInvClass, ui_inv.cpp's inventory window base
#include "..\..\wwlib\ref_ptr.h"

class SClient;
class RenderObjClass;
class ButtonGadgetClass;		// unknown_675120.h's buttons
class UIComboBoxClass;			// ui_combobox.h's combo box (the filter)
class Unknown669FB0Class;		// gd_inv_slider.h's inventory slider
class PDAInterfaceClass;		// pdaint.h's PDA

// The cargo window (0x10C bytes, tables 0xAF4AD4 / 0xAF4AC8 / 0xAF4ABC; pdaint.cpp's set-up 0x693230 makes it: the key
// 'Cargo', the PDA's window 0): the 'TSI04A' mesh, twelve slots 'TSI04A.C%02d', five buttons, the filter combo box and
// the slider; the sort and the filter in the options 'Inv_Sort_Cargo', 'Inv_Sort_Cargo_Reverse', 'Inv_Filter_Cargo'
class UIPdaCargoClass : public UIInvClass	// NAME 3: class-from-file (D24) for app\Client\ui_pda_cargo.cpp
{
public:
	enum { NUM_BUTTONS = 5 };							// NAME 1: the set-up's loop over 0xB83820 (gadgets 13..17)
	UIPdaCargoClass(SClient *client, PDAInterfaceClass *pda);	// 0x5CF9B0 (the kind 10; deleting 0x5CFCB0)  // NAME 1 [client, pda]: what the PDA passes
	virtual ~UIPdaCargoClass();							// 0x5CFCE0
	virtual void Unknown_54();							// +0x54 0x5CFE40 (FAIL_IFs '!InterfaceMesh' l.118, '!first_bttn' l.129; G_ASSERT 'font' l.167)
	virtual void Unknown_58();							// +0x58 0x5D03E0 (FAIL_IFs '!InterfaceMesh' l.184, '!Get_Client()' l.185, '!sc' l.187)
	virtual void Unknown_5C();							// +0x5C 0x5D05E0 (the options saved, the combo's, the base's)
	virtual void Unknown_60();							// +0x60 0x5D09B0 (every frame: the credits when changed, the combo's, the base's)
	virtual bool Unknown_68(int key);					// +0x68 0x5D0620 (the buttons 13..17, the filter 20 / 21)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5D09E0 (the base's and the combo's onto a new view)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5D0A20(bool on);						// 0x5D0A20 (0x695DC0: the base's +0xB8 bit 0)  // NAME 1 [on]: neutral
	void Unknown_5D0A50(bool all);						// 0x5D0A50 (the 'Credits' text; all: whatever changed)  // NAME 1 [all]: as ui_attrb.cpp's 0x547C20
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: the FAIL_IF '!Get_Client()')  // NAME 3: the FAIL_IF's
	void Unknown_5D0D20(bool reverse);					// 0x5D0D20 (the next sort, or the order reversed: 'Inv_Sort_%d_Name', the INVENTORY_SORT packet)  // NAME 2 [reverse]: what it toggles

	PDAInterfaceClass *PDA;								// +0xD0 (the constructor's)  // NAME 2: what it is
	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0xD4 ('TSI04A'; FAIL_IF '!InterfaceMesh')  // NAME 3: the FAIL_IF's
	ButtonGadgetClass *Buttons5[NUM_BUTTONS];			// +0xD8 ('BUTTON01' vault, 'BUTTON02' equipment, 'BUTTON03' sort / reverse, 'EXIT')  // NAME 1: neutral (the base has Buttons)
	RefCountPtr<UIComboBoxClass> Filter;				// +0xEC ('filter', 'Combo01'; the items 0xB83834)  // NAME 2: its sub-object's name
	Unknown669FB0Class *Slider;							// +0xF0 (gadget 19)  // NAME 2: its type's file
	int SortIndex;										// +0xF4 (0..2: 'Inv_Sort_%d_Name'; the option 'Inv_Sort_Cargo')  // NAME 2: what it holds
	bool SortReverse;									// +0xF8 (the option 'Inv_Sort_Cargo_Reverse')  // NAME 2: the option's
	std::string FilterName;								// +0xFC (the option 'Inv_Filter_Cargo')  // NAME 2: the option's
};

#endif // UI_PDA_CARGO_H
