// Earth & Beyond client (reconstruction)
// PROVISIONAL file name (D10): the lip-sync phoneme converter (Unknown_791310Class, unknown_personality.h),
// client 0x791310-0x792040 after the personality manager's file. Notes at the end of the file.
#include "unknown_personality.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define IS_VOWEL(c)	((c) == 'a' || (c) == 'e' || (c) == 'i' || (c) == 'o' || (c) == 'u')

// 0xB9DAB8: the letter groups and their mouth shapes (-1: none), sorted by 0x791310
struct Unknown_PhonemeGroupStruct	// NAME 1: neutral (D10)
{
	const char *Text;
	char Shapes[2];
};

static Unknown_PhonemeGroupStruct Unknown_B9DAB8[0x23] = {	// NAME 1 [Unknown_B9DAB8]: neutral (D10)
	{ "air", { 2, -1 } },
	{ "ar", { 3, -1 } },
	{ "ch", { 8, -1 } },
	{ "ea", { 1, -1 } },
	{ "ee", { 1, -1 } },
	{ "er", { 15, -1 } },
	{ "oa", { 3, -1 } },
	{ "oo", { 4, -1 } },
	{ "ou", { 3, 4 } },
	{ "ow", { 4, 0 } },
	{ "qu", { 4, -1 } },
	{ "sch", { 14, 12 } },
	{ "sh", { 8, -1 } },
	{ "tch", { 8, -1 } },
	{ "th", { 10, -1 } },
	{ "gn", { 13, -1 } },
	{ "kn", { 13, -1 } },
	{ "eye", { 2, -1 } },
	{ "uy", { 2, -1 } },
	{ "ar", { 3, -1 } },
	{ "kw", { 4, -1 } },
	{ "and", { 13, 11 } },
	{ "ze", { 6, 1 } },
	{ "ro", { 4, 3 } },
	{ "one", { 4, 0 } },
	{ "two", { 1, 4 } },
	{ "thr", { 10, 6 } },
	{ "fou", { 7, 3 } },
	{ "ive", { 2, 7 } },
	{ "six", { 5, 2 } },
	{ "se", { 5, 1 } },
	{ "ven", { 7, 1 } },
	{ "eight", { 0, 1 } },
	{ "ni", { 10, 2 } },
	{ "ne", { 10, -1 } },
};

// 0xBE8570: the groups by first letter a-z
static struct { int First; int Count; } Unknown_BE8570[26];	// NAME 1 [Unknown_BE8570]: neutral (D10)

// 0x7913B0
static int __cdecl Unknown_7913B0(const void *a, const void *b)	// NAME 1: neutral (D10)
{
	return(strcmp(((const Unknown_PhonemeGroupStruct *)a)->Text, ((const Unknown_PhonemeGroupStruct *)b)->Text));
}

// 0x791310
// NOT EXACT: functional for POC (D64), shape (the index loop)
Unknown_791310Class::Unknown_791310Class()
{
	qsort(Unknown_B9DAB8, 0x23, sizeof(Unknown_PhonemeGroupStruct), Unknown_7913B0);
	memset(Unknown_BE8570, 0, sizeof(Unknown_BE8570));
	for (int i = 0; i < 0x23; i++) {
		int letter = tolower(*Unknown_B9DAB8[i].Text) - 'a';
		if (letter >= 0 && letter < 26) {
			if (Unknown_BE8570[letter].Count == 0) {
				Unknown_BE8570[letter].First = i;
			}
			Unknown_BE8570[letter].Count++;
		}
	}
}

