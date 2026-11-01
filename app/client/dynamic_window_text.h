// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_text.h (name assumed from
**	dynamic_window_text.cpp, whose __FILE__ is 0xB76DB8; the client holds no string of this header)
**
**	A dynamic window's line of text: a FontStyleClass shown as the element's render object (table
**	0xAED9CC, 0x50 bytes; constructor 0x51D500, destructor 0x51D600; made by Create 0x51D290).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_TEXT_H  // NAME 3 [dynamic_window_text.h]: header name assumed from the client's dynamic_window_text.cpp
#define DYNAMIC_WINDOW_TEXT_H

#include "dynamic_window_render_object.h"
#include <string>

class FontStyleClass;

class DynamicWindowTextClass : public DynamicWindowRenderObjectClass  // NAME 2 [DynamicWindowTextClass]: class from file (D24)
{
public:
	DynamicWindowTextClass(const RefCountPtr<FontStyleClass> &font, unsigned int flags);			// 0x51D500  // NAME 1 [font, flags]: 'Font'; the base's
	virtual ~DynamicWindowTextClass();																// +0x04 0x51D600 (deleting 0x51D5D0)

	static RefCountPtr<DynamicWindowTextClass> Create(const std::string &font_name, const std::string &text, unsigned int flags);	// 0x51D290 (FAIL_IF 'font_name.empty()', '!font')  // NAME 1 [Create, text]: font_name the FAIL_IF's
	void Set_Text(const std::string &text);															// 0x51D690 (FAIL_IF '!Font'; FontStyleClass::Change_Text)  // NAME 1 [Set_Text, text]

protected:
	RefCountPtr<FontStyleClass> Font;																// +0x4C (FAIL_IF '!Font')
};

#endif
