// Earth & Beyond client (reconstruction): app\client\ui_popup_browser.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_popup_browser.cpp', 0x5DAE90-0x5DB670 by its assert and the table 0xAF59F8).
#include "ui_popup_browser.h"
#include "ui_chat_panel.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "tasks\tsk_game.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "gadget.h"
#include "gd_uiback.h"
#include "..\shared\g_assert.h"
#include "..\shared\auth_product.h"
// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SpaceClientClass *As_Space_Client();					// +0x18 (SpaceClientClass's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};





// @0x5DAE90
PopupBrowserInterface::PopupBrowserInterface(SpaceClientClass *client, int browser_id, const char *title, const char *url) :
	ChatPanelUIBase(client)
{

	((int *)Unknown70)[1] = 1;
	memset(&ExitButton, 0, sizeof(ExitButton));
	if (url) {
		Url.assign(url, std::char_traits<char>::length(url));
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
PopupBrowserInterface::~PopupBrowserInterface()
{
	if (Active) {
		Unknown_7CA2D0()->Unknown_7CB240(UnknownC8);
		InterfaceClass::Unknown_5C();
		((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_515400();
	}
}

// The font manager (0xBE67C4), as ui_login.cpp's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: neutral (D10)

// Unknown_54's button: its model's name (0xB8463C) and its text (0xBE5D70: zero, nothing writes it)
static const char *Unknown_B8463C[1] = { "exit_button" };	// NAME 1: neutral (D10)
const char *Unknown_BE5D70[1];	// NAME 1: neutral (D10)














// @0x5DB2D0
void PopupBrowserInterface::Unknown_54()
{
	if (Unknown_98("JWEB01", "fc_jweb01.ini")) {
		ExitButton = Add_Button(1, Unknown_B8463C[0], Unknown_BE5D70[0]);
		ExitButton->Set_First_Sort_Group(true);
		RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("TITLE_JWEB01", Interface_Mesh()));	// NAME 1: neutral (D10)
		FAIL_IF(!font) return;
		font->Change_Text(Title.c_str(), 0);
		UIBackGadget *back = Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));	// NAME 1: neutral (D10)
		if (back) {
			back->Set_First_Sort_Group(true);
		}
		UnknownA4 = 5;
	}
}

// @0x5DB4A0
void PopupBrowserInterface::Unknown_58()
{
	if (!Active) {
		ChatPanelUIBase::Unknown_58();
		((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_5153B0();
	}
}

// @0x5DB4E0
void PopupBrowserInterface::Unknown_5C()
{
	if (Active) {
		Unknown_7CA2D0()->Unknown_7CB240(UnknownC8);
		InterfaceClass::Unknown_5C();
		((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_515400();
	}
}

// @0x5DB530
bool PopupBrowserInterface::Unknown_68(int id)
{
	((LineDisplayClientClass *)Client)->As_Space_Client();
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (id) {
		case 1:
			Unknown_5C();
			Closed = 1;
			return(true);
	}
	return(false);
}

// @0x5DB580
void PopupBrowserInterface::Unknown_60()
{
	if (UnknownA4 && --UnknownA4 == 0) {
		RenderObjClass *browser = InterfaceMesh->Get_Sub_Object_By_Name("BROWSER");
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
**	* 0x5DB4A0-0x5DB5F6 exact (TG3); the class is declared in ui_popup_browser.h (CP1, for the chat
**	  panel that makes it); its members +0xA0-+0xC8 from these four.
**	* 0x5DB530 is a switch on the id (the client's `dec eax; je`) and returns the 1 it stores; its
**	  first statement is a call to As_Space_Client whose result the client drops (as written).
**	* 0x5DB580 calls the settings store's 0x7CA5C0 from both arms of an if: the client pushes the
**	  arguments twice and shares the call (a ?: argument makes one push).
**	* 0x5DB2D0 (Unknown_54, CP1): its FAIL_IF '!font' on the client's line 85; 0xBE5D70, the button's text, is a
**	  zero global nothing writes (only this function reads it).
**	* 0x5DAE90 is exact: explicit URL length preserves the asymmetric string cleanup expansions;
**	  clearing ExitButton through its address recovers the retail initialization sequence.
**	  Remove the prior dead Url.size() reads; all later assertion lines are preserved.
**	* The other functions of the table 0xAF59F8: not rebuilt.
*/
