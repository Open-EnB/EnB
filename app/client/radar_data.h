// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_data.h (name assumed from radar_data.cpp)
**
**	A client object's radar data (vtable 0xB06720), bound to the object through co_data.cpp; declared
**	as far as the callers need it.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_DATA_H  // NAME 3 [radar_data.h]: header name assumed from the client's radar_data.cpp
#define __RADAR_DATA_H

#include "co_data.h"
#include "vector3.h"
#include "co_player_relationship.h"

class PlayerObjectRelationshipChangeEvent;
class ClientGroupEvent;
class SClient;

class RadarDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVRadarDataClass@@ (co_data.cpp's registry)
{
public:
	RadarDataClass(ClientObjectClass *co);			// 0x7BB1D0 (radar_data.cpp)
	virtual ~RadarDataClass();							// +0x04 0x7BB420 (0x7BB450)

	void Unknown_7BB570();								// 0x7BB570 (radar_system.cpp's 0x7BCA70): sets Color from the state and the clock
	void Unknown_7BB8C0(int state);						// 0x7BB8C0: 0 none, 1 hovered, 2 targeted (radar_system.cpp)  // NAME 2 [state]: what 0x7BD6E0 / 0x7BD860 pass
	void Unknown_7BB910();								// 0x7BB910: sets Unknown78 (FAIL_IF '!client', line 201)
	void Unknown_7BB960();								// 0x7BB960: reads Colors (FAIL_IF '!client', line 209)
	void Unknown_7BBA90(PlayerObjectRelationshipChangeEvent *event, ClientObjectClass *object);	// 0x7BBA90: the object's relationship changed  // NAME 2 [event, object]: the event's type, the FAIL_IF
	void Unknown_7BBC00(ClientGroupEvent *event, ClientObjectClass *object);	// 0x7BBC00: the object's group changed  // NAME 2 [event, object]: the event's type, the FAIL_IF

	SClient *Peek_SClient() { return((SClient *)Unknown_18()); }	// NAME 1: neutral

	int Unknown08;						// +0x08 (gutter_data.cpp's 0x7B63C0: 1 and 2 show the labels; 0x7c bytes: co_data.cpp's new)  // NAME 1: neutral (D10)
	Vector3 Color;						// +0x0C (target_reticle.cpp 0x7C5690 colours the corners with it)  // NAME 2: what it does
	GutterColorSetStruct Colors;		// +0x18  // NAME 2: what it holds
	bool Unknown78;						// +0x78 (0x7C4940 tests it; 0x70BAC0's result)
	char Unknown79[0x7C - 0x79];		// +0x79
};

#endif //__RADAR_DATA_H
