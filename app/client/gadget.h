// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gadget.h (name assumed from gadget.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\gadget.cpp'; the client holds no string of this header)
**
**	The UI kit's gadget: the base of every button, text and slider of the windows (the client's RTTI
**	.?AVGadgetClass@@: SignalBaseClass::MemberCallback<GadgetClass, ...>). Table 0xAF9AF0 (29 slots, +0x30 pure),
**	constructor 0x65ED50, destructor 0x65EE60 (G_ASSERT 'Destroyed'), 0x30 bytes. Its one-line slots
**	0x50A8C0-0x50AAC0 are emitted next to smouse.cpp (inline here). Declared as far as gadget.cpp and intrface.cpp
**	use it (D10); the windows' own stand-ins (ui_*.cpp) are merged into it later.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GADGET_H  // NAME 3 [gadget.h]: header name assumed from the client's gadget.cpp
#define __GADGET_H

class SignalMap;
class LayerClass;
class InterfaceClass;
class GadgetListManagerClass;
class RenderObjClass;
class UserClickIndicatorClass;
class Vector3;
class UIViewClass;

struct KernelEventClass;  // kernel.h's input event (0x10 bytes): what slots +0x2C / +0x34 get

// What slot +0x08 returns (zero in the base; ui_avsel.cpp's buttons: their model at +0x30)
struct GadgetUnknown08Struct  // NAME 1: neutral (D10)
{
	char Unknown00[0x30];
	RenderObjClass *Unknown30;											// +0x30 (ui_avsel.cpp 0x55E070 highlights it)
};

// What slot +0x64 returns (0x18 bytes, zero in the base)
struct GadgetUnknown64Struct  // NAME 1: neutral (D10)
{
	GadgetUnknown64Struct() { Unknown00[0] = 0; Unknown00[1] = 0; Unknown00[2] = 0; Unknown00[3] = 0; Unknown00[4] = 0; Unknown00[5] = 0; }
	int Unknown00[6];
};

// A gadget's flash (+0x20, 0x10 bytes): the model, its clone in the scene and the click indicator
struct GadgetFlashStruct  // NAME 1: neutral (D10)
{
	GadgetFlashStruct() : Model(NULL), Clone(NULL), Indicator(NULL), Unknown0C(false) {}
	~GadgetFlashStruct();												// 0x660320

	RenderObjClass *Model;												// +0x00 (slot +0x24's)  // NAME 1: neutral (D10)
	RenderObjClass *Clone;												// +0x04 (its clone, in the model's scene)  // NAME 1: neutral (D10)
	UserClickIndicatorClass *Indicator;									// +0x08 (0x3C bytes, user_click_indicator.cpp)  // NAME 1: neutral (D10)
	bool Unknown0C;														// +0x0C (0x6609A0: held)
};

