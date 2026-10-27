// Earth & Beyond client (reconstruction): app\packets\pkgmdestroyitem.cpp. Notes at the end of the file.
#include "pkgmdestroyitem.h"
#include <stdio.h>

GMDestroyItemPacketClass::GMDestroyItemPacketClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown20(false)
{
	raw->Get(Unknown0C);
	int value = 0;
	raw->Get(value);
	Unknown20 = value != 0;
	char *name = raw->Get_String();
	ItemName = name;
	delete [] name;
}

GMDestroyItemPacketClass::GMDestroyItemPacketClass(int unknown0c, const char *item_name, bool unknown20) :
	Unknown0C(unknown0c),
	Unknown20(unknown20)
{
	if (item_name != NULL) {
		ItemName = item_name;
	}
}

GMDestroyItemPacketClass::~GMDestroyItemPacketClass()
{
}

RawPacketClass *GMDestroyItemPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(236, Get_Data_Size());
	raw->Add(Unknown0C);
	int value = Unknown20;
	raw->Add(value);
	raw->Add_String(ItemName.c_str());
	return(raw);
}

int GMDestroyItemPacketClass::Get_Data_Size()
{
	return(ItemName.length() + 1 + sizeof(Unknown0C) + sizeof(int));
}

int GMDestroyItemPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "GM Destroy Item { Item Name = %s }", ItemName.c_str()));
}


/*
**	Reconstruction notes (pkgmdestroyitem.cpp)
**
**	* Client 0x8714F0-0x871A00 (0x8714C0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x871A00): opcode 236, written by hand (builder P4, 2026-10-27). The raw constructor comes
**	  first in the object, then the value constructor.
**	* The file name is provisional (no assert names it).
*/
