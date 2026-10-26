// Earth & Beyond client (reconstruction): app\packets\pkgalaxymap.cpp. Notes at the end of the file.
#include "pkgalaxymap.h"
#include <stdio.h>
#include <string.h>

GalaxyMapPacketClass::GalaxyMapPacketClass(int type, LinearBufferClass &buffer, int channel) : Type(type), Data(0), Size(buffer.Get_Size()), Channel(channel)
{
	Data = new char[Size];
	memcpy(Data, buffer.Buffer, Size);
}

GalaxyMapPacketClass::GalaxyMapPacketClass(RawPacketClass *raw) : Type(-1), Data(0), Size(0), Channel(0)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];
	raw->Get(Data, Size);
}

RawPacketClass *GalaxyMapPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

GalaxyMapPacketClass::~GalaxyMapPacketClass()
{
	delete [] Data;
	Data = 0;
}

int GalaxyMapPacketClass::Get_Data_Size()
{
	return(sizeof(Type) + sizeof(Size) + Size);
}

int GalaxyMapPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Type=%d Size=%d Channel(PlayerID)=%d Data=%0.40s}", "GalaxyMapPacketClass", Type, Size, Channel, Data));
}

GalaxyMapRequestPacketClass::GalaxyMapRequestPacketClass(int type, LinearBufferClass &buffer, int channel, bool b) : GalaxyMapPacketClass(type, buffer, channel), Unknown1C(b)
{
}

GalaxyMapRequestPacketClass::GalaxyMapRequestPacketClass(RawPacketClass *raw) : GalaxyMapPacketClass(raw), Unknown1C(false)
{
	raw->Get(&Channel, sizeof(Channel));
	raw->Get(&Unknown1C, sizeof(Unknown1C));
}

RawPacketClass *GalaxyMapRequestPacketClass::Build()
{
	RawPacketClass *raw = GalaxyMapPacketClass::Build();
	raw->Add(&Channel, sizeof(Channel));
	raw->Add(&Unknown1C, sizeof(Unknown1C));
	return(raw);
}

int GalaxyMapRequestPacketClass::Get_Data_Size()
{
	return(GalaxyMapPacketClass::Get_Data_Size() + sizeof(Channel) + sizeof(Unknown1C));
}


/*
**	Reconstruction notes (pkgalaxymap.cpp)
**
**	* Client 0x870E90-0x8714C0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand from the client (builder P, 2026-10-26): the galaxy
**	  map packet (opcode 151: a type, a byte count and the bytes) and the request derived from it (152:
**	  also the channel and a flag). Get_Channel and Get_Type are inline (emitted before each class's
**	  scalar deleting destructor); Build takes its opcode from Get_Type. The request's Build calls the
**	  base's out of line, its destructor is implicit (the client's copy is folded with another class's
**	  identical destructor, 0x7A4FA0).
*/
