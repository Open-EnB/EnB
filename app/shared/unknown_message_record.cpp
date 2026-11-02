// Earth & Beyond client (reconstruction): the message server's record (unknown_message.h). Notes at the end of the file.
#include "unknown_message.h"
#include <stdio.h>
#include <stdlib.h>

// PLACEMENT: provisional (D137): see unknown_message.h; its own object because the reporters
// (unknown_message.cpp) call its constructor 0x4BB8A0 and destructor 0x4BB930 instead of inlining them

// 0x4BB8A0
Unknown4BB8A0Class::Unknown4BB8A0Class() :
	Level(0),
	Line(0),
	X(0),
	Y(0),
	Time(0)
{
}

// 0x4BB930
Unknown4BB8A0Class::~Unknown4BB8A0Class()
{
}

// 0x4BB9C0
void Unknown4BB8A0Class::To_String(Wstring &out)
{
	char buffer[128];
	char *file = (File.length() != 0) ? File.get() : "NULL";
	char *sender = (Sender.length() != 0) ? Sender.get() : "NULL";
	sprintf(buffer, "M%d`%s`%s`%d`%d`%d`%d.%06d`", Level, sender, file, Line, X, Y, Time.GetSec(), Time.GetUsec());
	out.set(buffer);
	out += Text;
}

// 0x4BBAA0
bit8 Unknown4BB8A0Class::From_String(Wstring &in)
{
	Wstring level;
	Wstring line;
	Wstring x;
	Wstring y;
	Wstring sec;
	Wstring usec("0");
	if (in.get(0) != 'M') {
		return(FALSE);
	}
	int pos = in.getToken(1, "`", level);
	if (pos == -1) return(FALSE);
	pos = in.getToken(pos, "`", Sender);
	if (pos == -1) return(FALSE);
	pos = in.getToken(pos, "`", File);
	if (pos == -1) return(FALSE);
	pos = in.getToken(pos, "`", line);
	if (pos == -1) return(FALSE);
	pos = in.getToken(pos, "`", x);
	if (pos == -1) return(FALSE);
	pos = in.getToken(pos, "`", y);
	if (pos == -1) return(FALSE);
	pos = in.getToken(pos, "`.", sec);
	if (pos == -1) return(FALSE);
	if (in.get()[pos] == '.') {
		pos = in.getToken(pos, "`.", usec);
		if (pos == -1) return(FALSE);
	}
	Text.set(in.get() + pos + 1);
	Level = atol(level.get());
	Line = atol(line.get());
	X = atol(x.get());
	Y = atol(y.get());
	Time.SetSec(atol(sec.get()));
	Time.SetUsec(atol(usec.get()));
	return(TRUE);
}


/*
**	Reconstruction notes
**
**	client.exe 0x4BB8A0-0x4BBAA0: one message of the server's list, its text form for the wire
**	(To_String) and back (From_String).
*/