// 0x791400
// NOT EXACT: functional for POC (D64), literal (the shapes' initial stores in another order)
int Unknown_791310Class::Unknown_791400(const char *text, int *phonemes, int max)
{
	char prev = '\0';
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int len = strlen(text);
	int last = -1;
	int count = 0;
	if (len > 0x3FE) {
		len = 0x3FF;
	}
	strncpy(buffer, text, len);
	_strlwr(buffer);
	const char *p = buffer;
	while (*p != '\0') {
		int shapes[2];
		int n = Unknown_7917B0(p, prev, shapes);
		if (n == 0) {
			shapes[1] = -1;
			shapes[0] = 11;
			n = 1;
			switch (*p) {
				case 'a':
					n = Unknown_791880(p, prev, shapes);
					break;
				case 'b':
				case 'm':
				case 'p':
					shapes[0] = 9;
					break;
				case 'c':
				case 'g':
				case 'k':
				case 'q':
					shapes[0] = 12;
					break;
				case 'd':
					shapes[0] = 11;
					break;
				case 'e':
					n = Unknown_791970(p, prev, shapes);
					break;
				case 'f':
				case 'v':
					shapes[0] = 7;
					break;
				case 'i':
					n = Unknown_791B00(p, prev, shapes);
					break;
				case 'j':
					shapes[0] = 8;
					break;
				case 'l':
					shapes[0] = 10;
					break;
				case 'n':
					shapes[0] = 13;
					break;
				case 'o':
					n = Unknown_791BF0(p, prev, shapes);
					break;
				case 'r':
					shapes[0] = 6;
					break;
				case 's':
					n = Unknown_791E50(p, prev, shapes);
					break;
				case 't':
					n = Unknown_791F10(p, prev, shapes);
					break;
				case 'u':
					n = Unknown_791F80(p, prev, shapes);
					break;
				case 'w':
					shapes[0] = 4;
					break;
				case 'x':
					shapes[0] = 12;
					shapes[1] = 14;
					break;
				case 'z':
					shapes[0] = 14;
					break;
			}
		}
		for (int i = 0; i < 2; i++) {
			if (shapes[i] >= 0 && shapes[i] != last) {
				last = shapes[i];
				*phonemes++ = shapes[i];
				if (++count >= max) {
					return(count);
				}
			}
		}
		prev = p[n - 1];
		p += n;
	}
	return(count);
}

// 0x7917B0
// NOT EXACT: functional for POC (D64), shape (the index entry's loads)
int Unknown_791310Class::Unknown_7917B0(const char *text, char prev, int *shapes)
{
	int letter = (char)tolower(*text) - 'a';
	if (letter >= 0 && letter < 26) {
		int count = Unknown_BE8570[letter].Count;
		Unknown_PhonemeGroupStruct *group = &Unknown_B9DAB8[Unknown_BE8570[letter].First + count - 1];
		for (int i = 0; i < Unknown_BE8570[letter].Count; i++, group--) {
			int len = strlen(group->Text);
			if (_strnicmp(text, group->Text, len) == 0) {
				shapes[0] = group->Shapes[0];
				shapes[1] = group->Shapes[1];
				return(len);
			}
		}
	}
	return(0);
}

// 0x791880
// NOT EXACT: functional for POC (D64), shape (the branch layout)
int Unknown_791310Class::Unknown_791880(const char *text, char prev, int *shapes)
{
	shapes[0] = 11;
	shapes[1] = -1;
	char c = text[1];
	if (c != '\0' && IS_VOWEL(c)) {
		if (c == 'i') {
			shapes[1] = 1;
			if (text[2] == 's') {
				shapes[0] = 0;
			}
			return(2);
		}
		if (c == 'o') {
			shapes[1] = 1;
			return(2);
		}
		if (c != 'u') {
			return(2);
		}
		if (prev == 'g') {
			shapes[1] = 1;
			return(2);
		}
		shapes[0] = 0;
		if (prev != 'l' && prev == 's') {
			shapes[1] = 4;
		}
		return(2);
	}
	c = text[2];
	if (c != '\0' && IS_VOWEL(c)) {
		shapes[1] = 1;
		return(1);
	}
	shapes[0] = 0;
	return(1);
}

// 0x791970
// NOT EXACT: functional for POC (D64), shape (the branch layout)
int Unknown_791310Class::Unknown_791970(const char *text, char prev, int *shapes)
{
	shapes[1] = -1;
	shapes[0] = 1;
	char c = text[1];
	if (c == '\0' || !IS_VOWEL(c)) {
		if (prev != 'b' && c != '\0') {
			shapes[0] = 4;
			return(1);
		}
		shapes[0] = 1;
		return(1);
	}
	switch (c) {
		case 'a':
			if (text[2] == 'u') {
				shapes[0] = 1;
				shapes[1] = 4;
				return(3);
			}
			break;
		case 'i':
			if (prev == 'h') {
				shapes[0] = 0;
				shapes[1] = 1;
				return(2);
			}
			if (text[2] == 'g') {
				shapes[0] = 11;
				shapes[1] = 1;
				return(2);
			}
			shapes[0] = (text[2] != 'z') + 1;
			return(2);
		case 'o':
			shapes[0] = (prev != 'p') ? 11 : 1;
			return(2);
		case 'u':
			shapes[0] = 4;
			return(2);
	}
	return(2);
}

