// Earth & Beyond client (reconstruction)
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include "gd_text_editor.h"
#include "unknown_671300.h"
#include "gd_slider.h"
#include "textgadg.h"
#include "sclient.h"
#include "fontmanager.h"
#include "font32bit.h"
#include "fontsys_cursor.h"
#include "rendobj.h"
#include "layer.h"
#include "scene.h"
#include "..\common\stredit.h"
#include "..\shared\unknown_font_split_string.h"
#include "..\kernel\kernel.h"
#include "..\audio\soundlib.h"
#include "..\input\input.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwmath\vector2.h"

extern unsigned int Unknown_BE5F34;										// 0xBE5F34 (gadget.cpp's: the event mask)  // NAME 1: neutral (D10)
// @0x671300
Unknown_671300Class::Unknown_671300Class(RenderObjClass *robj, int unknown1, const LayerClass &layer, int unknown2, int unknown3, int unknown4) :
	ButtonGadgetClass(robj, layer, 0x77, unknown4),
	Unknown64(false), Unknown65(false), Unknown66(false), Unknown68(NULL), Unknown6C(0), Unknown70(0), Unknown74(false),
	Unknown78(unknown2), Unknown7C(unknown3)
{
	if (unknown1) {
		Unknown68 = new UVControllerClass(unknown1, 0, 0);
	}
}
// @0x671410
Unknown_671300Class::Unknown_671300Class(RenderObjClass *robj, int unknown1, int unknown2, int unknown3, int unknown4) :
	ButtonGadgetClass(robj, 0x77, unknown4),
	Unknown64(false), Unknown65(false), Unknown66(false), Unknown68(NULL), Unknown6C(0), Unknown70(0), Unknown74(false),
	Unknown78(unknown2), Unknown7C(unknown3)
{
	if (unknown1) {
		Unknown68 = new UVControllerClass(unknown1, 0, 0);
	}
}
// @0x6714F0
Unknown_671300Class::~Unknown_671300Class()
{
	if (Unknown68) {
		delete Unknown68;
		Unknown68 = NULL;
	}
}
// @0x671570
bool Unknown_671300Class::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Unknown_60()) {
		if (mask & 0x11) {
			Unknown_66B770Class::Unknown_2C(event, x, y, mask);
			Unknown64 = true;
			return true;
		}
		if (mask & 0x22) {
			Unknown_66B770Class::Unknown_2C(event, x, y, mask);
			Unknown65 = true;
			return true;
		}
		if (mask & 0x44) {
			Unknown_66B770Class::Unknown_2C(event, x, y, mask);
			Unknown66 = true;
			return true;
		}
	}
	return false;
}
// @0x671630
void Unknown_671300Class::Unknown_671630()
{
	Unknown64 = false;
	Unknown65 = false;
	Unknown66 = false;
}
// @0x671650
void Unknown_671300Class::Unknown_A4(float speed)
{
	Unknown74 = true;
	Unknown70 = speed;
}
// @0x671670
void Unknown_671300Class::Unknown_A8()
{
	Unknown74 = false;
	Unknown6C = 0;
	Unknown70 = 0;
	Unknown68->Unknown_4A13A0(Vector2(0, 0));
}
// @0x6716B0
void Unknown_671300Class::Unknown_AC()
{
	if (Unknown68 && Unknown74) {
		Unknown6C += WW3D::Get_Frame_Time() * 0.001f;
		float offset = Unknown6C * Unknown70;
		Unknown68->Unknown_4A13A0(Vector2(Unknown78 * offset, Unknown7C * offset));
	}
}
// @0x671740
void Unknown_671300Class::Unknown_671740(bool on, float speed)
{
	if (on) {
		Unknown_A4(speed);
		Unknown_66C2D0();
		return;
	}
	Unknown_A8();
	Unknown_66C360();
}
// @0x671790
TextEditorGadgetClass::TextEditorGadgetClass(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &model, const char *font, GadgetSlider *slider, int maxlen, const char *text) :
	RObjGadgetClass(NULL, 0x105),
	Unknown60(true),
	Edit(NULL),
	Font(font),
	Split(NULL),
	TopLine(0),
	LineCount(0),
	CursorLine(0),
	CursorColumn(0),
	Width(0),
	Slider(slider),
	Unknown_A8(false),
	Unknown_A9(false),
	LineHeight(0),
	Unknown_B0(0),
	Client(NULL)
{
	Unknown5C_03 = false;
	Unknown_65FFA0(-1001);
	Set_First_Sort_Group(true);
	if (slider) {
		slider->Unknown_65FFC0(-1, this);
	}
	Cursor = Unknown_685680(iface, RefCountPtr<RenderObjClass>());
	Edit = new StrEditClass(maxlen, text, Unknown_B08CC4, NULL);
	Split = new FontSplitStringClass(RefCountPtr<Font32Class>());
	Split->Set_Escaped_New_Lines(false);
	Split->Set_Trim_Last(false);
	Split->Set_Trim_Lines(false);
	Set_Render_Obj(model.Peek());
}
// @0x671B30
TextEditorGadgetClass::~TextEditorGadgetClass()
{
	Set_Render_Obj(NULL);
	delete Edit;
	Edit = NULL;
	delete Split;
	Split = NULL;
	Slider = NULL;
}

