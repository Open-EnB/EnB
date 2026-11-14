// Earth & Beyond client (reconstruction): app\common\itemtype.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\itemtype.h (name assumed from itemtype.cpp, whose
**	__FILE__ is 0xBAEB90; no assert names the header)
**
**	An item's type ('System' .. 'Override'), its basic type ('Loot' .. 'Other') and the item lists' sort
**	orders ('no sort' .. 'value descending'), with their names and text converters (itemtype.cpp,
**	0x843110-0x8436BF), and the mount types' masks by name (ui_mount.cpp's Build 0x5C77A0 names a mount).
**	Only the mount type's name (Unknown_843440) and the trade window's category names (Unknown_BAE854) have
**	callers in the client; the rest is behind __ITEMTYPE_ALL.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ITEMTYPE_H  // NAME 3 [itemtype.h]: header name assumed from the client's itemtype.cpp
#define ITEMTYPE_H

const char *Unknown_843440(int mount_type);	// 0x843440: a mount's type as text (the mask table's name; NULL when none)  // NAME 1: neutral (D10); NAME 1 [mount_type]: what it is given
extern const char *Unknown_BAE854[];		// 0xBAE854: the trade window's category names ('Devices', ...; ui_trade.cpp)  // NAME 1: neutral (D10)

#ifdef __ITEMTYPE_ALL	// itemtype.cpp only: the client calls nothing else

// An item's type: the client's FAIL_IF texts 'item_type <= ITEMTYPE_NONE', 'item_type >= NUM_ITEMTYPE'.
enum ItemTypeEnum { ITEMTYPE_NONE = -1, ITEMTYPE_MISSILE = 15, NUM_ITEMTYPE = 19 };	// NAME 2: the client's 'UnknownItemType'; ITEMTYPE_MISSILE: the name 'Missile' (index 15) that 'Launcher' maps to
// An item's basic type: 'a <= BASICITEMTYPE_NOT_BASIC', 'a >= NUM_BASICITEMTYPE'.
enum BasicItemTypeEnum { BASICITEMTYPE_NOT_BASIC = -1, NUM_BASICITEMTYPE = 8 };	// NAME 2: the client's 'UnknownBasicItemType'
// An item list's sort order: 'a <= ITEMSORT_NAME_ASC', 'a >= NUM_ITEMSORT' (0 is 'no sort').
enum ItemSortEnum { ITEMSORT_NONE = 0, ITEMSORT_NAME_ASC = 1, NUM_ITEMSORT = 11 };	// NAME 3: the client's 'UnknownItemSortEnum'; ITEMSORT_NONE: the name 'no sort'

bool String_To_Value(const char *buf, ItemTypeEnum *item_type);				// 0x843110
char *Value_To_String(char *buf, int size, ItemTypeEnum item_type);			// 0x8431B0
std::string Item_Type_Name(ItemTypeEnum item_type);							// 0x8431F0: the type's display name ('Beam Weapon' ...)  // NAME 2: what it returns
int Unknown_8433C0(const char *buf);											// 0x8433C0: the mask of the first name buf starts with  // NAME 1: neutral (D10)
bool String_To_Value(const char *buf, BasicItemTypeEnum *a);					// 0x843480
char *Value_To_String(char *buf, int size, BasicItemTypeEnum a);				// 0x843500
bool String_To_Value(const char *buf, ItemSortEnum *a);						// 0x843590
char *Value_To_String(char *buf, int size, ItemSortEnum a);					// 0x843610 (copies the basic type's name: the client's)

#endif

#endif
