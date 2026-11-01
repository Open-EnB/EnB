// Earth & Beyond client (reconstruction): app\Client\font32bit.cpp. Notes at the end of the file.
#include "font32bit.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\surfaceclass.h"
#include "..\..\wwmath\vector2i.h"
#include <string.h>

class WW3D  // NAME 5: WW3D2's class (ww3d.h), declared here for the one call (as fontdatabase.cpp does)
{
public:
	static void Get_Device_Resolution(int &set_w, int &set_h, int &get_bits, bool &get_windowed);	// 0x9178F0
};


Font32Class::Font32Class(const char * font_name, int spacing, float scale) :
	XSpacing(spacing),
	MinSpacing(spacing),
	YSpacing(0),
	CharHeight(1),
	VHeight(1.0f),
	Scale(scale),
	ScreenWidth(640.0f),
	ScreenHeight(480.0f)
{
	memset(UOffsetTable, 0, sizeof(UOffsetTable));
	memset(VOffsetTable, 0, sizeof(VOffsetTable));
	memset(UWidthTable, 0, sizeof(UWidthTable));
	memset(CharWidthTable, 1, sizeof(CharWidthTable));
	Load_Font_Image(font_name);
	Set_XSpacing(MinSpacing);
}







Font32Class::Font32Class(void) :
	VHeight(1.0f),
	Scale(1.0f),
	ScreenWidth(640.0f),
	ScreenHeight(480.0f),
	CharHeight(1),
	YSpacing(0),
	MinSpacing(0)
{
	memset(UOffsetTable, 0, sizeof(UOffsetTable));
	memset(VOffsetTable, 0, sizeof(VOffsetTable));
	memset(UWidthTable, 0, sizeof(UWidthTable));
	memset(CharWidthTable, 0, sizeof(CharWidthTable));
	XSpacing = MinSpacing;
}

/*
**	Repacks the characters into a 128 or 256 square A4R4G4B4 image (font3d.cpp's, no caller in the client).
*/
// NOT EXACT: (D64) written from the pseudocode (font3d.cpp Minimize_Font_Image), no caller (0.87)
SurfaceClass * Font32Class::Minimize_Font_Image(SurfaceClass * surface)
{
	SurfaceClass::SurfaceDescription sd;
	surface->Get_Description(sd);
	float current_width = sd.Width;
	float current_height = sd.Height;
	int new_width;
	if (current_width < 256) {
		new_width = 128;
	} else {
		new_width = 256;
	}
	int new_height = new_width;
	RefCountPtr<SurfaceClass> new_surface = Create_NEW(new SurfaceClass(new_width, new_height, WW3D_FORMAT_A4R4G4B4));
	new_surface->Clear();
	float scale = (float)new_width;
	int new_x = 0;
	int new_y = 0;
	float ratio = current_width / scale;
	for (int char_index = 0; char_index < 256; char_index++) {
		int src_x = (int)(UOffsetTable[char_index] * current_width + 0.5f);
		int src_y = (int)(VOffsetTable[char_index] * current_height + 0.5f);
		int width = (int)(UWidthTable[char_index] * current_width + 0.5f);
		int height = (int)(VHeight * current_height + 0.5f);
		if (width != 0) {
			if (new_x + width > new_width) {
				new_x = 0;
				new_y += height;
			}
			new_surface->Copy(new_x, new_y, src_x, src_y, width, height, surface);
		}
		UOffsetTable[char_index] = (float)new_x / scale;
		VOffsetTable[char_index] = (float)new_y / scale;
		UWidthTable[char_index] = ratio * UWidthTable[char_index];
		new_x += width;
	}
	VHeight = (current_height / scale) * VHeight;
	REF_PTR_RELEASE(surface);
	Surface = new_surface;
	return(NULL);
}

