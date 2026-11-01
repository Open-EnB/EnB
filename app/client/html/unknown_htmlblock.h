// Earth & Beyond client (reconstruction): app\client\html\unknown_htmlblock.h. Notes in unknown_htmlblock.cpp.
#ifndef UNKNOWN_HTMLBLOCK_H
#define UNKNOWN_HTMLBLOCK_H

#include "unknown_htmlattrib.h"

// One laid-out piece of a block's text: 0x50 bytes, deleted by the block's destructor.
struct HtmlItemStruct  // NAME 2: what htmlview's parser lays out in a block (position, size, its attribute stacks)
{
	int X;  // +0x00  // NAME 2: moved right by the line's alignment (0x76F3D0)
	int Y;  // +0x04  // NAME 2: compared with the block's CursorY: the item's line
	int Unknown08;  // +0x08
	int Width;  // +0x0C  // NAME 2: summed over a line for its alignment
	int Height;  // +0x10  // NAME 2: the line's tallest sets its height
	int Unknown14;  // +0x14
	HtmlAttributeStackClass Attributes;  // +0x18  // NAME 2: read by Get_Font_Height in place of the block's styles
};

// A box of laid-out HTML: its items, child blocks, two attribute stacks and a text cursor. 0x1C8 bytes.
class HtmlBlockClass  // NAME 2: htmlview's layout box (parent, children, padding, cursor); constructor 0x76F0F0
{
public:
	HtmlBlockClass(void *owner, int width, int height, HtmlAttributeStackClass *attributes, HtmlAttributeStackClass *styles);  // 0x76F0F0
	~HtmlBlockClass(void);  // 0x76F2B0

	int Advance_X(int width, bool unused);  // 0x76F3A0  // NAME 2: CursorX += width
	int Advance_Y(int height, bool absolute);  // 0x76F3D0  // NAME 2: ends the cursor's line (alignment, baseline) and moves down
	void Add_Child(HtmlBlockClass *child);  // 0x76F710  // NAME 2: Children.addTail, child->Parent = this
	int Get_Abs_X(void);  // 0x76F8F0  // NAME 2: AbsX plus the parents'
	int Get_Abs_Y(void);  // 0x76F920  // NAME 2: AbsY plus the parents'
	void Set_X(int x);  // 0x76F950  // NAME 2: moves the children's AbsX by the difference
	void Set_Y(int y);  // 0x76F9C0  // NAME 2: moves the children's AbsY by the difference
	HtmlItemStruct *Get_Item(int index);  // 0x76FA30  // NAME 2: Items[index] or NULL
	int Get_Align(void);  // 0x76FA60  // NAME 2: 'Align' style: 0 center, 1 right, -1 otherwise
	int Get_Font_Height(HtmlItemStruct *item);  // 0x76FBD0  // NAME 2: 'Font.HSize' 1-4 -> 38/30/26/22, else ('Font.BaseSize' + 1) * 6, else 18
	char *Find_Style(HtmlItemStruct *item, char *key) { if (item != NULL) return item->Attributes.Get(key); return Styles.Get(key); }  // NAME 2: the item's value, else the block's (inline: Get_Font_Height pushes the key before choosing the stack)

	void Set_Style(char *key, char *value);  // 0x76FCE0  // NAME 2: Styles.Set
	void Set_Attribute(char *key, char *value);  // 0x76FD10  // NAME 2: Attributes.Set
	void Push_Style(char *key, char *value);  // 0x76FD40  // NAME 2: Styles.Push
	void Push_Attribute(char *key, char *value);  // 0x76FD70  // NAME 2: Attributes.Push
	bool Remove_Style(char *key);  // 0x76FDA0  // NAME 2: Styles.Remove
	bool Remove_Attribute(char *key);  // 0x76FDC0  // NAME 2: Attributes.Remove
	bit8 Pop_Style(char *key, char *value);  // 0x76FDE0  // NAME 2: Styles.Pop
	bit8 Pop_Attribute(char *key, char *value);  // 0x76FE10  // NAME 2: Attributes.Pop
	char *Get_Style(char *key);  // 0x76FE40  // NAME 2: Styles.Get
	char *Get_Attribute(char *key);  // 0x76FE60  // NAME 2: Attributes.Get

	int HPadding;  // +0x000  // NAME 2: 'Horizontal_Padding'
	int VPadding;  // +0x004  // NAME 2: 'Vertical_Padding'
	void *Owner;  // +0x008  // NAME 1: the constructor's first argument, not read here
	ArrayList<HtmlItemStruct *> Items;  // +0x00C  // NAME 2: owned (the destructor deletes them)
	ArrayList<HtmlBlockClass *> Children;  // +0x018  // NAME 2: Add_Child; not deleted here
	HtmlBlockClass *Parent;  // +0x024  // NAME 2: set by the parent's Add_Child
	HtmlAttributeStackClass Attributes;  // +0x028  // NAME 2: the constructor's 4th argument copied in; holds the paddings
	HtmlAttributeStackClass Styles;  // +0x060  // NAME 2: the 5th argument copied in; 'Align', 'Font.*'
	bool Unknown98;  // +0x098
	bool Unknown99;  // +0x099
	char Unknown9C[256];  // +0x09C  // not set by the constructor
	int X;  // +0x19C  // NAME 2: Set_X
	int Y;  // +0x1A0  // NAME 2: Set_Y
	int AbsX;  // +0x1A4  // NAME 2: summed up the parents by Get_Abs_X
	int AbsY;  // +0x1A8  // NAME 2: summed up the parents by Get_Abs_Y
	int Width;  // +0x1AC  // NAME 2: the constructor's 2nd argument; Advance_Y aligns within it
	int Height;  // +0x1B0  // NAME 2: the constructor's 3rd argument
	int CursorX;  // +0x1B4  // NAME 2: Advance_X; starts at the horizontal padding
	int CursorY;  // +0x1B8  // NAME 2: Advance_Y; starts at the vertical padding
	int RightPadding;  // +0x1BC  // NAME 2: the horizontal padding again
	int Unknown1C0;  // +0x1C0
	int Unknown1C4;  // +0x1C4
};

#endif
