// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (D10, interim placement D137): the object between input.cpp and w32kbd.cpp
**	(0x42F880-0x430450) has no __FILE__.
**
**	The window-message handler list: the main window's procedure (0x431DE0) asks it first; a
**	handler's result other than -1 is the message's result.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __WINMSG_H	// NAME 1 [winmsg.h]: provisional file name (D10)
#define __WINMSG_H

void Unknown_42F940(int message, int (*handler)(int, int));	// 0x42F940: adds a window-message handler  // NAME 1 [Unknown_42F940, message, handler]: as main_client.cpp
void Unknown_42FAC0(int message, int (*handler)(int, int));	// 0x42FAC0: removes it  // NAME 1 [Unknown_42FAC0, message, handler]: as main_client.cpp
int Unknown_42FBA0(int message, int wparam, int lparam);	// 0x42FBA0: the last handler's result, -1 when none takes it  // NAME 1 [Unknown_42FBA0, message, wparam, lparam]: neutral (D10)

#endif //__WINMSG_H
