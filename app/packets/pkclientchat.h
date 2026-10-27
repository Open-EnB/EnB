/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkclientchat.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8785B0-0x87A910 rebuilt so far (opcode 165); the file's other classes
**	(51, 163, 164, 166) are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCLIENTCHAT_H  // NAME 1 [pkclientchat]: provisional, pk + the object's chat classes (CLIENT_CHAT 51 first)
#define PKCLIENTCHAT_H

#include "pk.h"

// opcode 165 (0x00A5, CLIENT_CHAT_EVENT): vtable 0xB09E70, constructor 0x8790B0, 0x80 bytes
class ClientChatEventPacketClass : public PacketClass
{
public:
	ClientChatEventPacketClass(RawPacketClass *raw);	// 0x8790B0
	virtual ~ClientChatEventPacketClass();	// +0x00 0x879080 / 0x879280
	virtual RawPacketClass *Build();	// +0x04 0x879430
	virtual bool Process_Packet(void *client);	// +0x08 0x73B150 (not rebuilt)
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
};

#endif
