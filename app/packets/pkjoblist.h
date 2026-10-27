/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkjoblist.h (the .cpp named by an assert)
**
**	The packet classes of client 0x895F00-0x897490 rebuilt so far (opcode 148, 149, 150); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKJOBLIST_H
#define PKJOBLIST_H

#include "pk.h"

// opcode 148 (0x0094, JOB_DESCRIPTION): vtable 0xB0AF24, constructor 0x8969C0, 0x1C bytes
class JobDescriptionPacketClass : public PacketClass  // NAME 3: the client's Print text 'JobDescription' and FAIL_IF '!Title || !Description'
{
public:
	JobDescriptionPacketClass(unsigned int job_id, bool still_avail, const char *title, const char *description);	// 0x8968D0
	JobDescriptionPacketClass(RawPacketClass *raw);	// 0x8969C0
	virtual ~JobDescriptionPacketClass();	// +0x00 0x896990 / 0x896A90
	virtual RawPacketClass *Build();	// +0x04 0x896AE0
	virtual bool Process_Packet(void *client);	// +0x08 0x73A330 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x896BC0
	virtual int Print(char *buffer, int len);	// +0x10 0x896C30

	unsigned int JobID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'JobID:'
	bool StillAvail;	// +0x10 (1 byte as it is)  // NAME 3: the client's Print text 'StillAvail:'
	char *Title;	// +0x14 (nstrdup'd; sent as a string)  // NAME 3: the client's FAIL_IF and Print text 'Title:'
	char *Description;	// +0x18 (nstrdup'd; sent as a string)  // NAME 3: the client's FAIL_IF and Print text 'Description:'
};

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
