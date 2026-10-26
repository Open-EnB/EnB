/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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

// opcode 38 (0x0026, CHANGE_BASE_ASSET): vtable 0xB0A050, constructor 0x87E560, 0x28 bytes
class ChangeBaseAssetPacketClass : public PacketClass
{
public:
	ChangeBaseAssetPacketClass(RawPacketClass *raw);	// 0x87E560
	virtual RawPacketClass *Build();	// +0x04 0x87E620
	virtual bool Process_Packet(void *client);	// +0x08 0x737400 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x87E710
	virtual int Print(char *buffer, int len);	// +0x10 0x87E730
	virtual int Get_Channel() { return(Unknown24); }	// +0x18 0x87E4F0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int BaseAsset;	// +0x10  // NAME 3: the client's Print text 'BaseAsset='
	float Scale;	// +0x14  // NAME 3: the client's Print text 'Scale='
	float Unknown18[3];	// +0x18
	int Unknown24;	// +0x24
};

#endif
