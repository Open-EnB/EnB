// Earth & Beyond client (reconstruction): libs\wwonline\wlib\unknown_8b45b0.h
/*
**	PLACEMENT: provisional (D137): the object between wlib's monod.cpp and streamer.cpp in the client's
**	link order (0x8B45B0-0x8B4F60), with no __FILE__ string; no EA copy of wlib has it. A qsort with a
**	context argument, a class of empty members, a 20-byte digest printer, a one-shot SHA-1 (the constants
**	0x67452301 ... 0xC3D2E1F0, 0x5A827999 ... 0xCA62C1D6) and an ostream inserter for __int64.
*/

#ifndef UNKNOWN_8B45B0_HEADER
#define UNKNOWN_8B45B0_HEADER

class ostream;

// 0x8B45B0: the CRT's qsort (CUTOFF 8, a 30-entry stack) with a context passed to every comparison
void Unknown_8B45B0(void *base, unsigned num, unsigned width,
	int (*comp)(const void *, const void *, void *), void *context);	// NAME 1: neutral placeholder (D10)

// 0x8B4900-0x8B49E0: out-of-line members that do nothing (no caller, no vtable in the client: kept by the
// incremental link); 0x8B4A00 prints Digest as 40 hex digits into a local buffer and drops it
class Unknown_8B4900Class		// NAME 1: neutral placeholder (D10)
{
public:
	Unknown_8B4900Class(void);
	Unknown_8B4900Class(int);
	~Unknown_8B4900Class(void);
	int Unknown_8B4960(void);
	int Unknown_8B4980(void);
	int Unknown_8B49A0(void);
	int Unknown_8B49C0(int);
	int Unknown_8B49E0(void);
	void Print(void);		// NAME 1: what 0x8B4A00 does (sprintf "%02X" of 20 bytes, then "\n\n")

	unsigned char Digest[20];	// NAME 1: the 20 bytes 0x8B4A00 prints
};

// 0x8B4A90 (cdecl; CordMgr::Compute_Ticks calls it): SHA-1 of len bytes into digest[5], each word in
// network order; returns true
bool Cord_SHA(const void *data, int len, unsigned int *digest);	// NAME 1: placeholder (cordmgr.h's)

// 0x8B4F10: writes value in decimal (0x8B44E0, _i64toa)
ostream &operator << (ostream &os, __int64 value);

#endif
