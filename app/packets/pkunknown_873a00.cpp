// Earth & Beyond client (reconstruction): app\packets\pkunknown_873a00.cpp. Notes at the end of the file.
#include "pkunknown_873a00.h"

bool MoneyPacketDataClass::Write(LinearBufferClass *buffer)
{
	__int64 value = Money->Unknown00;
	buffer->Write(&value, sizeof(value));
	return(true);
}

bool MoneyPacketDataClass::Read(LinearBufferClass *buffer)
{
	__int64 value = 0;
	buffer->Read(&value, sizeof(value));
	*Money = value;
	return(true);
}

int MoneyPacketDataClass::Get_Size()
{
	return(sizeof(__int64));
}


/*
**	Reconstruction notes (pkunknown_873a00.cpp)
**
**	* Client 0x873A00-0x873AE0 (0x8739D0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x873AB0): the three virtuals of the MoneyClass packet-data wrapper (vtable 0xB0ADC4), written by hand
**	  (builder P5, 2026-10-27). The object holds nothing else.
**	* The file name is provisional (no assert names it).
*/
