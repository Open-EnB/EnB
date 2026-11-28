// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkchangebaseasset.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87E420-0x87E7A0 rebuilt so far (opcode 38); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCHANGEBASEASSET_H  // NAME 1 [pkchangebaseasset]: provisional, pk + its first named class's name
#define PKCHANGEBASEASSET_H

#include "pk.h"
#include "vector3.h"

// opcode 38 (0x0026, CHANGE_BASE_ASSET): vtable 0xB0A050, constructor 0x87E560, 0x28 bytes
class ChangeBaseAssetPacketClass : public UnknownPacketBaseClass
{
public:
	ChangeBaseAssetPacketClass(int game_id, int base_asset, float scale, const Vector3 &hsv_shift);	// 0x87E420  // NAME 2: the stores (no caller left)
	ChangeBaseAssetPacketClass(PacketClass *raw);	// 0x87E560
	virtual PacketClass *Build();	// +0x04 0x87E620
	virtual bool Process_Packet(void *client);	// +0x08 0x737400 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87E710
	virtual int Print(char *buffer, int len);	// +0x10 0x87E730
	virtual int Get_Channel() { return(Unknown24); }	// +0x18 0x87E4F0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int BaseAsset;	// +0x10  // NAME 3: the client's Print text 'BaseAsset='
	float Scale;	// +0x14  // NAME 3: the client's Print text 'Scale='
	Vector3 HSVShift;	// +0x18  // NAME 3: the client's Print text 'HSVShift='
	int Unknown24;	// +0x24
};

#endif
