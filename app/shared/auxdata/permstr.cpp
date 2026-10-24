// Earth & Beyond client (reconstruction): app\shared\auxdata\permstr.cpp. Notes at the end of the file.
#include <string.h>
#include "permstr.h"
#include "..\unknown_debug.h"
#include "..\..\..\wwlib\nstrdup.h"

// app\dbgmgr\dbgmgr.cpp's formatted debug output (0x509DC0; its header is not rebuilt yet)
void Unknown_Debug_Printf(const char *format, ...);

// One interned string: the table's slots chain these (size 0xC)
class PermStringEntryClass
{
public:
	PermStringEntryClass(const char *string, unsigned int hash, PermStringEntryClass *next);
	~PermStringEntryClass();

	char *String;						// +0x00 (the assert's name)
	unsigned int Hash;					// +0x04
	PermStringEntryClass *Next;		// +0x08
};

static PermStringEntryClass **s_Table;		// 0xBE3B30 (the asserts' name)
static int s_Size;							// 0xBE3B34 the first prime from 1500 (1511)
static int s_Count;							// 0xBE3B38 the PermStringInitClass objects alive

static bool Is_Prime(int number)
{
	for (int i = 2; i < number; i++) {
		if (number % i == 0) {
			return false;
		}
	}
	return true;
}

























PermStringInitClass::PermStringInitClass()
{
	if (s_Count++ == 0) {
		G_ASSERT(s_Table == 0);
		if (s_Table == 0) {
			int size = 1500;
			while (!Is_Prime(size)) {
				size++;
			}
			s_Size = size;
			s_Table = new PermStringEntryClass *[s_Size];





			G_ASSERT(s_Table);
			memset(s_Table, 0, s_Size * sizeof(PermStringEntryClass *));
		}
	}
}

PermStringInitClass::~PermStringInitClass()
{
	if (--s_Count == 0) {
		G_ASSERT(s_Table != 0);
		if (s_Table != 0) {
			for (int i = 0; i < s_Size; i++) {
				PermStringEntryClass *entry = s_Table[i];
				while (entry != 0) {
					PermStringEntryClass *next = entry->Next;
					delete entry;
					entry = next;
				}
			}
			delete [] s_Table;
			s_Table = 0;
			s_Size = 0;
		}
	}
}

























const char *PermString::Find(const char *string)
{
	G_ASSERT(s_Table != 0);
	if (s_Table == 0) {
		return string;
	}
	unsigned int hash = 0;
	const char *p = string;
	while (*p != 0) {
		hash += *p++;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	unsigned int slot = hash % s_Size;
	PermStringEntryClass *entry = s_Table[slot];
	while (entry != 0) {
		if (entry->Hash == hash && strcmp(string, entry->String) == 0) {
			return entry->String;
		}
		entry = entry->Next;
	}
	entry = new PermStringEntryClass(string, hash, s_Table[slot]);
	s_Table[slot] = entry;
	return entry->String;
}

void PermString::Print_Statistics()
{
	int entries = 0;
	int occupied = 0;
	int max_chain = 0;
	for (int i = 0; i < s_Size; i++) {
		PermStringEntryClass *entry = s_Table[i];
		if (entry != 0) {
			occupied++;
		}
		int chain = 0;
		for (; entry != 0; entry = entry->Next) {
			chain++;
		}
		entries += chain;
		if (chain > max_chain) {
			max_chain = chain;
		}
	}
	Unknown_Debug_Printf("PermString: %d slots, %d entries, %d occupied, %d max chain.\n", s_Size, entries, occupied, max_chain);
}

PermString &PermString::operator+=(const PermString &that)
{
	int length = strlen(String);
	int that_length = strlen(that.String);
	char *buffer = new char[length + that_length + 1];
	memcpy(buffer, String, length);
	memcpy(buffer + length, that.String, that_length);
	buffer[length + that_length] = 0;
	String = Find(buffer);
	delete [] buffer;
	return *this;
}






















PermStringEntryClass::PermStringEntryClass(const char *string, unsigned int hash, PermStringEntryClass *next) :
	Hash(hash),
	Next(next)
{
	String = nstrdup(string);
	G_ASSERT(String);
}

PermStringEntryClass::~PermStringEntryClass()
{
	delete [] String;
	String = 0;
}

// a second initialiser of this file's own (0xBE3B40, constructed by 0x4AE3B0)
static PermStringInitClass PermStringInitInstance;

/*
**	Reconstruction notes
**
**	client.exe 0x4ADC50-0x4AE3C5: PermString's table (Jenkins' one-at-a-time hash without the
**	final mix, chained slots). Rebuilt: PermStringInitClass's constructor 0x4AE1E0 (asserts on the
**	client's lines 63 and 76; the prime search is an inlined helper, Is_Prime here) and destructor
**	0x4AE2D0 (line 85), Find 0x4ADE20 (line 128; the entry's constructor, line 213, inlined),
**	Print_Statistics 0x4ADFF0, operator+= 0x4AE070, PermStringEntryClass's constructor 0x4AE140 and
**	destructor 0x4AE1B0. The header's initialiser is this object's first (0x4ADC50, with both
**	PermStringInitClass functions inlined, and its atexit 0x4ADD40).
**	0x4ADBB0 (the part of a name after its first dot, as a PermString) lies between name.cpp's last
**	function and 0x4ADC50: probably name.cpp's, not rebuilt yet.
*/
