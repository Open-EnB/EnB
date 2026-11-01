// Earth & Beyond client (reconstruction): app\client\html\htmlview.cpp (PARTIAL). Notes at the end of the file.
#include <windows.h>
#include "htmlview.h"
#include "..\..\shared\unknown_debug.h"
#include <stdio.h>
#include <string.h>

HtmlViewClass::HtmlViewClass(void) :
	Surf(NULL),
	Loader(NULL)
{
	Url[0] = '\0';
	Links.clear();
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	ScrollX = 0;
	ScrollY = 0;
	Dirty = 0;
}

HtmlViewClass::~HtmlViewClass(void)
{
}

bool HtmlViewClass::Clear(void)
{
	Links.clear();
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	ScrollX = 0;
	ScrollY = 0;
	return true;
}

bool HtmlViewClass::Open(char *url)
{
	Clear();
	strcpy(Url, url);
	if (Loader == NULL)
		return false;
	Loader->Open(url);
	return true;
}

bool HtmlViewClass::Set_Loader(HtmlLoaderClass *loader)
{
	Loader = loader;
	return true;
}

bool HtmlViewClass::Update(void)
{
	if (Loader != NULL)
		Loader->Update();
	bool dirty = false;
	if (Dirty)
		dirty = true;
	Dirty = 0;
	return dirty;
}

bool HtmlViewClass::Set_Surface(DSurface *surface)
{
	Surf = surface;
	Layout();
	return true;
}

bool HtmlViewClass::Clear_Links(void)
{
	Links.clear();
	return true;
}

bool HtmlViewClass::Clear_Blocks(void)
{
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	return true;
}


























































bool HtmlViewClass::Add_Data(char *name, char *url, char *data, int length, bool render)
{
	if (name != NULL && url != NULL && data != NULL && length != 0) {
		HtmlLinkStruct *link;
		if (!Find_Link(name, &link)) {
			HtmlLinkStruct entry;
			strcpy(entry.Url, url);
			strcpy(entry.Name, name);
			Links.addTail(entry);
			if (!Find_Link(name, &link))
				G_ASSERT(0);
		}
		if (link->Data.length() != 0)
			link->Data.remove(link->Data.length() - 1);
		link->Data.addMany(data, link->Data.length(), length);
		link->Data.addMany("", link->Data.length(), 1);
	}
	if (render) {
		Layout();
		Render(0, 0, 0, 0, Surf->Get_Width(), Surf->Get_Height());
	}
	return true;
}

void HtmlViewClass::Layout(void)
{
	HtmlBlockClass *block;
	while (Blocks.removeTail(block)) {
		delete block;
	}

	for (int i = 0; i < Links.length(); i++) {
		HtmlLinkStruct *link;
		if (!Links.getPointer(&link, i))
			link = NULL;
		if (strcmp(link->Name, Url) == 0) {
			HtmlAttributeStackClass attributes;
			char buffer[64];
			sprintf(buffer, "%d", 10);
			attributes.Set("Horizontal_Padding", buffer);
			sprintf(buffer, "%d", 16);
			attributes.Set("Vertical_Padding", buffer);
			block = new HtmlBlockClass(NULL, Surf->Get_Width(), Surf->Get_Height(), &attributes, NULL);
			block->AbsX = 0;
			block->AbsY = 0;
			Blocks.addTail(block);
			Parse(link, block, 0);
			Dirty = 1;
			return;
		}
	}
}

