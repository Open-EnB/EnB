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
class UIBackGadget;			// gd_uiback.h's
class MovableUIBackGadget;	// gd_uiback.h's
class CheckBoxGadgetClass;	// unknown_7e7e00.h's check box
class DestroyGadgetEvent;

// ui_chat_panel_base.cpp's class (0xA0 bytes; constructor 0x56D280 (the client to 0x540D30), destructor 0x56D300,
// table 0xAF00B4, 40 slots): a window over one model. The model's sub-objects become its gadgets by name (0x56D510 a
// button, 0x56D6F0 a check button), its back gadget takes the pointer and can drag the window (0x56D970), and its
// .ini gives the font styles of its texts (0x56DB20). The chat panel, the chat window and every window the chat panel
// opens derive from it; the chat panel releases a window when it finds Closed set.
class ChatPanelUIBase : public InterfaceClass	// NAME 5: the client's RTTI (the receiver in MemberCallback<MovableUIBackGadget, ChatPanelUIBase, void (ChatPanelUIBase::*)(MovableUIBackGadgetMoved *, MovableUIBackGadget *), MovableUIBackGadgetMoved> 0xB7D5E8: the typeid slot 0x56DF50 of the callback table 0xAF0174 that this class's destructor 0x56D330 builds with this as receiver and 0x56DEE0 as method)
{
public:
	ChatPanelUIBase(SClient *client);				// 0x56D280  // NAME 1 [client]: what 0x561DD0 passes
	virtual ~ChatPanelUIBase();					// 0x56D330 (deleting 0x56D300; out of line: ui_starbase.cpp's lights panel 0x646290 calls it)
	virtual void Unknown_58();							// +0x58 0x56D4F0 (InterfaceClass's: activates; Closed cleared first)
	virtual void Unknown_88(const RefCountPtr<RenderObjClass> &robj, const char *bone_name);	// +0x88 0x56DD80 (the model out of the layer and onto robj's bone; FAIL_IF '!robj', the assert's bone_name)
	virtual void Unknown_8C(const RefCountPtr<RenderObjClass> &robj);	// +0x8C 0x56DE60 (the model off robj and back into the layer; FAIL_IF 'robj.Peek() != InterfaceMesh->Get_Container()')
	virtual int Unknown_90() { return(0); }				// +0x90 0x5623B0 (emitted with the chat panel's table)
	virtual void Unknown_94() {}						// +0x94 0x5623D0
	virtual bool Unknown_98(const char *name, const char *ini);	// +0x98 0x56DB20 (the model and its ini: ui_starbase.cpp's lights panel passes 'light02a', 'fa_light02a.ini')  // NAME 1 [name, ini]: what 0x646360 passes
	virtual void Unknown_9C() {}						// +0x9C 0x5623F0 (the movable back gadget moved the window: 0x56DEE0 calls it)

	GadgetClass *Add_Button(int id, const char *bttn_name, const char *text);	// 0x56D510 (a button over the model's sub-object bttn_name, with the font style 'text' when given; the lights panel's 0x646360)  // NAME 2 [Add_Button]: what it does; NAME 1 [id, text]: what 0x646360 passes; NAME 3 [bttn_name]: the FAIL_IF's
	CheckBoxGadgetClass *Add_Check_Button(int id, const char *bttn_name, const char *text);	// 0x56D6F0 (FAIL_IF '!bttn_name || !*bttn_name', '!robj': a check button over the model's sub-object, 0x7E7F20; ui_option_channel.cpp passes 'check%02d' and NULL)  // NAME 2 [Add_Check_Button]: what it does; NAME 1 [id, text]: as 0x56D510's; NAME 3 [bttn_name]: the FAIL_IF's
	GadgetSlider *Add_Slider(int id, bool unknown);	// 0x56D8D0 (0x646360: its four sliders, ids 8-11; unknown is GadgetSlider's bool: the client pushes the argument's dword as loaded)  // NAME 2 [Add_Slider]: what it does; NAME 1 [id, unknown]: neutral
	UIBackGadget *Add_Back_Gadget(bool movable, const Vector3 &offset);	// 0x56D970 (FAIL_IF 'MovableBackGadget'; the back gadget over the whole model, id 0: movable, a MovableUIBackGadget kept in MovableBackGadget, with the offset; else a plain UIBackGadget)  // NAME 2 [Add_Back_Gadget]: what it does; NAME 1 [movable, offset]: what it does with them
	void On_Back_Gadget_Moved(DestroyGadgetEvent *event, GadgetClass *sender);	// 0x56DEE0 (MovableBackGadget moved the window: slot +0x9C. The client's RTTI 0xB7D5E8 gives the types: the event is MovableUIBackGadgetMoved and the sender MovableUIBackGadget; the tree's types stay until symbols.json's names of the callback's short functions 0x56DF10-0x56E130 follow)  // NAME 2 [On_Back_Gadget_Moved]: what it handles; NAME 1 [event, sender]: what the signal passes

	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0x84 the window's model (the asserts' name; its slot +0x70 finds the parts by name; the font styles' owner)  // NAME 3: the FAIL_IF 'robj.Peek() != InterfaceMesh->Get_Container()' (0x56DE60)
	RenderObjClass *Interface_Mesh() { return(InterfaceMesh.Peek()); }	// NAME 3: ui_chat_window.cpp's assert '!Interface_Mesh()' (0x56F780)
	std::string IniName;								// +0x88 (the font string file 0x56DB20 loads; the destructor unloads it)  // NAME 2: what 0x56DB20 stores
	int Closed;											// +0x98 (set by a window that closes itself, an exit button or a finished request; the chat panel's 0x5630B0 / 0x565F80 then release it; activation clears it)  // NAME 2: what the windows set it for
	MovableUIBackGadget *MovableBackGadget;				// +0x9C (0x56D970's movable back gadget)  // NAME 3: the FAIL_IF 'MovableBackGadget' (0x56D970)
};


#endif
