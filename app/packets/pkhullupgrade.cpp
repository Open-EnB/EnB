// Earth & Beyond client (reconstruction): app\packets\pkhullupgrade.cpp. Notes at the end of the file.
#include "pkhullupgrade.h"
#include <stdio.h>


HullUpgradePacketClass::HullUpgradePacketClass(int ship_id, int hull_upgrade_level) : ShipID(ship_id), HullUpgradeLevel(hull_upgrade_level)
{
}

HullUpgradePacketClass::HullUpgradePacketClass(RawPacketClass *raw)
{
	raw->Get(&ShipID, sizeof(ShipID));
	raw->Get(&HullUpgradeLevel, sizeof(HullUpgradeLevel));
}

RawPacketClass *HullUpgradePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(89, Get_Data_Size());
	raw->Add(&ShipID, sizeof(ShipID));
	raw->Add(&HullUpgradeLevel, sizeof(HullUpgradeLevel));
	return(raw);
}

int HullUpgradePacketClass::Get_Data_Size()
{
	return(sizeof(ShipID) + sizeof(HullUpgradeLevel));
}

int HullUpgradePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "HullUpgrade {ShipID=%d HullUpgradeLevel=%d}", ShipID, HullUpgradeLevel));
}


/*
**	Reconstruction notes (pkhullupgrade.cpp)
**
**	* Client 0x894A00-0x894C70 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