// NOT EXACT: 0x7761E0, the parent bounds' stores scheduled later than the client's (the clip RECT, bottom first, matches)
bool HtmlViewClass::Render(int scrollx, int scrolly, int x, int y, int width, int height)
{
	if (Surf == NULL)
		return false;

	ScrollX = scrollx;
	ScrollY = scrolly;

	RECT clip;
	clip.bottom = y + height;
	clip.top = y;
	clip.left = x;
	clip.right = x + width;

	HtmlBlockClass *block = NULL;
	for (int i = 0; i < Blocks.length(); i++) {
		Blocks.get(block, i);
		if (i == 0) {
			block->Set_X(scrollx);
			block->Set_Y(scrolly);
		}
		RECT rect = clip;
		HtmlBlockClass *parent = block->Parent;
		if (parent != NULL) {
			RECT bounds;
			bounds.left = parent->AbsX;
			bounds.top = parent->AbsY;
			bounds.right = parent->Get_Width() + bounds.left;
			bounds.bottom = parent->Get_Height() + bounds.top;
			if (!IntersectRect(&rect, &clip, &bounds))
				continue;
		}
		Draw_Block(block, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	return true;
}

bool HtmlViewClass::Draw_Block(HtmlBlockClass *block, int x, int y, int width, int height)
{
	if (Surf == NULL)
		return false;

	RECT clip;
	clip.bottom = y + height;
	clip.top = y;
	clip.left = x;
	clip.right = x + width;
	RECT bounds;
	bounds.bottom = block->Get_Height() + block->Get_Abs_Y();
	bounds.top = block->Get_Abs_Y();
	bounds.left = block->Get_Abs_X();
	bounds.right = block->Get_Width() + block->Get_Abs_X();
	RECT draw;
	if (!IntersectRect(&draw, &clip, &bounds))
		return false;

	HtmlImageClass *image = NULL;
	char *pointer = block->Get_Attribute("BackgroundPointer");
	if (pointer != NULL)
		sscanf(pointer, "%p", &image);
	if (image == NULL || image->Get_Surface() == NULL) {
		COLORREF color = 0xC0C0C0;
		char *colorref = block->Get_Attribute("BackgroundColorref");
		if (colorref != NULL || block->Owner == NULL) {
			HDC hdc = Surf->GetDC();
			if (colorref != NULL)
				color = atol(colorref);
			HBRUSH brush = CreateSolidBrush(color);
			FillRect(hdc, &draw, brush);
			DeleteObject(brush);
			Surf->ReleaseDC(hdc);
		}
	} else {
		int tilew = image->Get_Surface()->Get_Width();
		int tileh = image->Get_Surface()->Get_Height();
		for (int tx = bounds.left - tilew; tx <= bounds.right + tilew; tx += tilew) {
			for (int ty = bounds.top - tileh; ty <= bounds.bottom + tileh; ty += tileh) {
				image->Draw(Surf, tx - block->X % tilew, ty - block->Y % tileh, draw.left, draw.top, draw.right - draw.left, draw.bottom - draw.top);
			}
		}
	}

	for (int i = 0; i < block->Items.length(); i++) {
		HtmlItemStruct *item = block->Get_Item(i);
		RECT rect;
		rect.bottom = item->Y + item->Height - block->Get_Y() + block->Get_Abs_Y();
		rect.top = item->Y - block->Get_Y() + block->Get_Abs_Y();
		rect.left = item->X - block->Get_X() + block->Get_Abs_X();
		rect.right = item->X + item->Width - block->Get_X() + block->Get_Abs_X();
		RECT visible;
		if (IntersectRect(&visible, &draw, &rect)) {
			Draw_Item(block, item, block->Get_X() - block->Get_Abs_X(), block->Get_Y() - block->Get_Abs_Y(), draw.left, draw.top, draw.right - draw.left, draw.bottom - draw.top);
		}
	}
	Unknown13C = 4000;
	Unknown138 = 1000;
	return true;
}

char *HtmlViewClass::Click(int x, int y, int)
{
	static char href[256];

	for (int i = Blocks.length() - 1; i >= 0; i--) {
		HtmlBlockClass *block = NULL;
		Blocks.get(block, i);
		int bx = block->Get_X() + x - block->Get_Abs_X();
		int by = block->Get_Y() + y - block->Get_Abs_Y();
		for (int j = 0; j < block->Items.length(); j++) {
			HtmlItemStruct *item = block->Get_Item(j);
			if (bx >= item->X && bx <= item->X + item->Width && by >= item->Y && by <= item->Y + item->Height) {
				char *value = item->Attributes.Get("Anchor.HREF");
				if (value == NULL)
					return NULL;
				strcpy(href, value);
				Open(href);
				return href;
			}
		}
	}
	return NULL;
}

// NOT EXACT: 0x7772C0, the loop top reloads this before i (the client i first); Click's twin
char *HtmlViewClass::Hit_Test(int x, int y, int *type, int *index)
{
	for (int i = Blocks.length() - 1; i >= 0; i--) {
		HtmlBlockClass *block = NULL;
		Blocks.get(block, i);
		int bx = block->Get_X() + x - block->Get_Abs_X();
		int by = block->Get_Y() + y - block->Get_Abs_Y();
		for (int j = 0; j < block->Items.length(); j++) {
			HtmlItemStruct *item = block->Get_Item(j);
			if (bx >= item->X && bx <= item->X + item->Width && by >= item->Y && by <= item->Y + item->Height) {
				*index = j;
				char *value = item->Attributes.Get("Anchor.HREF");
				if (value != NULL) {
					*type = 2;
					return value;
				}
				*type = 1;
				return NULL;
			}
		}
	}
	*type = 1;
	*index = -1;
	return NULL;
}

bool HtmlViewClass::Find_Link(char *name, HtmlLinkStruct **link)
{
	for (int i = 0; i < Links.length(); i++) {
		Links.getPointer(link, i);
		if (strcmp((*link)->Name, name) == 0)
			return true;
	}
	*link = NULL;
	return false;
}

bool HtmlViewClass::Find_Image(char *name, HtmlImageClass **image)
{
	for (int i = 0; i < Images.length(); i++) {
		HtmlImageClass *entry;
		Images.get(entry, i);
		if (strcmp(entry->Get_Name(), name) == 0) {
			*image = entry;
			return true;
		}
	}
	*image = NULL;
	return false;
}

int HtmlViewClass::Get_Unknown13C(void)
{
	return Unknown13C;
}

int HtmlViewClass::Get_Unknown138(void)
{
	return Unknown138;
}

char *HtmlViewClass::Get_Title(void)
{
	char *title = "";
	if (Blocks.length() != 0) {
		HtmlBlockClass *block = NULL;
		Blocks.get(block, 0);
		title = block->Get_Attribute("Title");
		if (title == NULL)
			title = "";
	}
	return title;
}

void HtmlViewClass::Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised)
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

void HtmlViewClass::Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha)
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
void HtmlViewClass::Draw_Line_24(int, int, int, int, int, int, int, int, int, int)
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
**	* HV2 (2026-11-01): the view, HtmlViewClass (htmlview.h), 0x76FFD0-0x770720, every function exact. The
**	  file starts at 0x76FFD0: 0x76F0F0-0x76FFCF is HtmlBlockClass's object (unknown_htmlblock.cpp, ended by
**	  its ArrayList copies), and the view's ArrayList<HtmlLinkStruct> destructor is this file's template
**	  copy 0x7787C0. 0x770430 (the 0x210-byte link's destructor, a COMDAT inside the run) pairs with several.
**	  Forms: the constructor writes Clear's body out (its link destructors inline, Clear's call them);
**	  the block lists empty as `while (getTail(b)) { delete b; remove(length() - 1); }` (remove inline,
**	  its result unused); Update tests the byte Dirty into a bool set in a branch.
**	  Its constructor, destructor, Clear, Open and Clear_* have no callers in the client; image.cpp's
**	  0x76E9D0 calls 0x770860, which calls Layout and Render. HV1's UnknownHtmlDrawClass is the view (+0 its
**	  wwlib Surface: Get_Width / Get_Height size Layout's block). Draw_Line_32 0x7778E0: parked at 0.18
**	  (attempts.csv): the form of Draw_Line_16 compiles to the same shape but keeps alpha, not x1, cached.
**	* HtmlBlockClass's inline getters (Get_X / Get_Y / Get_Width / Get_Height) are why the client reads a
**	  field before an out-of-line call in one expression (Click, Draw_Block); the RECTs are filled bottom,
**	  top, left, right. NOT EXACT (scheduling): Render (the parent bounds' stores), Hit_Test (its loop top).
*/
