// Earth & Beyond client (reconstruction): app\packets\pkuitrigger.cpp. Notes at the end of the file.
#include "pkuitrigger.h"
#include <stdio.h>


UITriggerPacketClass::UITriggerPacketClass(int id, int action) : ID(id), Action(action)
{
}

UITriggerPacketClass::UITriggerPacketClass(PacketClass *raw)
{
	raw->Get(&ID, sizeof(ID));
	raw->Get(&Action, sizeof(Action));
}

PacketClass *UITriggerPacketClass::Build()
{
	PacketClass *raw = new PacketClass(101, Get_Data_Size());
	raw->Add(&ID, sizeof(ID));
	raw->Add(&Action, sizeof(Action));
	return(raw);
}

int UITriggerPacketClass::Get_Data_Size()
{
	return(sizeof(ID) + sizeof(Action));
}

int UITriggerPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ID=%d Action=%d }", "UITrigger", ID, Action));
}


/*
**	Reconstruction notes (pkuitrigger.cpp)
**
**	* Client 0x8AB800-0x8ABA50 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
