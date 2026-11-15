// Earth & Beyond client (reconstruction): app\common\nav_type_enum.h

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAV_TYPE_ENUM_H  // NAME 3 [nav_type_enum.h]: header name assumed from nav_type_enum.cpp
#define __NAV_TYPE_ENUM_H

enum NavType {			// NAME 5: client Type_Name 'NavType'
	NAV_TYPE_INVALID = -1,	// NAME 5: FAIL_IF 'v <= NAV_TYPE_INVALID'
	NAV_TYPE_NONE = 0,		// NAME 3: table 'None'
	NAV_TYPE_PATH = 1,		// NAME 3: table 'Path'
	NAV_TYPE_END = 2,		// NAME 3: table 'End'
	NAV_TYPE_COUNT = 3		// NAME 5: FAIL_IF 'v >= NAV_TYPE_COUNT'
};

const char *Type_Name(NavType v);						// 0x846A50
bool String_To_Value(const char *text, NavType *v);		// 0x846A70
char *Value_To_String(char *buf, int size, NavType v);	// 0x846AE0

#endif //__NAV_TYPE_ENUM_H
