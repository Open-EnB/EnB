// Earth & Beyond client (reconstruction): app\client\unknown_514cc0.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include <string>
#include "sclient.h"
#include "uiview.h"
#include "unknown_5b7ab0.h"

// 0x514CC0: the view at SpaceClientClass +0x1358 (sclient.h): the base with 1
MenuViewClass::MenuViewClass(ClientTaskClass *task) :
	UIViewClass(task, 1)
{
}

// 0x514DE0
MenuViewClass::~MenuViewClass()
{
	REF_PTR_RELEASE(Unknown4C);
}

// 0x514E70: slot +0x08: its interface object over a layer of the base's scene and camera
void MenuViewClass::Unknown_08()
{
	Unknown4C = new Unknown5B7AB0Class(this, LayerClass(Unknown1C, Unknown20.Peek()));
	Unknown4C->Unknown_54();
	Unknown_6B4940(Unknown4C);
}

/*
**	Notes
**
**	RECONSTRUCTED (S2, run 2026-11-18): client 0x514CC0-0x514F80, an object of its own by the CRT initialiser table:
**	mainview.cpp's object ends with its initialisers 0x514B30 (entry 148), <string>'s guard batched with the Element
**	array's numbering 0x514C40 (149) and 0x514C90 (150); this object's only initialiser is <string>'s plain guard
**	0x514F60 (151). It holds MenuViewClass (sclient.h; the view at SpaceClientClass +0x1358), moved out of the end of
**	mainview.cpp. No assert or string names the file: PROVISIONAL (D137).
*/
