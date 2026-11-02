// Earth & Beyond client (reconstruction): app\Client\mousetbl.h, mousetbl.cpp's cursor table (header name PROVISIONAL, D137: the
// client has no string for it; mousetbl.cpp and smouse.cpp share the struct and the table).
#ifndef MOUSETBL_H
#define MOUSETBL_H

class TextureClass;
class RenderObjClass;

enum
{
	MCURSOR_MAX = 25											// the client's name ('idx < MCURSOR_MAX')
};

// One cursor: its texture and hot spot (smouse.cpp's software mouse reads them)
struct MouseCursorStruct  // NAME 1: neutral (D10)
{
	MouseCursorStruct(int idx, const char *name, float hot_x, float hot_y);			// 0x722770  // NAME 1 [name, hot_x, hot_y]: what it does with them
	MouseCursorStruct(int idx, RenderObjClass *robj, float hot_x, float hot_y);	// 0x7227F0  // NAME 1 [robj, hot_x, hot_y]: as above
	~MouseCursorStruct();														// 0x722890

	TextureClass *Texture;		// +0x00
	int HotX;					// +0x04: the hot spot, pixels  // NAME 1: neutral (D10)
	int HotY;					// +0x08  // NAME 1: neutral (D10)
};

extern MouseCursorStruct *Unknown_BE7C70[MCURSOR_MAX];	// 0xBE7C70: the table, by cursor number  // NAME 1 [Unknown_BE7C70]: neutral (D10)

void Unknown_7228C0();		// 0x7228C0: load the table (reference counted)  // NAME 1 [Unknown_7228C0]: neutral (D10)
void Unknown_723150();		// 0x723150: unload it  // NAME 1 [Unknown_723150]: neutral (D10)

#endif
