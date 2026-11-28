// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkcreate.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87F920-0x87FCC0 rebuilt so far (opcode 4); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCREATE_H  // NAME 1 [pkcreate]: provisional, pk + its first named class's name
#define PKCREATE_H

#include "pk.h"
#include "vector3.h"

// opcode 4 (0x0004, CREATE): vtable 0xB0A140, constructor 0x87FA70, 0x28 bytes
class CreatePacketClass : public UnknownPacketBaseClass
{
public:
	CreatePacketClass(int game_id, short base_asset, char type, float scale, int unknown, const Vector3 &hsv, int channel);	// 0x87F920  // NAME 2: the stores (no caller left)
	CreatePacketClass(PacketClass *raw);	// 0x87FA70
	virtual PacketClass *Build();	// +0x04 0x87FB40
	virtual bool Process_Packet(void *client);	// +0x08 0x733700 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87FC30
	virtual int Print(char *buffer, int len);	// +0x10 0x87FC50
	virtual int Get_Channel() { return(Unknown24); }	// +0x18 0x87FA00

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	float Scale;	// +0x10  // NAME 3: the client's Print text 'Scale='
	short BaseAsset;	// +0x14  // NAME 3: the client's Print text 'BaseAsset='
	char Type;	// +0x16  // NAME 3: the client's Print text 'Type='
	char Unknown17[0x1];	// +0x17
	Vector3 HSV;	// +0x18  // NAME 3: the client's Print text 'HSV='
	int Unknown24;	// +0x24
};

#endif
