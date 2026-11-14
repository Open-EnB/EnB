// Earth & Beyond client (reconstruction): libs\wwonline\wlib\unknown_8b45b0.cpp
// PLACEMENT: provisional (D137): the object 0x8B45B0-0x8B4F60, between wlib's monod.cpp and streamer.cpp
// in the client's link order; no __FILE__ string in client.exe. Notes at the end of the file.

#include "unknown_8b45b0.h"
#include <string.h>
#include <stdio.h>
#include <iostream.h>

extern "C" unsigned long __stdcall htonl(unsigned long);
extern "C" unsigned long __stdcall ntohl(unsigned long);

// app\shared\unknown_8b4560.cpp (0x8B44E0, the code just before this object)
char *Unknown_8B44E0(__int64 value, char *buffer);

#define CUTOFF 8
#define STKSIZ (8*sizeof(void*) - 2)

static void swap(char *a, char *b, unsigned width)
{
	char tmp;
	if (a != b) {
		while (width--) {
			tmp = *a;
			*a++ = *b;
			*b++ = tmp;
		}
	}
}

static void shortsort(char *lo, char *hi, unsigned width, int (*comp)(const void *, const void *, void *), void *context)
{
	char *p, *max;
	while (hi > lo) {
		max = lo;
		for (p = lo + width; p <= hi; p += width) {
			if (comp(p, max, context) > 0) {
				max = p;
			}
		}
		swap(max, hi, width);
		hi -= width;
	}
}

// @0x8B45B0
void Unknown_8B45B0(void *base, unsigned num, unsigned width, int (*comp)(const void *, const void *, void *), void *context)
{
	char *lo, *hi;
	char *mid;
	char *loguy, *higuy;
	unsigned size;
	char *lostk[STKSIZ], *histk[STKSIZ];
	int stkptr;

	if (num < 2 || width == 0)
		return;

	stkptr = 0;

	lo = (char *)base;
	hi = (char *)base + width * (num - 1);

recurse:
	size = (hi - lo) / width + 1;

	if (size <= CUTOFF) {
		shortsort(lo, hi, width, comp, context);
	} else {
		mid = lo + (size / 2) * width;
		swap(mid, lo, width);

		loguy = lo;
		higuy = hi + width;

		for (;;) {
			do {
				loguy += width;
			} while (loguy <= hi && comp(loguy, lo, context) <= 0);

			do {
				higuy -= width;
			} while (higuy > lo && comp(higuy, lo, context) >= 0);

			if (higuy < loguy)
				break;

			swap(loguy, higuy, width);
		}

		swap(lo, higuy, width);

		if (higuy - 1 - lo >= hi - loguy) {
			if (lo + width < higuy) {
				lostk[stkptr] = lo;
				histk[stkptr] = higuy - width;
				++stkptr;
			}

			if (loguy < hi) {
				lo = loguy;
				goto recurse;
			}
		} else {
			if (loguy < hi) {
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;
			}

			if (lo + width < higuy) {
				hi = higuy - width;
				goto recurse;
			}
		}
	}

	--stkptr;
	if (stkptr >= 0) {
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;
	} else
		return;
}

// @0x8B4900
Unknown_8B4900Class::Unknown_8B4900Class(void)
{
}

// @0x8B4920
Unknown_8B4900Class::Unknown_8B4900Class(int)
{
}

// @0x8B4940
Unknown_8B4900Class::~Unknown_8B4900Class(void)
{
}

// @0x8B4960
int Unknown_8B4900Class::Unknown_8B4960(void)
{
	return 0;
}

// @0x8B4980
int Unknown_8B4900Class::Unknown_8B4980(void)
{
	return 0;
}

// @0x8B49A0
int Unknown_8B4900Class::Unknown_8B49A0(void)
{
	return 0;
}

// @0x8B49C0
int Unknown_8B4900Class::Unknown_8B49C0(int)
{
	return 0;
}

// @0x8B49E0
int Unknown_8B4900Class::Unknown_8B49E0(void)
{
	return 0;
}

// @0x8B4A00
void Unknown_8B4900Class::Print(void)
{
	char buf[128];
	memset(buf, 0, sizeof(buf));
	for (int i = 0; i < 20; i++) {
		sprintf(buf + strlen(buf), "%02X", Digest[i]);
	}
	sprintf(buf + strlen(buf), "\n\n");
}

// 0x80, then zeros: the message padding (Cord_SHA reads up to 72 bytes of it)
static unsigned char Padding[72] = { 0x80 };

#define ROTL(n, x)	(((x) << (n)) | ((x) >> (32 - (n))))

