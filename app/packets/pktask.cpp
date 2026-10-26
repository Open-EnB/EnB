// Earth & Beyond client (reconstruction): app\packets\pktask.cpp. Notes at the end of the file.
#include "pktask.h"
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
**	* Not rebuilt yet: opcode 5000, 5001, 5004, 5005, 5006, 5009, 5010 (declared in pk_classes.h).
*/
