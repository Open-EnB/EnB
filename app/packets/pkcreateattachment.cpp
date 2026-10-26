// Earth & Beyond client (reconstruction): app\packets\pkcreateattachment.cpp. Notes at the end of the file.
#include "pkcreateattachment.h"
#include <stdio.h>


CreateAttachmentPacketClass::CreateAttachmentPacketClass(RawPacketClass *raw)
{
	raw->Get(Parent_ID);
	raw->Get(Child_ID);
	raw->Get(Unknown18);
}

RawPacketClass *CreateAttachmentPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(74, Get_Data_Size());
	raw->Add(Parent_ID);
	raw->Add(Child_ID);
	raw->Add(Unknown18);
	return(raw);
}

int CreateAttachmentPacketClass::Get_Data_Size()
{
	return(sizeof(Parent_ID) + sizeof(Child_ID) + sizeof(Unknown18));
}

int CreateAttachmentPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s { Parent_ID=%d Child_ID=%d }", "Create Attachment", Parent_ID, Child_ID));
}


/*
**	Reconstruction notes (pkcreateattachment.cpp)
**
**	* Client 0x876A20-0x876D00 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
