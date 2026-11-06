#include "chat_window_history.h"
#include "..\shared\g_assert.h"

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

/*
**	Reconstruction notes (client.exe, the object 0x6812A0-0x6831CF; vtable 0xAFC180).
**	The client's __FILE__ 0xB8DBA8 is used by 0x682770 (lines 412 and 418), which walks this class's vector at +8 (the assert's
**	History) and returns its +0x40. The vtable's slots: 0x681610 (deleting), 0x6817E0, 0x6821F0, 0x6821C0, 0x682080, 0x6828D0.
*/
