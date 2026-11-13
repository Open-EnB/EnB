// Earth & Beyond client (reconstruction): app\client\ui_groupfind.h, the group finder's list of players looking for a
// group (the client's ui_groupfind.cpp per its asserts; this header's name is provisional, NAME 1). Laid out from the
// constructor 0x598DF0 and the destructor 0x599080 (O7, 2026-11-13).
#ifndef UI_GROUPFIND_H  // NAME 1 [ui_groupfind.h]: provisional header name for ui_groupfind.cpp
#define UI_GROUPFIND_H

#include <vector>
#include <list>
#include "ui_chat_panel_base.h"

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's
class FindMemberPacketClass;	// pkfindmember.h's (FIND_MEMBER 83)

// One player of the list (0x10 bytes; PlayersInfo and the packet's copy +0x148 hold them)
class UIGroupFindPlayerInfoClass	// NAME 2: what PlayersInfo holds (the assert's 'PlayersInfo')
{
public:
	UIGroupFindPlayerInfoClass() : Name(NULL), Class(NULL), Level(0), ID(0) {}
	~UIGroupFindPlayerInfoClass();						// 0x599830 (deleting 0x599DE0)

	char *Name;											// +0x00 ('Anonymous' when the object is not known; the 'name%02d_opt01' styles)  // NAME 2: its style's
	char *Class;										// +0x04 the race / profession ('Ter/Enf'; the 'rc%02d_opt01' styles)  // NAME 1: neutral
	int Level;											// +0x08 (the 'level%02d_opt01' styles)  // NAME 2: its style's
	unsigned ID;										// +0x0C the player's game id (Find_Object)  // NAME 2: what it holds
};

// One row's three texts (0x0C bytes; the eh vector constructor 0x598F20, destructor 0x598F40)
struct UIGroupFindRowStruct	// NAME 1: neutral (D10)
{
	UIGroupFindRowStruct() {}
	~UIGroupFindRowStruct() { Name = NULL; Class = NULL; Level = NULL; }

	RefCountPtr<FontSys> Name;							// +0x00 'name%02d_opt01'  // NAME 2: its style's
	RefCountPtr<FontSys> Class;							// +0x04 'rc%02d_opt01'  // NAME 1: as the player's
	RefCountPtr<FontSys> Level;							// +0x08 'level%02d_opt01'  // NAME 2: its style's
};

// ui_groupfind.cpp's window (0x154 bytes; constructor 0x598DF0 over ui_chat_panel_base.cpp's 0x56D280, destructor
// 0x599080, tables 0xAF1D80 / 0xAF1D74 / 0xAF1D68): the players looking for a group ('tsi25a'), sorted by name, by race
// and profession or by level. ui_groupopt.cpp's window holds it at +0xA8; FIND_MEMBER 83 fills it (0x59A400).
class UIGroupFindClass : public ChatPanelUIBase	// NAME 3: class-from-file (D24) for ui_groupfind.cpp (its tooltips' 'UI_GROUPFIND')
{
public:
	UIGroupFindClass(SClient *client);					// 0x598DF0 (ui_groupopt.cpp's 0x59AD10)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIGroupFindClass();						// 0x599080 (deleting 0x599050)
	virtual void Unknown_54();							// +0x54 0x599260 ('tsi25a', 'fa_tsi25a.ini': the buttons, the rows, the slider)
	virtual void Unknown_58();							// +0x58 0x599660 (activated: both lists emptied, the slider reset)
	virtual void Unknown_5C();							// +0x5C 0x599880 (deactivated: both lists emptied)
	virtual void Unknown_60();							// +0x60 0x5999F0 (every frame: the rows lit, the slider)
	virtual bool Unknown_68(int key);					// +0x68 0x599A30 (1 exit, 2-4 the sorts, 6-13 the rows)  // NAME 1 [key]: InterfaceClass's
	void Unknown_599B10(int count);						// 0x599B10 (the slider over count rows, 8 shown)  // NAME 1 [count]: neutral
	void Unknown_599B80();								// 0x599B80 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_599BC0();								// 0x599BC0 (the rows from Top)
	void Unknown_599D00();								// 0x599D00: PlayersInfo's players deleted, the vector emptied
	void Unknown_599E60();								// 0x599E60: +0x148's players deleted, the list emptied
	void Unknown_599F30(int sort);						// 0x599F30: PlayersInfo rebuilt from +0x148 without the player, sorted (0 name, 1 race / profession, 2 level)  // NAME 1 [sort]: neutral
	void Unknown_59A1D0();								// 0x59A1D0: the rows under the pointer lit
	void Unknown_59A290(int row);						// 0x59A290: a row clicked: its player targeted (FAIL_IFs 'index >= (int)PlayersInfo.size()', 'index < 0', '!targeting', '!co': index is Top + row)  // NAME 1 [row]: neutral
	int Unknown_59A3D0(int a, int b);					// 0x59A3D0: -1, 0 or 1 as a is below, equal to or above b (this unused: ret 8; the sorts expand it)  // NAME 1 [a, b]: neutral
	void Unknown_59A400(FindMemberPacketClass *packet);	// 0x59A400 (cpacket.cpp: FIND_MEMBER 83; FAIL_IFs 'race >= NUMBER_PLAYER_RACES', 'prof >= MAX_PLAYER_PROFESSIONS')  // NAME 1 [packet]: the handler's

	GadgetSlider *Slider;								// +0xA0 (the destructor's 0x66E460; 8 rows)  // NAME 1: neutral
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: neutral
	GadgetClass *Unknown0A8[4];							// +0xA8 the buttons 'exit', 'name', 'rc', 'level' (ids 1-4)
	GadgetClass *Unknown0B8[8];							// +0xB8 the rows 'name%02d' (ids 6-13; slot +0x30: the pointer over one)
	UIGroupFindRowStruct Unknown0D8[8];					// +0xD8 the rows' texts
	std::vector<UIGroupFindPlayerInfoClass *> PlayersInfo;	// +0x138  // NAME 3: the assert's
	std::list<UIGroupFindPlayerInfoClass *> Unknown148;	// +0x148 (FIND_MEMBER 83's players)
};

#endif // UI_GROUPFIND_H
