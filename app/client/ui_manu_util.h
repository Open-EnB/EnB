// Earth & Beyond client (reconstruction): app\client\ui_manu_util.h, the header of ui_manu_util.cpp (the client's file
// per its asserts' __FILE__ 'e:\build\beta\G\app\Client\ui_manu_util.cpp'; this header's name is provisional, NAME 1).
#ifndef UI_MANU_UTIL_H  // NAME 1 [ui_manu_util.h]: provisional header name for ui_manu_util.cpp (D137)
#define UI_MANU_UTIL_H

class ButtonGadgetClass;	// unknown_675120.h's (ui_manu.cpp makes them)
class RenderObjClass;

// One column of the manufacturing window's category icons (0x4C bytes; ui_manu.cpp keeps three at +0xC0..+0xC8): five
// buttons and five sub-objects of the window's mesh that fade in and out one after another. Its functions are
// ui_manu_util.cpp's object, 0x5B7370-0x5B7A8A (PermStringInitClass static first, the guarded <string> ids last).
class UIManuUnknownC0Class	// NAME 1: neutral (D10)
{
public:
	UIManuUnknownC0Class(int column);					// 0x5B73C0 (ui_manu.cpp's slot +0x54 passes 0..2)  // NAME 1 [column]: neutral
	~UIManuUnknownC0Class();							// 0x5B7430
	void Unknown_5B74A0();								// 0x5B74A0 (no callers: every button and sub-object shown)
	void Unknown_5B74F0(bool on);						// 0x5B74F0: hidden at once (true), else faded out  // NAME 1 [on]: neutral
	void Unknown_5B7570(bool *disabled, int primary, int category);	// 0x5B7570 (0x5B2230: the window's +0x98C and the first two picks; the icons, then faded in)  // NAME 3 [primary, category]: the asserts'; NAME 1 [disabled]: neutral
	void Unknown_5B7880();								// 0x5B7880: one step of the fade (ui_manu.cpp's slot +0x60)

	ButtonGadgetClass *Gadgets[5];						// +0x00 (slot +0x60: picked)  // NAME 2: what it holds
	RenderObjClass *Unknown14[5];						// +0x14 (released by the destructor; Set_Hidden)
	int IDs[5];											// +0x28 (-1 for none; what 0x5B2060 picks)  // NAME 2: what it holds
	int Unknown3C;										// +0x3C: fading in (1) or out (-1)
	int Unknown40;										// +0x40: the next button of the fade
	float Unknown44;									// +0x44: the seconds since the last step (a step each 0.1)
	int Unknown48;										// +0x48 (the constructor's argument: the column, 0..2)
};

#endif
