/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkclientchat.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8785B0-0x87A910 rebuilt so far (opcode 51, 163, 165, 164, 166); the file's other
**	classes (164, 166) are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCLIENTCHAT_H  // NAME 1 [pkclientchat]: provisional, pk + the object's chat classes (CLIENT_CHAT 51 first)
#define PKCLIENTCHAT_H

#include "pk.h"
#include <vector>

// opcode 51 (0x0033, CLIENT_CHAT): vtable 0xB09E10, constructor 0x8786E0, 0x20 bytes
class ClientChatPacketClass : public PacketClass  // NAME 2: Net-7's opcode name CLIENT_CHAT (the Net-7 opcode table; no structure check)
{
public:
	ClientChatPacketClass(int game_id, char *string, char type, char *target);	// 0x8785D0 (the strings not copied)
	ClientChatPacketClass(RawPacketClass *raw);	// 0x8786E0
	virtual ~ClientChatPacketClass();	// +0x00 0x8786B0 / 0x878800
	virtual RawPacketClass *Build();	// +0x04 0x878870
	virtual bool Process_Packet(void *client);	// +0x08 0x867DA0 (clientstub.cpp l.62)
	virtual int Get_Data_Size();	// +0x0C 0x878990
	virtual int Print(char *buffer, int len);	// +0x10 0x8789B0
	virtual int Get_Channel() { return(-2); }	// +0x18 0x878690
	virtual int Get_Priority() { return(0); }	// +0x1C 0x878670

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char *String;	// +0x10 (Size bytes, the text and its 0)  // NAME 3: the client's Print text 'String='
	short Size;	// +0x14  // NAME 3: the client's Print text 'Size='
	char Type;	// +0x16 (ChatTypeNames' index)  // NAME 3: the client's Print text 'Type='
	char *Target;	// +0x18 (TargetSize bytes, or none)  // NAME 3: the client's Print text 'Target='
	short TargetSize;	// +0x1C  // NAME 2: Target's Size
	bool Owned;	// +0x1E (the raw constructor new[]'d the strings: the destructor deletes them)  // NAME 2: what it means
};

// opcode 163 (0x00A3, CLIENT_CHAT_REQUEST): vtable 0xB09E40, constructor 0x878B10, 0x50 bytes
class ClientChatRequestPacketClass : public PacketClass  // NAME 2: Net-7's opcode name CLIENT_CHAT_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	ClientChatRequestPacketClass(int unknown10, int unknown0c);	// 0x878A50
	ClientChatRequestPacketClass(RawPacketClass *raw);	// 0x878B10
	virtual ~ClientChatRequestPacketClass();	// +0x00 0x878AE0 / 0x878C80
	virtual RawPacketClass *Build();	// +0x04 0x878DC0
	virtual bool Process_Packet(void *client);	// +0x08 0x868CA0 (clientstub.cpp l.302)
	virtual int Get_Data_Size();	// +0x0C 0x878EE0
	virtual int Print(char *buffer, int len);	// +0x10 0x878F30 (the class name: 'ClientChatRequestPacketClass')

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10 (-1 by default)
	std::string Unknown14;	// +0x14
	std::string Unknown24;	// +0x24
	std::string Unknown34;	// +0x34
	char *Unknown44;	// +0x44 (new[] Unknown48 bytes when Unknown48 isn't 0; 0x878D90, not rebuilt, sets it from another object)
	int Unknown48;	// +0x48 (the bytes at Unknown44)
	bool Unknown4C;	// +0x4C (Unknown44 is the packet's: the destructor deletes it)
};

// opcode 165 (0x00A5, CLIENT_CHAT_EVENT): vtable 0xB09E70, constructor 0x8790B0, 0x80 bytes
class ClientChatEventPacketClass : public PacketClass
{
public:
	ClientChatEventPacketClass(int unknown0c);	// 0x878F60
	ClientChatEventPacketClass(RawPacketClass *raw);	// 0x8790B0
	virtual ~ClientChatEventPacketClass();	// +0x00 0x879080 / 0x879280
	virtual RawPacketClass *Build();	// +0x04 0x879430
	virtual bool Process_Packet(void *client);	// +0x08 0x73B150 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x879570
	virtual int Print(char *buffer, int len);	// +0x10 0x8795F0 (the class name: 'ClientChatEventPacketClass')

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
	std::string Unknown14;	// +0x14
	std::string Unknown24;	// +0x24
	std::string Unknown34;	// +0x34
	std::string Unknown44;	// +0x44
	std::string Unknown54;	// +0x54
	std::string Unknown64;	// +0x64
	char *Unknown74;	// +0x74 (new[] Unknown78 bytes when Unknown78 isn't 0)
	int Unknown78;	// +0x78 (the bytes at Unknown74)
	bool Unknown7C;	// +0x7C (Unknown74 is the packet's: the destructor deletes it)

	const std::string &Get_Unknown14();	// 0x73CC30 (cpacket.cpp, out of line)  // NAME 1: neutral (D10): the member it returns
	const std::string &Get_Unknown54();	// 0x73CC50 (cpacket.cpp, out of line)  // NAME 1: neutral (D10): the member it returns
	const std::string &Get_Unknown64();	// 0x73CC70 (cpacket.cpp, out of line)  // NAME 1: neutral (D10): the member it returns
};

// opcode 164 (0x00A4, CLIENT_CHAT_LIST): vtable 0xB09EA0, constructor 0x879850, 0x40 bytes
class ClientChatListPacketClass : public PacketClass
{
public:
	ClientChatListPacketClass(int unknown0c, const std::string &unknown10);	// 0x879620
	ClientChatListPacketClass(RawPacketClass *raw);	// 0x879850
	virtual RawPacketClass *Build();	// +0x04 0x879BF0
	virtual bool Process_Packet(void *client);	// +0x08 0x73CC90 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x879D60
	virtual int Print(char *buffer, int len);	// +0x10 0x87A220 (the class name: 'ClientChatListPacketClass')

	int Unknown0C;	// +0x0C
	std::string Unknown10;	// +0x10
	std::vector<std::string> Unknown20;	// +0x20 (a 4-byte count, then the strings)
	std::vector<std::string> Unknown30;	// +0x30 (a 4-byte count, then the strings)
};

// opcode 166 (0x00A6, CLIENT_CHAT_ERROR): vtable 0xB09ED0, constructor 0x87A560, 0x44 bytes
class ClientChatErrorPacketClass : public PacketClass
{
public:
	ClientChatErrorPacketClass(int unknown0c);	// 0x87A250
	ClientChatErrorPacketClass(RawPacketClass *raw);	// 0x87A560
	virtual RawPacketClass *Build();	// +0x04 0x87A7B0
	virtual bool Process_Packet(void *client);	// +0x08 0x73D170 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87A8A0
	virtual int Print(char *buffer, int len);	// +0x10 0x87A8E0 (the class name: 'ClientChatErrorPacketClass')

	int Unknown0C;	// +0x0C (1 when received)
	int Unknown10;	// +0x10 (-1 by default)
	std::string Unknown14;	// +0x14
	std::string Unknown24;	// +0x24
	std::string Unknown34;	// +0x34
};

#endif
