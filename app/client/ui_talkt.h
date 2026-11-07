// Earth & Beyond client (reconstruction): app\client\ui_talkt.cpp's talk tree (0x180 bytes; table 0xAF8B4C / 0xAF8B40 /
// 0xAF8B34; client 0x64B6E0-0x64EF40). sclient.h includes it after the class's base. SClient's +0x1298 holds it or
// ui_talkt_space.cpp's (derived) +0x129C; the starbase view's +0x190 is one (0x69F8F0 hands &+0x190 straight to 0x74C9D0).
// Base: 0x64B6E0 calls 0x5BB320, and the slots +0x88 / +0x8C of its table are Unknown5BB320Class's 0x543750 / 0x543770.
#ifndef UI_TALKT_H
#define UI_TALKT_H

class TalkTreePacketClass;
struct TalkTreeResponseStruct;
class ButtonGadgetClass;
class FontSys;
class FontSplitStringClass;
class AudibleSoundClass;
class UITradeClass;
class DynamicWindowClass;
class UITalkTreeClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_talkt.cpp
{
public:
	UITalkTreeClass(SClient *client, int mode);	// 0x64B6E0 (the starbase view's 0x69F8F0 passes its client and 1; ui_talkt_space.cpp's 0x64F310 its client and 0)  // NAME 1 [client]: neutral; NAME 2 [mode]: indexes the meshes 'TLI00A' / 'TLI02A'
	~UITalkTreeClass();							// 0x64B940 (table 0xAF8B4C; slot 0x64B910)
	virtual void Unknown_54();					// +0x54 0x64BCA0
	virtual void Unknown_58();					// +0x58 0x64C800
	virtual void Unknown_5C();					// +0x5C 0x64C8D0
	virtual void Unknown_60();					// +0x60 0x64D770
	virtual void Unknown_64();					// +0x64 0x64DA40
	virtual bool Unknown_68(int event_id);		// +0x68 0x64D400  // NAME 3 [event_id]: the FAIL_IFs'
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x64CC40
	virtual void Unknown_90(TalkTreePacketClass *packet);	// +0x90 0x64CC90 (TALK_TREE 84)  // NAME 3 [packet]: the FAIL_IF's '!packet'

	const char *Unknown_64CF90();				// 0x64CF90: NPCText, NULL when empty (the starbase view's 0x69C8F0)
	const char *Unknown_64CFC0();				// 0x64CFC0: PlayerText, NULL when empty
	void Unknown_64CFF0();						// 0x64CFF0: NPCText emptied
	bool Unknown_64D3E0();						// 0x64D3E0: Quit_Talk_Tree(+0x158)
	bool Unknown_64EEA0();						// 0x64EEA0: the trade window is the active one
	void Unknown_64EED0();						// 0x64EED0 (0x742530)
	void Unknown_64D230(int action);			// 0x64D230 (TALK_TREE_ACTION 86)  // NAME 3 [action]: the FAIL_IF's
	void Unknown_64C7B0(const std::string &name);	// 0x64C7B0 (FAIL_IF '!NPCName'; ui_talkt_space.cpp's 0x64F630 passes the avatar's full name)  // NAME 1 [name]: what 0x64F630 passes
	void Unknown_64D360(char *text);			// 0x64D360: a line's text up to its '/' markers  // NAME 1 [text]: what it edits
	void Unknown_64D100();						// 0x64D100: PlayerText, the answers and the full answer's window cleared
	void Unknown_64DB20(bool restart);			// 0x64DB20: the next page of NPC lines  // NAME 1 [restart]: what it does
	bool Unknown_64DD50();						// 0x64DD50: fills NPCLines, true when a line was shown
	bool Unknown_64DF80(bool clear);			// 0x64DF80: fills the answers, true when there are any  // NAME 1 [clear]: what it does
	void Unknown_64E370(const RefCountPtr<InterfaceClass> &ui, bool quit);	// 0x64E370 (G_ASSERT '!ActiveUI')  // NAME 1 [ui, quit]: what +0x160, +0x164 get
	void Unknown_64E430(bool quit);				// 0x64E430: the trade window  // NAME 1 [quit]: what +0x164 gets
	void Unknown_64E640();						// 0x64E640: the speech stopped, SpeechFile emptied
	void Unknown_64E7A0();						// 0x64E7A0: the full answer's window follows the mouse-over answer
	void Unknown_64EDC0(bool more_text);		// 0x64EDC0: 'More' or 'Done' on BTTN_MORE  // NAME 1 [more_text]: what it does

	int Mode;									// +0xAC  // NAME 1: neutral (D10): indexes the mesh and style suffixes
	std::vector<TalkTreeResponseStruct> Responses;	// +0xB0  // NAME 3: the FAIL_IF's 'Responses.size()'
	ButtonGadgetClass *Buttons[10];				// +0xC0 (by gadget id: 1-5 the answers, BTTN_MORE 7)  // NAME 3: the FAIL_IF's 'Buttons[BTTN_MORE]'
	char *FontFile;								// +0xE8 ('fa_%s.ini')  // NAME 2: what it holds
	int NPCLineWidth;							// +0xEC  // NAME 2: what it holds
	int PlayerLineWidth;						// +0xF0  // NAME 2: what it holds
	int NextLine;								// +0xF4  // NAME 2: what it holds
	RefCountPtr<FontSys> NPCLines[6];			// +0xF8  // NAME 3: the G_ASSERT's 'NPCLines[lp]'
	RefCountPtr<FontSys> NPCName;				// +0x110  // NAME 3: the FAIL_IF's '!NPCName'
	RefCountPtr<FontSys> Reply;					// +0x114 ('Reply_%s')  // NAME 2: the style's name
	FontSplitStringClass *NPCSplit;				// +0x118  // NAME 2: what it holds
	std::string SpeechFile;						// +0x11C (TALK_TREE's NPCSoundFileName)  // NAME 2: what it holds
	RefCountPtr<AudibleSoundClass> NPCSpeechHandle;	// +0x12C  // NAME 3: the FAIL_IF's 'NPCSpeechHandle'
	std::string NPCText;						// +0x130  // NAME 2: what it holds
	std::string PlayerText;						// +0x140  // NAME 2: what it holds
	bool Unknown150;							// +0x150: Unknown154 to send (SELECT_TALK_TREE 85)
	int Unknown154;								// +0x154: the Selection to send
	int Unknown158;								// +0x158: Quit_Talk_Tree when below 0
	RefCountPtr<UITradeClass> TradeUI;			// +0x15C  // NAME 2: what it holds (0x650560)
	RefCountPtr<InterfaceClass> ActiveUI;		// +0x160  // NAME 3: the asserts' '!ActiveUI'
	bool Unknown164;							// +0x164
	RefCountPtr<DynamicWindowClass> Unknown168;	// +0x168 ('TALK_TREE_WINDOW')
	bool Unknown16C[5];							// +0x16C: the answer was cut, by answer
	int Unknown174;								// +0x174: the answer shown in +0x168, -1 for none
	int Unknown178;								// +0x178
	int Unknown17C;								// +0x17C
};

#endif
