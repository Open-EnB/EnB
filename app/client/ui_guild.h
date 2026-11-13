// Earth & Beyond client (reconstruction): app\client\ui_guild.h, the guild window (ui_guild.cpp's; both names
// provisional, NAME 1). Moved out of ui_guild.cpp (CP2, 2026-11-10) for its callers: the chat panel's +0xE0
// (cpacket.cpp's CLIENT_CHAT_EVENT 165, sclient.cpp's 0x750210 callers call 0x81BF40).
// PLACEMENT: provisional (D137): the header beside ui_guild.cpp
#ifndef UI_GUILD_H  // NAME 1 [ui_guild.h]: provisional header name for ui_guild.cpp
#define UI_GUILD_H

#include <string>
#include <vector>
#include <list>
#include "ui_chat_panel_base.h"	// ChatPanelUIBase (not ui_chat_panel.h: sclient.cpp declares its own view of the chat panel)
#include "..\packets\pkguildranknamesrequestclient.h"	// GuildRankNameStruct (the window's +0x180)

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's
class UIGuildMemberClass;	// ui_guild.cpp's: one member of the list
class UIGuildListClass;		// ui_guild_list.h's: the guild list window (+0xBC)
class UIPTradeCreditsClass;	// the text entry (ui_ptrade.cpp's credits entry)

// The guild window (0x190 bytes; constructor 0x819420 over ui_chat_panel_base.cpp's 0x56D280, destructor 0x8196D0,
// tables 0xB081A4 / 0xB08198 / 0xB0818C), ui_friends.cpp's twin; declared as far as the rebuilt functions use it
class UIGuildClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltip section 'UI_GUILD'
{
public:
	UIGuildClass(SClient *client);						// 0x819420  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIGuildClass();							// 0x8196D0 (deleting 0x8196A0)
	virtual void Unknown_54();							// +0x54 0x8199F0 ('tsi35a', 'fa_tsi35a.ini': the buttons, the rows, the list window and the two entries, the slider)
	virtual void Unknown_58();							// +0x58 0x819FB0 (activated: the lists emptied; the rank names and the list requested, or 'Create a new Guild')
	virtual void Unknown_5C();							// +0x5C 0x81A200 (deactivated: the buttons off, the lists emptied)
	virtual void Unknown_60();							// +0x60 0x81A330 (every frame: the slider, the rows, the entries; closed when out of a guild)
	virtual bool Unknown_68(int key);					// +0x68 0x81A420 (the buttons 1-13, the rows 15-21)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x81AA50 (the three entries moved too)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_81AAD0(int count);						// 0x81AAD0 (the slider over count rows, 7 shown)  // NAME 1 [count]: neutral
	void Unknown_81AB50();								// 0x81AB50 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_81ABA0();								// 0x81ABA0 (the rows from Top)
	void Unknown_81AC90();								// 0x81AC90 (the members deleted, the vector emptied)
	void Unknown_81AE10();								// 0x81AE10 (the list's members deleted, the list emptied)
	void Unknown_81AEB0();								// 0x81AEB0 (the vector rebuilt from the list, '(offline)' last, by name; the selection kept by name)
	void Unknown_81B350();								// 0x81B350 (the rows under the pointer or selected lit)
	void Unknown_81B3F0(int index);						// 0x81B3F0 (a row clicked: its name copied, the buttons updated)  // NAME 1 [index]: neutral
	void Unknown_81B610(int key);						// 0x81B610 (the selected member's /gc remove, promote, demote, contributions)  // NAME 1 [key]: slot +0x68's
	void Unknown_81BA60(bool on);						// 0x81BA60 (the buttons' slot +0x40 and tooltips by the rank's rights)  // NAME 1 [on]: neutral
	void Unknown_81BF40(); void Unknown_81C380(std::vector<std::string> lines); void Unknown_81C630(std::vector<GuildRankNameStruct> &ranks);	// 0x81BF40 ('/gc list all'); 0x81C380 (sclient.cpp 0x74DFF0, message type 3: the member lines; not rebuilt); 0x81C630 (sclient.cpp 0x751850: Unknown180 = ranks, then 0x81BA60(true); not rebuilt)  // NAME 1 [lines, ranks]: as 0x81C060's; what it copies
	int Unknown_81C930(std::string rank);				// 0x81C930 (a rank name's number: the rank names' vector)  // NAME 1 [rank]: neutral
	void Unknown_81BFD0();								// 0x81BFD0 (the rank names requested)
	void Unknown_81C060(std::vector<std::string> lines);	// 0x81C060 (sclient.cpp 0x74DFF0: lines added to the list window while it is active)  // NAME 1 [lines]: neutral
	void Unknown_81C1F0(std::vector<std::string> lines);	// 0x81C1F0 (the list window activated, its lines replaced)  // NAME 1 [lines]: neutral
	void Unknown_81CB90();								// 0x81CB90 (slot +0x60: an entry answered, its /gc recruit, tag, message, stats or create sent)
	void Unknown_81D130();								// 0x81D130 (no caller: every member's '/gc contributions')
	void Unknown_81C5E0(int rights, int unknown1, int unknown2, int unknown3);	// 0x81C5E0 (sclient.cpp: the rank's rights and limits)  // NAME 1 [rights, unknown1-3]: neutral

	GadgetSlider *Slider;								// +0xA0  // NAME 1: as ui_friends.h's
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: as ui_friends.h's
	int Selection;										// +0xA8 (-1 from the constructor)  // NAME 1: as ui_friends.h's
	std::string Unknown0AC;								// +0xAC (the selected member's name)
	RefCountPtr<UIGuildListClass> Unknown0BC;			// +0xBC the guild list window
	RefCountPtr<UIPTradeCreditsClass> Unknown0C0;		// +0xC0 the long text entry (ui_ptrade.cpp's credits entry: 2, 0xFE: the message, the guild name)
	RefCountPtr<UIPTradeCreditsClass> Unknown0C4;		// +0xC4 the short one (1, 0x14: a member's name, the tag)
	int Unknown0C8;										// +0xC8 the key the entries answer (2, 7, 8, 13, 24)
	std::vector<UIGuildMemberClass *> Unknown0CC;		// +0xCC the members, sorted
	std::list<UIGuildMemberClass *> Unknown0DC;			// +0xDC the members as received
	GadgetClass *Unknown0E8[13];						// +0xE8 the buttons
	GadgetClass *Unknown11C[7];							// +0x11C the rows
	RefCountPtr<FontSys> Unknown138[7];					// +0x138 the rows' names ('name%02d_TSI35a')
	RefCountPtr<FontSys> Unknown154[7];					// +0x154 the rows' locations ('LOCATION%02d_TSI35a')
	unsigned int Unknown170;							// +0x170 the rank's rights (bits 0-11: the buttons)
	int Unknown174;										// +0x174 (-1 from the constructor)
	int Unknown178;										// +0x178 (-1 from the constructor)
	int Unknown17C;										// +0x17C (-1 from the constructor)
	std::vector<GuildRankNameStruct> Unknown180;		// +0x180 the rank names
};

#endif // UI_GUILD_H
