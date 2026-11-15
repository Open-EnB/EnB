// Earth & Beyond client (reconstruction): app\common\pal_mgr.cpp (client __FILE__ e:\build\beta\G\app\Common\pal_mgr.cpp). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <vector>
#include "pal_mgr.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\targa.h"
#include "..\shared\g_assert.h"

struct Unknown848450Struct {	// NAME 1: neutral (D10): palette image width, height and BGR pixel buffer
	int Width;					// NAME 2: what it holds
	int Height;					// NAME 2: what it holds
	unsigned char *Data;		// NAME 2: what it holds

	Unknown848450Struct(int width, int height) : Width(width), Height(height), Data(NULL)
	{
		Data = new unsigned char[Width * Height * 3];
	}
};

std::vector<Unknown848450Struct *> Public_Palettes;
std::vector<Unknown848450Struct *> Secret_Palettes;

static void Unknown_848450(const char *filename, std::vector<Unknown848450Struct *> &palettes);	// NAME 1: neutral (D10)

void Unknown_848250(void)
{
	Public_Palettes.clear();
	Secret_Palettes.clear();

	INIClass public_ini("palette_public.ini");
	INIClass secret_ini("palette_secret.ini");

	char section[16];
	char image_name[32];

	int i = 0;
	while (true) {
		sprintf(section, "PAL%02d", i);
		if (!public_ini.Find_Section(section)) {
			break;
		}
		public_ini.Get_String(section, "Image", "", image_name, sizeof(image_name));
		Unknown_848450(image_name, Public_Palettes);
		i++;
	}

	i = 0;
	while (true) {
		sprintf(section, "PAL%02d", i);
		if (!secret_ini.Find_Section(section)) {
			break;
		}
		secret_ini.Get_String(section, "Image", "", image_name, sizeof(image_name));
		Unknown_848450(image_name, Secret_Palettes);
		i++;
	}
}





static void Unknown_848450(const char *filename, std::vector<Unknown848450Struct *> &palettes)
{
	Targa targa;

	FAIL_IF(TARGA_ERROR_HANDLER(targa.Open(filename, TGA_READMODE),filename)) return;
	FAIL_IF(TARGA_ERROR_HANDLER(targa.Load(filename, TGAF_IMAGE, false),filename)) return;
	FAIL_IF(targa.Header.PixelDepth != 24) return;
	targa.YFlip();
	Unknown848450Struct *image = new Unknown848450Struct(targa.Header.Width, targa.Header.Height);

	FAIL_IF(!image) return;
	memcpy(image->Data, targa.GetImage(), targa.Header.Width * targa.Header.Height * 3);
	palettes.push_back(image);
}

void Unknown_848810(void)
{
	Public_Palettes.clear();
	Secret_Palettes.clear();
}

static inline void Sample_Palette(Unknown848450Struct *pal, float u, float v, float *rgb)	// NAME 2: what it does
{
	int x = (int)(pal->Width * u);
	int y = (int)(pal->Height * v);
	int max_x = pal->Width - 1;
	if (x < 0) {
		x = 0;
	} else if (x > max_x) {
		x = max_x;
	}
	int max_y = pal->Height - 1;
	if (y < 0) {
		y = 0;
	} else if (y > max_y) {
		y = max_y;
	}
	int offset = (y * pal->Width + x) * 3;
	float b = pal->Data[offset];
	float g = pal->Data[offset + 1];
	float r = pal->Data[offset + 2];
	rgb[0] = r * (1.0f / 255.0f);
	rgb[1] = g * (1.0f / 255.0f);
	rgb[2] = b * (1.0f / 255.0f);
}



















void Unknown_8488A0(int palette, float u, float v, float *rgb)
{
	FAIL_IF(!rgb) return;
	FAIL_IF(palette < 0) return;
	FAIL_IF(palette > (int) Public_Palettes.size()) return;
	Sample_Palette(Public_Palettes[palette], u, v, rgb);
}


void Unknown_848A10(int palette, float u, float v, float *rgb)
{
	FAIL_IF(!rgb) return;
	FAIL_IF(palette < 0) return;
	FAIL_IF(palette > (int) Secret_Palettes.size()) return;
	Sample_Palette(Secret_Palettes[palette], u, v, rgb);
}

/*
**	Reconstruction notes:
**	- Client object: e:\build\beta\G\app\Common\pal_mgr.cpp (0x848130-0x848C61), after
**	  unknown_847d20.cpp's <string> ctype guard (0x848100) and before prof.cpp (0x848C80-0x848FA0).
**	- 0x848130 / 0x8481C0: Public_Palettes / Secret_Palettes static initialisers (.CRT$XCU).
**	- 0x848250: Unknown_848250 (reads palette_public.ini and palette_secret.ini PAL%02d Image entries).
**	- 0x848450: Unknown_848450 (static helper; asserts on lines 68, 69, 70, 74).
**	- 0x848810: Unknown_848810 (clears Public_Palettes and Secret_Palettes).
**	- 0x8488A0 / 0x848A10: Unknown_8488A0 / Unknown_848A10 (sample RGB from Public_Palettes /
**	  Secret_Palettes; asserts on lines 130-132, 139-141).
**	- 0x848B80, 0x848BB0, 0x848BD0, 0x848C10, 0x848C50: std::vector<Unknown848450Struct *>
**	  instantiations (size, _Destroy, _Ucopy, _Ufill, std::_Construct).
*/
