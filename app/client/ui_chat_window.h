// Earth & Beyond client (reconstruction): app\client\ui_chat_window.h, the chat window (the client's ui_chat_window.cpp
// per its asserts, 0x56F420-0x571EAF; this header's name is provisional, NAME 1). Laid out from the constructor
// 0x56F420 (the split window's 0x571F00 passes its first three arguments on) and the table 0xAF0390 (CW2, 2026-11-08).
#ifndef UI_CHAT_WINDOW_H  // NAME 1 [ui_chat_window.h]: provisional header name for ui_chat_window.cpp
#define UI_CHAT_WINDOW_H

#include <vector>
#include "ui_chat_panel_base.h"
#include "..\..\wwmath\rect.h"

class FontSys;					// fontmanager.h's (not included here: ui_chat_panel.cpp declares its own)
class TextCursorClass;			// fontsys_cursor.h's
class ButtonGadgetClass;		// unknown_675120.h's
class ChatWindowClass;			// chat_window.h's
class Unknown67D430Class;		// unknown_67d430.h's: the chat panel's ring of chat lines (+0x13C)
class Unknown65BFD0Class;		// chatgadg.cpp's text entry (0x80 bytes, constructor 0x65BFD0): the chat panel's +0x140
struct Unknown67D430EntryStruct;

// What the chat window's events carry, as far as 0x56C730 reads it
class ChatWindowInterfaceEvent { public: ChatWindowInterfaceEvent() : Unknown00(0) {} ChatWindowInterfaceEvent(int type) : Unknown00(type) {} int Unknown00; };	// NAME 5: the client's RTTI (.?AVChatWindowInterfaceEvent@@ 0xB7D4B0, the Event_Type of the callback table 0xAEFFF8): +0x00 1 is a click (0x56C6A0 passes a zeroed one to name the handler; 0x570390 emits 1)  // NAME 1 [type]: neutral

// A part of the model the text goes in: its rectangle between two bones (0x18 bytes; the window's +0xBC and each
// chat_win's +0x00, the asserts' 'vp')
struct ChatViewportStruct	// NAME 1: neutral (D10)
{
	ChatViewportStruct() : TLBone(0), BRBone(0) {}
	RectClass Rect;										// +0x00 (0x571A70: the bones' x and y)  // NAME 1: neutral
	int TLBone;											// +0x10  // NAME 3: the assert's 'chat_win->TLBone'
	int BRBone;											// +0x14  // NAME 3: the assert's 'chat_win->BRBone'
};

// One chat window's part of the model (0x1C bytes, 0x56FD30 news them; the asserts' 'chat_win', 'window')
struct ChatWinStruct : public ChatViewportStruct	// NAME 1: neutral (D10)
{
	ChatWinStruct(GadgetSlider *slider, unsigned int index);	// (inline in 0x56FD30)  // NAME 1 [slider, index]: neutral
	~ChatWinStruct();									// (inline in 0x56F560)
	ChatWindowClass *ChatWindow;						// +0x18 (0xC0 bytes, 0x67DC80)  // NAME 3: the assert's 'window->ChatWindow'
};

