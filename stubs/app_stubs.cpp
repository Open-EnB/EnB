/*
**	Earth & Beyond client (reconstruction): link-time stubs for app.lib (D44, docs/milestones.md 3.2)
**
**	The functions app.lib's objects call that the tree doesn't have yet (build.py unresolved,
**	2026-10-24). Each stub logs its first call (stub_log.h) and does as little as it can; the client
**	address, where known, is the real function to rebuild. The auxdata ones are M1.2's to-do list
**	(milestones.md 2.3). The three globals are data, so they can't log: they start at 0.
*/

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "stub_log.h"

#include "unknown_debug.h"
#include "kv_list.h"
#include "kv_pair_string.h"
#include "lookup.h"
#include "delta.h"
#include "linearbu.h"
#include "encode.h"
#include "strconv_lib.h"
#include "delta_string.h"
#include "duration.h"
#include "money.h"
#include "delta_template.h"

// ------------------------------------------------------------------------- E&B debug hooks

// 0x4BC260, the reporter of the assert functions (app/shared/unknown_debug.cpp: 0x4E9410, 0x4E9450).
// An assert is worth a line each time, not only the first.
void Unknown_Report_Message(const char *sender, const char *category, const char *file, int line,
	const char *format, ...)
{
	STUB_CALLED("Unknown_Report_Message (0x4BC260)");
	char text[1024];
	va_list va;
	va_start(va, format);
	_vsnprintf(text, sizeof(text) - 1, format, va);
	va_end(va);
	text[sizeof(text) - 1] = 0;
	Stub_Log_Line("%s: %s, %s(%d)", category, text, file, line);
}

// permstr.cpp's statistics line
void Unknown_Debug_Printf(const char *format, ...)
{
	STUB_CALLED("Unknown_Debug_Printf");
	char text[1024];
	va_list va;
	va_start(va, format);
	_vsnprintf(text, sizeof(text) - 1, format, va);
	va_end(va);
	text[sizeof(text) - 1] = 0;
	Stub_Log_Line("Unknown_Debug_Printf: %s", text);
}

// ------------------------------------------------------------------------- globals (data)

int DeltaUnknownGlobal = 0;					// delta.h
int DeltaCompoundUnknownGlobal = 0;			// delta.h
int DeltaStringClass::UnknownGlobal = 0;	// 0xBE3A9C

// ------------------------------------------------------------------------- strconv_lib.h

// The value <-> text conversions the key/value pairs use. Not stubs: the client's behaviour
// (0x4A3200 / 0x4A3240 / 0x4A3590 / 0x4A35C0), here until strconv_lib.cpp is rebuilt.
char *Value_To_String(char *buf, int size, float value)
{
	sprintf(buf, "%f", value);
	return buf;
}

bool String_To_Value(const char *text, float *value)
{
	*value = (float)atof(text);
	return true;
}

char *Value_To_String(char *buf, int size, unsigned int value)
{
	sprintf(buf, "%u", value);
	return buf;
}

bool String_To_Value(const char *text, unsigned int *value)
{
	return sscanf(text, "%u", value) == 1;
}

// MoneyClass's (money.cpp, 0x4F0AB0 / 0x4F0BE0 / 0x4F0640)
const char *Type_Name(MoneyClass *)
{
	STUB_CALLED("Type_Name(MoneyClass *) (0x4F0AB0)");
	return "MoneyClass";
}

char *Value_To_String(char *buf, int size, const MoneyClass &value)
{
	STUB_CALLED("Value_To_String(MoneyClass) (0x4F0BE0)");
	sprintf(buf, "%I64d", value.m_Unknown00);
	return buf;
}

bool String_To_Value(const char *text, MoneyClass *value)
{
	STUB_CALLED("String_To_Value(MoneyClass) (0x4F0640)");
	return false;
}
