// Earth & Beyond client (reconstruction): app\Client\tooltip.cpp (__FILE__ 0xB91938). Notes at the end of the file.
#include "tooltip.h"
#include "dynamic_window_class.h"
#include "fontmanager.h"
#include "..\shared\unknown_debug.h"

UIViewUnknown30Class::UIViewUnknown30Class(UIViewClass *view) :
	View(view),
	CurGadget(NULL),
	Unknown08(0),
	Unknown0C(false),
	Unknown0D(false),
	Unknown0E(false),
	Unknown0F(false),
	Hides(0),
	Window(),
	Unknown18(0),
	Unknown1C(0)
{
	memset(Lines, 0, sizeof(Lines));
	Window = Create_NEW(new DynamicWindowClass("TOOLTIPS", MAX_LINES));
	Window->Set_Level(2);
	for (int i = 0; i < MAX_LINES; i++) {
		Lines[i] = new FontStyleClass("TOOLTIP_FONT", 0, NULL);
	}
}

UIViewUnknown30Class::~UIViewUnknown30Class()
{
	Unknown_6AB930(false);
	for (int i = 0; i < MAX_LINES; i++) {
		REF_PTR_RELEASE(Lines[i]);
	}
	Window = NULL;
}

/*
**	Reconstruction notes
**
**	The file: tooltip.cpp's __FILE__ (0xB91938) is cited by 0x6AB850 ('!Hides', line 181), 0x6AB8F0
**	('CurGadget != gadget', line 210) and 0x6ABC60 ('lines > MAX_LINES', line 331); the constructor
**	0x6AB320 and the destructor 0x6AB4D0 come before them, with the EH handlers 0xAC14A9 / 0xAC14CB in
**	the same block as 0x6AB930's and 0x6ABC60's (textgadg.cpp's end at 0xAC13xx). Both exact.
**	The rest (0x6AB5B0, 0x6AB850-0x6ABE90) is not here yet; 0x6AB850's assert pins line 181.
*/
