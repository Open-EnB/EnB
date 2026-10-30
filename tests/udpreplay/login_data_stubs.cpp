// Earth & Beyond client (reconstruction)
// udpreplay: strconv_lib.cpp's long / __int64 conversions, which app\shared\unknown_login_data.cpp's key/value
// side calls (MASTER_JOIN 53's TicketClass brings it into the harness through app.lib). strconv_lib.cpp is not
// rebuilt; the packets don't use these (sprintf / sscanf stand-ins).
#include <stdio.h>

char *Value_To_String(char *buf, int size, long value)
{
	_snprintf(buf, size, "%ld", value);
	return buf;
}

bool String_To_Value(const char *text, long *value)
{
	return sscanf(text, "%ld", value) == 1;
}

char *Value_To_String(char *buf, int size, __int64 value)
{
	_snprintf(buf, size, "%I64d", value);
	return buf;
}

bool String_To_Value(const char *text, __int64 *value)
{
	return sscanf(text, "%I64d", value) == 1;
}
