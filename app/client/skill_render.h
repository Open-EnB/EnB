// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\skill_render.h (name assumed from skill_render.cpp, whose __FILE__ is
**	'e:\build\beta\G\app\Client\skill_render.cpp' at 0xB91528; the client holds no string of this header)
**
**	What draws a skill's slot: the ability's indicators, its 'REACTOR_BAR1' gauge and its range gauge. Table 0xAFD664,
**	constructor 0x6A5EA0, destructor 0x6A6070 (deleting 0x6A6040), 0x5C bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SKILL_RENDER_H
#define SKILL_RENDER_H

#include "slot_data_render.h"

class GaugeGadgetClass;	// gd_gauge.h
class ShortcutRangeGaugeClass;	// shortcut_range_gauge.h
class SkillDataClass;
class SlotDataGadgetClass;

class SkillRenderClass : public SlotDataRenderClass	// NAME 3: class-from-file (D24) for app\Client\skill_render.cpp
{
public:
	SkillRenderClass();													// 0x6A5EA0
	virtual ~SkillRenderClass();										// +0x04 0x6A6070 (deleting 0x6A6040)
	virtual bool Unknown_08(SlotDataClass *sd, RenderObjClass *robj);	// +0x08 0x6A6250
	virtual void Unknown_0C(std::string &out);							// +0x0C 0x6A6A80  // NAME 1 [out]: neutral
	virtual const char *Unknown_10(const char *serial_string);			// +0x10 0x6A6C00  // NAME 1 [serial_string]: as 0x6A90A0's assert
	virtual SkillRenderClass *Unknown_20() { return(this); }			// +0x20 0x6A6020
	virtual const char *Unknown_28(std::string *string);				// +0x28 0x6A7150  // NAME 3 [string]: its FAIL_IF
	virtual void Unknown_2C(bool force);								// +0x2C 0x6A6550  // NAME 1 [force]: neutral
	virtual TextureClass *Unknown_34();									// +0x34 0x6A6EA0
	virtual bool Unknown_38(Vector3 &color);							// +0x38 0x6A6EC0  // NAME 1 [color]: neutral
	virtual void Unknown_40(int slot);									// +0x40 0x6A7020  // NAME 1 [slot]: neutral
	virtual void Unknown_44(SlotDataRenderClass *render);				// +0x44 0x6A6F90  // NAME 3 [render]: its FAIL_IF
	virtual const char *Unknown_4C();											// +0x4C 0x6A72C0

	void Unknown_6A72E0(int ability, bool update);						// 0x6A72E0 (gd_attrb_skills.cpp's)  // NAME 1 [ability, update]: neutral
	int Get_34() { return(Unknown34); }									// (gd_attrb_skills.cpp's registers)  // NAME 1: neutral (D10)
	SkillDataClass *Unknown_6A7310();								// 0x6A7310 (the slot data's skill; asserts 'sd', 'skill')
	void Unknown_6A73C0(bool on, const RefCountPtr<RenderObjClass> &robj);	// 0x6A73C0 (onto or off the container)  // NAME 1 [on, robj]: neutral

	RefCountPtr<RenderObjClass> Unknown2C;								// +0x2C ('itemon01')
	GaugeGadgetClass *Unknown30;										// +0x30 (the 'REACTOR_BAR1' gauge)
	int Unknown34;														// +0x34 (the ability; -1)
	int Unknown38;														// +0x38 (0 and 1: shortcuts, 2: the skill list)
	RefCountPtr<RenderObjClass> ValidityIndicator;						// +0x3C ('item_dark')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> WrongTargetTypeIndicator;				// +0x40 ('item_noskill')  // NAME 3: its assert
	RefCountPtr<RenderObjClass> RechargeIndicator;						// +0x44 ('item_timer')  // NAME 3: its assert
	ShortcutRangeGaugeClass *Unknown48;										// +0x48 (the range gauge)
	float Unknown4C;													// +0x4C (the target's distance; -1)
	float Unknown50;													// +0x50 (the ability's range)
	unsigned int Unknown54;												// +0x54 (the flags last shown)
	bool Unknown58;														// +0x58 (ready, last shown)
};

SkillRenderClass *Unknown_6A7380(SlotDataGadgetClass *gadget);			// 0x6A7380 (FAIL_IF '!slot_render')  // NAME 1 [gadget]: neutral

#endif
