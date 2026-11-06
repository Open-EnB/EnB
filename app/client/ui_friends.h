// Earth & Beyond client (reconstruction): app\client\ui_friends.h, the friends window (the client's ui_friends.cpp
// per its asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x58F210 and the
// destructor 0x58F450 (FR1, 2026-11-05); declared as far as the rebuilt functions use it.
#ifndef UI_FRIENDS_H  // NAME 1 [ui_friends.h]: provisional header name for ui_friends.cpp
#define UI_FRIENDS_H

#include <string>
#include <vector>
#include <list>
#include "ui_chat_panel.h"
#include "ui_group.h"		// UISymbolChatClass (the window's +0xB0)

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's

// One player of the list (0x20 bytes; the window's list +0xC4 and PlayersInfo hold them)
class UIFriendsPlayerInfoClass	// NAME 2: what PlayersInfo holds (the asserts' 'PlayersInfo', '!friend_info')
{
public:
	UIFriendsPlayerInfoClass(std::string name, std::string location);	// 0x58F0F0 (0x5908B0)  // NAME 1 [name, location]: the members they set
	~UIFriendsPlayerInfoClass() {}						// 0x590780 (inline, called out of line from the deleting destructor 0x590C40)

	std::string Name;									// +0x00 (the rows' 'name%02d_TSI28A' styles)  // NAME 2: its style's
	std::string Location;								// +0x10 ('offline' when not on; 'LOCATION%02d_TSI28A'; the assert's 'location_list_size')  // NAME 2: its style's
};

// ui_friends.cpp's friends window (0x14C bytes; constructor 0x58F210 over ui_chat_panel_base.cpp's 0x56D280,
// destructor 0x58F450, table 0xAF1AD4)
class UIFriendsClass : public UIChatPanelBaseClass	// NAME 3: class-from-file (D24) for app\client\ui_friends.cpp
{
public:
	void Unknown_5908B0();								// 0x5908B0: PlayersInfo rebuilt from +0xC4, sorted (by name, or by location with 'offline' last)
	void Unknown_5906C0();								// 0x5906C0: PlayersInfo's players deleted, the vector emptied (0x5908B0 expands it; the destructor calls it)
	void Unknown_590810();								// 0x590810: +0xC4's players deleted, the list emptied (the destructor calls it)
	void Unknown_591030(bool on);						// 0x591030: the buttons and rows' slot +0x40; [2] / [4] off while nothing is selected  // NAME 1 [on]: neutral
	void Unknown_590550();								// 0x590550 (0x5908B0: the rows from the slider's position)
	void Unknown_590C70();								// 0x590C70 (slot +0x60, every frame: the rows under the pointer or selected lit)
	void Unknown_590DB0(int index);						// 0x590DB0: a row clicked (twice: 0x591EE0)  // NAME 1 [index]: neutral
	void Unknown_590E50();								// 0x590E50 (slot +0x60: a symbol picked is sent, request 8)
	void Unknown_591EE0();								// 0x591EE0 (FAIL_IFs 'Selection < 0 || Selection >= (int)PlayersInfo.size()', '!friend_info')

	GadgetSlider *Slider;								// +0xA0 (the destructor's 0x66E460; 0x5908B0: the list's length, 7 rows)  // NAME 1: neutral
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: neutral
	int Selection;										// +0xA8 (-1 from the constructor)  // NAME 3: the asserts'
	bool Unknown0AC;									// +0xAC (1 from the constructor: sorted by location)
	bool Unknown0AD;									// +0xAD ('RestrictedStatus')
	bool Unknown0AE;									// +0xAE
	RefCountPtr<UISymbolChatClass> SymbolChatUI;		// +0xB0 (the destructor's slot +0x5C)  // NAME 1: as ui_group.h's
	std::vector<UIFriendsPlayerInfoClass *> PlayersInfo;	// +0xB4  // NAME 3: the asserts'
	std::list<UIFriendsPlayerInfoClass *> Unknown0C4;	// +0xC4 (the destructor erases its nodes, not the players)
	GadgetClass *Unknown0D0[8];							// +0xD0 (the constructor clears them; [2] and [4] on while a row is selected)
	GadgetClass *Unknown0F0[7];							// +0xF0 the rows (slot +0x30: the pointer over one)
	int Unknown10C;										// +0x10C
	int Unknown110;										// +0x110
	RefCountPtr<FontSys> Unknown114[7];					// +0x114 the rows' names (eh vector constructor 0x553A80, destructor 0x51C630)
	RefCountPtr<FontSys> Unknown130[7];					// +0x130 the rows' locations ('offline': 0.5 alpha)
};

#endif // UI_FRIENDS_H
