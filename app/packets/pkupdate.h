/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkupdate.h (the .cpp named by an assert)
**
**	The packet classes of client 0x8ABA80-0x8AE7E0 rebuilt so far (opcode 64, 63); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUPDATE_H
#define PKUPDATE_H

#include "pk.h"

// opcode 64 (0x0040, CONSTANT_POSITIONAL_UPDATE): vtable 0xB0C0B4, constructor 0x8ABBF0, 0x34 bytes
class ConstantPositionalUpdatePacketClass : public PacketClass
{
public:
	ConstantPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8ABBF0
	virtual RawPacketClass *Build();	// +0x04 0x8ABCB0
	virtual bool Process_Packet(void *client);	// +0x08 0x733C80 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8ABDB0
	virtual int Print(char *buffer, int len);	// +0x10 0x8ABDD0
	virtual int Get_Channel() { return(Unknown2C); }	// +0x18 0x8ABB60
	virtual int Get_Priority() { return(Unknown30); }	// +0x1C 0x8ABB80
	virtual int Get_Type();	// +0x28 0x8ABD90  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	float Unknown10[3];	// +0x10
	char Unknown1C[16];	// +0x1C
	int Unknown2C;	// +0x2C
	int Unknown30;	// +0x30
};

// opcode 63 (0x003F, PLANET_POSITIONAL_UPDATE): vtable 0xB0C16C, constructor 0x8ADDF0, 0x3C bytes
class PlanetPositionalUpdatePacketClass : public PacketClass
{
public:
	PlanetPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8ADDF0
	virtual RawPacketClass *Build();	// +0x04 0x8ADF10
	virtual bool Process_Packet(void *client);	// +0x08 0x734A40 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8AE090
	virtual int Print(char *buffer, int len);	// +0x10 0x8AE0B0
	virtual int Get_Channel() { return(-1); }	// +0x18 0x8ADD80
	virtual int Get_Type();	// +0x28 0x8AE070  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int TimeStamp;	// +0x10  // NAME 3: the client's Print text 'TimeStamp='
	char Unknown14[12];	// +0x14
	int OrbitID;	// +0x20  // NAME 3: the client's Print text 'OrbitID='
	float OrbitDist;	// +0x24  // NAME 3: the client's Print text 'OrbitDist='
	float OrbitAngle;	// +0x28  // NAME 3: the client's Print text 'OrbitAngle='
	float OrbitRate;	// +0x2C  // NAME 3: the client's Print text 'OrbitRate='
	float RotateAngle;	// +0x30  // NAME 3: the client's Print text 'RotateAngle='
	float RotateRate;	// +0x34  // NAME 3: the client's Print text 'RotateRate='
	float TiltAngle;	// +0x38  // NAME 3: the client's Print text 'TiltAngle='
};

#endif
