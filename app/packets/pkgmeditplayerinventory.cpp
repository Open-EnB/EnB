// Earth & Beyond client (reconstruction): app\packets\pkgmeditplayerinventory.cpp. Notes at the end of the file.
#include "pkgmeditplayerinventory.h"
#include <stdio.h>

GMEditPlayerInventoryPacketClass::GMEditPlayerInventoryPacketClass(RawPacketClass *raw) :
	Unknown0C(0)
{
	raw->Get(Unknown0C);
	int count = 0;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		char *string = raw->Get_String();
		Strings.push_back(std::string(string));
		delete [] string;
	}
}

GMEditPlayerInventoryPacketClass::GMEditPlayerInventoryPacketClass(int unknown0c, const std::vector<std::string> &strings) :
	Unknown0C(unknown0c),
	Strings(strings)
{
}

GMEditPlayerInventoryPacketClass::~GMEditPlayerInventoryPacketClass()
{
}

RawPacketClass *GMEditPlayerInventoryPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(234, Get_Data_Size());
	raw->Add(Unknown0C);
	int count = Strings.size();
	raw->Add(count);
	for (int i = 0; i < count; i++) {
		raw->Add_String(Strings[i].c_str());
	}
	return(raw);
}

int GMEditPlayerInventoryPacketClass::Get_Data_Size()
{
	int size = sizeof(Unknown0C) + sizeof(int);
	for (int i = 0; i < (int)Strings.size(); i++) {
		size += Strings[i].length() + 1;
	}
	return(size);
}

int GMEditPlayerInventoryPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GM Edit Player Inventory"));
}


/*
**	Reconstruction notes (pkgmeditplayerinventory.cpp)
**
**	* Client 0x86F630-0x86FCD0 (0x86F600 is the previous object's guarded static, 0xC416FC; this one's is 0x86FCD0):
**	  opcode 234, written by hand (builder P4, 2026-10-27). The raw constructor comes first in the object.
**	* The file name is provisional (no assert names it).
*/
