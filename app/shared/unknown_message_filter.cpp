// Earth & Beyond client (reconstruction): the message server's client filter (unknown_message.h). Notes at the end of the file.
#include "unknown_message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\libs\wwonline\wlib\configfile.h"

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

// 0x4BE0F0
bit8 Unknown4BDFE0Class::Unknown_4BE0F0(FILE *out, char *section)
{
	fprintf(out, "[%s]%s", section, "\r\n");
	fprintf(out, "Name = %s%s", Unknown00.get(), "\r\n");
	fprintf(out, "Type = %d%s", Unknown08, "\r\n");
	fprintf(out, "Modules = %s%s", Unknown0C.get(), "\r\n");
	fprintf(out, "Files = %s%s", Unknown14.get(), "\r\n");
	fprintf(out, "Prefix = %s%s", Unknown1C.get(), "\r\n");
	fprintf(out, "%s", "\r\n");
	return TRUE;
}

// 0x4BE1E0
bit8 Unknown4BDFE0Class::Unknown_4BE1E0(ConfigFile &config, char *section)
{
	if (!config.getString("Name", Unknown00, section)) {
		return FALSE;
	}
	sint32 type;
	if (!config.getInt("Type", type, section)) {
		return FALSE;
	}
	Unknown08 = type;
	if (!config.getString("Modules", Unknown0C, section)) {
		return FALSE;
	}
	if (!config.getString("Files", Unknown14, section)) {
		return FALSE;
	}
	Unknown1C.set("");
	config.getString("Prefix", Unknown1C, section);
	return TRUE;
}

// 0x4BE2C0
// NOT EXACT: shape (explain.py: 14 masked rows; 0.80): register allocation only: the client keeps pass in bl
// (with its stack home) after the module test, ours reloads it from the stack; every call and branch is
// the client's (25 forms tried: helpers, loop and test forms, bool, early returns)
bit8 Unknown4BDFE0Class::Unknown_4BE2C0(Unknown4BB8A0Class *message)
{
	bit8 pass = TRUE;

	if ((message->Level & Unknown08) == 0) {
		return FALSE;
	}

	if (Unknown0C.length() != 0) {
		Wstring module;
		Wstring sender;
		for (int offset = Unknown0C.getToken(0, ",;", module); offset != -1; offset = Unknown0C.getToken(offset, ",;", module)) {
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

	if (pass && Unknown14.length() != 0) {
		Wstring file;
		for (int pos = Unknown14.getToken(0, ",;", file); pos != -1; pos = Unknown14.getToken(pos, ",;", file)) {
			file.removeSpaces();
			if (file.length() != 0 && file == message->File) {
				break;
			}
		}
		if (file.length() != 0 && file != message->File) {
			pass = FALSE;
		}
	}

	if (pass && Unknown1C.length() != 0) {
		while (Unknown1C.length() != 0) {
			if (Unknown1C.get()[Unknown1C.length() - 1] != ' ') {
				break;
			}
			Unknown1C.truncate(Unknown1C.length() - 1);
		}
		if (strncmp(Unknown1C.get(), message->Text.get(), Unknown1C.length()) != 0) {
			return FALSE;
		}
	}

	return pass;
}

// 0x4BE7A0
bit8 Unknown4BDFE0Class::Unknown_4BE7A0(Wstring &text)
{
	Wstring type;

	if (text.get(0) != 'S') {
		return FALSE;
	}
	int pos = text.getToken(1, "`", Unknown00);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", type);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", Unknown0C);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", Unknown14);
	if (pos == -1) {
		return FALSE;
	}
	pos = text.getToken(pos, "`", Unknown1C);
	if (pos == -1) {
		return FALSE;
	}
	Unknown08 = atol(type.get());
	return TRUE;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4BDFE0-0x4BE9D0. The client's text ('S' then '`'-separated name, type, modules,
**	files, prefix: 0x4BE7A0) and the ini form (0x4BE0F0 / 0x4BE1E0, no callers) give the fields'
**	keys: Name, Type, Modules, Files, Prefix.
*/
