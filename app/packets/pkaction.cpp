// Earth & Beyond client (reconstruction): app\packets\pkaction.cpp. Notes at the end of the file.
#include "pkaction.h"
#include <stdio.h>

ActionPacketClass::ActionPacketClass(int game_id, char action, int unknown14, int unknown18) :
	GameID(game_id),
	Action(action),
	Unknown14(unknown14),
	Unknown18(unknown18)
{
}

ActionPacketClass::ActionPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&Unknown14, sizeof(Unknown14));
	raw->Get(&Unknown18, sizeof(Unknown18));
}

RawPacketClass *ActionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(44, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Action, sizeof(Action));
	raw->Add(&Unknown14, sizeof(Unknown14));
	raw->Add(&Unknown18, sizeof(Unknown18));
	return(raw);
}

int ActionPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Action) + sizeof(Unknown14) + sizeof(Unknown18));
}

int ActionPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Action=%d Target %d OptionalVar %d}", "Action", GameID, Action, Unknown14, Unknown18));
}

Action2PacketClass::Action2PacketClass(int game_id, char action, const char *target, int optional_var) :
	GameID(game_id),
	Action(action),
	Target(target),
	OptionalVar(optional_var)
{
}

Action2PacketClass::Action2PacketClass(RawPacketClass *raw)
{
	raw->Get(GameID);
	raw->Get(Action);
	raw->Get(&Target);
	raw->Get(OptionalVar);
}

RawPacketClass *Action2PacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(45, Get_Data_Size());
	raw->Add(GameID);
	raw->Add(Action);
	raw->Add(Target);
	raw->Add(OptionalVar);
	return(raw);
}

int Action2PacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Target) + sizeof(GameID) + sizeof(Action) + sizeof(OptionalVar));
}

int Action2PacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Action=%d Target %s OptionalVar %d}", "Action", GameID, Action, Target, OptionalVar));
}


/*
**	Reconstruction notes (pkaction.cpp)
**
**	* Client 0x876360-0x8769F0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 45 (declared in pk_classes.h).
*/
