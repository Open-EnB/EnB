// Earth & Beyond client (reconstruction): app\shared\unknown_8b4560.cpp
// PLACEMENT: provisional (D137): the 64-bit text conversions 0x8B44C0 / 0x8B44E0 and byte swap functions 0x8B4510
// and 0x8B4560 (one run, after wlib's monod.cpp); no __FILE__ string in client.exe

#include "unknown_8b4560.h"
#include <stdlib.h>

// 0x8B44C0
__int64 Unknown_8B44C0(const char *text)	// NAME 1: neutral placeholder (D10)
{
	return(_atoi64(text));
}

// 0x8B44E0
char *Unknown_8B44E0(__int64 value, char *buffer)	// NAME 1: neutral placeholder (D10)
{
	return(_i64toa(value, buffer, 10));
}

extern "C" unsigned long __stdcall htonl(unsigned long);
extern "C" unsigned long __stdcall ntohl(unsigned long);

union U64 {
	unsigned __int64 val;
	struct {
		unsigned long lo;
		unsigned long hi;
	} s;
};

// 0x8B4510
unsigned __int64 Unknown_8B4510(unsigned __int64 value)		// NAME 1 [Unknown_8B4510]: wwonline's 64-bit htonl
{
	if (htonl(0x12345678) == 0x12345678) {
		return value;
	}
	U64 in_val;
	in_val.val = value;
	htonl(in_val.s.lo);
	htonl(in_val.s.hi);
	U64 out_val;
	out_val.s.lo = in_val.s.hi;
	out_val.s.hi = in_val.s.lo;
	return out_val.val;
}

// 0x8B4560
unsigned __int64 Unknown_8B4560(unsigned __int64 value)		// NAME 1 [Unknown_8B4560]: wwonline's 64-bit ntohl
{
	if (ntohl(0x12345678) == 0x12345678) {
		return value;
	}
	U64 in_val;
	in_val.val = value;
	ntohl(in_val.s.lo);
	ntohl(in_val.s.hi);
	U64 out_val;
	out_val.s.lo = in_val.s.hi;
	out_val.s.hi = in_val.s.lo;
	return out_val.val;
}
