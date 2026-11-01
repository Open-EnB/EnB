// Earth & Beyond client (reconstruction): app\client\html\htmlview.h. Notes at the end of htmlview.cpp.
#ifndef HTMLVIEW_H
#define HTMLVIEW_H

#include "..\..\..\wwlib\surface.h"
#include "unknown_htmlblock.h"

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
	ArrayList<int> Unknown00;  // +0x000  // element type unknown (only its destructor is seen)
	char Name[257];  // +0x00C  // NAME 2: 0x770860 compares it with its first argument
	char Url[259];  // +0x10D  // NAME 1: 0x770860's second argument copied in
};

struct HtmlUnknown20Struct  // NAME 1: element of the view's +0x20 list (only the list's destructor is seen)
{
	int Unknown00;
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
	bool Set_Surface(Surface *surface);  // 0x770690  // NAME 2: Surf = surface, then Layout
	bool Clear_Links(void);  // 0x7706B0
	bool Clear_Blocks(void);  // 0x770720
	void Layout(void);  // 0x770C70  // NAME 2: one block the surface's size for Url's link, then Parse
	void Parse(HtmlLinkStruct *link, HtmlBlockClass *block, int unknown);  // 0x7710D0  // NAME 2: the 15 KB tag parser (not written yet)

	void Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised);  // 0x777630
	void Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x7777C0
	void Draw_Line_32(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x7778E0
	void Draw_Line_24(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x777EC0
	void Draw_Line_16(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha);  // 0x777EE0

	Surface *Surf;  // +0x000  // NAME 2: Set_Surface; Lock / Stride / Get_Width / Get_Height called (HV1's UnknownHtmlDrawClass)
	HtmlLoaderClass *Loader;  // +0x004  // NAME 2: Set_Loader
	ArrayList<HtmlBlockClass *> Blocks;  // +0x008  // NAME 2: owned (Clear deletes them)
	ArrayList<HtmlLinkStruct> Links;  // +0x014  // NAME 1
	ArrayList<HtmlUnknown20Struct> Unknown20;  // +0x020
	bit8 Dirty;  // +0x02C  // NAME 2: Update returns and clears it (a byte tested, not a bool)
	char Url[259];  // +0x02D  // NAME 2: Open's strcpy; 259 bytes up to +0x130
	int Unknown130;  // +0x130  // zeroed by Clear
	int Unknown134;  // +0x134  // zeroed by Clear
};

#endif
