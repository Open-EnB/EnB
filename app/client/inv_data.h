// Earth & Beyond client (reconstruction): app\client\inv_data.h, an inventory's slot data (the client's inv_data.cpp per
// its asserts, 'e:\build\beta\G\app\Client\inv_data.cpp' at 0xB8FDC4; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x688410 and the destructor 0x6886D0 (ID1, 2026-11-10).
#ifndef INV_DATA_H  // NAME 1 [inv_data.h]: provisional header name for inv_data.cpp
#define INV_DATA_H

#include <vector>
#include <hash_set.h>
#include "slot_data.h"
#include "inv_data_peek.h"

class ClientObjectClass;

// An inventory's slot data (0x18C bytes, table 0xAFC9B0 and, for its RefCountClass at +0x18, 0xAFC9A4): ui_inv.cpp's slot
// +0x88 (0x59FA00) and ui_trade.cpp (0x652670) make it for an inventory type (1 the cargo, 2 the equipment, ...: 0x689100)
class InvDataClass : public SlotDataClass	// NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB8FDC4
{
public:
	enum { NO_SLOT = -1 };								// NAME 3: ui_inv.cpp's FAIL_IFs 'slot == InvDataClass::NO_SLOT'
	InvDataClass(int inv_type, SClient *client);		// 0x688410  // NAME 1 [inv_type, client]: what 0x59FA00 passes
	virtual ~InvDataClass();							// +0x00 0x6886D0 (deleting 0x6886A0)
	virtual InvDataClass *Unknown_10() { return(this); }	// +0x10 0x688680
	virtual bool Unknown_18();							// +0x18 0x688B50 (either list changed)
	virtual DeltaCompoundClass *Unknown_1C();			// +0x1C 0x6889F0 (the inventory's aux root, both lists attached to it)
	virtual bool Unknown_20();							// +0x20 0x688B90 (the shown slots rebuilt when the used slots changed)
	virtual void Unknown_24();							// +0x24 0x688EF0 (both lists attached again, then the base's)

	InvDataPeekClass *Unknown_688830();				// 0x688830 (+0x54)  // NAME 1: neutral (D10)
	InvDataPeekClass *Unknown_688850();				// 0x688850 (+0xD8)  // NAME 1: neutral (D10)
	void Unknown_688870(unsigned int mask, const std::hash_set<int> *ids);	// 0x688870 (ui_trade.cpp: a category's bit, 0 for all)  // NAME 1 [mask, ids]: neutral
	ClientObjectClass *Peek_Object() { return(Object); }	// NAME 3: the G_ASSERT '!Peek_Object()' (l.97)

protected:
	bool Unknown_688F70();								// 0x688F70 (the used slots, true when they changed)  // NAME 1: neutral (D10)
	static const char *Unknown_689100(int inv_type, int list);	// 0x689100 (the aux list's name: list 0 the items, 1 the ammo)  // NAME 1: neutral (D10); NAME 1 [list]: neutral

public:
	int Unknown50;										// +0x50 the inventory type (0x522860 routes 0x52AD00's property when 2)  // NAME 1: neutral
	InvDataPeekClass Unknown54;							// +0x54 the items (0x689100's list 0)  // NAME 1: neutral
	InvDataPeekClass UnknownD8;							// +0xD8 the ammo (0x689100's list 1)  // NAME 1: neutral
	unsigned int Unknown15C;							// +0x15C the item types shown (bits; -1 all)  // NAME 1: neutral
	unsigned int Unknown160;							// +0x160 the item types present (bits)  // NAME 1: neutral
	std::hash_set<int> Unknown164;						// +0x164 the item templates shown (-1 alone: none)  // NAME 1: neutral
	std::vector<bool> Unknown17C;						// +0x17C each slot used  // NAME 1: neutral
};

#endif //INV_DATA_H
