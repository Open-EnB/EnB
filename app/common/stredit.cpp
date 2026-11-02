// Earth & Beyond client (reconstruction): app\common\stredit.cpp. Notes after the declarations.
#include <string.h>
#include <ctype.h>
#include "stredit.h"
#include "..\kernel\kernel.h"
#include "..\input\input.h"
#include "..\shared\unknown_debug.h"

#define STRING_CHECK_VALUE '~'	// NAME 5: the asserts'


/*
**	Reconstruction notes (stredit.cpp, client 0x84AA10-0x84B200 so far; __FILE__ 0xBB0FEC, asserts on the client's
**	lines 335 / 382 / 400, D25: blank lines keep them there).
**
**	StrEditClass (stredit.h) is the text gadgets' edit buffer: String holds MaxLen characters, the terminator and
**	STRING_CHECK_VALUE after it; Cursor points into it; InsertMode (the constructor's true) is toggled by the Insert
**	key. Process_Key takes a key event: Home / End / Left / Right (with Ctrl by words), Insert, Delete, Backspace,
**	Enter as a newline with flag 0x40, and the event's character (input.cpp's 0x42EB80) through the flags and Chars.
**	The event's modifier bits are read as bit-fields in the client (shr / test 1: key up bit 12, shift 8, ctrl 9,
**	alt 10), which kernel.h's KernelEventClass does not declare: Process_Key is NOT EXACT for it.
**	Not yet: 0x84B200 (Clear, then Insert of a std::string's text), 0x84B270 and the file's rest to 0x84B8A0
**	(the asserts 'r >= VENDOR_TYPE_NUM' / 'r <= VENDOR_TYPE_INVALID').
*/

StrEditClass::StrEditClass(int maxlen, const char *string, unsigned int flags, const char *chars)
{
	String = new char[maxlen + 2];
	Chars = chars;
	Flags = flags;
	MaxLen = maxlen;
	InsertMode = true;
	memset(String, 0, maxlen + 1);
	if (string != NULL) {
		strncpy(String, string, maxlen);
	}
	StrLen = strlen(String);
	Cursor = String + StrLen;
	String[maxlen + 1] = STRING_CHECK_VALUE;
}

StrEditClass::~StrEditClass()
{
	delete String;
}
















































































































































// NOT EXACT: the modifier bits (0.58): the client tests the event's flags as bit-fields, see the notes
bool StrEditClass::Process_Key(KernelEventClass *event)
{
	bool result = false;
	if (event->Is_Up()) {
		return(false);
	}
	switch (event->Key) {
		case 1: case 2: case 9: case 0x0C:
		case 0x10: case 0x11: case 0x12: case 0x13: case 0x14:
		case 0x1B: case 0x21: case 0x22: case 0x26: case 0x28: case 0x2C:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75:
		case 0x76: case 0x77: case 0x78: case 0x79: case 0x7A: case 0x7B:
		case 0x90: case 0x91:
			break;

		case 0x23:
			Cursor = String + StrLen;
			result = true;
			break;

		case 0x24:
			Cursor = String;
			result = true;
			break;

		case 0x25:
			if (Cursor > String) {
				do {
					Cursor--;
					if (!event->Is_Ctrl() || isspace(*Cursor)) {
						result = true;
						goto done;
					}
				} while (Cursor > String);
			}
			result = true;
			break;

		case 0x27:
			if (Cursor < String + StrLen) {
				do {
					Cursor++;
					if (!event->Is_Ctrl() || isspace(*Cursor)) {
						result = true;
						goto done;
					}
				} while (Cursor < String + StrLen);
			}
			result = true;
			break;

		case 0x2D:
			InsertMode = !InsertMode;
			result = true;
			break;

		case 0x2E:
			if (Cursor != String + StrLen) {
				memcpy(Cursor, Cursor + 1, strlen(Cursor));
				StrLen--;
				result = true;
				break;
			}
			// at the end: as backspace

		case 8:
			if (Cursor > String) {
				Cursor--;
				memcpy(Cursor, Cursor + 1, strlen(Cursor));
				StrLen--;
			}
			result = true;
			break;

		default:
		{
			char c;
			if ((event->Flags & 0x700) && !event->Is_Shift()) {
				break;
			}
			if (event->Key == 0x0D) {
				if (!(Flags & 0x40)) {
					break;
				}
				c = '\n';
			} else {
				c = Unknown_42EB80(event);
				if (c == 0 || !isprint(c)) {
					break;
				}
				bool allowed = false;
				if (Chars != NULL) {
					if (!(Flags & 0x1C)) {
						if (!(Flags & 0x20)) {
							if (strchr(Chars, c) == NULL) {
								break;
							}
						} else if (strchr(Chars, c) != NULL) {
							break;
						}
					} else if (Flags & 0x20) {
						if (strchr(Chars, c) != NULL) {
							break;
						}
					} else if (strchr(Chars, c) != NULL) {
						allowed = true;
					}
				}
				if ((Flags & 0x200) && c == ' ') {
					allowed = true;
				}
				if (Flags & 1) {
					c = toupper(c);
				}
				if (Flags & 2) {
					c = tolower(c);
				}
				if (!allowed && (((Flags & 4) && !isdigit(c)) || ((Flags & 8) && !isalpha(c)) || ((Flags & 0x10) && !isalnum(c)))) {
					break;
				}
			}
			result = true;
			if (StrLen < MaxLen) {
				if (!InsertMode) {
					if (Cursor == String + StrLen) {
						Cursor[1] = '\0';
						StrLen++;
					}
				} else {
					memmove(Cursor + 1, Cursor, strlen(Cursor) + 1);
					StrLen++;
				}
				*Cursor++ = c;
			} else if (StrLen != MaxLen) {
				if (!InsertMode && Cursor != String + StrLen) {
					*Cursor++ = c;
				} else {
					result = false;
				}
			}
			break;
		}
	}
done:
	G_ASSERT(String[MaxLen + 1] == STRING_CHECK_VALUE);
	return(result);
}























bool StrEditClass::Insert(const char *text)
{
	if (text == NULL || *text == '\0') {
		return(false);
	}
	if (InsertMode) {
		int len = strlen(text);
		if (len + StrLen > MaxLen) {
			len = MaxLen - StrLen;
		}
		if (len == 0) {
			return(false);
		}
		char *src = Cursor;
		char *dst = Cursor + len;
		for (int i = strlen(Cursor); i >= 0; i--) {
			dst[i] = src[i];
		}
		memcpy(Cursor, text, len);
		Cursor += len;
		StrLen += len;
		G_ASSERT(StrLen == (int)strlen(String));
	} else {
		while (*text != '\0') {
			if (StrLen >= MaxLen) {
				*Cursor = '\0';
				return(false);
			}
			*Cursor++ = *text++;
			if (Cursor >= String + StrLen) {
				StrLen++;
				*Cursor = '\0';
			}
		}
	}




	G_ASSERT(String[MaxLen + 1] == STRING_CHECK_VALUE);
	return(true);
}

char *StrEditClass::Get_String(int length, char *buffer)
{
	if (length == -1) {
		length = StrLen + 1;
	}
	if (buffer == NULL) {
		buffer = new char[length];
	}
	strncpy(buffer, String, length);
	return(buffer);
}
