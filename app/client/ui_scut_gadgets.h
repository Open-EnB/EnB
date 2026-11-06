// Earth & Beyond client (reconstruction): app\client\ui_scut_gadgets.h. The header's name is provisional (NAME 2: the client
// has only ui_scut_gadgets.cpp's __FILE__ 0xB88EBC). The class is laid out from its constructor 0x6136D0 and destructor 0x613A20.
#ifndef UI_SCUT_GADGETS_H
#define UI_SCUT_GADGETS_H

#include <string>
#include <vector>
#include <list>
#include "..\..\wwlib\ref_ptr.h"

class UIScutClass;
class RObjGadgetClass;
class GadgetClass;
class ClientClass;
class SClient;

// What Create_Shortcut's second argument points to: 0x66CB20 hands a new gadget on to its +0x78 (UIScutBarClass::Unknown_611BD0)
class Unknown66CB20Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_66CB20(GadgetClass *gadget);			// 0x66CB20  // NAME 1 [gadget]: what 0x614360 passes
};

// One saved shortcut (0x1C bytes, the list node's 0x24): the sections of shortcut.ini (%d_Key, %d_Bar, %d_Group, %d_Button)
struct ShortcutDataStruct	// NAME 2: what it holds
{
	ShortcutDataStruct(const std::string &key, int bar, int group, int button) : Key(key), Bar(bar), Group(group), Button(button) {}	// NAME 2 [key, bar, group, button]: the entries'

	std::string Key;									// +0x00 (%d_Key; +0x04 the text)  // NAME 2: the entry's
	int Bar;											// +0x10 (%d_Bar; FAIL_IF 'bar < 0')  // NAME 3: the FAIL_IF's
	int Group;											// +0x14 (%d_Group; FAIL_IF 'group < 0')  // NAME 3: the FAIL_IF's
	int Button;											// +0x18 (%d_Button; FAIL_IF 'button < 0')  // NAME 3: the FAIL_IF's
};

// The gadgets on the shortcut bars and their saving in shortcut.ini (0x48 bytes, table 0xAF75DC, abstract: ui_slot_scut.cpp's
// 0x630490 derives from it). Slots +0x0C, +0x10, +0x14 are the base's _purecall (0x9CCD10).
class UIScutGadgetsClass	// NAME 3: class-from-file (D24) for ui_scut_gadgets.cpp
{
public:
	enum {
		STATE_NO_UI,		// NAME 2: Get_State's first return
		STATE_NOT_RESTORED,	// (G_ASSERT 'Get_State() != STATE_NOT_RESTORED' l.135)  // NAME 3: the FAIL_IF's
		STATE_RESTORED		// NAME 2: Get_State's other return
	};

	UIScutGadgetsClass(const std::string &name, SClient *sclient);	// 0x6136D0 (FAIL_IF '!sclient' l.65)  // NAME 3 [sclient]: the FAIL_IF's; NAME 2 [name]: Section's last part
	virtual ~UIScutGadgetsClass();						// +0x00 0x613A20 (deleting 0x6139F0)
	virtual void Set_UI(const RefCountPtr<UIScutClass> &ui);	// +0x04 0x613BA0: the old bars saved and emptied, the new one kept  // NAME 2: what it does
	virtual void Clear_UI();							// +0x08 0x613D70 (no direct caller)  // NAME 2: what it does
	virtual RObjGadgetClass *Unknown_0C(int arg) = 0;	// +0x0C (0x614360: makes the gadget)  // NAME 1 [arg]: neutral
	virtual bool Unknown_10(RObjGadgetClass *gadget, std::string &key) = 0;	// +0x10 (0x614FB0: the gadget's %d_Key)  // NAME 2 [gadget, key]: what it fills
	virtual RObjGadgetClass *Unknown_14(const char *key) = 0;	// +0x14 (0x614040: the gadget of a %d_Key)  // NAME 2 [key]: what it is given
	virtual int Unknown_18() { return(0); }				// +0x18 0x6139D0 (0x614360: the bar for a new gadget)

	int Get_State();									// 0x613F50 (the G_ASSERT's)  // NAME 3: the FAIL_IF's
	bool Restore();										// 0x613F80 (FAIL_IF l.135)  // NAME 2: what it does
	bool Restore(const std::list<ShortcutDataStruct> &list);	// 0x614040 (G_ASSERTs 'added' l.160, '!restored' l.166)  // NAME 2: what it does; NAME 1 [list]: neutral
	RObjGadgetClass *Create_Shortcut(int arg, Unknown66CB20Class *target);	// 0x614360 (FAIL_IF '!ShortcutUI' l.187; 'shortcut_created_sn_1.wav')  // NAME 2: what it does; NAME 1 [arg, target]: neutral
	bool Delete_Shortcut(int scut);						// 0x614660 (FAIL_IFs l.235, l.238)  // NAME 2: what it does; NAME 3 [scut]: the FAIL_IF's
	void Delete_All();									// 0x614730 (no direct caller)  // NAME 2: what it does
	void Delete_Gadgets();								// (inline in the .cpp: a budget layer, see the notes; 0x613BA0 calls 0x614660 out of line through it)  // NAME 1: neutral
	void Clean();										// 0x614840 (FAIL_IF l.304; no direct caller): the gadgets whose slot +0x88 says gone, unless held  // NAME 2: what it does
	void Load(std::list<ShortcutDataStruct> &list);		// 0x614980 (FAIL_IFs l.334-342)  // NAME 2: what it does; NAME 1 [list]: neutral
	void Save(const std::list<ShortcutDataStruct> &list);	// 0x614D90  // NAME 2: what it does; NAME 1 [list]: neutral
	void Build(std::list<ShortcutDataStruct> &list);	// 0x614FB0 (FAIL_IFs '!gadget' l.408, '!found' l.411): the bars into the list  // NAME 2: what it does; NAME 1 [list]: neutral
	void Save_Current();								// 0x6152C0  // NAME 2: what it does
	void Init_Name(ClientClass *client);				// 0x6153B0 (FAIL_IF '!client' l.434; no direct caller)  // NAME 2: what it does; NAME 3 [client]: the FAIL_IF's

	RefCountPtr<UIScutClass> ShortcutUI;				// +0x04 (FAIL_IF '!ShortcutUI' l.187)  // NAME 3: the FAIL_IF's
	std::vector<RObjGadgetClass *> ShortcutGadgets;		// +0x08 (FAIL_IF '(unsigned)scut >= ShortcutGadgets.size()' l.235)  // NAME 3: the FAIL_IF's
	std::string Section;								// +0x18: <the player's id>_<name>  // NAME 2: what it is in shortcut.ini
	std::string Name;									// +0x28 (shortcut.ini's 'Name')  // NAME 2: the entry's
	std::list<ShortcutDataStruct> Saved;				// +0x38  // NAME 2: what it holds
	bool ShortcutBeenCreated;							// +0x44 (shortcut.ini's 'ShortcutBeenCreated')  // NAME 2: the entry's
};

#endif // UI_SCUT_GADGETS_H
