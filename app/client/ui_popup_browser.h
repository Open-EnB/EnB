// Earth & Beyond client (reconstruction): app\client\ui_popup_browser.h, the pop-up browser panel of ui_popup_browser.cpp
// (the client's __FILE__ 'e:\build\beta\G\app\Client\ui_popup_browser.cpp'); the header's name follows the file's.
#ifndef UI_POPUP_BROWSER_H
#define UI_POPUP_BROWSER_H

#include <string>
#include "ui_chat_panel_base.h"

// The pop-up browser panel ('fc_jweb01.ini', 'JWEB01'; 0xCC bytes, table 0xAF59F8): the chat panel's request 11
// ('Credits', id 5) and its slot +0x68's key 16 ('Online Manual', id 6)
class UIPopupBrowserClass : public ChatPanelUIBase	// NAME 3: class-from-file (D24) for ui_popup_browser.cpp
{
public:
	UIPopupBrowserClass(SClient *client, int browser_id, const char *title, const char *url = NULL);	// 0x5DAE90  // NAME 1 [client, browser_id, title, url]: neutral
	virtual ~UIPopupBrowserClass();						// 0x5DB170 (deleting 0x5DB140; adjustors 0x5DB650 / 0x5DB660)

	virtual void Unknown_54();							// +0x54 0x5DB2D0 ('JWEB01': the exit button, the title 'TITLE_JWEB01', the back gadget; FAIL_IF '!font')
	virtual void Unknown_58();							// +0x58 0x5DB4A0 (activated: the base's, the software mouse hidden)
	virtual void Unknown_5C();							// +0x5C 0x5DB4E0 (deactivated: the browser closed, the mouse shown)
	virtual void Unknown_60();							// +0x60 0x5DB580 (every frame: the browser opened on the count's last frame)
	virtual bool Unknown_68(int id);					// +0x68 0x5DB530 (a button: 1 closes the panel)  // NAME 1 [id]: neutral

	GadgetClass *ExitButton;							// +0xA0 'exit_button' (Unknown_54)  // NAME 2: its model's name
	int UnknownA4;										// +0xA4 (frames until the browser opens)
	std::string Url;									// +0xA8 (empty: none)  // NAME 1: neutral
	std::string Title;									// +0xB8  // NAME 1: neutral
	int UnknownC8;										// +0xC8 (the browser's id at the settings store)
};

#endif // UI_POPUP_BROWSER_H
