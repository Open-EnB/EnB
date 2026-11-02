// Earth & Beyond client (reconstruction): app\Client\mousetbl.cpp (__FILE__ 0xB94DD0). Notes at the end of the file.
#include "..\shared\unknown_debug.h"
#include "assetmgr.h"
#include "texture.h"
#include "rendobj.h"
#include "matinfo.h"
#include "..\shared\auxdata\permstr.h"
enum
{
	MCURSOR_MAX = 25											// the client's name ('idx < MCURSOR_MAX')
};

// One cursor: its texture and hot spot (smouse.cpp's software mouse reads them)
struct MouseCursorStruct  // NAME 1: neutral (D10), as smouse.cpp
{
	MouseCursorStruct(int idx, const char *name, float hot_x, float hot_y);			// NAME 1 [name, hot_x, hot_y]: what it does with them
	MouseCursorStruct(int idx, RenderObjClass *robj, float hot_x, float hot_y);	// NAME 1 [robj, hot_x, hot_y]: as above
	~MouseCursorStruct();

	TextureClass *Texture;		// +0x00
	int HotX;					// +0x04: the hot spot, pixels  // NAME 1: as smouse.cpp
	int HotY;					// +0x08  // NAME 1: as smouse.cpp
};

MouseCursorStruct *Unknown_BE7C70[MCURSOR_MAX];			// 0xBE7C70: the table, by cursor number  // NAME 1 [Unknown_BE7C70]: neutral (D10), as smouse.cpp
static int Unknown_BE7CD4;									// 0xBE7CD4: how many loads are open  // NAME 1 [Unknown_BE7CD4]: neutral (D10)

// 0x722770
MouseCursorStruct::MouseCursorStruct(int idx, const char *name, float hot_x, float hot_y)
{


	G_ASSERT(idx < MCURSOR_MAX);
	WW3DAssetManager *mgr = WW3DAssetManager::Get_Instance();
	Texture = mgr->Get_Texture(name, MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	HotX = hot_x;
	HotY = hot_y;
	Unknown_BE7C70[idx] = this;
}




























// 0x7227F0
MouseCursorStruct::MouseCursorStruct(int idx, RenderObjClass *robj, float hot_x, float hot_y)
{

	G_ASSERT(idx < MCURSOR_MAX);
	MaterialInfoClass *info = robj->Get_Material_Info();
	Texture = info->Get_Texture(0);
	info->Release_Ref();
	HotX = hot_x;
	HotY = hot_y;
	Unknown_BE7C70[idx] = this;
}

// 0x722890
MouseCursorStruct::~MouseCursorStruct()
{
	if (Texture != NULL) {
		Texture->Release_Ref();
	}
}

// 0x7228C0
void Unknown_7228C0()
{
	if (Unknown_BE7CD4 == 0) {
		new MouseCursorStruct(0, "arrow_blank.tga", 0, 0);
		new MouseCursorStruct(1, "arrow.tga", 0, 0);
		new MouseCursorStruct(2, "cursor_w.tga", 0, 0);
		new MouseCursorStruct(3, "cursor_y.tga", 32, 32);
		new MouseCursorStruct(4, "csr0000.tga", 64, 50);
		new MouseCursorStruct(5, "c_talk.tga", 3, 15);
		new MouseCursorStruct(6, "c_terminal.tga", 10, 0);
		new MouseCursorStruct(7, "c_exit.tga", 10, 0);
		new MouseCursorStruct(22, "c_zoomin.tga", 0, 0);
		new MouseCursorStruct(23, "c_zoomout.tga", 0, 0);
		new MouseCursorStruct(24, "c_handgrab.tga", 0, 0);
		new MouseCursorStruct(8, "arr_bl3.tga", 0, 32);
		new MouseCursorStruct(9, "arr_l3.tga", 0, 16);
		new MouseCursorStruct(10, "arr_tl3.tga", 0, 16);
		new MouseCursorStruct(11, "arr_t3.tga", 0, 16);
		new MouseCursorStruct(12, "arr_tr3.tga", 0, 16);
		new MouseCursorStruct(13, "arr_r3.tga", 0, 16);
		new MouseCursorStruct(14, "arr_br3.tga", 0, 32);
		new MouseCursorStruct(15, "arr_bl4.tga", 0, 32);
		new MouseCursorStruct(16, "arr_l4.tga", 0, 16);
		new MouseCursorStruct(17, "arr_tl4.tga", 0, 16);
		new MouseCursorStruct(18, "arr_t4.tga", 0, 16);
		new MouseCursorStruct(19, "arr_tr4.tga", 0, 16);
		new MouseCursorStruct(20, "arr_r4.tga", 0, 16);
		new MouseCursorStruct(21, "arr_br4.tga", 0, 32);
	}
	Unknown_BE7CD4++;
}

// 0x723150
void Unknown_723150()
{
	if (--Unknown_BE7CD4 == 0) {
		for (int i = 0; i < MCURSOR_MAX; i++) {
			if (Unknown_BE7C70[i] != NULL) {
				delete Unknown_BE7C70[i];
				Unknown_BE7C70[i] = NULL;
			}
		}
	}
}


/*
**	Reconstruction notes (2026-11-02, builder KRN): every function above is byte-exact with appfile.py match
**	(0x722770, 0x7227F0, 0x722890, 0x7228C0, 0x723150), the header's PermStringInit initialiser and its
**	atexit destructor (0x7231C0, 0x7231F0, the static at 0xBE7CEC) with them.
**
**	* The asserts ('idx < MCURSOR_MAX', the client's lines 33 and 72) name the index and the bound; the rest
**	  is neutral (D10). smouse.cpp declares the same struct and the table for its software mouse.
**	* The loader inlines the name constructor 23 times (/Ob2) and calls it for the last two cursors
**	  (0x722770); the asset manager in a local (`mgr`) is what makes VC6 stop at 23: written in one
**	  expression it inlines 24 and the registers differ. The hot spots go in as floats (ftol).
**	* 0x7227F0 (a cursor from a render object's material texture) and 0x722890 (the destructor out of
**	  line) have no reference in the client; the unloader inlines the destructor.
**	* The loader / unloader are reference counted (0xBE7CD4): LoginTaskClass's constructor / destructor
**	  and SClient::Load_INIs / Unload_INIs each call them.
**	* The object's start: 0x722720 / 0x722740 before it are a singly linked list's Is_Empty / Count
**	  (template code, placed by the linker), 0x722660 a list insert; not this file's.
**	* No header: the struct and the enum live here (interim, D137); smouse.cpp repeats the struct.
*/
