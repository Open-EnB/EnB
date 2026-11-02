// Earth & Beyond client (reconstruction): app\client\unknown_6cce60.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "fontdatabase.h"
#include "font32bit.h"
#include "unknown_fontsound.h"
#include "..\..\wwlib\wwstring.h"
#include <string.h>

// fontdatabase.cpp defines it in its own file; its layout restated here (data members only, as unknown_6ca0c0.cpp does).
struct FontColorStruct  // fontdatabase.cpp's (0x10 bytes)
{
	StringClass Name;														// +0x00
	TPoint3D<int> RGB;														// +0x04
};

extern Unknown_6BCFB0Class *Unknown_BE67B4;	// NAME 1 [Unknown_BE67B4]: neutral (D10): FontSys's sounds (main_client.cpp declares it so)
void Unknown_42ECF0(float *x, float *y);	// 0x42ECF0 (cdecl): the pointer's position, 0..1 (gadget.cpp, scriptedmovie.cpp declare it so)  // NAME 1 [x, y]: neutral
bool Unknown_42EC60(int button, int arg);	// 0x42EC60 (cdecl): a button down (gadget.cpp declares it so)  // NAME 1 [button, arg]: neutral

// 0x6CCE60: Update_Text's countdowns after the effects (+0x254 .. +0x268, one per frame): at zero each puts back what
// its effect changed (+0x488 the offset, +0x27C, +0x4A4 the colour from Colors[+0x44C], +0x284, +0x288, and +0x29C
// with the text rebuilt from Info.Text, "..." added while +0x406 says the line is clipped).
// NOT EXACT: functional (D64), not yet worked toward the client's bytes (0x6CCE60)
void FontStyleClass::Unknown_6CCE60()
{
	if (Unknown254[0] > 0 && --Unknown254[0] == 0) {
		Unknown488[0] = 0;
		Unknown488[1] = 0;
		Unknown488[2] = 0;
	}
	if (Unknown254[2] > 0 && --Unknown254[2] == 0) {
		Unknown274[2] = 0;
	}
	if (Unknown254[1] > 0 && --Unknown254[1] == 0) {
		FontColorStruct *color = Colors[Unknown44C];
		float *rgb = (float *)&Unknown4A4;	// floats r, g, b, a (fontsys.cpp's 0x6D5170)
		rgb[0] = (float)color->RGB.X * 0.00392156f;
		rgb[1] = (float)color->RGB.Y * 0.00392156f;
		rgb[2] = (float)color->RGB.Z * 0.00392156f;
	}
	if (Unknown254[3] > 0 && --Unknown254[3] == 0) {
		Unknown274[4] = 0;
	}
	if (Unknown254[4] > 0 && --Unknown254[4] == 0) {
		Unknown274[5] = 0;
	}
	if (Unknown254[5] > 0 && --Unknown254[5] == 0) {
		Unknown274[10] = 0;
		StringClass text = Info.Text;
		if (Unknown406) {
			text += "...";
		}
		*(int *)((char *)Font + 0xD28) = *(int *)((char *)Font + 0xD2C);	// Font->XSpacing = Font->MinSpacing (protected in font32bit.h)
		Unknown4C4 = Unknown2D0.Unknown_6D8DC0(&Font, text, 0);
		Unknown2C4 = Unknown420[1] = Font->String_Pixel_Width(text);
		Unknown2C8 = Unknown420[3] = Font->Get_Height();
	}
}

