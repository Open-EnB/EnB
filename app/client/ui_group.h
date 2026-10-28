// Earth & Beyond client (reconstruction): app\client\ui_group.h, the group window (the client's ui_group.cpp per its
// asserts; this header's name is provisional, NAME 1). Moved out of cpacket.cpp (HN2, 2026-10-28).
#ifndef UI_GROUP_H  // NAME 1 [ui_group.h]: provisional header name for ui_group.cpp
#define UI_GROUP_H

#include "sclient.h"

// ui_group.cpp's group window (the main view's +0x1D8), as far as cpacket.cpp and the main view call it
class UIGroupClass : public UIUnknownBaseClass, public RefCountClass	// NAME 3: class-from-file (D24) for app\client\ui_group.cpp
{
public:
	void Unknown_597080(const char *text, const char *name, int index);	// 0x597080 (CLIENT_CHAT_EVENT 165: a 'group' message '<S>n name`text')  // NAME 1 [text, name, index]: the message's parts
};

#endif // UI_GROUP_H
