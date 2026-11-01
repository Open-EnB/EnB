// Earth & Beyond client (reconstruction): app\client\fontsys_cursor.cpp. Notes at the end of the file.
#include "fontsys_cursor.h"
#include "fontmanager.h"
#include "font32bit.h"
#include "..\shared\unknown_debug.h"

// local_interface_sort.cpp's sort object (8 bytes; constructor 0x6E0E80 over an interface, destructor 0x6E18B0), as intrface.cpp's
class InterfaceSortClass  // NAME 1: neutral (D10)
{
public:
	InterfaceSortClass(InterfaceClass *iface);		// 0x6E0E80  // NAME 1 [iface]: neutral
	~InterfaceSortClass();							// 0x6E18B0

	char Unknown00[8];
};

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1 [Unknown_4A1590]: neutral placeholder (D10)

RefCountPtr<TextCursorClass> Unknown_685680(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &unknown)
{
	return Create_NEW(new TextCursorClass(iface, unknown));
}

TextCursorClass::TextCursorClass(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &unknown) :
	Rect3DClass(Vector4(0, 0.7f, 0.1f, 0.5f), Vector4(0, 0, 1, 1)),
	Scene(NULL),
	Iface(iface)
{
	SortLevel = 1;
	Disable_Sort();
}

TextCursorClass::~TextCursorClass()
{
	Unknown_685DD0();
	Iface = NULL;
}




void TextCursorClass::Unknown_685A30(const RefCountPtr<SceneClass> &scene)
{
	if (Peek_Scene()) {
		Remove();
	}
	Scene = scene;
}

void TextCursorClass::Unknown_685AB0(const RefCountPtr<FontStyleClass> &line, int cursorpos, bool insert, bool center)
{
	if (Scene == NULL) {
		return;
	}























	FAIL_IF(!line) return;
	if (!Peek_Scene()) {
		InterfaceSortClass sort(Iface);
		Add(Scene.Peek());
	}


	const char *text = line->Unknown_6D87D0();
	FAIL_IF(!text) return;

	FontStyleClass *style = line.Peek();
	int level = style->Get_Sort_Level() - 1;
	if (level <= 1) level = 1;
	Set_Sort_Level(level);

	char *buffer = new char[cursorpos + 2];
	Unknown_4A1590(buffer, cursorpos + 2, text);
	char c = buffer[cursorpos];
	int width = (int)(c ? style->Font->Get_Char_Width(c, 640.0f) : style->Font->Get_Char_Width('n', 640.0f)) + 1;
	if (width < 2) {
		width = 2;
	}
	buffer[cursorpos] = 0;
	int x = (int)style->Font->Get_Text_Width(buffer, 640.0f);
	float height = style->Font->Get_Char_Height(480.0f);

	Vector3 pos;
	line->Unknown_6D8650(pos);
	if (center) {
		pos.X -= (float)((int)style->Font->Get_Text_Width(text, 640.0f) / 2);
	}
	if (line->Info.Justify[0] == 0) {
		pos.Y += height * 0.5f;
	}
	if (insert) {
		Set_Rect(Vector4(x + pos.X, pos.Y - (height - 2.0f), (float)width, 2.0f));
	} else {
		Set_Rect(Vector4(x + pos.X, pos.Y, (float)width, height - 1.0f));
	}
	delete [] buffer;
}

void TextCursorClass::Unknown_685DD0()
{
	if (Peek_Scene()) {
		Remove();
	}
}

/*
**	Reconstruction notes (fontsys_cursor.cpp, client 0x685680-0x685E70; __FILE__ 0xB8FD0C, FAIL_IF on the
**	client's lines 78 ('!line') and 86 ('!text'); D25: blank lines keep them there).
**
**	TextCursorClass: a Rect3DClass in (0, 0.7, 0.1, 0.5) over (0, 0, 1, 1), sort level 1, unsorted.
**	0x685AB0 puts it into its scene (inside local_interface_sort.cpp's scope over the interface) at the
**	line's cursor: one level under the line, as wide as the character at the cursor (an 'n' at the end;
**	at least 2), at the width of the text before it; an underline 2 high when inserting, else the font's
**	height less one; centered lines move it back by half the text's width. 0x6857C0 / 0x685800 / 0x685840
**	are the inline virtuals this object emitted first (DynamicMeshClass's bounding volumes, Rect3DClass's
**	Class_ID); 0x685E00 is RefCountPtr's Create_NEW, 0x685E30 <string>'s guarded static (0xC416FC).
*/
