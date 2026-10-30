/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalavatarlist.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8629A0-0x8633D0 (opcode 112).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALAVATARLIST_H  // NAME 1 [pkglobalavatarlist]: provisional, pk + its first named class's name
#define PKGLOBALAVATARLIST_H

#include "pk.h"
#include "..\shared\unknown_login_data.h"
#include "..\shared\avatar_desc.h"
#include "..\common\unknown_galaxy.h"
#include <list>

// opcode 112 (0x0070, GLOBAL_AVATAR_LIST): vtable 0xB09368, constructor 0x862D90, 0x20 bytes
class GlobalAvatarListPacketClass : public PacketClass  // NAME 2: Net-7's opcode name GLOBAL_AVATAR_LIST (the Net-7 opcode table), the client's Print text 'Avatar List'
{
public:
	GlobalAvatarListPacketClass(LoginCharacterClass *characters, AvatarDescClass *avatars, const std::list<LoginGalaxyClass> *galaxies);	// 0x8629A0  // NAME 1 [characters, avatars, galaxies]: what they hold
	GlobalAvatarListPacketClass(RawPacketClass *raw);	// 0x862D90
	virtual ~GlobalAvatarListPacketClass();	// +0x00 0x863050 / 0x862C60
	virtual RawPacketClass *Build();	// +0x04 0x863170
	virtual bool Process_Packet(void *client);	// +0x08 0x76B520 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8632A0
	virtual int Print(char *buffer, int len);	// +0x10 0x863330

	LoginCharacterClass *Characters;	// +0x0C (five, new[]'d when not given)  // NAME 1: what it points to
	AvatarDescClass *Avatars;	// +0x10 (five, new[]'d when not given)  // NAME 1: what it points to
	std::list<LoginGalaxyClass> Galaxies;	// +0x14  // NAME 1: copied to the login task's
};

#endif
