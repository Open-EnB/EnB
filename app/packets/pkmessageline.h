// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmessageline.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89D730-0x89E0F0 (opcodes 36 and 35) and their
**	MessageLinePacketClass with its message lines 32-34.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMESSAGELINE_H  // NAME 1 [pkmessageline]: provisional, pk + the object's MessageLinePacketClass
#define PKMESSAGELINE_H

#include "pk.h"
#include <limits.h>

// the base of PriorityMessageLinePacketClass, QueueMessageLinePacketClass, PushMessageLinePacketClass
// (opcodes 32-34, below): vtable 0xB0B3A4, constructor 0x89D8A0, 0x1C bytes
class MessageLinePacketClass : public PacketClass  // NAME 3: by convention from its derived classes PriorityMessageLinePacketClass, QueueMessageLinePacketClass, PushMessageLinePacketClass
{
public:
	MessageLinePacketClass(char *string, char *ephemeral_type, __int64 time_ms, int display_priority);	// 0x89D760
	MessageLinePacketClass(RawPacketClass *raw);	// 0x89D8A0
	virtual ~MessageLinePacketClass();	// +0x00 0x89D870 / 0x89D970
	virtual RawPacketClass *Build();	// +0x04 0x89D9C0
	virtual int Get_Data_Size();	// +0x0C 0x89DAB0
	virtual int Print(char *buffer, int len);	// +0x10 0x89DAF0
	virtual int Get_Channel() { return(-2); }	// +0x18 0x86E6A0
	virtual int Get_Priority() { return(0); }	// +0x1C 0x86E680
	virtual int Get_Type() = 0;	// +0x28  // NAME 2: what the derived classes' return (their opcode)
	virtual const char *Get_Name() = 0;	// +0x2C  // NAME 2: what the derived classes' return (a name)
	char *Get_String() { return(String); }	// inline (the handlers 32-34 read it)  // NAME 2: what it returns
	int Get_Time_MS() { return(TimeMS); }	// inline  // NAME 2: what it returns
	char *Get_Ephemeral_Type() { return(Ephemeral_Type); }	// inline  // NAME 2: what it returns
	int Get_Display_Priority() { return(Display_Priority); }	// inline  // NAME 2: what it returns

	char *String;	// +0x0C (new[]'d)  // NAME 3: the client's Print text 'String='
	char *Ephemeral_Type;	// +0x10 (new[]'d)  // NAME 3: the client's Print text 'Ephemeral_Type='
	int TimeMS;	// +0x14  // NAME 3: the client's Print text 'TimeMS='
	int Display_Priority;	// +0x18  // NAME 3: the client's Print text 'Display_Priority='
};

// The three message lines: their constructors are inlined over the base's where the packet switch
// builds them, so their inline virtuals and deleting destructors are pk.cpp's (0x86E6C0-0x86E850).
// opcode 32 (0x0020, PRIORITY_MESSAGE): vtable 0xB09760, constructor 0x89D8A0, 0x1C bytes
class PriorityMessageLinePacketClass : public MessageLinePacketClass
{
public:
	PriorityMessageLinePacketClass(RawPacketClass *raw) : MessageLinePacketClass(raw) {}	// inline (the case stores the vtable)
	virtual bool Process_Packet(void *client);	// +0x08 0x736A20 (cpacket.cpp)
	virtual int Get_Type() { return(0x20); }	// +0x28 0x86E6C0
	virtual const char *Get_Name() { return("PriorityMessage"); }	// +0x2C 0x86E6E0
};

// opcode 33 (0x0021, ?): vtable 0xB09724, constructor 0x89D8A0, 0x1C bytes
class QueueMessageLinePacketClass : public MessageLinePacketClass
{
public:
	QueueMessageLinePacketClass(RawPacketClass *raw) : MessageLinePacketClass(raw) {}	// inline (the case stores the vtable)
	virtual bool Process_Packet(void *client);	// +0x08 0x736AE0 (cpacket.cpp)
	virtual int Get_Type() { return(0x21); }	// +0x28 0x86E750
	virtual const char *Get_Name() { return("QueueMessage"); }	// +0x2C 0x86E770
};

// opcode 34 (0x0022, PUSH_MESSAGE): vtable 0xB096E8, constructor 0x89D8A0, 0x1C bytes
class PushMessageLinePacketClass : public MessageLinePacketClass
{
public:
	PushMessageLinePacketClass(RawPacketClass *raw) : MessageLinePacketClass(raw) {}	// inline (the case stores the vtable)
	virtual bool Process_Packet(void *client);	// +0x08 0x736BA0 (cpacket.cpp)
	virtual int Get_Type() { return(0x22); }	// +0x28 0x86E7E0
	virtual const char *Get_Name() { return("PushMessage"); }	// +0x2C 0x86E800
};


// opcode 36 (0x0024, ?): vtable 0xB0B3E0, constructor 0x89DC60, 0xC bytes
class ClearMessageLinePacketClass : public PacketClass
{
public:
	ClearMessageLinePacketClass();	// 0x89DBB0
	ClearMessageLinePacketClass(RawPacketClass *raw);	// 0x89DC60
	virtual RawPacketClass *Build();	// +0x04 0x89DCC0
	virtual bool Process_Packet(void *client);	// +0x08 0x736D00 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89DD40
	virtual int Print(char *buffer, int len);	// +0x10 0x89DD60
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89DC10
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89DBF0
};

// opcode 35 (0x0023, ?): vtable 0xB0B410, constructor 0x89DEB0, 0x10 bytes
class SuperLinePacketClass : public PacketClass
{
public:
	SuperLinePacketClass(char *string);	// 0x89DD90 (a copy: nstrdup)
	SuperLinePacketClass(RawPacketClass *raw);	// 0x89DEB0
	virtual ~SuperLinePacketClass();	// +0x00 0x89DE80 / 0x89DF40
	virtual RawPacketClass *Build();	// +0x04 0x89DF80
	virtual bool Process_Packet(void *client);	// +0x08 0x736C60 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89E050
	virtual int Print(char *buffer, int len);	// +0x10 0x89E080
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89DE40
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89DE20
	virtual int Get_Type() { return(35); }	// +0x28 0x89DE60  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's
	char *Get_String() { return(String); }	// inline (the handler 35 reads it)  // NAME 2: what it returns

	char *String;	// +0x0C (new[]'d, a 0-terminated string)  // NAME 3: the client's Print text 'String='
};

#endif
