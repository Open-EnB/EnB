/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\gameedit\pkgeaset.h (the .cpp named by its asserts)
**
**	The packet class of client 0x85DAA0-0x85DE00 (opcode 30020).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGEASET_H  // NAME 3 [pkgeaset.h]: the header of the client's gameedit\pkgeaset.cpp
#define PKGEASET_H

#include "..\pk.h"

// 0x7544 (30020, not in the packet switch: sent only): vtable 0xB08E28, constructor 0x85DB90, 0x14 bytes
class Packet_7544Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7544 (Print 'GameEditorAssetMapping')
{
public:
	Packet_7544Class(const char *mappings);	// 0x85DAA0 (Data nstrdup'd)
	Packet_7544Class(RawPacketClass *raw);	// 0x85DB90
	virtual ~Packet_7544Class();	// +0x00 0x85DC70 / 0x85DB60
	virtual RawPacketClass *Build();	// +0x04 0x85DCB0
	virtual bool Process_Packet(void *client);	// +0x08 0x85DDE0
	virtual int Get_Data_Size();	// +0x0C 0x85DD90
	virtual int Print(char *buffer, int len);	// +0x10 0x85DDB0

	char *Data;	// +0x0C (the mappings' text)  // NAME 3: the client's assert 'Data'
	int Size;	// +0x10 (4 bytes as they are; the text's with its 0)  // NAME 2: what it holds
};

#endif
