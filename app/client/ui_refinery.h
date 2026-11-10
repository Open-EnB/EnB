// Earth & Beyond client (reconstruction): app\client\ui_refinery.h, the refinery window (the client's ui_refinery.cpp per
// its asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x5EB810 and the destructor
// 0x5EBA50 (RF1, 2026-11-09); the window is ui_manu.cpp's manufacturing window cut down (TSM03A for TSM01A).
#ifndef UI_REFINERY_H  // NAME 1 [ui_refinery.h]: provisional header name for ui_refinery.cpp
#define UI_REFINERY_H

#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"

class GadgetSlider;				// gd_slider.h's slider (+0xB0)
class ButtonGadgetClass;		// unknown_675120.h's buttons (+0xBC, +0xCC)
class FontSys;					// fontmanager.h's text objects (+0xF4)
class SBaseViewClass;			// sbaseview.cpp's starbase view (+0xAC)
class DeltaCompoundClass;
class UIManuListClass;			// ui_manu.cpp's formula list ('KnownFormula')

// ui_inv.cpp's interface (0xE0 bytes, constructor 0x59EDC0) the window holds two of (+0xB4 'TSM03A.E%02d', +0xB8
// 'TSM03A.RESULT'), as far as this file calls it (as ui_manu.h's UIManuInvClass)
class UIRefineryInvClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	UIRefineryInvClass(SClient *client, RenderObjClass *model, int count, const char *format, int unknown);	// 0x59EDC0 (slot +0x54: 6 'TSM03A.E%02d' 12, 1 'TSM03A.RESULT' 13)  // NAME 1 [client, model, count, format, unknown]: as ui_manu.h's

	char Unknown84[0xE0 - 0x84];						// +0x84
};

// ui_refinery.cpp's refinery window (0xB08 bytes, tables 0xAF6DBC / 0xAF6DB0 / 0xAF6DA4; made by 0x69F8F0, sbaseview.cpp,
// case 5: the starbase view's +0x184)
class UIRefineryClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_refinery.cpp
{
public:
	UIRefineryClass(SBaseViewClass *view, const LayerClass &layer);	// 0x5EB810 (0xB08 bytes: 0x69F8F0's new)  // NAME 1 [view, layer]: what 0x69F8F0 passes (as ui_manu.h's)
	virtual ~UIRefineryClass();							// 0x5EBA50 (G_ASSERT 'State == UI_CLOSED', l.94; deleting 0x5EBA20)
	virtual void Unknown_54();							// 0x5EBC30
	virtual void Unknown_58();							// 0x5EC270
	virtual void Unknown_5C();							// 0x5EC570
	virtual void Unknown_60();							// 0x5EC6D0
	virtual bool Unknown_68(int key);					// 0x5EC990  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x5EC220 (InterfaceClass's, then the two sub-windows')  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5ECBB0(int index, bool show, const char *text);	// 0x5ECBB0 (FAIL_IF '!font', l.390): a text of Unknown_B85C48 set and shown  // NAME 1 [index, show, text]: as ui_manu.h's 0x5B1AF0
	void Unknown_5ECCE0(int row);						// 0x5ECCE0: row picked, its item id asked for  // NAME 1 [row]: neutral
	void Unknown_5ECDD0(int row);						// 0x5ECDD0: the row's text highlighted when it is the pick  // NAME 1 [row]: neutral
	void Unknown_5ECE20();								// 0x5ECE20: the rows refilled
	void Unknown_5ECEF0();								// 0x5ECEF0: the go and stack buttons by the item's 'Validity' and 'AdditionalIterations'
	void Unknown_5ED230();								// 0x5ED230: no pick, the slider to the top
	void Unknown_5ED2A0(int top);						// 0x5ED2A0: the slider to row top  // NAME 1 [top]: neutral
	void Unknown_5ED310();								// 0x5ED310: the first row from the slider (the rows refilled when it moved)
	void Unknown_5ED350();								// 0x5ED350: the slider moved to show the pick
	void Unknown_5ED440();								// 0x5ED440: a refresh asked for (+0xAF0 2)
	void Unknown_5ED470();								// 0x5ED470: the pick's item id asked for
	void Unknown_5ED530(int item_id);					// 0x5ED530: REFINERY_ITEM_ID sent  // NAME 2 [item_id]: the packet's ItemID
	void Unknown_5ED5F0();								// 0x5ED5F0: MANUFACTURE_ACTION 126 of 2 sent (refine)
	void Unknown_5ED800();								// 0x5ED800: MANUFACTURE_ACTION 126 of 3 sent (refine the stack)
	void Unknown_5EDA10(UIManuListClass *list);			// 0x5EDA10: the list filled from a formula list  // NAME 1 [list]: neutral
	void Unknown_5EDCC0();								// 0x5EDCC0: the credits and cost texts (0x5ECBB0 inlined twice)
	void Unknown_5EDFD0();								// 0x5EDFD0: the target's texts and the failure message

	SBaseViewClass *Unknown0AC;							// +0x0AC (the constructor's view)
	GadgetSlider *Unknown0B0;							// +0x0B0 (the list's slider: its +0xD4 the first row)
	RefCountPtr<UIRefineryInvClass> Unknown0B4;			// +0x0B4 'TSM03A.E%02d'
	RefCountPtr<UIRefineryInvClass> Unknown0B8;			// +0x0B8 'TSM03A.RESULT'
	ButtonGadgetClass *Unknown0BC[4];					// +0x0BC (+0xC0..+0xC8: slot +0x54's buttons 'exit', 'go', 'r_stack')
	ButtonGadgetClass *Unknown0CC[10];					// +0x0CC the rows' buttons ('desc%02d')
	RefCountPtr<FontSys> Unknown0F4[10];				// +0x0F4 the rows' texts
	const char *Unknown11C[500];						// +0x11C the list's names
	bool Unknown8EC[500];								// +0x8EC
	int Unknown0AE0;									// +0xAE0 the first row shown
	int Unknown0AE4;									// +0xAE4 the entries in the list
	int State;											// +0xAE8 (UI_CLOSED when shut)  // NAME 3: the assert's
	DeltaCompoundClass *Unknown0AEC;					// +0xAEC: the target item's aux data
	int Unknown0AF0;									// +0xAF0: what is pending (1 an item id, 2 a refresh, 3 an attempt)
	float Unknown0AF4;									// +0xAF4 (counted down by slot +0x60)
	int Unknown0AF8;									// +0xAF8 the pick (-1 for none)
	bool Unknown0AFC;									// +0xAFC (the PDA's 0x695780)
	bool Unknown0AFD;									// +0xAFD (the tutorial animation played)
	int Unknown0B00;									// +0xB00 the item id asked for (-1 for none)
	bool Unknown0B04;									// +0xB04 (a list shown)
	bool Unknown0B05;									// +0xB05 (an attempt made by the buttons)
};

#endif // UI_REFINERY_H
