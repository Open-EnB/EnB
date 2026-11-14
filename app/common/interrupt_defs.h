// Earth & Beyond client (reconstruction): app\common\interrupt_defs.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\interrupt_defs.h (name assumed from interrupt_defs.cpp,
**	whose __FILE__ is 0xBAE148; no assert names the header)
**
**	An equipment interrupt's state ('None', 'Powering Up', 'Interrupted', 'Interrupt Failed', 'Activated',
**	'Deactivated': the names 0xBAE0B8) and its text converters (interrupt_defs.cpp, 0x83D9E0-0x83DAD9).
**	aux_roots.h includes this header for the InterruptState field; it sees only what it declared before
**	(its declaration count moves the SYMBOL_PAD windows, workflow.md 3.4).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef INTERRUPT_DEFS_H  // NAME 3 [interrupt_defs.h]: header name assumed from the client's interrupt_defs.cpp
#define INTERRUPT_DEFS_H

// NAME 5: the client's Get_Type_Name string 'InterruptStateType' (KVPair vtable 0xB01B40, 0x7083B0 inline), leaf vtable 0xB009A8
enum InterruptStateType { UNKNOWN_B009A8_0 };	// vtable 0xB009A8, 4 bytes
inline const char *Type_Name(InterruptStateType *) { return "InterruptStateType"; }
bool String_To_Value(const char *text, InterruptStateType *value);	// 0x83DA80
char *Value_To_String(char *buf, int size, InterruptStateType value);	// 0x83D9E0

#endif
