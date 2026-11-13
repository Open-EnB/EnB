// Earth & Beyond client (reconstruction): app\client\unknown_56e170.h, the chat panel's reply list (the symbol chat
// menu of 'fc_symbolchat.ini', a row a name). The file name is provisional (NAME 1): no __FILE__, assert or string
// names the object 0x56E170-0x56F40F. Laid out from the constructor 0x56E2A0 and the destructor 0x56E5A0.
#ifndef UNKNOWN_56E170_H  // NAME 1 [unknown_56e170.h]: provisional header name (the object's first address)
#define UNKNOWN_56E170_H

#include <list>
#include <string>
#include <vector>
#include "intrface.h"
#include "..\..\wwlib\ref_ptr.h"

class SClient;				// sclient.h's
class DynamicWindowClass;	// dynamic_window_class.h's
class Unknown51C800Class;	// unknown_51c800.h's
class GadgetClass;			// gadget.h's

// A row of the list (0x0C bytes: 0x56E7F0 news it)
struct UIChatReplyListOptionStruct	// NAME 1: neutral (D10)
{
	UIChatReplyListOptionStruct();						// 0x56E170 (uncalled out of line: 0x56E7F0 inlines it)
	~UIChatReplyListOptionStruct();						// 0x56E190
	RefCountPtr<DynamicWindowClass> Window;				// +0x00 (SYMBOL_CHAT_OPTION)
	RefCountPtr<Unknown51C800Class> Text;				// +0x04 (the name)
	GadgetClass *Gadget;								// +0x08 (a DynamicWindowGadgetClass over Window; its id the row, from 1)
};

// The reply list at the chat panel's +0x1E8 (0xA4 bytes, the panel's 0x5629E0 makes it; tables 0xAF024C / 0xAF0240 /
// 0xAF0234): SYMBOL_CHAT_WINDOW with one SYMBOL_CHAT_SECTION, a SYMBOL_CHAT_OPTION row for each name handed in; the
// same shape as ui_starbase.cpp's symbol chat menu (UIStarbaseUnknown1F8Class)
class UIChatReplyListClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	UIChatReplyListClass(SClient *client);				// 0x56E2A0  // NAME 1 [client]: what 0x5629E0 passes
	virtual ~UIChatReplyListClass();					// +0x00 0x56E5A0 (deleting 0x56E570)
	virtual void Unknown_58();							// +0x58 0x56ED10 (opened at the pointer, 'fontclick.wav')
	virtual void Unknown_5C();							// +0x5C 0x56EE40
	virtual bool Unknown_68(int key);					// +0x68 0x56EE70 (a row chosen: kept in +0x84)  // NAME 1 [key]: InterfaceClass's
	void Unknown_56E7F0(std::list<std::string> &names, int arg);	// 0x56E7F0 (a row for each name)  // NAME 1 [names, arg]: what 0x56EED0 passes
	void Unknown_56EC50();								// 0x56EC50 (the rows removed; uncalled out of line)
	void Unknown_56EEB0();								// 0x56EEB0 (empty; no caller)
	void Unknown_56EED0(std::list<std::string> &names, int arg);	// 0x56EED0 (the rows rebuilt unless open; the panel's 0x567540)  // NAME 1 [names, arg]: what 0x567540 passes
	int Unknown_56EFB0();								// 0x56EFB0 (the row chosen (1-based), cleared; 0 for none; the panel's 0x567760)
	int Unknown84;										// +0x84 (the row chosen: 0x56EE70)
	RefCountPtr<DynamicWindowClass> Window;				// +0x88 (SYMBOL_CHAT_WINDOW)
	RefCountPtr<DynamicWindowClass> Section;			// +0x8C (SYMBOL_CHAT_SECTION)
	GadgetClass *Gadget;								// +0x90 (a DynamicWindowGadgetClass over Window)
	std::vector<UIChatReplyListOptionStruct *> Options;	// +0x94
};

#endif
