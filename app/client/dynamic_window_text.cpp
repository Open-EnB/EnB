// Earth & Beyond client (reconstruction): app\Client\dynamic_window_text.cpp (__FILE__ 0xB76DB8). Notes at the end of the file.
#include "dynamic_window_text.h"
#include "fontmanager.h"
#include "..\shared\unknown_debug.h"


















RefCountPtr<DynamicWindowTextClass> DynamicWindowTextClass::Create(const std::string &font_name, const std::string &text, unsigned int flags)
{
	FAIL_IF(font_name.empty()) return NULL;

	RefCountPtr<FontStyleClass> font = Create_NEW(new FontStyleClass(font_name.c_str(), 0, text.c_str()));
	FAIL_IF(!font) return NULL;

	return Create_NEW(new DynamicWindowTextClass(font, flags));
}

DynamicWindowTextClass::DynamicWindowTextClass(const RefCountPtr<FontStyleClass> &font, unsigned int flags) :
	DynamicWindowRenderObjectClass(flags),
	Font(font)
{
	Set_Render_Object(font);
}

DynamicWindowTextClass::~DynamicWindowTextClass()
{
}

void DynamicWindowTextClass::Set_Text(const std::string &text)
{
	FAIL_IF(!Font) return;
	Font->Change_Text(text.c_str(), 0);
	Invalidate();
}

/*
**	Reconstruction notes (DW1, 2026-11-01)
**
**	Client 0x51D290-0x51D74D: Create 0x51D290 (FAIL_IF 'font_name.empty()' on line 25, '!font' on 28; the
**	constructor inlined), the constructor 0x51D500, deleting destructor 0x51D5D0, destructor 0x51D600,
**	Set_Text 0x51D690 (FAIL_IF '!Font' on 46), then the template instances Create_NEW<DynamicWindowTextClass>
**	0x51D700 and ~RefCountPtr<FontStyleClass> 0x51D730.
*/
