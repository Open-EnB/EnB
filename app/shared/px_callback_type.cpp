// Earth & Beyond client (reconstruction): app\shared\px_callback_type.cpp. Notes at the end of the file.
#include "px_callback_type.h"
#include "..\shared\unknown_debug.h"
#include <string.h>

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

static const char * const _EventStrings[] = {	// the client's name, the FAIL_IF l.102 (0xB70A80)
	"Enter",
	"Exit",
	"Update"
};

// 0x453740
const char *PxCallbackType_Name()
{
	return("PxCallbackType");
}

// 0x453760
bool PxCallbackType_From_String(const char *string, PxCallbackType *type)
{
	if (string) {
		if (stricmp(string, "Accurate") == 0) {
			*type = PX_CALLBACK_ACCURATE;
			return(true);
		}
		if (stricmp(string, "Liberal") == 0) {
			*type = PX_CALLBACK_LIBERAL;
			return(true);
		}
		if (stricmp(string, "Conservative") == 0) {
			*type = PX_CALLBACK_CONSERVATIVE;
			return(true);
		}
		if (stricmp(string, "Polling") == 0) {
			*type = PX_CALLBACK_POLLING;
			return(true);
		}
	}
	return(false);
}

// 0x453830
char *PxCallbackType_To_String(char *buf, int size, PxCallbackType type)
{
	FAIL_IF(!buf) return(NULL);
	switch (type) {
		case PX_CALLBACK_ACCURATE:
			Unknown_4A1590(buf, size, "Accurate"); break;
		case PX_CALLBACK_LIBERAL:
			Unknown_4A1590(buf, size, "Liberal"); break;
		case PX_CALLBACK_CONSERVATIVE:
			Unknown_4A1590(buf, size, "Conservative"); break;
		case PX_CALLBACK_POLLING:
			Unknown_4A1590(buf, size, "Polling"); break;
		default:
			return(NULL);
	}
	return(buf);
}

// 0x453900
const char *PxCallbackEvent_Name()
{
	return("PxCallbackEvent");
}

// 0x453920
bool PxCallbackEvent_From_String(const char *string, PxCallbackEvent *event)
{
	for (int i = 0; i < (int)(sizeof(_EventStrings) / sizeof(_EventStrings[0])); i++) {
		if (stricmp(string, _EventStrings[i]) == 0) {
			*event = (PxCallbackEvent)i;
			return(true);
		}
	}
	return(false);
}


















// 0x453970
char *PxCallbackEvent_To_String(char *buf, int size, int index)
{
	FAIL_IF(index < 0) return(NULL);
	FAIL_IF(index >= sizeof(_EventStrings) / sizeof(_EventStrings[0])) return(NULL);
	FAIL_IF(!buf) return(NULL);
	Unknown_4A1590(buf, size, _EventStrings[index]); return(buf);
}



// 0x453A10
void Unknown_453A10()
{
	G_ASSERT(false);
}

/*
** Notes
**
** px_callback_type.cpp is client 0x453740-0x453A40, after px_callback_poll.cpp (whose this-adjusting thunks end at
** 0x453730) and before px_collision.cpp; its __FILE__ is pushed by the asserts (lines 47, 101-103, 112). The enum
** names and their value names are the client's strings ('PxCallbackType', 'PxCallbackEvent', the values'
** names); _EventStrings is the FAIL_IF's; the function names and the enumerators' spellings are provisional. No
** function here has a caller or a pointer reference in the client.
*/
