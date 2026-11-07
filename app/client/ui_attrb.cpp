// Earth & Beyond client (reconstruction): app\Client\ui_attrb.cpp, the character info window's skill panel. Notes at the
// end of the file.
#include <stdio.h>
#include <string.h>
#include <string>
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "..\packets\pkskill.h"
#include "..\audio\soundlib.h"
#include "ui_attrb.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x5495F0)

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

#include "gd_uiback.h"		// UIBackGadget (0x674260)

// What 0x6A5640 returns for a skill (0x548D60 reads it), as far as this file reads it
struct SkillLevelAvailabilityStruct  // NAME 1: neutral (D10)
{
	int Unknown00;										// +0x00 (3: no further level)
	union {
		unsigned int Unknown04;							// +0x04 (0: the upgrade allowed)
		struct {
			unsigned int Unknown04_00 : 1;				// bit 0: 'Upgrade requires level %d'
			unsigned int Unknown04_01 : 1;				// bit 1: 'Insufficient skill points'
			unsigned int Unknown04_02 : 1;				// bit 2
			unsigned int Unknown04_03 : 1;				// bit 3: 'Requires combat level %d'
			unsigned int Unknown04_04 : 1;				// bit 4: 'Requires trade level %d'
			unsigned int Unknown04_05 : 1;				// bit 5: 'Requires explore level %d'
			unsigned int Unknown04_06 : 1;				// bit 6: 'Maximum Level Attained'
			unsigned int Unknown04_07 : 1;				// bit 7: '%s Only.'
		};
	};
	bool Is_Bit00() const { return(Unknown04_00); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	bool Is_Bit01() const { return(Unknown04_01); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	bool Is_Bit03() const { return(Unknown04_03); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	bool Is_Bit04() const { return(Unknown04_04); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	bool Is_Bit05() const { return(Unknown04_05); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	bool Is_Bit06() const { return(Unknown04_06); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	bool Is_Bit07() const { return(Unknown04_07); }			// NAME 1: neutral (D10): a bool accessor (the client's word load, shr, test 1)
	int Unknown08;										// +0x08: the level the bits ask for
	int Unknown0C;										// +0x0C: the upgrade's cost in skill points
};

// The skill base's +0x50 (ui_attrb_skills.cpp's name), as far as this file calls it
class SkillDataPeekClass  // NAME 1: neutral (D10)
{
public:
	SkillLevelAvailabilityStruct &Get_Availability(int skill);		// 0x6A5640  // NAME 1 [skill]: neutral
	int Get_Level(int skill);						// 0x6A54B0: the skill's level  // NAME 1 [skill]: neutral
	int Get_Quest_Only_Levels(int skill);						// 0x6A5970: its highest level  // NAME 1 [skill]: neutral
	std::string Get_Affiliations(int skill);				// 0x6A5730: who may train it ('%s Only.')  // NAME 1 [skill]: neutral
};
// What ui_attrb_skills.cpp passes (its name), as far as this file calls it
class SkillDataClass  // NAME 1: neutral (D10)
{
public:
	int Get_Skill_Points();								// 0x6A47E0: the skill points available
	char Unknown00[0x50];								// +0x00
	SkillDataPeekClass Unknown50;						// +0x50
};

// skillnam.cpp's, as far as this file calls them (cdecl)
class SkillNamesDatabase { public: static const char *Unknown_8184D0(int skill);	// 0x8184D0: 'Combat', 'Trade' or 'Explore'  // NAME 1: neutral (D10)
	static const char *Unknown_818200(int skill);					// 0x818200: the skill's name  // NAME 1: neutral (D10)
	static const char *Unknown_818250(int skill);					// 0x818250: its description  // NAME 1: neutral (D10)
	static const char *Unknown_8182A0(int skill, int level);		// 0x8182A0: a level's detail  // NAME 1: neutral (D10)
	static const char *Unknown_818640(int skill, int index); };	// 0x818640: not yet trained  // NAME 1: neutral (D10)

// The level names (0xB79F04)
static const char *Unknown_B79F04[] = { "", "", "Improved", "Advanced", "Expert", "Super", "Ultra", "Master", "Extreme", "Awesome" };	// NAME 1: neutral (D10)

// @0x5482B0
Unknown548D60Class::Unknown548D60Class(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI12C", 1), client),
	UnknownAC(0),
	UnknownB0(0),
	UnknownB4(NULL),
	UnknownB8(NULL),
	UnknownBC(NULL)
{
	Unknown_BE67BC->Load("fa_tsi12c.ini");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}

// @0x548440
Unknown548D60Class::~Unknown548D60Class()
{
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("fa_tsi12c.ini");
}

// @0x548610
void Unknown548D60Class::Unknown_54()
{
	UnknownB8 = new ButtonGadgetClass(InterfaceMesh, "EXIT", 0x55, 3);
	Unknown64.Unknown_6870E0(UnknownB8, 1, View, 1, 0);
	UnknownB4 = new ButtonGadgetClass(InterfaceMesh, "UPGRADE", 0x55, 3);
	Unknown64.Unknown_6870E0(UnknownB4, 2, View, 1, 0);
	UnknownBC = Unknown_BE67C4->Find(InterfaceMesh->Peek_Mesh_Object());
	UnknownC0 = Create_Peek(Unknown_BE67C4->Get_Style("LVL_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownC4 = Create_Peek(Unknown_BE67C4->Get_Style("NOT_ACQUIRED_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownC8 = Create_Peek(Unknown_BE67C4->Get_Style("NEXT_LVL_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownCC = Create_Peek(Unknown_BE67C4->Get_Style("UPGRADE_BUTTON_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownD0 = Create_Peek(Unknown_BE67C4->Get_Style("POINTS_AVAILABLE_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownD4 = Create_Peek(Unknown_BE67C4->Get_Style("UPGRADE_REQUIRES_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownD8 = Create_Peek(Unknown_BE67C4->Get_Style("UPGRADE_COSTS_ATT", InterfaceMesh->Peek_Mesh_Object()));
	GadgetClass *gadget = new UIBackGadget(Create_Peek(InterfaceMesh->Peek_Object()));
	Unknown64.Unknown_6870E0(gadget, 3, View, 1, 0);
	Unknown_10();
}

// @0x548C50
bool Unknown548D60Class::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("skill_train_1.wav", 2);
			{
				int id = ((ClientClass *)Client)->AvatarID;
				((ClientClass *)Client)->Send_Packet(&SkillPacketClass(id, 1, UnknownAC));
			}
			return(true);
	}
	return(false);
}

// FontSys as 0x548D60 sets a style's colour (+0x494, its copy +0x4A4: ManagedFontClass::Unknown_6D1010's stores), inline
class AttrbFontSys : public FontSys	// NAME 1: neutral (D10)
{
public:
	void Set_Color(float r, float g, float b) { Unknown494.Color.Set(r, g, b); Unknown4A4 = Unknown494; }	// NAME 1 [Set_Color, r, g, b]: neutral (D10)
};

// NOT EXACT: field-offset (explain.py; a register choice: the title's colour: ours holds the style in ecx and &+0x494 in eax, the client the other way round,
// with the ecx / eax / edx choices after it; flags, branches and calls as the client's) (0x548D60)
// @0x548D60
void Unknown548D60Class::Unknown_548D60(SkillDataClass *list, int skill, int index)
{
	UnknownAC = skill;
	SkillLevelAvailabilityStruct *info = &list->Unknown50.Get_Availability(skill);
	int level = list->Unknown50.Get_Level(UnknownAC);
	int max = list->Unknown50.Get_Quest_Only_Levels(UnknownAC);
	bool maxed = (info->Unknown00 == 3 || level >= max);
	const char *category = SkillNamesDatabase::Unknown_8184D0(UnknownAC);
	const char *name = SkillNamesDatabase::Unknown_818200(UnknownAC);
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("TITLE_ATT", InterfaceMesh->Peek_Mesh_Object()));
	char buffer[0x40];
	if (strcmp(category, "Combat") == 0) {
		_snprintf(buffer, sizeof(buffer), "%s %s", Unknown_B79F04[level], name);
		title->Change_Text(buffer, 0);
		((AttrbFontSys *)title.Peek())->Set_Color(0.5f, 1.0f, 0.5f);
	} else if (strcmp(category, "Trade") == 0) {
		_snprintf(buffer, sizeof(buffer), "%s %s", Unknown_B79F04[level], name);
		title->Change_Text(buffer, 0);
		((AttrbFontSys *)title.Peek())->Set_Color(0.75f, 0.25f, 1.0f);
	} else if (strcmp(category, "Explore") == 0) {
		_snprintf(buffer, sizeof(buffer), "%s %s", Unknown_B79F04[level], name);
		title->Change_Text(buffer, 0);
		((AttrbFontSys *)title.Peek())->Set_Color(0.2f, 0.5f, 1.0f);
	} else {
		_snprintf(buffer, sizeof(buffer), "%s Level %d", name, level);
		title->Change_Text(buffer, 0);
		((AttrbFontSys *)title.Peek())->Set_Color(0.0f, 1.0f, 1.0f);
	}
	UnknownBC->Unknown_6D0A70("WRAP_SKILL_DESC_ATT", SkillNamesDatabase::Unknown_818250(UnknownAC));
	const char *detail;
	if (level) {
		UnknownC0->Set_Hidden(false);
		UnknownC4->Set_Hidden(true);
		detail = SkillNamesDatabase::Unknown_8182A0(UnknownAC, level);
	} else {
		UnknownC0->Set_Hidden(true);
		UnknownC4->Set_Hidden(false);
		detail = SkillNamesDatabase::Unknown_818640(UnknownAC, index);
	}
	UnknownBC->Unknown_6D0A70("WRAP_LEVEL_DETAIL_ATT", detail);
	if (info->Is_Bit06()) {
		UnknownBC->Unknown_6D0A70("WRAP_NEXT_LEVEL_DETAIL_ATT", "You have already attained the maximum level for this skill.");
		_snprintf(buffer, sizeof(buffer), "Maximum Level Attained");
	} else {
		UnknownBC->Unknown_6D0A70("WRAP_NEXT_LEVEL_DETAIL_ATT", SkillNamesDatabase::Unknown_8182A0(UnknownAC, level + 1));
		if (info->Is_Bit07()) {
			_snprintf(buffer, sizeof(buffer), "%s Only.", list->Unknown50.Get_Affiliations(UnknownAC).c_str());
		} else if (maxed) {
			buffer[0] = 0;
		} else if (info->Is_Bit00()) {
			_snprintf(buffer, sizeof(buffer), "Upgrade requires level %d", info->Unknown08);
		} else if (info->Is_Bit03()) {
			_snprintf(buffer, sizeof(buffer), "Requires combat level %d", info->Unknown08);
		} else if (info->Is_Bit04()) {
			_snprintf(buffer, sizeof(buffer), "Requires trade level %d", info->Unknown08);
		} else if (info->Is_Bit05()) {
			_snprintf(buffer, sizeof(buffer), "Requires explore level %d", info->Unknown08);
		} else if (info->Is_Bit01()) {
			_snprintf(buffer, sizeof(buffer), "Insufficient skill points");
		} else {
			_snprintf(buffer, sizeof(buffer), " ");
		}
	}
	UnknownD4->Change_Text(buffer, 0);
	_snprintf(buffer, sizeof(buffer), "Skill points available: %d", list->Get_Skill_Points());
	UnknownD0->Change_Text(buffer, 0);
	if (!maxed && !info->Is_Bit06()) {
		_snprintf(buffer, sizeof(buffer), "Upgrade costs %d skill points", info->Unknown0C);
	} else {
		_snprintf(buffer, sizeof(buffer), " ");
	}
	UnknownD8->Change_Text(buffer, 0);
	Unknown_58();
	if (info->Unknown04 == 0 && !maxed) {
		Unknown_5494E0(true);
	} else {
		Unknown_5494E0(false);
	}
}

#pragma auto_inline(off)
// @0x5494E0
void Unknown548D60Class::Unknown_5494E0(bool on)
{
	if (on) {
		UnknownB4->Unknown_40(true);
		UnknownB4->Unknown_8C(false);
		UnknownCC->Set_Hidden(false);
		UnknownD4->Set_Hidden(true);
		return;
	}
	UnknownB4->Unknown_40(false);
	UnknownB4->Unknown_8C(true);
	UnknownCC->Set_Hidden(true);
	UnknownD4->Set_Hidden(false);
}
#pragma auto_inline(on)

/*
**	Notes (AT2, 2026-11-06)
**
**	The client's object ui_attrb.obj runs 0x546A80-0x54962F (after ui_analyze.obj, before ui_attrb_skills.obj 0x549640):
**	two classes. First the attributes panel at ui_attrb_skills.cpp's +0xC0 (constructor 0x546A80, 0xB0 bytes; its
**	G_ASSERTs 'avatar' at lines 237 / 382, 0x547570 / 0x547E40), not in this file yet: when it comes, its functions go
**	above these (the panel's have no asserts, so their lines are free). Then the skill panel at ui_attrb_skills.cpp's
**	+0xFC (Unknown548D60Class): the constructor 0x5482B0, the deleting destructor 0x548410, the destructor 0x548440, slots
**	+0x54 0x548610 and +0x68 0x548C50, 0x5494E0 exact (auto_inline(off) around it: 0x548D60 calls it out of line); 0x548D60
**	(the selected skill's texts, called by ui_attrb_skills.cpp) NOT EXACT: the title's colour stores (tried: Vector3
**	assignment, Set, a FontSys pointer helper, a reference to +0x494, a local pointer, memcpy, a chained Set returning the
**	struct; attempts.csv). Its flags are bit fields read through bool accessors (the client's word load, shr, test 1;
**	plain shifts or bare bit fields give a byte test), the last call's argument an if / else of two calls. pkskill.h's inline ~SkillPacketClass 0x548D40 is emitted here between them. After the slots: the adjustor
**	thunks 0x5495D0 (-4) / 0x5495E0 (-0x38) of the deleting destructor and permstr.h's PermStringInitClass static
**	(0x5495A0 here; ui_attrb_skills.cpp's notes name 0x5495F0 / 0x549620).
*/
