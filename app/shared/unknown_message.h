// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown; neutral name, D10)
**
**	E&B's message reporters (printf-style, cdecl: sender, category, __FILE__, __LINE__, format, ...)
**	and the message server they feed (the message server task 0x8B20D0 makes it, with
**	-ENABLE_MESSAGES only): a reporter does nothing while no server is set (0xBE55D0).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_MESSAGE_H  // NAME 1 [unknown_message.h]: neutral file name (D10): the object has no __FILE__
#define __UNKNOWN_MESSAGE_H

// PLACEMENT: provisional (D137): the object 0x4BB8A0-0x4BE9D0 between kv_tripl.cpp's and avatar_desc.cpp's
// has no __FILE__, assert or class name; a string or an assert of its own would place it

#include "..\..\libs\wwonline\wlib\arraylist.h"
#include "..\..\libs\wwonline\wlib\wstring.h"
#include "..\..\libs\wwonline\wlib\wtime.h"
#include "..\..\libs\wwonline\wlib\critsec.h"

class TCPMgr;
class TCPCon;

// One message (0x34 bytes; the server's list holds them: ArrayList's growVector 0x4BC9B0, the copy 0x4BC8F0)
class Unknown4BB8A0Class	// NAME 1: after its constructor (D10)
{
public:
	Unknown4BB8A0Class();					// 0x4BB8A0
	~Unknown4BB8A0Class();					// 0x4BB930

	void To_String(Wstring &out);			// 0x4BB9C0 'M%d`%s`%s`%d`%d`%d`%d.%06d`' and the text  // NAME 2: what it does
	bit8 From_String(Wstring &in);			// 0x4BBAA0 its inverse  // NAME 2: what it does

	int Level;			// +0x00 the reporter's (1 debug ... 0x20)  // NAME 2: what it holds
	Wstring Sender;		// +0x04 sender ',' category  // NAME 2: what it holds
	Wstring File;		// +0x0C __FILE__ without its directories  // NAME 2: what it holds
	int Line;			// +0x14  // NAME 2: what it holds
	int X;				// +0x18 Message_Stat's position  // NAME 1: neutral
	int Y;				// +0x1C  // NAME 1: neutral
	Wtime Time;			// +0x20 when  // NAME 2: what it holds
	Wstring Text;		// +0x2C  // NAME 2: what it holds
};

// A connected client's filter (0x24 bytes; parsed by 0x4BE7A0 from the client's text, matched by 0x4BE2C0;
// its own object from 0x4BDFE0: the server calls its constructor and destructor)
class Unknown4BDFE0Class	// NAME 1: after its constructor (D10)
{
public:
	Unknown4BDFE0Class();								// 0x4BDFE0
	~Unknown4BDFE0Class();								// 0x4BDFE0's destructor 0x4BE060
	bit8 Unknown_4BE2C0(Unknown4BB8A0Class *message);	// 0x4BE2C0 the message passes it  // NAME 1: neutral (D10)
	bit8 Unknown_4BE7A0(Wstring &text);					// 0x4BE7A0 parses it  // NAME 1: neutral (D10)

	Wstring Unknown00;		// +0x00
	int Unknown08;			// +0x08 (0xFFFFFF: clears the client's filters)
	Wstring Unknown0C;		// +0x0C
	Wstring Unknown14;		// +0x14
	Wstring Unknown1C;		// +0x1C
};

// One connected client of the server (0x14 bytes)
struct Unknown4BCA80EntryStruct	// NAME 1: after the server's constructor (D10)
{
	Unknown4BCA80EntryStruct() : Mask(0) {}	// (0x4BCD30's store order: the mask with the list, before the connection)

	ArrayList<Unknown4BDFE0Class> Filters;	// +0x00  // NAME 2: what it holds
	uint32 Mask;							// +0x0C the levels its filters take  // NAME 2: what it holds
	TCPCon *Connection;						// +0x10  // NAME 2: what it holds
};

// The message server (constructor 0x4BCA80, destructor 0x4BCB50; one instance, 0xBE55D0)
class Unknown4BCA80Class	// NAME 1: after its constructor (D10)
{
public:
	Unknown4BCA80Class();								// 0x4BCA80
	~Unknown4BCA80Class();								// 0x4BCB50
	void Unknown_4BCCB0();								// 0x4BCCB0 listens on the first free port of 9999-10009  // NAME 1: neutral (D10)
	void Unknown_4BD470(int level, Wstring &sender, Wstring &file, int line, int x, int y, Wstring &text);	// 0x4BD470 (no caller)  // NAME 1: neutral (D10)
	bit8 Unknown_4BD3F0(Unknown4BCA80EntryStruct *client, Unknown4BB8A0Class *message);	// 0x4BD3F0 (no caller; this unused)  // NAME 1: neutral (D10)
	int Unknown_4BCD10();								// 0x4BCD10 the clients' count  // NAME 1: neutral (D10)
	void Unknown_4BCD30();								// 0x4BCD30 accepts clients, reads their filters, sends them the messages (the error reporters call it after the message)  // NAME 1: neutral (D10)
	void Unknown_4BCCF0(bool value);					// 0x4BCCF0 Unknown38  // NAME 1: neutral (D10)

	TCPMgr *Unknown00;		// +0x00 (0x28 bytes, made by the constructor)
	ArrayList<Unknown4BCA80EntryStruct> Unknown04;	// +0x04 the clients
	ArrayList<Unknown4BB8A0Class> Messages;	// +0x10  // NAME 2: what it holds
	int Unknown1C;			// +0x1C
	CritSec Unknown20;		// +0x20
	bool Unknown38;			// +0x38
};

Unknown4BCA80Class *Unknown_4BBE90();				// 0x4BBE90 the server  // NAME 1: neutral (D10)
void Unknown_4BBEB0(Unknown4BCA80Class *server);	// 0x4BBEB0  // NAME 1: neutral (D10)

// The reporters, by the level they pass (and the profiler label they run under)
void Unknown_4BBED0(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BBED0 1 "Message_Debug"  // NAME 1: neutral (D10)
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC000 2 "Message_Info"  // NAME 1: neutral (D10)
void Unknown_4BC130(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC130 4 "Message_Warn"  // NAME 1: neutral (D10)
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 8 "Message_Error", then 0x4BCD30  // NAME 1: neutral (D10)
void Unknown_4BC3A0(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC3A0 0x20 (no label), then 0x4BCD30  // NAME 1: neutral (D10)
void Unknown_4BC460(const char *sender, const char *category, const char *file, int line, int x, int y, const char *format, ...);	// 0x4BC460 0x10 "Message_Stat" (a debug window's line)  // NAME 1: neutral (D10)
// what they all hand the text to ("Message_Common")
void Unknown_4BC5A0(const char *sender, const char *category, const char *file, int line, int level, int x, int y, const char *text);	// 0x4BC5A0  // NAME 1: neutral (D10)

#endif //__UNKNOWN_MESSAGE_H
