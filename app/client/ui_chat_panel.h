// Earth & Beyond client (reconstruction): app\client\ui_chat_panel.h, the chat panel (the client's ui_chat_panel.cpp
// per its asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x561DD0 (0x268 bytes;
// SClient::Init news it) and the table 0xAEFF1C (CH1, 2026-10-31).
#ifndef UI_CHAT_PANEL_H  // NAME 1 [ui_chat_panel.h]: provisional header name for ui_chat_panel.cpp
#define UI_CHAT_PANEL_H

#include <string>
#include <vector>
#include "sclient.h"

class RenderObjClass;
class GadgetClass;			// gadget.cpp's

class AvatarPipClass;		// avatar_pip.cpp's (the panel's +0x110; declared in ui_chat_panel.cpp)

// A combo of the panel (Combo[]), as far as this file calls it
class UIChatComboClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_57B440();								// 0x57B440 (slot +0x7C, before each combo's slot +0x7C)
	void Unknown_57AD50(const char *text);				// 0x57AD50 (0x5698F0: +0x1F4's text for the first)  // NAME 1 [text]: neutral
};

// What the chat window's events carry, as far as 0x56C730 reads it
class UIChatIntEventClass { public: UIChatIntEventClass() : Unknown00(0) {} int Unknown00; };	// NAME 1: neutral (D10): +0x00 1 is a click (0x56C6A0 passes a zeroed one to name the handler)

class UIChatF8EventClass;

enum { NUM_COMBO = 5 };	// NAME 3: the assert's

// ui_chat_panel_base.cpp's class (0xA0 bytes; constructor 0x56D280 (the client to 0x540D30), destructor 0x56D300,
// table 0xAF00B4), as far as the chat panel uses it (interim placement here, D137: its own file is not rebuilt)
class UIChatPanelBaseClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_chat_panel_base.cpp
{
public:
	UIChatPanelBaseClass(SClient *client);				// 0x56D280  // NAME 1 [client]: what 0x561DD0 passes
	virtual void Unknown_58();							// +0x58 0x56D4F0
	virtual void Unknown_88();							// +0x88 0x56DD80
	virtual void Unknown_8C();							// +0x8C 0x56DE60
	virtual int Unknown_90() { return(0); }				// +0x90 0x5623B0 (emitted with the chat panel's table)
	virtual void Unknown_94() {}						// +0x94 0x5623D0
	virtual void Unknown_98();							// +0x98 0x56DB20
	virtual void Unknown_9C() {}						// +0x9C 0x5623F0

	RenderObjClass *Unknown84;							// +0x84 (its slot +0x70 finds the panel's parts by name; the font styles' owner)
	RenderObjClass *Peek_Unknown84() { return(Unknown84); }	// NAME 1: neutral (D10)
	std::vector<void *> Unknown88;						// +0x88 (the constructor: the allocator byte, three pointers)
	int Unknown98;										// +0x98
	int Unknown9C;										// +0x9C
};

// The base of the windows the panel holds (ChatInt, +0xF8, +0x104): an int at +0x98 (0x565F80: set, the window closed)
class UIChatPanelWindowClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	char Unknown84[0x98 - 0x84];						// +0x84
	int Unknown98;										// +0x98 (0x5630B0 / 0x565F80 test it)
};

// What the chat panel's ChatInt (+0x138) points to (the chat window), as far as this file calls it
class UIChatIntClass : public UIChatPanelWindowClass	// NAME 1: neutral (D10): the assert's 'ChatInt'
{
public:
	virtual void Unknown_88(); virtual void Unknown_8C(); virtual void Unknown_90(); virtual void Unknown_94();
	virtual void Unknown_98(); virtual void Unknown_9C(); virtual void Unknown_A0(); virtual void Unknown_A4();
	virtual void Unknown_A8(); virtual void Unknown_AC(bool on); virtual void Unknown_B0();
	virtual void Unknown_B4(const char *text);			// +0xB4 (0x565C40: 'To %s:')  // NAME 1 [text]: neutral
	virtual bool Unknown_B8(int key);					// +0xB8 (0x5638E0 hands slot +0x68's key on)  // NAME 1 [key]: what 0x5638E0 passes

