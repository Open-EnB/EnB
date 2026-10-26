// Earth & Beyond client (reconstruction): app\packets\pkcameracontrol.cpp. Notes at the end of the file.
#include "pkcameracontrol.h"
#include <stdio.h>


CameraControlPacketClass::CameraControlPacketClass(RawPacketClass *raw) : Message(-1), GameID(0), Unknown14(-1)
{
	raw->Get(Message);
	raw->Get(GameID);
}

RawPacketClass *CameraControlPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Message);
	raw->Add(GameID);
	return(raw);
}

int CameraControlPacketClass::Get_Type()
{
	return(146);
}

int CameraControlPacketClass::Get_Data_Size()
{
	return(sizeof(Message) + sizeof(GameID));
}

int CameraControlPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s { Message=%d GameID=%d}", "CameraControl", Message, GameID));
}


/*
**	Reconstruction notes (pkcameracontrol.cpp)
**
**	* Client 0x878280-0x8785A0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
