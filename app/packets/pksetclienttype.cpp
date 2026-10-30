// Earth & Beyond client (reconstruction): app\packets\pksetclienttype.cpp. Notes at the end of the file.
#include "pksetclienttype.h"
#include <stdio.h>

SetClientTypePacketClass::SetClientTypePacketClass(int type) :
	Type(type)
{
}

SetClientTypePacketClass::SetClientTypePacketClass(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
}

RawPacketClass *SetClientTypePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(60, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	return(raw);
}

int SetClientTypePacketClass::Get_Data_Size()
{
	return(sizeof(Type));
}

int SetClientTypePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Type=%s}", "Set_Client_Type", Sector_Type_Name(Type)));
}


/*
**	Reconstruction notes (pksetclienttype.cpp)
**
**	* Client 0x87FCF0-0x87FF30 (between CREATE's object, whose guarded static 0xC416FC is 0x87FCC0, and
**	  this one's at 0x87FF30): CLIENT_TYPE 60, written by hand (builder P4, 2026-10-26). Its implicit
**	  destructor is this object's (0x87FD60).
**	* The file name is provisional (no assert names it).
*/
