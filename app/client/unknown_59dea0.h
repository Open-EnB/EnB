// Earth & Beyond client (reconstruction): app\client\unknown_59dea0.h, the founders window (its model 'founders00a',
// 'fa_founders00a.ini'). No string names the file: the name is an interim one (D137). Notes in unknown_59dea0.cpp.
#ifndef UNKNOWN_59DEA0_H  // NAME 1 [unknown_59dea0.h]: neutral (D137)
#define UNKNOWN_59DEA0_H

#include "ui_chat_panel_base.h"

class GadgetClass;

// The founders window (0xA8 bytes, tables 0xAF2214 / 0xAF2208 / 0xAF21FC): 'yes' sends '/founder enable', 'no' closes it
class Unknown59DEA0Class : public ChatPanelUIBase	// NAME 1: neutral (D10)
{
public:
	Unknown59DEA0Class(SpaceClientClass *client);				// 0x59DEA0 (deleting 0x59DEF0)  // NAME 1 [client]: the base's
	virtual ~Unknown59DEA0Class();						// 0x59DF20
	virtual void Unknown_54();							// +0x54 0x59DFA0 ('founders00a': the two buttons)
	virtual bool Unknown_68(int key);					// +0x68 0x59E110 (1 'yes': '/founder enable'; 2 'no')  // NAME 1 [key]: InterfaceClass's

	GadgetClass *Buttons[2];							// +0xA0 'yes', 'no'  // NAME 2: what they are
};

#endif // UNKNOWN_59DEA0_H
