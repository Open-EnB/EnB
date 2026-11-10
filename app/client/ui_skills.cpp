// Earth & Beyond client (reconstruction): app\client\ui_skills.cpp, the skill windows' base (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\ui_skills.cpp' at 0xB8A428). Notes at the end of the file.
#include "ui_skills.h"
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "skill_data.h"
#include "skill_render.h"
#include "gd_slot_data.h"
#include "skillnam.h"
#include "skill_ability_names.h"
#include "ui_scut_gadgets.h"
#include "..\packets\pkskill.h"
#include "..\audio\soundlib.h"
#include "..\kernel\kernel.h"
#include "..\shared\g_assert.h"

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_inv.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18
};

// drag_slot_data.cpp's drag of a slot (SlotDataInterface's +0xA0), as this file calls it (as ui_inv.cpp declares it)
class DragSlotDataClass	// NAME 3: class-from-file (D24) for drag_slot_data.cpp (0x6851B0, 0x685500, 0x685610)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04();
	virtual bool Unknown_08();							// +0x08 (a drop onto a gadget)
	virtual void Unknown_0C();							// +0x0C
	char Unknown04[0x18 - 0x04];
	SlotDataRenderClass *Unknown18;						// +0x18 (the dragged slot's render)  // NAME 1: neutral (D10)
	void Unknown_685500(int slot, int mode, bool arg);	// 0x685500 (drag_slot_data.cpp)  // NAME 1 [slot, mode, arg]: what 0x5A05B0 passes
	int Get_Slot();										// 0x685610  // NAME 3: the assert 'from_slot != SlotDrag->Get_Slot()'
};

struct UISkillsViewUnknown44Struct	// NAME 1: neutral (D10): the view's slot +0x44, as this file reads it (as ui_inv.cpp's)
{
	char Unknown00[0x2C];
	DragSlotDataClass *Unknown2C;						// +0x2C (the drag under way)
};

class UISkillInfoClass;

// ui_slot_info.cpp's screen (SlotDataInterface's +0x9C), as far as this file calls it
class UISlotInfoClass	// NAME 3: class-from-file (D24) for app\Client\ui_slot_info.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88();
	virtual UISkillInfoClass *Unknown_8C();				// +0x8C (the skill information window, or NULL)
	char Unknown04[0xB0 - 0x04];
};

// ui_skill_info.cpp's window (0xCC bytes, 0x630280 makes it over UISlotInfoClass), as far as this file calls it
class UISkillInfoClass : public UISlotInfoClass	// NAME 3: class-from-file (D24) for app\Client\ui_skill_info.cpp
{
public:
	bool Unknown_62CA20(SlotDataGadgetClass *gadget);	// 0x62CA20  // NAME 1 [gadget]: what 0x62D540 passes
	SlotDataGadgetClass *UnknownB0;						// +0xB0 (the gadget shown)  // NAME 1: neutral (D10)
};

// 0x62D1F0
UISkillsClass::UISkillsClass(SClient *client) :
	SlotDataInterface(client)
{
	UnknownB8_3 = false;
	UnknownB0 = -1;
}

// 0x62D270
SlotDataClass *UISkillsClass::Unknown_88()
{
	return(new SkillDataClass((SClient *)Client));
}

// 0x62D300
UISkillsClass::~UISkillsClass()
{
}

// 0x62D330
void UISkillsClass::Unknown_54()
{
	SlotDataInterface::Unknown_54();
	Unknown_62E150();
}

// 0x62D360
bool UISkillsClass::Unknown_98(SlotDataGadgetClass *gadget, KernelEventClass *event, unsigned int mask)
{
	bool up = event->Up;
	bool right = (mask & 0xF0) != 0;
	if (!up) {
		SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
	}
	UISkillInfoClass *info = Unknown9C->Unknown_8C();
	if (info && info->UnknownB0) {
		if (gadget == info->UnknownB0) {
			if (up) {
				info->Unknown_5C();
			}
			if (right) {
				return(true);
			}
		} else {
			info->Unknown_5C();
		}
	}
	if (!SlotDataInterface::Unknown_98(gadget, event, mask) && !right) {
		if (mask & 0x1000) {
			if (up) {
				Unknown_62D730((SlotDataGadgetClass *)gadget->Unknown_A4());
				return(true);
			}
		} else {
			if (up) {
				Unknown_62DA30(gadget);
				return(true);
			}
			Unknown_62D960(gadget, event);
		}
	}
	return(true);
}








// 0x62D540
bool UISkillsClass::Unknown_A0(SlotDataGadgetClass *gadget)
{
	UISkillInfoClass *skill_info = Unknown9C->Unknown_8C();
	FAIL_IF(!skill_info) return(false);
	return(skill_info->Unknown_62CA20(gadget));
}

// 0x62D590
void UISkillsClass::Unknown_A4(std::string &name, int index, int arg)
{
	const char *skill = SkillNamesDatabase::Unknown_818200(index);
	name = skill ? skill : "Unknown Skill";
}





