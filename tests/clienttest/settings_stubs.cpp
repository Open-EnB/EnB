// Earth & Beyond client (reconstruction)
// clienttest: a hand stand-in gen_stubs.py cannot write (it skips a class returned by value): the settings store's
// Get_String (0x7CCF20), which settings_store.cpp's lkey thread 0x7CB8F0 calls. The WinInet imports it also uses
// come from WININET.LIB (clienttest.py's 'sys' list).
#include <windows.h>
#include <string>
#include "clienttest.h"
#include "..\..\..\app\client\tasks\tsk_game.h"

std::string Unknown_7CA2D0Class::Get_String(const char *)
{
	Trace("stub Unknown_7CA2D0Class::Get_String (0x7CCF20)");
	return std::string();
}
