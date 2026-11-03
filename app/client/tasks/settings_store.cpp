// Earth & Beyond client (reconstruction): the settings store (client 0x7C97B0-0x7CCF20: constructor, accessor, URL check, string settings).
// File PROVISIONAL (D137): no string ties it to a file; the store's other functions (0x7CA340-0x7CB4E0, 0x7CBC60-0x7CCB30) unbuilt. Notes at the end.
#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <string.h>
#include "tsk_game.h"

// 0x7C97B0
// NOT EXACT: shape (functional for the POC: the client's constructor also sets the two vtables, counts itself in a global,
// and fills the browser descriptors at +0x70 ('SUPPORT', 'PRIVACY', ... with their http://support.net-7.org URLs); only the
// state the login path reads is set here: the empty strings and map, the URL-check deadlines)
Unknown_7CA2D0Class::Unknown_7CA2D0Class()
{
	memset(Unknown00, 0, sizeof(Unknown00));
	Unknown2C = 0;
	memset(Unknown30, 0, sizeof(Unknown30));
	Unknown34 = false;
	unsigned int now = GetTickCount();
	Unknown38 = now + 5400000;	// 90 minutes
	Unknown3C = 0;
	Unknown40 = now + 6900000;	// 115 minutes
	Unknown44 = 0;
	Unknown48 = NULL;
	Unknown4C = 0;
	memset(Unknown70, 0, sizeof(Unknown70));
}

// 0x7CA2D0
// NOT EXACT: schedule (the client runs the constructor through a jump thunk and registers the destructor thunk with atexit)
Unknown_7CA2D0Class *Unknown_7CA2D0()
{
	static Unknown_7CA2D0Class store;
	return &store;
}

// @0x7CB610
void Unknown_7CA2D0Class::Unknown_7CB610(const char *url)
{
	Unknown50 = url;
}

// NOT EXACT: shape (functional: the deadline tests' branch layout differs)
void Unknown_7CA2D0Class::Unknown_7CB6F0(int force)
{
	if (Unknown50.length() == 0) {
		return;
	}

	unsigned int now = GetTickCount();
	if (Unknown48 == NULL) {
		if (Unknown44 != 0 && Unknown44 - now > 0x80000000) {
			Unknown4C = 4;
		} else if (Unknown40 - now > 0x80000000) {
			Unknown4C = 2;
		} else {
			if (!(char)force) {
				return;
			}
			if (Unknown3C != 0 && Unknown3C - now > 0x80000000) {
				Unknown4C = 3;
			} else if (Unknown38 - now > 0x80000000) {
				Unknown4C = 1;
			} else {
				return;
			}
		}
		DWORD id;
		Unknown48 = CreateThread(NULL, 0, Unknown_7CB8F0, this, 0, &id);
	} else {
		DWORD code = 0;
		GetExitCodeThread(Unknown48, &code);
		if (code == STILL_ACTIVE) {
			return;
		}
		if (code == 0) {
			switch (Unknown4C) {
				case 1:
				case 3:
					Unknown3C = now + 900000;
					Unknown38 = now + 900000;
					break;
				case 2:
				case 4:
					Unknown3C = 0;
					Unknown44 = now + 900000;
					Unknown40 = now + 900000;
					Unknown38 = now + 900000;
					break;
			}
		} else {
			Unknown38 = now + 5400000;
			Unknown3C = 0;
			Unknown40 = now + 6900000;
			Unknown44 = 0;
		}
		CloseHandle(Unknown48);
		Unknown48 = NULL;
		Unknown4C = 0;
	}
}

// NOT EXACT: inline-budget (the client expands string appends: _Eos / _Grow, and the store's static init with atexit)
unsigned long __stdcall Unknown_7CA2D0Class::Unknown_7CB8F0(void *store)
{
	Unknown_7CA2D0Class *self = (Unknown_7CA2D0Class *)store;
	Unknown_7CA2D0();
	unsigned long result = 0;
	std::string lkey = self->Get_String("lkey");
	if (lkey.length() != 0) {
		char url[1024];
		_snprintf(url, sizeof(url), self->Unknown50.c_str(), lkey.c_str());
		HINTERNET internet = InternetOpenA("ENBCLIENT", 0, NULL, NULL, 0);
		if (internet != NULL) {
			std::string reply;
			HINTERNET request = InternetOpenUrlA(internet, url, NULL, 0, 0x84003100, 0);
			if (request != NULL) {
				char buffer[100];
				DWORD read = 0;
				while (InternetReadFile(request, buffer, sizeof(buffer), &read) && read != 0) {
					reply.append(buffer, strlen(buffer));
					buffer[0] = '\0';
				}
				if (reply.substr(0, 7) == "Success") {
					result = 1;
				}
				InternetCloseHandle(request);
			}
			InternetCloseHandle(internet);
		}
	}
	return(result);
}

