/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): the login's ticket and character
**	slot, the module the client links after WWOnline's wnet (0x8C06E0-0x8C1B40, between udp.cpp's
**	UDP::Wait and WWMath::Init; docs/kernel-map-step9.md 5.1). Both are wire objects (Write /
**	Read / Get_Size) and the slot is also a key/value object (the visitor names its members).
**	TicketClass moved here from tsk_game.h, LoginCharacterClass from tsk_login.h (D90 9b).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_LOGIN_DATA_H  // NAME 1 [unknown_login_data.h]: neutral file name (D10)
#define UNKNOWN_LOGIN_DATA_H

#include "unknown_packetdata.h"
#include <string.h>

// The ticket's last 20 bytes (TicketClass +0x30): zeroed by default, copied and compared whole
class TicketDigestClass	// NAME 1: what it looks like (20 bytes)
{
public:
	TicketDigestClass() { memset(Data, 0, sizeof(Data)); }
	TicketDigestClass &operator=(const TicketDigestClass &that) { if (this != &that) memcpy(Data, that.Data, sizeof(Data)); return(*this); }
	bool operator==(const TicketDigestClass &that) const { return(memcmp(Data, that.Data, sizeof(Data)) == 0); }

	int Data[5];
};

// The login ticket (vtable 0xB0C608, 0x48 bytes; constructor 0x8C15F0, destructor 0x8C16D0, assignment
// 0x8C16F0): the master server's answer to the login, copied into SClient (+0x12D8) and the task.
#pragma pack(push, 4)		// the client's layout: the vtable pointer isn't padded for Character_ID
class TicketClass : public LoginPacketDataClass	// NAME 1: Init's assert 'kernel && ticket'
{
public:
	TicketClass(int unknown);							// 0x8C15F0 (ClientTask passes 0)
	TicketClass(const TicketClass &that);				// 0x8C1690
	virtual bool Write(LinearBufferClass *buffer);		// +0x00 0x8C1780
	virtual bool Read(LinearBufferClass *buffer);		// +0x04 0x8C1960
	virtual int Get_Size();								// +0x08 0x8C1B30
	virtual ~TicketClass();								// +0x0C 0x8C16D0 (deleting 0x8C1660)
	TicketClass &operator=(const TicketClass &that);	// 0x8C16F0

	int Unknown04;										// +0x04 (the constructor's argument)
	int Unknown08;										// +0x08
	int Unknown0C;										// +0x0C
	__int64 Character_ID;								// +0x10 NAME 2: stored as 'characterid' (Accept_Focus)
	int Unknown18;										// +0x18
	int Unknown1C;										// +0x1C
	int Unknown20;										// +0x20
	int Unknown24;										// +0x24
	int Unknown28;										// +0x28
	int Unknown2C;										// +0x2C
	TicketDigestClass Digest;							// +0x30 NAME 1: what it looks like
	bool Unknown44;										// +0x44: Read sets it when Digest is all zero
};
#pragma pack(pop)

// A character slot of the login's list (0x90 bytes, vtables 0xB0C560 / 0xB0C55C; LoginTaskClass's
// destructor destroys five with 0x8C0750). Its assignment is the implicit one (0x7683F0, 0x768CB0).
// The members' names are the visitor's keys (0x8C0AE0).
class LoginCharacterClass : public LoginPacketDataClass, public LoginInterfaceAEADC8Class	// NAME 1: what it holds
{
public:
	LoginCharacterClass();								// 0x8C06E0
	~LoginCharacterClass();								// 0x8C0750 (stores the vtables: explicit)
	virtual bool Write(LinearBufferClass *buffer);		// +0x00 0x8C0770 (vtable 0xB0C560)
	virtual bool Read(LinearBufferClass *buffer);		// +0x04 0x8C0940
	virtual int Get_Size();								// +0x08 0x8C0AC0
	virtual void Unknown_00(void *visitor);			// second table +0x00 0x8C0AE0 (0xB0C55C)

	int Slot;											// +0x08 NAME 2: its key 'slot'
	int Sector;											// +0x0C NAME 2: its key 'sector'
	int Galaxy_ID;										// +0x10 NAME 1: key 'shard_id'; compared with the galaxies' (0x769400)
	int Save_Count;										// +0x14 NAME 2: its key 'save_count'
	__int64 ID;											// +0x18 NAME 1: key 'avatar_id'; 0 for a new character (Go_Next_Step)
	__int64 Account_ID;									// +0x20 NAME 2: its key 'account_id'
	long Admin_Level;							// +0x28 NAME 2: its key 'admin_level'
	long GM_Flag;								// +0x2C NAME 2: its key 'gm_flag'
	int Level_Combat;									// +0x30 NAME 2: its key 'lvl_combat'
	int Level_Explore;									// +0x34 NAME 2: its key 'lvl_explore'
	int Level_Trade;									// +0x38 NAME 2: its key 'lvl_trade'
	char Sector_Name[0x51];								// +0x3C NAME 2: its key 'sector_name' (the constructor clears its first byte)
};

#endif
