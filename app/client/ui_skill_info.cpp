// Earth & Beyond client (reconstruction): app\client\ui_skill_info.cpp, a skill's information window (the client's file
// per its asserts, 'e:\build\beta\G\app\Client\ui_skill_info.cpp' at 0xB8A3B8). Notes at the end of the file.
#include "ui_skill_info.h"
#include "sclient.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "fontmanager.h"
#include "skill_data.h"
#include "skill_render.h"
#include "skill_ability_names.h"
#include "gd_slot_data.h"
#include "unknown_51c800.h"
#include "dynamic_window_class.h"
#include "local_interface_sort.h"
#include "..\shared\g_assert.h"
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10), as inv_trade_render.cpp declares it
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);				// 0x496AE0 (ro_funcs.cpp)
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);			// 0x495FD0 (ro_funcs.cpp)
bool Unknown_496110(RenderObjClass *robj, float r, float g, float b);				// 0x496110 (ro_funcs.cpp)
void Unknown_496730(RenderObjClass *robj, int sort_level, bool dec_if_match);		// 0x496730 (ro_funcs.cpp)

// A row of the window: an ability's name ('tsi01a_right') and its icon ('tsi01a_circle') (0x14 bytes; destructor 0x62CCC0)
class UISkillInfoAbilityClass	// NAME 1: neutral (D10)
{
public:
	UISkillInfoAbilityClass(const RefCountPtr<RenderObjClass> &robj, const RefCountPtr<RenderObjClass> &circle, const RefCountPtr<RenderObjClass> &icon, ManagedFontClass *font_group, int ability) :	// NAME 1 [robj, circle, icon, ability]: neutral; NAME 3 [font_group]: the FAIL_IF's
		Robj(robj),
		Circle(circle),
		Icon(icon),
		FontGroup(font_group),
		Ability(ability)
	{
	}

	RefCountPtr<RenderObjClass> Robj;			// +0x00 ('tsi01a_right': the name, the font group's id)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Circle;		// +0x04 ('tsi01a_circle', onto Robj's 'SKILL' bone)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Icon;			// +0x08 (Circle's 'ICON')  // NAME 1: neutral (D10)
	ManagedFontClass *FontGroup;				// +0x0C  // NAME 3: the FAIL_IF's 'font_group'
	int Ability;								// +0x10 (the ability's index)  // NAME 1: neutral (D10)
};

// 0x62B930
UISkillInfoClass::UISkillInfoClass(LineDisplayClientClass *client, SkillDataClass *skill) :
	UISlotInfoClass(client, "SKILL_INFO_MAIN"),
	UnknownB0(NULL),
	SkillData(skill)
{
	Unknown_BE67BC->Load("fc_skillinfo.ini");
	UnknownA9 = false;
}

// 0x62BA70
UISkillInfoClass::~UISkillInfoClass()
{
	Unknown_62CBB0();








	Unknown_BE67BC->Unload("fc_skillinfo.ini");
}

// 0x62BC40
bool UISkillInfoClass::Unknown_94()
{
	Unknown_62CBB0();










	FAIL_IF(!SkillRender) return false;
	FAIL_IF(!SkillData) return false;
	int skill = SkillRender->Get_Slot();
	unsigned int level = SkillData->Skills.Get_Level(skill);
	const Unknown8155B0Struct &abilities = SkillAbilityNamesDatabase::Unknown_8155B0(skill);
	int row = 0;
	RefCountPtr<Unknown51C800Class> label;			// NAME 1 [label, robj, skill_sub, circle, icon, texture, color, id]: neutral
	RefCountPtr<RenderObjClass> robj;
	RefCountPtr<RenderObjClass> skill_sub;
	RefCountPtr<RenderObjClass> circle;
	RefCountPtr<RenderObjClass> icon;
	RefCountPtr<TextureClass> texture;
	Vector3 color(0.0f, 0.0f, 0.0f);
	int id = 1;
	for (std::vector<Unknown815320Struct *>::const_iterator it2 = abilities.Abilities.begin(); it2 != abilities.Abilities.end(); ++it2) {
		Unknown815320Struct *ability = *it2;
		if (ability->Unknown18 <= level) {
			robj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi01a_right"));
			Unknown_496AE0(robj.Peek(), false);
			ManagedFontClass *font_group = Unknown_BE67C4->Add(robj);












			FAIL_IF(!font_group) continue;
			font_group->Unknown_6D0A70("WRAP_SKILL_INFO_NAME_RIGHT", ability->Name.c_str());
			label = Create_NEW(new Unknown51C800Class((FontSys *)robj.Peek(), 6));
			Add_Element(label, row++);
			skill_sub = Create_Get(robj->Get_Sub_Object_By_Name("SKILL"));
			ButtonGadgetClass *button = new ButtonGadgetClass(skill_sub.Peek(), 5, 3);
			GadgetClass::Set_Tooltip(button, "UI_SKILL_INFO", "ABILITY", ability->Description.c_str());
			Unknown84_08(button, id++, -1);
			circle = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi01a_circle"));
			Unknown_496AE0(circle.Peek(), false);
			icon = Create_Get(circle->Get_Sub_Object_By_Name("ICON"));
			texture = Create_Get(SkillAbilityNamesDatabase::Unknown_815690(ability->Unknown10, &color));
			Unknown_495FD0(icon.Peek(), texture.Peek(), 0);
			Unknown_496110(icon.Peek(), color.X, color.Y, color.Z);
			UnknownBC.push_back(new UISkillInfoAbilityClass(robj, circle, icon, font_group, ability->Unknown10));
		}
	}
	return true;
}

