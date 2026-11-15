// Earth & Beyond client (reconstruction): app\common\group_common.cpp. Notes at the end of the file.
#include "group_common.h"
#include <string.h>
#include "..\shared\unknown_4a1590.h"
#include "..\shared\g_assert.h"

// 0x83D6A0
char *Value_To_String(char *buf, int size, GroupAccessType value)
{
	switch (value) {
		case GROUP_ACCESS_GROUP:
			return(Bounded_Strcpy(buf, size, "group"));
		case GROUP_ACCESS_RESTRICTED:
			return(Bounded_Strcpy(buf, size, "restricted"));
		case GROUP_ACCESS_PUBLIC:
			return(Bounded_Strcpy(buf, size, "public"));
		default:
			G_ASSERT(0);
			return(Bounded_Strcpy(buf, size, "private"));
	}
}

// 0x83D740
bool String_To_Value(const char *text, GroupAccessType *value)
{
	if (!stricmp(text, "restricted")) {
		*value = GROUP_ACCESS_RESTRICTED;
		return(true);
	}
	if (!stricmp(text, "public")) {
		*value = GROUP_ACCESS_PUBLIC;
		return(true);
	}
	if (!stricmp(text, "group")) {
		*value = GROUP_ACCESS_GROUP;
		return(true);
	}
	return(false);
}

/*
**	Reconstruction notes (group_common.cpp, client 0x83D6A0-0x83D7A5; __FILE__ 0xBA9A84, G_ASSERT '0' on the client's
**	line 18 in 0x83D6A0).
**
**	- The object follows game_action_validity.cpp (its converters end before 0x83D6A0) and ends with 0x83D740's ret
**	  (0x83D7A5, then nop / int3 padding).
**	- The two converters follow strconv_lib's pattern (Value_To_String / String_To_Value). No client code calls them, so
**	  the enum's name is from its strings (NAME 2); 'private' is only Value_To_String's fallback (String_To_Value has
**	  no 'private').
*/
