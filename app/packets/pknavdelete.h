/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pknavdelete.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89E8C0-0x89EEF0 rebuilt so far (opcode 153, 154); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKNAVDELETE_H  // NAME 1 [pknavdelete]: provisional, pk + its first named class's name
#define PKNAVDELETE_H

#include "pk.h"

// opcode 153 (0x0099, NAVIGATION): vtable 0xB0B4A4, constructor 0x89E9F0, 0x24 bytes
class NavigationPacketClass : public PacketClass
{
public:
	NavigationPacketClass(int game_id, float signature, bool player_has_visited, int nav_type, bool is_huge);	// 0x89E8C0
	NavigationPacketClass(RawPacketClass *raw);	// 0x89E9F0
	virtual RawPacketClass *Build();	// +0x04 0x89EAE0
	virtual bool Process_Packet(void *client);	// +0x08 0x73A600 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x89EBD0
	virtual int Print(char *buffer, int len);	// +0x10 0x89EBF0
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x89E980

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	float Signature;	// +0x10  // NAME 3: the client's Print text 'Signature='
	bool PlayerHasVisited;	// +0x14  // NAME 3: the client's Print text 'PlayerHasVisited='
	int NavType;	// +0x18 (-1 by default)  // NAME 3: the client's Print text 'NavType='
	bool IsHuge;	// +0x1C  // NAME 3: the client's Print text 'IsHuge='
	int Channel;	// +0x20 (PacketClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
};

// opcode 154 (0x009A, ?): vtable 0xB0B4D4, constructor 0x89ED50, 0x14 bytes
class NavDeletePacketClass : public PacketClass
{
public:
	NavDeletePacketClass(int game_id);	// 0x89EC40
	NavDeletePacketClass(RawPacketClass *raw);	// 0x89ED50
	virtual RawPacketClass *Build();	// +0x04 0x89EDE0
	virtual bool Process_Packet(void *client);	// +0x08 0x73A710 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x89EEA0
	virtual int Print(char *buffer, int len);	// +0x10 0x89EEC0
	virtual int Get_Channel() { return(Unknown10); }	// +0x18 0x89ECE0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Unknown10;	// +0x10
};

#endif
