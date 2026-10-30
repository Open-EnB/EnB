// Earth & Beyond client (reconstruction): app\packets\pksoundcommand.cpp. Notes at the end of the file.
#include "pksoundcommand.h"
#include "..\..\wwlib\nstrdup.h"
#include <stdio.h>
#include <string.h>


ClientSoundPacketClass::ClientSoundPacketClass(const char *sound_name, int channel, bool queue) : SoundName(0), Channel(channel), Queue(queue)
{
	Length = strlen(sound_name) + 1;
	if (Length != 0) {
		SoundName = nstrdup(sound_name);
	}
}

ClientSoundPacketClass::~ClientSoundPacketClass()
{
	if (SoundName != 0) {
		delete [] SoundName;
	}
}

ClientSoundPacketClass::ClientSoundPacketClass(RawPacketClass *raw)
{
	raw->Get(&Length, sizeof(Length));
	if (Length != 0) {
		SoundName = new char[Length];
		raw->Get(SoundName, Length);
	}
	raw->Get(&Channel, sizeof(Channel));
	raw->Get(&Queue, sizeof(Queue));
}

RawPacketClass *ClientSoundPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(106, Get_Data_Size());
	raw->Add(&Length, sizeof(Length));
	if (Length != 0) {
		raw->Add(SoundName, Length);
	}
	raw->Add(&Channel, sizeof(Channel));
	raw->Add(&Queue, sizeof(Queue));
	return(raw);
}

int ClientSoundPacketClass::Get_Data_Size()
{
	return(sizeof(Length) + Length + sizeof(Channel) + sizeof(Queue));
}

int ClientSoundPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Client Sound Packet {SoundName=%s Channel=%d Queue=%d}", SoundName, Channel, Queue));
}

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
**	* Opcode 106 written by hand (builder P, 2026-10-26): pkdecode.py takes no length-counted buffer;
**	  pkemit.py leaves this file alone (HAND).
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 106 (declared in pk_classes.h).
*/
