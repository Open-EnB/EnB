// Earth & Beyond client (reconstruction): app\packets\pktask.cpp. Notes at the end of the file.
#include "pktask.h"
#include "pkguildplayeronline.h"
#include "..\shared\unknown_debug.h"
#include "nstrdup.h"
#include <string.h>
#include <stdio.h>


Unknown1388Struct::Unknown1388Struct() :
	Unknown00(0),
	Unknown04(0),
	Unknown08(0)
{
	Unknown0A[0] = 0;
	Unknown10A[0] = 0;
}

Unknown1388Struct::Unknown1388Struct(int unknown00, int unknown04, short unknown08, const char *unknown0a, const char *unknown10a, int unknown20c) :
	Unknown00(unknown00),
	Unknown04(unknown04),
	Unknown08(unknown08),
	Unknown20C(unknown20c)
{
	Unknown0A[0] = 0;
	Unknown10A[0] = 0;
	if (unknown0a) {
		strncpy(Unknown0A, unknown0a, sizeof(Unknown0A));
	}
	if (unknown10a) {
		strncpy(Unknown10A, unknown10a, sizeof(Unknown10A));
	}
}

Unknown1388Struct::Unknown1388Struct(const Unknown1388Struct &that)
{
	Unknown00 = that.Unknown00;
	Unknown04 = that.Unknown04;
	Unknown08 = that.Unknown08;
	strncpy(Unknown0A, that.Unknown0A, sizeof(Unknown0A));
	strncpy(Unknown10A, that.Unknown10A, sizeof(Unknown10A));
	Unknown20C = that.Unknown20C;
}

Unknown1388Struct &Unknown1388Struct::operator=(const Unknown1388Struct &that)
{
	Unknown00 = that.Unknown00;
	Unknown04 = that.Unknown04;
	Unknown08 = that.Unknown08;
	strncpy(Unknown0A, that.Unknown0A, sizeof(Unknown0A));
	strncpy(Unknown10A, that.Unknown10A, sizeof(Unknown10A));
	Unknown20C = that.Unknown20C;
	return(*this);
}

Unknown1389Struct::Unknown1389Struct() :
	Unknown210(0),
	Unknown214(0)
{
}

Unknown1389Struct::Unknown1389Struct(int unknown00, int unknown04, short unknown08, const char *unknown0a, const char *unknown10a, int unknown210, int unknown214) :
	Unknown1388Struct(unknown00, unknown04, unknown08, unknown0a, unknown10a, unknown214),
	Unknown210(unknown210),
	Unknown214(unknown214)
{
}

Unknown1389Struct::Unknown1389Struct(const Unknown1389Struct &that)
{
	*this = that;
}

Unknown1389Struct &Unknown1389Struct::operator=(const Unknown1389Struct &that)
{
	Unknown1388Struct::operator=(that);
	Unknown210 = that.Unknown210;
	Unknown214 = that.Unknown214;
	return(*this);
}

Packet_1388Class::Packet_1388Class(const Unknown1388Struct &data) :
	Data(data)
{
}

Packet_1388Class::Packet_1388Class(RawPacketClass *raw)
{
	raw->Get(&Data, sizeof(Data));
}

RawPacketClass *Packet_1388Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Data, sizeof(Data));
	return(raw);
}

int Packet_1388Class::Get_Type()
{
	return(5000);
}

int Packet_1388Class::Get_Data_Size()
{
	return(sizeof(Data));
}

int Packet_1388Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Task Hello"));
}

Packet_1389Class::Packet_1389Class(const Unknown1389Struct &data) :
	Data(data)
{
}

Packet_1389Class::Packet_1389Class(RawPacketClass *raw)
{
	raw->Get(&Data, sizeof(Data));
}

RawPacketClass *Packet_1389Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Data, sizeof(Data));
	return(raw);
}

int Packet_1389Class::Get_Type()
{
	return(5001);
}

int Packet_1389Class::Get_Data_Size()
{
	return(sizeof(Data));
}

int Packet_1389Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Sector Hello"));
}

