// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_registration.h (PROVISIONAL: name assumed
**	from ui_registration.cpp per D137)
**
**	ui_registration: registration UI dialogs and checks.
**	0x5FAD50: reads the "Registered" bool from the registry key
**	0x5FADE0: reads the "Trial" bool from the registry key
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_REGISTRATION_H  // NAME 3 [ui_registration.h]: header name assumed from ui_registration.cpp
#define UI_REGISTRATION_H

// 0x5FAD50: reads the "Registered" bool from the registry key (ui_registration.cpp)
bool Unknown_5FAD50();

// 0x5FADE0: reads the "Trial" bool from the registry key (ui_registration.cpp)
bool Unknown_5FADE0();

// 0x5FAE70: registration predicate (ui_registration.cpp)
bool Unknown_5FAE70();

#include <string>

// 0x5FB330: strips hyphens from a string (ui_registration.cpp)
std::string __stdcall Unknown_5FB330(std::string str);

// 0x5FB540: strips spaces from a string (ui_registration.cpp)
std::string __stdcall Unknown_5FB540(std::string str);

#endif // UI_REGISTRATION_H
