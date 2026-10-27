/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): a galaxy (shard) of the login's list,
**	its wire functions in app\common's unattributed run after galaxy_map.h (0x83C730-0x83C870;
**	docs/kernel-map-step9.md 5). Moved here from tsk_login.h (D90 9b). Its users: LoginTaskClass's
**	galaxy list, the avatar list packet (0x862D90), ui_avsel.cpp and ui_shard.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_GALAXY_H  // NAME 1 [unknown_galaxy.h]: neutral file name (D10)
#define UNKNOWN_GALAXY_H

#include "..\shared\unknown_packetdata.h"

// A galaxy of the login's list (vtable 0xAEFBD0, 0x60 bytes): written and read raw (no byte swap)
class LoginGalaxyClass : public LoginPacketDataClass	// NAME 1: what it holds
{
public:
	~LoginGalaxyClass() {}								// 0x76BBF0 (stores the vtable: explicit)
	virtual bool Write(LinearBufferClass *buffer);		// +0x00 0x83C730
	virtual bool Read(LinearBufferClass *buffer);		// +0x04 0x83C7C0
	virtual int Get_Size();								// +0x08 0x83C850

	char Name[0x41];									// +0x04 NAME 1: 0x40 bytes on the wire
	int ID;												// +0x48 NAME 1: compared with the characters' Galaxy_ID
	unsigned long Address;								// +0x4C NAME 1: Galaxy_Address's source
	short Unknown50;									// +0x50
	int Unknown54;										// +0x54
	int Unknown58;										// +0x58
	short Unknown5C;									// +0x5C
};

#endif
