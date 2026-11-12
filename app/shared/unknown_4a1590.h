// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (no string names the file; unknown_4a1590.h is a neutral name, D10)
**
**	The bounded string copy 0x4A1590, which 40 functions of the client call (packets, the login task,
**	the avatar and ship descriptions, the name tables' Get_Name functions). It is alone in its object,
**	between uvcontrl.cpp's and gclass.cpp's; until a file name appears its definition sits with
**	uvcontrl.cpp (an interim placement, D137; P-M1-5).
**
**	This header includes nothing on purpose: a file that takes it gains no static initialiser.
*/


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_4A1590_H  // NAME 1: neutral header name (D10)
#define __UNKNOWN_4A1590_H

// 0x4A1590: copies string into buffer, at most size - 1 characters, and always ends it with a 0
// (strncpy that never leaves the buffer open). size 0 writes nothing. Returns buffer.
char *Bounded_Strcpy(char *buffer, int size, const char *string);	// NAME 2: what it does (no client string names it)

#endif
