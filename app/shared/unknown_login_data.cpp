/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): the character slot and the login
**	ticket (unknown_login_data.h), client 0x8C06E0-0x8C1B40, linked after WWOnline's wnet.
*/

#include "unknown_login_data.h"
#include "auxdata\linearbu.h"
#include <winsock.h>

unsigned __int64 Unknown_8B4510(unsigned __int64 value);	// NAME 1 [Unknown_8B4510]: wwonline's 64-bit htonl (a byte-order probe, then two htonl)
unsigned __int64 Unknown_8B4560(unsigned __int64 value);	// NAME 1 [Unknown_8B4560]: wwonline's 64-bit ntohl

// 0x8C06E0
LoginCharacterClass::LoginCharacterClass() :
	Slot(0), Sector(0), Galaxy_ID(0), Save_Count(0), ID(0), Account_ID(0), Admin_Level(0), GM_Flag(0),
	Level_Combat(0), Level_Explore(0), Level_Trade(0)
{
	Sector_Name[0] = 0;
}

// 0x8C0750
LoginCharacterClass::~LoginCharacterClass()
{
}

// 0x8C0770
bool LoginCharacterClass::Write(LinearBufferClass *buffer)
{
	{
		int net = htonl(Slot);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(Sector);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(Galaxy_ID);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(Save_Count);
		buffer->Write(&net, 4);
	}
	{
		__int64 net = Unknown_8B4510(ID);
		buffer->Write(&net, 8);
	}
	{
		__int64 net = Unknown_8B4510(Account_ID);
		buffer->Write(&net, 8);
	}
	{
		int net = htonl(Admin_Level);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(GM_Flag);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(Level_Combat);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(Level_Explore);
		buffer->Write(&net, 4);
	}
	{
		int net = htonl(Level_Trade);
		buffer->Write(&net, 4);
	}
	buffer->Write(Sector_Name, sizeof(Sector_Name));
	return(true);
}

// 0x8C0940
bool LoginCharacterClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&Slot, 4);
	Slot = ntohl(Slot);
	buffer->Read(&Sector, 4);
	Sector = ntohl(Sector);
	buffer->Read(&Galaxy_ID, 4);
	Galaxy_ID = ntohl(Galaxy_ID);
	buffer->Read(&Save_Count, 4);
	Save_Count = ntohl(Save_Count);
	buffer->Read(&ID, 8);
	ID = Unknown_8B4560(ID);
	buffer->Read(&Account_ID, 8);
	Account_ID = Unknown_8B4560(Account_ID);
	buffer->Read(&Admin_Level, 4);
	Admin_Level = ntohl(Admin_Level);
	buffer->Read(&GM_Flag, 4);
	GM_Flag = ntohl(GM_Flag);
	buffer->Read(&Level_Combat, 4);
	Level_Combat = ntohl(Level_Combat);
	buffer->Read(&Level_Explore, 4);
	Level_Explore = ntohl(Level_Explore);
	buffer->Read(&Level_Trade, 4);
	Level_Trade = ntohl(Level_Trade);
	buffer->Read(Sector_Name, sizeof(Sector_Name));
	return(true);
}

// 0x8C0AC0
int LoginCharacterClass::Get_Size()
{
	return(0x85);
}

// 0x8C0AE0
void LoginCharacterClass::Unknown_00(void *object)
{
	LoginKVVisitorClass *visitor = (LoginKVVisitorClass *)object;

	visitor->Add(0x3FF, "slot", &Slot);
	visitor->Add(0x3FF, "sector", &Sector);
	visitor->Add(0x3FF, "shard_id", &Galaxy_ID);
	visitor->Add(0x3FF, "save_count", &Save_Count);
	visitor->Add(0x3FF, "avatar_id", &ID);
	visitor->Add(0x3FF, "account_id", &Account_ID);
	visitor->Add(0x3FF, "admin_level", &Admin_Level);
	visitor->Add(0x3FF, "gm_flag", &GM_Flag);
	visitor->Add(0x3FF, "sector_name", Sector_Name, sizeof(Sector_Name));
	visitor->Add(0x3FF, "lvl_combat", &Level_Combat);
	visitor->Add(0x3FF, "lvl_explore", &Level_Explore);
	visitor->Add(0x3FF, "lvl_trade", &Level_Trade);
}

