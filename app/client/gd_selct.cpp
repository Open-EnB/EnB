// Earth & Beyond client (reconstruction): gd_selct.cpp. Notes at the end of the file.
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gd_selct.h"
#include "sclient.h"
#include "rect3d.h"
#include "fontmanager.h"
#include "font32bit.h"
#include "vector3.h"
#include "vector4.h"
#include "tasks\tsk_login.h"
#include "..\common\stredit.h"
#include "..\input\input.h"
#include "..\kernel\kernel.h"
#include "..\shared\g_assert.h"

extern const int Unknown_B08C98;			// 0xB08C98 (4: StrEditClass's digits; textgadg.h's flag constants)  // NAME 1: neutral (D10), as ui_log.cpp
extern const int Unknown_B08CA8;			// 0xB08CA8 (0x40: enter)  // NAME 1: neutral (D10), as ui_log.cpp
extern const int Unknown_B08CAC;			// 0xB08CAC (0x80)  // NAME 1: neutral (D10), as textgadg.h


SelectGadgetClass::SelectGadgetClass(UIViewClass *view, int max) :
	RectangleGadgetClass(0, 0, 1, 1, 0x1FF),
	Finished(false),
	Armed(false),
	Max(max),
	Value(0),
	Edit(NULL),
	Text(NULL),
	TextX(0.0f),
	Bar(NULL),
	Frame(NULL),
	Back(NULL),
	TextBack(NULL),
	View(view)
{
	Unknown_65FFA0(-1000);
	Edit = new StrEditClass(10, NULL, Unknown_B08CAC | Unknown_B08CA8 | Unknown_B08C98, NULL);
	Bar = new Rect3DClass(Vector4(0.0f, 0.8f, 0.3f, 1.0f), Vector4(0.1f, 0.1f, 0.1f, 0.1f));
	Frame = new Rect3DClass(Vector4(0.1f, 1.0f, 0.1f, 1.0f), Vector4(0.1f, 0.1f, 0.1f, 0.1f));
	Back = new Rect3DClass(Vector4(0.1f, 0.1f, 0.1f, 1.0f), Vector4(0.1f, 0.1f, 0.1f, 0.1f));
	TextBack = new Rect3DClass(Vector4(0.1f, 0.1f, 0.1f, 1.0f), Vector4(0.1f, 0.1f, 0.1f, 0.1f));
	Text = new FontSys("GD_SELECT_FONT", NULL, NULL);
	Text->Set_Sort_Level(4);
	Bar->Set_Sort_Level(4);
	Bar->Disable_Sort();
	Back->Set_Sort_Level(5);
	Back->Disable_Sort();
	TextBack->Set_Sort_Level(5);
	TextBack->Disable_Sort();
	Frame->Set_Sort_Level(6);
	Frame->Disable_Sort();
}


SelectGadgetClass::~SelectGadgetClass()
{
	Unknown_40(false);
	delete Edit;
	Edit = NULL;
	REF_PTR_RELEASE(Bar);
	REF_PTR_RELEASE(Frame);
	REF_PTR_RELEASE(Back);
	REF_PTR_RELEASE(TextBack);
	if (Text) {
		Text->Remove();
		Text->Release_Ref();
		Text = NULL;
	}
}


bool SelectGadgetClass::Unknown_30(float x, float y)
{
	return(Unknown2C_05 && !Finished);
}

// NOT EXACT (0.73, attempts.py 0x66D1E0): the client pushes edi with ebp after the first test and loads the event after them; the rest matches
bool SelectGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!Unknown2C_05 || Finished) {
		return(false);
	}
	if (mask & 1) {
		Armed = true;
	} else if (mask & 4) {
		if (Armed) {
			Finished = true;
		}
	} else if (mask & 0x10) {
		Armed = true;
	} else if (mask & 0x40) {
		if (Armed) {
			Value = 0;
			Finished = true;
		}
	} else if (mask & 0x100) {
		switch (event->Key) {
			case 0x0D:
				if ((bool)event->Up) {
					if (Armed) {
						Finished = true;
					}
				} else {
					Armed = true;
				}
				break;
			case 0x1B:
				if ((bool)event->Up) {
					if (Armed) {
						Value = 0;
						Finished = true;
					}
				} else {
					Armed = true;
				}
				break;
			default: {
				Edit->Process_Key(event);
				char *text = Edit->Get_String();
				if (text) {
					int num = atoi(text);
					if (num > Max) {
						do {
							num /= 10;
							text[strlen(text) - 1] = 0;
						} while (num > Max);
						Edit->Clear();
						Edit->Insert(text);
					}
					delete [] text;
				}
				break;
			}
		}
	}
	GadgetClass::Unknown_2C(event, x, y, mask);
	if (Armed) {
		if (Finished) {
			Unknown_40(false);
		}
		return(true);
	}
	float width;
	char *text = Edit->Get_String();
	if (text && *text) {
		Value = atoi(text);
		width = ((float)Value / (float)Max) * Width;
	} else {
		width = Unknown_42EC90() * 640.0f - 320.0f - X;
		if (width < 0.0f) {
			width = 0.0f;
		} else if (width > Width) {
			width = Width;
		}
		Value = (int)(width / Width * Max);
	}
	delete [] text;
	Bar->Set_Rect(Vector4(X, Y, width, Height));
	Text->Unknown_6CE420(0, "%d", Value);
	Text->Set_Position(Vector3(TextX, Y, 0.0f));
	return(true);
}















































































































































































