// Earth & Beyond client (reconstruction): app\common\stredit.cpp. Notes after the declarations.
#include <string.h>
#include <ctype.h>
#include "stredit.h"
#include "..\kernel\kernel.h"
#include "..\input\input.h"
#include "..\shared\g_assert.h"

#define STRING_CHECK_VALUE '~'	// NAME 5: the asserts'
#include <string>

/*
**	Reconstruction notes (stredit.cpp, client 0x84AA10-0x84B350; __FILE__ 0xBB0FEC, asserts on the client's
**	lines 335 / 382 / 400, D25: blank lines keep them there).
**
**	StringEditClass (stredit.h) is the text gadgets' edit buffer: String holds MaxLen characters, the terminator and
**	STRING_CHECK_VALUE after it; Cursor points into it; InsertMode (the constructor's true) is toggled by the Insert
**	key. Process_Key takes a key event: Home / End / Left / Right (with Ctrl by words), Insert, Delete, Backspace,
**	Enter as a newline with flag 0x40, and the event's character (input.cpp's 0x42EB80) through the flags and Chars.
**	The event's modifier bits are read as bit-fields in the client (shr / test 1: key up bit 12, shift 8, ctrl 9,
**	alt 10: kernel.h's KernelEventClass bit-fields). Process_Key's key is an int local read before the up test, the
**	result an int (the strchr test's 'allowed' shares it), the character an int; c == 0 and !isprint(c) an if each, == MaxLen its own arm.
**	<string>'s guarded initialiser at 0x84B330 ends the object (workflow.md 3.3): the client's next function, 0x84B360
**	("Effect Radius : %d"), is another file's.
*/

StringEditClass::StringEditClass(int maxlen, const char *string, unsigned int flags, const char *chars)
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

StringEditClass::~StringEditClass()
{
	delete String;
}
















































































































































// The character's two rejections are an if each and a full buffer has its own result = true: see the notes above
bool StringEditClass::Process_Key(KernelEventClass *event)
{
	int key = event->Key;
	int result = false;
	if (event->Is_Up()) {
		return(false);
	}
	switch (key) {
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
			while (Cursor > String) {
				Cursor--;
				if (!event->Is_Ctrl() || isspace(*Cursor)) {
					break;
				}
			}




			result = true;
			break;

		case 0x27:
			while (Cursor < String + StrLen) {
				Cursor++;
				if (!event->Is_Ctrl() || isspace(*Cursor)) {
					break;
				}
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
			int c;
			if ((event->Flags & 0x700) && !event->Is_Shift()) {
				break;
			}
			if (key == 0x0D) {
				if (!(Flags & 0x40)) {
					break;
				}
				c = '\n';
			} else {
				c = (unsigned char)Unknown_42EB80(event);
				if (c == 0) { break; }
				if (!isprint(c)) { break; }

				if (Chars != NULL) {
					if (!(Flags & 0x1C)) {
						if (Flags & 0x20) {
							if (strchr(Chars, c) != NULL) {
								break;
							}
						} else if (strchr(Chars, c) == NULL) {
							break;
						}
					} else if (Flags & 0x20) {
						if (strchr(Chars, c) != NULL) {
							break;
						}
					} else if (strchr(Chars, c) != NULL) {
						result = true;
					}
				}
				if ((Flags & 0x200) && c == ' ') {
					result = true;
				}
				if (Flags & 1) {
					c = toupper(c);
				}
				if (Flags & 2) {
					c = tolower(c);
				}
				if (!result && (((Flags & 4) && !isdigit(c)) || ((Flags & 8) && !isalpha(c)) || ((Flags & 0x10) && !isalnum(c)))) {
					break;
				}
			}
			result = true;
			if (StrLen < MaxLen) {
				if (InsertMode) {
					memmove(Cursor + 1, Cursor, strlen(Cursor) + 1);
					StrLen++;
				} else {
					if (Cursor == String + StrLen) {
						Cursor[1] = '\0';
						StrLen++;
					}
				}
				*Cursor++ = c;
			} else if (StrLen == MaxLen) { result = true; } else {
				if (!InsertMode && Cursor != String + StrLen) {
					*Cursor++ = c;
				} else {
					result = false;
				}
			}
			break;
		}
	}

	G_ASSERT(String[MaxLen + 1] == STRING_CHECK_VALUE);
	return(result ? true : false);
}























bool StringEditClass::Insert(const char *text)
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

char *StringEditClass::Get_String(int length, char *buffer)
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

void StringEditClass::Set_String(std::string &text)
{
	Clear();
	Insert(text.c_str());
}

void StringEditClass::Insert_String(std::string &text)
{
	bool mode = InsertMode;
	InsertMode = true;
	Insert(text.c_str());
	InsertMode = mode;
}

bool StringEditClass::Set_Cursor(unsigned int position)
{
	int old = Cursor - String;
	if (position > (unsigned int)StrLen) {
		position = StrLen;
	}
	Cursor = String + position;
	return(old != Cursor - String);
}

bool StringEditClass::Move_Cursor(int delta)
{
	return(Set_Cursor((Cursor - String) + delta));
}
