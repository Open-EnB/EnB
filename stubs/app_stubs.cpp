// Earth & Beyond client (reconstruction): link-time stubs for app.lib (D44, docs/milestones.md 3.2)
/*
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
#include <ctype.h>
#include "stub_log.h"

#include "g_assert.h"
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
#include "random.h"
#include "matrix3d.h"

// ------------------------------------------------------------------------- E&B debug hooks

// (0x4BC260, the assert functions' reporter, is real: app/shared/message.cpp)

// ------------------------------------------------------------------------- globals (data)

int DeltaUnknownGlobal = 0;					// delta.h
int DeltaCompoundClass::UnknownGlobal = 0;	// 0xBE3A74
int DeltaStringClass::UnknownGlobal = 0;	// 0xBE3A9C

// ------------------------------------------------------------------------- strconv_lib.h

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

// MoneyClass's (money.cpp, 0x4F0AB0 / 0x4F0BE0 / 0x4F0640)
char *Value_To_String(char *buf, int size, const MoneyClass &value)
{
	STUB_CALLED("Value_To_String(MoneyClass) (0x4F0BE0)");
	sprintf(buf, "%I64d", value.Unknown00);
	return buf;
}

// money.cpp's by-value type name and comparisons (0x4F0AB0, 0x4F0600, 0x4F0640), declared in delta_money.cpp
const char *Type_Name(MoneyClass)
{
	STUB_CALLED("Type_Name(MoneyClass) (0x4F0AB0)");
	return "MoneyClass";
}


// ------------------------------------------------------------------------- lego_identifier.cpp's helpers

#include <string>
#include <vector>

// 0x4F00C0: money.cpp's conversion (opcode 220's Print): the amount in decimal; with the flag set, through 0x4FAEE0,
// which puts a ',' between every three digits from the right when the text is all digits (the client's behaviour:
// "Credits: 29,597,354" in the PDA's character window)
std::string MoneyClass::Unknown_4F00C0(bool commas) const
{
	char buffer[32];
	std::string text(_i64toa(Unknown00, buffer, 10));
	if (commas && !text.empty()) {
		bool digits = true;
		for (unsigned i = 0; i < text.size(); i++) {
			if (!isdigit((unsigned char)text[i])) digits = false;
		}
		if (digits) {
			for (int at = (int)text.size() - 3; at > 0; at -= 3) {
				text.insert(at, 1, ',');
			}
		}
	}
	return text;
}

// 0x8B44E0: the int64 in decimal (the client's behaviour)
char *Unknown_8B44E0(__int64 value, char *buffer)
{
	return _i64toa(value, buffer, 10);
}

// 0x8B44C0: the int64 from decimal text (the client's behaviour; strconv_lib.cpp's 0x4A3AB0 calls it)
__int64 Unknown_8B44C0(const char *text)
{
	return _atoi64(text);
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

// ------------------------------------------------------------------------- the warp camera's placements (unknown_7979a0.cpp)

// 0xBDB1C0: a global Random4Class with seed 0x1105 (its initialiser 0x444240; its home file is unknown). Data, so it
// can't log; seeded as the client's
Random4Class Unknown_BDB1C0(0x1105);

// 0x82ECB0 (not rebuilt): a frame along dir at pos (gen_stubs skips a by-value Matrix3D). A stand-in that works: the
// object's X axis toward pos + dir
Matrix3D Unknown_82ECB0(const Vector3 &dir, const Vector3 &pos)
{
	STUB_CALLED("Unknown_82ECB0 (0x82ECB0)");
	Matrix3D tm(true);
	tm.Obj_Look_At(pos, pos + dir, 0.0f);
	return tm;
}

// starbase_lights.cpp 0x764AB0 / 0x764CF0 (not rebuilt; gen_stubs skips a by-value Vector3): a light's pad point
// and the ambient colour, called by ui_starbase.cpp's lights panel, linked since its constructor 0x6333E0 is real
#include "starbase_lights.h"
Vector3 Unknown_764480Class::Unknown_764AB0(int a, int b, int c, int index)
{
	STUB_CALLED("Unknown_764480Class::Unknown_764AB0 (0x764AB0)");
	return Vector3(0.5f, 0.5f, 0.0f);
}
Vector3 Unknown_764480Class::Unknown_764CF0(int a, int b, int c)
{
	STUB_CALLED("Unknown_764480Class::Unknown_764CF0 (0x764CF0)");
	return Vector3(0.5f, 0.5f, 0.5f);
}

// ui_item_examination_title.cpp's names (not rebuilt; gen_stubs skips a by-value std::string), linked since
// ui_slot_info.cpp's 0x630280 news the item examination window
std::string Unknown_7EA190(int id)
{
	STUB_CALLED("Unknown_7EA190 (0x7EA190): no icon");
	return std::string();
}
std::string Unknown_49E210(int damage_type)
{
	STUB_CALLED("Unknown_49E210 (0x49E210): no damage type name");
	return std::string();
}
std::string Unknown_7EB4D0(int type)
{
	STUB_CALLED("Unknown_7EB4D0 (0x7EB4D0): no item type name");
	return std::string();
}
