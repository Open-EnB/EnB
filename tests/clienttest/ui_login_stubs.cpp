// Earth & Beyond client (reconstruction)
// clienttest: the font manager global 0xBE67C4 (fontmanager.h's FontManagerClass *; loadingview.cpp, ui_login.cpp and
// ui_avsel.cpp reach it). An empty manager: its constructor would build the debug font through the font database
// (0xBE67B8, null here), so only the members are constructed; the loading view's Unknown_6D2310 then finds no font.
#include <new>
#include <string.h>
#include "..\..\..\app\client\fontmanager.h"

static FontManagerClass *Empty_Font_Manager()
{
	void *memory = operator new(sizeof(FontManagerClass));
	memset(memory, 0, sizeof(FontManagerClass));
	FontManagerClass *manager = (FontManagerClass *)memory;
	new (&manager->Fonts) std::hash_map<RenderObjClass *, ManagedFontClass *>;
	return manager;
}
FontManagerClass *Unknown_BE67C4 = Empty_Font_Manager();	// 0xBE67C4
