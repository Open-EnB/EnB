// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeaset.cpp. Notes at the end of the file.
#include "pkgeaset.h"
#include "..\..\shared\unknown_debug.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

Packet_7544Class::Packet_7544Class(const char *mappings)
{
	Data = nstrdup(mappings);




	G_ASSERT(Data);
	Size = strlen(Data) + 1;
}

Packet_7544Class::Packet_7544Class(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];

	G_ASSERT(Data);
	raw->Get(Data, Size);
}

Packet_7544Class::~Packet_7544Class()
{
	if (Data != NULL) {
		delete [] Data;
		Data = NULL;
	}
}

RawPacketClass *Packet_7544Class::Build()
{
	RawPacketClass *packet = new RawPacketClass(30020, Get_Data_Size());



	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int Packet_7544Class::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_7544Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {mappings...}", "GameEditorAssetMapping"));
}

bool Packet_7544Class::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgeaset.cpp)
**
**	* The client's app\Packets\gameedit\pkgeaset.cpp (its asserts' __FILE__), 0x85DAA0-0x85DE00 (0x85DA70 is
**	  the previous object's guarded static, 0xC416FC; this one's is 0x85DE00): the send-only game-editor
**	  packet 30020 (not in the packet switch: its Process_Packet returns true here), written by hand
**	  (builder P5, 2026-10-27): G_ASSERT(Data) on lines 15 and 24, Build's G_ASSERT(packet) on line 42.
*/
