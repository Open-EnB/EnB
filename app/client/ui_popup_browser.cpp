// Earth & Beyond client (reconstruction): app\client\ui_popup_browser.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_popup_browser.cpp', 0x5DAE90-0x5DB670 by its assert and the table 0xAF59F8).
#include "ui_chat_panel.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "tasks\tsk_game.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "..\shared\g_assert.h"
#include <string>

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// The settings store's browser calls, as ui_registration.cpp's
class PopupBrowserSettingsClass { public: void Unknown_7CB240(int a); };	// NAME 1 [PopupBrowserSettingsClass, a]: as ui_registration.cpp's

// The pop-up browser panel ('fc_jweb01.ini', 'JWEB01'; table 0xAF59F8), declared as far as its rebuilt members use it
class UIPopupBrowserClass : public ChatPanelUIBase	// NAME 3: class-from-file (D24) for ui_popup_browser.cpp
{
public:
	UIPopupBrowserClass(SClient *client, int browser_id, const char *title, const char *url = NULL);	// 0x5DAE90  // NAME 1 [client, browser_id, title, url]: neutral
	virtual ~UIPopupBrowserClass();						// 0x5DB170 (deleting 0x5DB140; adjustors 0x5DB650 / 0x5DB660)

	virtual void Unknown_54();							// +0x54 0x5DB2D0
	virtual void Unknown_58();							// +0x58 0x5DB4A0 (activated: the base's, the software mouse hidden)
	virtual void Unknown_5C();							// +0x5C 0x5DB4E0 (deactivated: the browser closed, the mouse shown)
	virtual void Unknown_60();							// +0x60 0x5DB580 (every frame: the browser opened on the count's last frame)
	virtual bool Unknown_68(int id);					// +0x68 0x5DB530 (a button: 1 closes the panel)  // NAME 1 [id]: neutral

	int UnknownA0;										// +0xA0
	int UnknownA4;										// +0xA4 (frames until the browser opens)
	std::string Url;									// +0xA8 (empty: none)  // NAME 1: neutral
	std::string Title;									// +0xB8  // NAME 1: neutral
	int UnknownC8;										// +0xC8 (the browser's id at the settings store)
};

// NOT EXACT: inline-budget (0.52, 160 instructions vs 189: the client calls Url's _Tidy(false) out of line; ours expands it)
// @0x5DAE90
UIPopupBrowserClass::UIPopupBrowserClass(SClient *client, int browser_id, const char *title, const char *url) :
	ChatPanelUIBase(client)
{
	Url.size(); Url.size();		// dead reads: spend the inline budget toward the client's out-of-line _Tidy
	((int *)Unknown70)[1] = 1;
	UnknownA0 = 0;
	if (url) {
		Url.assign(url);
	} else {
		Url.assign("");
	}
	if (title) {
		Title.assign(title);
	} else {
		Title = "";
	}
	UnknownC8 = browser_id;
}

// @0x5DB170
UIPopupBrowserClass::~UIPopupBrowserClass()
{
	if (Active) {
		((PopupBrowserSettingsClass *)Unknown_7CA2D0())->Unknown_7CB240(UnknownC8);
		InterfaceClass::Unknown_5C();
		((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_515400();
	}
}

// NOT EXACT: stub
// @0x5DB2D0
void UIPopupBrowserClass::Unknown_54()
{
}

// @0x5DB4A0
void UIPopupBrowserClass::Unknown_58()
{
	if (!Active) {
		ChatPanelUIBase::Unknown_58();
		((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_5153B0();
	}
}

// @0x5DB4E0
void UIPopupBrowserClass::Unknown_5C()
{
	if (Active) {
		((PopupBrowserSettingsClass *)Unknown_7CA2D0())->Unknown_7CB240(UnknownC8);
		InterfaceClass::Unknown_5C();
		((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_515400();
	}
}

// @0x5DB530
bool UIPopupBrowserClass::Unknown_68(int id)
{
	((LineDisplayClientClass *)Client)->As_Space_Client();
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (id) {
		case 1:
			Unknown_5C();
			Unknown98 = 1;
			return(true);
	}
	return(false);
}

// @0x5DB580
void UIPopupBrowserClass::Unknown_60()
{
	if (UnknownA4 && --UnknownA4 == 0) {
		RenderObjClass *browser = Unknown84->Get_Sub_Object_By_Name("BROWSER");
		if (Url.length()) {
			Unknown_7CA2D0()->Unknown_7CA5C0(UnknownC8, browser, Url.c_str());
		} else {
			Unknown_7CA2D0()->Unknown_7CA5C0(UnknownC8, browser, NULL);
		}
		browser->Release_Ref();
	}
}

/*
**	Reconstruction notes (ui_popup_browser.cpp)
**
**	* 0x5DB4A0-0x5DB5F6 exact (TG3); the class is declared only as far as they use it, in this file
**	  (interim placement, D137); its members +0xA0-+0xC8 from these four.
**	* 0x5DB530 is a switch on the id (the client's `dec eax; je`) and returns the 1 it stores; its
**	  first statement is a call to As_Space_Client whose result the client drops (as written).
**	* 0x5DB580 calls the settings store's 0x7CA5C0 from both arms of an if: the client pushes the
**	  arguments twice and shares the call (a ?: argument makes one push).
**	* The table 0xAF59F8 and the other functions: not rebuilt.
*/
