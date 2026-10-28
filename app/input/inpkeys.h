/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown. The object has no __FILE__ string; it is linked between
**	app\input\inpdsp.cpp and app\input\input.cpp (alphabetical in that folder), so the name
**	inpkeys.h / inpkeys.cpp is provisional (D10).
**
**	The key names: two SGI hash_maps (D87) filled once by 0x42BC90, a key's name -> its code
**	(0xBDA940) and a code -> its name (0xBDA928); inpdsp.cpp's ini reader and writer and the
**	control keys manager turn keys into text and back through them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INPKEYS_H  // NAME 2 [inpkeys.h]: provisional: the object has no __FILE__ (it sits between inpdsp.cpp and input.cpp)
#define __INPKEYS_H

#include <string>
#include "..\kernel\kernel.h"

unsigned int Unknown_42B610(const std::string &name);							// 0x42B610: a key's code from its name (0: none)  // NAME 1 [Unknown_42B610]: neutral (D10)
const std::string &Unknown_42B6A0(unsigned int key);								// 0x42B6A0: a key's name from its code ("": none)  // NAME 1 [Unknown_42B6A0]: neutral (D10)
std::string &Unknown_42B7E0(const KernelEventClass &event, std::string &name);	// 0x42B7E0: an event's text, "Ctrl-", "Alt-", "Shift-" and the key's name  // NAME 1 [Unknown_42B7E0]: neutral (D10)
void Unknown_42BAE0(unsigned int key, const char *name);							// 0x42BAE0: enters a key in both tables  // NAME 1 [Unknown_42BAE0]: neutral (D10)
void Unknown_42BC90();																// 0x42BC90: fills the tables (once, on the first lookup)  // NAME 1 [Unknown_42BC90]: neutral (D10)

#endif //__INPKEYS_H
