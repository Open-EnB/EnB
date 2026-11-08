// Earth & Beyond client (reconstruction): app\client\ui_chat_window_split.h, the split chat window (the client's
// ui_chat_window_split.cpp, 0x571EE0-0x5734CF, by the object after ui_chat_window.cpp's; this header's name is
// provisional, NAME 1). The chat panel's ChatInt holds one (0x56C180: the kinds 0..2). Laid out from the constructor
// 0x571F00 (0x120 bytes) and the table 0xAF052C (CW2, 2026-11-08); its file is not rebuilt yet.
#ifndef UI_CHAT_WINDOW_SPLIT_H  // NAME 1 [ui_chat_window_split.h]: provisional header name for ui_chat_window_split.cpp
#define UI_CHAT_WINDOW_SPLIT_H

#include "ui_chat_window.h"

// The split chat window (0x120 bytes; constructor 0x571F00, table 0xAF052C: slots +0x00 0x572010, +0x54 0x5723B0,
// +0x58 0x572DE0, +0x5C 0x572E00, +0x60 0x572E20, +0x68 0x572EE0, +0xA8 0x573220)
class UIChatWindowSplitClass : public ChatWindowInterface	// NAME 3: class-from-file (D24) for app\client\ui_chat_window_split.cpp
{
public:
	UIChatWindowSplitClass(SClient *client, Unknown67D430Class *history, Unknown65BFD0Class *edit, int kind);	// 0x571F00 (0x56C180: the panel's +0x13C, +0x140 and the kind 0..2)  // NAME 1 [client, history, edit, kind]: what 0x56C180 passes
	virtual ~UIChatWindowSplitClass();					// 0x572040 (deleting 0x572010)
	virtual void Unknown_54();							// +0x54 0x5723B0 (the set-up: 'ChatWinSplit_*')
	virtual void Unknown_58();							// +0x58 0x572DE0
	virtual void Unknown_5C();							// +0x5C 0x572E00
	virtual void Unknown_60();							// +0x60 0x572E20
	virtual bool Unknown_68(int key);					// +0x68 0x572EE0
	virtual void Unknown_A8();							// +0xA8 0x573220

	int UnknownE8;										// +0xE8 (the kind: 0 'ChatWinSplit_None', 1 'ChatWinSplit_Vert', 2 'ChatWinSplit_Horz')
	int UnknownEC;										// +0xEC
	int UnknownF0;										// +0xF0
	int UnknownF4;										// +0xF4
	int UnknownF8;										// +0xF8 (-1)
	int UnknownFC;										// +0xFC
	int Unknown100;										// +0x100
	int Unknown104;										// +0x104
	int Unknown108;										// +0x108
	int Unknown10C;										// +0x10C (-1)
	int Unknown110;										// +0x110
	int Unknown114;										// +0x114
	bool Unknown118;									// +0x118 (the kind is 2)
	const char *Unknown11C;								// +0x11C (the kind's model name)
};

#endif
