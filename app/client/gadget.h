// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gadget.h (name assumed from gadget.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\gadget.cpp'; the client holds no string of this header)
**
**	The UI kit's gadget: the base of every button, text and slider of the windows (the client's RTTI
**	.?AVGadgetClass@@: SignalBaseClass::MemberCallback<GadgetClass, ...>). Table 0xAF9AF0 (29 slots, +0x30 pure),
**	constructor 0x65ED50 (inline, out of line in gadget.obj), destructor 0x65EE60 (G_ASSERT 'Destroyed'), 0x30
**	bytes. Its one-line slots 0x50A8C0-0x50AAC0 are emitted next to smouse.cpp (inline here). Declared as far as
**	intrface.cpp uses it (D10); the windows' own stand-ins (ui_*.cpp) are merged into it later.
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

class GadgetClass  // NAME 5: the client's RTTI .?AVGadgetClass@@
{
public:
	GadgetClass(unsigned int mask);										// 0x65ED50 (+0x10; the id from the counter 0xB8C9E0)  // NAME 1 [mask]: slot +0x34 masks its argument with it
	virtual ~GadgetClass();												// +0x00 0x65EE60 (G_ASSERT 'Destroyed'; deleting 0x65EDD0)
	virtual void Unknown_04();											// +0x04 0x65EF90 (empty; Destroy calls it before the delete)
	virtual int Unknown_08() { return(0); }								// +0x08 0x50A8C0
	virtual int Unknown_0C() { return(0); }								// +0x0C 0x50A8E0
	virtual int Unknown_10() { return(0); }								// +0x10 0x50A900
	virtual int Unknown_14() { return(0); }								// +0x14 0x50A920
	virtual int Unknown_18() { return(0); }								// +0x18 0x50A940
	virtual int Unknown_1C() { return(0); }								// +0x1C 0x50A960
	virtual void Unknown_20(int arg) {}									// +0x20 0x50A980  // NAME 1 [arg]: neutral
	virtual int Unknown_24(int a, int b, int c) { return(0); }			// +0x24 0x50A9A0  // NAME 1 [a, b, c]: neutral
	virtual void Unknown_28(LayerClass *layer) {}						// +0x28 0x50A9C0 (the windows pass their layer)  // NAME 1 [layer]: what the windows pass
	virtual bool Unknown_2C(short *event, int a, int b, unsigned int mask);	// +0x2C 0x65F4B0 (the event gets the id)  // NAME 1 [event, a, b, mask]: neutral
	virtual bool Unknown_30(float x, float y) = 0;						// +0x30 (pure; ui_avsel.cpp: the pointer over the gadget)  // NAME 1 [x, y]: neutral
	virtual bool Unknown_34(short *event, int a, int b, unsigned int mask);	// +0x34 0x65F500 (mask &= +0x10, then slot +0x2C)  // NAME 1 [event, a, b, mask]: neutral
	virtual int Unknown_38() { return(0); }								// +0x38 0x50A9E0
	virtual int Unknown_3C(int arg) { return(0); }						// +0x3C 0x50AA00  // NAME 1 [arg]: neutral
	virtual void Unknown_40(bool on);									// +0x40 0x65F640 (bit 5 of +0x2C; off: the flash and the held gadgets cleared)  // NAME 1 [on]: neutral
	virtual void Unknown_44(int a, int b, int c) {}						// +0x44 0x50AA20  // NAME 1 [a, b, c]: neutral
	virtual void Set_Unknown18(void *arg) { Unknown18 = arg; }			// +0x48 0x50AA40  // NAME 1: neutral (D10): the member it sets
	virtual void *Get_Unknown18() { return(Unknown18); }				// +0x4C 0x50AA60  // NAME 1: neutral (D10): the member it returns
	virtual void Unknown_50() {}										// +0x50 0x50AA80
	virtual void Unknown_54(GadgetListManagerClass *manager, InterfaceClass *parent);	// +0x54 0x6600F0 (FAIL_IF '!manager': onto the manager, 0x661700)  // NAME 2 [manager]: the FAIL_IF's; NAME 1 [parent]: what intrface.cpp passes (its +0x78)
	virtual void Unknown_58(GadgetListManagerClass *manager);			// +0x58 0x660140 (FAIL_IF '!manager': off the manager, 0x6617E0)  // NAME 2 [manager]: the FAIL_IF's
	virtual int Get_Id() { return(Id); }								// +0x5C 0x50AAA0  // NAME 1: neutral (D10): the member it returns
	virtual bool Unknown_60() { return(Unknown2C_05); }					// +0x60 0x50AAC0 (bit 5 of +0x2C: slot +0x40's)
	virtual void Unknown_64();											// +0x64 0x6602F0
	virtual void Unknown_68(GadgetClass **sticky);						// +0x68 0x6601E0 (G_ASSERT '*sticky == NULL')  // NAME 2 [sticky]: the assert's
	virtual void Unknown_6C(GadgetClass **sticky);						// +0x6C 0x660250 (G_ASSERT '*sticky == this')  // NAME 2 [sticky]: the assert's
	virtual void Unknown_70();											// +0x70 0x6602C0