// 0x7CCB30
// NOT EXACT: shape (the client's std::map insert / erase and string assigns are inlined helpers; the order of the temporaries differs)
void Unknown_7CA2D0Class::Set_String(const char *key, const char *value)
{
	char canon_key[2084];
	DWORD canon_key_size = sizeof(canon_key);
	canon_key[0] = '\0';
	if (!InternetCanonicalizeUrlA(key, canon_key, &canon_key_size, 0)) {
		return;
	}
	if (value != NULL) {
		char canon_value[2084];
		DWORD canon_value_size = sizeof(canon_value);
		canon_value[0] = '\0';
		if (!InternetCanonicalizeUrlA(value, canon_value, &canon_value_size, 0)) {
			return;
		}
		if (strlen(value) != 0) {
			Unknown60[std::string(canon_key)] = canon_value;
			return;
		}
	}
	std::map<std::string, std::string>::iterator it = Unknown60.find(std::string(canon_key));
	if (it != Unknown60.end()) {
		Unknown60.erase(it);
	}
}

// @0x7CCE80
void Unknown_7CA2D0Class::Set_Int(const char *key, int value)
{
	char buffer[100];
	_snprintf(buffer, 100, "%d", value);
	Set_String(key, buffer);
}

// @0x7CCED0
void Unknown_7CA2D0Class::Set_Int64(const char *key, __int64 value)
{
	char buffer[100];
	_snprintf(buffer, 100, "%I64d", value);
	Set_String(key, buffer);
}

// 0x7CCF20
// NOT EXACT: shape (the client reads the entry with the map's operator[], which adds an empty one for a missing key, then
// canonicalises the value; its failures return the empty default string)
std::string Unknown_7CA2D0Class::Get_String(const char *key)
{
	char canon_key[2084];
	DWORD canon_key_size = sizeof(canon_key);
	canon_key[0] = '\0';
	if (!InternetCanonicalizeUrlA(key, canon_key, &canon_key_size, 0)) {
		return std::string("");
	}
	std::string &entry = Unknown60[std::string(canon_key)];
	char canon_value[2084];
	DWORD canon_value_size = sizeof(canon_value);
	canon_value[0] = '\0';
	if (!InternetCanonicalizeUrlA(entry.c_str(), canon_value, &canon_value_size, 0)) {
		return std::string("");
	}
	return std::string(canon_value);
}

/*
**	Reconstruction notes
**
**	client.exe: Unknown_7CA2D0Class (tsk_game.h), the settings store, is a function-static object (0x1B4
**	bytes at 0xBE8890, constructor 0x7C97B0, accessor 0x7CA2D0). Set_String / Get_String keep canonicalised
**	(InternetCanonicalizeUrlA) keys and values in a std::map at +0x60 (an empty value erases the entry).
**	0x7CB6F0 is the login and game tasks' per-frame poll: with
**	an lkey URL set (0x7CB610, auth.ini's LKeyUrl) and a deadline passed it starts 0x7CB8F0 on a thread,
**	which fetches the URL formatted with the stored 'lkey' and returns 1 for a reply starting "Success";
**	the next poll collects the exit code and sets the next deadlines (15 min after a success, 90 / 115 min
**	after a failure; the constructor's first ones: 90 and 115 minutes after start, so no fetch before then).
**	NOT EXACT (D64, functional for the POC): 0x7C97B0 (no vtables, no browser descriptors), 0x7CA2D0, 0x7CB6F0,
**	0x7CB8F0 (the reply read with strlen as the client does: the 100-byte buffer is not terminated), 0x7CCB30,
**	0x7CCF20. Not built here: the destructor 0x7CA340, 0x7CB2E0 / 0x7CB3D0 (the callers in ui_login.cpp,
**	ui_popupmsg.cpp and main_client.cpp use other class types for them). ACT (run 2026-11-02).
*/
