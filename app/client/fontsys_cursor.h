// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\fontsys_cursor.h (PROVISIONAL, after fontsys_cursor.cpp's
**	__FILE__ 'e:\build\beta\G\app\Client\fontsys_cursor.cpp' 0xB8FD0C; the client holds no string of this header)
**
**	fontsys_cursor.cpp's TextCursorClass: the edit boxes' text cursor, a Rect3DClass (0xFC bytes, vtable
**	0xAFC53C) drawn into a scene at a FontStyleClass line's cursor position: an underline when inserting,
**	else a bar the line's height. textgadg.cpp's TextGadgetClass holds one (0x685680 makes it).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FONTSYS_CURSOR_H  // NAME 3 [fontsys_cursor.h]: the header of fontsys_cursor.cpp
#define FONTSYS_CURSOR_H

#include "rect3d.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\scene.h"

class InterfaceClass;
class FontStyleClass;

class TextCursorClass : public Rect3DClass  // NAME 1 [TextCursorClass]: neutral (D10): what it is
{
public:
	TextCursorClass(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &unknown);	// 0x685860 (inlined in 0x685680; ret 8, unknown unused)  // NAME 1 [iface, unknown]: what it stores; neutral
	virtual ~TextCursorClass();														// 0x685960

	void Unknown_685A30(const RefCountPtr<SceneClass> &scene);						// 0x685A30 (taken out of its scene; the scene it is drawn into)  // NAME 1 [scene]: what 0x6AB0F0 passes
	void Unknown_685AB0(const RefCountPtr<FontStyleClass> &line, int cursorpos, bool insert, bool center);	// 0x685AB0: into the scene, at the line's cursor position  // NAME 2 [line]: the FAIL_IF's '!line'  // NAME 1 [cursorpos, insert, center]: what it does with them
	void Unknown_685DD0();															// 0x685DD0 (hidden: out of its scene)  // NAME 1: neutral (D10)

	RefCountPtr<SceneClass> Scene;													// +0xF4  // NAME 2: what it holds
	InterfaceClass *Iface;															// +0xF8: for the sort scope (local_interface_sort.cpp's 0x6E0E80)  // NAME 2: what it holds
};

RefCountPtr<TextCursorClass> Unknown_685680(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &unknown = NULL);	// 0x685680 (cdecl; the new cursor; unknown unused)  // NAME 1 [iface, unknown]: what 0x6AA810 passes

#endif
