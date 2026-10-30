// Earth & Beyond client (reconstruction): app\packets\pkspline.cpp. Notes at the end of the file.
#include "pkspline.h"
#include "..\shared\spln_db.h"
#include <stdio.h>

SplinePacketClass::SplinePacketClass(int count, SplineEntryClass **entries)
{
	Blocksize = SplineDatabaseClass::Get_Packed_Size(count, entries);
	Block = new char[Blocksize];
	SplineDatabaseClass::Pack(count, entries, Block);
}

SplinePacketClass::SplinePacketClass(RawPacketClass *raw)
{
	raw->Get(&Blocksize, sizeof(Blocksize));
	Block = new char[Blocksize];
	raw->Get(Block, Blocksize);
}

SplinePacketClass::~SplinePacketClass()
{
	delete [] Block;
}

RawPacketClass *SplinePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(56, Get_Data_Size());
	raw->Add(&Blocksize, sizeof(Blocksize));
	raw->Add(Block, Blocksize);
	return(raw);
}

int SplinePacketClass::Get_Data_Size()
{
	return(Blocksize + sizeof(Blocksize));
}

int SplinePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Blocksize=%d}", "Spline", Blocksize));
}


/*
**	Reconstruction notes (pkspline.cpp)
**
**	* Client 0x8A57A0-0x8A5A80 (0x8A5770 is the previous object's guarded static, 0xC416FC; this one's is 0x8A5A80):
**	  opcode 56, written by hand (builder P4, 2026-10-27). The value constructor comes first.
**	* The file name is provisional (no assert names it).
*/
