// Earth & Beyond client (reconstruction): app\client\unknown_8b3e70.h
/*
**	PLACEMENT: provisional (D137): one object 0x8B3E70-0x8B42F0 between wlib's configfile.cpp and critsec.cpp
**	in the client's link order (combining queue rows 515 and 454); no __FILE__ string in client.exe.
**	Contains CRC-32 (polynomial 0xEDB88320) and CRC-64 (polynomial 0xC2DC3A07BB09D9FF) table builders
**	and buffer / C-string update routines, including 0x8B3EC0 (Cord_CRC).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_8B3E70_H
#define __UNKNOWN_8B3E70_H

// 0x8B3E70: builds the 256-entry reflected CRC-32 lookup table (polynomial 0xEDB88320)
void Unknown_8B3E70(void);	// NAME 1: neutral placeholder (D10)

// 0x8B3EC0: updates crc over len bytes of data using the CRC-32 table (lazily built; declared in cordcon.h too)
unsigned int Cord_CRC(const unsigned char *data, int len, unsigned int crc);	// NAME 1: placeholder (as cordcon.h's)

// 0x8B3F60: updates crc over the C string str using the CRC-32 table (lazily built)
unsigned int Unknown_8B3F60(const char *str, unsigned int crc);	// NAME 1: neutral placeholder (D10)

// 0x8B4020: builds the 256-entry reflected CRC-64 lookup table (polynomial 0xC2DC3A07BB09D9FF)
void Unknown_8B4020(void);	// NAME 1: neutral placeholder (D10)

// 0x8B40A0: updates crc over len bytes of data using the CRC-64 table (lazily built)
unsigned __int64 Unknown_8B40A0(const unsigned char *data, int len, unsigned __int64 crc);	// NAME 1: neutral placeholder (D10)

// 0x8B41C0: updates crc over the C string str using the CRC-64 table (lazily built)
unsigned __int64 Unknown_8B41C0(const char *str, unsigned __int64 crc);	// NAME 1: neutral placeholder (D10)

#endif //__UNKNOWN_8B3E70_H