	UIChatIntClass(SClient *client, int a, int b, int kind);	// 0x571F00 (0x120 bytes; 0x56C180: +0x13C, +0x140 and the kind 0..2)  // NAME 1 [client, a, b, kind]: what 0x56C180 passes
	void Unknown_5417D0(InterfaceClass *owner);			// 0x5417D0 (intrface.cpp's; 0x56C180 passes the panel)  // NAME 1 [owner]: neutral
	void Unknown_571430();								// 0x571430 (0x56C7B0, on the +0xF8 window's events)

};

// The chat panel (SClient +0x127C; 0x268 bytes, constructor 0x561DD0, table 0xAEFF1C)
class UIChatPanelClass : public UIChatPanelBaseClass	// NAME 3: class-from-file (D24) for app\client\ui_chat_panel.cpp
{
public:
	UIChatPanelClass(SClient *client);					// 0x561DD0
	virtual ~UIChatPanelClass();						// 0x562410
	virtual void Unknown_58();							// 0x562F50 (the pip set, the chat window and the reply list activated)
	virtual void Unknown_5C();							// 0x563060 (the combo released, the chat window deactivated)
	virtual void Unknown_60();							// 0x5630B0 (every frame)
	virtual bool Unknown_68(int key);					// 0x563280
	virtual void Unknown_7C(UIViewClass *view, int arg);	// 0x563620 (onto the view with its windows and combos)  // NAME 1 [view, arg]: intrface.h's
	virtual void Unknown_70(ControlKeysManagerClass *manager);	// 0x56BC10 (FAIL_IF '!manager' l.2309)  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_74(ControlKeysManagerClass *manager);	// 0x56BD80 (FAIL_IF '!manager' l.2317)  // NAME 3 [manager]: the FAIL_IF's

