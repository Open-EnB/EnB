// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgespwn.cpp. Notes at the end of the file.
#include "pkgespwn.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
#include <string.h>

GameEditorRequestSpawnDefaultsPacketClass::GameEditorRequestSpawnDefaultsPacketClass(char *s) :
	Size(0),
	Data(NULL),
	Owned(false)
{





	G_ASSERT(s);
	Data = s;
	Size = strlen(s) + 1;
}

GameEditorRequestSpawnDefaultsPacketClass::GameEditorRequestSpawnDefaultsPacketClass(PacketClass *raw) :
	Size(0),
	Data(NULL),
	Owned(false)
{
	raw->Get(&Size, sizeof(Size));
	if (Size > 0) {
		Data = new char[Size];



		G_ASSERT(Data);
		Owned = true;
		raw->Get(Data, Size);
	}
}

GameEditorRequestSpawnDefaultsPacketClass::~GameEditorRequestSpawnDefaultsPacketClass()
{
	if (Owned) {
		delete [] Data;
	}
}

PacketClass *GameEditorRequestSpawnDefaultsPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30017, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	if (Size > 0) {
		raw->Add(Data, Size);
	}
	return(raw);
}

int GameEditorRequestSpawnDefaultsPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorRequestSpawnDefaultsPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {SpawnLocationTemplateName=%s}", "GameEditorRequestSpawnDefaults", Data));
}

GameEditorSpawnDefaultsPacketClass::GameEditorSpawnDefaultsPacketClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

GameEditorSpawnDefaultsPacketClass::GameEditorSpawnDefaultsPacketClass(PacketClass *raw) :
	Data(NULL),
	Size(0),
	Owned(false)
{
	raw->Get(&Size, sizeof(Size));
	if (Size > 0) {
		Data = new char[Size];












		G_ASSERT(Data);
		Owned = true;
		raw->Get(Data, Size);
	}
}

GameEditorSpawnDefaultsPacketClass::~GameEditorSpawnDefaultsPacketClass()
{
	if (Owned) {
		delete [] Data;
	}
}

PacketClass *GameEditorSpawnDefaultsPacketClass::Build()
{
	PacketClass *packet = new PacketClass(30018, Get_Data_Size());
	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int GameEditorSpawnDefaultsPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorSpawnDefaultsPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {DefaultValues=%s}", "GameEditorSpawnDefaults", Data));
}

bool GameEditorSpawnDefaultsPacketClass::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgespwn.cpp)
**
**	* The client's app\Packets\gameedit\pkgespwn.cpp (its asserts' __FILE__), 0x862350-0x862970 (0x862320 is
**	  the previous object's guarded static, 0xC416FC; this one's is 0x862970). Written by hand (builder P5,
**	  2026-10-27): opcode 30017 (a text the value constructor keeps, G_ASSERT(s) on line 17, the raw one's
**	  G_ASSERT(Data) on line 33) and the send-only 30018 (not in the packet switch: its Process_Packet
**	  returns true here; G_ASSERT(Data) on line 93 and Build's G_ASSERT(packet) on line 109).
*/