/*
**	Brings each character's cell in to its pixels (font3d.cpp's Make_Proportional): U / width tables,
**	CharWidthTable, then the common top and the height of all characters.
*/
// NOT EXACT: (D64) written from the pseudocode (font3d.cpp Make_Proportional with a fixed width for empty cells, 0.21)
void Font32Class::Make_Proportional(SurfaceClass * surface)
{
	SurfaceClass::SurfaceDescription sd;
	surface->Get_Description(sd);
	float width = sd.Width;
	float height = sd.Height;
	int fixed = 0;
	int top = height;
	int bottom = 0;
	for (int char_index = 0; char_index < 256; char_index++) {
		int x0 = (int)(UOffsetTable[char_index] * width + 0.5);
		int y0 = (int)(VOffsetTable[char_index] * height + 0.5);
		Vector2i minb(x0, y0);
		Vector2i maxb(x0 + (int)(UWidthTable[char_index] * width + 0.5), y0 + (int)(VHeight * height + 0.5));
		surface->FindBB(&minb, &maxb);
		int left = minb.I;
		int right = maxb.I + 1;
		if (fixed == 0 && left < right) {
			fixed = right - left;
			if (MinSpacing + fixed < 1) {
				fixed = 1 - MinSpacing;
			}
		}
		if (right <= left) {
			left = x0;
			if (fixed == 0) {
				right = x0 + 1;
				if (MinSpacing < 0) {
					right -= MinSpacing;
				}
			} else {
				right = x0 + fixed;
			}
		}
		if (minb.J - y0 < top) {
			top = minb.J - y0;
		}
		if (maxb.J - y0 > bottom) {
			bottom = maxb.J - y0;
		}
		UOffsetTable[char_index] = (float)left / (float)sd.Width;
		UWidthTable[char_index] = (float)(right - left) / (float)sd.Width;
		CharWidthTable[char_index] = right - left;
	}
	if (top != 0) {
		for (int index = 0; index < 256; index++) {
			VOffsetTable[index] += (float)top / (float)sd.Height;
		}
	}
	CharHeight = bottom - top + 1;
	VHeight = (float)CharHeight / (float)sd.Height;
}







































































































































































/*
**	Loads the font image (font3d.cpp's Load_Font_Image): a strike (one row of characters) or a 16 x 16 grid
**	made proportional; the image becomes A4R4G4B4.
*/

bool Font32Class::Load_Font_Image(const char * filename)
{
	Update_Resolution();

	RefCountPtr<SurfaceClass> surface = Create_NEW(new SurfaceClass(filename));
	G_ASSERT(surface);

	SurfaceClass::SurfaceDescription sd;
	surface->Get_Description(sd);
	if (sd.Format != WW3D_FORMAT_A4R4G4B4) {
		RefCountPtr<SurfaceClass> converted = Create_NEW(new SurfaceClass(sd.Width, sd.Height, WW3D_FORMAT_A4R4G4B4));
		converted->Copy(0, 0, 0, 0, sd.Width, sd.Height, surface.Peek());
		surface = converted;
	}

	if (sd.Width > 8 * sd.Height) {
		VHeight = 1.0f;
		CharHeight = sd.Height;
		int column = 0;
		int width = sd.Width;
		for (int char_index = 0; char_index < 256; char_index++) {
			if (char_index < 0x21) {
				UOffsetTable[char_index] = 0;
				VOffsetTable[char_index] = 0;
				UWidthTable[char_index] = 0;
				CharWidthTable[char_index] = 0;
			} else {
				while (column < width && surface->Is_Transparent_Column(column)) column++;
				int start = column;
				while (column < width && !surface->Is_Transparent_Column(column)) column++;
				int end = column;
				UOffsetTable[char_index] = (float)start / width;
				VOffsetTable[char_index] = 0;
				UWidthTable[char_index] = (float)(end - start) / width;
				CharWidthTable[char_index] = end - start;
			}
		}
		Surface = surface;
		surface = NULL;
	} else {
		float font_width = sd.Width;
		float font_height = sd.Height;
		int char_index = 0;
		float mono_pixel_width = font_width * 0.0625f;
		float mono_pixel_height = font_height * 0.0625f;
		float u_width = mono_pixel_width / font_width;
		unsigned char char_width = (unsigned char)mono_pixel_width;
		for (; char_index < 256; char_index++) {
			UOffsetTable[char_index] = (float)((char_index % 16) * mono_pixel_width) / font_width;
			VOffsetTable[char_index] = (float)((char_index / 16) * mono_pixel_height) / font_height;
			UWidthTable[char_index] = u_width;
			CharWidthTable[char_index] = char_width;
		}
		VHeight = mono_pixel_height / font_height;
		CharHeight = (unsigned char)mono_pixel_height;
		Surface = surface;
		surface = NULL;
		Make_Proportional(Surface.Peek());
	}
	return(true);
}








































/*
**	Copies str's characters from the font image into dest at y 0, each followed by XSpacing columns of the
**	space character's cell (0x6BC950, FontSys 0x6D8DC0).
*/

