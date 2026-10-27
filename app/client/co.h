/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\co.h (name assumed from co.cpp)
**
**	co.cpp's client object, ClientObjectClass (the RTTI's .?AVClientObjectClass@@): co.cpp is not
**	rebuilt, so the class is declared only as far as client.cpp, cpacket.cpp and co_data.cpp use it
**	(moved here from client.h). What the client shows of it: the constructor 0x6F2820 (vtable
**	0xB0070C; its first argument, the client, at +0x7C: CREATE 0x733700 passes its SClient), the
**	destructor 0x6F2FE0 (it deletes the object's data through co_data.cpp's Remove_All 0x79D390 and
**	resets a MultiFixedPoolDecalSystemClass vtable at +0x4C), ClientClass's object list node at
**	+0x60 and the game id at +0x90. aux_roots.h's UnknownCOClass (the factory 0x6F6000) is the
**	same class. Names: D10, D24, D56.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CO_H  // NAME 3 [co.h]: header name assumed from the client's co.cpp
#define __CO_H

#include "hashlist.h"
#include <string>
#include "..\shared\unknown_debug.h"

class RenderObjClass;
class SubPartsPacket;
class ClientClass;

class ClientObjectClass		// NAME 3: the RTTI's .?AVClientObjectClass@@ for co.cpp's class, not tied to this layout by a vtable
{
public:
	virtual ~ClientObjectClass();						// +0x00 (ClientClass's destructor deletes through it)
	virtual void Unknown_04();
	virtual void Unknown_08();							// +0x08: ClientClass's destructor calls it before the delete
	virtual void Unknown_0C(ClientObjectClass *child, int a, int b);	// +0x0C (cpacket.cpp: CREATE_ATTACHMENT 74 passes the child, its Unknown18 and 0)  // NAME 1 [child, a, b]: what the handler passes

	unsigned Get_Game_ID() { return(GameID); }			// (the assert's name)
	void Get_Name(std::string &name);					// 0x6FF4B0  // NAME 2: Find_Object compares it with a name
	bool Unknown_6FFE90();								// 0x6FFE90 (co.cpp)
	int Unknown_6FFF00();								// 0x6FFF00 (co.cpp)
	int Unknown_6FFF50();								// 0x6FFF50 (co.cpp)
	bool Unknown_700400();								// 0x700400 (co.cpp)
	RenderObjClass *Unknown_6F3BC0();					// 0x6F3BC0 (co.cpp; cpacket.cpp's DECAL 16 releases the model it returns)
	bool Unknown_6F5DC0();								// 0x6F5DC0 (co.cpp; cpacket.cpp's SET_TARGET 25 tests it)
	void Unknown_6FEF70(SubPartsPacket *packet);		// 0x6FEF70 (co.cpp; cpacket.cpp's SUBPARTS 180)  // NAME 1 [packet]: the handler's packet
	void Unknown_70E750(int id);						// 0x70E750 (cpacket.cpp's SET_TARGET 25 passes its TargetID)  // NAME 1 [id]: the packet's TargetID
	int Unknown_7283B0()								// co.cpp's, inline in client.cpp's 0x7283B0 (there its FAIL_IF has co.cpp's __FILE__, l.2461)
	{
		if (Unknown94 >= 0 && Unknown94 < 2) {
			FAIL_IF(!AuxData) {
				return(0);
			}
			return(*(int *)(AuxData + 0x39C4));
		}
		return(0);
	}

	char Unknown04[0x60 - 0x04];						// +0x04
	HashNodeClass<ClientObjectClass *> Node;			// +0x60 ClientClass::ObjectList's node  // NAME 2: what it is
	ClientClass *Client;								// +0x7C the constructor's first argument (CREATE passes its SClient; co_data.cpp calls its slot +0x18)  // NAME 2: what it holds
	char Unknown80[0x88 - 0x80];						// +0x80
	char *AuxData;										// +0x88 (co.cpp's FAIL_IF '!aux_data', l.2461)  // NAME 3: the FAIL_IF's name
	int Unknown8C;										// +0x8C
	unsigned GameID;									// +0x90  // NAME 3: Get_Game_ID's
	char Unknown94;										// +0x94
	char Unknown95[0xAC - 0x95];						// +0x95
	RenderObjClass *Model;								// +0xAC (cpacket.cpp's render-state handlers: Class_ID, then rs_track.cpp's methods)  // NAME 2: what it holds
};

#endif //__CO_H
