// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\user_click_indicator.h (name assumed from user_click_indicator.cpp)
**
**	The indicator of a clicked object (0x3C bytes), as far as gutter_data.cpp uses it; gadget.cpp keeps its own stand-in
**	declaration of the part it uses. Not rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __USER_CLICK_INDICATOR_H  // NAME 3 [user_click_indicator.h]: header name assumed from the client's user_click_indicator.cpp
#define __USER_CLICK_INDICATOR_H

#include "ref_ptr.h"

class SceneClass;
class SphereClass;
class UIViewClass;

class UserClickIndicatorClass  // NAME 3: class-from-file (D24)
{
public:
	UserClickIndicatorClass();							// 0x6B3A90
	~UserClickIndicatorClass();							// 0x6B3C20
	bool Unknown_6B3D00(UIViewClass *view);				// 0x6B3D00  // NAME 1 [view]: gutter_data.cpp's 0x7B6040's FAIL_IF
	void Unknown_6B3ED0(const RefCountPtr<SceneClass> &scene);	// 0x6B3ED0  // NAME 1 [scene]: gutter_data.cpp's 0x7B58F0
	void Unknown_6B3F50(const SphereClass &sphere);		// 0x6B3F50 (each frame)  // NAME 1 [sphere]: gadget.cpp's
	void Unknown_6B4400();								// 0x6B4400

	char Unknown00[0x3C];
};

#endif //__USER_CLICK_INDICATOR_H
