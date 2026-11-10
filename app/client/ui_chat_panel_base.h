// Earth & Beyond client (reconstruction): app\client\ui_chat_panel_base.h, the base of the chat panel and the chat
// window (the client's ui_chat_panel_base.cpp per its asserts; this header's name is provisional, NAME 1). Moved out
// of ui_chat_panel.h for ui_chat_window.h (CW2, 2026-11-08).
#ifndef UI_CHAT_PANEL_BASE_H  // NAME 1 [ui_chat_panel_base.h]: provisional header name for ui_chat_panel_base.cpp
#define UI_CHAT_PANEL_BASE_H

#include <string>
#include "sclient.h"

class RenderObjClass;
class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class UIManuCheckButtonClass;	// ui_manu.cpp's check button
class DestroyGadgetEvent;

// ui_chat_panel_base.cpp's class (0xA0 bytes; constructor 0x56D280 (the client to 0x540D30), destructor 0x56D300,
// table 0xAF00B4), as far as the chat panel and the chat window use it
class ChatPanelUIBase : public InterfaceClass	// NAME 5: the client's RTTI (the receiver in MemberCallback<MovableUIBackGadget, ChatPanelUIBase, ...> 0xB7D5E8: the typeid slot 0x56DF50 of the callback table 0xAF0174 that this class's destructor 0x56D330 builds with this as receiver and 0x56DEE0 as method)
{
public:
	ChatPanelUIBase(SClient *client);				// 0x56D280  // NAME 1 [client]: what 0x561DD0 passes
	virtual ~ChatPanelUIBase();					// 0x56D330 (deleting 0x56D300; out of line: ui_starbase.cpp's lights panel 0x646290 calls it)
	virtual void Unknown_58();							// +0x58 0x56D4F0
	virtual void Unknown_88(const RefCountPtr<RenderObjClass> &robj, const char *bone_name);	// +0x88 0x56DD80 (FAIL_IF '!robj', the assert's bone_name)
	virtual void Unknown_8C(const RefCountPtr<RenderObjClass> &robj);	// +0x8C 0x56DE60 (FAIL_IF 'robj.Peek() != InterfaceMesh->Get_Container()')
	virtual int Unknown_90() { return(0); }				// +0x90 0x5623B0 (emitted with the chat panel's table)
	virtual void Unknown_94() {}						// +0x94 0x5623D0
	virtual bool Unknown_98(const char *name, const char *ini);	// +0x98 0x56DB20 (the model and its ini: ui_starbase.cpp's lights panel passes 'light02a', 'fa_light02a.ini')  // NAME 1 [name, ini]: what 0x646360 passes
	virtual void Unknown_9C() {}						// +0x9C 0x5623F0

	GadgetClass *Unknown_56D510(int id, const char *button, const char *text);	// 0x56D510 (the lights panel's 0x646360: a button by its model's name, with its text)  // NAME 1 [id, button, text]: what 0x646360 passes
	UIManuCheckButtonClass *Unknown_56D6F0(int id, const char *bttn_name, const char *text);	// 0x56D6F0 (FAIL_IF '!bttn_name || !*bttn_name', '!robj': a check button by its model's name, 0x7E7F20; ui_option_channel.cpp passes 'check%02d' and NULL)  // NAME 1 [id, text]: as 0x56D510's; NAME 3 [bttn_name]: the FAIL_IF's
	GadgetSlider *Unknown_56D8D0(int id, bool unknown);	// 0x56D8D0 (0x646360: its four sliders, ids 8-11; unknown is GadgetSlider's bool: the client pushes the argument's dword as loaded)  // NAME 1 [id, unknown]: neutral
	void Unknown_56DEE0(DestroyGadgetEvent *event, GadgetClass *sender);	// 0x56DEE0 (the handler of +0x9C's destruction: slot +0x9C)  // NAME 1 [event, sender]: what the signal passes
	void Unknown_56D970(int id, const Vector3 &offset);	// 0x56D970 (G_ASSERT 'MovableBackGadget'; 0x646360 passes 1 and a zero vector)  // NAME 1 [id, offset]: neutral

	RefCountPtr<RenderObjClass> Unknown84;				// +0x84, the asserts' InterfaceMesh (its slot +0x70 finds the panel's parts by name; the font styles' owner)
	RenderObjClass *Peek_Unknown84() { return(Unknown84.Peek()); }	// NAME 1: neutral (D10)
	RenderObjClass *Interface_Mesh() { return(Unknown84.Peek()); }	// NAME 3: ui_chat_window.cpp's assert '!Interface_Mesh()' (0x56F780)
	std::string Unknown88;								// +0x88 (the string file 0x56DB20 loads; the destructor unloads it)
	int Unknown98;										// +0x98
	int Unknown9C;										// +0x9C
};

#endif
