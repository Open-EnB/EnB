// Earth & Beyond client (reconstruction): app\client\unknown_8b3e70.cpp
// PLACEMENT: provisional (D137): one object 0x8B3E70-0x8B42F0 between wlib's configfile.cpp and critsec.cpp
// in the client's link order (combining queue rows 515 and 454); no __FILE__ string in client.exe.

#include "unknown_8b3e70.h"
#include <string.h>

static unsigned int crc32_table[256];	// NAME 2: CRC-32 table at 0xBF43E4
static bool crc32_made = false;	// NAME 2: CRC-32 init flag at 0xBF47E4
static unsigned __int64 crc64_table[256];	// NAME 2: CRC-64 table at 0xBF48B8
static bool crc64_made = false;	// NAME 2: CRC-64 init flag at 0xBF50B8

// 0x8B3E70
void Unknown_8B3E70(void)	// NAME 1: neutral placeholder (D10)
{
	for (int i = 0; i < 256; i++) {
		unsigned int c = i;
		for (int k = 8; k != 0; k--) {
			if (c & 1) {
				c = (c >> 1) ^ 0xEDB88320;
			} else {
				c >>= 1;
			}
		}
		crc32_table[i] = c;
	}
	crc32_made = true;
}

// Static helper matching 0x8B3EC0's body (inlined by Unknown_8B3F60; 0x8B3EC0 itself is outside this claim).
static unsigned int crc32_buf(const unsigned char *data, int len, unsigned int crc)	// NAME 2: static helper for 0x8B3EC0
{
	if (!crc32_made) {
		Unknown_8B3E70();
	}
	for (int i = 0; i < len; i++) {
		crc = (crc >> 8) ^ crc32_table[(data[i] ^ crc) & 0xFF];
	}
	return crc;
}

// 0x8B3F60
unsigned int Unknown_8B3F60(const char *str, unsigned int crc)	// NAME 1: neutral placeholder (D10)
{
	int len = strlen(str);
	return crc32_buf((const unsigned char *)str, len, crc);
}

// 0x8B4020
void Unknown_8B4020(void)	// NAME 1: neutral placeholder (D10)
{
	for (int i = 0; i < 256; i++) {
		unsigned __int64 c = i;
		for (int k = 8; k != 0; k--) {
			if (c & 1) {
				c = (c >> 1) ^ 0xC2DC3A07BB09D9FFui64;
			} else {
				c >>= 1;
			}
		}
		crc64_table[i] = c;
	}
	crc64_made = true;
}

// 0x8B40A0
unsigned __int64 Unknown_8B40A0(const unsigned char *data, int len, unsigned __int64 crc)	// NAME 1: neutral placeholder (D10)
{
	unsigned __int64 val = crc;
	if (!crc64_made) {
		Unknown_8B4020();
	}
	for (int i = 0; i < len; i++) {
		val = (val >> 8) ^ crc64_table[(data[i] ^ val) & 0xFF];
	}
	return val;
}

// 0x8B41C0
unsigned __int64 Unknown_8B41C0(const char *str, unsigned __int64 crc)	// NAME 1: neutral placeholder (D10)
{
	unsigned __int64 val = crc;
	int len = strlen(str);
	if (!crc64_made) {
		Unknown_8B4020();
	}
	for (int i = 0; i < len; i++) {
		val = (val >> 8) ^ crc64_table[((unsigned char)str[i] ^ val) & 0xFF];
	}
	return val;
}
