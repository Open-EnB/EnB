// Earth & Beyond client (reconstruction): app\packets\pkclientchat.cpp. Notes at the end of the file.
#include "pkclientchat.h"
#include <stdio.h>
#include <string.h>

static char *ChatTypeNames[] = {	// 0xBB363C (Print's Type)  // NAME 2: what it holds
	"To_Target",
	"To_Group",
	"To_Guild",
	"To_Local_Area",
	"To_Entire_Sector",
	0
};

ClientChatPacketClass::ClientChatPacketClass(int game_id, char *string, char type, char *target) :
	GameID(game_id),
	String(0),
	Size(0),
	Type(type),
	Target(0),
	TargetSize(0),
	Owned(false)
{
	Size = (short)(strlen(string) + 1);
	String = string;
	if (target != 0) {
		TargetSize = (short)(strlen(target) + 1);
		Target = target;
	}
}

ClientChatPacketClass::ClientChatPacketClass(RawPacketClass *raw) :
	Target(0),
	Owned(true)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Size, sizeof(Size));
	String = new char[Size];
	raw->Get(String, Size);
	raw->Get(&TargetSize, sizeof(TargetSize));
	if (TargetSize != 0) {
		Target = new char[TargetSize];
		raw->Get(Target, TargetSize);
	}
}

ClientChatPacketClass::~ClientChatPacketClass()
{
	if (Owned) {
		if (String != 0) {
			delete [] String;
			String = 0;
		}
		if (Target != 0) {
			delete [] Target;
			Target = 0;
		}
	}
}

RawPacketClass *ClientChatPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(51, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Size, sizeof(Size));
	raw->Add(String, Size);
	raw->Add(&TargetSize, sizeof(TargetSize));
	if (Target != 0) {
		raw->Add(Target, TargetSize);
	}
	return(raw);
}

int ClientChatPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Type) + sizeof(Size) + Size + sizeof(TargetSize) + TargetSize);
}

int ClientChatPacketClass::Print(char *buffer, int)
{
	char *type = ChatTypeNames[Type];
	char text[128];
	short len = Size;
	if (len >= 128) {
		len = 128;
	}
	strncpy(text, String, len);
	return(sprintf(buffer, "%s {GameID=%d Type=%s String=%s Size=%hd Target=%s}", "Client_Chat", GameID, type, text, Size, Target));
}

ClientChatRequestPacketClass::ClientChatRequestPacketClass(int unknown10, int unknown0c) :
	Unknown0C(unknown0c),
	Unknown10(unknown10),
	Unknown44(0),
	Unknown48(0),
	Unknown4C(false)
{
}

ClientChatRequestPacketClass::ClientChatRequestPacketClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(-1),
	Unknown44(0),
	Unknown48(0),
	Unknown4C(false)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(&Unknown14);
	raw->Get(&Unknown24);
	raw->Get(&Unknown34);
	raw->Get(&Unknown48, sizeof(Unknown48));
	if (Unknown48 != 0) {
		Unknown4C = true;
		Unknown44 = new char[Unknown48];
		raw->Get(Unknown44, Unknown48);
	}
}

ClientChatRequestPacketClass::~ClientChatRequestPacketClass()
{
	if (Unknown4C && Unknown44 != 0) {
		delete [] Unknown44;
		Unknown44 = 0;
	}
}

RawPacketClass *ClientChatRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(163, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown14);
	raw->Add(Unknown24);
	raw->Add(Unknown34);
	raw->Add(&Unknown48, sizeof(Unknown48));
	if (Unknown48 != 0) {
		raw->Add(Unknown44, Unknown48);
	}
	return(raw);
}

int ClientChatRequestPacketClass::Get_Data_Size()
{
	int size = sizeof(Unknown0C) + sizeof(Unknown10);
	size += RawPacketClass::Get_Size(Unknown14);
	size += RawPacketClass::Get_Size(Unknown24);
	size += RawPacketClass::Get_Size(Unknown34);
	return(size + sizeof(Unknown48) + Unknown48);
}

int ClientChatRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ClientChatRequestPacketClass"));
}

ClientChatEventPacketClass::ClientChatEventPacketClass(int unknown0c) :
	Unknown0C(unknown0c),
	Unknown10(0),
	Unknown74(0),
	Unknown78(0),
	Unknown7C(false)
{
}


ClientChatEventPacketClass::ClientChatEventPacketClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(0),
	Unknown74(0),
	Unknown78(0),
	Unknown7C(false)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(&Unknown14);
	raw->Get(&Unknown24);
	raw->Get(&Unknown34);
	raw->Get(&Unknown44);
	raw->Get(&Unknown54);
	raw->Get(&Unknown64);
	raw->Get(&Unknown78, sizeof(Unknown78));
	if (Unknown78 != 0) {
		Unknown7C = true;
		Unknown74 = new char[Unknown78];
		raw->Get(Unknown74, Unknown78);
	}
}

ClientChatEventPacketClass::~ClientChatEventPacketClass()
{
	if (Unknown7C && Unknown74 != 0) {
		delete [] Unknown74;
		Unknown74 = 0;
	}
}