// 0x791B00
// NOT EXACT: functional for POC (D64), shape (the branch layout)
int Unknown_791310Class::Unknown_791B00(const char *text, char prev, int *shapes)
{
	shapes[0] = 2;
	shapes[1] = -1;
	char c = text[1];
	if (c == '\0' || !IS_VOWEL(c)) {
		shapes[0] = 2;
		shapes[1] = -1;
		return(1);
	}
	shapes[0] = 11;
	shapes[1] = -1;
	if (text[1] == 'e') {
		c = text[2];
		if (c == '\0' && prev == 'l') {
			shapes[0] = 0;
			shapes[1] = 1;
			return(2);
		}
		if (c != 'u' && c != 'w') {
			shapes[0] = 1;
			return(2);
		}
		shapes[0] = 4;
		return(3);
	}
	if (text[1] == 'o') {
		shapes[0] = 1;
	}
	return(2);
}

// 0x791BF0
// NOT EXACT: functional for POC (D64), shape (the branch layout)
int Unknown_791310Class::Unknown_791BF0(const char *text, char prev, int *shapes)
{
	shapes[0] = 3;
	shapes[1] = -1;
	char c = text[1];
	if (c == '\0') {
		return(1);
	}
	if (!IS_VOWEL(c)) {
		c = text[2];
		if (c != '\0' && IS_VOWEL(c)) {
			shapes[0] = 4;
			shapes[1] = -1;
			return(1);
		}
		shapes[0] = 3;
		shapes[1] = -1;
		return(1);
	}
	shapes[0] = 5;
	shapes[1] = -1;
	switch (text[1]) {
		case 'a':
			if (text[-2] == 'b' && text[-1] == 'r') {
				shapes[0] = 5;
				return(2);
			}
			shapes[0] = 3;
			shapes[1] = 4;
			return(2);
		case 'e':
			shapes[0] = 4;
			if (text[-1] == 'd' && text[2] == 's') {
				shapes[0] = 5;
				return(2);
			}
			if (text[-2] == 'p' && text[-1] == 'h') {
				shapes[0] = 1;
				return(2);
			}
			break;
		case 'i':
			shapes[0] = 5;
			shapes[1] = 1;
			return(2);
		case 'o':
			shapes[0] = 4;
			return(2);
		case 'u':
			shapes[0] = 4;
			if (text[-2] == 't') {
				if (text[-1] == 'r') {
					shapes[0] = 5;
					return(2);
				}
				if (text[-1] == 'h') {
					if (strncmp(text + 2, "ght", 3) != 0) {
						return(2);
					}
					shapes[0] = 5;
					return(5);
				}
			}
			if (text[2] == 'r') {
				shapes[0] = 11;
				return(3);
			}
			break;
	}
	return(2);
}

// 0x791E50
// NOT EXACT: functional for POC (D64), shape (the branch layout)
int Unknown_791310Class::Unknown_791E50(const char *text, char prev, int *shapes)
{
	shapes[0] = 14;
	shapes[1] = -1;
	char c = text[1];
	if (c == 'h' || c == 'u') {
		shapes[0] = 8;
		return(1);
	}
	if (c == 'c') {
		if (text[2] == 'h') {
			shapes[0] = 8;
			shapes[1] = 11;
			return(3);
		}
		shapes[0] = 11;
		return(2);
	}
	if (strncmp(text + 1, "eou", 3) == 0) {
		shapes[0] = 8;
		return(4);
	}
	return(1);
}

// 0x791F10
// NOT EXACT: functional for POC (D64), shape (2 rows)
int Unknown_791310Class::Unknown_791F10(const char *text, char prev, int *shapes)
{
	int n = 1;
	shapes[0] = 11;
	if (text[1] == 'c') {
		if (text[2] == 'h') {
			shapes[0] = 8;
			return(3);
		}
	} else if (text[1] == 'h') {
		shapes[0] = 10;
		n = 2;
	}
	return(n);
}

// 0x791F80
// NOT EXACT: functional for POC (D64), shape (registers and the branch layout)
int Unknown_791310Class::Unknown_791F80(const char *text, char prev, int *shapes)
{
	shapes[0] = 5;
	char c = text[1];
	if (c == '\0' || !IS_VOWEL(c)) {
		c = text[2];
		if (c != '\0' && !IS_VOWEL(c)) {
			shapes[0] = 5;
			shapes[1] = -1;
		}
		return(1);
	}
	if (c == 'e' || c == 'i') {
		shapes[0] = 4;
		shapes[1] = -1;
	}
	return(2);
}

/*
**	Reconstruction notes
**
**	0x791400's 't' is inline in the client (0x791F10's code, which nothing calls). The 'o' rules read
**	text[-2] and text[-1] before the buffer's start, as the client does. 0x792040 on is another file's.
*/
