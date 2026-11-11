// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_faction_panel.h (name assumed from ui_faction_panel.cpp,
**	whose __FILE__ is 0xB7F1F8; the client holds no string of this header)
**
**	The faction tab's description panel: a 'TSI30B' mesh over the strings of 'fa_tsi30a.ini', an exit button
**	and the faction's name and description (WRAP_NAME_TSI30B / WRAP_DESC_TSI30B). 0xC8 bytes, tables 0xAF1988 /
**	0xAF197C / 0xAF1970 (the mesh interface base's slots +0x58 / +0x5C / +0x60 / +0x7C kept); ui_faction.cpp's
**	0x58D400 makes it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_FACTION_PANEL_H  // NAME 3 [ui_faction_panel.h]: header name assumed from the client's ui_faction_panel.cpp
#define UI_FACTION_PANEL_H

#include "sclient.h"
#include <string>

class ButtonGadgetClass;	// unknown_675120.h's (+0xC0)
class ManagedFontClass;		// fontmanager.h's (+0xC4)

class UIFactionPanelClass : public Unknown5BB320Class	// NAME 2 [UIFactionPanelClass]: class from file (D24), ui_faction_panel.cpp
{
public:
	UIFactionPanelClass(SClient *client);				// 0x58EB50 (0xC8 bytes)  // NAME 1 [client]: as UIBioClass's
	virtual ~UIFactionPanelClass();						// 0x58ECB0 (deleting 0x58EC80, its thunk 0x58F080)
	virtual void Unknown_54();							// +0x54 0x58EDC0 (the exit button and the back gadget; G_ASSERT 'WrapGroup')
	virtual bool Unknown_68(int key);					// +0x68 0x58EF80 (key 1: a sound and Unknown_5C)  // NAME 1 [key]: InterfaceClass's

	void Set_Faction(const std::string &faction);		// 0x58EFC0 (the faction's name and description, then Unknown_58)  // NAME 1 [Set_Faction, faction]: what it does

	std::string UnknownAC;								// +0xAC (only made and freed here)  // NAME 1: neutral (D10)
	int UnknownBC;										// +0xBC (0)  // NAME 1: neutral (D10)
	ButtonGadgetClass *ExitButton;						// +0xC0 ('EXIT01', gadget 1)  // NAME 1 [ExitButton]: its model 'EXIT01'
	ManagedFontClass *WrapGroup;						// +0xC4 (G_ASSERT 'WrapGroup')
};

#endif
