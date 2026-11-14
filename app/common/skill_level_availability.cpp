// Earth & Beyond client (reconstruction): app\Common\skill_level_availability.cpp (__FILE__ 0xBB0C44). Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#define __SKILL_LEVEL_AVAILABILITY_ALL
#include "skill_level_availability.h"
#include "..\shared\unknown_4a1590.h"
#include "..\shared\g_assert.h"

static const char *_ClassQualificationNames[] = {	// 0xBB0B80: the client's FAIL_IF text
	"NotFound",
	"NotAvailable",
	"Automatic",
	"QuestOnly",
	"Trainable",
};






















// 0x84A470
bool String_To_Value(const char *buf, ClassQualificationType *class_qualification)
{
	for (int i = 0; i < sizeof(_ClassQualificationNames) / sizeof(const char *); i++) {
		if (stricmp(buf, _ClassQualificationNames[i]) == 0) {
			*class_qualification = (ClassQualificationType)i;
			return(true);
		}
	}
	return(false);
}



// 0x84A4C0
char *Value_To_String(char *buf, int size, ClassQualificationType class_qualification)
{
	FAIL_IF(class_qualification < 0) return(Bounded_Strcpy(buf, size, "NotFound"));
	FAIL_IF(class_qualification >= sizeof(_ClassQualificationNames)/sizeof(const char *)) return(Bounded_Strcpy(buf, size, "NotFound"));
	return(Bounded_Strcpy(buf, size, _ClassQualificationNames[class_qualification]));
}







// 0x84A560
char *Value_To_String(char *buf, int size, const SkillLevelAvailabilityStruct &value)
{
	char tmp[256];
	sprintf(tmp, "%d,%d,%d,%d", value.I00, value.I04, value.I08, value.I0C);
	G_ASSERT((int)strlen(tmp) < size);
	return(Bounded_Strcpy(buf, size, tmp));
}

// 0x84A600
bool String_To_Value(const char *text, SkillLevelAvailabilityStruct *value)
{
	return(sscanf(text, "%d,%d,%d,%d", &value->I00, &value->I04, &value->I08, &value->I0C) == 4);
}

/*
**	Reconstruction notes
**
**	The object: 0x84A470-0x84A62D, after sector_types.cpp's guarded <string> initialiser 0x84A440 (this
**	file has none); starbase_defs.cpp follows at 0x84A640. Its data: the names 0xBB0B80, their texts, the
**	FAIL_IF / G_ASSERT texts, the __FILE__ string 0xBB0C44 and '%d,%d,%d,%d' (0xBB0C88); starbase_defs'
**	tables start at 0xBB0C98. The 4-number value's converters are called by the KVPair instances of
**	SkillLevelAvailabilityStruct (0x759D00, 0x75A190); the class qualification's have no caller. The
**	asserts pin lines 55, 56 and 71.
*/
