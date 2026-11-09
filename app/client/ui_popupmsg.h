// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_popupmsg.h (PROVISIONAL, D137: the header of ui_popupmsg.cpp)
**
**	The end screen ui_popupmsg.cpp rebuilds (0x5DB670-0x5DBEB0); charview.cpp's 0x679160 makes it for mode 0
**	(CHARACTER_POPUP_MESSAGE).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_POPUPMSG_H  // NAME 1 [ui_popupmsg.h]: provisional header name for ui_popupmsg.cpp
#define UI_POPUPMSG_H

#include "intrface.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"

class UIViewClass;
class LoginTaskClass;
class CharacterViewClass;
class RenderObjClass;
class GadgetClass;


// The end screen (0xD4 bytes, tables 0xAF5B70 / 0xAF5B64 / 0xAF5B58; charview.cpp's 0x679160 makes it for mode 0)
class Unknown5DB670InterfaceClass : public InterfaceClass	// NAME 1: neutral (D10): charview.cpp's
{
public:
	Unknown5DB670InterfaceClass(UIViewClass *view, LayerClass *layer);	// 0x5DB670  // NAME 1 [view, layer]: as LoginInterface's
	virtual ~Unknown5DB670InterfaceClass();				// 0x5DB9F0 (slot 0x5DB9C0)
	virtual void Unknown_60();							// 0x5DBB70 (every frame: the 30 second count down)
	virtual bool Unknown_68(int key);					// 0x5DBD00 (the buttons: 1-6 a site, 7 exit)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x5DBB00 (the model off the old view's scene, onto the new)
	void Unknown_5DBD50(int site);						// 0x5DBD50: the site's page opened  // NAME 1 [site]: the G_ASSERT's
	const char *Get_Site_URL(int site);					// 0x5DBEB0 (G_ASSERT 'site >= 0 && site < NUM_SITES')  // NAME 1: what it returns; NAME 3 [site]: the G_ASSERT's

	LayerClass Layer;									// +0x84 (a copy of the constructor's layer)  // NAME 1: what it holds
	LoginTaskClass *Task;								// +0xA8 (the character view's +0x14's slot +0x08)  // NAME 1: as LoginInterface's
	CharacterViewClass *CharacterView;					// +0xAC (the view's slot +0x28)  // NAME 1: as LoginInterface's
	RefCountPtr<RenderObjClass> Model;					// +0xB0 ('end_screen00')  // NAME 1: what it holds
	float Unknown0B4;									// +0xB4 (seconds shown; -1 before the first frame)
	GadgetClass *Buttons[7];							// +0xB8 ('logo00' .. 'exit')  // NAME 1: neutral (D10)
};

#endif
