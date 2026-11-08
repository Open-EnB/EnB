#include "chat_window_history.h"
#include "..\shared\g_assert.h"
#include <string.h>

// NOT EXACT: Text's copy is inlined here; the client calls string's copy constructor (0x681400), and everything after it shifts
ChatWindowHistoryClass::ChatWindowHistoryClass(int unknown) :
	Splitter(RefCountPtr<Font32Class>(), NULL, 0.0f, 0),
	Unknown44(unknown),
	Unknown48(0),
	Unknown4C(0),
	Unknown50(0),
	Unknown54(0x7FFFFF),
	Unknown58(0),
	Unknown5C(false)
{
	NullLine = Create_NEW(new ChatLineClass(-1, "", Vector3(0.0f, 0.0f, 0.0f), true, true));
	NullLine->Null = true;
	Splitter.Set_Trim_Lines(false);
	Splitter.Set_Escaped_New_Lines(false);
}

ChatWindowHistoryClass::~ChatWindowHistoryClass()
{
	History.clear();
	Unknown4C = 0;
	Unknown50 = 0;
}

void ChatWindowHistoryClass::Set_Font(const RefCountPtr<Font32Class> &font)
{
	Splitter.Set_Font(font);
	Unknown_681DB0();
	Unknown_6818C0();
}

bool ChatWindowHistoryClass::Unknown_04(unsigned int max_lines, unsigned int width)
{
	bool changed = false;
	if (Unknown48 != max_lines) {
		unsigned int count = Unknown50;
		changed = true;
		if (Unknown48 == count) {
			Unknown50 = max_lines;
			Unknown48 = max_lines;
		} else {
			Unknown48 = max_lines;
			if (count > max_lines) {
				Unknown50 = max_lines;
			}
		}
		if (Unknown50 > History.size()) {
			Unknown50 = History.size();
		}
		if (Unknown4C != 0) {
			Unknown4C = Unknown4C - Unknown50 + count;
			if (Unknown4C < 0) {
				Unknown4C = 0;
			}
		}
	}
	if (Unknown54 != width) {
		width = (width / 8) * 8;
		changed = true;
		if (width == 0) {
			width = 1;
		}
		if (Unknown54 != width) {
			Unknown54 = width;
			Unknown_681DB0();
			Unknown_6818C0();
		}
	}
	return changed;
}

void ChatWindowHistoryClass::Unknown_6818C0()
{
	std::vector< RefCountPtr<ChatLineClass> > old;
	old.swap(History);
	for (std::vector< RefCountPtr<ChatLineClass> >::iterator it = old.begin(); it != old.end(); ++it) {
		const char *text = (*it)->Text.c_str();
		if (Splitter.Split(text, (float)Unknown54, 0)) {
			int count = Splitter.Count();
			for (int i = 0; i < count; i++) {
				RefCountPtr<ChatLineClass> line = Create_NEW(new ChatLineClass(Unknown58++, Splitter.Get(i), (*it)->Color, i == 0, i + 1 == count));
				line->Set_Bit_3();
				History.push_back(line);
			}
		} else {
			RefCountPtr<ChatLineClass> line = Create_NEW(new ChatLineClass(Unknown58++, text, (*it)->Color, true, true));
			line->Unknown29 = true;
			History.push_back(line);
			line->Set_Bit_3();
		}
	}
	if (History.size() < Unknown50) {
		Unknown50 = History.size();
	}
}

// 0x681DB0
void ChatWindowHistoryClass::Unknown_681DB0()
{
	std::vector< RefCountPtr<ChatLineClass> > old;
	old.swap(History);
	std::string text;
	for (std::vector< RefCountPtr<ChatLineClass> >::iterator it = old.begin(); it != old.end(); ++it) {
		text += (*it)->Text;
		if ((*it)->Is_Last()) {
			RefCountPtr<ChatLineClass> line = Create_NEW(new ChatLineClass(Unknown58++, text, (*it)->Color, true, true));
			line->Set_Bit_3();
			History.push_back(line);
			text.erase();
		}
	}
}