	static void Destroy(GadgetClass *del_me);							// 0x65EED0 (FAIL_IF '!del_me': DestroyGadgetEvent, slot +0x40 off, 'Destroyed', slot +0x04, deleted)  // NAME 2: describes what it does; NAME 2 [del_me]: the FAIL_IF's
	static SignalMap &Get_Signal_Map();									// 0x65ECE0: the function-local static 0xBE5EE8 (DestroyGadgetEvent, FlashingGadgetEvent)  // NAME 2: as ControlKeysManager's

	int Id;																// +0x04 (from the counter 0xB8C9E0; slot +0x5C)  // NAME 2: what slot +0x2C writes into the event
	int Unknown08;														// +0x08
	int Unknown0C;														// +0x0C
	unsigned int Mask;													// +0x10 (the constructor's; slot +0x34 masks with it)  // NAME 1: neutral (D10)
	char *Unknown14;													// +0x14 (a tooltip's text: 0x65EFB0, 0x65F2D0; 0x65F320 returns it)
	void *Unknown18;													// +0x18 (slots +0x48 / +0x4C; slot +0x2C calls its +0x44)
	int Unknown1C;														// +0x1C
	void *Unknown20;													// +0x20 (the flash: slot +0x40 deletes it)
	char *Unknown24;													// +0x24 (the tooltip's name, '%s_%s': 0x65F340)
	float Unknown28;													// +0x28 (slot +0x34: a timer, 0.5 when bit 0 is set)
	unsigned int Unknown2C_00 : 1;										// +0x2C bit 0 (slot +0x34 toggles it)
	unsigned int Unknown2C_01 : 4;										// +0x2C bits 1-4
	unsigned int Unknown2C_05 : 1;										// +0x2C bit 5 (slot +0x40 / +0x60; the constructor sets it)
	unsigned int Unknown2C_06 : 1;										// +0x2C bit 6
	unsigned int Unknown2C_07 : 1;										// +0x2C bit 7 (the tooltip changed)
	unsigned int Unknown2C_08 : 1;										// +0x2C bit 8
	unsigned int Destroyed : 1;											// +0x2C bit 9 (G_ASSERT 'Destroyed')  // NAME 3: the assert's
	unsigned int Unknown2C_0A : 1;										// +0x2C bit 10 (ui_avsel.cpp sets it)
	unsigned int Unknown2C_0B : 1;										// +0x2C bit 11 (ui_group.cpp: 0x6603A0 sets it, 0x6607D0 clears it)
	unsigned int Unknown2C_0C : 2;										// +0x2C bits 12-13
	unsigned int Unknown2C_0E : 18;										// +0x2C bits 14-31 (the constructor leaves them)
};

// The gadget intrface.cpp news for a partial deactivation (0x40 bytes, constructor 0x660D30, table 0xAF9B80)
class InterfaceDeactivationGadgetClass : public GadgetClass  // NAME 1: neutral (D10)
{
public:
	InterfaceDeactivationGadgetClass(int a, int b, int c, int d, int e);	// 0x660D30 (intrface.cpp passes 0, 0, 0, 0, 0)  // NAME 1 [a, b, c, d, e]: neutral
	virtual bool Unknown_30(float x, float y);							// +0x30 0x660DD0
	virtual void Unknown_64();											// +0x64 0x660E40

	char Unknown30[0x40 - 0x30];										// +0x30
};

#endif // __GADGET_H
