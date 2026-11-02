// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\local_interface_sort.h (name assumed from
**	local_interface_sort.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\local_interface_sort.cpp';
**	the client holds no string of this header)
**
**	The sort scope (8 bytes): while it lives, the render objects added to the interface scene go to one
**	interface's sort list (the static sort list's current interface); its destructor puts the previous one back.
**	The member name SortList is the asserts' ('!SortList'); the rest are neutral (D10).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __LOCAL_INTERFACE_SORT_H  // NAME 3 [local_interface_sort.h]: header name assumed from the client's local_interface_sort.cpp
#define __LOCAL_INTERFACE_SORT_H

#include "..\..\wwlib\ref_ptr.h"

class InterfaceClass;
class InterfaceStaticSortListClass;
class InterfaceSceneClass;
class SceneClass;
class UIViewClass;
class RenderObjClass;

class InterfaceSortClass  // NAME 1: neutral (D10; intrface.cpp's and fontsys_cursor.cpp's name)
{
public:
	InterfaceSortClass(InterfaceClass *ui);					// 0x6E0E80 (FAIL_IF '!ui')  // NAME 2 [ui]: the FAIL_IF's
	InterfaceSortClass(InterfaceStaticSortListClass *sort_list, bool front);	// 0x6E1090 (the front or the back list)  // NAME 1 [sort_list, front]: neutral
	InterfaceSortClass(InterfaceSceneClass *ui_scene, bool front);		// 0x6E1200 (FAIL_IF '!ui_scene')  // NAME 2 [ui_scene]: the FAIL_IF's  // NAME 1 [front]: neutral
	InterfaceSortClass(SceneClass *castable_ui_scene, bool front);		// 0x6E1390 (FAIL_IF '!castable_ui_scene')  // NAME 2 [castable_ui_scene]: the FAIL_IF's  // NAME 1 [front]: neutral
	InterfaceSortClass(UIViewClass *uiview, bool front);				// 0x6E1560 (FAIL_IF '!uiview')  // NAME 2 [uiview]: the FAIL_IF's  // NAME 1 [front]: neutral
	InterfaceSortClass(const RefCountPtr<RenderObjClass> &robj);		// 0x6E1730 (the interface the object was sorted into)  // NAME 1 [robj]: neutral
	~InterfaceSortClass();									// 0x6E18B0

	void Unknown_6E1940(bool front);						// 0x6E1940 (onto the front or the back list; the previous one kept)  // NAME 1 [front]: as the constructors'

	InterfaceStaticSortListClass *SortList;					// +0x00 (the interface's view's scene's static sort list)  // NAME 2: the asserts' '!SortList'
	RefCountPtr<InterfaceClass> Previous;					// +0x04 (the current interface before this scope)  // NAME 1: neutral
};

// The windows' sort scope over an interface (0x541980) or a RefCountPtr to one (0x5419B0), emitted in intrface.cpp:
// a constructor that only passes its argument to InterfaceSortClass's (the client's 90 windows call 0x541980)
class InterfaceSortScopeClass : public InterfaceSortClass  // NAME 1: neutral (D10)
{
public:
	InterfaceSortScopeClass(InterfaceClass *ui);			// 0x541980  // NAME 2 [ui]: InterfaceSortClass's
	InterfaceSortScopeClass(const RefCountPtr<InterfaceClass> &ui);	// 0x5419B0  // NAME 2 [ui]: InterfaceSortClass's
};

#endif
