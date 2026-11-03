// Earth & Beyond client (reconstruction): app\shared\unknown_8b4560.cpp
// PLACEMENT: provisional (D137): 64-bit byte swap functions 0x8B4510 and 0x8B4560; no __FILE__ string in client.exe

#include "unknown_8b4560.h"

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
