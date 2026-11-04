// Earth & Beyond client (reconstruction): app\shared\strconv_lib.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strconv_lib.h"
#include "vector3.h"
#include "matrix3d.h"
#include "unknown_debug.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1 [Unknown_4A1590]: neutral placeholder (D10)
char *Unknown_8B44E0(__int64 value, char *buffer);	// 0x8B44E0: _i64toa(value, buffer, 10) (not rebuilt)  // NAME 1: neutral placeholder (D10)
__int64 Unknown_8B44C0(const char *text);	// 0x8B44C0: _atoi64(text) (not rebuilt)  // NAME 1: neutral placeholder (D10)

// The conversions for the wwmath types and __int64 (the client's 0x4A3750-0x4A3B10). The text goes
// through a buffer on the stack and the caller's buffer gets a bounded copy of it; the asserts sit on
// the client's lines (24, 57; D25).



char *Value_To_String(char *buf, int size, const Vector3 &value)
{
	char tmp[128];
	sprintf(tmp, "%f,%f,%f", value.X, value.Y, value.Z);
	G_ASSERT((int)strlen(tmp) < size);
	return Unknown_4A1590(buf, size, tmp);
}

bool String_To_Value(const char *text, Vector3 *value)
{
	return sscanf(text, "%f,%f,%f", &value->X, &value->Y, &value->Z) == 3;
}

// The matrix as its axes and translation (Get_X_Vector .. Get_Translation: the client reads the
// columns), twelve numbers in four groups of three














char *Value_To_String(char *buf, int size, const Matrix3D &value)
{
	char tmp[256];
	Vector3 x = value.Get_X_Vector();
	Vector3 y = value.Get_Y_Vector();
	Vector3 z = value.Get_Z_Vector();
	Vector3 t = value.Get_Translation();
	sprintf(tmp, "%f,%f,%f, %f,%f,%f, %f,%f,%f, %f,%f,%f", x.X, x.Y, x.Z, y.X, y.Y, y.Z, z.X, z.Y, z.Z, t.X, t.Y, t.Z);
	G_ASSERT((int)strlen(tmp) < size);
	return Unknown_4A1590(buf, size, tmp);
}

bool String_To_Value(const char *text, Matrix3D *value)
{
	Vector3 x, y, z, t;
	int n = sscanf(text, "%f,%f,%f, %f,%f,%f, %f,%f,%f, %f,%f,%f", &x.X, &x.Y, &x.Z, &y.X, &y.Y, &y.Z, &z.X, &z.Y, &z.Z, &t.X, &t.Y, &t.Z);
	value->Set(x, y, z, t);
	return n == 12;
}

char *Value_To_String(char *buf, int size, __int64 value)
{
	Unknown_8B44E0(value, buf);
	return buf;
}

bool String_To_Value(const char *text, __int64 *value)
{
	*value = Unknown_8B44C0(text);
	return true;
}

char *Value_To_String(char *buf, int size, unsigned __int64 value)
{
	Unknown_8B44E0(value, buf);
	return buf;
}

bool String_To_Value(const char *text, unsigned __int64 *value)
{
	*value = Unknown_8B44C0(text);
	return true;
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\strconv_lib.cpp (the client\'s __FILE__, asserts on lines 24 and 57)
**
**	client.exe 0x4A3750-0x4A3B10, all byte-exact: Value_To_String / String_To_Value for Vector3
**	(0x4A3750 / 0x4A3810, "%f,%f,%f"), Matrix3D (0x4A3850 / 0x4A3990, its axes and translation, twelve
**	"%f") and __int64 (0x4A3A80 / 0x4A3AB0, through 0x8B44E0 / 0x8B44C0). The object before it
**	(0x4A30A0-0x4A3640, the plain types and std::string) has no assert and no __FILE__: unknown_strconv.cpp.
**	0x4A3AE0 / 0x4A3B10 have the same bytes as the __int64 pair, uncalled: unsigned __int64 by the
**	order of the pairs (NAME 2: the type is a guess). Names: strconv_lib.h's (NAME 2).
*/
