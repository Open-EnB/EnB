#include "sclient.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"







































// 0x691570 (the destructor 0x691680, deleting 0x691650)
LoadingViewClass::LoadingViewClass(ClientTaskClass *ctask) :
	Unknown65B810ViewClass(ctask, 0),
	Unknown50(0),
	Unknown54(0),
	Unknown68(false),
	Unknown69(false)
{
	G_ASSERT(ctask);
}

LoadingViewClass::~LoadingViewClass()
{
}

// 0x691770: slot +0x04: the base's Init, then both scenes' fog off
void LoadingViewClass::Unknown_04()
{
	UIViewClass::Unknown_04();
	Unknown1C->Set_Fog_Enable(false);
	Unknown24->Set_Fog_Enable(false);
}

// 0x6917A0: slot +0x10: UIViewClass's Shutdown (past the second view's 0x65B8B0), then the loading screen's objects
void LoadingViewClass::Unknown_10()
{
	UIViewClass::Unknown_10();
	Unknown58 = NULL;
	Unknown5C = NULL;
	Unknown60 = NULL;
	Unknown64 = NULL;
}

/*
**	Notes (LD4, 2026-10-29)
**
**	loadingview.cpp is the loading view (LoadingViewClass, sclient.h: 0x6C bytes over uiview.cpp's second view,
**	table 0xAFCF7C), client 0x6912A0-0x6927C0 (appfile.py show app\client\loadingview.cpp; the constructor's
**	G_ASSERT 'ctask' on the client's line 52, __FILE__ e:\build\beta\G\app\Client\loadingview.cpp).
**	Not rebuilt: 0x6912A0 / 0x691320 before the constructor, SERVER_HANDOFF's 0x691830 and its string wrapper
**	0x692160, START 0x692330 and the loading screen 0x692420 (slot +0x0C): +0x58..+0x64 are RefCountPtrs of a
**	class with a slot +0x38 that FontManagerClass::Unknown_6D2310 takes by reference, not yet declared.
*/
