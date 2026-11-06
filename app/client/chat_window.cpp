#include "chat_window.h"
#include "..\shared\g_assert.h"
#include <stdio.h>

ChatWindowClass::ChatWindowClass(void *owner, int unknown, int index) :
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

void ChatWindowClass::Clear_Fonts()
{
	for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		const RefCountPtr<ChatFontSys> &font_ptr = *it;




























































































































































































































































		FAIL_IF(!font_ptr) continue;
		font_ptr->Set_Line(RefCountPtr<ChatLineClass>());
	}
	Fonts.erase(Fonts.begin(), Fonts.end());
}

/*
**	Reconstruction notes (client.exe, the object 0x67DC80-0x68128F; vtable 0xAFBEF0).
**	The client's __FILE__ 0xB8DAC8 and its first literals: 'Chat_Line_' 0xB8DAAC (the constructor), '!font_ptr' 0xB8DABC.
**	The FAIL_IF on line 293 is Clear_Fonts's (the destructor inlines it; its own copy is 0x67F680).
*/
