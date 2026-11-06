#include "chat_window.h"
#include "..\shared\g_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ChatWindowClass::ChatWindowClass(GadgetSlider *owner, int unknown, int index) :
	ChatWindowHistoryClass(unknown),
	Unknown64(0),
	Unknown68(0),
	Unknown6C(0),
	Unknown70(0),
	Unknown74(owner),
	Unknown78(0),
	Unknown90(0),
	Unknown94(0),
	Unknown98(0),
	Unknown9C(true),
	Unknown9D(false),
	Unknown9E(false),
	UnknownA0(0),
	UnknownA4(0),
	UnknownA8(1),
	Name("Chat_Line_")
{
	char buffer[12];
	sprintf(buffer, "%02d", index);
	Name += buffer;
	Unknown_67E670();
}

ChatWindowClass::~ChatWindowClass()
{
	Clear_Fonts();
	Unknown60 = NULL;
	Unknown74 = NULL;
}

void ChatWindowClass::Unknown_67E180(const RefCountPtr<ChatLineClass> &line)
{
	Unknown60 = line;
	for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		const RefCountPtr<ChatFontSys> &font_ptr = *it;







































































































		FAIL_IF(!font_ptr) continue;
		font_ptr->Remove();
	}
	Unknown9C = true;
}

void ChatWindowClass::Unknown_0C(int unknown1, int unknown2)
{
	Unknown9D = true;
	ChatWindowHistoryClass::Unknown_0C(unknown1, unknown2);
}

int ChatWindowClass::Unknown_08(int unknown1, int unknown2)
{
	int count = ChatWindowHistoryClass::Unknown_08(unknown1, unknown2);
	if (count == 0) {
		return 0;
	}
	if (Fonts.empty()) {
		return count;
	}
	Unknown9C = true;
	if (Unknown4C != 0) {
		Unknown74->Unknown_6702D0();
		Unknown_680790();
		return count;
	}
	for (int i = 0; i < count; i++) {
		const RefCountPtr<ChatLineClass> &chat_line = Unknown_682770(i);


















		FAIL_IF(chat_line->Is_Null()) break;
		const RefCountPtr<ChatFontSys> &line_ptr = Get_Font(chat_line->Unknown08);

		FAIL_IF(!line_ptr) continue;
		line_ptr->Set_Line_Always(chat_line);
	}
	Unknown90 = Unknown_682730(Unknown_682840())->Unknown08;
	Unknown_6809B0();
	Unknown_680790();
	return count;
}

int ChatWindowClass::Unknown_10(int unknown)
{
	int result = ChatWindowHistoryClass::Unknown_10(unknown);
	if (result != 0) {
		Unknown_67FC60();
		Unknown_680790();
	}
	return result;
}

bool ChatWindowClass::Unknown_14(const char *command)
{
	if (strncmp(command, "/chat render", strlen("/chat render")) == 0) {
		UnknownA0 = atoi(command + strlen("/chat render") + 1);
		return true;
	}
	if (strncmp(command, "/chat extra", strlen("/chat extra")) == 0) {
		int value = atoi(command + strlen("/chat extra") + 1);
		if (UnknownA4 != value) {
			UnknownA4 = value;
			Unknown_04(Unknown48, Unknown54);
		}
		return true;
	}
	if (strncmp(command, "/chat build", strlen("/chat build")) == 0) {
		UnknownA8 = atoi(command + strlen("/chat build") + 1);
		return true;
	}
	return false;
}

bool ChatWindowClass::Unknown_04(unsigned int max_lines, unsigned int width)
{
	bool changed = ChatWindowHistoryClass::Unknown_04(max_lines, width);
	if (Unknown_67F7F0(max_lines)) {
		changed = true;
	}
	if (changed) {
		Unknown_67FC60();
		Unknown_680790();
	}
	return changed;
}

// NOT EXACT: the clamp of `spaces` only: the client tests spaces < 1 twice (0x67E78C, 0x67E79D); VC6 folds the second here
void ChatWindowClass::Unknown_67E670()
{
	RefCountPtr<ChatFontSys> font = Create_NEW(new ChatFontSys(Name.c_str()));
	Font = *(const RefCountPtr<Font32Class> *)&font->Font;  // FontSys::Font is held like a RefCountPtr here (0x67E71F)
	int width = (int)(Font->Get_Char_Width(' ', 640.0f) < 1.0f ? 1.0f : Font->Get_Char_Width(' ', 640.0f));
	int spaces = 8 / width;
	int count = (unsigned int)(spaces < 1 ? 1 : spaces) > 3 ? 3 : (spaces >= 1 ? spaces : 1);
	memset(UnknownBC, ' ', count);
	UnknownBC[count] = 0;
	Set_Font(Font);
}

void ChatWindowClass::Clear_Fonts()
{
	for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		const RefCountPtr<ChatFontSys> &font_ptr = *it;


























		FAIL_IF(!font_ptr) continue;
		font_ptr->Set_Line(RefCountPtr<ChatLineClass>());
	}
	Fonts.erase(Fonts.begin(), Fonts.end());
}

void ChatWindowClass::Unknown_680670()
{
	if (Unknown_10(Unknown94 - Get_Slider_Value())) {
		Unknown9C = true;
	}
	Unknown94 = Get_Slider_Value();
}

void ChatWindowClass::Unknown_6806C0()
{
	Clear_Fonts();
	Unknown_67E670();
	Unknown_67F7F0(Unknown48);
	Unknown_6804E0(&Unknown64);
	Unknown9D = true;
}

void ChatWindowClass::Unknown_680700(bool unknown)
{
	Unknown9E = unknown;
	Unknown_680790();
	if (Unknown9E) {
		for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
			const RefCountPtr<ChatFontSys> &font_ptr = *it;
































































































































































































































			FAIL_IF(!font_ptr) continue;
			font_ptr->Remove();
		}
	} else {
		Unknown9C = true;
	}
}

const RefCountPtr<ChatFontSys> &ChatWindowClass::Get_Font(unsigned int index)
{
	static RefCountPtr<ChatFontSys> null_font;
	unsigned int size = Fonts.size();







































	FAIL_IF(!size) return null_font;
	return Fonts[index % size];
}

/*
**	Reconstruction notes (client.exe, the object 0x67DC80-0x68128F; vtable 0xAFBEF0).
**	The client's __FILE__ 0xB8DAC8 and its first literals: 'Chat_Line_' 0xB8DAAC (the constructor), '!font_ptr' 0xB8DABC.
**	The FAIL_IF on line 293 is Clear_Fonts's (the destructor inlines it; its own copy is 0x67F680).
*/
