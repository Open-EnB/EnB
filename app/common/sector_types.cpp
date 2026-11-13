// Earth & Beyond client (reconstruction): app\Common\sector_types.cpp (__FILE__ 0xBB0B04). Notes at the end of the file.
#include <string>
#include <string.h>
#include "sector_types.h"
#include "..\shared\unknown_4a1590.h"
#include "..\shared\g_assert.h"

char *SectorTypeNames[SECTOR_COUNT] = {	// 0xBB0AC0  // NAME 1: what it holds
	"SectorSpace",
	"SectorPlanet",
	"SectorGasGiant",
};




















// 0x84A2A0
bool String_To_Value(const char *buf, SectorType *s)
{
	FAIL_IF(!buf) return(false);
	for (int i = 0; i < SECTOR_COUNT; i++) {
		if (stricmp(buf, SectorTypeNames[i]) == 0) {
			*s = (SectorType)i;
			return(true);
		}
	}
	return(false);
}



// 0x84A320
char *Value_To_String(char *buf, int size, SectorType s)
{
	FAIL_IF(!buf) return(NULL);
	return(Bounded_Strcpy(buf, size, Sector_Type_Name(s)));
}



// 0x84A3B0
char *Sector_Type_Name(int s)
{
	FAIL_IF(s < SECTOR_SPACE || s >= SECTOR_COUNT) return("InvalidSectorType");
	return(SectorTypeNames[s]);
}

// 0x84A3F0
UnknownSectorPairClass::UnknownSectorPairClass() :
	Unknown00(0),
	Unknown04(0)
{
}

// 0x84A410
UnknownSectorPairClass::UnknownSectorPairClass(int a, int b) :
	Unknown00(a),
	Unknown04(b)
{
}

/*
**	Reconstruction notes
**
**	The object runs from recustomization_cost.cpp's guarded <string> initialiser (0x84A270) to its own
**	(0x84A440); the table lookup after it (0x84A470, the names 'NotFound' .. 'Trainable' of 0xBB0B80) and
**	0x84A4C0 are skill_level_availability.cpp's (its __FILE__ 0xBB0C44 and its strings follow the table).
**	The FAIL_IFs pin lines 36, 51 and 60; the name (0x84A3B0) is inlined into Value_To_String (its line 60
**	stays there). The two-word class has no caller in the client and no name.
*/