// 0x6CD180: true while the pointer is over the text (+0x250 1: inside the screen rectangle +0x430; 2: inside the text's
// box, its half size plus Info.BoxOffset, placed by Info.Justify); counts the frames in +0x290 and plays Info.SoundOver
// on the first.
// NOT EXACT: functional (D64), not yet worked toward the client's bytes (0x6CD180)
bool FontStyleClass::Unknown_6CD180()
{
	if (Unknown440 != 1 || Unknown250 == 0 || !Unknown4E4) {
		return false;
	}
	float sx = (float)Width * 0.0015625f;		// 1 / 640
	float sy = (float)Height * 0.0020833334f;	// 1 / 480
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	x = (float)Width * x - (float)(Width / 2);
	y = (float)(Height / 2) - (float)Height * y;
	if (Unknown250 == 1) {
		if (!((float)Unknown430[3] * sy < y) || !((float)Unknown430[2] * sy > y) ||
			!((float)Unknown430[0] * sx < x) || !((float)Unknown430[1] * sx > x)) {
			Unknown274[7] = 0;
			return false;
		}
	} else {
		int half_x = (int)((float)(Unknown2C4 / 2) + Info.BoxOffset[0]);
		int half_y = (int)((float)(Unknown2C8 / 2) + Info.BoxOffset[1]);
		int a = half_x;
		int b = half_y;
		if (Info.Rotate90) {
			a = half_y;
			b = half_x;
		}
		float px;
		float py;
		if (Unknown302) {
			px = Unknown464[0] + Unknown47C.X;
			py = Unknown464[1] + Unknown47C.Y;
		} else {
			Vector3 pos = Get_Position();
			px = pos.X + Unknown47C.X;
			py = Unknown47C.Y + pos.Y;
		}
		px *= sx;
		py *= sy;
		if (Info.Justify[0] == 1) {
			y = (float)b + y;
		} else if (Info.Justify[0] == 2) {
			y = y - (float)b;
		}
		if (Info.Justify[1] == 3) {
			x = x - (float)a;
		} else if (Info.Justify[1] == 4) {
			x = (float)a + x;
		}
		if (!(py - (float)b < y) || !(y < py + (float)b) || !(px - (float)a < x) || !(px + (float)a > x)) {
			Unknown274[7] = 0;
			return false;
		}
	}
	if (++Unknown274[7] != 1) {
		return true;
	}
	if (Unknown300) {
		Unknown_BE67B4->Play(Info.SoundOver.ID, Info.SoundOver.Vol, Info.SoundOver.Dur);
	}
	Unknown274[1] = 0;
	return true;
}

// 0x6CD6F0: true while the click effects run (+0x294 ms, from Info.TriggerTime): +0x250 2 starts them on button 1 down
// over the text (0x6CD180's +0x290), 1 while +0x301 is set; the start resets the timers and plays Info.SoundClick.
// NOT EXACT: functional (D64), not yet worked toward the client's bytes (0x6CD6F0)
bool FontStyleClass::Unknown_6CD6F0()
{
	if (Unknown440 != 1) {
		return false;
	}
	int mode = Unknown250;
	if (mode == 0) {
		return false;
	}
	if (mode == 2 && Unknown274[8] > 0) {
		return true;
	}
	if (mode == 1 && Unknown274[8] > 0) {
		if (Unknown301) {
			return true;
		}
		Unknown274[8] = 0;
		return false;
	}
	if (!(mode == 1 && Unknown301)) {
		if (mode != 2 || !Unknown_42EC60(1, 0) || Unknown274[7] <= 0) {
			return false;
		}
	}
	Unknown274[8] = (int)(Info.TriggerTime * 1000.0f);
	Unknown270 = 0;
	Unknown274[0] = 0;
	Unknown274[9] = 0;
	Unknown274[10] = 0;
	Unknown274[1] = 0;
	Unknown414 = 0;
	Unknown404 = false;
	memset(Unknown303, 0, sizeof(Unknown303));
	if (Unknown300) {
		Unknown_BE67B4->Play(Info.SoundClick.ID, Info.SoundClick.Vol, Info.SoundClick.Dur);
	}
	return true;
}

/*
**	Notes
**
**	Interim file (D137, TXT3): FontStyleClass's members 0x6CCE60-0x6CD900, right after the layout core 0x6CA0C0
**	(unknown_6ca0c0.cpp, TXT2) and probably the same source file (fontsys.cpp's constructors are at 0x6D47C0, so
**	the client's FontSys code spans two objects or one large one); no __FILE__ or string ties them yet.
**	The +0x254 .. +0x29C words are fontmanager.h's Unknown254[6] / Unknown274[16] (+0x290 = Unknown274[7] the
**	mouse-over frames, +0x294 = Unknown274[8] the click time left).
*/
