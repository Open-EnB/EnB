// Earth & Beyond client (reconstruction): E&B's message server (unknown_message.h). Notes at the end of the file.
#include "unknown_message.h"
#include <string.h>
#include <time.h>
#include "..\..\libs\wwonline\wnet\tcpmgr.h"
#include "..\..\libs\wwonline\wnet\tcpcon.h"

// PLACEMENT: provisional (D137): see unknown_message.h; its own object because the reporters
// (unknown_message.cpp) call 0x4BCD10 instead of inlining it

// 0x4BCA80
Unknown4BCA80Class::Unknown4BCA80Class() :
	Unknown00(NULL),
	Unknown1C(0),
	Unknown38(false)
{
	Unknown00 = new TCPMgr;
}

// 0x4BCB50
Unknown4BCA80Class::~Unknown4BCA80Class()
{
	delete Unknown00;
}

// 0x4BCCB0
void Unknown4BCA80Class::Unknown_4BCCB0()
{
	for (uint16 port = 9999; port <= 10009; port++) {
		if (Unknown00->addListener(0, port, FALSE) == TRUE) {
			return;
		}
	}
}

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



// 0x4BD3F0
bit8 Unknown4BCA80Class::Unknown_4BD3F0(Unknown4BCA80EntryStruct *client, Unknown4BB8A0Class *message)
{
	if (client != NULL && message != NULL) {
		int count = client->Filters.length();
		for (int i = 0; i < count; i++) {
			Unknown4BDFE0Class *filter;
			client->Filters.getPointer(&filter, i);
			if ((client->Mask & message->Level) != 0 && filter->Unknown_4BE2C0(message) == TRUE) {
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

// 0x4BD470
void Unknown4BCA80Class::Unknown_4BD470(int level, Wstring &sender, Wstring &file, int line, int x, int y, Wstring &text)
{
	char *name = file.get();
	char *next;
	while ((next = strchr(name, '\\')) != NULL) {
		name = next + 1;
	}
	while ((next = strchr(name, '/')) != NULL) {
		name = next + 1;
	}
	Unknown4BB8A0Class message;
	message.Level = level;
	message.Sender = sender;
	message.File.set(name);
	message.Line = line;
	message.Text = text;
	message.Time = time(NULL);
	message.X = x;
	message.Y = y;
	if (Unknown38) {
		Unknown20.lock();
	}
	Messages.addTail(message);
	if (Unknown38) {
		Unknown20.unlock();
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4BCA80-0x4BD470 without 0x4BCD30: the server's constructor, destructor, listener and two
**	unreferenced methods (0x4BD3F0 a client's filters against a message, 0x4BD470 another way in for a
**	message, under the lock), with the ArrayList destructors they instantiate (0x4BD3A0, 0x4BD8C0,
**	0x4BD910). Not rebuilt yet: 0x4BCD30 (accept clients, read their filters, send them the messages:
**	the error reporters call it) and the ArrayList instances only it uses (0x4BD870, 0x4BD960, 0x4BD990,
**	0x4BDBE0, 0x4BDEF0).
*/
