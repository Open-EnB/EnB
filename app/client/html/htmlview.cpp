// Earth & Beyond client (reconstruction): app\client\html\htmlview.cpp (PARTIAL). Notes at the end of the file.
#include "..\..\..\wwlib\surface.h"

class UnknownHtmlDrawClass  // NAME 1: the object 0x7767A0 draws its frames through; +0 its surface
{
public:
	void Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised);
	void Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);
	void Draw_Line_32(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);
	void Draw_Line_24(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);
	void Draw_Line_16(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);

	Surface *Surf;
};

void UnknownHtmlDrawClass::Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised)
{
	int light = 0xFFFFFF;
	int dark = 0;
	if (!raised) {
		light = 0;
		dark = 0xFFFFFF;
	}

	for (int i = 0; i < thickness; i++) {
		Draw_Line(x + i - 1, y + i, x + width - 1 - i - 1, y + i, clipx, clipy, clipw, cliph, light, 0x7F);
		Draw_Line(x + i, y + height - 1 - i, x + width - 1 - i, y + height - 1 - i, clipx, clipy, clipw, cliph, dark, 0x7F);
		Draw_Line(x + i, y + i, x + i, y + height - 1 - i, clipx, clipy, clipw, cliph, light, 0x7F);
		Draw_Line(x + width - 1 - i, y + i - 1, x + width - 1 - i, y + height - 1 - i - 1, clipx, clipy, clipw, cliph, dark, 0x7F);
	}
}

void UnknownHtmlDrawClass::Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha)
{
	if (Surf->Bytes_Per_Pixel() == 4) {
		Draw_Line_32(x1, y1, x2, y2, clipx, clipy, clipw, cliph, color, alpha);
	} else if (Surf->Bytes_Per_Pixel() == 3) {
		Draw_Line_24(x1, y1, x2, y2, clipx, clipy, clipw, cliph, color, alpha);
	} else if (Surf->Bytes_Per_Pixel() == 2) {
		Draw_Line_16(x1, y1, x2, y2, clipx, clipy, clipw, cliph, color, alpha);
	}
}

#pragma auto_inline(off)
void UnknownHtmlDrawClass::Draw_Line_24(int, int, int, int, int, int, int, int, int, int)
{
}
#pragma auto_inline(on)

/*
**	Reconstruction notes (HV1, 2026-11-01).
**	* The object (htmlview.cpp's __FILE__ 0xB9CA28: asserts in 0x770860 line 158, 0x7710D0 line 926 and
**	  0x7759E0 lines 1114 / 1115): its own functions from after image.cpp's (that object's vtable 0xB043F0
**	  holds 0x76E910 / 0x76E9D0 / 0x76E990) to 0x7778E0's line drawer, then its template copies
**	  0x778590-0x779120 (DynamicVectorClass instances called by 0x770860 / 0x7710D0); 0x7791F0 starts the
**	  next object, unknown_htmlattrib.cpp. Literals 0xB9C9A4-0xB9CD20 ('%s\%s' and 'Vertical_Padding' first
**	  used by 0x76E9D0 / 0x76F0F0, before the __FILE__ string) sit in this run of .rdata too.
**	* So far: the frame drawing the client calls from 0x7767A0, on an object whose +0 is a wwlib Surface
**	  (slots +0x34 Lock, +0x38 Unlock, +0x40 Bytes_Per_Pixel, +0x44 Stride): Draw_Bevel 0x777630 and
**	  Draw_Line 0x7777C0 exact, the empty 24-bit line 0x777EC0 exact at its marker (3 bytes; kept out of
**	  line by auto_inline(off), as the client calls it). Not yet: the 32-bit and 16-bit alpha line
**	  drawers 0x7778E0 / 0x777EE0 (alpha factors in the statics 0xC41550 / 0xC41554 and 0xC4153C /
**	  0xC4154C), the view and its parser. The class sits here until the view's header exists.
**	* The functions with asserts must sit on the client's lines (D25): everything above stays below line
**	  158 until 0x770860 is written; this partial file has none of them yet.
*/
