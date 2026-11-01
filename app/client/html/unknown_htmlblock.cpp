// Earth & Beyond client (reconstruction): app\client\html\unknown_htmlblock.cpp. Notes at the end of the file.
#include "unknown_htmlblock.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

HtmlBlockClass::HtmlBlockClass(void *owner, int width, int height, HtmlAttributeStackClass *attributes, HtmlAttributeStackClass *styles) :
	HPadding(0),
	VPadding(0),
	Owner(owner),
	Parent(NULL)
{
	Unknown98 = false;
	X = 0;
	Y = 0;
	AbsX = 0;
	AbsY = 0;
	Width = width;
	Height = height;
	CursorX = 0;
	CursorY = 0;
	RightPadding = 0;
	Unknown1C0 = 0;
	Unknown1C4 = 0;

	if (attributes != NULL)
		Attributes.Add_All(*attributes);
	if (styles != NULL)
		Styles.Add_All(*styles);

	char *value = Attributes.Get("Horizontal_Padding");
	if (value != NULL) {
		CursorX = atol(value);
		HPadding = atol(value);
		RightPadding = atol(value);
	}
	value = Attributes.Get("Vertical_Padding");
	if (value != NULL) {
		CursorY = atol(value);
		VPadding = atol(value);
	}
	Unknown99 = false;
}

HtmlBlockClass::~HtmlBlockClass(void)
{
	HtmlItemStruct *item;
	while (Items.removeTail(item)) {
		delete item;
	}
}

int HtmlBlockClass::Advance_X(int width, bool)
{
	CursorX += width;
	return CursorX;
}

int HtmlBlockClass::Advance_Y(int height, bool absolute)
{
	if (absolute) {
		CursorY += height;
		return CursorY;
	}
	if (Items.length() == 0)
		return CursorY;

	char *align = Styles.Get("Align");
	if (align != NULL) {
		int right;
		if (strcmp(align, "p.center") == 0 || strcmp(align, "center") == 0) {
			right = 0;
		} else if (strcmp(align, "p.right") == 0 || strcmp(align, "right") == 0) {
			right = 1;
		} else {
			goto valign;
		}

		HtmlItemStruct *item = NULL;
		int width = 0;
		int i;
		for (i = 0; i < Items.length(); i++) {
			Items.get(item, i);
			if (item->Y == CursorY)
				width += item->Width;
		}
		int offset = Width - HPadding * 2 - width;
		if (!right)
			offset /= 2;
		if (offset < 0)
			offset = 0;
		for (i = 0; i < Items.length(); i++) {
			Items.get(item, i);
			if (item->Y == CursorY)
				item->X += offset;
		}
	}

valign:
	HtmlItemStruct *item = NULL;
	int lineheight = 0;
	int i;
	for (i = 0; i < Items.length(); i++) {
		Items.get(item, i);
		if (item->Y == CursorY && item->Height > lineheight)
			lineheight = item->Height;
	}
	for (i = 0; i < Items.length(); i++) {
		Items.get(item, i);
		if (item->Y == CursorY)
			item->Y += lineheight - item->Height;
	}
	Items.get(item, Items.length() - 1);
	if (item->Height + 20 + item->Y < lineheight + CursorY + height)
		height = item->Height - CursorY - lineheight + 20 + item->Y;
	CursorY += lineheight + height;
	return CursorY;
}

void HtmlBlockClass::Add_Child(HtmlBlockClass *child)
{
	Children.addTail(child);
	child->Parent = this;
}

int HtmlBlockClass::Get_Abs_X(void)
{
	if (Parent == NULL)
		return AbsX;
	int x = AbsX;
	return x + Parent->Get_Abs_X();
}

int HtmlBlockClass::Get_Abs_Y(void)
{
	if (Parent == NULL)
		return AbsY;
	int y = AbsY;
	return y + Parent->Get_Abs_Y();
}

void HtmlBlockClass::Set_X(int x)
{
	int delta = X - x;
	X = x;
	for (int i = 0; i < Children.length(); i++) {
		HtmlBlockClass *child;
		Children.get(child, i);
		child->AbsX += delta;
	}
}

