// Earth & Beyond client (reconstruction): app\client\ui_petition_player.h, the petition's players window (its model
// 'hsi04a', 'fa_hsi04a.ini'; the tooltips' section 'UI_PETITION_PLAYER'). No string names the file:
// ui_petition_player.cpp and this header are provisional names (NAME 1, D137), after the tooltips' section and the link order.
#ifndef UI_PETITION_PLAYER_H  // NAME 1 [ui_petition_player.h]: provisional header name
#define UI_PETITION_PLAYER_H

#include "ui_petition_avatar_pip.h"

// The players window (0xC8 bytes, tables 0xAF5248 / 0xAF523C / 0xAF5230): the GM avatar's picture-in-picture and six
// buttons (1 exit, 2 bad name, 3 chat harassment, 4 game harassment, 5 spam, 6 cheating); 2-6 open the players page
// (ui_petition_player_entry.cpp, type 3-7) and keep it in Page until it closes. ui_petition.cpp's button 7 makes it.
class UIPetitionPlayerClass : public UIPetitionAvatarPipClass	// NAME 1: neutral, after its tooltips' section 'UI_PETITION_PLAYER'
{
public:
	UIPetitionPlayerClass(SClient *client);				// 0x5D5740 (deleting 0x5D57B0)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIPetitionPlayerClass();					// 0x5D57E0
	virtual void Unknown_54();							// +0x54 0x5D5880 ('hsi04a': the buttons, the back gadget at 'frame04', the PIP)
	virtual void Unknown_58();							// +0x58 0x5D5A50 (the base's)
	virtual void Unknown_5C();							// +0x5C 0x5D5A70 (the base's)
	virtual void Unknown_60();							// +0x60 0x5D5A90 (a closed page released, then the PIP's)
	virtual void Unknown_64();							// +0x64 0x5D5B50 (empty)
	virtual bool Unknown_68(int key);					// +0x68 0x5D5B70 (1 exit, 2-6 the players page)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5D5EC0 (InterfaceClass's, then the page's)  // NAME 1 [view, arg]: intrface.h's
	void Unknown_5D5F00(int key);						// 0x5D5F00 (buttons 2-6 on but key's; inlined into 0x5D5B70)  // NAME 1 [key]: Unknown_68's
	void Unknown_5D5F40();								// 0x5D5F40 (buttons 2-6 on; inlined into 0x5D5A90)

	RefCountPtr<ChatPanelUIBase> Page;					// +0xA8 (the page a button opened; released when its Closed is set)  // NAME 2: what it holds
	GadgetClass *Buttons[7];							// +0xAC (by id; [0] unused)  // NAME 2: what they are
};

#endif // UI_PETITION_PLAYER_H
