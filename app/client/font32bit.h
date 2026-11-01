// Earth & Beyond client (reconstruction): app\client\font32bit.h (D24: paired with the client's font32bit.cpp).
// Font32Class: a wwlib FontClass over a 4-bit-alpha SurfaceClass font image, Renegade's Font3DDataClass tables
// (font3d.cpp) with the spacing and the screen scale; fontdatabase.cpp's fonts (font32Ptr).
#ifndef FONT32BIT_H
#define FONT32BIT_H

#include "..\..\wwlib\font.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"

class SurfaceClass;

class Font32Class : public FontClass, public RefCountClass							// NAME 2: what font32Ptr points at (0xD30 bytes; vtables 0xAFE94C / 0xAFE940)
{
	public:
		Font32Class(void);															// 0x6BBD50, no caller
		Font32Class(const char * font_name, int spacing, float scale);				// 0x6BBAC0
		virtual ~Font32Class(void);													// 0x6BBCC0 (deleting 0x6BBC90)

		virtual int Char_Pixel_Width(char c) const;									// 0x6BCC40
		virtual int String_Pixel_Width(char const * string) const;					// 0x6BCBB0
		virtual int Get_Width(void) const { return(CharWidthTable['H']); }			// 0x6BBC20
		virtual int Get_Height(void) const { return(CharHeight); }					// 0x6BBC40
		virtual Point2D Print(char const * string, ::Surface & surface, Rect const & cliprect, Point2D const & point, ConvertClass const & converter, unsigned char const * remap = NULL) const;	// 0x6BCC70 (does nothing)
		virtual int Set_XSpacing(int x);											// 0x6BCED0
		virtual int Set_YSpacing(int y);											// 0x6BCF00

		virtual int Get_Line_Spacing(void) const { return(CharHeight + YSpacing); }	// 0x6BBC60  // NAME 2 [Get_Line_Spacing]: the height plus YSpacing
		virtual float Get_Text_Width(const char * text, float screen_width);		// 0x6BCCE0  // NAME 2: String_Pixel_Width scaled to the screen width
		virtual float Get_Char_Width(char c, float screen_width);					// 0x6BCD20  // NAME 2: Char_Pixel_Width, scaled
		virtual float Get_Char_Height(float screen_height);							// 0x6BCD60  // NAME 2 [Get_Char_Height]: Get_Height scaled to the screen height (not Get_Height: VC6 groups the overload next to it)
		virtual float Get_Line_Height(float screen_height);						// 0x6BCDA0  // NAME 2: Get_Line_Spacing, scaled

		SurfaceClass * Peek_Surface(void) { return(Surface.Peek()); }						// 0x6BC930  // NAME 2 [Peek_Surface]: Westwood's Peek_ style
		void Print_To_Surface(char const * str, SurfaceClass * dest);				// 0x6BC950 (FontSys 0x6D8DC0)  // NAME 2 [Print_To_Surface]: copies each character's cells to dest
		void Update_Resolution(void);												// 0x6BCDE0, inlined into Load_Font_Image  // NAME 2 [Update_Resolution]: ScreenWidth / ScreenHeight from WW3D

	protected:
		SurfaceClass * Minimize_Font_Image(SurfaceClass * surface);				// 0x6BBE20, no caller (font3d.cpp's name, NAME 4)
		void Make_Proportional(SurfaceClass * surface);							// 0x6BC120 (font3d.cpp's name, NAME 4)
		bool Load_Font_Image(const char * filename);								// 0x6BC3A0 (font3d.cpp's name, NAME 4)

		RefCountPtr<SurfaceClass> Surface;											// +0x0C  // NAME 2: the font image
		float UOffsetTable[256];													// +0x10 (font3d.cpp's names)
		float VOffsetTable[256];													// +0x410
		float UWidthTable[256];														// +0x810
		float VHeight;																// +0xC10
		float Scale;																// +0xC14  // NAME 2: Scale%d
		float ScreenWidth;															// +0xC18  // NAME 2: 640 until Update_Resolution
		float ScreenHeight;															// +0xC1C  // NAME 2: 480 until Update_Resolution
		unsigned char CharWidthTable[256];											// +0xC20
		unsigned char CharHeight;													// +0xD20
		int YSpacing;																// +0xD24  // NAME 2: Set_YSpacing's
		int XSpacing;																// +0xD28  // NAME 2: Set_XSpacing's
		int MinSpacing;																// +0xD2C  // NAME 2: Spacing%d, Set_XSpacing's lower bound
};

#endif
