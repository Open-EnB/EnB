// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown; neutral name, D10)
**
**	E&B's assert macros, compiled in in the release client (no NDEBUG test):
**
**	  G_ASSERT(expr);              expr false: 0x4E9410 reports "ASSERTION FAILED: %s" (category
**	                               "G_ASSERT") and the code goes on.
**	  FAIL_IF(expr) statement;     expr true: 0x4E9450 reports "ERROR: %s" (category "FAIL_IF"), then
**	                               the statement (the recovery) runs: FAIL_IF(buff_size < 1) buff_size = 1;
**
**	Both functions are cdecl (expression text, __FILE__, __LINE__) and pass them on to the same
**	reporter, 0x4BC260("", category, file, line, format, expression) ("" is 0xBDA908). __LINE__ is pushed as an
**	immediate, so every assert in a reconstructed file sits on the client's line (D25).
**
**	Where the header lives: E&B's app\shared objects are linked in alphabetical order, and 0x4E9410
**	and 0x4E9450 lie between event.cpp's functions (to 0x4E8E40) and ini_manager.cpp's (first assert
**	0x4EA5A0), in one object with E&B's global operator new and delete (0x4E9CB0, 0x4E9D10) and a
**	std::map instance (0x4E94F0-0x4E9C40). That object has no __FILE__ string of its own, so its name
**	(and this header's) is unknown: something sorting between "event" and "ini_manager".
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_DEBUG_H  // NAME 1 [unknown_debug.h]: neutral file name (D37, D10): the object has no __FILE__
#define __UNKNOWN_DEBUG_H

void Unknown_Assert_Failed(const char *expr, const char *file, int line);		// 0x4E9410
void Unknown_Fail_If_Report(const char *expr, const char *file, int line);		// 0x4E9450

#define G_ASSERT(expr)	((expr) ? (void)0 : Unknown_Assert_Failed(#expr, __FILE__, __LINE__))
#define FAIL_IF(expr)	if ((expr) ? (Unknown_Fail_If_Report(#expr, __FILE__, __LINE__), 1) : 0)

#endif //__UNKNOWN_DEBUG_H
