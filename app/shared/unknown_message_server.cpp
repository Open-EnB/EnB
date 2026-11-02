// Earth & Beyond client (reconstruction): E&B's message server (unknown_message.h). Notes at the end of the file.
#include "unknown_message.h"

// PLACEMENT: provisional (D137): see unknown_message.h; its own object because the reporters
// (unknown_message.cpp) call 0x4BCD10 instead of inlining it

// 0x4BCCF0
void Unknown4BCA80Class::Unknown_4BCCF0(bool value)
{
	Unknown38 = value;
}

// 0x4BCD10
int Unknown4BCA80Class::Unknown_4BCD10()
{
	return Unknown04.length();
}



/*
**	Reconstruction notes
**
**	client.exe 0x4BCCF0 / 0x4BCD10. Not rebuilt yet: the constructor 0x4BCA80 / destructor 0x4BCB50,
**	the listener 0x4BCCB0 (TCPMgr, ports 9999-10009), 0x4BCD30 (the send) and the INI reader ('Prefix',
**	'Modules', 'Files') to 0x4BE9D0.
*/
