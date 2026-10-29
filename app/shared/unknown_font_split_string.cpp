// Earth & Beyond client (reconstruction): app\shared\ (file name unknown, D10). Notes at the end of the file.
// PLACEMENT: provisional (D137): a string, assert or link map naming the object 0x4FA8F0-0x4FAC0F; it links
// between split_string.obj and symbol_substitution.obj, so its real name sorts between those two.
#include "unknown_font_split_string.h"


FontSplitStringClass::FontSplitStringClass(const RefCountPtr<Font32Class> &font, const char *text, float width, int cursor) :
	SplitStringClass(NULL, 0, 0)
{
	Set_Font(font);
	if (text) {
		Split(text, width, cursor);
	}
}


FontSplitStringClass::~FontSplitStringClass()
{
	Split(NULL, 0, 0);
	Set_Font(NULL);
}


void FontSplitStringClass::Set_Font(const RefCountPtr<Font32Class> &font)
{
	Font = font;
}


float FontSplitStringClass::Get_Text_Width(const char *buff)
{
	if (Font) {
		return Font->Get_Text_Width(buff, 640.0f);
	}
	return SplitStringClass::Get_Text_Width(buff);
}


float FontSplitStringClass::Get_Char_Width(char c)
{
	if (Font) {
		return Font->Get_Char_Width(c, 640.0f);
	}
	return SplitStringClass::Get_Char_Width(c);
}


/*
**	Reconstruction notes
**
**	client.exe 0x4FA8F0-0x4FAC0F: the constructor 0x4FA8F0 (deleting destructor 0x4FA9C0), the
**	destructor 0x4FA9F0, Set_Font 0x4FAAC0 (inlined in both: the destructor's Set_Font(NULL) is the
**	stack temporary of EH state 2), the width virtuals 0x4FAB30 / 0x4FAB70, RefCountPtr<Font32Class>'s
**	destructor 0x4FABB0 (the instance this object emits) and the static initialiser 0x4FABF0 that
**	ends the object (the same as split_string.cpp's 0x4FA8C0).
*/