// 0x62D6A0
SlotDataGadgetClass *UISkillsClass::Unknown_AC(RenderObjClass *robj, const int *slot)
{
	SlotDataGadgetClass *gadget = SlotDataInterface::Unknown_AC(robj, slot);
	SkillRenderClass *render = Unknown_6A7380(gadget);
	if (render) render->Unknown38 = 2; else G_ASSERT(render);
	return(gadget);
}

// 0x62D710
SkillDataClass *UISkillsClass::Unknown_62D710()
{
	return(SlotData->Unknown_14());
}









// 0x62D730
// NOT EXACT: functional (D64), from the pseudocode
void UISkillsClass::Unknown_62D730(SlotDataGadgetClass *gadget)
{
	if (!gadget) return;
	if (gadget->Unknown_670910() == -1) return;
	SkillRenderClass *skill_render = Unknown_6A7380(gadget);
	FAIL_IF(!skill_render) return;
	SkillDataClass *data = skill_render->Unknown_6A7310();
	FAIL_IF(!data) return;
	SkillAbilityDataPeekClass abilities = data->Abilities;
	int ability = skill_render->Unknown34;
	if (ability != -1) {
		if (!abilities.Get_Can_Be_Used_While_Incapacitated(ability) && Client->As_Space_Client()->Unknown_750B60()) {
			Client->As_Space_Client()->Unknown_74D990(0x11, "Cannot perform action while incapacitated.", 0);
		} else {
			((ClientClass *)Client)->Send_Packet(&SkillPacket_88Class(((ClientClass *)Client)->AvatarID, 0, ability));
		}
	}
}











// 0x62D960
void UISkillsClass::Unknown_62D960(SlotDataGadgetClass *gadget, KernelEventClass *event)
{
	if (((UISkillsViewUnknown44Struct *)View->Unknown_44())->Unknown2C == NULL) {
		int slot = gadget->Unknown_670910();
		SlotDrag->Unknown_685500(slot, 1, false);
		SlotDataRenderClass *render = SlotDrag->Unknown18;
		FAIL_IF(!render) return;
		SkillRenderClass *srender = render->Unknown_20();

		FAIL_IF(!srender) return;
		srender->Unknown_44(Unknown_6A7380(gadget));
	}
}






// 0x62DA30
void UISkillsClass::Unknown_62DA30(SlotDataGadgetClass *gadget)
{
	if (((UISkillsViewUnknown44Struct *)View->Unknown_44())->Unknown2C == SlotDrag) {
		int from_slot = gadget->Unknown_670910();
		FAIL_IF(from_slot != SlotDrag->Get_Slot()) return;
		if (!SlotDrag->Unknown_08()) {
			Unknown_62D730((SlotDataGadgetClass *)gadget->Unknown_A4());
			return;
		}
		SlotDrag->Unknown_0C();
		if (from_slot != -1) {
			GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
			if (target) {
				Unknown66CB20Class *button = (Unknown66CB20Class *)target->Unknown_14();
				if (button && ShortcutUI) {
					SkillRenderClass *render = Unknown_6A7380((SlotDataGadgetClass *)gadget->Unknown_10());
					int ability = render->Unknown34;
					int slot = render->Get_Slot();
					RObjGadgetClass *shortcut = ShortcutUI->Create_Shortcut(from_slot, button);
					SkillRenderClass *new_render = Unknown_6A7380((SlotDataGadgetClass *)shortcut->Unknown_10());



















					FAIL_IF(!new_render) return;
					new_render->Unknown_6A72E0(ability, true);
					new_render->Unknown_40(slot);
					ShortcutUI->Save_Current();
				}
			}
		}
	}
}

// 0x62DC00
bool UISkillsClass::Unknown_9C(int slot, bool arg)
{
	const Unknown8155B0Struct &abilities = SkillAbilityNamesDatabase::Unknown_8155B0(slot);
	if (abilities.NumActivatable) {
		unsigned int level = Unknown_62D710()->Skills.Get_Level(slot);
		for (std::vector<Unknown815320Struct *>::const_iterator it = abilities.Abilities.begin(); it != abilities.Abilities.end(); it++) {
			if ((*it)->Unknown1C && (*it)->Unknown18 <= level) {
				return(true);
			}
		}
	}
	return(false);
}


/*
**	Notes (SK1, 2026-11-09).
**	The object runs from before 0x62D1F0 (its PermStringInitClass static, 0xBE5E50) to the adjustor thunks 0x62DCB0 /
**	0x62DCC0. UISkillsClass is the class-from-file name (D24) of the window ui_attrb_skills.h's attributes tab is over
**	(it was flattened there as Unknown62D1F0Class). Written from the pseudocode, the asserts on the client's lines;
**	the functions so marked (D64) are functional only. 0x62D360 inlines 0x62D960 (its FAIL_IFs l.224 / 227).
*/