class GadgetClass  // NAME 5: the client's RTTI .?AVGadgetClass@@
{
public:
	GadgetClass(unsigned int mask);										// 0x65ED50 (+0x10; the id from the counter 0xB8C9E0)  // NAME 1 [mask]: slot +0x34 masks its argument with it
	virtual ~GadgetClass();												// +0x00 0x65EE60 (G_ASSERT 'Destroyed'; deleting 0x65EDD0)
	virtual void Unknown_04();											// +0x04 0x65EF90 (empty; Destroy calls it before the delete)
	virtual GadgetUnknown08Struct *Unknown_08() { return(NULL); }		// +0x08 0x50A8C0
	virtual int Unknown_0C() { return(0); }								// +0x0C 0x50A8E0
	virtual int Unknown_10() { return(0); }								// +0x10 0x50A900
	virtual int Unknown_14() { return(0); }								// +0x14 0x50A920
	virtual int Unknown_18() { return(0); }								// +0x18 0x50A940
	virtual int Unknown_1C() { return(0); }								// +0x1C 0x50A960
	virtual void Unknown_20(int arg) {}									// +0x20 0x50A980  // NAME 1 [arg]: neutral
	virtual RenderObjClass *Unknown_24(Vector3 *unknown, Vector3 *position, bool indicator) { return(NULL); }	// +0x24 0x50A9A0 (the flash's model and where it is)  // NAME 1 [unknown, position, indicator]: what 0x6603A0 / 0x6609A0 pass
	virtual void Unknown_28(LayerClass *layer) {}						// +0x28 0x50A9C0 (the windows pass their layer)  // NAME 1 [layer]: what the windows pass
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x65F4B0 (the event gets the id; the owner's slot +0x44)  // NAME 1 [event, x, y, mask]: what 0x65F780 passes
	virtual bool Unknown_30(float x, float y) = 0;						// +0x30 (pure; 0x65F780: the pointer over the gadget)  // NAME 1 [x, y]: what 0x65F780 passes
	virtual bool Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x34 0x65F500 (mask &= +0x10, then slot +0x2C)  // NAME 1 [event, x, y, mask]: what 0x65F780 passes
	virtual int Unknown_38() { return(0); }								// +0x38 0x50A9E0
	virtual int Unknown_3C(int arg) { return(0); }						// +0x3C 0x50AA00  // NAME 1 [arg]: neutral
	virtual void Unknown_40(bool on);									// +0x40 0x65F640 (bit 5 of +0x2C; off: the held gadgets and the flash cleared)  // NAME 1 [on]: neutral
	virtual void Unknown_44(unsigned int mask, KernelEventClass *event, GadgetClass *gadget) {}	// +0x44 0x50AA20 (slot +0x2C: an owned gadget's event)  // NAME 1 [mask, event, gadget]: what 0x65F4B0 passes
	virtual void Set_Owner(GadgetClass *owner) { Owner = owner; }		// +0x48 0x50AA40  // NAME 1: neutral (D10): the member it sets
	virtual GadgetClass *Get_Owner() { return(Owner); }					// +0x4C 0x50AA60  // NAME 1: neutral (D10): the member it returns
	virtual void Unknown_50() {}										// +0x50 0x50AA80
	virtual void Unknown_54(GadgetListManagerClass *manager, InterfaceClass *iface);	// +0x54 0x6600F0 (FAIL_IF '!manager': onto the manager, 0x661700; 'InList')  // NAME 2 [manager]: the FAIL_IF's; NAME 1 [iface]: what intrface.cpp passes (its +0x78)
	virtual void Unknown_58(GadgetListManagerClass *manager);			// +0x58 0x660140 (FAIL_IF '!manager': off the manager, 0x6617E0)  // NAME 2 [manager]: the FAIL_IF's
	virtual int Get_Id() { return(Id); }								// +0x5C 0x50AAA0  // NAME 1: neutral (D10): the member it returns
	virtual bool Unknown_60() { return(Unknown2C_05); }					// +0x60 0x50AAC0 (bit 5 of +0x2C: slot +0x40's)
	virtual GadgetUnknown64Struct Unknown_64();							// +0x64 0x6602F0 (zero)
	virtual bool Unknown_68();											// +0x68 0x6601E0 (G_ASSERT '*sticky == NULL': held by the last event's buttons)
	virtual void Unknown_6C();											// +0x6C 0x660250 (G_ASSERT '*sticky == this': released)
	virtual void Unknown_70();											// +0x70 0x6602C0 (both held gadgets' slot +0x6C)

	static void Destroy(GadgetClass *del_me);							// 0x65EED0 (FAIL_IF '!del_me': DestroyGadgetEvent, slot +0x40 off, 'Destroyed', slot +0x04, deleted)  // NAME 2: describes what it does; NAME 2 [del_me]: the FAIL_IF's
	static SignalMap &Get_Signal_Map();									// 0x65ECE0: the function-local static 0xBE5EE8 (DestroyGadgetEvent, FlashingGadgetEvent)  // NAME 2: as ControlKeysManager's
	static void Set_Tooltip(GadgetClass *gadget, const char *section, const char *entry, ...);	// 0x65EFB0 (tooltip.ini; ui_group.cpp: "UI_FORMATION", "HELP")  // NAME 2: describes what it does; NAME 1 [gadget, section, entry]: INIClass's
	void Set_Unknown14(const char *text);								// 0x65F2D0 (the tooltip's text; ui_chat_panel.cpp passes NULL)  // NAME 1: neutral (D10): the member it sets; NAME 1 [text]: neutral
	const char *Get_Unknown14();										// 0x65F320  // NAME 1: neutral (D10): the member it returns
	static void Set_Tooltip_Name(GadgetClass *gadget, const char *format, ...);	// 0x65F340 ('_%d' after the name)  // NAME 2: describes what it does; NAME 1 [gadget, format]: neutral
	static GadgetClass *Unknown_65F5F0(GadgetListManagerClass *manager, bool a, bool b);	// 0x65F5F0 (FAIL_IF '!manager': the manager's 0x661AA0)  // NAME 2 [manager]: the FAIL_IF's; NAME 1 [a, b]: neutral
	static KernelEventClass Process_Input(GadgetListManagerClass *manager);	// 0x65F780 (the input queue's next event to the manager's gadgets)  // NAME 1: describes what it does; NAME 1 [manager]: neutral
	void Unknown_65FFA0(int arg);										// 0x65FFA0 (+0x0C; ui_group.cpp passes -1)  // NAME 1 [arg]: neutral
	void Unknown_65FFC0(int arg, GadgetClass *gadget);					// 0x65FFC0 (+0x0C: the gadget's, or this one's, plus arg)  // NAME 1 [arg, gadget]: neutral
	void Set_First_Sort_Group(bool on);									// 0x65FFF0 (FAIL_IF 'InList', 'LastSortGroup')  // NAME 2: the FAIL_IFs; NAME 1 [on]: neutral
	void Set_Last_Sort_Group(bool on);									// 0x660070 (FAIL_IF 'InList', 'FirstSortGroup')  // NAME 2: the FAIL_IFs; NAME 1 [on]: neutral
	void Unknown_6603A0(bool indicator, UIViewClass *view);				// 0x6603A0 (G_ASSERT '!Flash': the flash started; ui_group.cpp)  // NAME 1 [indicator, view]: neutral
	void Unknown_6607D0();												// 0x6607D0 (the flash stopped, FlashingGadgetEvent)
	void Unknown_6608F0();												// 0x6608F0 (the flash's clone to the model's transform)
	bool Is_Unknown2C_00();												// 0x660940  // NAME 1: neutral (D10)
	InterfaceClass *Get_Interface() { return(Unknown08); }				// (inline: gadget_list_manager.cpp's 0x661BB0 copies it)  // NAME 1: neutral (D10): what it returns
	bool Is_Unknown2C_02() { return(Unknown2C_02); }					// (inline: gadget_list_manager.cpp's 0x661AA0)  // NAME 1: neutral (D10)
	bool Is_Unknown2C_04() { return(Unknown2C_04); }					// (inline: 0x661AA0)  // NAME 1: neutral (D10)
	bool Is_First_Sort_Group() { return(FirstSortGroup); }				// (inline: 0x661BB0)  // NAME 2: as Set_First_Sort_Group
	bool Is_Last_Sort_Group() { return(LastSortGroup); }				// (inline: 0x661BB0)  // NAME 2: as Set_Last_Sort_Group
	void Toggle_Unknown2C_00();											// 0x660960 (bit 0, with +0x28's half second)  // NAME 1: neutral (D10)
	void Unknown_6609A0();												// 0x6609A0 (FAIL_IF '!Flash': the flash's frame)

	int Id;																// +0x04 (from the counter 0xB8C9E0; slot +0x5C)  // NAME 2: what slot +0x2C writes into the event
	InterfaceClass *Unknown08;											// +0x08 (slot +0x54's interface; 0x65F780 emits GadgetGroupToFrontEvent with it)
	int Unknown0C;														// +0x0C (0x65FFA0 / 0x65FFC0)
	unsigned int Mask;													// +0x10 (the constructor's; slot +0x34 masks with it)  // NAME 1: neutral (D10)
	char *Unknown14;													// +0x14 (the tooltip's text: 0x65EFB0, 0x65F2D0; 0x65F320 returns it)
	GadgetClass *Owner;													// +0x18 (slots +0x48 / +0x4C; slot +0x2C calls its +0x44)  // NAME 1: neutral (D10)
	int Unknown1C;														// +0x1C
	GadgetFlashStruct *Flash;											// +0x20 (the asserts' '!Flash')  // NAME 3: the asserts'
	char *Unknown24;													// +0x24 (the tooltip's name, '%s_%s': 0x65F340)
	float Unknown28;													// +0x28 (slot +0x34: a timer, 0.5 when bit 0 is set)
	unsigned int Unknown2C_00 : 1;										// +0x2C bit 0 (slot +0x34 toggles it)
	unsigned int InList : 1;											// +0x2C bit 1 (FAIL_IF 'InList'; slot +0x54 sets it)  // NAME 3: the FAIL_IFs'
	unsigned int Unknown2C_02 : 1;										// +0x2C bit 2 (0x65F780: under the pointer; the windows' mouse-over sound)
	unsigned int Unknown2C_03 : 1;										// +0x2C bit 3
	unsigned int Unknown2C_04 : 1;										// +0x2C bit 4
	unsigned int Unknown2C_05 : 1;										// +0x2C bit 5 (slot +0x40 / +0x60; the constructor sets it)
	unsigned int Unknown2C_06 : 1;										// +0x2C bit 6
	unsigned int Unknown2C_07 : 1;										// +0x2C bit 7 (the tooltip changed)
	unsigned int Unknown2C_08 : 1;										// +0x2C bit 8
	unsigned int Destroyed : 1;											// +0x2C bit 9 (G_ASSERT 'Destroyed')  // NAME 3: the assert's
	unsigned int Unknown2C_0A : 1;										// +0x2C bit 10 (0x6603A0: no model)
	unsigned int Unknown2C_0B : 1;										// +0x2C bit 11 (0x6609A0: the flash stays)
	unsigned int FirstSortGroup : 1;									// +0x2C bit 12 (FAIL_IF 'FirstSortGroup')  // NAME 3: the FAIL_IF's
	unsigned int LastSortGroup : 1;										// +0x2C bit 13 (FAIL_IF 'LastSortGroup')  // NAME 3: the FAIL_IF's
	unsigned int Unknown2C_0E : 18;										// +0x2C bits 14-31 (the constructor leaves them)
};

// The gadget intrface.cpp news for a partial deactivation (0x40 bytes, constructor 0x660D30, table 0xAF9B80)
class InterfaceDeactivationGadgetClass : public GadgetClass  // NAME 1: neutral (D10)
{
public:
	InterfaceDeactivationGadgetClass(int a, int b, int c, int d, int e);	// 0x660D30 (intrface.cpp passes 0, 0, 0, 0, 0)  // NAME 1 [a, b, c, d, e]: neutral
	virtual bool Unknown_30(float x, float y);							// +0x30 0x660DD0
	virtual GadgetUnknown64Struct Unknown_64();							// +0x64 0x660E40

	char Unknown30[0x40 - 0x30];										// +0x30
};

class DestroyGadgetEvent  // NAME 5: the client's RTTI .?AVDestroyGadgetEvent@@
{
};

class FlashingGadgetEvent  // NAME 5: the client's RTTI .?AVFlashingGadgetEvent@@
{
public:
	FlashingGadgetEvent(GadgetClass *gadget) : Gadget(gadget) {}		// NAME 1 [gadget]: neutral

	GadgetClass *Gadget;												// +0x00  // NAME 1: neutral (D10)
};

class GadgetGroupToFrontEvent  // NAME 5: the client's RTTI .?AVGadgetGroupToFrontEvent@@
{
public:
	GadgetGroupToFrontEvent(InterfaceClass *iface) : Interface(iface) {}	// NAME 1 [iface]: neutral

	InterfaceClass *Interface;											// +0x00  // NAME 1: neutral (D10)
};

extern GadgetClass *Unknown_BE5F24;										// 0xBE5F24 (held by the first button: slot +0x68)  // NAME 1: neutral (D10)
extern GadgetClass *Unknown_BE5F28;										// 0xBE5F28 (held by the second button)  // NAME 1: neutral (D10)

#endif // __GADGET_H
