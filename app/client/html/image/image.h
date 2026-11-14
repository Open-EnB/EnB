// Earth & Beyond client (reconstruction): app\client\html\image\image.h. Notes at the end of image.cpp.
#ifndef IMAGE_H
#define IMAGE_H

#include "..\..\..\..\wwlib\dsurface.h"

class HtmlViewClass;

// An image of an html page: its surface and the name the page links it by. vtable 0xB043D4 (6 slots); the
// gif image (gifload.h) is the only kind the view makes.
class HtmlImageClass  // NAME 1: element of the view's +0x20 list (image.cpp's class)
{
public:
	HtmlImageClass(char *name, int bits);  // 0x76E4F0  // NAME 2: the name copied to +0xC, bits per pixel kept
	virtual ~HtmlImageClass(void);  // 0x76E5B0

	virtual int Set_Data(char *data, int length);  // slot +4, 0x76E5E0  // NAME 2: the parser hands it the data of the link named like the image; here G_ASSERT(0) (line 23)
	virtual Surface *Get_Surface(void) {return(Surf);}  // slot +8 (the copy kept is 0x76D410)  // NAME 2: Draw_Block tiles it (Get_Width / Get_Height)
	virtual char *Get_Name(void) {return(Name);}  // slot +0xC (the copy kept is 0x76D430)  // NAME 2: Find_Image's key
	virtual bool Draw(Surface *surface, int x, int y, int clipx, int clipy, int clipw, int cliph);  // slot +0x10, 0x76E610  // NAME 2: Blit_From of Surf at x, y clipped to the rectangle
	virtual bool Create_Surface(int width, int height);  // slot +0x14, 0x76E730  // NAME 2: a system-memory DSurface of Bits (asserts 'Surf', 'Surf->In_Video_Ram()==false')

	DSurface *Surf;  // +0x004  // NAME 2: the asserts' 'Surf'
	int Bits;  // +0x008  // NAME 2: the view passes Bytes_Per_Pixel() << 3; Create_Surface's 32 / 24 / 16
	char Name[256];  // +0x00C  // NAME 2: Get_Name; 256: the gif's fields start at +0x10C
};

// What the view hands a url to: Open is given the view's url, Update is polled by the view's Update.
// vtable 0xB04400 (its destructor 0x76E960, then two _purecall slots).
class HtmlLoaderClass  // NAME 1: the object at the view's +4 (slots +4 / +8 called)
{
public:
	virtual ~HtmlLoaderClass(void) {}  // slot +0 (the deleting destructor 0x76E960)
	virtual bool Open(char *url) = 0;  // slot +4  // NAME 2: given the view's url by Open
	virtual bool Update(void) = 0;  // slot +8  // NAME 2: polled by the view's Update
};

// The loader that reads a url as a file of a directory: vtable 0xB043F0. No caller of its constructor is
// known in client.exe (the view's Set_Loader is given one from outside the html files).
class HtmlFileLoaderClass : public HtmlLoaderClass  // NAME 1: sprintf('%s\\%s', Path, url) and fopen 'rb'
{
public:
	HtmlFileLoaderClass(HtmlViewClass *view, char *path);  // 0x76E8A0
	virtual ~HtmlFileLoaderClass(void);  // 0x76E940 (its deleting destructor 0x76E910)

	virtual bool Open(char *url);  // slot +4, 0x76E9D0  // NAME 2: the file's data to the view's Add_Data, 256 bytes at a time
	virtual bool Update(void);  // slot +8, 0x76E990  // NAME 2: after an Open, the view's Add_Data(NULL, ..., true) (Layout and Render)

	HtmlViewClass *View;  // +0x004  // NAME 2: Add_Data's this
	bool Loaded;  // +0x008  // NAME 2: set by Open, cleared by Update
	char Path[256];  // +0x009  // NAME 2: the directory sprintf puts before the url (size not settled: nothing after it)
};

// An entry of Get_Font's cache: 0x90 bytes (the ArrayList moves 0x24 dwords).
struct HtmlFontStruct  // NAME 1: Get_Font's cache entry
{
	char Face[128];  // +0x00  // NAME 2: compared with Get_Font's face
	int Width;  // +0x80
	int Height;  // +0x84
	int Flags;  // +0x88
	HFONT Font;  // +0x8C
};

HFONT Get_Font(HDC hdc, char *face, int width, int height, int flags);  // 0x76EB70  // NAME 2: Petzold's EzCreateFont (SaveDC, GM_ADVANCED, DPtoLP, the tmAveCharWidth width) with a cache; flags 1 bold, 2 italic, 4 underline, 8 strikeout

#endif
