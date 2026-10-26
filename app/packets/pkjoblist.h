/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkjoblist.h (the .cpp named by an assert)
**
**	The packet classes of client 0x895F00-0x897490 rebuilt so far (opcode 149, 150); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKJOBLIST_H
#define PKJOBLIST_H

#include "pk.h"

// opcode 149 (0x0095, JOB_DELETE): vtable 0xB0AF54, constructor 0x896D10, 0x10 bytes
class JobDeletePacketClass : public PacketClass
{
public:
	JobDeletePacketClass(int job_id);	// 0x896C80
	JobDeletePacketClass(RawPacketClass *raw);	// 0x896D10
	virtual RawPacketClass *Build();	// +0x04 0x896DA0
	virtual bool Process_Packet(void *client);	// +0x08 0x73A420 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x896E60
	virtual int Print(char *buffer, int len);	// +0x10 0x896E80

	int JobID;	// +0x0C  // NAME 3: the client's Print text 'JobID='
};

// opcode 150 (0x0096, JOB_ACCEPT_REPLY): vtable 0xB0AF84, constructor 0x896F60, 0x18 bytes
class JobAcceptReplyPacketClass : public PacketClass
{
public:
	JobAcceptReplyPacketClass(int job_id, int accept_status, int seconds_remaining);	// 0x896EC0
	JobAcceptReplyPacketClass(RawPacketClass *raw);	// 0x896F60
	virtual RawPacketClass *Build();	// +0x04 0x897020
	virtual bool Process_Packet(void *client);	// +0x08 0x73A510 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x897100
	virtual int Print(char *buffer, int len);	// +0x10 0x897120

	int JobID;	// +0x0C  // NAME 3: the client's Print text 'JobID='
	int AcceptStatus;	// +0x10  // NAME 3: the client's Print text 'AcceptStatus='
	int SecondsRemaining;	// +0x14  // NAME 3: the client's Print text 'SecondsRemaining='
};

#endif