Packet_138AClass::Packet_138AClass(int type, int num) : Type(type), Num(num)
{
}

Packet_138AClass::Packet_138AClass(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Num, sizeof(Num));
}

RawPacketClass *Packet_138AClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Num, sizeof(Num));
	return(raw);
}

int Packet_138AClass::Get_Type()
{
	return(5002);
}

int Packet_138AClass::Get_Data_Size()
{
	return(sizeof(Type) + sizeof(Num));
}

int Packet_138AClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Type=%d Num=%d}", "Task Start", Type, Num));
}

Packet_138BClass::Packet_138BClass(int type, int num) : Type(type), Num(num)
{
}

Packet_138BClass::Packet_138BClass(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Num, sizeof(Num));
}

RawPacketClass *Packet_138BClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Num, sizeof(Num));
	return(raw);
}

int Packet_138BClass::Get_Type()
{
	return(5003);
}

int Packet_138BClass::Get_Data_Size()
{
	return(sizeof(Type) + sizeof(Num));
}

int Packet_138BClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Type=%d Num=%d}", "Task Stop", Type, Num));
}

Packet_138CClass::Packet_138CClass(int type, int num) :
	Packet_138AClass(type, num)
{
	Unknown14 = 0;
}

Packet_138CClass::Packet_138CClass(RawPacketClass *raw) :
	Packet_138AClass(raw)
{
	Unknown14 = 0;
}

int Packet_138CClass::Get_Type()
{
	return(5004);
}

int Packet_138CClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Type=%d Num=%d}", "Task Request", Type, Num));
}

Packet_138DClass::Packet_138DClass(int ip, unsigned short port, char *string) :
	Ip(ip),
	Port(port)
{
	Size = strlen(string) + 1;
	String = nstrdup(string);
}

Packet_138DClass::Packet_138DClass(RawPacketClass *raw)
{
	raw->Get(&Ip, sizeof(Ip));
	raw->Get(&Port, sizeof(Port));
	raw->Get(&Size, sizeof(Size));
	String = new char[Size];
	raw->Get(String, Size);
}

Packet_138DClass::~Packet_138DClass()
{
	delete [] String;
	String = NULL;
}

RawPacketClass *Packet_138DClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Ip, sizeof(Ip));
	raw->Add(&Port, sizeof(Port));
	raw->Add(&Size, sizeof(Size));
	raw->Add(String, Size);
	return(raw);
}

int Packet_138DClass::Get_Type()
{
	return(5005);
}

int Packet_138DClass::Get_Data_Size()
{
	return(Size + sizeof(Ip) + sizeof(Port) + sizeof(Size));
}

int Packet_138DClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Ip=%x Port=%hx}", "Task Ping", Ip, Port));
}

Packet_138EClass::Packet_138EClass(int ip, unsigned short port) :
	Packet_138DClass(ip, port, "")
{
}

Packet_138EClass::Packet_138EClass(RawPacketClass *raw) :
	Packet_138DClass(raw)
{
}

int Packet_138EClass::Get_Type()
{
	return(5006);
}

int Packet_138EClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Ip=%x Port=%hx}", "Task Pong", Ip, Port));
}














































































































Packet_1391Class::Packet_1391Class(char *name, __int64 character_object_id, char *account, __int64 account_id, int ip, unsigned short port, int sector, int status, __int64 unknown38) :
	Name(NULL),
	CharacterObjectID(character_object_id),
	Account(NULL),
	AccountID(account_id),
	Ip(ip),
	Port(port),
	Sector(sector),
	Status(status),
	Unknown38(unknown38)
{
	G_ASSERT(name && account);
	Name = nstrdup(name);
	Account = nstrdup(account);
	G_ASSERT(Name && Account);
}

Packet_1391Class::Packet_1391Class(RawPacketClass *raw)
{
	raw->Get(&Name);
	raw->Get(&CharacterObjectID, sizeof(CharacterObjectID));
	raw->Get(&Account);
	raw->Get(AccountID);
	raw->Get(&Ip, sizeof(Ip));
	raw->Get(&Port, sizeof(Port));
	raw->Get(Sector);
	raw->Get(&Status, sizeof(Status));
	raw->Get(&Unknown38, sizeof(Unknown38));
}


