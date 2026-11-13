// Earth & Beyond client (reconstruction): app\shared\damtype.cpp, the damage types (client 0x49D2D0-0x49E5BF).
// Notes at the end.
#include "damtype.h"
#include "g_assert.h"
#include "unknown_4a1590.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwlib\random.h"
#include <string.h>
#include <vector>

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's

// A damage type's section (0x24 bytes: 0x49D460 news it)
struct DamageTypeInfoStruct	// NAME 1: neutral (D10)
{
	DamageTypeInfoStruct() :
		DamageType(0),
		DamageSpread(DAMAGE_SPREAD_ONE_QUADRANT),
		ShieldDamageModifier(1.0f),
		SystemDamageModifier(1.0f),
		OrganicDamageModifier(1.0f),
		DamageVolatility(0.0f),
		ApplicationInterval(0.0f),
		ApplicationPercentage(1.0f)
	{
	}
	int DamageType;					// +0x00
	int DamageSpread;				// +0x04 ('DamageSpread')
	float ShieldDamageModifier;		// +0x08
	float SystemDamageModifier;		// +0x0C
	float OrganicDamageModifier;	// +0x10
	float InterruptionModifier;		// +0x14 (not set by the constructor)
	float DamageVolatility;			// +0x18
	float ApplicationInterval;		// +0x1C
	float ApplicationPercentage;	// +0x20
};

static std::vector<DamageTypeInfoStruct *> DamageTypes;	// 0xBE34F8 (a type a slot, NULL for a missing section)  // NAME 1: neutral (D10)

static const char *DamageTypeNames[DAMTYPE_COUNT + 1] = { "Impact", "Explosive", "Plasma", "Energy", "Emp", "Chemical", "Absolute", "System", "Effect", NULL };	// 0xB727E8  // NAME 1: neutral (D10)
static const char *DamageTypeDisplayNames[DAMTYPE_COUNT] = { "Impact", "Explosive", "Plasma", "Energy", "EMP", "Chemical", "", "", "" };	// 0xB72810  // NAME 1: neutral (D10)
static const char *DamageMarkTypeNames[DAMAGE_MARK_TYPE_COUNT + 1] = { "None", "Spot", "Line", "Blotch", NULL };	// 0xB72834  // NAME 1: neutral (D10)
static const char *DamageSpreadNames[DAMAGE_SPREAD_COUNT + 1] = { "One_System", "One_Quadrant", NULL };	// 0xB72848  // NAME 1: neutral (D10)













// 0x49D460
static void Read_Section(INIClass &ini, const char *section_name, int dam_type)	// NAME 1: what it does; NAME 1 [dam_type]: the accessors'
{

	FAIL_IF(!ini.Section_Present(section_name)) {
		DamageTypes.push_back(NULL);
		return;
	}

	DamageTypeInfoStruct *info = new DamageTypeInfoStruct;

	FAIL_IF(!info) {
		DamageTypes.push_back(NULL);
		return;
	}
	info->DamageType = dam_type;
	info->ShieldDamageModifier = ini.Get_Float(section_name, "ShieldDamageModifier", 1.0f);
	info->SystemDamageModifier = ini.Get_Float(section_name, "SystemDamageModifier", 1.0f);
	info->OrganicDamageModifier = ini.Get_Float(section_name, "OrganicDamageModifier", 1.0f);
	info->InterruptionModifier = ini.Get_Float(section_name, "InterruptionModifier", 0.25f);
	info->DamageVolatility = ini.Get_Float(section_name, "DamageVolatility", 0.0f);
	info->ApplicationInterval = ini.Get_Float(section_name, "ApplicationInterval", 0.0f);
	info->ApplicationPercentage = ini.Get_Float(section_name, "ApplicationPercentage", 1.0f);
	StringClass spread;
	ini.Get_String(spread, section_name, "DamageSpread", "One_Quadrant");
	Damage_Spread_From_Name(spread, &info->DamageSpread);
	DamageTypes.push_back(info);
}








