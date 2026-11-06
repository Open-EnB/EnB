// Earth & Beyond client (reconstruction): app\client\gd_attrb_skills.cpp, the attributes tab's skill gadget
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\gd_attrb_skills.cpp' at 0xB8CCCC). Notes at the end.
#include <stdio.h>
#include <string>
#include <vector>
#include "..\shared\g_assert.h"
#include "gd_attrb_skills.h"
#include "ui_attrb_skills.h"
#include "fontmanager.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "texture.h"
#include "..\shared\split_string.h"

extern const int Unknown_B0804C;	// 0xB0804C: 40, the tooltip's width (read as an int, so not a literal; no other reader)  // NAME 1: neutral (D10)
#define MAX_SKILLS 11	// NAME 3: the assert's; the dots' array

// What the skill data holds about a skill, as far as this file calls it (0x6A5440's class, at the data's +0x50)
struct Unknown6A5640Struct { int Unknown00; int Unknown04; };	// NAME 1: neutral (D10), as ui_attrb.cpp's
class Unknown6A5440Class	// NAME 1: neutral (D10), as ui_attrb_skills.cpp's
{
public:
	bool Unknown_6A5470(int skill);	Unknown6A5640Struct *Unknown_6A5640(int skill);	int Unknown_6A54B0(int skill);	int Unknown_6A59E0(int skill);	// NAME 1 [skill]: neutral
};
// ui_attrb_skills.cpp's list (0x62D710's answer); its reference count sits at +0x18 here
class Unknown6A7A00Base { public: virtual void Unknown_00(); char Unknown04[0x14]; };	// NAME 1: neutral (D10)
class Unknown6A7A00Class : public Unknown6A7A00Base, public RefCountClass	// NAME 1: neutral (D10)
{
public:
	char Unknown20[0x30];	Unknown6A5440Class Unknown50;	// +0x50
};
// gd_slot_data.cpp's gadget (UnknownF0's elements), as far as this file calls it
class Unknown670C90Class : public RObjGadgetClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_670C90(bool on);	char Unknown60[8];	bool Unknown68;	// 0x670C90 (Unknown68); +0x68  // NAME 1 [on]: neutral
};
// skill_render.cpp's (0x6A7380: a slot gadget's) and skill_ability_names.cpp's, as far as this file calls them
class Unknown6A7380Class { public: void Unknown_6A72E0(int ability, int arg); char Unknown00[0x34]; int Unknown34; int Get_34() { return(Unknown34); } };	// NAME 1: neutral (D10)
Unknown6A7380Class *Unknown_6A7380(Unknown670C90Class *gadget);	// NAME 1: neutral (D10)
struct Unknown815320Struct { char Unknown00[0x10]; int Unknown10; int Unknown14; unsigned int Unknown18; bool Unknown1C; unsigned int Get_18() { return(Unknown18); } };	// NAME 1: neutral (D10)
Unknown815320Struct *Unknown_815320(int ability);	// NAME 1: neutral (D10)
struct Unknown8155B0Struct { std::vector<Unknown815320Struct *> Abilities; int Unknown10; };	// NAME 3 [Abilities]: its assert
const Unknown8155B0Struct &Unknown_8155B0(int skill);	// NAME 1: neutral (D10)
const char *Unknown_818200(int skill);	const char *Unknown_818250(int skill);	const char *Unknown_8182A0(int skill, int level);	// as ui_attrb.cpp's
unsigned int Unknown_8185C0(int skill, int index);	// NAME 1: neutral (D10)
bool Unknown_496960(RenderObjClass *robj, float x, float y, LayerClass &layer);	// ro_funcs.cpp's  // NAME 1: neutral (D10)
void Unknown_496AE0(RenderObjClass *robj, int arg);	// ro_funcs.cpp's  // NAME 1: neutral (D10)
void Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int index);	// ro_funcs.cpp's  // NAME 1: neutral (D10)

