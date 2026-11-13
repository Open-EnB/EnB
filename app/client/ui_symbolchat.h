// Earth & Beyond client (reconstruction): app\client\ui_symbolchat.h, the symbol chat menu of the group window (the
// client's ui_symbolchat.cpp per its __FILE__; this header's name is provisional, NAME 1). Laid out from the
// constructor 0x64A480 and the destructor 0x64A7D0.
#ifndef UI_SYMBOLCHAT_H  // NAME 1 [ui_symbolchat.h]: provisional header name for ui_symbolchat.cpp
#define UI_SYMBOLCHAT_H

#include <vector>
#include "intrface.h"
#include "..\..\wwlib\ref_ptr.h"

class SClient;				// sclient.h's
class DynamicWindowClass;	// dynamic_window_class.h's
class Unknown51C800Class;	// unknown_51c800.h's
class GadgetClass;			// gadget.h's

// A symbol of the menu (0x14 bytes: 0x64A9B0 news it)
struct UISymbolChatOptionStruct	// NAME 1: neutral (D10)
{
	UISymbolChatOptionStruct();							// 0x64A330 (uncalled out of line: 0x64A9B0 inlines it)
	~UISymbolChatOptionStruct();						// 0x64A360
	RefCountPtr<DynamicWindowClass> Window;				// +0x00 (SYMBOL_CHAT_OPTION)
	RefCountPtr<Unknown51C800Class> Text;				// +0x04 (the SymbolString)
	GadgetClass *Gadget;								// +0x08 (a DynamicWindowGadgetClass over Window; its id the symbol, from 1)
	char *Unknown0C;									// +0x0C (the SymbolAsset, nstrdup'd)
	bool Unknown10;										// +0x10 (a CONTEXT symbol: the third section)
};

// ui_symbolchat.cpp's symbol chat menu, the group window's +0xAC (0xAC bytes, the group window's 0x594040 makes it;
// tables 0xAF8A08 / 0xAF89FC / 0xAF89F0): SYMBOL_CHAT_WINDOW with a SYMBOL_CHAT_SECTION for each of the GENERIC, TEAM
// and CONTEXT symbols of 'symbolchatdef.ini' ('fc_symbolchat.ini' its text)
class UISymbolChatClass : public InterfaceClass	// NAME 2: the FAIL_IF's '!SymbolChatUI'
{
public:
	UISymbolChatClass(SClient *sclient);				// 0x64A480 (0x594040)  // NAME 1 [sclient]: what 0x594040 passes
	virtual ~UISymbolChatClass();						// +0x00 0x64A7D0 (deleting 0x64A7A0)
	virtual void Unknown_58();							// +0x58 0x64B050 (opened at the pointer, 'fontclick.wav')
	virtual void Unknown_5C();							// +0x5C 0x64B180
	virtual void Unknown_60();							// +0x60 0x64B230 (closed by a click outside it)
	virtual bool Unknown_68(int key);					// +0x68 0x64B1C0 (a symbol chosen: kept in +0x84)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x64B200 (only onto another view)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_64A9B0();								// 0x64A9B0 (the symbols of 'symbolchatdef.ini' made)
	void Unknown_64AF80();								// 0x64AF80 (the symbols removed)
	void Unknown_64B2C0();								// 0x64B2C0 (the symbols rebuilt unless open; 0x594040, after its slot +0x54)
	int Unknown_64B2F0(char *text, bool *leader);		// 0x64B2F0: the symbol picked, as text, and whether the leader sent it (0x596DB0)  // NAME 1 [text, leader]: what 0x596DB0 hands on
	int Unknown84;										// +0x84 (the symbol chosen: 0x64B1C0)
	RefCountPtr<DynamicWindowClass> Window;				// +0x88 (SYMBOL_CHAT_WINDOW)
	RefCountPtr<DynamicWindowClass> Sections[3];		// +0x8C (SYMBOL_CHAT_SECTION: GENERIC, TEAM, CONTEXT)
	GadgetClass *Gadget;								// +0x98 (a DynamicWindowGadgetClass over Window)
	std::vector<UISymbolChatOptionStruct *> Options;	// +0x9C
};

#endif
