// Earth & Beyond client (reconstruction): app\client\ui_shard.h, the galaxy (shard) select window of character
// creation (the client's ui_shard.cpp per its FAIL_IF; this header's name is provisional, NAME 1). Notes at the end
// of ui_shard.cpp.
#ifndef UI_SHARD_H  // NAME 1 [ui_shard.h]: provisional header name for ui_shard.cpp
#define UI_SHARD_H

#include "intrface.h"
#include "..\common\unknown_galaxy.h"
#include <vector>

class GadgetClass;
class LayerClass;
class LoginTaskClass;
class TextGadgetClass;
class CharacterViewClass;

// The galaxy select window (ui_shard.cpp's __FILE__ 0xB89390, the FAIL_IF 'ShardSelection < 0 || ShardSelection >=
// (int)SortedGalaxies.size()'; layout fa_fe02a.ini on the model FE02A), 0xEC bytes, tables 0xAF76A0 / 0xAF7694 /
// 0xAF7688. The character view's mode 5 (LoginTaskClass step 5): after the ship, or for a character not yet
// registered; its Accept names the character and sends the creation (LoginTaskClass slot +0x48).
class Unknown615A50InterfaceClass : public InterfaceClass	// NAME 1: neutral (D10), as charview.cpp's (no RTTI: the window has no gadget callback)
{
public:
	Unknown615A50InterfaceClass(UIViewClass *view, LayerClass *layer);	// 0x615A50 (charview.cpp's 0x679160, mode 5)  // NAME 1 [view, layer]: as LoginInterface's
	virtual ~Unknown615A50InterfaceClass();			// 0x616090 (0x616060 the deleting one)
	virtual void Unknown_60();						// +0x60 0x616220 (every frame)
	virtual bool Unknown_68(int key);				// +0x68 0x6164E0 (the events)  // NAME 1 [key]: InterfaceClass's

	void Unknown_616A10();							// 0x616A10 (the frame: the narrator, the task's answer)
	void Unknown_616DA0();							// 0x616DA0 (the six rows: galaxy names and populations)
	void Unknown_617190();							// 0x617190 (the up / down arrows on or off)
	void Unknown_617270();							// 0x617270 (one row up; inlined in slot +0x68)
	void Unknown_6172A0();							// 0x6172A0 (one row down; inlined in slot +0x68)
	void Unknown_617310();							// 0x617310 (the buttons on; Accept dimmed without a name and a galaxy)
	void Unknown_617540();							// 0x617540 (the buttons off, their texts dimmed)
	void Unknown_617660(bool named);				// 0x617660 (the name button's texture: green once named)  // NAME 1 [named]: what it selects
	void Unknown_6176F0();							// 0x6176F0 (SortedGalaxies from the task's list, by +0x54)
	void Unknown_6179D0(int row);					// 0x6179D0 (a row picked; inlined in slot +0x68)  // NAME 1 [row]: neutral
	int Unknown_617A10();							// 0x617A10 (the picked galaxy's ID; the FAIL_IF)
	void Unknown_617AA0();							// 0x617AA0 (the tutorial skip toggled; inlined in slot +0x68)

	CharacterViewClass *CharacterView;				// +0x84 (the view's slot +0x28)  // NAME 1: as ui_class.h's
	LoginTaskClass *LoginTask;						// +0x88 (the view's +0x14's slot +0x08)  // NAME 1: as ui_class.h's
	int Unknown8C;									// +0x8C (the first row shown)
	int ShardSelection;								// +0x90 (-1: none)  // NAME 3: the FAIL_IF's
	int Unknown94;									// +0x94 (how it leaves: 9 next, 10 back)
	TextGadgetClass *NameGadget;					// +0x98 ('Name_fe02a')  // NAME 1: as ui_login.cpp's
	bool Unknown9C;									// +0x9C (a name is set)
	bool Unknown9D;									// +0x9D (skip the beginner tutorial: the creation's last argument)
	bool Unknown9E;									// +0x9E (tutorial.dat read)
	GadgetClass *Buttons[10];						// +0xA0 ('button_shard00' .. 'exit', ids 1-10)  // NAME 1: as ui_class.h's
	GadgetClass *Arrows[2];							// +0xC8 ('button_up', 'button_down', ids 13, 14)  // NAME 1: neutral (D10)
	bool UnknownD0;									// +0xD0
	bool UnknownD1;									// +0xD1 (the quit box is up)
	bool UnknownD2;									// +0xD2 (the narrator's first animation runs)
	bool UnknownD3;									// +0xD3 (the narrator's last animation runs)
	bool UnknownD4;									// +0xD4
	bool UnknownD5;									// +0xD5
	std::vector<LoginGalaxyClass> SortedGalaxies;	// +0xD8 (0x60 bytes each)  // NAME 3: the FAIL_IF's
	bool UnknownE8;									// +0xE8 (the creation sent: waiting for the answer)
};

#endif // UI_SHARD_H