Packet_1391Class::~Packet_1391Class()
{
	G_ASSERT(Name && Account);
	delete [] Name;
	delete [] Account;
	Account = NULL;
	Name = NULL;
}

RawPacketClass *Packet_1391Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Name);
	raw->Add(&CharacterObjectID, sizeof(CharacterObjectID));
	raw->Add(Account);
	raw->Add(AccountID);
	raw->Add(&Ip, sizeof(Ip));
	raw->Add(&Port, sizeof(Port));
	raw->Add(Sector);
	raw->Add(&Status, sizeof(Status));
	raw->Add(&Unknown38, sizeof(Unknown38));
	return(raw);
}

int Packet_1391Class::Get_Type()
{
	return(5009);
}

int Packet_1391Class::Get_Data_Size()
{
	return(strlen(Account) + strlen(Name) + 2 * (sizeof(int) + 1) + sizeof(CharacterObjectID) + sizeof(AccountID) + sizeof(Ip) + sizeof(Port) + sizeof(Sector) + sizeof(Status) + sizeof(Unknown38));
}

int Packet_1391Class::Print(char *buffer, int)
{
	char object_id[32];
	char account_id[32];
	Unknown_8B44E0(CharacterObjectID, object_id);
	Unknown_8B44E0(AccountID, account_id);
	return(sprintf(buffer, "%s { Name=%s CharacterObjectID=%s Account=%s AccountID=%s Ip=%x Port=%hx Sector=%d Status=%d }", "PlayerStatus", Name, object_id, Account, account_id, Ip, Port, Sector, Status));
}

Packet_1392Class::Packet_1392Class(const std::list<Unknown1389Struct> &list) :
	List(list)
{
}

Packet_1392Class::Packet_1392Class(RawPacketClass *raw)
{
	List.clear();
	int count = 0;
	raw->Get(&count, sizeof(count));
	Unknown1389Struct entry;
	for (int i = 0; i < count; i++) {
		raw->Get(&entry, sizeof(entry));
		List.push_back(entry);
	}
}

RawPacketClass *Packet_1392Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	int count = List.size();
	raw->Add(&count, sizeof(count));
	for (std::list<Unknown1389Struct>::iterator it = List.begin(); it != List.end(); ++it) {
		raw->Add(&*it, sizeof(*it));
	}
	return(raw);
}

int Packet_1392Class::Get_Type()
{
	return(5010);
}

int Packet_1392Class::Get_Data_Size()
{
	return(List.size() * sizeof(Unknown1389Struct) + sizeof(int));
}

int Packet_1392Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "SectorList"));
}

Packet_1393Class::Packet_1393Class()
{
}

Packet_1393Class::Packet_1393Class(RawPacketClass *)
{
}

RawPacketClass *Packet_1393Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	return(raw);
}

int Packet_1393Class::Get_Type()
{
	return(5011);
}

int Packet_1393Class::Get_Data_Size()
{
	return(0);
}

int Packet_1393Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Request Sector List"));
}


/*
**	Reconstruction notes (pktask.cpp)
**
**	* Client 0x8A8610-0x8AAA80 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* Opcodes 5004 (Packet_138AClass's derived class), 5005, 5006 (its derived class) and 5009 written by hand (builder P4, 2026-10-27);
**	  5009's asserts sit on the client's lines 422, 425 and 444 (blank lines above them).
**	* Opcodes 5000, 5001 and 5010 with their two structs written by hand (builder P5, 2026-10-27): the
**	  structs' members are out of line at the top of the object (0x8A8610-0x8A8970), each the client's
**	  first function; the copy constructor and operator= of the 0x210-byte struct are the same bytes
**	  (0x8A86D0 / 0x8A8750: one pattern at two addresses, not counted), and so are std::list's erase
**	  0x8AAA30 and its _Tidy-like clear 0x8AA9C0 for every POD list (D54).
*/
