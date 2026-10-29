// Earth & Beyond client (reconstruction): app\shared\split_string.cpp. Notes at the end of the file.
#include <string.h>
#include "split_string.h"
#include "unknown_debug.h"


SplitStringClass::SplitStringClass(const char *text, float width, int cursor) :
	Width(0),
	CursorLine(0),
	CursorColumn(0),
	NewLines(0),
	EscapedNewLines(true),
	TrimLast(true),
	TrimLines(true)
{
	if (text) {
		Split(text, width, cursor);
	}
}


SplitStringClass::~SplitStringClass()
{
	Split(NULL, 0, 0);
}


// NOT EXACT: register and stack-slot allocation of in / out / skip (the client keeps skip in the cursor argument's slot); 2 forms
bool SplitStringClass::Split(const char *text, float width, int cursor)
{
	Lines.clear();
	Width = 0;
	CursorColumn = 0;
	CursorLine = 0;
	NewLines = 0;

	if (text == NULL || *text == 0) {
		return true;
	}

	unsigned int size = strlen(text) + 1;
	float line_width = 0.0f;
	char *buf = new char[size];
	memset(buf, 0, size);
	char *out = buf;
	const char *in = text;
	const char *cursor_pos = text + cursor;

	while (*in) {
		int skip = 0;
		if (in == cursor_pos) {
			CursorLine = Lines.size();
			CursorColumn = strlen(buf);
		}

		float char_width = Get_Char_Width(*in);
		if (width > 0.0f && char_width + line_width > width) {

			// back to the last space
			for (; out > buf; out--, in--) {
				if (*out == ' ') {
					out[1] = 0;
					in++;
					line_width = Get_Text_Width(buf);
					goto add_line;
				}
			}

			// one word wider than the width: the whole text is the line
			if (strlen(buf) == 0) {
				Lines.clear();
				Width = -1.0f;
				Lines.push_back(std::string(text));
				delete [] buf;
				return false;
			}
			in += strlen(buf);

		} else {
			if (EscapedNewLines && in[0] == '\\' && in[1] == 'n') {
				skip = 2;
			} else if (in[0] == '\n') {
				skip = 1;
			} else if (in[0] == '\r' && in[1] == '\n') {
				skip = 2;
			} else {
				*out++ = *in++;
				line_width += char_width;
				continue;
			}
			NewLines++;
		}

add_line:
		if (skip) {
			memset(out, ' ', skip);
			in += skip;
		}
		std::string line(buf);
		if (TrimLines) {
			Lines.push_back(line.substr(0, line.find_last_not_of(' ') + 1));
		} else {
			Lines.push_back(line);
		}
		memset(buf, 0, size);
		out = buf;
		if (Width < line_width) {
			Width = line_width;
		}
		line_width = 0.0f;
	}

	if (in <= cursor_pos) {
		CursorLine = Lines.size();
		CursorColumn = strlen(buf);
	}

	if (*buf) {
		if (TrimLast) {
			std::string line(buf);
			unsigned int pos = line.find_last_not_of(' ');
			if (pos != std::string::npos) {
				Lines.push_back(line.substr(0, pos + 1));
			}
		} else {
			std::string line(buf);
			Lines.push_back(line);
		}
		if (Width < line_width) {
			Width = line_width;
		}
	}

	int last = Lines.size() - 1;
	if (last > CursorLine) {
		while (last > CursorLine) {
			if (Lines[last].length() != 0) {
				break;
			}
			Lines.pop_back();
			last--;
		}
	} else {
		while (last < CursorLine) {
			Lines.push_back(std::string(""));
			last++;
		}
	}

	delete [] buf;
	return true;
}


unsigned int SplitStringClass::Count()
{
	return Lines.size();
}


const char *SplitStringClass::Get(unsigned int index)
{
	if (index >= Lines.size()) {
		return NULL;
	}
	return Lines[index].c_str();
}













































































































float SplitStringClass::Get_Text_Width(const char *buff)
{
	FAIL_IF(!buff) {
		return 0.0f;
	}
	return (float)strlen(buff);
}


float SplitStringClass::Get_Char_Width(char c)
{
	return c ? 1.0f : 0.0f;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4F9BF0-0x4FA8DF: the constructor 0x4F9BF0 (with its deleting destructor 0x4F9C90),
**	the destructor 0x4F9CC0, Split 0x4F9D90, Count 0x4FA710, Get 0x4FA740, the two width virtuals
**	0x4FA790 (FAIL_IF line 279, '!buff') / 0x4FA800, std::string::find_last_not_of 0x4FA830 (the
**	instance this object emits) and the static initialiser 0x4FA8C0 that ends the object. The
**	font-measured subclass after it (0x4FA8F0) is an object of its own (unknown_font_split_string.cpp).
**	The pooled "" Split pads lines with is 0xBDA908.
*/
