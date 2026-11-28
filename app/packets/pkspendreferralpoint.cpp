// Earth & Beyond client (reconstruction): app\packets\pkspendreferralpoint.cpp. Notes at the end of the file.
#include "pkspendreferralpoint.h"
#include <stdio.h>


SpendReferralPointPacketClass::SpendReferralPointPacketClass(int unknown0_c) : Unknown0C(unknown0_c), Unknown10(0)
{
}

SpendReferralPointPacketClass::~SpendReferralPointPacketClass()
{
}

SpendReferralPointPacketClass::SpendReferralPointPacketClass(PacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(Unknown0C);
}

PacketClass *SpendReferralPointPacketClass::Build()
{
	PacketClass *raw = new PacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown0C);
	return(raw);
}

int SpendReferralPointPacketClass::Get_Type()
{
	return(232);
}

int SpendReferralPointPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C));
}

int SpendReferralPointPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "SpendReferralPoint"));
}


/*
**	Reconstruction notes (pkspendreferralpoint.cpp)
**
**	* Client 0x8A5510-0x8A5770 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class SpendReferralPointPacketClass (thunk order) and its place in retail's object order (between pksoundcommand and pkspline), NAME 3; no assert names it.
*/
