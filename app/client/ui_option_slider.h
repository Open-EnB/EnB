// Earth & Beyond client (reconstruction): app\client\ui_option_slider.h, the options' slider pages (the model 'opt04a'
// with 'fa_opt04a-graphics.ini', 'fa_opt04a-sound.ini' or 'fa_opt04a-interface.ini'; the tooltips' section
// 'UI_OPTION_SLIDER'). No string names the file: ui_option_slider.cpp and this header are provisional names (NAME 1,
// D137: P-M1-2), after ui_option_quit.cpp in the client (CP1).
#ifndef UI_OPTION_SLIDER_H  // NAME 1 [ui_option_slider.h]: provisional header name
#define UI_OPTION_SLIDER_H

#include "ui_chat_panel_base.h"

class CheckBoxGadgetClass;	// ui_manu.cpp's check button (0x7E8030: checked or not; +0x64)
class GadgetSlider;				// gd_slider.h's

// The slider pages (0xE4 bytes, tables 0xAF4974 / 0xAF4968 / 0xAF495C): the graphics (page 0), the sound (page 1) or the
// interface (page 2); the chat panel's requests 5-7
class UIOptionSliderClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_OPTION_SLIDER'
{
public:
	UIOptionSliderClass(SClient *client, int page, bool mode);	// 0x5CEDD0 (deleting 0x5CEE90)  // NAME 1 [client, page, mode]: what it does with them
	virtual ~UIOptionSliderClass();						// 0x5CEEC0 (the sliders off the model)
	virtual void Unknown_54();							// +0x54 0x5CEFB0 ('opt04a': the buttons 'exit', 'ok', 'cancel', 'resolution', the five sliders, the check)
	virtual void Unknown_58();							// +0x58 0x5CF230 (the base's, then 0x5CF6C0 and 0x5CF580, when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5CF2C0 (InterfaceClass's when Active)
	virtual void Unknown_60();							// +0x60 0x5CF2E0 (until Closed: the sliders into the options, 0x5CF880)
	virtual bool Unknown_68(int key);					// +0x68 0x5CF310 (1 exit, 3 cancel: the options back; 2 ok: the sliders kept; 4 resolution: the chat panel's page 10; 12 the check)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5CF580();								// 0x5CF580 (page 0: the sliders' texts and the sliders on unless the check is set)
	void Unknown_5CF6C0();								// 0x5CF6C0 (the check and the sliders from the options, the values kept)
	void Unknown_5CF7E0();								// 0x5CF7E0 (the check and the sliders into the options)
	void Unknown_5CF880(bool restore);					// 0x5CF880 (page 1: the sliders, or the kept values, into the options)  // NAME 1 [restore]: what it does

	int Page;											// +0xA0 (the constructor's; 0 graphics, 1 sound, 2 interface: the ini, the texts and the options)  // NAME 1: neutral (D10)
	bool Mode;											// +0xA4 (the constructor's; false: ok and cancel go back to the chat panel's page 1)  // NAME 1: neutral (D10)
	CheckBoxGadgetClass *Check;						// +0xA8 'check00' (page 0: Graphics_Default, 1: Doppler_Enabled)  // NAME 2: what it is
	GadgetClass *Buttons[4];							// +0xAC 'exit', 'ok', 'cancel', 'resolution'  // NAME 2: what they are
	GadgetSlider *Sliders[5];							// +0xBC ids 6-10, one per option  // NAME 2: what they are
	float Values[5];									// +0xD0 the options' values when the page opened (0x5CF6C0)  // NAME 2: what they hold
};

#endif // UI_OPTION_SLIDER_H
