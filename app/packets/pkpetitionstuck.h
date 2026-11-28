// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkpetitionstuck.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x89F740-0x89FB20 (opcode 136).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKPETITIONSTUCK_H  // NAME 1 [pkpetitionstuck]: provisional, pk + its class's name
#define PKPETITIONSTUCK_H

#include "pk.h"

// opcode 136 (0x0088, ?): vtable 0xB0B594, constructor 0x89F860, 0x20 bytes
class PetitionPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	PetitionPacketClass(int game_id, int game_id2, const char *subject, const char *complaint, const char *player_list);	// 0x89F740
	PetitionPacketClass(PacketClass *raw);	// 0x89F860
	virtual ~PetitionPacketClass();	// +0x00 0x89F830 / 0x89F930
	virtual PacketClass *Encode();	// +0x04 0x89F9A0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x868A00 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89FA90
	virtual int Print(char *buffer, int len);	// +0x10 0x89FAE0

	int GameID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'GameID='
	int GameID2;	// +0x10 (4 bytes as they are)  // NAME 2: Print's second 'GameID='
	char *Subject;	// +0x14 (nstrdup'd)  // NAME 3: the client's Print text 'Subject ='
	char *Complaint;	// +0x18 (nstrdup'd)  // NAME 3: the client's Print text 'Complaint ='
	char *PlayerList;	// +0x1C (nstrdup'd)  // NAME 3: the client's Print text 'PlayerList ='
};

#endif
