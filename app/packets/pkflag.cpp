// Earth & Beyond client (reconstruction): e:\build\beta\G\app\Packets\pkflag.cpp. Notes at the end of the file.
#include "pkflag.h"


void Add_Flagged(RawPacketClass *raw, unsigned int flags, int flag, const void *data, int len)
{
	if (flags & (1 << flag)) {
		raw->Add(data, len);
	}
}


void Get_Flagged(RawPacketClass *raw, unsigned int flags, int flag, void *data, int len)
{
	if (flags & (1 << flag)) {
		raw->Get(data, len);
	}
}


int Flagged_Size(unsigned int flags, int flag, int len)
{
	return((flags & (1 << flag)) ? len : 0);
}


void Set_Flag(unsigned int *flags, int flag, bool set)
{
	if (set) {
		*flags |= 1 << flag;
	}
}


bool Is_Flag_Set(unsigned int flags, int flag)
{
	return((flags & (1 << flag)) != 0);
}









void FlagPacketClass::Write_Flags(RawPacketClass *raw, unsigned int flags)
{
	unsigned int flag_max = Get_Flag_Count();
	G_ASSERT(flag_max <= 32);
	G_ASSERT(flags < (unsigned int)(1 << flag_max));
	if (flag_max <= 8) {
		unsigned char value8;
		G_ASSERT(flags <= 255);
		value8 = (unsigned char)flags;
		raw->Add(&value8, sizeof(value8));
	} else if (flag_max <= 16) {
		G_ASSERT(flags < 0xffff);
		unsigned short value16 = (unsigned short)flags;
		raw->Add(&value16, sizeof(value16));
	} else {
		raw->Add(&flags, sizeof(flags));
	}
}

void FlagPacketClass::Read_Flags(RawPacketClass *raw, unsigned int *flags)
{
	unsigned int flag_max = Get_Flag_Count();

	G_ASSERT(flag_max <= 32);
	if (flag_max <= 8) {
		unsigned char value8 = 0;
		raw->Get(&value8, sizeof(value8));
		*flags = value8;
	} else if (flag_max <= 16) {
		unsigned short value16 = 0;
		raw->Get(&value16, sizeof(value16));
		*flags = value16;
	} else {
		raw->Get(flags, sizeof(*flags));
	}
}


int FlagPacketClass::Get_Flag_Size()
{
	unsigned int flag_max = Get_Flag_Count();

	G_ASSERT(flag_max <= 32);
	if (flag_max <= 8) {
		return(1);
	}
	return((flag_max <= 16) ? 2 : 4);
}

/*
**	Reconstruction notes (pkflag.cpp)
**
**	* The client's file, 0x887570-0x8878D0 (the object ends with the guarded static every <string>
**	  includer has, 0xC416FC). Written by hand (builder P, 2026-10-26): five helpers that read, write
**	  and test a field by its bit in a flag mask, and FlagPacketClass's mask in 1, 2 or 4 bytes by
**	  its Get_Flag_Count (+0x28). The asserts sit on the client's lines 51, 52, 55, 59, 71 and 90
**	  (D25); the code above them is laid out to fit.
*/
