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


// ------------------------------------------------------------------------- strconv_lib.h

// (MoneyClass's Type_Name 0x4F0AB0 and Value_To_String 0x4F0BE0 are money.cpp's, by value)


// ------------------------------------------------------------------------- lego_identifier.cpp's helpers

#include <string>
#include <vector>