void HtmlBlockClass::Set_Y(int y)
{
	int delta = Y - y;
	Y = y;
	for (int i = 0; i < Children.length(); i++) {
		HtmlBlockClass *child;
		Children.get(child, i);
		child->AbsY += delta;
	}
}

HtmlItemStruct *HtmlBlockClass::Get_Item(int index)
{
	HtmlItemStruct *item = NULL;
	Items.get(item, index);
	return item;
}

int HtmlBlockClass::Get_Align(void)
{
	char *align = Styles.Get("Align");
	if (align == NULL)
		return -1;
	if (strcmp(align, "p.center") == 0 || strcmp(align, "center") == 0)
		return 0;
	if (strcmp(align, "p.right") == 0 || strcmp(align, "right") == 0)
		return 1;
	return -1;
}

int HtmlBlockClass::Get_Font_Height(HtmlItemStruct *item)
{
	char *size = Find_Style(item, "Font.HSize");
	if (size != NULL && isdigit(*size)) {
		switch (atol(size)) {
			case 1:
				return 38;
			case 2:
				return 30;
			case 3:
				return 26;
			case 4:
				return 22;
		}
	} else {
		char *base = Find_Style(item, "Font.BaseSize");
		if (base == NULL)
			return 18;
		return (atol(base) + 1) * 6;
	}
	return 18;
}

void HtmlBlockClass::Set_Style(char *key, char *value)
{
	Styles.Set(key, value);
}

void HtmlBlockClass::Set_Attribute(char *key, char *value)
{
	Attributes.Set(key, value);
}

void HtmlBlockClass::Push_Style(char *key, char *value)
{
	Styles.Push(key, value);
}

void HtmlBlockClass::Push_Attribute(char *key, char *value)
{
	Attributes.Push(key, value);
}

bool HtmlBlockClass::Remove_Style(char *key)
{
	return Styles.Remove(key);
}

bool HtmlBlockClass::Remove_Attribute(char *key)
{
	return Attributes.Remove(key);
}

bit8 HtmlBlockClass::Pop_Style(char *key, char *value)
{
	return Styles.Pop(key, value);
}

bit8 HtmlBlockClass::Pop_Attribute(char *key, char *value)
{
	return Attributes.Pop(key, value);
}

char *HtmlBlockClass::Get_Style(char *key)
{
	return Styles.Get(key);
}

char *HtmlBlockClass::Get_Attribute(char *key)
{
	return Attributes.Get(key);
}

/*
**	Reconstruction notes (HV2, 2026-11-01).
**	* The object: client 0x76F0F0-0x76FFCF, HtmlBlockClass's members 0x76F0F0-0x76FE60, then its template
**	  copies (ArrayList<HtmlItemStruct *> / ArrayList<HtmlBlockClass *> destructors 0x76FE80 / 0x76FEB0 and
**	  ArrayList<HtmlItemStruct *>::remove 0x76FEE0, from libs\wwonline\wlib). A run of template copies ends
**	  an object, so htmlview.cpp's own code starts at 0x76FFD0 (the view's constructor); its first assert
**	  (0x770860, line 158) leaves no room for these 25 functions above it either. No __FILE__ or assert
**	  names the file: unknown_htmlblock.cpp is a neutral name (D137 placement). Its literals ('Align',
**	  'p.center', 'Font.HSize', 'Horizontal_Padding'...) sit before htmlview.cpp's __FILE__ in .rdata, but
**	  literals are merged across objects (no file evidence). 0x76EB70 (the font cache before it, called
**	  from htmlview.cpp) is not this object's: it and 0x76E9D0 (image.cpp's vtable slot, just before it)
**	  share the static ArrayList at 0xBE82E0, so it ends that object.
**	* 0x76FCC0 (`xor eax, eax; ret`, between Get_Font_Height and the wrappers) is not written: its class is
**	  unknown (no caller, no vtable found yet).
**	* Forms: Get_Abs_X / Y read AbsX into a local after the NULL test (the client loads it before the call);
**	  Set_X / Y's child pointer is uninitialised (it takes the argument's slot, as the client's does).
*/
