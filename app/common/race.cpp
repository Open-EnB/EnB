// Earth & Beyond client (reconstruction): race.cpp (the client's __FILE__ e:\build\beta\G\app\Common\race.cpp),
// client 0x848FF0-0x8492A6: PARTIAL: only the converters; the race costs after them (0x8492F0-0x849B50, asserts 'Costs.Per_Cost[slot] == 0') are not rebuilt.
// The race names in client order: Terran, Jenquai, Progen, Vrix, Grey, Dark Vrix, Elendri, Game, Other (0xBB06E0).
// Each type's Type_Name / String_To_Value / Value_To_String, as the key/value pairs call them
// (KVPairTemplateClass<RaceType> in shipcust_desc.cpp); the bit field's text joins the names with '|'.
#include "race.h"
#include <string.h>
#include "..\shared\unknown_debug.h"

static char *RaceNames[] = { "Terran", "Jenquai", "Progen", "Vrix", "Grey", "Dark Vrix", "Elendri", "Game", "Other" };	// 0xBB06E0  // NAME 1: what it holds

// 0x848FF0
const char *Type_Name(RaceType)
{
	return("RaceType");
}

// 0x849010
bool String_To_Value(const char *text, RaceType *value)
{
	for (int i = 0; i < (int)(sizeof(RaceNames) / sizeof(RaceNames[0])); i++) {
		if (stricmp(text, RaceNames[i]) == 0) {
			*value = (RaceType)i;
			return(true);
		}
	}
	return(false);
}

// 0x849060
char *Value_To_String(char *buf, int size, RaceType value)
{
	strncpy(buf, RaceNames[value], size);
	return(buf);
}

// 0x8490A0
const char *Type_Name(RaceBitFieldType)
{
	return("RaceBitFieldType");
}

// 0x8490C0
bool String_To_Value(const char *text, RaceBitFieldType *value)
{
	char copy[256];

	strcpy(copy, text);
	int bits = 0;
	char *token = strtok(copy, "|");
	while (token != NULL) {
		RaceType race;
		if (!String_To_Value(token, &race)) {
			return(false);
		}
		bits |= 1 << race;
		token = strtok(NULL, "|");
	}
	*value = (RaceBitFieldType)bits;
	return(true);
}






















































// 0x8491B0
char *Value_To_String(char *buf, int size, RaceBitFieldType value)
{
	buf[0] = 0;
	for (int i = 0; i < (int)(sizeof(RaceNames) / sizeof(RaceNames[0])); i++) {
		if (value & (1 << i)) {
			int length = strlen(RaceNames[i]) + 2;
			if (size < length) {
				G_ASSERT(!"Not enough space to create RaceBitFieldType");
				break;
			}
			strcat(buf, RaceNames[i]);
			strcat(buf, "|");
			size -= length;
		}
	}
	if (buf[0] != 0) {
		buf[strlen(buf) - 1] = 0;
	}
	return(buf);
}
