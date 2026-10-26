// Earth & Beyond client (reconstruction): app\packets\pkreferralpointsavailable.cpp. Notes at the end of the file.
#include "pkreferralpointsavailable.h"
#include <stdio.h>


ReferralPointsAvailablePacketClass::ReferralPointsAvailablePacketClass(RawPacketClass *raw) : PointsTotal(0), PointsSpent(0)
{
	raw->Get(PointsTotal);
	raw->Get(PointsSpent);
}

RawPacketClass *ReferralPointsAvailablePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(233, Get_Data_Size());
	raw->Add(PointsTotal);
	raw->Add(PointsSpent);
	return(raw);
}

int ReferralPointsAvailablePacketClass::Get_Data_Size()
{
	return(sizeof(PointsTotal) + sizeof(PointsSpent));
}

int ReferralPointsAvailablePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s : PointsTotal=%d  PointsSpent=%d", "ReferralPointsAvailablePacketClass", PointsTotal, PointsSpent));
}


/*
**	Reconstruction notes (pkreferralpointsavailable.cpp)
**
**	* Client 0x878030-0x878250 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
