// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeobj.cpp. Notes at the end of the file.
#include "pkgeobj.h"
#include "..\..\shared\unknown_debug.h"
#include <stdio.h>


Packet_7540Class::Packet_7540Class(char *data, int size) :
	Size(size),
	Data(data),
	Owned(false)
{
}

Packet_7540Class::Packet_7540Class(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];



	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_7540Class::~Packet_7540Class()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

RawPacketClass *Packet_7540Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30016, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int Packet_7540Class::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_7540Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorCreateFinal", Data));
}

Packet_753EClass::Packet_753EClass(char *data, int size) :
	Size(size),
	Data(data),
	Owned(false)
{
}

Packet_753EClass::Packet_753EClass(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];













	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_753EClass::~Packet_753EClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

RawPacketClass *Packet_753EClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30014, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int Packet_753EClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_753EClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorCreateEmpty", Data));
}

Packet_753FClass::Packet_753FClass()
{
}

Packet_753FClass::Packet_753FClass(RawPacketClass *)
{
}

RawPacketClass *Packet_753FClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30015, Get_Data_Size());
	return(raw);
}

int Packet_753FClass::Get_Data_Size()
{
	return(0);
}

int Packet_753FClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GameEditorCreateCancel"));
}

Packet_7539Class::Packet_7539Class(int game_id) : GameID(game_id)
{
}

Packet_7539Class::Packet_7539Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *Packet_7539Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30009, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int Packet_7539Class::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_7539Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorDelete", GameID));
}

Packet_753AClass::Packet_753AClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_753AClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30010, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_753AClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Unknown10));
}

int Packet_753AClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorMove", GameID));
}

Packet_753BClass::Packet_753BClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_753BClass::Packet_753BClass(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];























































	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_753BClass::~Packet_753BClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

RawPacketClass *Packet_753BClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30011, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int Packet_753BClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_753BClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorTemplateCategories", Data));
}

bool Packet_753BClass::Process_Packet(void *)
{
	return(true);
}

Packet_753CClass::Packet_753CClass(char *data, int size) :
	Size(size),
	Data(data),
	Owned(false)
{
}

Packet_753CClass::Packet_753CClass(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];
















	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_753CClass::~Packet_753CClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

RawPacketClass *Packet_753CClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30012, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int Packet_753CClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_753CClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorRequestTemplateList", Data));
}

Packet_753DClass::Packet_753DClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_753DClass::Packet_753DClass(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];













	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

Packet_753DClass::~Packet_753DClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

RawPacketClass *Packet_753DClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30013, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int Packet_753DClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_753DClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorTemplateList", Data));
}

bool Packet_753DClass::Process_Packet(void *)
{
	return(true);
}

/*
**	Reconstruction notes (gameedit\pkgeobj.cpp)
**
**	* Client 0x85F870-0x860D00 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* Named pkunknown_85f870.cpp until the rename (D95(c), builder P6, 2026-10-27).
**	* Opcodes 30016, 30014, 30012 and the send-only 30011 / 30013 (not in the packet switch; their
**	  Process_Packet returns true) written by hand (builder P5, 2026-10-27): a sized data block each,
**	  G_ASSERT(Data) on the client's lines 21, 76, 247, 310 and 365 (blank lines above them). The
**	  object is the client's app\packets\gameedit\pkgeobj.cpp (those asserts' __FILE__).
*/