// 0x662DC0
Unknown664500Class::Unknown664500Class() :
	ButtonGadgetClass((RenderObjClass *)NULL, 5, 3),
	Unknown6C(-1),
	Unknown88(-1),
	Unknown104(0),
	Unknown108(MAX_SKILLS),
	Unknown10C(false),
	Unknown10D(false),
	Unknown110(0)
{
	Unknown70 = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("TSI12D"));
	Unknown_496AE0(Unknown70.Peek(), 1);
	Unknown100 = Create_NEW(new FontSys("SKILL_NAME_ATT", 0, NULL));
	Unknown70->Add_Sub_Object_To_Bone(Unknown100.Peek(), "DESC00");
	LargeBttn = Create_Get(Unknown70->Get_Sub_Object_By_Name("HIGHLIGHT01"));












	G_ASSERT(LargeBttn);
	SmallBttn = Create_Get(Unknown70->Get_Sub_Object_By_Name("HIGHLIGHT00")); G_ASSERT(SmallBttn);
	LargeDarken = Create_Get(Unknown70->Get_Sub_Object_By_Name("DARKEN01")); G_ASSERT(LargeDarken);
	SmallDarken = Create_Get(Unknown70->Get_Sub_Object_By_Name("DARKEN00")); G_ASSERT(SmallDarken);
	UpgradeRObj = Create_Get(Unknown70->Get_Sub_Object_By_Name("UP00")); G_ASSERT(UpgradeRObj);
	LargeDarken->Set_Sort_Level(Unknown100->Get_Sort_Level() - 1);
	SmallDarken->Set_Sort_Level(Unknown100->Get_Sort_Level() - 1);
	char name[0x41];
	int lp;
	for (lp = 0; lp < MAX_SKILLS; lp++) {
		_snprintf(name, 0x41, "DOT00_%02d", lp);
		Unknown8C[lp] = Create_Get(Unknown70->Get_Sub_Object_By_Name(name));
	}
	for (lp = 0; lp < 7; lp++) {
		_snprintf(name, 0x41, "SKILL_BOX%02d", lp);
		AbilityIconBox[lp] = Create_Get(Unknown70->Get_Sub_Object_By_Name(name));





		G_ASSERT(AbilityIconBox[lp]);
		_snprintf(name, 0x41, "ICON_BOX%02d", lp);
		AbilityIcons[lp] = Create_Get(Unknown70->Get_Sub_Object_By_Name(name));

		G_ASSERT(AbilityIcons[lp]);
	}
	RObjGadgetClass::Set_Render_Obj(SmallBttn.Peek());
}

// 0x6636E0
Unknown664500Class::~Unknown664500Class()
{
	RObjGadgetClass::Set_Render_Obj(NULL);
	Unknown100->Remove();
}

// 0x663950
bool Unknown664500Class::Unknown_30(float x, float y)
{
	if (RObjGadgetClass::Unknown_30(x, y)) {
	bool changed = false;
	if (Unknown88 != -1) {
		if (Unknown_496960(Unknown8C[Unknown88].Peek(), x, y, Layer)) {
			return(true);
		}
		Unknown88 = -1;
		changed = true;
	}
	RefCountPtr<RenderObjClass> dot;
	for (int lp = 0; lp < Unknown108; lp++) {






		FAIL_IF(lp >= MAX_SKILLS) break;
		dot = Unknown8C[lp];
		if (dot && Unknown_496960(dot.Peek(), x, y, Layer)) {
			Unknown88 = lp;
			changed = true;
			break;
		}
	}
	if (changed) {
		Unknown_664C60();
	}
	return(true);
	} else {
		if (Unknown88 != -1) {
			Unknown88 = -1;
			Unknown_664C60();
		}
		return(false);
	}
}

// 0x663B50
RenderObjClass *Unknown664500Class::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	if (Unknown88 != -1) {
		*position = Unknown8C[Unknown88]->Get_Position();
		return(Unknown8C[Unknown88].Peek());
	}
	return(RObjGadgetClass::Unknown_24(position, frame, indicator));
}

// 0x663BE0
void Unknown664500Class::Unknown_A4(bool arg)
{

	FAIL_IF(!AttachTo) return;
	Unknown6A7A00Class &data = *SkillData;
	if (!arg && !data.Unknown50.Unknown_6A5470(Unknown6C)) {
		return;
	}
	Unknown6A5640Struct *info = (*SkillData).Unknown50.Unknown_6A5640(Unknown6C);
	Unknown104 = data.Unknown50.Unknown_6A54B0(Unknown6C);
	Unknown108 = data.Unknown50.Unknown_6A59E0(Unknown6C);
	Unknown10C = info->Unknown04 == 0;
	if (info->Unknown00 != 4) {
		Unknown10C = false;
	}
	if (Unknown104 > 0) {
		if (Unknown10D) {
			LargeDarken->Set_Hidden(true);
		} else {
			SmallDarken->Set_Hidden(true);
		}
	} else {
		if (Unknown10D) {
			LargeDarken->Set_Hidden(false);
		} else {
			SmallDarken->Set_Hidden(false);
		}
	}
	UpgradeRObj->Set_Hidden(!Unknown10C);
	Unknown_6649F0();
	Unknown_664C60();
}

// 0x663DA0
void Unknown664500Class::Unknown_28(LayerClass *layer)
{
	for (std::vector<Unknown670C90Class *>::iterator it = UnknownF0.begin(); it != UnknownF0.end(); it++) {
		(*it)->Unknown_28(layer);
	}
	RObjGadgetClass::Unknown_28(layer);
}

