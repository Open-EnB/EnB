// Earth & Beyond client (reconstruction): the settings store's lkey check (client 0x7CB6F0-0x7CBC60).
// File PROVISIONAL (D137): no string ties it to a file; the store's other functions (0x7C97B0-0x7CCF20) unbuilt. Notes at the end.
#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include "tsk_game.h"

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

/*
**	Reconstruction notes
**
**	client.exe: Unknown_7CA2D0Class (tsk_game.h), the settings store, is a function-static object (0x1B4
**	bytes at 0xBE8890, constructor 0x7C97B0). 0x7CB6F0 is the login and game tasks' per-frame poll: with
**	an lkey URL set (0x7CB610, auth.ini's LKeyUrl) and a deadline passed it starts 0x7CB8F0 on a thread,
**	which fetches the URL formatted with the stored 'lkey' and returns 1 for a reply starting "Success";
**	the next poll collects the exit code and sets the next deadlines (15 min after a success, 90 / 115 min
**	after a failure). NOT EXACT (D64, functional for the POC): 0x7CB6F0, 0x7CB8F0 (the reply read with
**	strlen as the client does: the 100-byte buffer is not terminated). ACT (run 2026-11-02).
*/
