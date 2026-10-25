/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\ (file name unknown; see tsk_menu.cpp)
**
**	MenuTask (its strings: 'MenuTask: accepting focus.', 'MenuTask: Starting Login Task.'): the
**	task between the movies and the login. As the focused task it hands itself event 0x43 every
**	cycle, which starts a LoginTaskClass, gives it the focus and removes the MenuTask; event 0x51
**	stops the kernel. Vtable 0xB04228, 0x10 bytes (docs/kernel-map.md 3).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TSK_MENU_H  // NAME 1 [tsk_menu.h]: as tsk_menu.cpp
#define TSK_MENU_H

#include "..\..\kernel\kernel.h"

class MenuTaskClass : public TaskClass		// NAME 3: the client's 'MenuTask' (its log) in LoginTaskClass's form (D24)
{
public:
	MenuTaskClass();									// 0x76BC50 (the implicit destructor 0x76BCF0, scalar deleting 0x76BCC0)
	virtual bool Accept_Focus();						// +0x28 0x76BD10
	virtual bool Yield_Focus();							// +0x2C 0x76BD30: only once the login task is started
	virtual int Get_Interval() { return 0; }			// +0x34 0x76BCA0
	virtual void Pre_Run();								// +0x38 0x76BD70: event 0x43 to Handle_Input
	virtual bool Delete_On_Remove() { return true; }	// +0x44 0x76BC80

	void Handle_Input(KernelEventClass *event);			// 0x76BDB0  // NAME 2: what it does

	bool Done;											// +0x0C NAME 2: the login task is started (Yield_Focus)
};

#endif
