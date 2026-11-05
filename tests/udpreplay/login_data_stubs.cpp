// Earth & Beyond client (reconstruction)
// udpreplay: strconv_lib.cpp's __int64 conversions (the long ones are
// unknown_strconv.cpp's, from app.lib), which app\shared\unknown_login_data.cpp's key/value
// side calls (MASTER_JOIN 53's TicketClass brings it into the harness through app.lib). strconv_lib.obj is not
// linked here (its helpers 0x4A1590 / 0x8B44E0 / 0x8B44C0); the packets don't use these (stand-ins).
#include <stdio.h>

char *Value_To_String(char *buf, int size, __int64 value)
{
	_snprintf(buf, size, "%I64d", value);
	return buf;
}

bool String_To_Value(const char *text, __int64 *value)
{
	return sscanf(text, "%I64d", value) == 1;
}
