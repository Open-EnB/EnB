// Earth & Beyond client (reconstruction): app\client\skill_render.cpp, what draws a skill's slot
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\skill_render.cpp' at 0xB91528). Notes at the end.
#include <string>
#include <vector>
#include "..\shared\g_assert.h"
#include "skill_render.h"
#include "gd_slot_data.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "vector3.h"

class ClientObjectClass;

#include "sclient.h"	// (0x744FE0; this file's class below keeps the slot +0x28 and the time at +0x10BC, which sclient.h does not hold)
class Unknown744FE0Class { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual ClientObjectClass *Unknown_28(); char Unknown04[0x10BC - 4]; unsigned int Unknown10BC; };	// NAME 1: neutral (D10)
// skill_data.cpp's list (gd_attrb_skills.cpp's SkillDataClass): its bases sit at +0x50 and +0xB0 (the `add ecx, 0x50` after the arguments)
struct UnknownDurationSum { UnknownDurationSum(__int64 v) : Value(v) {} UnknownDurationSum operator+(const UnknownDurationSum &r) const { return(UnknownDurationSum(Value + r.Value)); } __int64 Value; };	// (added with a member operator, 0x6A6550; duration.h's class has none yet)  // NAME 2: describes what it does
class SkillDataPeekClass { public: bool Unknown_6A5470(int skill); int Get_Level(int skill); DurationClass Get_Recycle_Time(int skill); DurationClass Get_Last_Activation(int skill); char Unknown00[0x60]; };	// NAME 1: neutral (D10), as gd_attrb_skills.cpp's; NAME 1 [skill]: neutral
enum TargetGeneratorSummaryBF; enum GameActionValidityBitFlag; class SkillAbilityDataPeekClass { public: TargetGeneratorSummaryBF Get_Targets(int ability); int Get_Range(int ability); GameActionValidityBitFlag Get_Validity(int ability); };	// NAME 1: neutral (D10); NAME 1 [ability]: neutral
class Unknown6A7A00Base { public: char Unknown00[0x20]; Unknown744FE0Class *Unknown20; char Unknown24[0x2C]; };	// NAME 1: neutral (D10)
class SkillDataClass : public Unknown6A7A00Base, public SkillDataPeekClass, public SkillAbilityDataPeekClass { public: int Get_Level(int skill) { return(SkillDataPeekClass::Get_Level(skill)); } int Get_Skill_Powerup_Ability_Number(); float Unknown_6A4C20(); };	// NAME 1: neutral (D10); NAME 2 [Get_Level]: 0x6A54B0's result is compared with the abilities' levels; NAME 1 [skill]: neutral
class SlotDataClass { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual SkillDataClass *Unknown_14(); };	// NAME 3: its local sd (0x6A7310)
// gd_gauge.cpp's gauge (ui_faction.cpp's), shortcut_range_gauge.cpp's
class Unknown669570Class : public RObjGadgetClass { public: Unknown669570Class(RenderObjClass *robj, const char *name); virtual void Unknown_A4(float value); char Unknown60[0x6C - 0x60]; };	// NAME 1: neutral (D10); NAME 1 [robj, name, value]: neutral
class Unknown6A3D60Class { public: Unknown6A3D60Class(RenderObjClass *robj); ~Unknown6A3D60Class(); void Unknown_6A4020(float distance, float range, int state, bool arg); char Unknown00[0x18]; };	// NAME 1: neutral (D10); NAME 1 [robj, distance, range, state, arg]: neutral

