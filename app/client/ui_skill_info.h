// Earth & Beyond client (reconstruction): app\client\ui_skill_info.h, a skill's information window (the client's
// ui_skill_info.cpp, 'e:\build\beta\G\app\Client\ui_skill_info.cpp' at 0xB8A3B8; this header's name is provisional,
// NAME 1). The object is 0x62B930-0x62D19F (the constructor first; the adjustor thunks 0x62D170 / 0x62D180 /
// 0x62D190 last); ui_slot_info.cpp's 0x630280 makes it, ui_skills.cpp's 0x62D540 shows a slot's skill in it.
#ifndef UI_SKILL_INFO_H  // NAME 1 [ui_skill_info.h]: provisional header name for ui_skill_info.cpp
#define UI_SKILL_INFO_H

#include "ui_slot_info.h"

class SkillDataClass;
class SkillRenderClass;
class SlotDataGadgetClass;
class UISkillInfoAbilityClass;

// ui_skill_info.cpp's window (0xCC bytes; tables 0xAF7C64 / 0xAF7C58 / 0xAF7C4C / 0xAF7C34; 'SKILL_INFO_MAIN',
// 'fc_skillinfo.ini'): a skill's abilities, one row each, with the slot's shortcut button beside them
class UISkillInfoClass : public UISlotInfoClass  // NAME 3: class-from-file (D24) for app\Client\ui_skill_info.cpp
{
public:
	UISkillInfoClass(LineDisplayClientClass *client, SkillDataClass *skill);				// 0x62B930  // NAME 1 [client, skill]: what 0x630280 passes
	virtual ~UISkillInfoClass();															// 0x62BA70 (slot 0x62BA40)
	virtual void Unknown_58();																// 0x62C520 (beside the slot's shortcut: FAIL_IF '!window', '!scut_robj', '!bttn_sub')  // NAME 1: neutral (D10)
	virtual void Unknown_5C();																// 0x62C8E0 (the gadget let go)  // NAME 1: neutral (D10)
	virtual void Unknown_60();																// 0x62C9B0 (UnknownA8: the gadget's bit 2 clear)  // NAME 1: neutral (D10)
	virtual bool Unknown_68(int key);														// 0x62C910 (key 1..n: the n-th ability into the shortcut)  // NAME 1 [key]: InterfaceClass's
	virtual int Unknown_8C() { return((int)this); }									// +0x8C 0x62BA20 (itself: ui_skills.cpp reads it as UISkillInfoClass *; the base keeps int, retyping it loses 0x532170's short credit)  // NAME 1: neutral (D10)
	virtual UISlotInfoItemClass *Unknown_90();												// +0x90 0x62C500 (SkillData)  // NAME 1: neutral (D10)
	virtual bool Unknown_94();																// +0x94 0x62BC40 (the abilities' rows rebuilt: FAIL_IF '!SkillRender', '!SkillData', '!font_group')  // NAME 1: neutral (D10)

	SkillDataClass *Unknown_62C9E0();														// 0x62C9E0 (no caller; FAIL_IF '!SkillData', its slot +0x14)  // NAME 1: neutral (D10)
	bool Unknown_62CA20(SlotDataGadgetClass *gadget);										// 0x62CA20 (the gadget's skill render taken: FAIL_IF '!SkillRender'; NULL lets it go)  // NAME 1 [gadget]: what 0x62D540 passes
	void Unknown_62CBB0();																	// 0x62CBB0 (no caller: the rows removed)  // NAME 1: neutral (D10)

	SlotDataGadgetClass *UnknownB0;															// +0xB0 (the gadget shown)  // NAME 1: neutral (D10)
	RefCountPtr<SkillRenderClass> SkillRender;												// +0xB4 (its +0x38: 1 while shown here)  // NAME 3: the FAIL_IFs'
	SkillDataClass *SkillData;																// +0xB8  // NAME 3: the FAIL_IFs'
	std::vector<UISkillInfoAbilityClass *> UnknownBC;										// +0xBC (one per ability shown; Unknown_68's keys)  // NAME 1: neutral (D10)
};

#endif
