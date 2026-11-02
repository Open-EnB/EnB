// Earth & Beyond client (reconstruction): the message server's client filter (unknown_message.h). Notes at the end of the file.
#include "unknown_message.h"

// PLACEMENT: provisional (D137): see unknown_message.h; its own object because the server
// (unknown_message_server.cpp) calls its constructor 0x4BDFE0 and destructor 0x4BE060 instead of inlining them

// 0x4BDFE0
Unknown4BDFE0Class::Unknown4BDFE0Class() :
	Unknown08(0)
{
}

// 0x4BE060
Unknown4BDFE0Class::~Unknown4BDFE0Class()
{
}


/*
**	Reconstruction notes
**
**	client.exe 0x4BDFE0 / 0x4BE060. Not rebuilt yet: the rest of the object, 0x4BE0F0, 0x4BE1E0, the
**	match 0x4BE2C0 and the parser 0x4BE7A0 (to 0x4BE9D0).
*/