// skill_ability_names.cpp's, skillnam.cpp's, slot_data_render.cpp's, ro_funcs.cpp's, distance_calc_client.cpp's
struct Unknown815320Struct { std::string Name; int Unknown10; int Unknown14; unsigned int Unknown18; bool Unknown1C; Vector3 Unknown20; };	// NAME 1: neutral (D10); NAME 2 [Name]: 0x6A6A80 writes it as the ability
Unknown815320Struct *Unknown_815320(int ability);	// NAME 1: neutral (D10)
struct Unknown8155B0Struct { std::vector<Unknown815320Struct *> Abilities; int NumActivatable; };	// NAME 3 [Abilities, NumActivatable]: asserts
const Unknown8155B0Struct &Unknown_8155B0(int skill);	// NAME 1: neutral (D10)
int Unknown_815260(const std::string &name);	void Unknown_815720(std::string *string, int ability, bool below, unsigned int level);	// NAME 1: neutral (D10)
class SkillNamesDatabase { public: static const char *Unknown_818200(int skill); static int Unknown_818520(const char *name); static TextureClass *Unknown_818430(int skill); };	// NAME 1: neutral (D10)
void Unknown_6A93E0(const char *value, std::string &string);	const char *Unknown_6A9660(const char *state, std::string &string);	// NAME 1: neutral (D10)
void Unknown_496730(RenderObjClass *robj, int a, bool b);	float Unknown_7E6060(ClientObjectClass *a, ClientObjectClass *b);	// NAME 1: neutral (D10)

// 0x6A5EA0
SkillRenderClass::SkillRenderClass() :
	Unknown30(0),
	Unknown34(-1),
	Unknown38(0),
	Unknown48(0),
	Unknown4C(-1.0f),
	Unknown50(0),
	Unknown54(0),
	Unknown58(false)
{
	Unknown2C = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("itemon01"));
	Unknown30 = new Unknown669570Class(Unknown2C.Peek(), "REACTOR_BAR1");
}

// 0x6A6070
SkillRenderClass::~SkillRenderClass()
{
	if (Unknown2C) {
		Unknown2C->Remove();
		Unknown2C = NULL;
	}
	if (Unknown30) {
		GadgetClass::Destroy(Unknown30);
		Unknown30 = 0;
	}
	if (ValidityIndicator) {
		ValidityIndicator->Remove();
		ValidityIndicator = NULL;
	}
	if (WrongTargetTypeIndicator) {
		WrongTargetTypeIndicator->Remove();
		WrongTargetTypeIndicator = NULL;
	}
	if (RechargeIndicator) {
		RechargeIndicator->Remove();
		RechargeIndicator = NULL;
	}
	delete Unknown48;
	Unknown48 = 0;
}



























// 0x6A6250
bool SkillRenderClass::Unknown_08(SlotDataClass *sd, RenderObjClass *robj)
{
	if (!SlotDataRenderClass::Unknown_08(sd, robj)) {
		return(false);
	}
	ValidityIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_dark"));
	WrongTargetTypeIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_noskill"));
	RechargeIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_timer"));
	G_ASSERT(ValidityIndicator);
	G_ASSERT(WrongTargetTypeIndicator);
	G_ASSERT(RechargeIndicator);

	RenderObjClass *render_object = Mesh;
	FAIL_IF(!render_object) return(false);
	Unknown48 = new Unknown6A3D60Class(render_object);
	return(true);
}















