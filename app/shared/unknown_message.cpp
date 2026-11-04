// Earth & Beyond client (reconstruction): E&B's message reporters (unknown_message.h). Notes at the end of the file.
#include "unknown_message.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\wwdebug\wwprofile.h"

// PLACEMENT: provisional (D137): see unknown_message.h

static Unknown4BCA80Class *Unknown_BE55D0;	// 0xBE55D0 the server  // NAME 1: neutral (D10)

// 0x4BBE90
Unknown4BCA80Class *Get_Internal_Server_Protocol()
{
	return Unknown_BE55D0;
}

// 0x4BBEB0
void Set_Internal_Server_Protocol(Unknown4BCA80Class *server)
{
	Unknown_BE55D0 = server;
}

// 0x4BBED0
void Unknown_4BBED0(const char *sender, const char *category, const char *file, int line, const char *format, ...)
{
	WWPROFILE("Message_Debug");
	if (Unknown_BE55D0 == NULL || !Unknown_BE55D0->Unknown_4BCD10()) {
		return;
	}
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	Unknown_4BC5A0(sender, category, file, line, 1, 0, 0, buffer);
}

// 0x4BC000
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...)
{
	WWPROFILE("Message_Info");
	if (Unknown_BE55D0 == NULL || !Unknown_BE55D0->Unknown_4BCD10()) {
		return;
	}
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	Unknown_4BC5A0(sender, category, file, line, 2, 0, 0, buffer);
}

// 0x4BC130
void Unknown_4BC130(const char *sender, const char *category, const char *file, int line, const char *format, ...)
{
	WWPROFILE("Message_Warn");
	if (Unknown_BE55D0 == NULL || !Unknown_BE55D0->Unknown_4BCD10()) {
		return;
	}
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	Unknown_4BC5A0(sender, category, file, line, 4, 0, 0, buffer);
}

// 0x4BC260
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...)
{
	WWPROFILE("Message_Error");
	if (Unknown_BE55D0 == NULL || !Unknown_BE55D0->Unknown_4BCD10()) {
		return;
	}
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	Unknown_4BC5A0(sender, category, file, line, 8, 0, 0, buffer);
	Unknown_BE55D0->Unknown_4BCD30();
}

// 0x4BC3A0
void Unknown_4BC3A0(const char *sender, const char *category, const char *file, int line, const char *format, ...)
{
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	if (Unknown_BE55D0 != NULL && Unknown_BE55D0->Unknown_4BCD10()) {
		Unknown_4BC5A0(sender, category, file, line, 0x20, 0, 0, buffer);
		Unknown_BE55D0->Unknown_4BCD30();
	}
}

// 0x4BC460
void Unknown_4BC460(const char *sender, const char *category, const char *file, int line, int x, int y, const char *format, ...)
{
	WWPROFILE("Message_Stat");
	if (Unknown_BE55D0 == NULL || !Unknown_BE55D0->Unknown_4BCD10()) {
		return;
	}
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	Unknown_4BC5A0(sender, category, file, line, 0x10, x, y, buffer);
}

// 0x4BC5A0
void Unknown_4BC5A0(const char *sender, const char *category, const char *file, int line, int level, int x, int y, const char *text)
{
	WWPROFILE("Message_Common");
	if (Unknown_BE55D0 == NULL) {
		return;
	}
	Unknown4BB8A0Class message;
	message.Level = level;
	message.Sender.set((char *)sender);
	if (message.Sender.length() != 0) {
		message.Sender += ",";
	}
	message.Sender += (char *)category;
	const char *name;
	while ((name = strchr(file, '\\')) != NULL) {
		file = name + 1;
	}
	while ((name = strchr(file, '/')) != NULL) {
		file = name + 1;
	}
	message.File.set((char *)file);
	message.Line = line;
	message.Text.set((char *)text);
	message.Time.Update();
	message.X = x;
	message.Y = y;
	Unknown_BE55D0->Messages.addTail(message);
	Unknown_BE55D0->Unknown_4BCD30();
}

/*
**	Reconstruction notes
**
**	client.exe 0x4BBE90-0x4BC9B0: the server's accessors, the reporters (the profiled ones return early,
**	which puts the sample's destructor on both paths as the client has it) and their common path
**	0x4BC5A0, which appends a record to the server's ArrayList (its copy 0x4BC8F0 and growVector 0x4BC9B0
**	are this object's instances). The record's own functions and the server's 0x4BCD10 are called, not
**	inlined: they are other objects' (unknown_message_record.cpp, unknown_message_server.cpp).
*/
