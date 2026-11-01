// Earth & Beyond client (reconstruction): app\client\textgadg.cpp. Notes after the declarations.
#include <windows.h>
#include <string.h>
#include "textgadg.h"
#include "..\common\stredit.h"
#include "fontmanager.h"
#include "font32bit.h"
#include "..\kernel\kernel.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "layer.h"
#include "scene.h"
#include "ww3d.h"
#include "..\shared\unknown_debug.h"

// fontsys_cursor.cpp's text cursor (0xFC bytes, a render object: 0x685680 makes it), as far as this file uses it
class TextCursorClass : public RenderObjClass  // NAME 1: neutral (D10): what it is
{
public:
	void Unknown_685A30(const RefCountPtr<SceneClass> &scene);			// 0x685A30 (into the scene)  // NAME 1 [scene]: what 0x6AB0F0 passes
	void Unknown_685AB0(const RefCountPtr<FontStyleClass> &text, int cursorpos, bool insert, bool flag);	// 0x685AB0 (at the text's cursor position)  // NAME 1 [text, cursorpos, insert, flag]: what 0x6AAD10 passes
	void Unknown_685DD0();												// 0x685DD0 (hidden)
};
RefCountPtr<TextCursorClass> Unknown_685680(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &unknown = NULL);	// 0x685680 (cdecl; the new cursor)  // NAME 1 [iface, unknown]: what 0x6AA810 passes

void Unknown_42EE00(bool on);					// 0x42EE00 (input.cpp: the keyboard's text mode; main_client.cpp declares it with an int)  // NAME 1 [on]: neutral
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300  // NAME 1 [robj]: as ui_avatr.cpp's

typedef char Unknown_RefCountPtr_Scene_Size[sizeof(RefCountPtr<SceneClass>)];	// NAME 1: not the client's: instantiates RefCountPtr<SceneClass> so its friend Create_Peek (0x514960) is visible (VC6 injects a template's friends on instantiation, as sclient.cpp)

TextBaseGadgetClass *TextBaseGadgetClass::_ActiveUI = NULL;

/*
**	Reconstruction notes (textgadg.cpp, client 0x6AA000-0x6AB310; __FILE__ 0xB91874, asserts on the client's lines
**	194-564, D25: blank lines keep them there).
**
**	TextBaseGadgetClass (0x6AA000-0x6AA810): the text in StrEditClass Edit (stredit.cpp, not rebuilt: its constructor,
**	destructor, Insert, Get_String and Process_Key are stand-ins in the POC); Hidden holds a star per character for
**	the password box. Slot +0x74 moves the input focus ('_ActiveUI', 0xBE66C0) and the keyboard's text mode
**	(input.cpp's 0x42EE00, a bool here). Slot +0x2C takes the key events (mask 0x100) while active: tab to Next (or
**	Unknown39 when none), enter, escape (Unknown3A), the rest to Edit (Unknown3B, Changed).
**	TextGadgetClass (0x6AA810-0x6AB190): a FontStyleClass (fontmanager.h, 0x4E8 bytes) named string_name on the model
**	(or its top container), the box of the model's sub-object named by the style's Attach, a cursor
**	(fontsys_cursor.cpp's TextCursorClass, 0x685680). 0x6AAD10 runs every frame: on Changed the text redrawn
**	(clipped to the box around the cursor, 0x6AB190; with flag 0x400 scrolled to fit the screen-scaled box), the
**	cursor moved; while active the cursor blinks at GetCaretBlinkTime (flag 0x1000).
**	The flags are extern constants in .rdata 0xB08C90-0xB08CC4 (1 ... 0x1000, 0x2C0): their file is not known; the POC
**	defines them (programs/clientpoc/poc_data.cpp).
**	0x6AB320-0x6AB5B0 after this file ('TOOLTIPS', 'Interface_Tooltip_Delay', calls into tooltip.cpp) are not this
**	file's: tooltip.cpp's start or a file between.
*/

TextBaseGadgetClass::TextBaseGadgetClass(int length, unsigned int flags, int unknown) :
	GadgetClass(0x100),
	Edit(NULL),
	Hidden(NULL),
	Changed(false),
	Unknown39(false),
	Unknown3A(false),
	Unknown3B(false),
	Active(false),
	Next(NULL)
{
	Edit = new StrEditClass(length, NULL, flags, unknown);
	Hidden = new StrEditClass(length, NULL, Unknown_B08CC4, 0);
}

TextBaseGadgetClass::~TextBaseGadgetClass()
{
	if (_ActiveUI == this) {
		Unknown_74(false);
	}
	delete Edit;
	delete Hidden;
}

