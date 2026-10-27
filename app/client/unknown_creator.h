/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): the characters the Character and
**	Starship Creator shares with the client, kept as files in a folder under the common
**	application data ('Westwood Studios\Earth and Beyond\Character and Starship Creator'), and the
**	serialisable name stored with them. Client 0x78CEE0-0x78E190, the unattributed run between
**	avatar.cpp and nlp.cpp (docs/kernel-map-step9.md 5). Moved here from tsk_login.h (D90 9b).
**	D69: the constructor reads the registry and creates folders; never run against the real system.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_CREATOR_H  // NAME 1 [unknown_creator.h]: neutral file name (D10)
#define UNKNOWN_CREATOR_H

#include "..\shared\unknown_packetdata.h"
#include "wwstring.h"

class LoginCharacterClass;
class AvatarDescClass;
class ShipCustDescClass;

// The serialisable name (vtable 0xB04160: Write 0x78E0E0 writes its 0x80 bytes; 0x84 bytes)
class LoginNameDataClass : public LoginPacketDataClass	// NAME 1: neutral (D10)
{
public:
	LoginNameDataClass() { Name[0] = 0; }
	virtual bool Write(LinearBufferClass *buffer);		// +0x00 0x78E0E0
	virtual bool Read(LinearBufferClass *buffer);		// +0x04 0x78E110
	virtual int Get_Size();								// +0x08 0x78E140
	void Set_Name(const char *name);					// 0x78E0A0 (no caller)  // NAME 2: what it does

	char Name[0x80];									// +0x04 NAME 1: 0x80 bytes on the wire
};

// What 0x7683F0 and 0x768CB0 make on the stack (its destructor is StringClass's, 0x7689C0)
class CharacterCreatorClass	// NAME 2: its folder and registry key, 'Character and Starship Creator'
{
public:
	CharacterCreatorClass();							// 0x78CEE0: the folder, made if needed
	void Unknown_78D480(int slot, LoginCharacterClass *character, AvatarDescClass *avatar, ShipCustDescClass *ship, LoginNameDataClass *name);	// 0x78D480: reads 'Avatar<slot>.dat'  // NAME 1 [avatar, ship]: their classes
	void Unknown_78DAF0(int slot);						// 0x78DAF0 (Unknown_50 passes the selected slot)

	StringClass Folder;									// +0x00 NAME 2: the folder, with its '\'
};

int __stdcall Unknown_78D450(const unsigned char *data, unsigned int size);	// 0x78D450 (no caller): a byte sum  // NAME 1 [Unknown_78D450]: neutral (D10)
int Make_Folder(const char *path);						// 0x78E060 (inlined in the constructor; no caller)  // NAME 2: what it does

#endif
