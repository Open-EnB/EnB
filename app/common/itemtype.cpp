// Earth & Beyond client (reconstruction): app\Common\itemtype.cpp (__FILE__ 0xBAEB90). Notes at the end of the file.
#include <string>
#include <string.h>
#define __ITEMTYPE_ALL
#include "itemtype.h"
#include "..\shared\g_assert.h"

struct ItemTypeMaskStruct {	// a mount type's name and mask  // NAME 1: what it holds
	const char *Name;	// NAME 2: what it holds
	int Mask;			// NAME 2: what it holds
};

static ItemTypeMaskStruct _ItemTypeMasks[] = {	// 0xBAE7B0  // NAME 1: what it holds
	{ "None", 0 },
	{ "System", 0x20809 },
	{ "Weapon", 0x1C002 },
	{ "Turret", 0x1C022 },
	{ "Engine", 0x40 },
	{ "Power", 0x80 },
	{ "Controller", 0x100 },
	{ "Ammo", 0x400 },
	{ "Shield", 0x4 },
	{ NULL, 0 },
};

static const char *_ItemTypeNames[] = {	// 0xBAE800  // NAME 1: what it holds
	"System", "Weapon", "Shield", "Sensor", "Ejection", "Turret", "Engine", "Power", "Controller", "Robot", "Ammo",
	"Equipable", "Activatable", "Base", "Beam", "Missile", "Projectile", "Counter", "Override", "All", NULL,
};

const char *Unknown_BAE854[] = {	// 0xBAE854 (ui_trade.cpp)
	"Devices", "Weapon", "Shields", "Sensors", "Ejection", "Turrets", "Engines", "Reactors", "Controller", "Robots", "Ammo",
	"Equipable", "Activatable", "Goods", "Beam", "Missile", "Projectile", "Counter", "Override", "All", NULL,
};

static const char *_BasicItemTypeNames[] = {	// 0xBAE8A8  // NAME 1: what it holds
	"Loot", "Quest Item", "Delivery Item", "Trade Good", "Component", "Harvestable", "Resource", "Other", NULL,
};

static const char *_ItemTypeDisplayNames[] = {	// 0xBAE8CC  // NAME 1: what it holds
	"System", "Weapon", "Shield", "Sensor", "Ejector", "Turret", "Engine", "Reactor", "Controller", "Robot", "Ammo",
	"System", "System", "Base", "Beam Weapon", "Missile Launcher", "Projectile Weapon", "Countermeasure", "Overrides", "All", NULL,
};

static const char *_ItemSortNames[] = {	// 0xBAE920  // NAME 1: what it holds
	"no sort", "name ascending", "name descending", "tech ascending", "tech descending", "type ascending",
	"type descending", "quality ascending", "quality descending", "value ascending", "value descending", NULL,
};

































































































// 0x843110
bool String_To_Value(const char *buf, ItemTypeEnum *item_type)
{
	for (int i = 0; i < (int)(sizeof(_ItemTypeNames) / sizeof(_ItemTypeNames[0])); i++) {
		if (_ItemTypeNames[i] && stricmp(buf, _ItemTypeNames[i]) == 0) {
			*item_type = (ItemTypeEnum)i;
			return(true);
		}
	}
	if (stricmp(buf, "Launcher") == 0) {
		*item_type = (ItemTypeEnum)ITEMTYPE_MISSILE;
		return(true);
	}
	G_ASSERT(0);
	return(false);
}

// 0x8431B0
char *Value_To_String(char *buf, int size, ItemTypeEnum item_type)
{
	strncpy(buf, _ItemTypeNames[item_type], size);
	return(buf);
}




























// 0x8431F0
std::string Item_Type_Name(ItemTypeEnum item_type)
{
	FAIL_IF(item_type <= ITEMTYPE_NONE) return(std::string("UnknownItemType"));
	FAIL_IF(item_type >= NUM_ITEMTYPE) return(std::string("UnknownItemType"));
	return(std::string(_ItemTypeDisplayNames[item_type]));
}

// 0x8433C0
int Unknown_8433C0(const char *buf)
{
	const char *name;
	for (int i = 0; (name = _ItemTypeMasks[i].Name) != NULL; i++) {
		if (strnicmp(buf, name, strlen(name)) == 0) {
			return(_ItemTypeMasks[i].Mask);
		}
	}
	return(0);
}

// 0x843440
const char *Unknown_843440(int mount_type)
{
	const char *name;
	for (int i = 0; (name = _ItemTypeMasks[i].Name) != NULL; i++) {
		if (mount_type == _ItemTypeMasks[i].Mask) {
			return(name);
		}
		if (mount_type == 0) {
			return(NULL);
		}
	}
	return(NULL);
}








































// 0x843480
bool String_To_Value(const char *buf, BasicItemTypeEnum *a)
{
	for (int i = 0; i < NUM_BASICITEMTYPE; i++) {
		if (_BasicItemTypeNames[i] && stricmp(buf, _BasicItemTypeNames[i]) == 0) {
			*a = (BasicItemTypeEnum)i;
			return(true);
		}
	}
	G_ASSERT(0);
	return(false);
}
// 0x843500
char *Value_To_String(char *buf, int size, BasicItemTypeEnum a)
{
	FAIL_IF(a <= BASICITEMTYPE_NOT_BASIC) return("UnknownBasicItemType");
	FAIL_IF(a >= NUM_BASICITEMTYPE) return("UnknownBasicItemType");
	strncpy(buf, _BasicItemTypeNames[a], size);
	return(buf);
}
























// 0x843590
bool String_To_Value(const char *buf, ItemSortEnum *a)
{
	for (int i = 0; i < NUM_ITEMSORT; i++) {
		if (_ItemSortNames[i] && stricmp(buf, _ItemSortNames[i]) == 0) {
			*a = (ItemSortEnum)i;
			return(true);
		}
	}
	G_ASSERT(0);
	return(false);
}

// 0x843610
char *Value_To_String(char *buf, int size, ItemSortEnum a)
{
	FAIL_IF(a <= ITEMSORT_NAME_ASC) return("UnknownItemSortEnum");
	FAIL_IF(a >= NUM_ITEMSORT) return("UnknownItemSortEnum");
	strncpy(buf, _BasicItemTypeNames[a], size);
	return(buf);
}

/*
**	Reconstruction notes
**
**	The object: 0x843110 (after item_state.cpp's guarded <string> initialiser 0x8430E0) to its own guarded
**	<string> initialiser 0x8436A0; lego_identifier.cpp opens at 0x8436D0 (its EmptyName's initialiser; the
**	initialiser list 0xB6CFA0.. holds 0x8430E0, 0x8436A0, 0x8436D0 with no separator, so the list does not
**	split these objects). Its data, in definition order: the mask table 0xBAE7B0, the item type names
**	0xBAE800, the trade window's category names 0xBAE854 (Unknown_BAE854, ui_trade.cpp), the basic type
**	names 0xBAE8A8, the display names 0xBAE8CC, the sort names 0xBAE920, then the texts and the __FILE__
**	string 0xBAEB90. The asserts pin lines 159, 200/201, 280, 286/287, 324 and 331/332. The client's own
**	slips are kept: the sort order's Value_To_String copies the basic type's name and refuses 'name
**	ascending' (the FAIL_IF 'a <= ITEMSORT_NAME_ASC'); 'Launcher' reads as 'Missile' (15).
**	Value_To_String 0x843610 is exact: the client's function list runs it into the initialiser after it.
*/
