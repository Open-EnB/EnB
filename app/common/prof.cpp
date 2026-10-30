// Earth & Beyond client (reconstruction): prof.cpp (the client's __FILE__ e:\build\beta\G\app\Common\prof.cpp),
// client 0x848CA0-0x848FA0: the whole object (the template copies before 0x848CA0 are the previous object's).
// The profession names in client order: Warrior, Tradesman, Explorer, Other (0xBB05E8; 'Merchant' also reads as Tradesman).
// Each type's Type_Name / String_To_Value / Value_To_String, as the key/value pairs call them
// (KVPairTemplateClass<ProfessionType> in shipcust_desc.cpp); the bit field's text joins the names with '|'.
#include "prof.h"
#include <string.h>
#include "..\shared\unknown_debug.h"

static char *ProfessionNames[] = { "Warrior", "Tradesman", "Explorer", "Other" };	// 0xBB05E8  // NAME 1: what it holds

// 0x848CA0
const char *Type_Name(ProfessionType)
{
	return("ProfessionType");
}

// 0x848CC0
bool String_To_Value(const char *text, ProfessionType *value)
{
	for (int i = 0; i < (int)(sizeof(ProfessionNames) / sizeof(ProfessionNames[0])); i++) {
		if (stricmp(text, ProfessionNames[i]) == 0) {
			*value = (ProfessionType)i;
			return(true);
		}
	}
	if (stricmp(text, "Merchant") == 0) {
		*value = (ProfessionType)1;
		return(true);
	}
	return(false);
}

// 0x848D40
char *Value_To_String(char *buf, int size, ProfessionType value)
{
	strncpy(buf, ProfessionNames[value], size);
	return(buf);
}

// 0x848D80
const char *Type_Name(ProfessionBitFieldType)
{
	return("ProfessionBitFieldType");
}

// 0x848DA0
bool String_To_Value(const char *text, ProfessionBitFieldType *value)
{
	char copy[256];

	strcpy(copy, text);
	int bits = 0;
	char *token = strtok(copy, "|");
	while (token != NULL) {
		ProfessionType profession;
		if (!String_To_Value(token, &profession)) {
			return(false);
		}
		bits |= 1 << profession;
		token = strtok(NULL, "|");
	}
	*value = (ProfessionBitFieldType)bits;
	return(true);
}






















// 0x848EB0
char *Value_To_String(char *buf, int size, ProfessionBitFieldType value)
{
	buf[0] = 0;
	for (int i = 0; i < (int)(sizeof(ProfessionNames) / sizeof(ProfessionNames[0])); i++) {
		if (value & (1 << i)) {
			int length = strlen(ProfessionNames[i]) + 2;
			if (size < length) {
				G_ASSERT(!"Not enough space to create ProfessionBitFieldType");
				break;
			}
			strcat(buf, ProfessionNames[i]);
			strcat(buf, "|");
			size -= length;
		}
	}
	if (buf[0] != 0) {
		buf[strlen(buf) - 1] = 0;
	}
	return(buf);
}
