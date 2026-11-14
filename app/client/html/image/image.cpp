// Earth & Beyond client (reconstruction): app\client\html\image\image.cpp. Notes at the end of the file.
#include "image.h"
#include "..\htmlview.h"
#include "..\..\..\shared\g_assert.h"
#include <stdio.h>
#include <math.h>

HtmlImageClass::HtmlImageClass(char *name, int bits) : Surf(NULL), Bits(bits)
{
	strcpy(Name, name);
}

HtmlImageClass::~HtmlImageClass(void)
{
	if (Surf != NULL) {
		delete Surf;
		Surf = NULL;
	}
}

int HtmlImageClass::Set_Data(char *data, int length)
{
	G_ASSERT(0);
	return(0);
}

// NOT EXACT: 0x76E610, functional (D64): the client builds dest at x, y and subtracts the clip origin in place after
// the second Get_Width / Get_Height pair (frame order); no form tried matches (see attempts.csv)
bool HtmlImageClass::Draw(Surface *surface, int x, int y, int clipx, int clipy, int clipw, int cliph)
{
	if (Surf == NULL)
		return(false);

	Rect dest(x - clipx, y - clipy, Surf->Get_Width(), Surf->Get_Height());
	Rect source(0, 0, Surf->Get_Width(), Surf->Get_Height());
	Rect sclip = source;
	surface->Blit_From(Rect(clipx, clipy, clipw, cliph), dest, *Surf, sclip, source, false);
	return(true);
}
































bool HtmlImageClass::Create_Surface(int width, int height)
{
	if (Surf != NULL) {
		delete Surf;
		Surf = NULL;
	}

	DDPIXELFORMAT format;
	format.dwSize = sizeof(format);
	format.dwFlags = DDPF_RGB;
	if (Bits == 32) {
		format.dwRGBBitCount = 32;
		format.dwRBitMask = 0xFF0000;
		format.dwGBitMask = 0xFF00;
		format.dwBBitMask = 0xFF;
		format.dwRGBAlphaBitMask = 0;
	} else if (Bits == 24) {
		format.dwRGBBitCount = 24;
		format.dwRBitMask = 0xFF0000;
		format.dwGBitMask = 0xFF00;
		format.dwBBitMask = 0xFF;
		format.dwRGBAlphaBitMask = 0;
	} else if (Bits == 16) {
		format.dwRGBBitCount = 16;
		format.dwRBitMask = 0x7C00;
		format.dwGBitMask = 0x3E0;
		format.dwBBitMask = 0x1F;
	}




	Surf = new DSurface(width, height, true, &format);
	G_ASSERT(Surf);
	G_ASSERT(Surf->In_Video_Ram()==false);
	return(true);
}

HtmlFileLoaderClass::HtmlFileLoaderClass(HtmlViewClass *view, char *path) :
	View(view),
	Loaded(false)
{
	strcpy(Path, path);
}

HtmlFileLoaderClass::~HtmlFileLoaderClass(void)
{
}

bool HtmlFileLoaderClass::Update(void)
{
	if (Loaded) {
		View->Add_Data(NULL, NULL, NULL, 0, true);
		Loaded = false;
	}
	return(false);
}

bool HtmlFileLoaderClass::Open(char *url)
{
	char filename[256];
	char buffer[256];

	sprintf(filename, "%s\\%s", Path, url);
	for (int i = 0; i < (int)strlen(filename); i++) {
		if (filename[i] == '/')
			filename[i] = '\\';
	}

	FILE *file = fopen(filename, "rb");
	if (file == NULL)
		return(false);

	int count = fread(buffer, 1, sizeof(buffer), file);
	while (count != 0) {
		View->Add_Data(url, Get_Mime_Type(url), buffer, count, false);
		count = fread(buffer, 1, sizeof(buffer), file);
	}
	Loaded = true;
	fclose(file);
	return(true);
}

static ArrayList<HtmlFontStruct> FontCache;

// NOT EXACT: 0x76EB70, functional (D64): frame 0x118 vs 0x110 (the client keeps add()'s pos in the POINT's slot and
// does not fold its pos > Entries_ test)
HFONT Get_Font(HDC hdc, char *face, int width, int height, int flags)
{
	HtmlFontStruct entry;
	for (int i = 0; i < FontCache.length(); i++) {
		FontCache.get(entry, i);
		if (strcmp(entry.Face, face) == 0 && entry.Width == width && entry.Height == height && entry.Flags == flags)
			return(entry.Font);
	}

	if (hdc == NULL)
		return(NULL);

	SaveDC(hdc);
	SetGraphicsMode(hdc, GM_ADVANCED);
	ModifyWorldTransform(hdc, NULL, MWT_IDENTITY);
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowOrgEx(hdc, 0, 0, NULL);

	POINT pt;
	pt.x = width;
	pt.y = height;
	DPtoLP(hdc, &pt, 1);

	LOGFONT lf;
	lf.lfHeight = -pt.y;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = (flags & 1) ? 700 : 0;
	lf.lfItalic = (flags & 2) ? 1 : 0;
	lf.lfUnderline = (flags & 4) ? 1 : 0;
	lf.lfStrikeOut = (flags & 8) ? 1 : 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	strcpy(lf.lfFaceName, face);

	HFONT font = CreateFontIndirect(&lf);
	if (width != 0) {
		TEXTMETRIC tm;
		font = (HFONT)SelectObject(hdc, font);
		GetTextMetrics(hdc, &tm);
		DeleteObject(SelectObject(hdc, font));
		lf.lfWidth = (int)(tm.tmAveCharWidth * fabs(pt.x) / fabs(pt.y) + 0.5);
		font = CreateFontIndirect(&lf);
	}
	RestoreDC(hdc, -1);

	if (font == NULL)
		return(NULL);

	strcpy(entry.Face, face);
	entry.Width = width;
	entry.Height = height;
	entry.Flags = flags;
	entry.Font = font;
	FontCache.add(entry, FontCache.length());
	return(font);
}

/*
** Notes (reconstruction).
**
**	* The file: its __FILE__ 'e:\build\beta\G\app\Client\html\image\image.cpp' (0xB9C93C) in the asserts of
**	  Set_Data (G_ASSERT(0), line 23) and Create_Surface ('Surf' 105, 'Surf->In_Video_Ram()==false' 106). Its object
**	  0x76E4F0-0x76F0F0: before it gifload.cpp's Out_Line 0x76E3D0, after it unknown_htmlblock.cpp's HtmlBlockClass
**	  constructor 0x76F0F0. Its literal '%s\\%s' (0xB9C9A4) follows the assert strings with no other __FILE__
**	  between, and Get_Font's cache (0xBE82E0) follows gifload.cpp's bss (0xBE82DC): so the loader and the font
**	  cache are taken as this file's (provisional: no assert of theirs names it).
**	* HtmlImageClass's Get_Surface / Get_Name are inline: the copies kept are gifload.cpp's (0x76D410 / 0x76D430,
**	  first in link order); both vtables (0xB043B8 the gif's, 0xB043D4 this one) point at them.
**	* HtmlFileLoaderClass's destructor 0x76E940 is out of line (its deleting destructor 0x76E910 calls it) and
**	  only stores the base vtable; the base's is inline (its deleting destructor 0x76E960 holds the store).
**	* Get_Font is Petzold's EzCreateFont without the dpi scaling (width and height go straight to DPtoLP), with
**	  lfCharSet 0 and strcpy, and a static ArrayList cache in front (its dynamic initializer 0x76EB10, the
**	  atexit destructor 0x76EB40).
*/