// 0x8C15F0
TicketClass::TicketClass(int unknown) :
	Unknown04(unknown), Unknown08(0), Unknown0C(0), Character_ID(0), Unknown18(0), Unknown1C(0),
	Unknown20(0), Unknown24(0), Unknown28(0), Unknown2C(0), Unknown44(false)
{
}

// 0x8C1690
TicketClass::TicketClass(const TicketClass &that)
{
	*this = that;
}

// 0x8C16D0
TicketClass::~TicketClass()
{
}

// 0x8C16F0
TicketClass &TicketClass::operator=(const TicketClass &that)
{
	if (this != &that) {
		Unknown04 = that.Unknown04;
		Unknown08 = that.Unknown08;
		Unknown0C = that.Unknown0C;
		Character_ID = that.Character_ID;
		Unknown18 = that.Unknown18;
		Unknown1C = that.Unknown1C;
		Unknown20 = that.Unknown20;
		Unknown24 = that.Unknown24;
		Unknown28 = that.Unknown28;
		Unknown2C = that.Unknown2C;
		Digest = that.Digest;
		Unknown44 = that.Unknown44;
	}
	return(*this);
}

// 0x8C1780
bool TicketClass::Write(LinearBufferClass *buffer)
{
	int net;
	__int64 net64;

	buffer->Get_Pos();
	net = htonl(Unknown04);
	buffer->Write(&net, 4);
	net = htonl(Unknown08);
	buffer->Write(&net, 4);
	net = htonl(Unknown0C);
	buffer->Write(&net, 4);
	net64 = Unknown_8B4510(Character_ID);
	buffer->Write(&net64, 8);
	net = htonl(Unknown18);
	buffer->Write(&net, 4);
	net = htonl(Unknown1C);
	buffer->Write(&net, 4);
	net = htonl(Unknown20);
	buffer->Write(&net, 4);
	net = htonl(Unknown24);
	buffer->Write(&net, 4);
	net = htonl(Unknown28);
	buffer->Write(&net, 4);
	net = htonl(Unknown2C);
	buffer->Write(&net, 4);
	if (TicketDigestClass() == Digest) {
		buffer->Get_Pos();
	}
	buffer->Write(&Digest, sizeof(Digest));
	return(true);
}

// 0x8C1960
bool TicketClass::Read(LinearBufferClass *buffer)
{
	int net;

	buffer->Get_Pos();
	buffer->Read(&net, 4);
	Unknown04 = ntohl(net);
	buffer->Read(&Unknown08, 4);
	Unknown08 = ntohl(Unknown08);
	buffer->Read(&net, 4);
	Unknown0C = ntohl(net);
	buffer->Read(&Character_ID, 8);
	Character_ID = Unknown_8B4560(Character_ID);
	buffer->Read(&Unknown18, 4);
	Unknown18 = ntohl(Unknown18);
	buffer->Read(&Unknown1C, 4);
	Unknown1C = ntohl(Unknown1C);
	buffer->Read(&Unknown20, 4);
	Unknown20 = ntohl(Unknown20);
	buffer->Read(&Unknown24, 4);
	Unknown24 = ntohl(Unknown24);
	buffer->Read(&Unknown28, 4);
	Unknown28 = ntohl(Unknown28);
	buffer->Read(&Unknown2C, 4);
	Unknown2C = ntohl(Unknown2C);
	buffer->Get_Pos();
	TicketDigestClass zero;
	buffer->Read(&Digest, sizeof(Digest));
	if (Digest == zero) {
		Unknown44 = true;
		return(true);
	}
	Unknown44 = false;
	return(true);
}

// 0x8C1B30
int TicketClass::Get_Size()
{
	return(0x40);
}

/*
**	Reconstruction notes
**
**	The module's other functions, 0x8C0C40-0x8C1570, are the key/value templates the slot's
**	visitor instantiates (the encoder's adds for __int64 0x8C0C40 and a 4-byte value 0x8C0D80,
**	their pairs and pointer accessors: vtables 0xB0C570-0xB0C5CC); not rebuilt (unknown_packetdata.h
**	declares the adds). Write's and Read's unused Get_Pos calls are the client's (a debug output's
**	argument left behind, presumably).
*/
