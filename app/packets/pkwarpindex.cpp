// Earth & Beyond client (reconstruction): app\packets\pkwarpindex.cpp. Notes at the end of the file.
#include "pkwarpindex.h"
#include <stdio.h>


WarpIndexPacketClass::WarpIndexPacketClass(int index) : Index(index)
{
}

WarpIndexPacketClass::WarpIndexPacketClass(RawPacketClass *raw)
{
	Index = -1;
	raw->Get(&Index, sizeof(Index));
}

RawPacketClass *WarpIndexPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(156, Get_Data_Size());
	raw->Add(&Index, sizeof(Index));
	return(raw);
}

int WarpIndexPacketClass::Get_Data_Size()
{
	return(sizeof(Index));
}

int WarpIndexPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Index=%d}", "Warp_Index", Index));
}


/*
**	Reconstruction notes (pkwarpindex.cpp)
**
**	* Client 0x8B1AA0-0x8B2050 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 155 (declared in pk_classes.h).
*/
