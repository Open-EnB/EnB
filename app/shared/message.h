// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (client file name unknown; name chosen, D198)
**
**	E&B's message reporters (printf-style, cdecl: sender, category, __FILE__, __LINE__, format, ...)
**	and the message server they feed (the message server task 0x8B20D0 makes it, with
**	-ENABLE_MESSAGES only): a reporter does nothing while no server is set (0xBE55D0).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MESSAGE_H  // NAME 2 [message.h]: chosen file name (D198): the object has no __FILE__
#define __MESSAGE_H

// PLACEMENT: provisional (D137): the object 0x4BB8A0-0x4BE9D0 between kv_tripl.cpp's and avatar_desc.cpp's
// has no __FILE__, assert or class name; a string or an assert of its own would place it

#include "..\..\libs\wwonline\wlib\arraylist.h"
#include "..\..\libs\wwonline\wlib\wstring.h"
#include "..\..\libs\wwonline\wlib\wtime.h"
#include "..\..\libs\wwonline\wlib\critsec.h"
#include <stdio.h>

class TCPMgr;
class ConfigFile;
class TCPCon;

// One message (0x34 bytes; the server's list holds them: ArrayList's growVector 0x4BC9B0, the copy 0x4BC8F0)
class MessageRecordClass	// NAME 2: what it holds, one reported message (D198)
{
public:
	MessageRecordClass();					// 0x4BB8A0
	~MessageRecordClass();					// 0x4BB930

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
class MessageFilterClass	// NAME 2: what it does, filters messages (D198)
{
public:
	MessageFilterClass();								// 0x4BDFE0
	~MessageFilterClass();								// 0x4BDFE0's destructor 0x4BE060
	bit8 Unknown_4BE0F0(FILE *out, char *section);		// 0x4BE0F0 writes it as an ini section (no caller)  // NAME 1: neutral (D10)
	bit8 Unknown_4BE1E0(ConfigFile &config, char *section);	// 0x4BE1E0 reads it from one (no caller)  // NAME 1: neutral (D10)
	bit8 Unknown_4BE2C0(MessageRecordClass *message);	// 0x4BE2C0 the message passes it  // NAME 1: neutral (D10)
	bit8 Unknown_4BE7A0(Wstring &text);					// 0x4BE7A0 parses it  // NAME 1: neutral (D10)

	Wstring Name;			// +0x00  // NAME 5: the client's ini key "Name" (0x4BE0F0 / 0x4BE1E0)
	int Type;				// +0x08 the levels it takes (0xFFFFFF: clears the client's filters)  // NAME 5: ini key "Type"
	Wstring Modules;		// +0x0C senders / categories (',' or ';' separated)  // NAME 5: ini key "Modules"
	Wstring Files;			// +0x14 __FILE__ names (',' or ';' separated)  // NAME 5: ini key "Files"
	Wstring Prefix;			// +0x1C the text's start  // NAME 5: ini key "Prefix"
};

// One connected client of the server (0x14 bytes)
struct MessageServerProtocolEntryStruct	// NAME 2: a client of the message server (D198)
{
	MessageServerProtocolEntryStruct() : Mask(0) {}	// (0x4BCD30's store order: the mask with the list, before the connection)

	ArrayList<MessageFilterClass> Filters;	// +0x00  // NAME 2: what it holds
	uint32 Mask;							// +0x0C the levels its filters take  // NAME 2: what it holds
	TCPCon *Connection;						// +0x10  // NAME 2: what it holds
};

// The message server (constructor 0x4BCA80, destructor 0x4BCB50; one instance, 0xBE55D0)
class MessageServerProtocolClass	// NAME 2: the message server protocol (D198)
{
public:
	MessageServerProtocolClass();								// 0x4BCA80
	~MessageServerProtocolClass();								// 0x4BCB50
	void Unknown_4BCCB0();								// 0x4BCCB0 listens on the first free port of 9999-10009  // NAME 1: neutral (D10)
	void Unknown_4BD470(int level, Wstring &sender, Wstring &file, int line, int x, int y, Wstring &text);	// 0x4BD470 (no caller)  // NAME 1: neutral (D10)
	bit8 Unknown_4BD3F0(MessageServerProtocolEntryStruct *client, MessageRecordClass *message);	// 0x4BD3F0 (no caller; this unused)  // NAME 1: neutral (D10)
	int Unknown_4BCD10();								// 0x4BCD10 the clients' count  // NAME 1: neutral (D10)
	void Unknown_4BCD30();								// 0x4BCD30 accepts clients, reads their filters, sends them the messages (the error reporters call it after the message)  // NAME 1: neutral (D10)
	void Unknown_4BCCF0(bool value);					// 0x4BCCF0 Unknown38  // NAME 1: neutral (D10)

	TCPMgr *Unknown00;		// +0x00 (0x28 bytes, made by the constructor)
	ArrayList<MessageServerProtocolEntryStruct> Unknown04;	// +0x04 the clients
	ArrayList<MessageRecordClass> Messages;	// +0x10  // NAME 2: what it holds
	int Unknown1C;			// +0x1C
	CritSec Unknown20;		// +0x20
	bool Unknown38;			// +0x38
};

MessageServerProtocolClass *Get_Internal_Server_Protocol();		// 0x4BBE90 the server  // NAME 5: msgserv.cpp's assert 'Get_Internal_Server_Protocol() == NULL'
void Set_Internal_Server_Protocol(MessageServerProtocolClass *server);	// 0x4BBEB0  // NAME 1: the getter's pair

// The reporters, by the level they pass (and the profiler label they run under)
void Message_Debug(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BBED0 1 "Message_Debug"  // NAME 5: the client's WWPROFILE("Message_Debug") label (D198)
void Message_Info(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC000 2 "Message_Info"  // NAME 5: the client's WWPROFILE("Message_Info") label (D198)
void Message_Warn(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC130 4 "Message_Warn"  // NAME 5: the client's WWPROFILE("Message_Warn") label (D198)
void Message_Error(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 8 "Message_Error", then 0x4BCD30  // NAME 5: the client's WWPROFILE("Message_Error") label (D198)
void Message_Critical(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC3A0 0x20 (no label), then 0x4BCD30  // NAME 2: what it does: level 0x20, the highest; the client's name is unknown (D198)
void Message_Stat(const char *sender, const char *category, const char *file, int line, int x, int y, const char *format, ...);	// 0x4BC460 0x10 "Message_Stat" (a debug window's line)  // NAME 5: the client's WWPROFILE("Message_Stat") label (D198)
// what they all hand the text to ("Message_Common")
void Message_Common(const char *sender, const char *category, const char *file, int line, int level, int x, int y, const char *text);	// 0x4BC5A0  // NAME 5: the client's WWPROFILE("Message_Common") label (D198)

#endif //__MESSAGE_H
