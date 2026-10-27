/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkserverparameters.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A5AB0-0x8A60F0 (opcode 66).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSERVERPARAMETERS_H  // NAME 1 [pkserverparameters]: provisional, pk + its class's name
#define PKSERVERPARAMETERS_H

#include "pk.h"
#include "vector2.h"

// opcode 66 (0x0042, SERVER_PARAMETERS): vtable 0xB0BB6C, constructor 0x8A5C10, 0x58 bytes
class ServerParametersPacketClass : public PacketClass
{
public:
	ServerParametersPacketClass(float zband_min, float zband_max, const Vector2 &min, const Vector2 &max, float fog_near, float fog_far, int debris_mode, bool light_backdrop, bool fog_backdrop, bool swap_backdrop, float backdrop_fog_near, float backdrop_fog_far, float max_tilt, bool auto_level, float impulse_rate, float decay_velocity, float decay_spin, short backdrop_base_asset, unsigned int sector_num);	// 0x8A5AB0
	ServerParametersPacketClass(RawPacketClass *raw);	// 0x8A5C10
	virtual RawPacketClass *Build();	// +0x04 0x8A5DD0
	virtual bool Process_Packet(void *client);	// +0x08 0x737F00 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A5FB0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A5FD0

	float ZBandMin;	// +0x0C  // NAME 3: the client's Print text 'ZBandMin='
	float ZBandMax;	// +0x10  // NAME 3: the client's Print text 'ZBandMax='
	Vector2 Min;	// +0x14 (XMin, YMin)  // NAME 3: the client's Print text 'XMin= / YMin ='
	Vector2 Max;	// +0x1C (XMax, YMax)  // NAME 3: the client's Print text 'XMax = / YMax ='
	float FogNear;	// +0x24  // NAME 3: the client's Print text 'FogNear='
	float FogFar;	// +0x28  // NAME 3: the client's Print text 'FogFar='
	int DebrisMode;	// +0x2C  // NAME 3: the client's Print text 'DebrisMode='
	bool LightBackdrop;	// +0x30  // NAME 3: the client's Print text 'LightBackdrop='
	bool FogBackdrop;	// +0x31  // NAME 3: the client's Print text 'FogBackdrop='
	bool SwapBackdrop;	// +0x32  // NAME 3: the client's Print text 'SwapBackdrop='
	float BackdropFogNear;	// +0x34  // NAME 3: the client's Print text 'BackdropFogNear='
	float BackdropFogFar;	// +0x38  // NAME 3: the client's Print text 'BackdropFogFar='
	float MaxTilt;	// +0x3C  // NAME 3: the client's Print text 'MaxTilt='
	bool AutoLevel;	// +0x40  // NAME 3: the client's Print text 'AutoLevel='
	float ImpulseRate;	// +0x44  // NAME 3: the client's Print text 'ImpulseRate='
	float DecayVelocity;	// +0x48  // NAME 3: the client's Print text 'DecayVelocity='
	float DecaySpin;	// +0x4C  // NAME 3: the client's Print text 'DecaySpin='
	short BackdropBaseAsset;	// +0x50  // NAME 3: the client's Print text 'BackdropBaseAsset='
	unsigned int SectorNum;	// +0x54  // NAME 3: the client's Print text 'SectorNum='
};

#endif