void Font32Class::Print_To_Surface(char const * str, SurfaceClass * dest)
{
	int x = 0;
	FAIL_IF(!str) return;

	int index = 0;
	SurfaceClass::SurfaceDescription sd;
	Surface->Get_Description(sd);
	int width = sd.Width;
	int height = sd.Height;
	int space_x = (int)(width * UOffsetTable[' '] + 0.5f);
	int space_y = (int)(height * VOffsetTable[' '] + 0.5f);
	int char_height = CharHeight;
	if (*str == 0) return;
	do {
		unsigned char c = str[index];
		int src_x = (int)(width * UOffsetTable[c] + 0.5f);
		int src_y = (int)(height * VOffsetTable[c] + 0.5f);
		int char_width = (int)(width * UWidthTable[c] + 0.5f);
		if (c == ' ' && index > 0 && XSpacing < 0) {
			dest->Copy(x - XSpacing, 0, src_x, src_y, char_width + XSpacing, char_height, Surface.Peek());
		} else {
			dest->Copy(x, 0, src_x, src_y, char_width, char_height, Surface.Peek());
		}
		x += char_width;
		int left = XSpacing;
		if (left > 0) {
			int done = 0;
			do {
				int count = (left > 4) ? 4 : left;
				dest->Copy(x + done, 0, space_x, space_y, count, char_height, Surface.Peek());
				done += count;
				left -= count;
			} while (left > 0);
		}
		x += XSpacing;
		index++;
	} while (str[index] != 0);
}











int Font32Class::String_Pixel_Width(char const * string) const
{
	FAIL_IF(!string) return(0);

	int width = 0;
	for (; *string != 0; string++) {
		width += CharWidthTable[(unsigned char)*string] + XSpacing;
	}
	if (XSpacing < 0) {
		width -= XSpacing;
	}
	return(width);
}


int Font32Class::Char_Pixel_Width(char c) const
{
	return(CharWidthTable[(unsigned char)c] + XSpacing);
}

// NOT EXACT: (D64) a prefix of the client's 81 bytes: the static's guard and atexit differ
Point2D Font32Class::Print(char const *, ::Surface &, Rect const &, Point2D const &, ConvertClass const &, unsigned char const *) const
{
	static Point2D _point;
	return(_point);
}


float Font32Class::Get_Text_Width(const char * text, float screen_width)
{
	return((screen_width / ScreenWidth) * (String_Pixel_Width(text) * Scale));
}


float Font32Class::Get_Char_Width(char c, float screen_width)
{
	return((screen_width / ScreenWidth) * (Char_Pixel_Width(c) * Scale));
}


float Font32Class::Get_Char_Height(float screen_height)
{
	return((screen_height / ScreenHeight) * (Get_Height() * Scale));
}


float Font32Class::Get_Line_Height(float screen_height)
{
	return((screen_height / ScreenHeight) * (Get_Line_Spacing() * Scale));
}




















void Font32Class::Update_Resolution(void)
{
	int resw = 0;
	int resh = 0;
	int bits = 0;
	bool windowed = false;
	WW3D::Get_Device_Resolution(resw, resh, bits, windowed);
	G_ASSERT(resw);
	G_ASSERT(resh);
	ScreenWidth = resw ? (float)resw : 1.0f;
	ScreenHeight = resh ? (float)resh : 1.0f;
}


int Font32Class::Set_XSpacing(int x)
{
	if (x < MinSpacing) {
		return(XSpacing);
	}
	XSpacing = x;
	return(x);
}


int Font32Class::Set_YSpacing(int y)
{
	int min = -(Get_Height() / 2);
	if (y < min) {
		YSpacing = min;
		return(min);
	}
	YSpacing = y;
	return(y);
}

/*
**	Reconstruction notes (font32bit.cpp)
**
**	The client's object 0x6BBAC0-0x6BCFA0 (__FILE__ e:\build\beta\G\app\Client\font32bit.cpp: FAIL_IF l.439 / 488,
**	G_ASSERT l.335 / 563 / 564), before unknown_fontsound.cpp (0x6BCFB0). Font32Class is Renegade's
**	Font3DDataClass (ww3d2/font3d.cpp: the U / V / width tables, Make_Proportional, Load_Font_Image,
**	Minimize_Font_Image) made a FontClass with spacing and a screen scale. Functional (D64) first, FNT1 2026-11-01.
*/
