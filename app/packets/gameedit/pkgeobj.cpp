// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeobj.cpp. Notes at the end of the file.
#include "pkgeobj.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>


GameEditorCreateFinalPacketClass::GameEditorCreateFinalPacketClass(char *data, int size) :
	Size(size),
	Data(data),
	Owned(false)
{
}

GameEditorCreateFinalPacketClass::GameEditorCreateFinalPacketClass(PacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];



	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

GameEditorCreateFinalPacketClass::~GameEditorCreateFinalPacketClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

PacketClass *GameEditorCreateFinalPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30016, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int GameEditorCreateFinalPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorCreateFinalPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorCreateFinal", Data));
}

GameEditorCreateEmptyPacketClass::GameEditorCreateEmptyPacketClass(char *data, int size) :
	Size(size),
	Data(data),
	Owned(false)
{
}

GameEditorCreateEmptyPacketClass::GameEditorCreateEmptyPacketClass(PacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];













	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

GameEditorCreateEmptyPacketClass::~GameEditorCreateEmptyPacketClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

PacketClass *GameEditorCreateEmptyPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30014, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int GameEditorCreateEmptyPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorCreateEmptyPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorCreateEmpty", Data));
}

GameEditorCreateCancelPacketClass::GameEditorCreateCancelPacketClass()
{
}

GameEditorCreateCancelPacketClass::GameEditorCreateCancelPacketClass(PacketClass *)
{
}

PacketClass *GameEditorCreateCancelPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30015, Get_Data_Size());
	return(raw);
}

int GameEditorCreateCancelPacketClass::Get_Data_Size()
{
	return(0);
}

int GameEditorCreateCancelPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GameEditorCreateCancel"));
}

GameEditorDeletePacketClass::GameEditorDeletePacketClass(int game_id) : GameID(game_id)
{
}

GameEditorDeletePacketClass::GameEditorDeletePacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

PacketClass *GameEditorDeletePacketClass::Build()
{
	PacketClass *raw = new PacketClass(30009, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int GameEditorDeletePacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int GameEditorDeletePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorDelete", GameID));
}

GameEditorMovePacketClass::GameEditorMovePacketClass(int game_id, const Matrix3D &transform) : GameID(game_id), Transform(transform) {}

GameEditorMovePacketClass::GameEditorMovePacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Transform, sizeof(Transform));
}

PacketClass *GameEditorMovePacketClass::Build()
{
	PacketClass *raw = new PacketClass(30010, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Transform, sizeof(Transform));
	return(raw);
}

int GameEditorMovePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Transform));
}

int GameEditorMovePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorMove", GameID));
}

GameEditorTemplateCategoriesPacketClass::GameEditorTemplateCategoriesPacketClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

GameEditorTemplateCategoriesPacketClass::GameEditorTemplateCategoriesPacketClass(PacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];





















































	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

GameEditorTemplateCategoriesPacketClass::~GameEditorTemplateCategoriesPacketClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

PacketClass *GameEditorTemplateCategoriesPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30011, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int GameEditorTemplateCategoriesPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorTemplateCategoriesPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorTemplateCategories", Data));
}

bool GameEditorTemplateCategoriesPacketClass::Process_Packet(void *)
{
	return(true);
}

GameEditorRequestTemplateListPacketClass::GameEditorRequestTemplateListPacketClass(char *data, int size) :
	Size(size),
	Data(data),
	Owned(false)
{
}

GameEditorRequestTemplateListPacketClass::GameEditorRequestTemplateListPacketClass(PacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];
















	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

GameEditorRequestTemplateListPacketClass::~GameEditorRequestTemplateListPacketClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

PacketClass *GameEditorRequestTemplateListPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30012, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int GameEditorRequestTemplateListPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorRequestTemplateListPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorRequestTemplateList", Data));
}

GameEditorTemplateListPacketClass::GameEditorTemplateListPacketClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

GameEditorTemplateListPacketClass::GameEditorTemplateListPacketClass(PacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];













	G_ASSERT(Data);
	Owned = true;
	raw->Get(Data, Size);
}

GameEditorTemplateListPacketClass::~GameEditorTemplateListPacketClass()
{
	if (Owned) {
		delete [] Data;
		Data = NULL;
	}
}

PacketClass *GameEditorTemplateListPacketClass::Build()
{
	PacketClass *raw = new PacketClass(30013, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int GameEditorTemplateListPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorTemplateListPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {%s}", "GameEditorTemplateList", Data));
}

bool GameEditorTemplateListPacketClass::Process_Packet(void *)
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
