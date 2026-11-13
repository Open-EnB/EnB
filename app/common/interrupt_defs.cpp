// Earth & Beyond client (reconstruction): app\Common\interrupt_defs.cpp (__FILE__ 0xBAE148). Notes at the end of the file.
#include <string.h>
#include "interrupt_defs.h"
#include "..\shared\unknown_4a1590.h"
#include "..\shared\g_assert.h"

static const char *_InterruptStateNames[] = {	// 0xBAE0B8  // NAME 1: what it holds
	"None",
	"Powering Up",
	"Interrupted",
	"Interrupt Failed",
	"Activated",
	"Deactivated",
};
static const int num_states = sizeof(_InterruptStateNames) / sizeof(_InterruptStateNames[0]);	// the client's FAIL_IF text 'idx >= num_states'





// 0x83D9E0
char *Value_To_String(char *buf, int size, InterruptStateType value)
{
	FAIL_IF(!buf) return(NULL);
	int idx = (int)value;	// the client's FAIL_IF texts


	FAIL_IF(idx < 0) {
		buf[0] = 0;
		return(buf);
	}
	FAIL_IF(idx >= num_states) {
		buf[0] = 0;
		return(buf);
	}
	return(Bounded_Strcpy(buf, size, _InterruptStateNames[idx]));
}
// 0x83DA80
bool String_To_Value(const char *buf, InterruptStateType *value)
{
	FAIL_IF(!buf) return(false);
	for (int i = 0; i < num_states; i++) {
		if (stricmp(buf, _InterruptStateNames[i]) == 0) {
			*value = (InterruptStateType)i;
			return(true);
		}
	}
	return(false);
}

/*
**	Reconstruction notes
**
**	The object: 0x83D9E0-0x83DAD9 (after guild_strings.cpp; item_effect_description.cpp follows at
**	0x83DB00), its data the names 0xBAE0B8, their texts, the FAIL_IF texts and the __FILE__ string
**	0xBAE148. The FAIL_IFs pin lines 24, 28, 32 and 41. The type's name is inline (Type_Name in the
**	header; the KVPair's Get_Type_Name 0x7083B0 holds the string), so the object has no Type_Name.
*/
