// Earth & Beyond client (reconstruction): app\shared\damtype.h, the damage types (damtype.cpp, client 0x49D2D0-0x49E5BF).
// The header's name follows damtype.cpp's __FILE__ (no string names the header).
#ifndef DAMTYPE_H
#define DAMTYPE_H

#include <string>

// The damage types: the INI sections 'IMPACT' ... 'CHEMICAL' of 0x49D340, the names 'Impact' ... 'Effect' of table
// 0xB727E8; DAMTYPE_FIRST and DAMTYPE_COUNT are the client's (FAIL_IF 'damage_type < DAMTYPE_FIRST', line 313)
enum {
	DAMTYPE_FIRST = 0,
	DAMTYPE_IMPACT = DAMTYPE_FIRST,		// NAME 2: 'IMPACT' / 'Impact'
	DAMTYPE_EXPLOSIVE,					// NAME 2: 'EXPLOSIVE'
	DAMTYPE_PLASMA,						// NAME 2: 'PLASMA'
	DAMTYPE_ENERGY,						// NAME 2: 'ENERGY'
	DAMTYPE_EMP,						// NAME 2: 'EMP'
	DAMTYPE_CHEMICAL,					// NAME 2: 'CHEMICAL'
	DAMTYPE_ABSOLUTE,					// NAME 2: 'Absolute'
	DAMTYPE_SYSTEM,						// NAME 2: 'System'
	DAMTYPE_EFFECT,						// NAME 2: 'Effect'
	DAMTYPE_COUNT						// 9 (FAIL_IF 'dam_type >= DAMTYPE_COUNT', line 97)
};

// The damage marks: table 0xB72834 ('None', 'Spot', 'Line', 'Blotch'); DAMAGE_MARK_TYPE_BLOTCH is the client's (line 349)
enum {
	DAMAGE_MARK_TYPE_NONE,				// NAME 2: 'None'
	DAMAGE_MARK_TYPE_SPOT,				// NAME 2: 'Spot'
	DAMAGE_MARK_TYPE_LINE,				// NAME 2: 'Line'
	DAMAGE_MARK_TYPE_BLOTCH,
	DAMAGE_MARK_TYPE_COUNT				// NAME 1: the lookup's bound (0x49E340)
};

// The damage spreads: table 0xB72848 ('One_System', 'One_Quadrant'); DAMAGE_SPREAD_ONE_QUADRANT is the client's (line 384)
enum {
	DAMAGE_SPREAD_ONE_SYSTEM,			// NAME 2: 'One_System'
	DAMAGE_SPREAD_ONE_QUADRANT,
	DAMAGE_SPREAD_COUNT					// NAME 1: the lookup's bound (0x49E480)
};

void Load_Damage_Types(const char *filename);		// 0x49D340 (a section a type; no caller in the client)  // NAME 1: what it does; NAME 1 [filename]: INIClass::Load's
void Free_Damage_Types();							// 0x49D760 (no caller)  // NAME 1: what it does
int Get_Damage_Spread(int dam_type);				// 0x49D7D0 (no caller)  // NAME 2: 'DamageSpread'
float Get_Shield_Damage_Modifier(int dam_type);		// 0x49D870 (no caller)  // NAME 2: 'ShieldDamageModifier'
float Get_System_Damage_Modifier(int dam_type);		// 0x49D930 (no caller)  // NAME 2: 'SystemDamageModifier'
float Get_Organic_Damage_Modifier(int dam_type);	// 0x49D9F0 (no caller)  // NAME 2: 'OrganicDamageModifier'
float Get_Interruption_Modifier(int dam_type);		// 0x49DAB0 (no caller)  // NAME 2: 'InterruptionModifier'
float Get_Damage_Volatility(int dam_type);			// 0x49DB70 (no caller)  // NAME 2: 'DamageVolatility'
float Get_Volatility_Multiplier(int dam_type);		// 0x49DC30 (no caller)  // NAME 2: 'volatility_multiplier' (FAIL_IF, line 194)
float Get_Application_Interval(int dam_type);		// 0x49DDF0 (no caller)  // NAME 2: 'ApplicationInterval'
float Get_Application_Percentage(int dam_type);		// 0x49DEB0 (no caller)  // NAME 2: 'ApplicationPercentage'
int Has_Application_Interval(int dam_type);			// 0x49DF70 (an int, 1 or 0; no caller)  // NAME 1: what it tests
const char *Get_Resist_Name(int dam_type);			// 0x49E030 ('ResistImpact' ...; no caller)  // NAME 1: what it returns
bool Damage_Type_From_Name(const char *name, int *dam_type);	// 0x49E0D0 (no caller)  // NAME 1: what it does; NAME 1 [name]: neutral
char *Damage_Type_Name(char *buffer, int size, int a);	// 0x49E150 ('BadDamageType'; no caller)  // NAME 1: what it does; NAME 1 [buffer, size]: Bounded_Strcpy's
std::string Unknown_49E210(int damage_type);		// 0x49E210 (a damage type's name, "" for the last three; UIItemExaminationTitleClass's)  // NAME 1: neutral (D10)
bool Damage_Mark_Type_From_Name(const char *name, int *type);	// 0x49E340 (no caller)  // NAME 1: what it does; NAME 1 [name, type]: neutral
char *Damage_Mark_Type_Name(char *buffer, int size, int a);	// 0x49E3C0 ('BadDamageMarkType'; no caller)  // NAME 1: what it does; NAME 1 [buffer, size]: Bounded_Strcpy's
bool Damage_Spread_From_Name(const char *name, int *spread);	// 0x49E480 (inlined in 0x49D460)  // NAME 1: what it does; NAME 1 [name, spread]: neutral
char *Damage_Spread_Name(char *buffer, int size, int a);	// 0x49E500 ('BadDamageSpreadType'; no caller)  // NAME 1: what it does; NAME 1 [buffer, size]: Bounded_Strcpy's

#endif
