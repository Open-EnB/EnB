// Earth & Beyond client (reconstruction): the message server's client filter (message.h). Notes at the end of the file.
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\libs\wwonline\wlib\configfile.h"
#include <ctype.h>
#include <string>

// PLACEMENT: provisional (D137): see message.h; its own object because the server
// (message_server.cpp) calls its constructor 0x4BDFE0 and destructor 0x4BE060 instead of inlining them

// 0x4BDFE0
MessageFilterClass::MessageFilterClass() :
	Type(0)
{
}

// 0x4BE060
MessageFilterClass::~MessageFilterClass()
{
}

// 0x4BE0F0
bit8 MessageFilterClass::Unknown_4BE0F0(FILE *out, char *section)
{
	fprintf(out, "[%s]%s", section, "\r\n");
	fprintf(out, "Name = %s%s", Name.get(), "\r\n");
	fprintf(out, "Type = %d%s", Type, "\r\n");
	fprintf(out, "Modules = %s%s", Modules.get(), "\r\n");
	fprintf(out, "Files = %s%s", Files.get(), "\r\n");
	fprintf(out, "Prefix = %s%s", Prefix.get(), "\r\n");
	fprintf(out, "%s", "\r\n");
	return TRUE;
}

// 0x4BE1E0
bit8 MessageFilterClass::Unknown_4BE1E0(ConfigFile &config, char *section)
{
	if (!config.getString("Name", Name, section)) {
		return FALSE;
	}
	sint32 type;
	if (!config.getInt("Type", type, section)) {
		return FALSE;
	}
	Type = type;
	if (!config.getString("Modules", Modules, section)) {
		return FALSE;
	}
	if (!config.getString("Files", Files, section)) {
		return FALSE;
	}
	Prefix.set("");
	config.getString("Prefix", Prefix, section);
	return TRUE;
}

// 0x4BE2C0
// NOT EXACT: shape (explain.py: 14 masked rows; 0.80): register allocation only: the client keeps pass in bl
// (with its stack home) after the module test, ours reloads it from the stack; every call and branch is
// the client's (25 forms tried: helpers, loop and test forms, bool, early returns)
bit8 MessageFilterClass::Unknown_4BE2C0(MessageRecordClass *message)
{
	bit8 pass = TRUE;

	if ((message->Level & Type) == 0) {
		return FALSE;
	}

	if (Modules.length() != 0) {
		Wstring module;
		Wstring sender;
		for (int offset = Modules.getToken(0, ",;", module); offset != -1; offset = Modules.getToken(offset, ",;", module)) {
			module.removeSpaces();
			if (module.length() == 0) {
				continue;
			}
			sender.set("");
			for (int pos = message->Sender.getToken(0, ",;", sender); pos != -1; pos = message->Sender.getToken(pos, ",;", sender)) {
				sender.removeSpaces();
				if (sender.length() != 0 && module == sender) {
					break;
				}
			}
			if (module == sender) {
				break;
			}
		}
		if (module.length() != 0 && module != sender) {
			pass = FALSE;
		}
	}

	if (pass && Files.length() != 0) {
		Wstring file;
		for (int pos = Files.getToken(0, ",;", file); pos != -1; pos = Files.getToken(pos, ",;", file)) {
			file.removeSpaces();
			if (file.length() != 0 && file == message->File) {
				break;
			}
		}
		if (file.length() != 0 && file != message->File) {
			pass = FALSE;
		}
	}

	if (pass && Prefix.length() != 0) {
		while (Prefix.length() != 0) {
			if (Prefix.get()[Prefix.length() - 1] != ' ') {
				break;
			}
			Prefix.truncate(Prefix.length() - 1);
		}
		if (strncmp(Prefix.get(), message->Text.get(), Prefix.length()) != 0) {
			return FALSE;
		}
	}

	return pass;
}

// 0x4BE7A0
bit8 MessageFilterClass::Unknown_4BE7A0(Wstring &text)
{
	Wstring type;

	if (text.get(0) != 'S') {
		return FALSE;
	}
	int pos = text.getToken(1, "`", Name);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", type);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", Modules);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", Files);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", Prefix);
	if (pos == -1) {
		return FALSE;
	}
	Type = atol(type.get());
	return TRUE;
}


// PLACEMENT: provisional (D137): 0x4BE9D0 is the last function before the 0x50-byte int3 run that ends
// at avatar_desc.cpp's first (0x4BEB50); its callers (avatar_desc.cpp's 0x4C1460 / 0x4C15F0 / Convert_Names,
// 0x5D6DC0, 0x616220, 0x649FE0) call it out of line. Its own object or this one: no string, no assert

// 0x4BE9D0 a name with its first letter upper case and the rest lower case, cut to 16 characters
std::string Unknown_4BE9D0(const char *name)	// NAME 1 [Unknown_4BE9D0]: neutral (D10)
{
	char buffer[64];
	int count = strlen(name);
	int length = (count < 16) ? count : 16;
	for (int i = 0; i < length; i++) {
		if (i == 0) {
			buffer[i] = toupper(name[i]);
		} else {
			buffer[i] = tolower(name[i]);
		}
	}
	buffer[length] = 0;
	std::string result(buffer);
	return result;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4BDFE0-0x4BEAFF. The client's text ('S' then '`'-separated name, type, modules,
**	files, prefix: 0x4BE7A0) and the ini form (0x4BE0F0 / 0x4BE1E0, no callers) give the fields'
**	keys: Name, Type, Modules, Files, Prefix.
*/