// 0x663DF0
int Unknown664500Class::Unknown_663DF0(UIAttrbSkillsClass *ui, int skill, int index)
{















	FAIL_IF(!ui) return(-1);


	FAIL_IF(AttachTo) return(-1);
	FAIL_IF(SkillData) return(-1);

	FAIL_IF(!LargeBttn) return(-1);
	FAIL_IF(!SmallBttn) return(-1);
	FAIL_IF(!LargeDarken) return(-1);
	FAIL_IF(!SmallDarken) return(-1);
	AttachTo = ui->InterfaceMesh;

	FAIL_IF(!AttachTo) return(-1);
	SkillData = Create_Peek(ui->Unknown_62D710());
	Unknown6C = skill;
	Unknown100->Change_Text(Unknown_818200(skill), 0);
	const Unknown8155B0Struct &skill_a = Unknown_8155B0(skill);
	int count = 0;
	Unknown110 = skill_a.Unknown10;
	std::vector<Unknown815320Struct *>::const_iterator it = skill_a.Abilities.begin();
	for (int lp = 0; lp < 7; lp++) {
		RefCountPtr<RenderObjClass> a_robj = AbilityIcons[lp];
		RefCountPtr<RenderObjClass> box_robj = AbilityIconBox[lp];









		FAIL_IF(!a_robj) continue;
		FAIL_IF(!box_robj) continue;
		a_robj->Set_Hidden(true);
		box_robj->Set_Hidden(true);
		if (it == skill_a.Abilities.end()) {
			continue;
		}
		Unknown815320Struct *ability = *it;
		if (ability) {
			if (ability->Unknown1C && ability->Unknown18 <= Unknown_8185C0(Unknown6C, index)) {
				Unknown670C90Class *gadget = ui->Unknown_AC(a_robj.Peek(), &Unknown6C);
				if (gadget) {
					UnknownF0.push_back(gadget);
					count++;
					Unknown6A7380Class *skill_render = Unknown_6A7380(gadget);
					if (skill_render) {
						skill_render->Unknown_6A72E0(ability->Unknown10, 1);
					} else {











						G_ASSERT(skill_render);
					}
				} else {
					G_ASSERT(gadget);
				}
			}
		} else {

			G_ASSERT(ability);
		}
		it++;
	}



	G_ASSERT(it == skill_a.Abilities.end());
	if (count != Unknown110) {
		Unknown110 = count;
	}
	return(Unknown110);
}

// 0x664430
void Unknown664500Class::Unknown_664430()
{
	Unknown_6647F0();
	AttachTo = NULL;
	SkillData = NULL;
	Unknown6C = -1;
	Unknown110 = 0;
	UnknownF0.clear();
}

// 0x664500
void Unknown664500Class::Unknown_664500()
{
	for (std::vector<Unknown670C90Class *>::iterator it = UnknownF0.begin(); it != UnknownF0.end(); it++) {
		Unknown670C90Class *gadget = *it;
		if (gadget->Unknown_60()) {
			Unknown815320Struct *ability = Unknown_815320(Unknown_6A7380(gadget)->Get_34());
			bool on = ability->Get_18() <= (unsigned int)Unknown104;
			if (on != gadget->Unknown68) {
				gadget->Unknown_670C90(on);
				gadget->Unknown_66C2D0();
				gadget->Unknown_66C360();
			}
		}
	}
}

// 0x6645A0
int Unknown664500Class::Unknown_6645A0(int level, bool arg)
{
	if (Unknown88 != -1) {
		Unknown88 = -1;
		Unknown_664C60();
	}














	FAIL_IF(!AttachTo) return(0);
	char bone[0x40];
	_snprintf(bone, 0x40, "FILL_BONE%02d", level);
	AttachTo->Add_Sub_Object_To_Bone(Unknown70.Peek(), bone);
	Unknown10D = Unknown110 && arg;
	LargeBttn->Set_Hidden(!Unknown10D);
	SmallBttn->Set_Hidden(Unknown10D);
	Set_Render_Obj(Unknown10D ? LargeBttn.Peek() : SmallBttn.Peek());
	LargeDarken->Set_Hidden(true);
	SmallDarken->Set_Hidden(true);
	Unknown_664890(false);
	Unknown_A4(true);
	if (arg) {
		int lp = 0;
		for (std::vector<Unknown670C90Class *>::iterator it = UnknownF0.begin(); it != UnknownF0.end(); it++) {
			Unknown670C90Class *gadget = *it;
			AbilityIconBox[lp++]->Set_Hidden(false);
			gadget->Unknown_40(true);
			gadget->Unknown_8C(false);
			Unknown6A7380Class *skill_render = Unknown_6A7380(gadget);











			FAIL_IF(!skill_render) continue;
			Unknown815320Struct *ability = Unknown_815320(skill_render->Get_34());
			gadget->Unknown_670C90(ability->Get_18() <= (unsigned int)Unknown104);
		}
	}
	Unknown_66B770Class::Unknown_40(true);
	return(Unknown110);
}

