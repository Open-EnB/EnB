/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pktalktreeaction.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A6A80-0x8A7E70 rebuilt so far (opcode 86, 85); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKTALKTREEACTION_H  // NAME 1 [pktalktreeaction]: provisional, pk + its first named class's name
#define PKTALKTREEACTION_H

#include "pk.h"
#include <string>
#include <vector>

// One response of TALK_TREE 84: 0x14 bytes (an int sent as its 4 bytes, the text as a 0-terminated string)
struct TalkTreeResponseStruct  // NAME 2: Print's 'NumResponses' counts them
{
	TalkTreeResponseStruct(int unknown00, const std::string &text) : Unknown00(unknown00), Text(text) {}

	int Unknown00;	// +0x00  // NAME 1: neutral (D10)
	std::string Text;	// +0x04  // NAME 2: what it holds
};

// opcode 84 (0x0054, TALK_TREE): vtable 0xB0BC60, constructor 0x8A6CA0, 0x3C bytes
class TalkTreePacketClass : public PacketClass  // NAME 3: the client's Print text 'TalkTreePacket'
{
public:
	TalkTreePacketClass(const std::string &npc_say, const std::string &npc_sound_file_name);	// 0x8A6A80
	TalkTreePacketClass(RawPacketClass *raw);	// 0x8A6CA0
	virtual ~TalkTreePacketClass();	// +0x00 0x8A6B90 / 0x8A7040
	virtual RawPacketClass *Build();	// +0x04 0x8A7180
	virtual bool Process_Packet(void *client);	// +0x08 0x738670 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A7300
	virtual int Print(char *buffer, int len);	// +0x10 0x8A7340
	virtual int Get_Priority() { return(0); }	// +0x1C 0x8A6B70

	void Add_Response(int unknown00, const std::string &text);	// 0x8A6BC0 (push_back)  // NAME 2: what it does
	void Swap_Responses(std::vector<TalkTreeResponseStruct> &responses);	// 0x8A7160 (the UI's 0x64CC90)  // NAME 2: what it does

	std::string NPCSay;	// +0x0C  // NAME 3: the client's Print text 'NPCSay='
	std::string NPCSoundFileName;	// +0x1C  // NAME 3: the client's Print text 'NPCSoundFileName'
	std::vector<TalkTreeResponseStruct> Responses;	// +0x2C (a 1-byte count, then each response)  // NAME 3: the client's Print text 'NumResponses='
};

// opcode 86 (0x0056, TALK_TREE_ACTION): vtable 0xB0BC90, constructor 0x8A7430, 0x10 bytes
class TalkTreeActionPacketClass : public PacketClass
{
public:
	TalkTreeActionPacketClass(int action);	// 0x8A73C0
	TalkTreeActionPacketClass(RawPacketClass *raw);	// 0x8A7430
	virtual RawPacketClass *Build();	// +0x04 0x8A74E0
	virtual bool Process_Packet(void *client);	// +0x08 0x738730 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A75A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A75C0

	int Action;	// +0x0C  // NAME 3: the client's Print text 'Action='
};

// opcode 85 (0x0055, SELECT_TALK_TREE): vtable 0xB0BCC0, constructor 0x8A7670, 0x14 bytes
class SelectTalkTreePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name SELECT_TALK_TREE (the Net-7 opcode table; no structure check)
{
public:
	SelectTalkTreePacketClass(RawPacketClass *raw);	// 0x8A7670
	virtual RawPacketClass *Build();	// +0x04 0x8A7710
	virtual bool Process_Packet(void *client);	// +0x08 0x867FB0 (clientstub.cpp l.95)
	virtual int Get_Data_Size();	// +0x0C 0x8A77E0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A7800

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
	char Selection;	// +0x10  // NAME 3: the client's Print text 'Selection='
	char Unknown11[0x3];	// +0x11
};

#endif