// NOT EXACT: the key-up test (0.65): the client reads the event's flags as a bit-field ((Flags >> 12) & 1 in a register),
// VC6 folds this form to a byte test; kernel.h's KernelEventClass has a plain unsigned short
bool TextBaseGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if ((mask & 0x100) && Active) {
		if (!((event->Flags >> 12) & 1)) {
			switch (event->Key) {
				case 9:
					SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
					Unknown_74(false);
					if (Next != NULL) {
						Next->Unknown_74(true);
					} else {
						Unknown39 = true;
					}
					break;
				case 0x0D:
					SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
					Unknown_74(false);
					break;
				case 0x1B:
					SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
					Unknown_74(false);
					Unknown3A = true;
					break;
				default:
					SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
					if (Edit->Process_Key(event)) {
						Unknown3B = true;
						Changed = true;
					}
					break;
			}
		}
		GadgetClass::Unknown_2C(event, x, y, mask);
		return(true);
	}
	return(false);
}







































































// NOT EXACT: registers (0.77): the inlined stars (0x6AA5F0) and the cursor offset take other registers
void TextBaseGadgetClass::Unknown_6AA340(char *print, int s_size, int *cursorpos, bool *insert, const char *prompt, bool password)
{
	int dummy;
	if (cursorpos == NULL) {
		cursorpos = &dummy;
	}
	G_ASSERT(print);
	G_ASSERT(cursorpos);
	if (prompt != NULL) {


		G_ASSERT((unsigned)s_size > strlen(prompt));
		strcpy(print, prompt);
	} else {
		*print = '\0';
	}
	*cursorpos = strlen(print);
	StrEditClass *edit = Edit;
	if (password) {
		Unknown_6AA5F0();
		edit = Hidden;
	}
	char *text = edit->Get_String();
	if (text != NULL) {
		strncat(print, text, s_size - strlen(print) - 1);
		*cursorpos += Edit->Cursor - Edit->String;
		delete text;
	}
	if (insert != NULL) {
		*insert = Edit->InsertMode;
	}
}

void TextBaseGadgetClass::Set_Text(const char *text, bool append)
{
	Changed = true;
	if (!append) {
		Edit->Clear();
	}
	if (text != NULL && *text != '\0') {
		Edit->Insert(text);
	}
}

void TextBaseGadgetClass::Unknown_6AA5F0()
{
	char stars[256];
	int len = strlen(Edit->String);
	if (len > 0) {
		memset(stars, '*', len);
	}
	stars[len] = '\0';
	Hidden->Clear();
	Hidden->Insert(stars);
}

int TextBaseGadgetClass::Get_Text_Length()
{
	return(Edit->Get_Length());
}

char *TextBaseGadgetClass::Get_Text()
{
	return(Edit->Get_String(Edit->Get_Length() + 1));
}

char *TextBaseGadgetClass::Get_Hidden_Text()
{
	return(Hidden->Get_String(Hidden->Get_Length() + 1));
}




























void TextBaseGadgetClass::Unknown_74(bool on)
{
	if (Active != on) {
		Active = on;
		if (on) {
			FAIL_IF(_ActiveUI == this) {
			} else if (_ActiveUI != NULL) {
				_ActiveUI->Unknown_74(false);
			}
			_ActiveUI = this;
			if (!Unknown3B) {
				Edit->Clear();
			}
		} else if (_ActiveUI == this) {
			_ActiveUI = NULL;
		}
		Unknown_42EE00(Active);
		Changed = true;
	}
}





























TextGadgetClass::TextGadgetClass(InterfaceClass *iface, int length, const char *string_name, RenderObjClass *model, unsigned int flags, int unknown) :
	TextBaseGadgetClass(length, flags, unknown),
	Text(NULL),
	Model(Unknown_497300(model)),
	Cursor(NULL),
	Unknown50(' '),
	Unknown51(' '),
	Unknown54(0),
	Unknown58(1.0f),
	Unknown5C(false),
	Unknown78(0),
	Flags(flags)
{
	G_ASSERT(string_name && *string_name);
	Cursor = Unknown_685680(iface);
	if (!(Flags & Unknown_B08CBC)) {
		while (model->Get_Container() != NULL) {
			model = model->Get_Container();
		}
	}
	Text = Create_NEW(new FontStyleClass(string_name, (int)model, 0));
	RenderObjClass *sub = model->Get_Sub_Object_By_Name(Text->Info.Attach);
	Box = sub->Get_Bounding_Box();
	sub->Release_Ref();
	if (Flags & Unknown_B08CB0) {
		Vector3 pos = Text->Get_Position();
		Unknown78 = pos.X - (Box.Center.X - Box.Extent.X);
	} else {
		Unknown78 = 0;
	}
}