// 0x6A6550
void SkillRenderClass::Unknown_2C(bool force)
{
	SkillDataClass *skills = Unknown_6A7310();
	FAIL_IF(!skills) return;

	int slot = Unknown20;
	if (slot != -1 && skills->Unknown_6A5470(slot)) {
		force = true;
	}
	SlotDataRenderClass::Unknown_2C(force);

	if (Unknown38 == 0 || Unknown38 == 1) {
		int active = skills->Get_Skill_Powerup_Ability_Number();
		if (active == Unknown34 && Unknown34 != -1) {
			if (!Unknown2C->Get_Container()) {
				RenderObjClass *robj = Mesh;
				if (Container && robj) {
					Container->Add_Sub_Object(Unknown2C.Peek());
					Unknown_496730(Unknown2C.Peek(), robj->Get_Sort_Level() - 1, 0);
				}
			}
			Unknown30->Unknown_A4(skills->Unknown_6A4C20());
		} else {
			if (Unknown2C->Get_Container()) {
				Unknown2C->Remove();
			}
		}
	}

	Unknown4C = -1.0f;
	switch (Unknown38) {
		case 0:
		case 1:
			break;
		case 2:
			if (force) {
				Unknown_6A8FE0(ValidityIndicator.Peek());
				Unknown_6A8FE0(WrongTargetTypeIndicator.Peek());
				Unknown_6A8FE0(RechargeIndicator.Peek());
			}
			return;
		default:
			if (force) {
				Unknown_6A8FE0(ValidityIndicator.Peek());
				Unknown_6A8FE0(WrongTargetTypeIndicator.Peek());
				Unknown_6A8FE0(RechargeIndicator.Peek());












				G_ASSERT(false);
			}
			return;
	}

	Unknown744FE0Class *sclient = Unknown_6A7310()->Unknown20;

	FAIL_IF(!sclient) return;
	ClientObjectClass *a = ((SClient *)sclient)->Get_Target();
	if (a) {
		ClientObjectClass *b = sclient->Unknown_28();
		if (b) {
			Unknown4C = Unknown_7E6060(a, b);
		}
	}

	SkillDataClass *skill_data = Unknown_6A7310();







	FAIL_IF(!skill_data) return;
	SkillDataPeekClass &skill = *skill_data;
	SkillAbilityDataPeekClass &ability = *skill_data;

	unsigned int flags = ability.Get_Validity(Unknown34);
	unsigned int flags2 = ability.Get_Targets(Unknown34);
	float range = (float)ability.Get_Range(Unknown34);
	Unknown50 = range;
	if (Unknown48) {
		Unknown48->Unknown_6A4020(Unknown4C, range, (flags & 0x3F8) ? 0 : (((flags2 & 0x34) != 0) ? 2 : 0), true);
	}

	__int64 now = sclient->Unknown10BC;
	bool ready = (((const UnknownDurationSum &)skill.Get_Recycle_Time(slot) + (const UnknownDurationSum &)skill.Get_Last_Activation(slot)).Value <= now);
	if (force || flags != Unknown54 || Unknown58 != ready) {
		Unknown_6A73C0((flags & 0x170) != 0, WrongTargetTypeIndicator);
		Unknown_6A73C0(flags != 0 || !ready, ValidityIndicator);
		Unknown_6A73C0(!ready, RechargeIndicator);
		Unknown54 = flags;
		Unknown58 = ready;
	}
}

// 0x6A6A80
void SkillRenderClass::Unknown_0C(std::string &out)
{
	SlotDataRenderClass::Unknown_0C(out);
	const char *name = SkillNamesDatabase::Unknown_818200(Unknown20);
	std::string string;
	Unknown_6A93E0(name, string);
	Unknown_6A93E0(Unknown_815320(Unknown34)->Name.c_str(), string);
	out += string;
}
























// 0x6A6C00
const char *SkillRenderClass::Unknown_10(const char *serial_string)
{
	const char *state = SlotDataRenderClass::Unknown_10(serial_string);
	FAIL_IF(!state) return(0);

	std::string string;
	state = Unknown_6A9660(state, string);



	FAIL_IF(string.empty()) return(0);
	int slot = SkillNamesDatabase::Unknown_818520(string.c_str());

	FAIL_IF(slot < 0) return(0);

	state = Unknown_6A9660(state, string);





	FAIL_IF(string.empty()) return(0);
	Unknown_6A72E0(Unknown_815260(string), false);
	Unknown_40(slot);
	return(state);
}

// 0x6A6EA0
TextureClass *SkillRenderClass::Unknown_34()
{
	return(SkillNamesDatabase::Unknown_818430(Get_Slot()));
}

// 0x6A6EC0
bool SkillRenderClass::Unknown_38(Vector3 &color)
{
	unsigned int level = Unknown_6A7310()->Get_Level(Unknown20);
	color.Set(0.2f, 0.2f, 0.2f);
	if (Unknown34 >= 0) {
		Unknown815320Struct *ability = Unknown_815320(Unknown34);
		if (ability->Unknown18 <= level) {
			color.X = ability->Unknown20.X;
			color.Y = ability->Unknown20.Y;
			color.Z = ability->Unknown20.Z;
		}
	}
	return(true);
}






// 0x6A6F90
void SkillRenderClass::Unknown_44(SlotDataRenderClass *render)
{
	FAIL_IF(!render) return;

	SkillRenderClass *skill_render = render->Unknown_20();
	FAIL_IF(!skill_render) return;
	Unknown34 = skill_render->Unknown34;
	SlotDataRenderClass::Unknown_44(render);
}





