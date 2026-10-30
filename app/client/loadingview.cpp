#include "sclient.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\render2d.h"
#include "..\shared\auxdata\unknown_root.h"
#include "mss.h"
#include <string.h>

// fontsys.cpp's text object, as far as the loading text calls it
class FontSys : public RenderObjClass { public: void Change_Text(const char *text, int unknown); };	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)  // NAME 1 [text, unknown]: what 0x692420 passes
// fontmanager.h's FontManagerClass at 0xBE67C4 (its header includes fontdatabase.h, whose 0xBE67B8 clashes with tsk_game.h's): 0x6D2310 removes the fonts kept for a render object, 0x6D25D0 finds a named one
class LoadingViewFontManagerClass { public: void Unknown_6D2310(const RefCountPtr<RenderObjClass> &object); FontSys *Unknown_6D25D0(const char *name, RenderObjClass *object); }; extern LoadingViewFontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)  // NAME 1 [object, name]: what this file passes
// (fontdatabase.h's FontStringFilesClass at 0xBE67BC: sclient.h's SClientFontDatabaseClass)


// the 0xFC-byte render object at +0x64 (constructor 0x7B3E30), as far as this file calls it
class Unknown7B3E30Class : public RenderObjClass
{
};






















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

// 0x692160: SERVER_HANDOFF's screen with no names (SClient 0x74BA30, 0x7992F0)
void LoadingViewClass::Unknown_692160()
{
	std::string a, b, c, d;
	Unknown_691830(a, b, c, d);
}

// 0x692330: START 5: the loading screen's objects and fonts away, then Miles' mixing back
void LoadingViewClass::Unknown_692330()
{
	if (Unknown68) {
		if (Unknown64) {
			Unknown64->Remove();
		}
		Unknown58->Remove();
		Unknown_BE67C4->Unknown_6D2310(Unknown58);
		Unknown_BE67BC->Unknown_6C55B0("fc_map_load00.ini");
		Unknown58 = NULL;
		Unknown5C = NULL;
		Unknown60 = NULL;
		Unknown64 = NULL;
		Unknown68 = false;
	}
	AIL_set_preference(DIG_DS_MIX_FRAGMENT_CNT, 8);
	AIL_serve();
}

// 0x692420: slot +0x0C: 'Loading' with one to five dots, once a second; the base's render; once the avatar is
// incapacitated, the loading screen goes as START's does
int LoadingViewClass::Unknown_0C(bool on)
{
	static unsigned int last;	// 0xBE65D0  // NAME 1: what it holds
	static int dots = 5;		// 0xB90830  // NAME 1: what it holds
	unsigned int now = Unknown_Get_Ticks();
	WW3D::Sync(now);
	if (now - last > 1000) {
		FontSys *text = Unknown_BE67C4->Unknown_6D25D0("Loading_Text_map_load00", Unknown58.Peek());
		if (++dots > 5) {
			dots = 1;
		}
		char buffer[52];
		strcpy(buffer, "Loading");
		for (int i = 0; i < dots; i++) {
			strcat(buffer, ".");
		}
		text->Change_Text(buffer, 0);
		last = now;
	}
	int result = UIViewClass::Unknown_0C(on);
	((SClient *)Client->Unknown_18())->Text->Reset();
	if (((SClient *)Client->Unknown_18())->Unknown_750B60()) {
		if (Unknown68) {
			if (Unknown64) {
				Unknown64->Remove();
			}
			Unknown58->Remove();
			Unknown_BE67C4->Unknown_6D2310(Unknown58);
			Unknown_BE67BC->Unknown_6C55B0("fc_map_load00.ini");
			Unknown58 = NULL;
			Unknown5C = NULL;
			Unknown60 = NULL;
			Unknown64 = NULL;
			Unknown68 = false;
		}
		AIL_set_preference(DIG_DS_MIX_FRAGMENT_CNT, 8);
		AIL_serve();
	}
	return(result);
}

/*
**	Notes (LD4, 2026-10-29)
**
**	loadingview.cpp is the loading view (LoadingViewClass, sclient.h: 0x6C bytes over uiview.cpp's second view,
**	table 0xAFCF7C), client 0x6912A0-0x6927C0 (appfile.py show app\client\loadingview.cpp; the constructor's
**	G_ASSERT 'ctask' on the client's line 52, __FILE__ e:\build\beta\G\app\Client\loadingview.cpp).
**	+0x58..+0x60 are the asset manager's render objects 'map_load00', 'map_select', 'map_from' (0x691830; slot
**	+0x38 RenderObjClass::Remove), +0x64 a 0xFC-byte render object of its own (constructor 0x7B3E30, its own
**	RefCountPtr instances 0x692680 / 0x6926B0); the font manager 0xBE67C4 keeps fonts per render object (0x6D2310
**	takes the RefCountPtr, 0x6D25D0 the pointer). START 0x692330 and the slot +0x0C 0x692420 rebuilt (UI1).
**	Not rebuilt: 0x6912A0 / 0x691320 before the constructor (no callers: a helper over a render object, a
**	second object and a Vector3, its class unknown), SERVER_HANDOFF's 0x691830 (0x548-byte frame, a 0x6E1200
**	local; the asset loads and the fc_map_load00.INI fonts).
*/
