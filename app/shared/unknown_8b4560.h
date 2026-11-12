#ifndef UNKNOWN_8B4560_H
#define UNKNOWN_8B4560_H

// PLACEMENT: provisional (D137): the 64-bit text conversions 0x8B44C0 / 0x8B44E0 and byte swap functions 0x8B4510
// and 0x8B4560 (one run, after wlib's monod.cpp); no __FILE__ string in client.exe

__int64 Unknown_8B44C0(const char *text);	// NAME 1: neutral placeholder (D10): _atoi64(text)
char *Unknown_8B44E0(__int64 value, char *buffer);	// NAME 1: neutral placeholder (D10): _i64toa(value, buffer, 10)
unsigned __int64 Unknown_8B4510(unsigned __int64 value);	// NAME 1 [Unknown_8B4510]: wwonline's 64-bit htonl
unsigned __int64 Unknown_8B4560(unsigned __int64 value);	// NAME 1 [Unknown_8B4560]: wwonline's 64-bit ntohl

#endif
