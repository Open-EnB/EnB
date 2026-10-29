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
#include <string.h>
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
#include "pk_player_object_relationship.h"
#include "pkverbrequest.h"
#include "pkstarbasetow.h"
#include "pkmanufactureitemcatagorypacket.h"
#include "pkguildplayeronline.h"
#include "pkmessagechatstream.h"

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
int DeltaCompoundClass::UnknownGlobal = 0;	// 0xBE3A74
int DeltaStringClass::UnknownGlobal = 0;	// 0xBE3A9C

// ------------------------------------------------------------------------- strconv_lib.h

// The value <-> text conversions the key/value pairs use. Not stubs: the client's behaviour
// (0x4A3200 / 0x4A3240 / 0x4A3590 / 0x4A35C0), here until strconv_lib.cpp is rebuilt.
// 0x4A30A0 (strconv_lib.cpp): the client's behaviour
char *Value_To_String(char *buf, int size, bool value)
{
	strcpy(buf, value ? "true" : "false");
	return buf;
}

// faction_and_reputation.cpp 0x835890: the reaction's name from the client's table 0xBA9614 (not
// known yet: its number), 'BadReaction' out of range
char *Value_To_String(char *buf, int size, MobReactionType reaction)
{
	STUB_CALLED("Value_To_String(MobReactionType) (0x835890)");
	if (reaction < 0 || reaction >= MOB_REACTION_COUNT) {
		_snprintf(buf, size, "BadReaction");
	} else {
		_snprintf(buf, size, "%d", (int)reaction);
	}
	return buf;
}

// 0x848C80: RELATIONSHIP's relationship (opcode 137), the client's defaults
RelationshipClass::RelationshipClass()
{
	Reaction = (MobReactionType)1;
	IsAttacking = false;
}

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

// ------------------------------------------------------------------------- lego_identifier.cpp's helpers

#include <string>
#include <vector>

// 0x4F00C0: money.cpp's conversion (opcode 220's Print): the amount in decimal
std::string MoneyClass::Unknown_4F00C0(bool) const
{
	char buffer[32];
	return std::string(_i64toa(m_Unknown00, buffer, 10));
}

// 0x4C4360-0x4C48C0: the chat-stream decoder (opcode 95's Print): the text left empty
Unknown_ChatStreamClass::Unknown_ChatStreamClass(const void *, int) : Unknown04(0), Unknown08(0) {}
Unknown_ChatStreamClass::~Unknown_ChatStreamClass() {}
char *Unknown_ChatStreamClass::Unknown_00() { return (char *)Unknown04; }
int Unknown_ChatStreamClass::Unknown_04() { return Unknown08; }
void Unknown_ChatStreamClass::Unknown_4C48C0(char *) { STUB_CALLED("Unknown_ChatStreamClass::Unknown_4C48C0 (0x4C48C0)"); }

// 0x8B44E0: the int64 in decimal (the client's behaviour)
char *Unknown_8B44E0(__int64 value, char *buffer)
{
	return _i64toa(value, buffer, 10);
}

// 0x4A1590: a bounded strcpy (at most size - 1 characters, always ended): the client's behaviour
char *Unknown_4A1590(char *buffer, int size, const char *string)
{
	if (size == 0) {
		return buffer;
	}
	strncpy(buffer, string, size - 1);
	buffer[size - 1] = 0;
	return buffer;
}

// 0x4FC2F0: string split at separator into list (lego_identifier.cpp's Set_Name), the client's
// behaviour as far as the lego identifiers need it
void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator)
{
	list.clear();
	if (string.size() == 0) {
		return;
	}
	std::string::size_type pos = 0;
	for (;;) {
		std::string::size_type at = string.find(separator, pos);
		if (at == std::string::npos || separator.size() == 0) {
			list.push_back(string.substr(pos));
			return;
		}
		list.push_back(string.substr(pos, at - pos));
		pos = at + separator.size();
	}
}

// ------------------------------------------------------------------------- VERB_UPDATE's Print (pkverbrequest.cpp)

// 0x84C470: a verb's name from the client's table 0xBB1218 (not known yet: its number)
char *Value_To_String(char *buf, int size, VerbEnum verb)
{
	STUB_CALLED("Value_To_String(VerbEnum) (0x84C470)");
	_snprintf(buf, size, "%d", (int)verb);
	return buf;
}

// ------------------------------------------------------------------------- STARBASE_SET's Print (pkstarbasetow.cpp)

// sector_types.cpp 0x84A3B0: a sector type's name from the client's table 0xBB0AC0 (3 names)
char *Sector_Type_Name(int sector_type)
{
	STUB_CALLED("Sector_Type_Name (0x84A3B0)");
	static char *names[] = { "SectorSpace", "SectorPlanet", "SectorGasGiant" };
	if (sector_type < 0 || sector_type >= 3) {
		return "InvalidSectorType";
	}
	return names[sector_type];
}

// ------------------------------------------------------------------------- MANUFACTURE_ITEM_CATAGORY's Print

// manufacturing_defs.cpp 0x845910: a manufacturing mode's name (the client's table 0xBAEF30), "error"
// out of range
char *Manufacturing_Mode_Name(char *buffer, int len, int mode)
{
	STUB_CALLED("Manufacturing_Mode_Name (0x845910)");
	static char *names[] = { "None", "Manufacture", "Analyze", "Dismantle" };
	return strncpy(buffer, (mode >= 0 && mode < 4) ? names[mode] : "error", len);
}
