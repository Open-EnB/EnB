// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\rs.cpp (its asserts from client 0x458370 on); this
**	part: client 0x4581E0-0x458292. Interim placement (D137): the functions sit before rs.cpp's first
**	assert and share its statics with the render state's lock functions 0x458040-0x458150.
**
**	The render state class's start-up and shut-down (WinMain's step 'initializing render state
**	class'): the critical section and the event its loader thread waits on. Notes at the end.
*/



#include <windows.h>

static CRITICAL_SECTION *Unknown_BDC094;			// 0xBDC094: the render states' lock  // NAME 1 [Unknown_BDC094]: neutral (D10)
static HANDLE Unknown_BDC098;						// 0xBDC098: the loader thread's event  // NAME 1 [Unknown_BDC098]: neutral (D10)

// 0x4581E0: the start-up (WinMain's 'initializing render state class')
bool Unknown_4581E0()								// NAME 1 [Unknown_4581E0]: as main_client.cpp
{
	if (Unknown_BDC094 == NULL) {
		Unknown_BDC094 = new CRITICAL_SECTION;
		InitializeCriticalSection(Unknown_BDC094);
	}
	if (Unknown_BDC098 == NULL) {
		Unknown_BDC098 = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	return(Unknown_BDC098 != NULL);
}

// 0x458250: the shut-down
void Unknown_458250()								// NAME 1 [Unknown_458250]: as main_client.cpp
{
	if (Unknown_BDC094 != NULL) {
		DeleteCriticalSection(Unknown_BDC094);
		delete Unknown_BDC094;
		Unknown_BDC094 = NULL;
	}
	if (Unknown_BDC098 != NULL) {
		CloseHandle(Unknown_BDC098);
		Unknown_BDC098 = NULL;
	}
}
/*
**	Reconstruction notes
**
**	client.exe: 0x4581E0 and 0x458250 exact (WinMain calls them around the display's start-up and
**	shut-down). The statics 0xBDC094 / 0xBDC098 are also used by 0x458040 (waits while bit 12 of a
**	render state's flags +0x28 is set), 0x4580D0 (starts 0x458150 with _beginthread unless bits 9 / 12
**	are set) and the thread 0x458150 (calls the state's virtual +0x10 under bit 12): RenderStateClass's
**	members (rs_track.h declares the class for now), so the two are probably its statics; free functions
**	until rs.h exists. The rest of rs.cpp's range (0x457BB0-0x458160, 0x4582C0-0x458350, the asserts
**	from 0x458370) is not rebuilt yet.
*/
