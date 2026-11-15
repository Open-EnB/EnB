// Earth & Beyond client (reconstruction): app\common\nav_type_enum.cpp (__FILE__ 0xBAF6F8). Notes at the end of the file.

#include "nav_type_enum.h"
#include "..\shared\g_assert.h"
#include "..\shared\unknown_4a1590.h"
#include <string.h>

static const char *NavTypeNames[] = {	// 0xBAF694
	"None",
	"Path",
	"End",
	NULL
};

// @0x846A50
const char *Type_Name(NavType)
{
	return("NavType");
}

// @0x846A70
bool String_To_Value(const char *text, NavType *v)
{
	for (int i = 0; i < NAV_TYPE_COUNT; i++) {
		if (stricmp(text, NavTypeNames[i]) == 0) {
			*v = (NavType)i;
			return(true);
		}
	}
	*v = NAV_TYPE_INVALID;
	return(false);
}




































// Line-aligned so FAIL_IF matches client __LINE__ 73 and 74.
// @0x846AE0
char *Value_To_String(char *buf, int size, NavType v)
{
	FAIL_IF(v <= NAV_TYPE_INVALID) return(Bounded_Strcpy(buf, size, "invalid"));
	FAIL_IF(v >= NAV_TYPE_COUNT) return(Bounded_Strcpy(buf, size, "invalid"));
	Bounded_Strcpy(buf, size, NavTypeNames[v]);
	return(buf);
}

/*
**	Reconstruction notes:
**
**	* Client 0x846A50-0x846B60 (__FILE__ 0xBAF6F8 e:\build\beta\G\app\common\nav_type_enum.cpp):
**	  all three functions (Type_Name 0x846A50, String_To_Value 0x846A70, Value_To_String 0x846AE0)
**	  match the client bytes exactly; lines 35-68 are blank padding so FAIL_IF expands to __LINE__ 73 and 74.
*/