// The chat window (0xE8 bytes; constructor 0x56F420, destructor 0x56F560, table 0xAF0390); the chat panel's ChatInt
// holds the split window derived from it (ui_chat_window_split.cpp, 0x571F00)
class ChatWindowInterface : public UIChatPanelBaseClass	// NAME 5: the client's RTTI (.?AVChatWindowInterface@@ 0xB7D3E0, the Sender_Type of the callback table 0xAEFFF8 and of 0x570390's emit): the assert's 'ChatInt'
{
public:
	ChatWindowInterface(SClient *client, Unknown67D430Class *history, Unknown65BFD0Class *edit);	// 0x56F420  // NAME 3 [history]: the asserts' ChatHistory; NAME 1 [client, edit]: neutral
	virtual ~ChatWindowInterface();						// 0x56F560 (deleting 0x56F530)
	virtual void Unknown_54();							// +0x54 0x56F780 (G_ASSERT '!Interface_Mesh()' l.119: the set-up)
	virtual void Unknown_58();							// +0x58 0x5701C0 (activated)
	virtual void Unknown_5C();							// +0x5C 0x570210 (deactivated)
	virtual void Unknown_60();							// +0x60 0x570340 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x570390 (the keys 1-6 and the sliders 8 on)
	virtual void Unknown_70(ControlKeysManager *manager);	// +0x70 0x570510 (FAIL_IF '!manager' l.331)  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_74(ControlKeysManager *manager);	// +0x74 0x5708F0 (FAIL_IF '!manager' l.343)  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x570260 (onto the view: the windows into its scene)  // NAME 1 [view, arg]: intrface.h's
	virtual bool Unknown_98(const char *name, const char *ini);	// +0x98 0x56F9F0 (G_ASSERT 'false' l.163: the model's parts)  // NAME 1 [name, ini]: the base's
	virtual void Unknown_A0(unsigned int count);		// +0xA0 0x56FD30 (the chat windows made, the ring's lines put in them)  // NAME 1 [count]: neutral
	virtual void Unknown_A4();							// +0xA4 0x570CD0 (the viewports from the bones)
	virtual void Unknown_A8();							// +0xA8 0x571210 (the frame shown or hidden)
	virtual void Unknown_AC(const char *text);			// +0xAC 0x570F50 (the text entry's 0x65E7C0)  // NAME 1 [text]: neutral
	virtual void Unknown_B0();							// +0xB0 0x570FF0 (the ring's newest line shown)
	virtual void Unknown_B4(const char *text);			// +0xB4 0x571170 (FAIL_IF '!DestBttnText' l.425: 0x565C40's 'To %s:')  // NAME 1 [text]: neutral
	virtual bool Unknown_B8(int key);					// +0xB8 0x5711C0 (0x5638E0 hands slot +0x68's key on: each window's slot +0x14)  // NAME 1 [key]: what 0x5638E0 passes
	virtual void Unknown_BC(int lines, unsigned int index);	// +0xBC 0x571860 (scrolled by lines)  // NAME 1 [lines, index]: neutral
	virtual void Unknown_C0(int lines, unsigned int index);	// +0xC0 0x5718B0 (scrolled back by lines)  // NAME 1 [lines, index]: neutral
	virtual void Unknown_C4(int pages, unsigned int index);	// +0xC4 0x571900 (scrolled by pages: the window's +0x48 lines)  // NAME 1 [pages, index]: neutral
	virtual void Unknown_C8(int pages, unsigned int index);	// +0xC8 0x571950 (scrolled back by pages)  // NAME 1 [pages, index]: neutral
	virtual void Unknown_CC();							// +0xCC 0x570E40 (the viewport, the windows redrawn)

	Unknown65BFD0Class *Unknown_570F20();				// 0x570F20 (0x63A100): the text entry while it has the focus
	void Unknown_571430();								// 0x571430 (0x56C7B0, on the +0xF8 window's events)
	void Unknown_571470(bool force);					// 0x571470: the entry's text into the line  // NAME 1 [force]: neutral
	void Unknown_5715F0(bool on, bool force);			// 0x5715F0: the entry line shown or hidden  // NAME 1 [on, force]: neutral
	ChatWindowClass *Unknown_571810(unsigned int index);	// 0x571810 (out of line, uncalled: the file's callers inline it)  // NAME 1 [index]: neutral
	Vector3 Unknown_5719A0(const Unknown67D430EntryStruct &entry);	// 0x5719A0 (FAIL_IFs '!client', '!sclient' ll.612 / 615: the line type's colour)  // NAME 1 [entry]: neutral
	void Unknown_571A70(ChatViewportStruct *vp);		// 0x571A70 (FAIL_IF '!vp' l.625: the rectangle from the bones)  // NAME 3 [vp]: the FAIL_IF's

	Unknown67D430Class *ChatHistory;					// +0xA0  // NAME 3: the assert's '!ChatHistory'
	Unknown65BFD0Class *UnknownA4;						// +0xA4 (the text entry; the gadget 7)
	std::vector<ChatWinStruct *> ChatWindows;			// +0xA8  // NAME 3: the assert's '!ChatWindows.empty()'
	RefCountPtr<FontSys> UnknownB8;						// +0xB8 ('Chat_Line': the entry's text)
	ChatViewportStruct Viewport;						// +0xBC ('chat_vp_tl', 'chat_vp_br')  // NAME 1: neutral
	RefCountPtr<RenderObjClass> UnknownD4;				// +0xD4 ('TEXTBOX')
	RefCountPtr<TextCursorClass> UnknownD8;				// +0xD8 (the entry's cursor, 0x685680)
	ButtonGadgetClass *UnknownDC;						// +0xDC ('DEST_BOX', the gadget 1)
	RefCountPtr<FontSys> DestBttnText;					// +0xE0 ('Chat_Dest_Bttn')  // NAME 3: the assert's '!DestBttnText'
	bool UnknownE4;										// +0xE4 (0x571210 flips it: the frame hidden)
};

#endif
