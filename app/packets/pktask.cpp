// Earth & Beyond client (reconstruction): app\packets\pktask.cpp. Notes at the end of the file.
#include "pktask.h"
#include "nstrdup.h"
#include <string.h>
#include <stdio.h>


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
**	* Opcodes 5004 (Packet_138AClass's derived class), 5005 and 5006 (its derived class) written by hand (builder P4, 2026-10-27).
**	* Not rebuilt yet: opcode 5000, 5001, 5009, 5010 (declared in pk_classes.h).
*/
