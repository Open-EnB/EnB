// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkverbrequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AEAE0-0x8B15C0 (opcodes 90, 91, 92): one client object, so one
**	file (pkverblist.h merged in, P-PK2-2).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKVERBREQUEST_H  // NAME 1 [pkverbrequest]: provisional, pk + its first named class's name
#define PKVERBREQUEST_H

#include "pk.h"
#include <set>
#include <string>

// opcode 90 (0x005A, VERB_REQUEST): vtable 0xB0C234, constructor 0x8AEB60, 0x18 bytes
class VerbRequestPacketClass : public PacketClass  // NAME 2: Net-7's opcode name VERB_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	VerbRequestPacketClass(int subject_id, int object_id, int action);	// 0x8AEAE0
	VerbRequestPacketClass(RawPacketClass *raw);	// 0x8AEB60
	virtual RawPacketClass *Build();	// +0x04 0x8AEC00
	virtual bool Process_Packet(void *client);	// +0x08 0x868010 (clientstub.cpp l.101)
	virtual int Get_Data_Size();	// +0x0C 0x8AED00
	virtual int Print(char *buffer, int len);	// +0x10 0x8AED20
	virtual int Get_Type();	// +0x28 0x8AECE0  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int SubjectID;	// +0x0C  // NAME 3: the client's Print text 'SubjectID='
	int ObjectID;	// +0x10  // NAME 3: the client's Print text 'ObjectID='
	int Action;	// +0x14  // NAME 3: the client's Print text 'Action='
};

// A verb (the client's Value_To_String 0x84C470 names it from its table 0xBB1218 as 'VerbEnum':
// the high 16 bits pick the verb, the low 16 bits are an argument)
enum VerbEnum  // NAME 4: the client's own label 'VerbEnum' (0x84C470's table name)
{
	VERB_NONE = 0	// NAME 1: neutral placeholder (D10)
};

char *Value_To_String(char *buf, int size, VerbEnum verb);	// 0x84C470 (not rebuilt)

// opcode 91 (0x005B, ?): vtable 0xB0C268, constructor 0x8AF150, 0x20 bytes
class VerbListPacketClass : public PacketClass  // NAME 3: the client's Print text 'VerbList'
{
public:
	VerbListPacketClass(int game_id, const std::set<VerbEnum> &verbs);	// 0x8AED60
	VerbListPacketClass(RawPacketClass *raw);	// 0x8AF150
	virtual RawPacketClass *Build();	// +0x04 0x8AF550
	virtual bool Process_Packet(void *client);	// +0x08 0x7387F0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8AF6B0
	virtual int Print(char *buffer, int len);	// +0x10 0x8AF6D0
	virtual int Get_Type() { return(91); }	// +0x28 0x8AF690

	int GameID;	// +0x0C (network order)  // NAME 3: the client's Print text 'GameID='
	std::set<VerbEnum> Verbs;	// +0x10  // NAME 3: the client's Print text 'Verbs='
};

// opcode 92 (0x005C, VERB_UPDATE): vtable 0xB0C29C, constructor 0x8AFD10, 0x30 bytes
class VerbUpdatePacketClass : public PacketClass
{
public:
	VerbUpdatePacketClass(RawPacketClass *raw);	// 0x8AFD10
	virtual RawPacketClass *Build();	// +0x04 0x8AFF40
	virtual bool Process_Packet(void *client);	// +0x08 0x7388B0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8B0130
	virtual int Print(char *buffer, int len);	// +0x10 0x8B0150
	virtual int Get_Type() { return(92); }	// +0x28 0x8B0110

	int GameID;	// +0x0C (network order)  // NAME 3: the client's Print text 'GameID='
	std::set<VerbEnum> Drops;	// +0x10  // NAME 3: the client's Print text 'Drops=' (the first string it prints)
	std::set<VerbEnum> Adds;	// +0x20  // NAME 3: the client's Print text 'Adds='
};

#endif
