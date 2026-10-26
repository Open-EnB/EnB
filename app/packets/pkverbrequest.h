/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkverbrequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AEAE0-0x8B15C0 rebuilt so far (opcode 90); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKVERBREQUEST_H  // NAME 1 [pkverbrequest]: provisional, pk + its first named class's name
#define PKVERBREQUEST_H

#include "pk.h"

// opcode 90 (0x005A, VERB_REQUEST): vtable 0xB0C234, constructor 0x8AEB60, 0x18 bytes
class VerbRequestPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name VERB_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	VerbRequestPacketClass(int subject_id, int object_id, int action);	// 0x8AEAE0
	VerbRequestPacketClass(RawPacketClass *raw);	// 0x8AEB60
	virtual RawPacketClass *Build();	// +0x04 0x8AEC00
	virtual bool Process_Packet(void *client);	// +0x08 0x868010 (clientstub.cpp l.101)
	virtual int Get_Data_Size();	// +0x0C 0x8AED00
	virtual int Print(char *buffer, int len);	// +0x10 0x8AED20
	virtual int Get_Type();	// +0x28 0x8AECE0  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int SubjectID;	// +0x0C  // NAME 3: the client's Print text 'SubjectID='
	int ObjectID;	// +0x10  // NAME 3: the client's Print text 'ObjectID='
	int Action;	// +0x14  // NAME 3: the client's Print text 'Action='
};

#endif
