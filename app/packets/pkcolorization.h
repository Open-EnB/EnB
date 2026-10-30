// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkcolorization.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87F2F0-0x87F8F0 (opcode 17).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCOLORIZATION_H  // NAME 1 [pkcolorization]: provisional, pk + its class's name
#define PKCOLORIZATION_H

#include "pk.h"
#include <vector>
#include "vector3.h"

// One region of a COLORIZATION packet: 32 bytes, read and written field by field (its vector's
// copy and insert are another object's instances, 0x62A560 / 0x62A0A0)
// Half a region: an int and a Vector3. The client copies a region as two of these (the inline copy in
// 0x87F320 addresses each region from its last dword, as a nested copy constructor does)
struct ColorizationHalfStruct  // NAME 1: neutral (D10), half of a region
{
	int Unknown00;	// +0x00
	Vector3 Unknown04;	// +0x04
};

struct ColorizationRegionStruct  // NAME 1: neutral (D10), the packet's 'NumRegions'
{
	ColorizationHalfStruct Unknown00;	// +0x00
	ColorizationHalfStruct Unknown10;	// +0x10
};

// opcode 17 (0x0011, COLORIZATION): vtable 0xB0A110, constructor 0x87F510, 0x24 bytes
class ColorizationPacketClass : public PacketClass
{
public:
	ColorizationPacketClass(int game_id, const std::vector<ColorizationRegionStruct> &regions);	// 0x87F320
	ColorizationPacketClass(RawPacketClass *raw);	// 0x87F510
	virtual RawPacketClass *Build();	// +0x04 0x87F730
	virtual bool Process_Packet(void *client);	// +0x08 0x735ED0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87F870
	virtual int Print(char *buffer, int len);	// +0x10 0x87F8A0
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x87F480

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	std::vector<ColorizationRegionStruct> Regions;	// +0x10 (a short count, then the regions)  // NAME 3: the client's Print text 'NumRegions='
	int Channel;	// +0x20 (PacketClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
};

#endif