	bool Unknown_5638E0(int key);						// 0x5638E0: the chat window's slot +0xB8, false without one  // NAME 1 [key]: neutral
	void Unknown_563910(unsigned id, bool flag);		// 0x563910 (CLIENT_CHAT_EVENT 165: the sender's game id, 0; the main view: the target, true)  // NAME 1 [id, flag]: what cpacket.cpp passes
	void Unknown_563C50();								// 0x563C50: the pip's target checked
	void Unknown_563DC0(bool on);						// 0x563DC0 (G_ASSERT '!font' l.710)  // NAME 1 [on]: neutral
	void Unknown_564030(bool on);						// 0x564030  // NAME 1 [on]: neutral
	void Unknown_564150(bool on);						// 0x564150 (FAIL_IFs '!big_robj', '!bigbutframe' ll.782 / 783)  // NAME 1 [on]: neutral
	void Unknown_564400(bool on);						// 0x564400 (FAIL_IFs '!small_robj', '!smallbutframe' ll.798 / 799)  // NAME 1 [on]: neutral
	void Unknown_5646A0(int channel);					// 0x5646A0  // NAME 1 [channel]: neutral
	void Unknown_564F50();								// 0x564F50 (0x65DE90): +0x1F4 copied to +0x204, 0x5646A0 on the default channel
	void Unknown_565100();								// 0x565100: 0x5646A0 on +0x14C
	void Unknown_565120();								// 0x565120
	void Unknown_565390(int arg);						// 0x565390  // NAME 1 [arg]: neutral
	const char *Unknown_565BA0();						// 0x565BA0: +0x248's text
	const char *Unknown_565BC0();						// 0x565BC0: the +0x158 name for +0x148 (6..9), else NULL
	const char *Unknown_565C00();						// 0x565C00: the +0x188 name for +0x150 (0..4), else NULL
	void Unknown_565C40(int index);						// 0x565C40: talk to the +0x188 name 'index' ('To %s:')  // NAME 1 [index]: neutral
	void Unknown_565D80(const char *name);				// 0x565D80 (CLIENT_CHAT_EVENT 165: the private sender, first in the +0x188 names)  // NAME 1 [name]: cpacket.cpp's
	void Unknown_5658B0(const char *name);				// 0x5658B0  // NAME 1 [name]: neutral
	void Unknown_565F30(int arg);
	void Unknown_565F80();								// 0x565F80 (every frame): the window +0x108 asks for opened at +0x104
	void Unknown_566AF0();								// 0x566AF0 (every frame): the window +0xFC asks for opened at +0xF8						// 0x565F30 (0x7436A0: Esc, 2)  // NAME 1 [arg]: what 0x7436A0 passes
	void Unknown_567760();								// 0x567760: the reply list's pick
	void Unknown_567930(int channel);					// 0x567930: the default channel, saved in 'Chat_Default_Channel'  // NAME 1 [channel]: neutral
	bool Unknown_567540(int arg);						// 0x567540: the reply list over the +0x188 names ("Unable to reply")  // NAME 1 [arg]: neutral
	bool Unknown_567970();								// 0x567970: +0x1E8 is active
	void Unknown_5679A0(int event, const std::string &channel, const std::string &name, bool flag);	// 0x5679A0 (CLIENT_CHAT_EVENT 165, events 7, 8, 9, 23, 24)  // NAME 1 [event, channel, name, flag]: cpacket.cpp's
	void Unknown_567DF0(int code);						// 0x567DF0 (CLIENT_CHAT_ERROR 166 on entering a channel: the error code, or 2)  // NAME 1 [code]: cpacket.cpp's
	void Unknown_5680B0(const std::string &name);		// 0x5680B0: +0x214 set, +0x224 cleared, combo 0 rebuilt  // NAME 1 [name]: neutral
	std::string Unknown_5682C0();						// 0x5682C0: +0x214
	std::string Unknown_568440();						// 0x568440: +0x204
	void Unknown_5685C0();								// 0x5685C0: the +0xF8 window released
	void Unknown_568690();								// 0x568690: the +0x104 window released
	int Unknown_5688E0(void **list);				// 0x5688E0: the count of a NULL-terminated list  // NAME 1 [list]: neutral
	void Unknown_568910();								// 0x568910: the combo released
	void Unknown_568950();								// 0x568950: the five combos rebuilt, the combo released
	void Unknown_5689D0(int index);						// 0x5689D0  // NAME 1 [index]: neutral
	void Unknown_5698F0(int id);						// 0x5698F0 (FAIL_IF '(index < 0) || (index >= NUM_COMBO)' l.1980)  // NAME 1 [id]: neutral
	void Unknown_569A00();								// 0x569A00 (slot +0x68's key 15: the options combo)
	void Unknown_56B700();								// 0x56B700: the chat requests sent ('RestrictedStatus'), once
	void Unknown_56BA40(bool on);						// 0x56BA40 (SClient's Do_Frame, when the lag changes)  // NAME 1 [on]: what Do_Frame passes
	bool Unknown_56BA80();								// 0x56BA80
	void Unknown_56BAA0(bool show, const char *text);	// 0x56BAA0 (FAIL_IF '!font' l.2297; 0x74E710: 1 and the message)  // NAME 1 [show, text]: what 0x74E710 passes
	void Unknown_56BEF0();								// 0x56BEF0 (FAIL_IF '!sclient' l.2326): talk to the main view's target
	void Unknown_56C0C0();								// 0x56C0C0 (G_ASSERT 'false' l.2377: 'DefaultChatWindow')
	void Unknown_56C180(int window);					// 0x56C180  // NAME 1 [window]: neutral
	void Unknown_56C6A0();								// 0x56C6A0: the chat window's handler removed, the window released
	void Unknown_56C7E0();								// 0x56C7E0 (request 12: the chat font settings)
	void Unknown_56C730(const UIChatIntEventClass &event, UIChatIntClass *chat_int);	// 0x56C730: the chat window's event handler (FAIL_IF 'chat_int != ChatInt.Peek()' l.2441)  // NAME 3 [chat_int]: the FAIL_IF's; NAME 1 [event]: neutral
	void Unknown_56C7B0(const UIChatF8EventClass &event, InterfaceClass *sender);	// 0x56C7B0: the +0xF8 window's event handler (the chat window's 0x571430)  // NAME 1 [event, sender]: neutral

