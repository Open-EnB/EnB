// Earth & Beyond client (reconstruction): app\client\ui_petition.h, the petition window (its model 'hsi01a',
// 'fa_hsi01a.ini'; the tooltips' section 'UI_PETITION'). No string names the file: ui_petition.cpp and this header are
// provisional names (NAME 1, D137), after the window's tooltips' section and its place before ui_petition_avatar_pip.cpp.
#ifndef UI_PETITION_H  // NAME 1 [ui_petition.h]: provisional header name
#define UI_PETITION_H

#include "ui_petition_avatar_pip.h"

// The petition window (0xCC bytes, tables 0xAF4E14 / 0xAF4E08 / 0xAF4DFC): the GM avatar's picture-in-picture and
// seven buttons (1 exit, 2 knowledge base, 3 bug, 4 billing, 5 gameplay, 6 stuck, 7 player); 2-4 open a pop-up browser,
// 5 / 6 the stuck page, 7 the players window, each kept in Page until it closes. The chat panel makes it (0x569A00).
class UIPetitionClass : public UIPetitionAvatarPipClass	// NAME 1: neutral, after its tooltips' section 'UI_PETITION'
{
public:
	UIPetitionClass(SClient *client);					// 0x5D33B0 (deleting 0x5D3420)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIPetitionClass();							// 0x5D3450
	virtual void Unknown_54();							// +0x54 0x5D34F0 ('hsi01a': the buttons, their colours, the back gadget at 'frame04', the PIP)
	virtual void Unknown_60();							// +0x60 0x5D3710 (the page checked, then the PIP's)
	virtual void Unknown_64();							// +0x64 0x5D3800 (the page checked)
	virtual bool Unknown_68(int key);					// +0x68 0x5D38F0 (1 exit, 2-4 a pop-up browser, 5 / 6 the stuck page, 7 the players window)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5D36D0 (InterfaceClass's, then the page's)  // NAME 1 [view, arg]: intrface.h's
	void Unknown_5D3F80();								// 0x5D3F80 (a closed page released, the buttons reset; inlined into 0x5D3710 / 0x5D3800)
	void Unknown_5D4070(int key);						// 0x5D4070 (buttons 2-7 on but key's; inlined into 0x5D38F0)  // NAME 1 [key]: Unknown_68's
	void Unknown_5D40B0();								// 0x5D40B0 (buttons 2-7 on, button 5's tooltip 'GAMEPLAY'; inlined into 0x5D3F80)

	RefCountPtr<ChatPanelUIBase> Page;					// +0xA8 (the window a button opened; released when its Closed is set)  // NAME 2: what it holds
	GadgetClass *Buttons[8];							// +0xAC (by id; [0] unused)  // NAME 2: what they are
};

#endif // UI_PETITION_H