TextGadgetClass::~TextGadgetClass()
{
	if (Active) {
		Unknown_74(false);
	}
	Text = NULL;
}

void TextGadgetClass::Unknown_74(bool on)
{
	if (!on) {
		Cursor->Unknown_685DD0();
	}
	TextBaseGadgetClass::Unknown_74(on);
}







// NOT EXACT: registers and stack (0.56; 0x128 bytes of locals in the client, 0x130 here): the client keeps the text
// in ebx and converts the screen width for every width call (here once, kept on the stack)
void TextGadgetClass::Unknown_6AAD10()
{
	bool changed = Changed;
	Changed = false;
	if (changed) {
		Font32Class *&font = Text->Font;
		G_ASSERT(font);
		if (font == NULL) {
			return;
		}
		int max = Edit->MaxLen;



		FAIL_IF(max >= 255) {
			max = 255;
		}
		char print[256];
		int cursorpos;
		bool insert;
		Unknown_6AA340(print, max + 1, &cursorpos, &insert, NULL, Unknown5C);
		char *string = print;
		if (Model) {
			string = Unknown_6AB190(print, Model->Get_Bounding_Box().Extent.X * Unknown58 * 2.0f, font, &cursorpos);
		}
		if (Flags & Unknown_B08CB8) {
			if (Active) {
				int width;
				int height;
				int bits;
				bool windowed;
				WW3D::Get_Device_Resolution(width, height, bits, windowed);
				float text_width = font->Get_Text_Width(print, (float)width);
				float max_width = (Box.Extent.X * 2.0f - Unknown78 * 2.0f) * (width / 640.0f);
				if (text_width >= max_width) {
					char *p = print;
					while (font->Get_Text_Width(p++, (float)width) > max_width && *p != '\0') {
					}
					if (*p-- != '\0') {
						memmove(print, p, strlen(p) + 1);
					}
				}
			}
			Vector3 pos = Text->Get_Position();
			Text->Unknown_6D88F0(Box.Center.X - Box.Extent.X + Unknown78, pos.Y + Box.Extent.Y, Box.Extent.X + Box.Center.X - Unknown78, pos.Y - Box.Extent.Y);
		}
		Text->Change_Text(string, 0);
		Unknown54 = 0;
		Cursor->Unknown_685AB0(Text, cursorpos, insert, (Flags & Unknown_B08CAC) != 0);
	}
	if (Active) {
		if (Flags & Unknown_B08CC0) {
			unsigned int now = GetTickCount();
			if (now - Unknown54 > GetCaretBlinkTime()) {
				if (!Cursor->Is_Hidden() && Unknown54 != 0) {
					Cursor->Set_Hidden(true);
				} else {
					Cursor->Set_Hidden(false);
				}
				Unknown54 = now;
			}
		} else {
			Text->Unknown_6CE910(true);
		}
	} else {
		Text->Unknown_6CE910(false);
		Cursor->Unknown_685DD0();
	}
}

// NOT EXACT: the box copy (0.29): the client copies the six floats one by one, as an AABoxClass return would
// (0x660E40 builds one: centre and half extents); gadget.h's GadgetUnknown64Struct is six ints (copied by rep movsd)
GadgetUnknown64Struct TextGadgetClass::Unknown_64()
{
	if (Text) {
		return(*(const GadgetUnknown64Struct *)&Text->Get_Bounding_Box());
	}
	return(GadgetClass::Unknown_64());
}

void TextGadgetClass::Unknown_28(LayerClass *layer)
{
	Cursor->Unknown_685A30(Create_Peek(layer->Scene));
}































































// NOT EXACT: registers (0.73): the string and the cursor take other registers in the first loop
char *TextGadgetClass::Unknown_6AB190(char *string, float pwidth, Font32Class *&font, int *cp)
{
	FAIL_IF(pwidth <= 0.0f) {
		pwidth = 1.0f;
	}
	float width = font->Get_Text_Width(string, 640.0f);
	if (width > pwidth) {
		int i = strlen(string) - 1;
		while (*cp < i) {
			width -= font->Get_Char_Width(string[i--], 640.0f);
			if (width <= pwidth) {
				break;
			}
		}
		string[i + 1] = '\0';
		while (width > pwidth) {
			width -= font->Get_Char_Width(*string++, 640.0f);
			(*cp)--;
		}






		G_ASSERT(*cp >= 0);
	}
	return(string);
}