// NOT EXACT: registers (client this esi, it ebp, line edi with a home at +0x24; here ebx, edi, esi) and the slots of first and height swapped
// @0x671CB0
void TextEditorGadgetClass::Set_Render_Obj(RenderObjClass *robj)
{
	for (std::vector<RefCountPtr<FontSys> >::iterator it = Lines.begin(); it != Lines.end(); it++) {
		RefCountPtr<FontSys> line = *it;
		line->Remove();
	}
	Lines.clear();
	RObjGadgetClass::Set_Render_Obj(robj);
	float height;
	RenderObjClass *object = RObj;
	if (object) {
		RenderObjClass *top = object->Get_Container();
		if (top == NULL) {
			top = object;
		}
		RefCountPtr<FontSys> first = Create_Get(new FontSys(Font.c_str(), (int)top, NULL));
		Lines.push_back(first);
		Split->Set_Font(*(RefCountPtr<Font32Class> *)&first->Font);
		float width = 0;
		height = 0;
		first->Unknown_6D5B10(width, height);
		height = height + 1.0f;
		height = floor(height);
		LineHeight = height;
		Unknown_B0 = width;
		const AABoxClass &box = object->Get_Bounding_Box();
		int rows = box.Extent.Y * 2 / height;
		Width = box.Extent.X * 2;
		height /= 480.0f;
		float y = 0;
		first->Unknown47C.X = 0;
		first->Unknown47C.Y = 0;
		for (int i = 0; i < rows - 1; i++) {
			first = Create_Get(new FontSys(Font.c_str(), (int)top, NULL));
			Lines.insert(Lines.end(), 1, first);
			y -= height;
			first->Unknown47C.X = 0;
			first->Unknown47C.Y = y * 480.0f;
		}
		Update_Lines(true);
	}
}
// @0x6720A0
bool TextEditorGadgetClass::Unknown_30(float x, float y)
{
	if (Slider) {
		if (Slider->Is_Held()) {
			Unknown_A9 = true;
			return false;
		}
		if (Unknown_A9) {
			Update_Lines(false);
		}
	}
	return RObjGadgetClass::Unknown_30(x, y);
}

// NOT EXACT: only 0x672A60 inlined in this copy of 0x673150; the client calls it
// @0x672120
bool TextEditorGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Unknown_BE5F24 == this) {
		if (Unknown_A9) {
			Unknown_A9 = false;
			return true;
		}
		if (!event->Is_Up()) {
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
		}
		if (mask & 0x100) {
			if (!event->Is_Up()) {
				switch (event->Key) {
					case 0x26:
						Cursor_Up(true);
						break;
					case 0x22:
						Page_Down();
						break;
					case 0x28:
						Cursor_Down(true);
						break;
					case 0x21:
						Page_Up();
						break;
					case 0x24:
						Cursor_Home();
						break;
					case 0x23:
						Cursor_End();
						break;
					case 0x1B:
						Drop_Focus();
						break;
					default:
						if (Edit->Process_Key(event)) {
							Update_Lines(true);
						}
						break;
				}
			}
			GadgetClass::Unknown_2C(event, x, y, mask);
			return true;
		}
		if (mask & 5) {
			GadgetClass::Unknown_2C(event, x, y, mask);
			if (!RObjGadgetClass::Unknown_30(x, y) || Unknown_6735A0(x, y)) {
				Drop_Focus();
			} else if (mask & 1) {
				Unknown_6732F0(x, y);
			}
		}
		return true;
	}
	if (mask & 1) {
		GadgetClass::Unknown_2C(event, x, y, mask);
		SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
		Unknown_6732F0(x, y);
		Take_Focus();
		return true;
	}
	return false;
}
// @0x6725B0
bool TextEditorGadgetClass::Unknown_68()
{
	if (GadgetClass::Unknown_68()) {
		for (std::vector<RefCountPtr<FontSys> >::iterator it = Lines.begin(); it != Lines.end(); it++) {
			RefCountPtr<FontSys> line = *it;
			line->Unknown_6CE910(true);
		}
		return true;
	}
	return false;
}

