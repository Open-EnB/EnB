// Earth & Beyond client (reconstruction): app\client\ui_attrb.h, the character info window's skill panel (the client's
// ui_attrb.cpp per its strings, 'e:\build\beta\G\app\Client\ui_attrb.cpp'; this header's name is provisional, NAME 1).
// Laid out from the constructor 0x5482B0 and the destructor 0x548440.
#ifndef UI_ATTRB_H  // NAME 1 [ui_attrb.h]: provisional header name for ui_attrb.cpp
#define UI_ATTRB_H

#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"

class ButtonGadgetClass;	// unknown_675120.h's (+0xB4, +0xB8)
class ManagedFontClass;		// fontmanager.h's (+0xBC)
class FontSys;				// fontmanager.h's (+0xC0..+0xD8)

// The skill panel (0xDC bytes, tables 0xAEF3B0 / 0xAEF3A4 / 0xAEF398; ui_attrb_skills.cpp's 0x549AA0 makes it and
// keeps it at +0xFC): a 'TSI12C' mesh, the strings of 'fa_tsi12c.ini', the buttons 'EXIT' (gadget 1) and 'UPGRADE' (2)
// and a 0x674260 gadget (3)
class Unknown548D60Class : public Unknown5BB320Class	// NAME 1: neutral (D10), as ui_attrb_skills.cpp's
{
public:
	Unknown548D60Class(SClient *client);				// 0x5482B0 (0xDC bytes)  // NAME 1 [client]: as UIBioClass's
	virtual ~Unknown548D60Class();						// 0x548440 (deleting 0x548410)
	virtual void Unknown_54();							// 0x548610 (the buttons, the styles, the gadget)
	virtual bool Unknown_68(int key);					// 0x548C50 (key 1 closes, key 2 sends SkillPacketClass)  // NAME 1 [key]: InterfaceClass's
	void Unknown_548D60(class Unknown6A7A00Class *list, int skill, int index);	// 0x548D60: the selected skill's texts (ui_attrb_skills.cpp calls it)  // NAME 1 [list, skill, index]: as ui_attrb_skills.cpp's
	void Unknown_5494E0(bool on);						// 0x5494E0 ('UPGRADE' on, its style shown and the requirement hidden; or the other way)  // NAME 1 [on]: neutral

	int UnknownAC;										// +0xAC (the skill SkillPacketClass carries)
	int UnknownB0;										// +0xB0
	ButtonGadgetClass *UnknownB4;						// +0xB4 'UPGRADE' (gadget 2)
	ButtonGadgetClass *UnknownB8;						// +0xB8 'EXIT' (gadget 1)
	ManagedFontClass *UnknownBC;						// +0xBC (the mesh's font)
	RefCountPtr<FontSys> UnknownC0;						// +0xC0 'LVL_ATT'
	RefCountPtr<FontSys> UnknownC4;						// +0xC4 'NOT_ACQUIRED_ATT'
	RefCountPtr<FontSys> UnknownC8;						// +0xC8 'NEXT_LVL_ATT'
	RefCountPtr<FontSys> UnknownCC;						// +0xCC 'UPGRADE_BUTTON_ATT'
	RefCountPtr<FontSys> UnknownD0;						// +0xD0 'POINTS_AVAILABLE_ATT'
	RefCountPtr<FontSys> UnknownD4;						// +0xD4 'UPGRADE_REQUIRES_ATT'
	RefCountPtr<FontSys> UnknownD8;						// +0xD8 'UPGRADE_COSTS_ATT'
};

#endif // UI_ATTRB_H