// 0x6A7020
void SkillRenderClass::Unknown_40(int slot)
{
	if (slot == -1) {
		Unknown34 = -1;
	} else if (Unknown34 == -1) {
		const Unknown8155B0Struct &s_ability = Unknown_8155B0(slot);
		FAIL_IF(!s_ability.NumActivatable) return;

		SkillDataClass *skill_data = Unknown_6A7310();

		FAIL_IF(!skill_data) return;
		unsigned int level = skill_data->Get_Level(slot);
		for (std::vector<Unknown815320Struct *>::const_iterator it = s_ability.Abilities.begin(); it != s_ability.Abilities.end(); ++it) {
			Unknown815320Struct *ability = *it;
			if (ability->Unknown1C && ability->Unknown18 <= level) {
				Unknown34 = ability->Unknown10;
			}
		}
	}
	SlotDataRenderClass::Unknown_40(slot);
}









// 0x6A7150
const char *SkillRenderClass::Unknown_28(std::string *string)
{
	FAIL_IF(!string) return(0);

	SlotDataRenderClass::Unknown_28(string);
	if (Unknown38 == 1) {
		return("LOC_SHORTCUT_OPEN");
	}
	Unknown815320Struct *ability = Unknown_815320(Unknown34);
	unsigned int level = Unknown_6A7310()->Get_Level(Unknown20);
	bool enough = ability->Unknown18 <= level;
	Unknown_815720(string, ability->Unknown10, !enough, level);
	switch (Unknown38) {
		default:












			G_ASSERT(false);
			return("LOC_SKILL_LIST_NA");
		case 2:
			return(enough ? "LOC_SKILL_LIST" : "LOC_SKILL_LIST_NA");
		case 0:
			return("LOC_SHORTCUT");
	}
}

// 0x6A72C0
const char *SkillRenderClass::Unknown_4C() { return(0); }

// 0x6A72E0
void SkillRenderClass::Unknown_6A72E0(int ability, bool update)
{
	Unknown34 = ability;
	if (update) Unknown_2C(true);
}


// 0x6A7310
SkillDataClass *SkillRenderClass::Unknown_6A7310()
{
	SlotDataClass *sd = SlotData; FAIL_IF(!sd) return(0);

	SkillDataClass *skill = sd->Unknown_14();
	G_ASSERT(skill);
	return(skill);
}



// 0x6A7380
SkillRenderClass *Unknown_6A7380(SlotDataGadgetClass *gadget)
{
	SlotDataRenderClass *slot_render = gadget->SlotRender;
	FAIL_IF(!slot_render) return(0);
	return(slot_render->Unknown_20());
}

// 0x6A73C0
void SkillRenderClass::Unknown_6A73C0(bool on, const RefCountPtr<RenderObjClass> &robj)
{
	if (on) {
		Unknown_6A8F00(robj.Peek());
	} else {
		Unknown_6A8FE0(robj.Peek());
	}
}

/*
**	Reconstruction notes.
**
**	Object boundaries: <string>'s guarded initialiser ends the previous object at 0x6A5E00 and this one at 0x6A73F0.
**	The constructor 0x6A5EA0 uses the literals just below this file's __FILE__ ('REACTOR_BAR1' 0xB914EC, 'itemon01'
**	0xB914FC). 0x6A7420-0x6A7B50 is another object: it ends at its own initialiser 0x6A7B20.
**
**	0x6A7310 ('!sd', 'skill' on lines 429 and 432) is inlined into 0x6A6550, 0x6A6EC0, 0x6A7020 and 0x6A7150, and
**	0x6A73C0 three times into 0x6A6550; both are defined after those callers, as the line numbers say. The default of
**	0x6A7150's switch comes first in the source: its 'LOC_SKILL_LIST_NA' has the highest address of the three literals.
**	Forms the bytes needed: Get_Level as an inline on the skill (0x6A6EC0, 0x6A7150: `add ecx, 0x50` after the argument),
**	operator+ on the time value (0x6A6550: the load order of the sum), __int64 now before the two calls (the same),
**	if / else if with the base's call after it in 0x6A7020 (tail duplication keeps `push ebx`; an early return folds it to
**	the -1 in eax), Get_Slot in 0x6A6EA0, Unknown_6A72E0 in 0x6A6C00 (spends the inline budget that leaves the first ~string to _Tidy).
*/
