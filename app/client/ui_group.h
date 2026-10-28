// Earth & Beyond client (reconstruction): app\client\ui_group.h, the group window (the client's ui_group.cpp per its
// asserts; this header's name is provisional, NAME 1). Moved out of cpacket.cpp (HN2, 2026-10-28).
#ifndef UI_GROUP_H  // NAME 1 [ui_group.h]: provisional header name for ui_group.cpp
#define UI_GROUP_H

#include "sclient.h"

// ui_group.cpp's group window (the main view's +0x1D8), as far as cpacket.cpp and the main view call it
class UIGroupClass : public UIUnknownBaseClass, public RefCountClass	// NAME 3: class-from-file (D24) for app\client\ui_group.cpp
{
public:
	UIGroupClass(SClient *client);						// 0x593BD0 (0xF4 bytes; the main view's 0x50C690)  // NAME 1 [client]: what 0x50C690 passes
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50();
	virtual void Unknown_54();							// +0x54 (0x50C690)
	virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60(); virtual void Unknown_64();
	virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70(); virtual void Unknown_74();
	virtual void Unknown_78();
	virtual void Unknown_7C(MainViewClass *view, int arg);	// +0x7C (0x50C690: the view, 0)  // NAME 1 [view, arg]: what 0x50C690 passes
	void Unknown_597080(const char *text, const char *name, int index);	// 0x597080 (CLIENT_CHAT_EVENT 165: a 'group' message '<S>n name`text')  // NAME 1 [text, name, index]: the message's parts
	char Unknown40[0xF4 - 0x40];						// +0x40
};

#endif // UI_GROUP_H
