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



// 0x4BCD30
// NOT EXACT: shape (explain.py: registers, 2 rows once masked): the two inlined ArrayList destructor calls
// ('Filters.clear()' and 'remove') form their element's address as offset + vector where ours loads the
// vector first; everything else is the client's (0.85)
void Unknown4BCA80Class::Unknown_4BCD30()
{
	TCPCon *con = NULL;
	Unknown4BCA80EntryStruct *client;
	Unknown4BB8A0Class *message;
	char buffer[256];
	int clients;
	int count;
	int i;
	int j;
	int n;
	time_t now = time(NULL);
	if (Unknown38) {
		Unknown20.lock();
	}
	clients = Unknown04.length();
	if (now != Unknown1C) {
		Unknown1C = now;
		while (Unknown00->getIncomingConnection(&con, 0, 0)) {
			Unknown00->setBufferedWrites(con, TRUE);
			Unknown4BCA80EntryStruct entry;
			entry.Connection = con;
			Unknown04.addTail(entry);
		}
		clients = Unknown04.length();
		j = 0;
		for (i = 0; i < clients; i++) {
			if (!Unknown04.getPointer(&client, j)) {
				break;
			}
			for (n = client->Connection->read((uint8 *)buffer, 255, 0); n > 0; n = client->Connection->read((uint8 *)buffer, 255, 0)) {
				Unknown4BDFE0Class filter;
				buffer[n] = 0;
				if ((int)strlen(buffer) >= n) {
					client->Connection->unread((uint8 *)buffer, n);
					break;
				}
				Wstring text;
				text.set(buffer);
				if (filter.Unknown_4BE7A0(text)) {
					if (filter.Type == 0xFFFFFF) {
						client->Filters.clear();
						client->Mask = 0;
					} else {
						client->Filters.addTail(filter);
						client->Mask |= filter.Type;
					}
				}
				client->Connection->unread((uint8 *)buffer + strlen(buffer) + 1, n - strlen(buffer) - 1);
			}
			if (n == 0) {
				Unknown00->setBufferedWrites(client->Connection, FALSE);
				client->Connection->close(TRUE);
				delete client->Connection;
				Unknown04.remove(j);
			} else {
				j++;
			}
		}
	}
	count = Messages.length();
	for (i = 0; i < count; i++) {
		Messages.getPointer(&message, i);
		Wstring text;
		message->To_String(text);
		for (j = 0; j < clients; j++) {
			Unknown04.getPointer(&client, j);
			if (Unknown_4BD3F0(client, message)) {
				client->Connection->write((uint8 *)text.get(), text.length() + 1, 0);
			}
		}
	}
	Messages.clear();
	Unknown00->pumpWriters();
	if (Unknown38) {
		Unknown20.unlock();
	}
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
**	client.exe 0x4BCA80-0x4BD470: the server's constructor, destructor, listener, 0x4BCD30 (once a second:
**	accept clients, read their filter lines (0xFFFFFF clears a client's filters), drop closed ones; then
**	send every queued message's text to the clients whose filters take it, and empty the queue) and two
**	unreferenced methods (0x4BD3F0 a client's filters against a message, 0x4BCD30 inlines it; 0x4BD470
**	another way in for a message, under the lock), with the ArrayList instances they use (0x4BD3A0-0x4BDEF0).
*/