// 0x6647F0
void Unknown664500Class::Unknown_6647F0()
{
	if (Unknown_60()) {
		Unknown_66B770Class::Unknown_40(false);
		int lp = 0;
		for (std::vector<Unknown670C90Class *>::iterator it = UnknownF0.begin(); it != UnknownF0.end(); it++) {
			Unknown670C90Class *gadget = *it;
			gadget->Unknown_40(false);
			gadget->Unknown_8C(true);
			gadget->Unknown_670C90(false);
			AbilityIconBox[lp++]->Set_Hidden(true);
		}
		Unknown70->Remove();
	}
}

// 0x664890
void Unknown664500Class::Unknown_664890(bool on)
{
	RefCountPtr<TextureClass> texture;
	if (on) {
		texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("I_BLANK_SEL.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	} else {
		texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("co0101.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	}
	if (texture) {
		Unknown_90(texture.Peek());
	}
}

// 0x6649F0
void Unknown664500Class::Unknown_6649F0()
{
	int lp;
	for (lp = 0; lp < Unknown104; lp++) {
		if (Unknown8C[lp]) {
			Unknown_664AE0(lp, 1);
			Unknown8C[lp]->Set_Hidden(false);
		}
	}
	for (lp = Unknown104; lp < Unknown108; lp++) {
		if (Unknown8C[lp]) {
			Unknown_664AE0(lp, 0);
			Unknown8C[lp]->Set_Hidden(false);
		}
	}
	for (lp = Unknown108; lp < MAX_SKILLS; lp++) {
		if (Unknown8C[lp]) {
			Unknown8C[lp]->Set_Hidden(true);
		}
	}
}

// 0x664AE0
void Unknown664500Class::Unknown_664AE0(int index, int state)
{
	RefCountPtr<TextureClass> texture;
	if (state == 0) {
		texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("IF_DOT00.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	} else if (state == 1) {
		texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("IF_DOT01.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	}
	if (texture) {
		Unknown_495FD0(Unknown8C[index].Peek(), texture.Peek(), 0);
	}
}

// 0x664C60
void Unknown664500Class::Unknown_664C60()
{
	std::string text;
	if (Unknown88 == -1) {
		char buffer[256];
		if (Unknown104 > 0) {
			_snprintf(buffer, 256, "Level %d %s", Unknown104, Unknown110 ? "Activated Skill" : "Passive Skill");
		} else {
			_snprintf(buffer, 256, "Unavailable %s", Unknown110 ? "Activated Skill" : "Passive Skill");
		}
		text = buffer;
		SplitStringClass split(Unknown_818250(Unknown6C), (float)Unknown_B0804C, 0);
		for (unsigned int lp = 0; lp < split.Count(); lp++) {
			text += split.Get(lp);
			text += "\n";
		}
		text += "Click for description";
	} else {
		text = Unknown_8182A0(Unknown6C, Unknown88 + 1);
	}
	Set_Unknown14(text.c_str());
}

// The client's 40 (0xB0804C, .rdata just after a hash table's primes; only 0x664D5B reads it, fild): defined after its use so
// that the read isn't folded. Its real object isn't known: interim here (D137).
extern const int Unknown_B0804C = 40;

/*
**	Notes (AS1, 2026-11-06)
**
**	Placed by the client's __FILE__ 'e:\build\beta\G\app\Client\gd_attrb_skills.cpp' (0xB8CCCC) on the asserts of the
**	constructor 0x662DC0 (lines 79-104), 0x663950 (136), 0x663BE0 (171), 0x663DF0 (228-304) and 0x6645A0 (360, 391), and
**	for the unasserted 0x664890 / 0x664AE0 / 0x664C60 by their literals in the same block ('co0101.tga' .. 'Click for
**	description', 0xB8CE1C-0xB8CEB0, ending before 'index >= Elements.size()', gd_buffgadg.cpp's). The object runs from
**	0x662DC0 to the static's initialiser 0x665120 (gd_buffgadg.cpp's notes); 0x6650B0 / 0x6650E0 are Create_Peek and
**	~RefCountPtr of the skill data.
**
**	Forms: 0x663DF0's asserts run backwards (289, 292, 297) because each is the else of its if. 0x663BE0 keeps *SkillData
**	in a reference across 0x6A5640's own operator *. 0x6645A0 / 0x6647F0 walk the icon boxes by an index (post-increment
**	inside the subscript: `mov eax, ebx; add ebx, 4`), a pointer starts it outside the empty test; ability->Get_18() and
**	skill_render->Get_34() are inline accessors because they choose the registers.
*/
