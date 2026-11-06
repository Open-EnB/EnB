// Earth & Beyond client (reconstruction): E&B's message reporters (message.h). Notes at the end of the file.
#include "message.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\wwdebug\wwprofile.h"

// PLACEMENT: provisional (D137): see message.h

static MessageServerProtocolClass *Unknown_BE55D0;	// 0xBE55D0 the server  // NAME 1: neutral (D10)

// 0x4BBE90
MessageServerProtocolClass *Get_Internal_Server_Protocol()
{
	return Unknown_BE55D0;
}

// 0x4BBEB0
void Set_Internal_Server_Protocol(MessageServerProtocolClass *server)
{
	Unknown_BE55D0 = server;
}

// 0x4BBED0
void Message_Debug(const char *sender, const char *category, const char *file, int line, const char *format, ...)
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
	Message_Common(sender, category, file, line, 1, 0, 0, buffer);
}

// 0x4BC000
void Message_Info(const char *sender, const char *category, const char *file, int line, const char *format, ...)
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
	Message_Common(sender, category, file, line, 2, 0, 0, buffer);
}

// 0x4BC130
void Message_Warn(const char *sender, const char *category, const char *file, int line, const char *format, ...)
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
	Message_Common(sender, category, file, line, 4, 0, 0, buffer);
}

// 0x4BC260
void Message_Error(const char *sender, const char *category, const char *file, int line, const char *format, ...)
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
	Message_Common(sender, category, file, line, 8, 0, 0, buffer);
	Unknown_BE55D0->Unknown_4BCD30();
}

// 0x4BC3A0
void Message_Critical(const char *sender, const char *category, const char *file, int line, const char *format, ...)
{
	char buffer[2048 + 1];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, 2048, format, args);
	buffer[2048] = 0;
	if (Unknown_BE55D0 != NULL && Unknown_BE55D0->Unknown_4BCD10()) {
		Message_Common(sender, category, file, line, 0x20, 0, 0, buffer);
		Unknown_BE55D0->Unknown_4BCD30();
	}
}

// 0x4BC460
void Message_Stat(const char *sender, const char *category, const char *file, int line, int x, int y, const char *format, ...)
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
	Message_Common(sender, category, file, line, 0x10, x, y, buffer);
}

// 0x4BC5A0
void Message_Common(const char *sender, const char *category, const char *file, int line, int level, int x, int y, const char *text)
{
	WWPROFILE("Message_Common");
	if (Unknown_BE55D0 == NULL) {
		return;
	}
	MessageRecordClass message;
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
**	inlined: they are other objects' (message_record.cpp, message_server.cpp).
*/
