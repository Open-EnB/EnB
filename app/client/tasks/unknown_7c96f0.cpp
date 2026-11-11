// Earth & Beyond client (reconstruction): the browsers' game dispatch object's one method (client 0x7C96F0-0x7C97B0: one object).
// File PROVISIONAL (D137): no string names it. Notes at the end.
#include <windows.h>
#include <ole2.h>
#include <atlbase.h>
#include "tsk_game.h"

// @0x7C96F0
long __stdcall Unknown_7CD500Class::Unknown_0C(unsigned short **text)
{
	CComBSTR value(Unknown_7CA2D0()->Unknown_7CB4E0());
	*text = value.Copy();
	return(S_OK);
}

/*
**	Reconstruction notes
**
**	client.exe: an object of its own between ui_starmap.cpp's (which ends at 0x7C96E0) and the settings store's
**	(settings_store.cpp, from 0x7C97B0): this one function and <string>'s guarded initialiser 0x7C9780, which closes
**	every object that includes <string>. The store's accessor 0x7CA2D0 and 0x7CB4E0 are calls here (in the store's
**	own file VC6 would expand them), so the method is not in the store's file: the class (tsk_game.h's
**	Unknown_7CD500Class, an ATL object in the client) had its own source file for its one out-of-line method; its
**	inline members are COMDATs in the store's object. ATL's CComBSTR(LPCSTR) and Copy() are inline in it.
*/
