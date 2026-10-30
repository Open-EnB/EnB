// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\shared\. px_callback_type.cpp is the client's (its __FILE__, client
**	0x453740-0x453A40); this header's name is provisional: no string names it.
**
**	The proximity callbacks' two enumerations and their names ("PxCallbackType": Accurate, Liberal, Conservative,
**	Polling; "PxCallbackEvent": Enter, Exit, Update) with string conversions both ways. Nothing in the client calls
**	these functions (no call, jump or pointer reference to them or their thunks).
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_CALLBACK_TYPE_H  // NAME 1 [px_callback_type.h]: provisional header name (px_callback_type.cpp's)
#define PX_CALLBACK_TYPE_H

enum PxCallbackType {  // NAME 3: the client's name string 'PxCallbackType' (0x453740)
	PX_CALLBACK_ACCURATE = 1,  // NAME 3: the string 'Accurate'
	PX_CALLBACK_LIBERAL = 2,  // NAME 3: the string 'Liberal'
	PX_CALLBACK_CONSERVATIVE = 4,  // NAME 3: the string 'Conservative'
	PX_CALLBACK_POLLING = 8  // NAME 3: the string 'Polling'
};

enum PxCallbackEvent {  // NAME 3: the client's name string 'PxCallbackEvent' (0x453900)
	PX_EVENT_ENTER = 0,  // NAME 3: _EventStrings[0] 'Enter'
	PX_EVENT_EXIT = 1,  // NAME 3: _EventStrings[1] 'Exit'
	PX_EVENT_UPDATE = 2  // NAME 3: _EventStrings[2] 'Update' (the loose box's process passes 2)
};

const char *PxCallbackType_Name();												// 0x453740 'PxCallbackType'  // NAME 2: what it returns
bool PxCallbackType_From_String(const char *string, PxCallbackType *type);		// 0x453760 (stricmp)  // NAME 2 [string, type]: what it does
char *PxCallbackType_To_String(char *buf, int size, PxCallbackType type);		// 0x453830 (FAIL_IF '!buf' l.47)  // NAME 2 [size, type]: what it does
const char *PxCallbackEvent_Name();												// 0x453900 'PxCallbackEvent'  // NAME 2: what it returns
bool PxCallbackEvent_From_String(const char *string, PxCallbackEvent *event);	// 0x453920 (stricmp over _EventStrings)  // NAME 2 [string, event]: what it does
char *PxCallbackEvent_To_String(char *buf, int size, int index);				// 0x453970 (FAIL_IFs l.101-103)  // NAME 2 [size]: what it does
void Unknown_453A10();															// 0x453A10 (G_ASSERT(false) l.112)

#endif