RawPacketClass *ClientChatEventPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(165, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown14);
	raw->Add(Unknown24);
	raw->Add(Unknown34);
	raw->Add(Unknown44);
	raw->Add(Unknown54);
	raw->Add(Unknown64);
	raw->Add(&Unknown78, sizeof(Unknown78));
	if (Unknown74 != 0) {
		raw->Add(Unknown74, Unknown78);
	}
	return(raw);
}

int ClientChatEventPacketClass::Get_Data_Size()
{
	int size = sizeof(Unknown0C) + sizeof(Unknown10);
	size += RawPacketClass::Get_Size(Unknown14);
	size += RawPacketClass::Get_Size(Unknown24);
	size += RawPacketClass::Get_Size(Unknown34);
	size += RawPacketClass::Get_Size(Unknown44);
	size += RawPacketClass::Get_Size(Unknown54);
	size += RawPacketClass::Get_Size(Unknown64);
	return(size + sizeof(Unknown78) + Unknown78);
}

int ClientChatEventPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ClientChatEventPacketClass"));
}

ClientChatListPacketClass::ClientChatListPacketClass(int unknown0c, const std::string &unknown10) :
	Unknown0C(unknown0c),
	Unknown10(unknown10)
{
}

ClientChatListPacketClass::ClientChatListPacketClass(RawPacketClass *raw) :
	Unknown0C(-1)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10);
	unsigned int count = 0;
	raw->Get(&count, sizeof(count));
	Unknown20.reserve(count);
	unsigned int i;
	for (i = 0; i < count; i++) {
		std::string name;
		raw->Get(&name);
		Unknown20.push_back(name);
	}
	count = 0;
	raw->Get(&count, sizeof(count));
	Unknown30.reserve(count);
	for (i = 0; i < count; i++) {
		std::string name;
		raw->Get(&name);
		Unknown30.push_back(name);
	}
}

RawPacketClass *ClientChatListPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(164, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(Unknown10);
	int count = Unknown20.size();
	raw->Add(&count, sizeof(count));
	std::vector<std::string>::iterator it;
	for (it = Unknown20.begin(); it != Unknown20.end(); ++it) {
		raw->Add(*it);
	}
	count = Unknown30.size();
	raw->Add(&count, sizeof(count));
	for (it = Unknown30.begin(); it != Unknown30.end(); ++it) {
		raw->Add(*it);
	}
	return(raw);
}

int ClientChatListPacketClass::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown10) + sizeof(Unknown0C) + sizeof(int);
	std::vector<std::string>::iterator it;
	for (it = Unknown20.begin(); it != Unknown20.end(); ++it) {
		size += RawPacketClass::Get_Size(*it);
	}
	size += sizeof(int);
	for (it = Unknown30.begin(); it != Unknown30.end(); ++it) {
		size += RawPacketClass::Get_Size(*it);
	}
	return(size);
}

int ClientChatListPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ClientChatListPacketClass"));
}

ClientChatErrorPacketClass::ClientChatErrorPacketClass(int unknown0c) :
	Unknown0C(unknown0c),
	Unknown10(-1),
	Unknown14(""),
	Unknown24(""),
	Unknown34("")
{
}

ClientChatErrorPacketClass::ClientChatErrorPacketClass(RawPacketClass *raw) :
	Unknown0C(1),
	Unknown10(-1),
	Unknown14(""),
	Unknown24(""),
	Unknown34("")
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(&Unknown14);
	raw->Get(&Unknown24);
	raw->Get(&Unknown34);
}

RawPacketClass *ClientChatErrorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(166, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown14);
	raw->Add(Unknown24);
	raw->Add(Unknown34);
	return(raw);
}

int ClientChatErrorPacketClass::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown14) + sizeof(Unknown0C) + sizeof(Unknown10);
	size += RawPacketClass::Get_Size(Unknown24);
	return(size + RawPacketClass::Get_Size(Unknown34));
}

int ClientChatErrorPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ClientChatErrorPacketClass"));
}

/*
**	Reconstruction notes (pkclientchat.cpp)
**
**	* Client 0x8785B0-0x87A910 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320): the chat packets 51, 163, 165, 164, 166 in that order. Opcodes 51, 163
**	  and 165 (CLIENT_CHAT_EVENT, the class named by its Print) written by hand (builder P3, 2026-10-26),
**	  18 of 18 exact in appfile.py match: length-counted strings (51) or std::strings and a
**	  length-counted buffer the packet owns once it has read one (163, 165). The value constructors
**	  count a string before storing it (strlen first: the zero store of the initialiser stays).
**	  Get_Data_Size adds one string per statement and the buffer's count and bytes last.
**	* Opcodes 164 (two std::vector<std::string>, a 4-byte count each; the raw constructor zeroes the
**	  count again before the second) and 166 (three strings, "" by default) by builder P4 (2026-10-26),
**	  10 of 10 exact. 163's
**	  0x878D90 (it sets the buffer from another object's virtual +4 and member +4) is not rebuilt.
**	* The file name is provisional (no assert names it). Not rebuilt yet: opcode 164, 166 (vectors of strings)
**	  (declared in pk_classes.h).
*/
