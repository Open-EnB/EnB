// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_attrb_skills.h (name assumed from gd_attrb_skills.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_attrb_skills.cpp' at 0xB8CCCC; the client holds no string of this header)
**
**	The attributes tab's skill gadget (ui_attrb_skills.cpp's UnknownC4 elements): a button over the 'TSI12D' model
**	with the skill's name, its level dots and its abilities' slot gadgets. Table 0xAF9D1C (42 slots), constructor
**	0x662DC0, destructor 0x6636E0 (deleting 0x6636B0), 0x114 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_ATTRB_SKILLS_H  // NAME 3 [gd_attrb_skills.h]: header name assumed from the client's gd_attrb_skills.cpp
#define GD_ATTRB_SKILLS_H

#include <vector>
#include "always.h"
#include "..\..\wwlib\ref_ptr.h"
#include "unknown_675120.h"

class RenderObjClass;
class FontSys;
class UIAttrbSkillsClass;
class SkillDataClass;
class SlotDataGadgetClass;

class Unknown664500Class : public ButtonGadgetClass	// NAME 1: neutral (D10)
{
public:
	Unknown664500Class();									// 0x662DC0 (0x114 bytes; ui_attrb_skills.cpp's 0x54ACE0 makes it)
	virtual ~Unknown664500Class();							// +0x00 0x6636E0 (deleting 0x6636B0)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x663B50 (the hovered dot, else the base's)
	virtual void Unknown_28(LayerClass *layer);				// +0x28 0x663DA0 (the slot gadgets too)
	virtual bool Unknown_30(float x, float y);				// +0x30 0x663950 (which dot the pointer is on: Unknown88)
	virtual void Unknown_A4(bool arg);						// +0xA4 0x663BE0 (the levels read from the skill data)  // NAME 1 [arg]: neutral

	int Unknown_663DF0(UIAttrbSkillsClass *ui, int skill, int index);	// 0x663DF0 (returns Unknown110)  // NAME 1 [skill, index]: neutral; ui: its FAIL_IF '!ui'
	void Unknown_664430();									// 0x664430 (hidden, detached, its slot gadgets dropped)
	void Unknown_664500();									// 0x664500 (each shown slot gadget's level state)
	int Unknown_6645A0(int level, bool arg);				// 0x6645A0 (onto 'FILL_BONE%02d', large or small)  // NAME 1 [level, arg]: neutral
	void Unknown_6647F0();									// 0x6647F0 (hidden)
	void Unknown_664890(bool on);							// 0x664890 (the 'I_BLANK_SEL' / 'co0101' texture)  // NAME 1 [on]: neutral
	void Unknown_6649F0();									// 0x6649F0 (the level dots)
	void Unknown_664AE0(int index, int state);				// 0x664AE0 (a dot's texture: 0 'IF_DOT00', 1 'IF_DOT01')  // NAME 1 [index, state]: neutral
	void Unknown_664C60();									// 0x664C60 (the tooltip's text)

	RefCountPtr<RenderObjClass> AttachTo;					// +0x64  // NAME 3: its asserts
	RefCountPtr<SkillDataClass> SkillData;				// +0x68  // NAME 3: its assert
	int Unknown6C;											// +0x6C (the skill)
	RefCountPtr<RenderObjClass> Unknown70;					// +0x70 (the 'TSI12D' model)
	RefCountPtr<RenderObjClass> LargeBttn;					// +0x74 ('HIGHLIGHT01')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> SmallBttn;					// +0x78 ('HIGHLIGHT00')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> LargeDarken;				// +0x7C ('DARKEN01')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> SmallDarken;				// +0x80 ('DARKEN00')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> UpgradeRObj;				// +0x84 ('UP00')  // NAME 3: its assert
	int Unknown88;											// +0x88 (the dot under the pointer, -1)
	RefCountPtr<RenderObjClass> Unknown8C[11];				// +0x8C ('DOT00_%02d'; MAX_SKILLS, the assert's)
	RefCountPtr<RenderObjClass> AbilityIcons[7];			// +0xB8 ('ICON_BOX%02d')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> AbilityIconBox[7];			// +0xD4 ('SKILL_BOX%02d')  // NAME 3: its assert
	std::vector<SlotDataGadgetClass *> UnknownF0;			// +0xF0 (its allocator byte, then +0xF4 first / +0xF8 last / +0xFC end)
	RefCountPtr<FontSys> Unknown100;						// +0x100 ('SKILL_NAME_ATT', on 'DESC00')
	int Unknown104;											// +0x104 (0x6A54B0: the level)
	int Unknown108;											// +0x108 (0x6A59E0: the dots shown; 11 at first)
	bool Unknown10C;										// +0x10C
	bool Unknown10D;										// +0x10D (large)
	int Unknown110;											// +0x110 (the slot gadgets; 0x54B370 counts the gadgets where it is set)
};

#endif
