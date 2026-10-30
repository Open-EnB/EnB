// Earth & Beyond client (reconstruction)
/*
**	PROVISIONAL file name (no assert or __FILE__ names it): the login galaxy's wire functions
**	(unknown_galaxy.h), client 0x83C730-0x83C870 in app\common's run after galaxy_map.h.
*/



#include "unknown_galaxy.h"
#include "..\shared\auxdata\linearbu.h"

// 0x83C730
bool LoginGalaxyClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(Name, 0x40);
	buffer->Write(&ID, 4);
	buffer->Write(&Address, 4);
	buffer->Write(&Unknown50, 2);
	buffer->Write(&Unknown54, 4);
	buffer->Write(&Unknown58, 4);
	buffer->Write(&Unknown5C, 2);
	return(true);
}

// 0x83C7C0
bool LoginGalaxyClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(Name, 0x40);
	buffer->Read(&ID, 4);
	buffer->Read(&Address, 4);
	buffer->Read(&Unknown50, 2);
	buffer->Read(&Unknown54, 4);
	buffer->Read(&Unknown58, 4);
	buffer->Read(&Unknown5C, 2);
	return(true);
}

// 0x83C850
int LoginGalaxyClass::Get_Size()
{
	return(0x54);
}

/*
**	Reconstruction notes
**
**	The three functions sit among other app\common code (the neighbours 0x83C640-0x83C6D0 copy
**	reference-counted handles); the class's file is unknown. The destructor is inline (out of line
**	in tsk_login.cpp, 0x76BBF0).
*/
