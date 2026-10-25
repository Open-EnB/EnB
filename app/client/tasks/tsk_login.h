/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_login.h (name assumed from tsk_login.cpp)
**
**	LoginTaskClass (the client's name: tsk_login.cpp's asserts), declared only as far as MenuTask
**	uses it: the constructor 0x7675D0 and the size (0x1148). Vtable 0xB040D4 (docs/kernel-map.md 3);
**	tsk_login.cpp isn't rebuilt yet (kernel-map.md 6, step 9).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TSK_LOGIN_H  // NAME 3 [tsk_login.h]: header name assumed from the client's tsk_login.cpp
#define TSK_LOGIN_H

#include "..\..\kernel\kernel.h"

class LoginTaskClass : public TaskClass
{
public:
	LoginTaskClass(int mode);							// 0x7675D0  // NAME 1 [mode]: MenuTask passes 1, or 9 (0x5FAE70 and not 0x5FAD50)
	virtual bool Accept_Focus();						// +0x28 ('LoginTask: accepting focus.')
	virtual int Get_Interval();							// +0x34
	virtual bool Delete_On_Remove();					// +0x44

	char Unknown000C[0x1148 - 0x0C];					// the members, not traced yet
};

#endif