static DamageTypeInfoStruct *Get_Info(int dam_type)	// NAME 1: what it does
{
	FAIL_IF(dam_type < 0) return NULL;
	FAIL_IF(dam_type >= DAMTYPE_COUNT) return NULL;
	if ((unsigned)dam_type < DamageTypes.size()) {
		DamageTypeInfoStruct *info = DamageTypes[dam_type];


		G_ASSERT(info);
		return info;
	}
	return NULL;
}

// 0x49D340
void Load_Damage_Types(const char *filename)
{
	INIClass ini;
	if (!ini.Load(filename)) {


		G_ASSERT(0);
	} else {
		Read_Section(ini, "IMPACT", DAMTYPE_IMPACT);
		Read_Section(ini, "EXPLOSIVE", DAMTYPE_EXPLOSIVE);
		Read_Section(ini, "PLASMA", DAMTYPE_PLASMA);
		Read_Section(ini, "ENERGY", DAMTYPE_ENERGY);
		Read_Section(ini, "EMP", DAMTYPE_EMP);
		Read_Section(ini, "CHEMICAL", DAMTYPE_CHEMICAL);
	}
}

// 0x49D760
void Free_Damage_Types()
{
	for (std::vector<DamageTypeInfoStruct *>::iterator it = DamageTypes.begin(); it != DamageTypes.end(); it++) {
		delete *it;
	}
	DamageTypes.clear();
}

// 0x49D7D0
int Get_Damage_Spread(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->DamageSpread : DAMAGE_SPREAD_ONE_QUADRANT;
}
// 0x49D870
float Get_Shield_Damage_Modifier(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->ShieldDamageModifier : 1.0f;
}
// 0x49D930
float Get_System_Damage_Modifier(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->SystemDamageModifier : 1.0f;
}
// 0x49D9F0
float Get_Organic_Damage_Modifier(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->OrganicDamageModifier : 1.0f;
}
// 0x49DAB0
float Get_Interruption_Modifier(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->InterruptionModifier : 0.25f;
}
// 0x49DB70
float Get_Damage_Volatility(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->DamageVolatility : 0.0f;
}












// 0x49DC30
float Get_Volatility_Multiplier(int dam_type)
{
	float volatility = Get_Damage_Volatility(dam_type);
	FAIL_IF(volatility < 0.0f) return 1.0f;
	FAIL_IF(volatility > 1.0f) return 1.0f;
	int range = Unknown_BDBB84(0, (int)(volatility * 100.0f));	// NAME 1 [range]: neutral
	if (Unknown_BDBB84() % 2) {
		range = -range;
	}
	float volatility_multiplier = 1.0f + range * 0.01f;
	FAIL_IF(volatility_multiplier < 0.0f) return 1.0f;
	return volatility_multiplier;
}

// 0x49DDF0
float Get_Application_Interval(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->ApplicationInterval : 0.0f;
}

// 0x49DEB0
float Get_Application_Percentage(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	return info ? info->ApplicationPercentage : 1.0f;
}

// 0x49DF70
int Has_Application_Interval(int dam_type)
{
	DamageTypeInfoStruct *info = Get_Info(dam_type);
	if (info) {
		return info->ApplicationInterval != 0.0f;
	}
	return 0;
}








// 0x49E030
const char *Get_Resist_Name(int dam_type)
{
	switch (dam_type) {
		case DAMTYPE_IMPACT:
			return "ResistImpact";
		case DAMTYPE_EXPLOSIVE:
			return "ResistExplosive";
		case DAMTYPE_PLASMA:
			return "ResistPlasma";
		case DAMTYPE_ENERGY:
			return "ResistEnergy";
		case DAMTYPE_EMP:
			return "ResistEMP";
		case DAMTYPE_CHEMICAL:
			return "ResistChemical";
		case DAMTYPE_ABSOLUTE:
		case DAMTYPE_SYSTEM:
		case DAMTYPE_EFFECT:
		case DAMTYPE_COUNT:
			break;
		default:
			G_ASSERT(0);
			break;
	}
	return NULL;
}



















// 0x49E0D0
bool Damage_Type_From_Name(const char *name, int *dam_type)
{
	for (int i = 0; i < DAMTYPE_COUNT; i++) {
		if (stricmp(name, DamageTypeNames[i]) == 0) {
			*dam_type = i;
			return true;
		}
	}
	G_ASSERT(0);
	return false;
}

