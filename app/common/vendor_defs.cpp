// Earth & Beyond client (reconstruction): app\common\vendor_defs.cpp
// Client 0x84C270-0x84C42C: the whole object (the vendor's and the ambient character's type converters).
// Notes at the end of the file.
#include "vendor_defs.h"
#include <string.h>
#include "..\shared\unknown_debug.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

static char *VendorTypeNames[] = { "Weapon", "System", "Core", "Consumable", "Junk", "Component", "Resource", "BlackMarket" };	// 0xBB10D8  // NAME 1: what it holds
static char *AmbientTypeNames[] = { "None", "Npc", "Vendor", "Bartender" };	// 0xBB10F8  // NAME 1: what it holds














// 0x84C270
const char *Type_Name(VendorTypeEnum *)
{
	return("VendorTypeEnum");
}

// 0x84C290
bool String_To_Value(const char *text, VendorTypeEnum *value)
{
	for (int i = 0; i < sizeof(VendorTypeNames) / sizeof(VendorTypeNames[0]); i++) {
		if (stricmp(text, VendorTypeNames[i]) == 0) {
			*value = (VendorTypeEnum)i;
			return(true);
		}
	}
	return(false);
}

// 0x84C2E0
char *Value_To_String(char *buf, int size, VendorTypeEnum r)
{
	FAIL_IF(r <= VENDOR_TYPE_INVALID) return("invalid");
	FAIL_IF(r >= VENDOR_TYPE_NUM) return("invalid");
	return(Unknown_4A1590(buf, size, VendorTypeNames[r]));
}













// 0x84C360
const char *Type_Name(AmbientTypeEnum *)
{
	return("AmbientTypeEnum");
}

// 0x84C380
bool String_To_Value(const char *text, AmbientTypeEnum *value)
{
	for (int i = 0; i < sizeof(AmbientTypeNames) / sizeof(AmbientTypeNames[0]); i++) {
		if (stricmp(text, AmbientTypeNames[i]) == 0) {
			*value = (AmbientTypeEnum)i;
			return(true);
		}
	}
	return(false);
}

// 0x84C3D0
char *Value_To_String(char *buf, int size, AmbientTypeEnum r)
{
	FAIL_IF(r <= AMBIENT_TYPE_INVALID) return("invalid");
	FAIL_IF(r >= AMBIENT_TYPE_NUM) return("invalid");
	return(Unknown_4A1590(buf, size, AmbientTypeNames[r]));
}

/*
**	Notes (reconstruction)
**
**	The FAIL_IFs sit on the client's lines (47 / 48, 85 / 86); the blank lines keep them there.
**	The object's data (0xBB10D8-0xBB1210): the two name tables, then the strings. Nothing in the
**	client calls the six functions (their incremental thunks have no references either): they are
**	the key/value pairs' converters for the two types, kept by the linker. The search loops
**	compare unsigned (jb): the bound is a sizeof.
*/
