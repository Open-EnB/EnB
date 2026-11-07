// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context_harvestable.h (the client's
// ui_mfd_extended_context_harvestable.cpp per its asserts; this header's name is provisional, NAME 1)
#ifndef UI_MFD_EXTENDED_CONTEXT_HARVESTABLE_H  // NAME 1 [ui_mfd_extended_context_harvestable.h]: provisional header name
#define UI_MFD_EXTENDED_CONTEXT_HARVESTABLE_H

#include <list>
#include <vector>
#include "ui_mfd_extended_context_strings.h"

class ClientClass;

// What the client's inv_data_peek.cpp / .h give for one slot's item (only its name is read here)
struct InvDataPeekItemStruct	// NAME 1: neutral (D10)
{
	char Unknown00[0x14];
	std::string Name;									// +0x14  // NAME 2: '%s (%d)'
};

// The inventory array a panel reads (0x84 bytes; inv_data_peek.cpp is not rebuilt yet: declared as far as this file calls it)
class InvDataPeekClass	// NAME 3: D24 class-from-file for inv_data_peek.cpp
{
public:
	InvDataPeekClass(ClientClass *client, const char *name, bool flag);	// 0x689A80  // NAME 1 [client, name, flag]: what it does with them
	~InvDataPeekClass();								// 0x689AD0
	void Attach(char *aux_data);						// 0x689AF0  // NAME 1: neutral (D10)
	bool Changed();										// 0x689C30  // NAME 2: what it returns
	bool Slot_Changed(unsigned int slot);				// 0x689C60  // NAME 2: what it returns
	int Get_ID(unsigned int slot);						// 0x689CA0 (-1, -2: none)  // NAME 2: what it returns
	InvDataPeekItemStruct *Get_Item(unsigned int slot, bool flag);	// 0x68AAA0  // NAME 2: what it returns
	int Get_Quantity(unsigned int slot);				// 0x689D20  // NAME 2: '%s (%d)'

	char Unknown00[0x44];
	unsigned int Count;									// +0x44  // NAME 2: what it counts
	char Unknown48[0x3C];
};

// The panel of a harvestable object's contents (0xE4 bytes, vtable 0xAF3934; 'TEXT%02d_HARV' from 'fa_harvesting.ini')
class UIMFDExtendedContextHarvestableClass : public UIMFDExtendedContextStringsClass	// NAME 3: D24 class-from-file for ui_mfd_extended_context_harvestable.cpp
{
public:
	UIMFDExtendedContextHarvestableClass(ClientObjectClass *co, UIMFDClass *owner);	// 0x5BFB00
	virtual ~UIMFDExtendedContextHarvestableClass();	// 0x5BFDD0 (deleting 0x5BFDA0)
	virtual void Unknown_08();							// 0x5BFF10
	virtual void Unknown_0C();							// 0x5BFF30
	virtual void Unknown_10();							// 0x5BFF50
	virtual std::string Get_Font_Name(int index);		// 0x5C0110
	virtual bool Line_Changed(unsigned int line_index);	// 0x5C0240
	virtual std::string Get_Line_Text(unsigned int line_index);	// 0x5C0350

	bool Has_Contents(unsigned int slot);				// 0x5C0750  // NAME 2: what it returns
	void Update_Index_Map();							// 0x5C07E0  // NAME 2: what it does

	InvDataPeekClass Peek;								// +0x30  // NAME 2: what it is
	bool UnknownB4;										// +0xB4
	std::list<int> Slots;								// +0xB8 (the slots with contents)  // NAME 2: what it holds
	std::vector<int> CurrentIndexMap;					// +0xC4  // NAME 5: the client's assert
	std::vector<int> PreviousIndexMap;					// +0xD4  // NAME 5: the client's assert
};

#endif
