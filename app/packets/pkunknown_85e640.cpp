// Earth & Beyond client (reconstruction): app\packets\pkunknown_85e640.cpp. Notes at the end of the file.
#include "pkunknown_85e640.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>
#include <string.h>

Packet_7535Class::Packet_7535Class(int game_id, char subscribe, char *s) : GameID(game_id), Subscribe(subscribe != 0), Size(0), Data(NULL), Owned(false)
{



	G_ASSERT(s);
	Data = s;
	Size = strlen(s) + 1;
}

Packet_7535Class::Packet_7535Class(RawPacketClass *raw) : GameID(0), Subscribe(0), Size(0), Data(NULL), Owned(false)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Subscribe, sizeof(Subscribe));
	raw->Get(&Size, sizeof(Size));
	if (Size > 0) {
		Data = new char[Size];




		G_ASSERT(Data);
		Owned = true;
		raw->Get(Data, Size);
	}
}

Packet_7535Class::~Packet_7535Class()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_7535Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30005, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Subscribe, sizeof(Subscribe));
	raw->Add(&Size, sizeof(Size));
	if (Size > 0) {
		raw->Add(Data, Size);
	}
	return(raw);
}

int Packet_7535Class::Get_Data_Size()
{
	return(Size + sizeof(GameID) + sizeof(Subscribe) + sizeof(Size));
}

int Packet_7535Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Subscribe=%s Keys=%s}", "GameEditorRequestKeyValues", GameID, Subscribe ? "yes" : "no", Data));
}


Packet_7536Class::Packet_7536Class(int game_id) : GameID(game_id)
{
}

Packet_7536Class::Packet_7536Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *Packet_7536Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30006, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int Packet_7536Class::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_7536Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorCancelKeyValues", GameID));
}

Packet_7537Class::Packet_7537Class(int game_id, char *data, int size) :
	GameID(game_id),
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_7537Class::Packet_7537Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];

















	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_7537Class::~Packet_7537Class()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_7537Class::Build()
{
	RawPacketClass *packet = new RawPacketClass(30007, Get_Data_Size());

	G_ASSERT(packet);
	packet->Add(&GameID, sizeof(GameID));
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int Packet_7537Class::Get_Data_Size()
{
	return(Size + sizeof(GameID) + sizeof(Size));
}

int Packet_7537Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorUpdateKeyValues", GameID));
}

bool Packet_7537Class::Process_Packet(void *)
{
	return(true);
}


Packet_7538Class::Packet_7538Class(int game_id, char *data, int size) :
	GameID(game_id),
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_7538Class::Packet_7538Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];









	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_7538Class::~Packet_7538Class()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_7538Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30008, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int Packet_7538Class::Get_Data_Size()
{
	return(Size + sizeof(GameID) + sizeof(Size));
}

int Packet_7538Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorModifyKeyValues", GameID));
}


/*
**	Reconstruction notes (pkunknown_85e640.cpp)
**
**	* Client 0x85E640-0x85F220 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* Opcodes 30005, 30008 and the send-only 30007 (not in the packet switch; Process_Packet true) written
**	  by hand (builder P5, 2026-10-27): G_ASSERT(s) on line 12, G_ASSERT(Data) on lines 28, 120 and 181,
**	  30007 Build's G_ASSERT(packet) on line 136 (blank lines above them).
**	* The object is the client's app\packets\gameedit\pkgekv.cpp (those asserts' __FILE__); the file
**	  keeps its provisional name here until the rename is decided (P5's report).
*/