int ChatWindowHistoryClass::Unknown_10(int unknown)
{
	unsigned int page = Unknown48;
	int result = 0;
	if (page > 0 && unknown != 0) {
		int before = History.size() - Unknown50 - Unknown4C;
		if (unknown > 0) {
			if (Unknown5C && unknown >= (int)page) {
				unknown = page;
			}
			Unknown4C += unknown;
		} else {
			if (Unknown5C) {
				unknown = -unknown;
				if (unknown >= (int)page) {
					unknown = page;
				}
			} else {
				unknown = -unknown;
			}
			Unknown4C -= unknown;
		}
		while (Unknown4C > 0 && Unknown50 < page) {
			Unknown4C--;
			Unknown50++;
		}
		int limit = History.size() - Unknown50;
		if (Unknown4C > limit) {
			Unknown4C = limit;
		}
		while (Unknown4C < 0) {
			Unknown4C++;
			if (Unknown50 != 0) {
				Unknown50--;
			}
		}
		result = History.size() - Unknown4C - Unknown50 - before;
	}
	return result;
}

void ChatWindowHistoryClass::Unknown_0C(const Vector3 &color, const std::string &text)
{
	Unknown4C = 0;
	ChatWindowHistoryClass::Unknown_08(color, text);
}

// NOT EXACT: slots and registers: count in the text parameter's slot, i in ebx; the loop's index test calls size() out of line (0x6824F0)
int ChatWindowHistoryClass::Unknown_08(const Vector3 &color, const std::string &text)
{
	if (text.empty()) {
		return 0;
	}
	Splitter.Set_Trim_Last(false);
	Splitter.Set_Escaped_New_Lines(false);
	Splitter.Split(text.c_str(), (float)Unknown54, 0);
	int count = Splitter.Count();
	if (count == 1) {
		RefCountPtr<ChatLineClass> line = Create_NEW(new ChatLineClass(Unknown58++, text, color, true, true));
		History.push_back(line);
		Unknown_682730(History.size() - Unknown48 - 1)->Set_Bit_3();
	} else {
		for (int i = 0; i < count; i++) {
			RefCountPtr<ChatLineClass> line = Create_NEW(new ChatLineClass(Unknown58++, Splitter.Get(i), color, i == 0, i + 1 == count));
			History.push_back(line);
			Unknown_682730(History.size() - Unknown48 - 1)->Set_Bit_3();
		}
	}
	for (int i = 0; i < count; i++) {
		if (Unknown4C != 0) {
			Unknown4C++;
		} else {
			if (Unknown50 >= Unknown48) {
				break;
			}
			Unknown50++;
		}
	}
	unsigned int keep = Unknown44 > Unknown48 ? Unknown44 : Unknown48;
	while (History.size() > keep) {
		std::vector< RefCountPtr<ChatLineClass> >::iterator it = History.begin();
		while (!(*it)->Is_Last()) {
			++it;
		}
		History.erase(History.begin(), it + 1);
	}
	return count;
}

const RefCountPtr<ChatLineClass> &ChatWindowHistoryClass::Unknown_682730(unsigned int index)
{
	if (index >= History.size()) {
		return NullLine;
	}
	return History[index];
}

// NOT EXACT: the walk back: the client keeps the mask 2 in al and an unrotated loop (0x6827BB); this rotates it
const RefCountPtr<ChatLineClass> &ChatWindowHistoryClass::Unknown_682770(unsigned int index)
{
	if (History.empty()) {
		return NullLine;
	}
	std::vector< RefCountPtr<ChatLineClass> >::iterator it = History.end() - 1;






























































































































































































	G_ASSERT((*it)->Is_Last());
	unsigned int count;
	for (count = 1; !(*it)->Is_First(); --it, count++) {



		FAIL_IF(it == History.begin()) break;
	}
	if (index >= count) {
		return NullLine;
	}
	if (index != 0) {
		it += index;
	}
	return *it;
}

int ChatWindowHistoryClass::Unknown_682840()
{
	return History.size() - Unknown50 - Unknown4C;
}

int ChatWindowHistoryClass::Unknown_682880()
{
	int count = History.size() - Unknown48;
	return (count > 0) ? count : 0;
}

bool ChatWindowHistoryClass::Unknown_14(const char *command)
{
	if (strncmp(command, "/chat page", strlen("/chat page")) == 0) {
		Unknown5C = !Unknown5C;
		return true;
	}
	return false;
}

/*
**	Reconstruction notes (client.exe, the object 0x6812A0-0x6831CF; vtable 0xAFC180).
**	The client's __FILE__ 0xB8DBA8 is used by 0x682770 (lines 412 and 418), which walks this class's vector at +8 (the assert's
**	History) and returns its +0x40. The vtable's slots: 0x681610 (deleting), 0x6817E0, 0x6821F0, 0x6821C0, 0x682080, 0x6828D0.
*/