// @0x672680
void TextEditorGadgetClass::Unknown_6C()
{
	GadgetClass::Unknown_6C();
	for (std::vector<RefCountPtr<FontSys> >::iterator it = Lines.begin(); it != Lines.end(); it++) {
		RefCountPtr<FontSys> line = *it;
		line->Unknown_6CE910(false);
	}
	Cursor->Unknown_685DD0();
}

// @0x672740
void TextEditorGadgetClass::Unknown_74()
{
	RefCountPtr<SceneClass> scene = Create_Peek(Layer.Peek_Scene());
	Cursor->Unknown_685A30(scene);
}

// @0x6727E0
void TextEditorGadgetClass::Unknown_78()
{
	Cursor->Unknown_685A30(NULL);
}

// @0x672870
std::string TextEditorGadgetClass::Get_Text()
{
	char *string = Edit->Get_String();
	std::string text(string);
	delete string;
	return text;
}

// @0x6729B0
void TextEditorGadgetClass::Set_Text(const std::string &text)
{
	Edit->Set_String(text);
	Update_Lines(true);
	Slider->Unknown_A4(0, false);
}

// @0x6729F0
void TextEditorGadgetClass::Insert_Text(const std::string &text)
{
	if (Unknown60) {
		Edit->Clear();
		Unknown60 = false;
	}
	Edit->Insert_String(text);
	Update_Lines(true);
}








// @0x672A60
void TextEditorGadgetClass::Unknown_672A60()
{
	FAIL_IF(!Slider) {
		return;
	}
	TopLine = Slider->Value;
	Update_Lines(false);
}

// @0x672AD0
void TextEditorGadgetClass::Unknown_672AD0()
{
	if (Slider) {
		Slider->Unknown_66FD80(false);
	}
}

// @0x672B00
void TextEditorGadgetClass::Update_Lines(bool split)
{
	if (split) {
		char *text = Edit->Get_String();
		Split->Split(text, (float)Width, Edit->Cursor - Edit->String);
		delete text;
		CursorLine = Split->CursorLine;
		CursorColumn = Split->CursorColumn;
		LineCount = Split->Count();
		int rows = Lines.size();
		if (LineCount - rows <= 0) {
			TopLine = 0;
			if (Slider) {
				Slider->Unknown_66FA90(0, 0, true);
			}
		} else {
			if (CursorLine < TopLine) {
				TopLine = CursorLine;
			}
			if (CursorLine >= TopLine + rows) {
				TopLine = CursorLine - rows + 1;
			}
			if (Slider) {
				Slider->Unknown_66FA90((float)LineCount, (float)rows, true);
				Slider->Unknown_A4((float)TopLine, false);
			}
		}
	}
	int index = TopLine;
	bool shown = false;
	for (std::vector<RefCountPtr<FontSys> >::iterator it = Lines.begin(); it != Lines.end(); it++) {
		RefCountPtr<FontSys> line = *it;
		line->Change_Text(Split->Get(index), 0);
		if (index == CursorLine) {
			Cursor->Unknown_685AB0(line, CursorColumn, Edit->Get_Insert_Mode(), false);
			shown = true;
		}
		index++;
	}
	if (!shown || Unknown_BE5F24 != this) {
		Cursor->Unknown_685DD0();
	}
}

// @0x672DC0
bool TextEditorGadgetClass::Cursor_Up(bool update)
{
	bool moved = false;
	if (CursorLine) {
		int length = strlen(Split->Get(CursorLine - 1));
		Edit->Move_Cursor(-(length < CursorColumn ? CursorColumn + 1 : length));
		moved = true;
	}
	if (update) {
		Update_Lines(true);
	}
	return moved;
}

#pragma auto_inline(off)
// @0x672E40
bool TextEditorGadgetClass::Cursor_Down(bool update)
{
	bool moved = false;
	if (CursorLine + 1 < LineCount) {
		int rest = strlen(Split->Get(CursorLine)) - CursorColumn;
		int next = strlen(Split->Get(CursorLine + 1));
		Edit->Move_Cursor(CursorColumn >= next ? next + rest : CursorColumn + rest);
		moved = true;
	}
	if (update) {
		Update_Lines(true);
	}
	return moved;
}
#pragma auto_inline(on)

// @0x672F00
bool TextEditorGadgetClass::Page_Up()
{
	int count = Lines.size() - 1;
	while (count--) {
		Cursor_Up(false);
	}
	return Cursor_Up(true);
}

// @0x672FF0
void TextEditorGadgetClass::Page_Down()
{
	int count = Lines.size() - 1;
	while (count--) {
		Cursor_Down(false);
	}
	Cursor_Down(true);
}

// @0x673040
bool TextEditorGadgetClass::Cursor_Home()
{
	bool moved;
	if (CursorColumn) {
		moved = Edit->Move_Cursor(-CursorColumn);
		CursorColumn = 0;
	} else {
		moved = Edit->Set_Cursor(0);
		CursorLine = 0;
	}
	if (moved) {
		Update_Lines(false);
		return true;
	}
	return false;
}