// 0x62C500
UISlotInfoItemClass *UISkillInfoClass::Unknown_90()
{
	return((UISlotInfoItemClass *)SkillData);
}














// 0x62C520
void UISkillInfoClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	int bone = 0;	// NAME 1 [bone, size, box]: neutral
	DynamicWindowClass *window = MainWindow.Peek();
	FAIL_IF(!window) return;
	FAIL_IF(!SkillRender) return;
	Vector2 size = window->Get_Size();

	RenderObjClass *mesh = SkillRender->Mesh;	// NAME 1 [mesh]: SlotDataRenderClass's
	RefCountPtr<RenderObjClass> scut_robj = Create_Peek(mesh);
	FAIL_IF(!scut_robj) return;
	const AABoxClass &box = scut_robj->Get_Bounding_Box();
	Unknown_58A580(Vector2(box.Center.X - box.Extent.X + size.X * 0.5f, size.Y * 0.5f + box.Extent.Y + box.Center.Y));
	UnknownA8 = false;
	UISlotInfoClass::Unknown_58();
	for (std::vector<UISkillInfoAbilityClass *>::iterator it = UnknownBC.begin(); it != UnknownBC.end(); ++it) {
		UISkillInfoAbilityClass *ability = *it;
		if (bone == 0) {
			RefCountPtr<RenderObjClass> skill_sub = Create_Get(ability->Robj->Get_Sub_Object_By_Name("SKILL"));
			bone = ability->Robj->Get_Sub_Object_Bone_Index(skill_sub.Peek());
		}
		ability->Robj->Add_Sub_Object_To_Bone(ability->Circle.Peek(), bone);
		RefCountPtr<RenderObjClass> bttn_sub = Create_Get(ability->Robj->Get_Sub_Object(bone));












		FAIL_IF(!bttn_sub) continue;
		int level = bttn_sub->Get_Sort_Level();	// NAME 1 [level]: neutral
		Unknown_496730(ability->Circle.Peek(), level - 1, false);
		Unknown_496730(ability->Icon.Peek(), level - 2, false);
		ability->FontGroup->Unknown_6D01C0(level - 1, NULL);
	}
}


// 0x62C8E0
void UISkillInfoClass::Unknown_5C()
{
	Unknown_62CA20(NULL);
	if (Active) {
		UISlotInfoClass::Unknown_5C();
	}
}
// 0x62C910
bool UISkillInfoClass::Unknown_68(int key)
{
	FAIL_IF(!SkillRender) return false;
	unsigned int index = key - 1;	// NAME 1 [index]: neutral
	if (index < UnknownBC.size()) {
		SkillRender->Unknown_6A72E0(UnknownBC[index]->Ability, true);
		Unknown_5C();
		return true;
	}
	return UISlotInfoClass::Unknown_68(key);
}

// 0x62C9B0
void UISkillInfoClass::Unknown_60()
{
	if (UnknownB0) {
		UnknownA8 = !UnknownB0->Unknown2C_02;
	}
	UISlotInfoClass::Unknown_60();
}




// 0x62C9E0
SkillDataClass *UISkillInfoClass::Unknown_62C9E0()
{
	FAIL_IF(!SkillData) return NULL;
	return(SkillData->Unknown_14());
}

// 0x62CA20
bool UISkillInfoClass::Unknown_62CA20(SlotDataGadgetClass *gadget)
{
	if (gadget) {
		SkillRender = Create_Peek(Unknown_6A7380(gadget));
		FAIL_IF(!SkillRender) return false;
		if (SkillRender->Unknown38) {
			SkillRender = NULL;
			return false;
		}
		UnknownB0 = gadget;
		SkillRender->Unknown38 = 1;
		return true;
	} else if (SkillRender) {
		SkillRender->Unknown38 = 0;
		SkillRender = NULL;
		UnknownB0 = NULL;
	}
	return false;
}

// 0x62CBB0
void UISkillInfoClass::Unknown_62CBB0()
{
	Unknown_58ADA0();
	Remove_All(true);
	for (std::vector<UISkillInfoAbilityClass *>::iterator it = UnknownBC.begin(); it != UnknownBC.end(); ++it) {
		UISkillInfoAbilityClass *ability = *it;
		Unknown_BE67C4->Remove(ability->Robj.Peek());
		ability->Circle->Remove();
		delete ability;
	}
	UnknownBC.clear();
}

/*
** Reconstruction notes (O6, run 2026-11-10b).
**
** The object 0x62B930-0x62D19F: the constructor 0x62B930, slot +0x8C 0x62BA20 (inline in the header), the destructor
** 0x62BA70 (deleting 0x62BA40), the members in address order up to 0x62CBB0, then the row class's destructor 0x62CCC0
** (deleting 0x62CC90), the instances of RefCountPtr<SkillRenderClass> (Create_Peek 0x62CD90, destructor 0x62CDC0), of
** std::vector<UISkillInfoAbilityClass *> (destructor 0x62CDF0, clear 0x62CE30, _Destroy 0x62CE80, insert 0x62CEA0)
** and the adjustor thunks 0x62D170 / 0x62D180 / 0x62D190. Slots from the table 0xAF7C64: +0x58 0x62C520, +0x5C
** 0x62C8E0, +0x60 0x62C9B0, +0x68 0x62C910, +0x8C 0x62BA20, +0x90 0x62C500, +0x94 0x62BC40.
*/
