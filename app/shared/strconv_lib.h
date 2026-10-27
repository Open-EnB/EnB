/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\strconv_lib.h (name assumed from strconv_lib.cpp)
**
**	Value <-> text conversions for the key/value pairs (kv_pair.h). The client's are in
**	strconv_lib.cpp's object, before its first assert (line 24, 0x4A3750): one pair per type,
**	0x4A30A0-0x4A35F0 (bool "true"/"false", parsing true/on/yes/1 and false/off/no/0; float and
**	double "%f" / atof; int, unsigned int, short, long, unsigned long through sprintf / sscanf).
**	Only the ones the tree uses are declared; the names are guesses (D10). Their definitions are in
**	stubs/app_stubs.cpp until strconv_lib.cpp is rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STRCONV_LIB_H  // NAME 3 [strconv_lib.h]: header name assumed from the client's strconv_lib.cpp (D57)
#define __STRCONV_LIB_H

char *Value_To_String(char *buf, int size, bool value);					// 0x4A30A0 ("true" / "false")  // NAME 2: describes what it does
char *Value_To_String(char *buf, int size, float value);					// 0x4A3200 ("%f")  // NAME 2: describes what it does
bool String_To_Value(const char *text, float *value);						// 0x4A3240 (atof)  // NAME 2: describes what it does
char *Value_To_String(char *buf, int size, unsigned int value);			// 0x4A3590 ("%u")
bool String_To_Value(const char *text, unsigned int *value);			// 0x4A35C0 (sscanf "%u")

// The money pair's are MoneyClass's own (money.cpp, not in the tree: 0x4F0AB0, 0x4F0BE0, 0x4F0640,
// called from the pair 0x4A7600-0x4A7670); stubs/ stands in for them
class MoneyClass;
const char *Type_Name(MoneyClass *);  // NAME 2: describes what it does
char *Value_To_String(char *buf, int size, const MoneyClass &value);
bool String_To_Value(const char *text, MoneyClass *value);

// The pairs' type names (KVPairTemplateClass<T>::Get_Type_Name: float 0x448580, unsigned int 0x4A8830)
inline const char *Type_Name(float *) { return "float"; }
inline const char *Type_Name(unsigned int *) { return "unsigned int"; }

#endif //__STRCONV_LIB_H
