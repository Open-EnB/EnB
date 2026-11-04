// Earth & Beyond client (reconstruction): app\shared\unknown_strconv.cpp. Notes at the end of the file.
// NAME 1 [unknown_strconv.cpp]: INTERIM file name (D137): the object 0x4A30A0-0x4A3640 has no assert and no __FILE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "strconv_lib.h"

char *Value_To_String(char *buf, int size, bool value)
{
	strcpy(buf, value ? "true" : "false");
	return buf;
}

bool String_To_Value(const char *text, bool *value)
{
	if (strncmp(text, "true", 4) == 0 || strncmp(text, "on", 2) == 0 || strncmp(text, "yes", 3) == 0 || strncmp(text, "1", 1) == 0) {
		*value = true;
		return true;
	}
	if (strncmp(text, "false", 5) == 0 || strncmp(text, "off", 3) == 0 || strncmp(text, "no", 2) == 0 || strncmp(text, "0", 1) == 0) {
		*value = false;
		return true;
	}
	return false;
}

char *Value_To_String(char *buf, int size, float value)
{
	sprintf(buf, "%f", value);
	return buf;
}

bool String_To_Value(const char *text, float *value)
{
	*value = (float)atof(text);
	return true;
}

char *Value_To_String(char *buf, int size, double value)
{
	sprintf(buf, "%f", value);
	return buf;
}

bool String_To_Value(const char *text, double *value)
{
	*value = atof(text);
	return true;
}

char *Value_To_String(char *buf, int size, char value)
{
	sprintf(buf, "%d", value);
	return buf;
}

bool String_To_Value(const char *text, char *value)
{
	int i = 0;
	if (sscanf(text, "%d", &i) == 1 && i >= -128 && i <= 127) {
		*value = (char)i;
		return true;
	}
	return false;
}

char *Value_To_String(char *buf, int size, unsigned char value)
{
	sprintf(buf, "%u", value);
	return buf;
}

bool String_To_Value(const char *text, unsigned char *value)
{
	int i = 0;
	if (sscanf(text, "%u", &i) == 1 && i <= 255) {
		*value = (unsigned char)i;
		return true;
	}
	return false;
}

char *Value_To_String(char *buf, int size, int value)
{
	sprintf(buf, "%d", value);
	return buf;
}

bool String_To_Value(const char *text, int *value)
{
	return sscanf(text, "%d", value) == 1;
}

char *Value_To_String(char *buf, int size, short value)
{
	sprintf(buf, "%d", value);
	return buf;
}

bool String_To_Value(const char *text, short *value)
{
	return sscanf(text, "%hd", value) == 1;
}

char *Value_To_String(char *buf, int size, long value)
{
	sprintf(buf, "%ld", value);
	return buf;
}

bool String_To_Value(const char *text, long *value)
{
	return sscanf(text, "%ld", value) == 1;
}

char *Value_To_String(char *buf, int size, unsigned long value)
{
	sprintf(buf, "%lu", value);
	return buf;
}

bool String_To_Value(const char *text, unsigned long *value)
{
	return sscanf(text, "%lu", value) == 1;
}

char *Value_To_String(char *buf, int size, unsigned int value)
{
	sprintf(buf, "%u", value);
	return buf;
}

bool String_To_Value(const char *text, unsigned int *value)
{
	return sscanf(text, "%u", value) == 1;
}

char *Value_To_String(char *buf, int size, const std::string &value)
{
	strcpy(buf, value.c_str());
	return buf;
}

bool String_To_Value(const char *text, std::string *value)
{
	*value = text;
	return true;
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (INTERIM name, D137: the object has no assert and no __FILE__; it sits
**	between gclass.cpp's, which ends at 0x4A3000, and strconv_lib.cpp's, from 0x4A3750, whose first
**	assert is on line 24, too early for these 22 functions)
**
**	client.exe 0x4A30A0-0x4A3640, all byte-exact: Value_To_String / String_To_Value for bool (0x4A30A0 /
**	0x4A30F0), float, double, char, unsigned char, int, short, long, unsigned long, unsigned int and
**	std::string (0x4A35F0 / 0x4A3640). The size argument is unused (sprintf, strcpy). <string>'s guarded
**	static (0x4A3720, 0xC416FC, atexit 0x42B320) closes the object. Callers: the key/value pairs'
**	Get_Value_String / Set_Value_String (bool 0x4487A0 / 0x448B60, float, char, int 0x4C3D30 / 0x4C4290,
**	long, unsigned int) and the packets' Print (bool). Names: strconv_lib.h's (NAME 2); the declarations
**	the pairs share are strconv_lib.h's, the rest are declared where they are used.
*/
