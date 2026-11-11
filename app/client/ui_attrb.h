// Earth & Beyond client (reconstruction): app\client\ui_attrb.h, the character info window's attributes and skill panels (the client's
// ui_attrb.cpp per its strings, 'e:\build\beta\G\app\Client\ui_attrb.cpp'; this header's name is provisional, NAME 1).
// Laid out from the constructors 0x546A80 / 0x5482B0 and the destructors 0x546B50 / 0x548440.
#ifndef UI_ATTRB_H  // NAME 1 [ui_attrb.h]: provisional header name for ui_attrb.cpp
#define UI_ATTRB_H

#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"

class RenderObjClass;
class AvatarPipClass;		// avatar_pip.h's (+0xA8)
class UIExperienceClass;	// ui_experience.h's experience bars (+0x8C)
class FontSys;
template<class T> class DeltaTemplateClass;

// The attributes panel (0xB0 bytes, tables 0xAEF26C / 0xAEF260 / 0xAEF254; ui_attrb_skills.cpp's 0x549AA0 makes it with
// the tab's mesh and keeps it at +0xC0): a 'TSI12A' mesh on the tab mesh's bone 'CHAR_ATTRIB', the strings of
// 'fa_att.ini', the avatar's picture, name, class, levels, rank, credits and experience debt
class Unknown546A80Class : public InterfaceClass	// NAME 1: neutral (D10), as ui_attrb_skills.cpp's
{
public:
	Unknown546A80Class(SClient *client, RefCountPtr<RenderObjClass> &mesh);	// 0x546A80 (the client to 0x540D30)  // NAME 1 [client, mesh]: what 0x549AA0 passes
	virtual ~Unknown546A80Class();						// 0x546B50 (deleting 0x546B20)
	virtual void Unknown_54();							// +0x54 0x546DD0 (the mesh, the experience display, the picture, the styles)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5472C0 (the base's)  // NAME 1 [view, arg]: InterfaceClass's
	virtual void Unknown_58();							// +0x58 0x5472F0 (onto the tab mesh, every text set)
	virtual void Unknown_5C();							// +0x5C 0x547370
	void Unknown_5473B0();								// 0x5473B0 (the picture's avatar unless +0xAC; UIStarbaseClass::Unknown_60 calls it)
	virtual void Unknown_60();							// +0x60 0x5473F0 (every frame: the picture, the changed texts)
	virtual bool Unknown_68(int key);					// +0x68 0x547550 (false)  // NAME 1 [key]: InterfaceClass's
	void Unknown_547570();								// 0x547570 (the name and the class; G_ASSERT 'avatar' line 237)
	int Unknown_547730(DeltaTemplateClass<int> *field, const char *style);	// 0x547730 (the field's value, or -1, into the style; 0x5477C0 inlines it)  // NAME 1 [field, style]: neutral
	void Unknown_5477C0(bool all);						// 0x5477C0 (the three levels and their sum; then the experience display)  // NAME 1 [all]: neutral (true: every set field, false: the changed ones)
	void Unknown_547C20(bool all);						// 0x547C20 ('Credits: %s')  // NAME 1 [all]: neutral
	void Unknown_547E40(bool all);						// 0x547E40 (the rank; G_ASSERT 'avatar' line 382)  // NAME 1 [all]: neutral
	void Unknown_547FD0(bool all);						// 0x547FD0 ('XP Debt: %d')  // NAME 1 [all]: neutral

	RefCountPtr<RenderObjClass> Unknown84;				// +0x84 (the tab's mesh: the constructor's)
	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0x88 ('TSI12A')  // NAME 1: as pdaint.h's
	UIExperienceClass *Experience;						// +0x8C (0x58C450, 0x28 bytes)  // NAME 1: as pdaint.h's
	RefCountPtr<FontSys> Unknown90;						// +0x90 'CHARACTER_NAME_ATT'
	RefCountPtr<FontSys> Unknown94;						// +0x94 'CHARACTER_LEVEL_ATT'
	RefCountPtr<FontSys> Unknown98;						// +0x98 'CHARACTER_CLASS_NAME_ATT'
	RefCountPtr<FontSys> Unknown9C;						// +0x9C 'CHARACTER_RANK_ATT'
	RefCountPtr<FontSys> UnknownA0;						// +0xA0 'CHARACTER_CREDITS_ATT'
	RefCountPtr<FontSys> UnknownA4;						// +0xA4 'EXP_DEBT_ATT'
	AvatarPipClass *AvatarPip;							// +0xA8 (0x3C bytes; the 'PICTURE' sub-object)  // NAME 1: its class's
	bool UnknownAC;										// +0xAC (true from Unknown_58 until Unknown_60 sets the picture up)
};

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
	void Unknown_548D60(class SkillDataClass *list, int skill, int index);	// 0x548D60: the selected skill's texts (ui_attrb_skills.cpp calls it)  // NAME 1 [list, skill, index]: as ui_attrb_skills.cpp's
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
