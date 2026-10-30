// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkloothulkpermission.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8737A0-0x8739D0 rebuilt so far (opcode 140); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKLOOTHULKPERMISSION_H  // NAME 1 [pkloothulkpermission]: provisional, pk + its first named class's name
#define PKLOOTHULKPERMISSION_H

#include "pk.h"

// opcode 140 (0x008C, LOOT_HULK_PERMISSION): vtable 0xB09AE4, constructor 0x873830, 0x10 bytes
class LootHulkPermissionPacketClass : public PacketClass
{
public:
	LootHulkPermissionPacketClass(int hulk_game_id);	// 0x8737A0
	LootHulkPermissionPacketClass(RawPacketClass *raw);	// 0x873830
	virtual RawPacketClass *Build();	// +0x04 0x8738C0
	virtual bool Process_Packet(void *client);	// +0x08 0x7396F0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x873980
	virtual int Print(char *buffer, int len);	// +0x10 0x8739A0

	int HulkGameID;	// +0x0C  // NAME 3: the client's Print text 'HulkGameID='
};

#endif