// NOT EXACT: the client's 912 bytes against 911, the same instructions but for the registers (explain.py:
// shape, 29 rows once registers are masked): the digest pointer in ebx, the round counter in ebp
// @0x8B4A90
bool Cord_SHA(const void *data, int len, unsigned int *digest)
{
	unsigned long block[2][16];
	unsigned long w[80];
	unsigned pad;
	unsigned total;
	unsigned nblocks;
	unsigned i;
	unsigned t;

	memset(block[0], 0, sizeof(block[0]));
	memset(block[1], 0, sizeof(block[1]));

	pad = 72 - (len + 8) % 64;
	total = len + pad;

	if (total > 64) {
		if (pad > 64) {
			memcpy(block[0], (const char *)data + len - (128 - pad), 128 - pad);
			memcpy((char *)block[0] + (128 - pad), Padding, pad - 64);
			memcpy(block[1], Padding + (pad - 64), 64);
		} else if (pad == 64) {
			memcpy(block[0], (const char *)data + len - 64, 64);
			memcpy(block[1], Padding, 64);
		} else if (pad < 64) {
			memcpy(block[0], (const char *)data + len - (128 - pad), 64);
			memcpy(block[1], (const char *)data + len - (64 - pad), 64 - pad);
			memcpy((char *)block[1] + (64 - pad), Padding, pad);
		}
	} else {
		memcpy(block[1], data, len);
		memcpy((char *)block[1] + len, Padding, 64 - len);
	}

	block[1][15] = htonl(len * 8);
	nblocks = total / 64;

	digest[0] = 0x67452301;
	digest[1] = 0xEFCDAB89;
	digest[2] = 0x98BADCFE;
	digest[3] = 0x10325476;
	digest[4] = 0xC3D2E1F0;

	for (i = 0; i < nblocks; i++) {
		const unsigned long *src = (const unsigned long *)((const char *)data + i * 64);
		if (i == nblocks - 2) {
			src = block[0];
		} else if (i == nblocks - 1) {
			src = block[1];
		}
		for (t = 0; t < 16; t++) {
			w[t] = ntohl(src[t]);
		}
		for (t = 16; t < 80; t++) {
			unsigned long x = w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16];
			w[t] = ROTL(1, x);
		}

		unsigned long a = digest[0];
		unsigned long b = digest[1];
		unsigned long c = digest[2];
		unsigned long d = digest[3];
		unsigned long e = digest[4];

		for (t = 0; t < 80; t++) {
			unsigned long hi = a << 5;
			unsigned long lo = a >> 27;
			unsigned long f, k;
			if (t < 20) {
				f = (b & c) | (~b & d);
			} else if (t >= 40 && t < 60) {
				f = (b & c) | ((b | c) & d);
			} else {
				f = b ^ c ^ d;
			}
			if (t < 20) {
				k = 0x5A827999;
			} else if (t < 40) {
				k = 0x6ED9EBA1;
			} else if (t < 60) {
				k = 0x8F1BBCDC;
			} else {
				k = 0xCA62C1D6;
			}
			unsigned long temp = (lo | hi) + k + f + e + w[t];
			e = d;
			d = c;
			c = ROTL(30, b);
			b = a;
			a = temp;
		}

		digest[0] += a;
		digest[1] += b;
		digest[2] += c;
		digest[3] += d;
		digest[4] += e;
	}

	for (i = 0; i < 5; i++) {
		digest[i] = htonl(digest[i]);
	}
	return true;
}

// @0x8B4F10
ostream &operator << (ostream &os, __int64 value)
{
	char buf[64];
	Unknown_8B44E0(value, buf);
	os << buf;
	return os;
}

/*
** Notes (O29, 2026-11-14):
** - No EA copy has this object: every wlib (Zero Hour's matchbot and mangler, Generals', Renegade's
**   Code\Tools\wlib) goes configfile, critsec, monod, sem4, streamer; the queue's hint "SHAEngine" is wwlib's
**   class (wwlib\sha.cpp, 0x9E93B0-0x9E97B0 in the client), not this one: here SHA-1 is one C function.
** - 0x8B45B0 is the VC6 CRT's qsort.c with a context argument (shortsort and swap inlined); 0x8B4F10 is an
**   ostream inserter over 0x8B44E0 (old iostreams: 0xA40DAB is libcimt's ostream::operator<<(const char *)).
** - Cord_SHA: the block logic, ported line for line to Python, equals hashlib's SHA-1 for lengths 0-299. The rotate by 5 is two separate words because the client
**   computes a << 5 and a >> 27 before f and k and ors them after (one temporary on the stack, 0x1E4 bytes);
**   f and k as statements give the client's order of the sum (k, f, e, w[t]).
** - The code just before (0x8B44C0-0x8B4560: app\shared\unknown_8b4560.cpp's _atoi64 / _i64toa wrappers
**   and 64-bit byte swaps) is likely the same object: 0x8B4F10 calls 0x8B44E0.
*/
