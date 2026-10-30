// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\influence.h (name assumed from influence.cpp)
**
**	influence.cpp's object (vtable 0xB07730, 0xB8 bytes; its base's vtable 0xAF0DD0, constructor 0x7E9940):
**	the record CREATE 4 makes for a type 10 object and puts in ClientClass's third list (+0x86C, keyed by
**	+0x24). Declared as far as CREATE needs it (its constructor, slot +0x10 and size); not rebuilt.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INFLUENCE_H  // NAME 3 [influence.h]: header name assumed from the client's influence.cpp
#define __INFLUENCE_H

#include "client.h"

class RenderObjClass;

class InfluenceClass : public Unknown86CRecordClass	// NAME 3: class-from-file (D24) for app\client\influence.cpp
{
public:
	InfluenceClass(ClientClass *client, unsigned id, RenderObjClass *model);	// 0x7E87C0 (CREATE 4: its client, the client's +0x113C decremented, the new model)  // NAME 1 [client, id, model]: what CREATE passes

	virtual void Unknown_04();						// +0x04
	virtual bool Unknown_08(const Vector3 &position);	// +0x08 (the base's: SClient::Process_AI passes the camera's position)  // NAME 1 [position]: what it gets
	virtual void Unknown_0C();						// +0x0C
	virtual void Unknown_10(int game_id);			// +0x10 (CREATE passes the packet's GameID)  // NAME 1 [game_id]: what CREATE passes

	char Unknown28[0xB8 - 0x28];		// +0x28: not rebuilt (0xB8 bytes: CREATE's new)
};

#endif //__INFLUENCE_H
