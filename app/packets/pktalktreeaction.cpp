// Earth & Beyond client (reconstruction): app\packets\pktalktreeaction.cpp. Notes at the end of the file.
#include "pktalktreeaction.h"
#include <stdio.h>

TalkTreePacketClass::TalkTreePacketClass(const std::string &npc_say, const std::string &npc_sound_file_name) :
	NPCSay(npc_say),
	NPCSoundFileName(npc_sound_file_name)
{
}

void TalkTreePacketClass::Add_Response(int unknown00, const std::string &text)
{
	Responses.push_back(TalkTreeResponseStruct(unknown00, text));
}

TalkTreePacketClass::TalkTreePacketClass(RawPacketClass *raw)
{
	char *string = raw->Get_String();
	NPCSay = string;
	delete string;
	string = raw->Get_String();
	NPCSoundFileName = string;
	delete string;
	char count;
	raw->Get(&count, 1);
	TalkTreeResponseStruct response(0, "");
	for (char i = 0; i < count; i++) {
		raw->Get(&response.Unknown00, sizeof(response.Unknown00));
		string = raw->Get_String();
		response.Text = string;
		delete string;
		Responses.push_back(response);
	}
}

TalkTreePacketClass::~TalkTreePacketClass()
{
}

void TalkTreePacketClass::Swap_Responses(std::vector<TalkTreeResponseStruct> &responses)
{
	responses.swap(Responses);
}

RawPacketClass *TalkTreePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(84, Get_Data_Size());
	raw->Add_String(NPCSay.c_str());
	raw->Add_String(NPCSoundFileName.c_str());
	char count = Responses.size();
	raw->Add(&count, sizeof(count));
	for (std::vector<TalkTreeResponseStruct>::iterator it = Responses.begin(); it != Responses.end(); it++) {
		raw->Add(&it->Unknown00, sizeof(it->Unknown00));
		raw->Add_String(it->Text.c_str());
	}
	return(raw);
}

int TalkTreePacketClass::Get_Data_Size()
{
	int size = NPCSay.length() + 1 + NPCSoundFileName.length() + 1 + sizeof(char);
	for (std::vector<TalkTreeResponseStruct>::iterator it = Responses.begin(); it != Responses.end(); it++) {
		size += sizeof(it->Unknown00) + it->Text.length() + 1;
	}
	return(size);
}

int TalkTreePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {NumResponses=%d NPCSay=%s NPCSoundFileName%s}", "TalkTreePacket", Responses.size(),
		NPCSay.c_str(), NPCSoundFileName.c_str()));
}

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
**	* TALK_TREE 84 written by hand (builder P6, 2026-10-27): two strings and a std::vector of responses
**	  (an int and a string each); Add_Response 0x8A6BC0 sits between the value and the raw constructor.
*/
