// Earth & Beyond client (reconstruction): app\client\ui_vault.h, the PDA's vault window (the client's ui_vault.cpp per
// its asserts, 'e:\build\beta\G\app\Client\ui_vault.cpp' at 0xB8C3B0; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x656FF0, the destructor 0x6572F0 and the set-up 0x657430 (O15, 2026-11-13).
#ifndef UI_VAULT_H  // NAME 1 [ui_vault.h]: provisional header name for ui_vault.cpp
#define UI_VAULT_H

#include <string>
#include "ui_inv.h"		// UIInvClass, ui_inv.cpp's inventory window base
#include "..\..\wwlib\ref_ptr.h"

class SClient;
class RenderObjClass;
class ButtonGadgetClass;		// unknown_675120.h's buttons
class UIComboBoxClass;			// ui_combobox.h's combo box (the filter)

// The vault window (0xFC bytes, tables 0xAF92F0 (53 slots) / 0xAF92E4 / 0xAF92D8; pdaint.cpp's set-up 0x693230 makes it:
// the key 'Vault', the PDA's window 3): the 'VAULT00' mesh, 36 slots 'VAULT00.C%02d', three buttons, the filter combo
// box and a slider; the sort and the filter in the options 'Inv_Sort_Vault', 'Inv_Sort_Vault_Reverse', 'Inv_Filter_Vault'
class UIVaultClass : public UIInvClass	// NAME 3: class-from-file (D24) for app\Client\ui_vault.cpp
{
public:
	enum { NUM_BUTTONS = 3 };							// NAME 1: the set-up's loop over 0xB8C2EC (gadgets 37..39)
	UIVaultClass(SClient *client);						// 0x656FF0 (the kind 17; deleting 0x6572C0)  // NAME 1 [client]: what the PDA passes
	virtual ~UIVaultClass();							// 0x6572F0
	virtual void Unknown_54();							// +0x54 0x657430 (FAIL_IFs '!InterfaceMesh' l.115, '!first_bttn' l.126)
	virtual void Unknown_58();							// +0x58 0x657970 (FAIL_IF '!InterfaceMesh' l.176)
	virtual void Unknown_5C();							// +0x5C 0x6579F0 (the options saved, the combo's, the base's)
	virtual void Unknown_60();							// +0x60 0x657A30 (every frame: the combo's, the base's)
	virtual bool Unknown_68(int key);					// +0x68 0x657AA0 (the buttons 37..39, the filter 42 / 43)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x657A60 (the base's and the combo's onto a new view)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_657DA0(bool reverse);					// 0x657DA0 (the next sort, or the order reversed: 'Inv_Sort_%d_Name', the INVENTORY_SORT packet)  // NAME 2 [reverse]: what it toggles

	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0xD0 ('VAULT00'; FAIL_IF '!InterfaceMesh')  // NAME 3: the FAIL_IF's
	ButtonGadgetClass *Buttons3[NUM_BUTTONS];			// +0xD4 ('EXIT', 'BUTTON01' sort, 'BUTTON01' reverse)  // NAME 1: neutral (the base has Buttons)
	RefCountPtr<UIComboBoxClass> Filter;				// +0xE0 ('filter', 'Combo01'; the items 0xB8C2F8)  // NAME 2: its sub-object's name
	int SortIndex;										// +0xE4 (0..2: 'Inv_Sort_%d_Name'; the option 'Inv_Sort_Vault')  // NAME 2: what it holds
	bool SortReverse;									// +0xE8 (the option 'Inv_Sort_Vault_Reverse')  // NAME 2: the option's
	std::string FilterName;								// +0xEC (the option 'Inv_Filter_Vault')  // NAME 2: the option's
};

#endif // UI_VAULT_H
