// Earth & Beyond client (reconstruction): app\common\weapontype.cpp
// Client 0x84C5D0-0x84C69D: the whole object (a weapon type's converters). Notes at the end of the file.
#include "weapontype.h"
#include <string.h>
#include "..\shared\unknown_debug.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

static char *WeaponTypeNames[] = { "Beam", "Projectile", "Missile", "Area Effect" };	// 0xBB1358  // NAME 1: what it holds






















// 0x84C5D0
bool String_To_Value(const char *text, WeaponType *value)
{
	for (int i = WEAPON_TYPE_FIRST; i < MAX_TYPE_WEAPON; i++) {
		if (stricmp(text, WeaponTypeNames[i]) == 0) {
			*value = (WeaponType)i;
			return(true);
		}
	}
	G_ASSERT(0);
	return(false);
}

// 0x84C640
char *Value_To_String(char *buf, int size, WeaponType a)
{
	FAIL_IF(a < WEAPON_TYPE_FIRST) return(buf);
	FAIL_IF(a >= MAX_TYPE_WEAPON) return(buf);
	Unknown_4A1590(buf, size, WeaponTypeNames[a]);
	return(buf);
}

/*
**	Notes (reconstruction)
**
**	The asserts sit on the client's lines (41; 48 / 49). Nothing in the client calls the two
**	functions (their incremental thunks have no references either); the file has no Type_Name.
*/
