// Earth & Beyond client (reconstruction): app\packets\pktrade.cpp. Notes at the end of the file.
#include "pktrade.h"
#include <stdio.h>


TradePacketClass::TradePacketClass(RawPacketClass *raw)
{
	raw->Get(&PartnerID, sizeof(PartnerID));
	raw->Get(&Type, sizeof(Type));
}

RawPacketClass *TradePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(31, Get_Data_Size());
	raw->Add(&PartnerID, sizeof(PartnerID));
	raw->Add(&Type, sizeof(Type));
	return(raw);
}

int TradePacketClass::Get_Data_Size()
{
	return(sizeof(PartnerID) + sizeof(Type));
}

int TradePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Trade_Action {PartnerID=%d Type=%d}", PartnerID, Type));
}


/*
**	Reconstruction notes (pktrade.cpp)
**
**	* Client 0x8AB290-0x8AB530 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