void SelectGadgetClass::Unknown_40(bool on)
{
	if (on) {
		Finished = false;
		Armed = false;
		Edit->Clear();
		X = 0.0f;
		Y = 0.0f;
		LoginUnknown18Class *mouse = (LoginUnknown18Class *)View->Unknown_44();
		if (mouse) {
			X = mouse->Unknown1C;
			Y = mouse->Unknown20;
			X += mouse->Unknown14;
			Y += mouse->Unknown18;
		} else {
			X = Unknown_42EC90();
			Y = Unknown_42ECC0();
		}
		X = X * 640.0f - 320.0f;
		Y = 240.0f - Y * 480.0f;
		int digits = 0;
		for (int num = Max; num != 0; digits++) {
			num /= 10;
		}
		RefCountPtr<Font32Class> &font = Text->Font;
		float char_width;
		if (font) {
			char_width = (font->Get_Char_Width('5', 640.0f) + 2.0f) * digits;
		} else {
			G_ASSERT(font);
			char_width = 8.0f;
		}
		Width = (float)Max;
		Height = font->Get_Char_Height(480.0f) + 1.0f;
		if (Width < char_width * 3.0f) {
			Width = char_width * 3.0f;
		}
		if (Width > 192.0f) {
			Width = 192.0f;
		}
		float edge = 320.0f - char_width;
		if (X > edge) {
			X = edge;
		}
		X -= Width;
		if (X < -edge) {
			X = -edge;
		}
		if (Y > Height) {
			Y -= Height;
		}
		if (Y > 480.0f - Height) {
			Y = 480.0f - Height;
		}
		X = (float)floor(X);
		Y = (float)floor(Y);
		Bar->Set_Rect(Vector4(X, Y, Width, Height));
		Frame->Set_Rect(Vector4(X - 1.0f, Y + 1.0f, char_width + Width + 3.0f, Height + 2.0f));
		Back->Set_Rect(Vector4(X, Y, Width, Height));
		TextX = Width + X + 1.0f;
		TextBack->Set_Rect(Vector4(TextX, Y, char_width, Height));
		TextX += 1.0f;
		Text->Unknown_6CE420(2, "%d", Max);
		Value = Max;
		if (!Text->Peek_Scene()) {
			Text->Add((SceneClass *)View->Unknown_50());
		}
		if (!Bar->Peek_Scene()) {
			Bar->Add((SceneClass *)View->Unknown_50());
		}
		if (!Back->Peek_Scene()) {
			Back->Add((SceneClass *)View->Unknown_50());
		}
		if (!TextBack->Peek_Scene()) {
			TextBack->Add((SceneClass *)View->Unknown_50());
		}
		if (!Frame->Peek_Scene()) {
			Frame->Add((SceneClass *)View->Unknown_50());
		}
	} else {
		if (Frame->Peek_Scene()) {
			Frame->Remove();
		}
		if (TextBack->Peek_Scene()) {
			TextBack->Remove();
		}
		if (Back->Peek_Scene()) {
			Back->Remove();
		}
		if (Bar->Peek_Scene()) {
			Bar->Remove();
		}
		if (Text->Peek_Scene()) {
			Text->Remove();
		}
	}
	GadgetClass::Unknown_40(on);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_selct.cpp (the client's own string: G_ASSERT 'font' l.369 of
**	0x66D520; GD_SELECT_FONT 0xB8D3A4)
**
**	The quantity selector (gd_selct.h): 0x66CC00-0x66D911, table 0xAFADD4; after it int3 padding to 0x66DA20 and the
**	headers' static initialiser 0x66DA20 (atexit 0x42B320, as gd_scut_proxy.cpp's), then gd_ship_equip_pip.cpp.
**	The flags of the edit are textgadg.h's constants 0xB08CAC | 0xB08CA8 | 0xB08C98 (0xC4), loaded, not folded.
**	The rectangles are WW3D-refcounted (REF_PTR_RELEASE); the text is taken out of its scene before its release.
**	Slot +0x2C reads the key's release bit as a bool (mov / shr 12 / test 1); a plain test of the bit-field or of
**	Flags & 0x1000 gives test byte ptr [edi + 3], 0x10. Slot +0x40's digit count divides first, then counts.
*/
