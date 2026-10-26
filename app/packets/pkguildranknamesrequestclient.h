/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkguildranknamesrequestclient.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x88BD40-0x88EC40 rebuilt so far (opcode 212); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGUILDRANKNAMESREQUESTCLIENT_H  // NAME 1 [pkguildranknamesrequestclient]: provisional, pk + its first named class's name
#define PKGUILDRANKNAMESREQUESTCLIENT_H

#include "pk.h"

// opcode 212 (0x00D4, GUILD_RANK_NAMES_REQUEST_CLIENT): vtable 0xB0A9D4, constructor 0x88D8C0, 0x10 bytes
class GuildRankNamesRequestClientPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildRankNamesRequestClient {' and the PacketClass convention
{
public:
	GuildRankNamesRequestClientPacketClass(int player_id);	// 0x88D850
	GuildRankNamesRequestClientPacketClass(RawPacketClass *raw);	// 0x88D8C0
	virtual RawPacketClass *Build();	// +0x04 0x88D950
	virtual bool Process_Packet(void *client);	// +0x08 0x8691B0 (clientstub.cpp l.383)
	virtual int Get_Data_Size();	// +0x0C 0x88DA10
	virtual int Print(char *buffer, int len);	// +0x10 0x88DA30

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
};

#endif