// @0x6730B0
bool TextEditorGadgetClass::Cursor_End()
{
	const char *line = Split->Get(CursorLine);
	if (line) {
		int rest = strlen(line) - CursorColumn - 1;
		if (rest) {
			if (Edit->Move_Cursor(rest)) {
				CursorColumn += rest;
				Update_Lines(false);
				return true;
			}
		} else {
			if (Edit->Set_Cursor(Edit->Get_Max_Len())) {
				Update_Lines(true);
				return true;
			}
		}
	}
	return false;
}

// @0x673150
void TextEditorGadgetClass::Take_Focus()
{
	if (Unknown_BE5F24 != this && Unknown_BE5F24 == NULL) {
		if ((Unknown_BE5F34 & 0xF) == 0) {
			Unknown_BE5F34 = 1;
		}
		Unknown_68();
		Unknown_A8 = Unknown_42EE00(true);
		if (Unknown60) {
			Edit->Clear();
			Unknown60 = false;
		}
		Update_Lines(true);
		Unknown_672A60();
	}
}

// @0x673240
void TextEditorGadgetClass::Drop_Focus()
{
	if (Unknown_BE5F24 == this) {
		Unknown_6C();
		Unknown_42EE00(Unknown_A8);
	}
}

// @0x673280
bool TextEditorGadgetClass::Has_Focus()
{
	return Unknown_BE5F24 == this;
}

// @0x6732A0
bool TextEditorGadgetClass::Unknown_6732A0()
{
	if (Unknown_BE5F24 != this) {
		Unknown_BE5F34 = 1;
		Unknown_70();
		Unknown_68();
		Unknown_A8 = Unknown_42EE00(true);
		return true;
	}
	return false;
}

// NOT EXACT: ebx and ebp swapped (length / line), one fcomp scheduled after the column's copy
// @0x6732F0
void TextEditorGadgetClass::Unknown_6732F0(float x, float y)
{
	const Matrix3D &tm = Unknown_66C6C0();
	float top = tm[1][3];
	float left = tm[0][3];
	float sx = x * 640.0f - 320.0f;
	float sy = 240.0f - y * 480.0f;
	float px = sx - left;
	float py = top - sy;
	int count = Split->Count();
	int row = py / LineHeight + TopLine;
	int length = 0;
	if (count == 0) {
		row = 0;
	} else if (row >= count - 1) {
		row = count - 1;
	}
	const char *line = Split->Get(row);
	if (line) {
		length = strlen(line);
	}
	RefCountPtr<FontSys> measure = Create_Get(new FontSys(Font.c_str(), 0, NULL));
	int column = 0;
	char buffer[1025];
	for (int i = 1; i < length; i++) {
		float width;
		float height;
		strncpy(buffer, line, i);
		buffer[i] = '\0';
		measure->Change_Text(buffer, 0);
		measure->Unknown_6D5B10(width, height);
		column = i;
		if (px - Unknown_B0 <= width) {
			break;
		}
	}
	CursorLine = row;
	CursorColumn = column;
	length = column;
	for (int j = 0; j < row; j++) {
		const char *text = Split->Get(j);
		if (text) {
			length += strlen(text);
		}
	}
	Edit->Set_Cursor(length);
	Update_Lines(false);
}

#pragma auto_inline(off)
// NOT EXACT: registers (the client hoists the load of Client between the test and the branch; x and y in edx and ecx)
// @0x6735A0
bool TextEditorGadgetClass::Unknown_6735A0(float x, float y)
{
	if (Client) {
		UIViewClass *view;
		if (Client->Unknown_744B60()) {
			view = Client->Unknown135C;
		} else {
			view = Client->MainView;
		}
		if (view && view->Get_Unknown34() && view->Get_Unknown34()->Unknown_5898F0(x, y)) {
			return true;
		}
	}
	return false;
}
#pragma auto_inline(on)

/*
**	Reconstruction notes (gd_text_editor.cpp, client 0x671300-0x673EB0; __FILE__ 0xB8D6FC, FAIL_IF '!Slider' on the
**	client's line 361, D25: blank lines keep it there).
**
**	The object's bounds: <string>'s initialiser (workflow.md 3.3) ends the object before at 0x6712D0 and this one at
**	0x673EB0, with none between, and every gadget object of the range carries one. So Unknown_671300Class (0x671300-0x671790,
**	table 0xAFB278, no string of its own) sits in this object: D137, the placement is evidence of bounds only, the header
**	unknown_671300.h is interim. The table 0xAFB3D4 follows 0xAFB278 in .rdata.
**
**	The rest of the object: std::vector<RefCountPtr<FontSys> >'s instances 0x673620-0x673E80 and the initialiser.
*/