// 0x49E150
char *Damage_Type_Name(char *buffer, int size, int a)
{
	FAIL_IF(a < 0) return Bounded_Strcpy(buffer, size, "BadDamageType");
	FAIL_IF(a >= DAMTYPE_COUNT) return Bounded_Strcpy(buffer, size, "BadDamageType");
	Bounded_Strcpy(buffer, size, DamageTypeNames[a]);
	return buffer;
}














// 0x49E210
std::string Unknown_49E210(int damage_type)
{
	FAIL_IF(damage_type < DAMTYPE_FIRST) return "";
	FAIL_IF(damage_type >=DAMTYPE_COUNT) return "";
	return DamageTypeDisplayNames[damage_type];
}















// 0x49E340
bool Damage_Mark_Type_From_Name(const char *name, int *type)
{
	for (int i = 0; i < DAMAGE_MARK_TYPE_COUNT; i++) {
		if (stricmp(name, DamageMarkTypeNames[i]) == 0) {
			*type = i;
			return true;
		}
	}
	G_ASSERT(0);
	return false;
}

// 0x49E3C0
char *Damage_Mark_Type_Name(char *buffer, int size, int a)
{
	FAIL_IF(a < 0) return Bounded_Strcpy(buffer, size, "BadDamageMarkType");
	FAIL_IF(a > DAMAGE_MARK_TYPE_BLOTCH) return Bounded_Strcpy(buffer, size, "BadDamageMarkType");
	Bounded_Strcpy(buffer, size, DamageMarkTypeNames[a]);
	return buffer;
}














// 0x49E480
bool Damage_Spread_From_Name(const char *name, int *spread)
{
	for (int i = 0; i < DAMAGE_SPREAD_COUNT; i++) {
		if (stricmp(name, DamageSpreadNames[i]) == 0) {
			*spread = i;
			return true;
		}
	}
	G_ASSERT(0);
	return false;
}

// 0x49E500
char *Damage_Spread_Name(char *buffer, int size, int a)
{
	FAIL_IF(a < 0) return Bounded_Strcpy(buffer, size, "BadDamageSpreadType");
	FAIL_IF(a > DAMAGE_SPREAD_ONE_QUADRANT) return Bounded_Strcpy(buffer, size, "BadDamageSpreadType");
	Bounded_Strcpy(buffer, size, DamageSpreadNames[a]);
	return buffer;
}

// Reconstruction notes (O13, run 2026-11-12b)
// - The object: __FILE__ e:\build\beta\G\app\shared\damtype.cpp (0xB72928), its asserts and FAIL_IFs on the client's
//   lines (62 / 69 the section reader, 96 / 97 / 102 Get_Info inlined in every accessor, 115, 187 / 188 / 194, 251, 284,
//   291 / 292, 313 / 314, 341, 348 / 349, 376, 383 / 384). It starts with the static vector's initialiser 0x49D2D0 and
//   its atexit destructor 0x49D310 (the vector 0xBE34F8, which every function here reads), after the previous object's
//   template instances 0x49D260 / 0x49D2A0; it ends with vector<DamageTypeInfoStruct *>::insert 0x49E5C0 (push_back's
//   out-of-line insert) and the guarded ctype initialiser 0x49E860; observer.cpp follows at 0x49E890.
// - The section reader 0x49D460 is a file static defined before Load_Damage_Types and emitted right after it, its first
//   caller (workflow.md 3.1.2); Damage_Spread_From_Name (line 376) is inlined in it. 0x49D760 is placed by position
//   (no assert): it frees the vector the file owns.
// - Nothing in the client calls Load_Damage_Types or the accessors; only Unknown_49E210 has a caller
//   (UIItemExaminationTitleClass::Set_Subtitles 0x53A2F0). The tables: 0xB727E8 (the names, NULL-ended), 0xB72810 (the
//   display names, "" for the last three), 0xB72834 (the marks, NULL-ended), 0xB72848 (the spreads, NULL-ended).
// - Has_Application_Interval returns an int (mov eax, 1), not a bool.
