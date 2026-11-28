// Earth & Beyond client (reconstruction): app\packets\pkgmresetmission.cpp. Notes at the end of the file.
#include "pkgmresetmission.h"
#include <stdio.h>
#include "..\shared\auth_product.h"
GMResetMissionRequestPacketClass::GMResetMissionRequestPacketClass(PacketClass *raw) :
	Unknown0C(0),
	Unknown20(false)
{
	raw->Get(Unknown0C);
	int value = 0;
	raw->Get(value);
	Unknown20 = value != 0;
	char *name = raw->Get_String();
	MissionName = name;
	delete [] name;
}

GMResetMissionRequestPacketClass::GMResetMissionRequestPacketClass(int unknown0c, const char *mission_name, bool unknown20) :
	Unknown0C(unknown0c),
	Unknown20(unknown20)
{
	if (mission_name != NULL) {
		MissionName = mission_name;
	}
}

GMResetMissionRequestPacketClass::~GMResetMissionRequestPacketClass()
{
}

PacketClass *GMResetMissionRequestPacketClass::Build()
{
	PacketClass *raw = new PacketClass(237, Get_Data_Size());
	raw->Add(Unknown0C);
	int value = Unknown20;
	raw->Add(value);
	raw->Add_String(MissionName.c_str());
	return(raw);
}

int GMResetMissionRequestPacketClass::Get_Data_Size()
{
	return(MissionName.length() + 1 + sizeof(Unknown0C) + sizeof(int));
}

int GMResetMissionRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "GM Reset Mission { Mission Name = %s }", MissionName.c_str()));
}


/*
**	Reconstruction notes (pkgmresetmission.cpp)
**
**	* Client 0x871A30-0x871F40 (0x871A00 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x871F40): opcode 237 (GM Destroy Item 236's twin), written by hand (builder P4, 2026-10-27). The raw constructor comes
**	  first in the object, then the value constructor.
**	* The file name is provisional (no assert names it).
*/