	int UnknownA0;										// +0xA0
	GadgetClass *UnknownA4[7];							// +0xA4 (the constructor clears all seven)
	RefCountPtr<UIChatComboClass> Combo[NUM_COMBO];				// +0xC0 (NUM_COMBO, the assert's)  // NAME 2: the assert's NUM_COMBO
	RefCountPtr<UIChatComboClass> UnknownD4;				// +0xD4: the combo shown (0x5698F0)
	void *UnknownD8;									// +0xD8 (the +0xF8 window's parts: 0x5685C0 clears +0xD8..+0xEC)
	void *UnknownDC;									// +0xDC
	void *UnknownE0;									// +0xE0
	void *UnknownE4;									// +0xE4
	void *UnknownE8;									// +0xE8
	void *UnknownEC;									// +0xEC
	InterfaceClass *UnknownF0;							// +0xF0
	bool UnknownF4;										// +0xF4
	RefCountPtr<UIChatPanelWindowClass> UnknownF8;		// +0xF8
	int UnknownFC;										// +0xFC
	int Unknown100;										// +0x100
	RefCountPtr<UIChatPanelWindowClass> Unknown104;		// +0x104 (0x565F80: the window requested through +0x108)
	int Unknown108;										// +0x108
	bool Unknown10C;									// +0x10C
	bool Unknown10D;									// +0x10D (an event handler registered on +0xF8)
	AvatarPipClass *Unknown110;							// +0x110
	RenderObjClass *Unknown114;							// +0x114 (slot +0x184)
	unsigned Unknown118;								// +0x118 (a game id)
	int Unknown11C;										// +0x11C
	bool Unknown120;									// +0x120 (constructor: true)
	std::string Unknown124;								// +0x124
	int Unknown134;										// +0x134 (the chat window's kind, 'DefaultChatWindow')
	RefCountPtr<UIChatIntClass> ChatInt;				// +0x138  // NAME 3: the assert's 'ChatInt.Peek()'
	int Unknown13C;										// +0x13C
	int Unknown140;										// +0x140
	int Unknown144;										// +0x144 (the default channel)
	int Unknown148;										// +0x148
	int Unknown14C;										// +0x14C (constructor: 6)
	int Unknown150;										// +0x150
	bool Unknown154;									// +0x154
	std::string Unknown158[3];							// +0x158
	std::string Unknown188[5];							// +0x188
	std::string Unknown1D8;								// +0x1D8
	InterfaceClass *Unknown1E8;							// +0x1E8
	int Unknown1EC;										// +0x1EC
	bool Unknown1F0;									// +0x1F0
	std::string Unknown1F4;								// +0x1F4
	std::string Unknown204;								// +0x204
	std::string Unknown214;								// +0x214
	bool Unknown224;									// +0x224
	bool Unknown225;									// +0x225
	bool Unknown226;									// +0x226
	bool Unknown227;									// +0x227
	std::vector<std::string> Unknown228;				// +0x228 (0x565120: the channels offered, +0x214 last)
	std::string Unknown238;								// +0x238
	std::string Unknown248;								// +0x248
	RenderObjClass *Unknown258;							// +0x258 (slot +0x184)
	bool Unknown25C;									// +0x25C (the lag flag: 0x56BA40 / 0x56BA80)
	float Unknown260;									// +0x260 (the lag light's blink time, 0x5630B0)
	bool MessageButtonShowing;							// +0x264 (sclient.cpp's getter)  // NAME 2: its getter's
	bool Unknown265;									// +0x265
	bool Unknown266;									// +0x266
	bool Unknown267;									// +0x267
};

#endif
