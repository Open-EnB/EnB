// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_icon.h (name assumed from
**	dynamic_window_icon.cpp, whose __FILE__ is 0xB76BA0; the client holds no string of this header)
**
**	DynamicWindowIconClass: a dynamic window element showing one texture as a centred quad of a given size
**	(table 0xAED4FC: DynamicWindowRenderObjectClass's slots, its own destructor; 0x54 bytes; constructor
**	0x519E60, destructor 0x519F20). The item examination window makes its close button and its title's icon
**	with it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_ICON_H  // NAME 3 [dynamic_window_icon.h]: header name assumed from the client's dynamic_window_icon.cpp
#define DYNAMIC_WINDOW_ICON_H

#include "dynamic_window_render_object.h"
#include <string>

class TextureClass;

class DynamicWindowIconClass : public DynamicWindowRenderObjectClass  // NAME 2 [DynamicWindowIconClass]: class from file (D24), dynamic_window_icon.cpp
{
public:
	DynamicWindowIconClass(const RefCountPtr<RenderObjClass> &render_object, const Vector2 &size, unsigned int flags);	// 0x519E60  // NAME 2 [render_object, size]: Create's FAIL_IF names; NAME 1 [flags]: the base's
	virtual ~DynamicWindowIconClass();																// +0x04 0x519F20 (deleting 0x519EF0)

	static RefCountPtr<DynamicWindowIconClass> Create(const std::string &texture_name, const Vector2 &size, unsigned int flags);	// 0x519A20 (FAIL_IF 'texture_name.empty()', 'size.X <= 0 || size.Y <= 0', '!render_object')  // NAME 1 [Create, flags]: as DynamicWindowTextClass's
	static RefCountPtr<DynamicWindowIconClass> Create(const RefCountPtr<TextureClass> &texture, const Vector2 &size, unsigned int flags);	// 0x519C40 (FAIL_IF '!texture')
	void Set_Texture(const std::string &texture_name);												// 0x519F40 (the base's Set_Render_Object)  // NAME 1 [Set_Texture]: what it does
	void Set_Texture(const RefCountPtr<TextureClass> &texture);									// 0x519FE0 (the base's Replace_Render_Object; the item examination title's icon)  // NAME 1 [Set_Texture]: what it does

	static RefCountPtr<RenderObjClass> Create_Render_Object(const std::string &texture_name, const Vector2 &size);	// 0x51A080 (the texture through the asset manager)  // NAME 1 [Create_Render_Object]: as ToggleButtonDynamicWindow's
	static RefCountPtr<RenderObjClass> Create_Render_Object(const RefCountPtr<TextureClass> &texture, const Vector2 &size);	// 0x51A190 (FAIL_IF '!texture': a two-triangle quad of size)  // NAME 1 [Create_Render_Object]: as ToggleButtonDynamicWindow's

protected:
	Vector2 IconSize;																			// +0x4C (Set_Texture's quad size)  // NAME 1 [IconSize]: what it sizes
};

#endif
