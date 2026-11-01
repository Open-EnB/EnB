// Earth & Beyond client (reconstruction): app\client\html\htmlview.h. Notes at the end of htmlview.cpp.
#ifndef HTMLVIEW_H
#define HTMLVIEW_H

#include "..\..\..\wwlib\dsurface.h"
#include "unknown_htmlblock.h"

HFONT Get_Font(HDC hdc, char *face, int unknown, int height, int flags);  // 0x76EB70  // NAME 2: a cached CreateFontIndirect (not placed: before unknown_htmlblock.cpp)

// What the view hands a url to: slot +4 opens it, slot +8 is polled by Update (no implementation found yet).
class HtmlLoaderClass  // NAME 1: the object at the view's +4 (slots +4 / +8 called)
{
public:
	virtual void Unknown_00(void) = 0;  // slot +0, not called here
	virtual void Open(char *url) = 0;  // slot +4  // NAME 2: given the view's url by Open
	virtual void Update(void) = 0;  // slot +8  // NAME 2: polled by the view's Update
};

// 0x210 bytes; the view's Links (0x770860 builds one on its stack and looks them up by Name). Its destructor
// 0x770430 is called out of line by Clear.
struct HtmlLinkStruct  // NAME 1: element of the view's +0x14 list
{
	ArrayList<char> Data;  // +0x000  // NAME 2: Add_Data appends to it and keeps it NUL-terminated
	char Name[257];  // +0x00C  // NAME 2: 0x770860 compares it with its first argument
	char Url[257];  // +0x10D  // NAME 1: 0x770860's second argument copied in (257: the copy in 0x7789F0 moves 0x40 dwords and a byte)
};

// What the view's +0x20 list holds: Find_Image compares slot +0xC's string with a name.
class HtmlImageClass  // NAME 1: element of the view's +0x20 list (image.cpp's class, not placed yet)
{
public:
	virtual void Unknown_00(void) = 0;
	virtual void Unknown_04(void) = 0;
	virtual Surface *Get_Surface(void) = 0;  // slot +8  // NAME 2: Draw_Block tiles it (Get_Width / Get_Height)
	virtual char *Get_Name(void) = 0;  // slot +0xC  // NAME 2: Find_Image's key
	virtual void Draw(Surface *surface, int x, int y, int clipx, int clipy, int clipw, int cliph) = 0;  // slot +0x10  // NAME 2: Draw_Block's tiles
};

class HtmlViewClass  // NAME 3: htmlview.cpp's class (D24); its template copy ArrayList<HtmlLinkStruct>::~ArrayList is 0x7787C0
{
public:
	HtmlViewClass(void);  // 0x76FFD0
	~HtmlViewClass(void);  // 0x7701D0

	bool Clear(void);  // 0x7702A0  // NAME 2: Links and Blocks emptied (the blocks deleted)
	bool Open(char *url);  // 0x770460  // NAME 2: Clear, url kept, handed to the loader
	bool Set_Loader(HtmlLoaderClass *loader);  // 0x770640
	bool Update(void);  // 0x770660  // NAME 2: polls the loader; returns and clears Dirty
	bool Set_Surface(DSurface *surface);  // 0x770690  // NAME 2: Surf = surface, then Layout
	bool Clear_Links(void);  // 0x7706B0
	bool Clear_Blocks(void);  // 0x770720
	bool Add_Data(char *name, char *url, char *data, int length, bool render);  // 0x770860  // NAME 2: appends to the link named name (made with url if missing); render: Layout and Render
	void Layout(void);  // 0x770C70  // NAME 2: one block the surface's size for Url's link, then Parse
	void Parse(HtmlLinkStruct *link, HtmlBlockClass *block, int unknown);  // 0x7710D0  // NAME 2: the 15 KB tag parser (not written yet)
	bool Add_Text(HtmlBlockClass *block, char *cptr_in, int maxlength, int maxwidth, int unknown);  // 0x7759E0  // NAME 2: lays text out in lines of items (asserts 'maxlength > 0', 'cptr_in')
	bool Render(int scrollx, int scrolly, int x, int y, int width, int height);  // 0x7761E0  // NAME 2: draws every block clipped to the rectangle
	bool Draw_Block(HtmlBlockClass *block, int x, int y, int width, int height);  // 0x776390  // NAME 2: the background (BackgroundPointer image tiles or BackgroundColorref) and the visible items
	bool Draw_Item(HtmlBlockClass *block, HtmlItemStruct *item, int offx, int offy, int clipx, int clipy, int clipw, int cliph);  // 0x7767A0  // NAME 2: by its Type: Text (wrapped TextOut), HR / Table / Cell (bevels), LI (a filled box), Image
	char *Click(int x, int y, int unknown);  // 0x776FC0  // NAME 2: opens the 'Anchor.HREF' of the item under x, y
	char *Hit_Test(int x, int y, int *type, int *index);  // 0x7772C0  // NAME 2: the item under x, y: its index, its href (type 2) or none (1)
	bool Find_Link(char *name, HtmlLinkStruct **link);  // 0x777420
	bool Find_Image(char *name, HtmlImageClass **image);  // 0x7774F0
	int Get_Unknown13C(void);  // 0x7775B0
	int Get_Unknown138(void);  // 0x7775D0
	char *Get_Title(void);  // 0x7775F0  // NAME 2: the first block's 'Title' attribute, else ""

	void Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised);  // 0x777630
	void Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x7777C0
	void Draw_Line_32(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x7778E0
	void Draw_Line_24(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x777EC0
	void Draw_Line_16(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x777EE0

	DSurface *Surf;  // +0x000  // NAME 2: Set_Surface; Lock / Stride / Get_Width / Get_Height, GetDC (HV1's UnknownHtmlDrawClass)
	HtmlLoaderClass *Loader;  // +0x004  // NAME 2: Set_Loader
	ArrayList<HtmlBlockClass *> Blocks;  // +0x008  // NAME 2: owned (Clear deletes them)
	ArrayList<HtmlLinkStruct> Links;  // +0x014  // NAME 1
	ArrayList<HtmlImageClass *> Images;  // +0x020  // NAME 2: Find_Image
	bit8 Dirty;  // +0x02C  // NAME 2: Update returns and clears it (a byte tested, not a bool)
	char Url[259];  // +0x02D  // NAME 2: Open's strcpy; 259 bytes up to +0x130
	int ScrollX;  // +0x130  // NAME 2: Render's first argument (also given to the first block's Set_X); zeroed by Clear
	int ScrollY;  // +0x134  // NAME 2: Render's second argument; zeroed by Clear
	int Unknown138;  // +0x138  // set to 1000 by 0x776390
	int Unknown13C;  // +0x13C  // set to 4000 by 0x776390
};

#endif
