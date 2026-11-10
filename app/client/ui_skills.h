// Earth & Beyond client (reconstruction): app\client\ui_skills.h, the skill windows' base (the client's ui_skills.cpp per
// its asserts, 'e:\build\beta\G\app\Client\ui_skills.cpp' at 0xB8A428; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x62D1F0 (over ui_slot_data.cpp's 0x62DE80) and its tables 0xAF7DBC / 0xAF7DB0 / 0xAF7DA4.
#ifndef UI_SKILLS_H  // NAME 1 [ui_skills.h]: provisional header name for ui_skills.cpp
#define UI_SKILLS_H

#include "ui_slot_data.h"

class SClient;
class SkillDataClass;	// skill_data.h's (what 0x62D710 returns; ui_attrb_skills.cpp declares it as far as it calls it)

// ui_skills.cpp's window (0xBC bytes, constructor 0x62D1F0, destructor 0x62D300 (deleting 0x62D240)): the slots of the
// avatar's skills; the attributes tab (ui_attrb_skills.h) is over it
class UISkillsClass : public SlotDataInterface	// NAME 3: class-from-file (D24) for app\Client\ui_skills.cpp
{
public:
	UISkillsClass(SClient *client);						// 0x62D1F0  // NAME 1 [client]: as SlotDataInterface's
	virtual ~UISkillsClass();							// 0x62D300 (deleting 0x62D240)
	virtual void Unknown_54();							// 0x62D330 (the base's, then the shortcut bar)
	virtual SlotDataClass *Unknown_88();				// +0x88 0x62D270 (a new SkillDataClass)
	virtual InterfaceClass *Unknown_94() { return(this); }	// +0x94 0x5497A0 (inline: emitted with the attributes tab's table)
	virtual bool Unknown_98(SlotDataGadgetClass *gadget, KernelEventClass *event, unsigned int mask);	// +0x98 0x62D360 ('TextClick03.wav'; FAIL_IFs l.224 / 227 through 0x62D960)  // NAME 1 [gadget, event, mask]: the base's
	virtual bool Unknown_9C(int slot, bool arg);		// +0x9C 0x62DC00 (the skill has an ability at its level)  // NAME 1 [slot, arg]: the base's
	virtual bool Unknown_A0(SlotDataGadgetClass *gadget);	// +0xA0 0x62D540 (FAIL_IF '!skill_info' l.148)  // NAME 1 [gadget]: the base's
	virtual void Unknown_A4(std::string &name, int index, int arg);	// +0xA4 0x62D590 (the skill's name, or 'Unknown Skill')  // NAME 1 [name, index, arg]: the base's
	virtual const char *Unknown_A8() { return("Skills"); }	// +0xA8 0x5497C0 (inline: emitted with the attributes tab's table)
	virtual SlotDataGadgetClass *Unknown_AC(RenderObjClass *robj, const int *slot);	// +0xAC 0x62D6A0 (G_ASSERT 'render' l.168: the base's, its render a skill list's)  // NAME 1 [robj, slot]: the base's
	SkillDataClass *Unknown_62D710();					// 0x62D710 (SlotData's slot +0x14)
	void Unknown_62D730(SlotDataGadgetClass *gadget);	// 0x62D730 (FAIL_IFs '!skill_render' l.193, '!data' l.195: the ability used)  // NAME 1 [gadget]: neutral
	void Unknown_62D960(SlotDataGadgetClass *gadget, KernelEventClass *event);	// 0x62D960 (FAIL_IFs '!render' l.224, '!srender' l.227: a drag starts; ret 8, the second unused)  // NAME 1 [gadget, event]: neutral
	void Unknown_62DA30(SlotDataGadgetClass *gadget);	// 0x62DA30 (FAIL_IFs l.242 / 277: a drop, onto the shortcut bar)  // NAME 1 [gadget]: neutral
};

#endif // UI_SKILLS_H
