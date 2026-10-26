// Earth & Beyond client (reconstruction): app\packets\pktalktreeaction.cpp. Notes at the end of the file.
#include "pktalktreeaction.h"
#include <stdio.h>


TalkTreeActionPacketClass::TalkTreeActionPacketClass(int action) : Action(action)
{
}

TalkTreeActionPacketClass::TalkTreeActionPacketClass(RawPacketClass *raw)
{
	raw->Get(&Action, sizeof(Action));
}

RawPacketClass *TalkTreeActionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(86, Get_Data_Size());
	raw->Add(&Action, sizeof(Action));
	return(raw);
}

int TalkTreeActionPacketClass::Get_Data_Size()
{
	return(sizeof(Action));
}

int TalkTreeActionPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Action=%d}", "TalkTreeActionPacket", Action));
}

SelectTalkTreePacketClass::SelectTalkTreePacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
	raw->Get(&Selection, sizeof(Selection));
}

RawPacketClass *SelectTalkTreePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(85, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	raw->Add(&Selection, sizeof(Selection));
	return(raw);
}

int SelectTalkTreePacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Selection));
}

int SelectTalkTreePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d Selection=%d}", "SELECT_TALK_TREE", PlayerID, Selection));
}


/*
**	Reconstruction notes (pktalktreeaction.cpp)
**
**	* Client 0x8A6A80-0x8A7E70 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 84 (declared in pk_classes.h).
*/
