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
	sprintf(buf, "%I64d", value.Amount);
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
