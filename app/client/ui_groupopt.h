// Earth & Beyond client (reconstruction): app\client\ui_groupopt.h, the group options window (the client's
// ui_groupopt.cpp per its asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x59AD10
// and the destructor 0x59AEE0 (O7, 2026-11-13).
#ifndef UI_GROUPOPT_H  // NAME 1 [ui_groupopt.h]: provisional header name for ui_groupopt.cpp
#define UI_GROUPOPT_H

#include "ui_chat_panel_base.h"
#include "ui_groupfind.h"

class GadgetClass;			// gadget.cpp's
class CheckBoxGadgetClass;	// unknown_7e7e00.h's check box (0x7E8030: checked or not; +0x64)

// ui_groupopt.cpp's window (0xD4 bytes; constructor 0x59AD10 over ui_chat_panel_base.cpp's 0x56D280, destructor
// 0x59AEE0, tables 0xAF1EE0 / 0xAF1ED4 / 0xAF1EC8; ui_chat_panel.cpp's 0x569A00 makes it): look for a group, the
// list of players looking (ui_groupfind.cpp's window), leave or disband, and the group's options ('tsi24a').
class UIGroupOptClass : public ChatPanelUIBase	// NAME 3: class-from-file (D24) for ui_groupopt.cpp
{
public:
	UIGroupOptClass(SClient *client);					// 0x59AD10  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIGroupOptClass();							// 0x59AEE0 (deleting 0x59AEB0)
	virtual void Unknown_54();							// +0x54 0x59AFF0 ('tsi24a', 'fa_tsi24a.ini': the buttons, the check buttons)
	virtual void Unknown_58();							// +0x58 0x59B150 (activated: the buttons and checks from GroupInfo)
	virtual void Unknown_5C();							// +0x5C 0x59B1F0 (deactivated: the list window too)
	virtual void Unknown_60();							// +0x60 0x59B220 (every frame: GroupInfo's members, the avatar, the find button's text; FAIL_IF '!sclient')
	virtual bool Unknown_68(int key);					// +0x68 0x59B380 (1 exit, 2 look for a group, 3 the list, 4 leave, 5 disband, 7-10 the options)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x59B8D0 (the list window moved too)  // NAME 1 [view, arg]: InterfaceClass's
	virtual int Unknown_90() { return((int)this); }	// +0x90 0x59AE90 (cpacket.cpp's FIND_MEMBER 83 reaches FindUI through it)
	void Unknown_59B910(bool on);						// 0x59B910: the buttons and checks' slot +0x40, then 0x59B990  // NAME 1 [on]: neutral
	void Unknown_59B990();								// 0x59B990: the buttons and checks as the player is in a group and leads it
	bool Unknown_59BBC0();								// 0x59BBC0: the player is in a group (SClient 0x74D570; FAIL_IF '!sclient')
	bool Unknown_59BC00();								// 0x59BC00: the player leads its group (SClient 0x74D690; FAIL_IF '!sclient')
	void Unknown_59BC40();								// 0x59BC40: LookingForGroup and the options from GroupInfo (FAIL_IFs '!sclient', '!sclient->AuxData')

	bool Unknown0A0;									// +0xA0 GroupInfo.LookingForGroup (key 2 toggles it)
	unsigned Unknown0A4;								// +0xA4 the avatar's game id last seen (slot +0x60)
	RefCountPtr<UIGroupFindClass> FindUI;				// +0xA8 the list of players looking for a group (0x154 bytes, 0x598DF0)  // NAME 2: what it holds
	GadgetClass *Unknown0AC[5];							// +0xAC the buttons 'exit', 'looking', 'find', 'leave', 'disband' (ids 1-5)
	CheckBoxGadgetClass *Unknown0C0[5];					// +0xC0 the check buttons 'invites_check' .. 'looting_check' (ids 7-11)
};

#endif // UI_GROUPOPT_H
