// Earth & Beyond client (reconstruction): app\packets\pksoundcommand.cpp. Notes at the end of the file.
#include "pksoundcommand.h"
#include <stdio.h>


SoundCommandPacketClass::SoundCommandPacketClass(int command, int channel) : Command(command), Channel(channel)
{
}

SoundCommandPacketClass::SoundCommandPacketClass(RawPacketClass *raw)
{
	raw->Get(&Command, sizeof(Command));
	raw->Get(&Channel, sizeof(Channel));
}

RawPacketClass *SoundCommandPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(107, Get_Data_Size());
	raw->Add(&Command, sizeof(Command));
	raw->Add(&Channel, sizeof(Channel));
	return(raw);
}

int SoundCommandPacketClass::Get_Data_Size()
{
	return(sizeof(Command) + sizeof(Channel));
}

int SoundCommandPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Client Sound Command Packet {Command=%d Channel=%d}", Command, Channel));
}


/*
**	Reconstruction notes (pksoundcommand.cpp)
**
**	* Client 0x8A4F20-0x8A54E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 106 (declared in pk_classes.h).
*/
