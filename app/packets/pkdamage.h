/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkdamage.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87FF60-0x880290 rebuilt so far (opcode 100); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKDAMAGE_H  // NAME 1 [pkdamage]: provisional, pk + its first named class's name
#define PKDAMAGE_H

#include "pk.h"

// opcode 100 (0x0064, CLIENT_DAMAGE): vtable 0xB0A1A0, constructor 0x880030, 0x24 bytes
class DamagePacketClass : public PacketClass
{
public:
	DamagePacketClass(float damage, float modifer, int type, int inflicted, int source, int victim);	// 0x87FF60
	DamagePacketClass(RawPacketClass *raw);	// 0x880030
	virtual RawPacketClass *Build();	// +0x04 0x880110
	virtual bool Process_Packet(void *client);	// +0x08 0x738970 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x880220
	virtual int Print(char *buffer, int len);	// +0x10 0x880240

	float Damage;	// +0x0C  // NAME 3: the client's Print text 'Damage='
	float Modifer;	// +0x10  // NAME 3: the client's Print text 'Modifer='
	int Type;	// +0x14  // NAME 3: the client's Print text 'Type='
	int Inflicted;	// +0x18  // NAME 3: the client's Print text 'Inflicted='
	int Source;	// +0x1C  // NAME 3: the client's Print text 'Source='
	int Victim;	// +0x20  // NAME 3: the client's Print text 'Victim='
};

#endif
