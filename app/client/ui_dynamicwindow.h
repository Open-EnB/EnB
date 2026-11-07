// Earth & Beyond client (reconstruction): app\client\ui_dynamicwindow.h, the screen that shows a dynamic window (the
// client's ui_dynamicwindow.cpp, 'e:\build\beta\G\app\Client\ui_dynamicwindow.cpp' at 0xB7EF24; this header's name is
// provisional, NAME 1). Declarations only, laid out from the constructor 0x589D30 (its code is not in the tree yet).
#ifndef UI_DYNAMICWINDOW_H  // NAME 1 [ui_dynamicwindow.h]: provisional header name for ui_dynamicwindow.cpp
#define UI_DYNAMICWINDOW_H

#include "intrface.h"
#include "..\..\wwlib\ref_ptr.h"
#include <vector>

class GadgetClass;
class DynamicWindowClass;
class Vector2;
class DynamicWindowElementClass;

// The window's gadget side at +0x84 (table 0xAF1380: the destructor 0x58A030 / deleting 0x58A050 and four pure slots).
// The panels get it as their listener (ui_item_examination_options.cpp's rows)
class UIDynamicWindowUnknown84Class  // NAME 1: neutral (D10)
{
public:
	virtual ~UIDynamicWindowUnknown84Class() {}											// 0x58A030 (deleting 0x58A050)
	virtual GadgetClass *Unknown84_04() = 0;													// +0x04 (0x58AA40: +0x8C, the main gadget)  // NAME 1: neutral (D10)
	virtual void Unknown84_08(GadgetClass *gadget, int arg1, int arg2) = 0;					// +0x08 (0x58AA60: FAIL_IF 'Is_Active()', '!gadget')  // NAME 1 [gadget, arg1, arg2]: neutral
	virtual void Unknown84_0C(GadgetClass *gadget) = 0;										// +0x0C (0x58AD00: out of the gadget list)  // NAME 1 [gadget]: neutral
	virtual int Unknown84_10() = 0;															// +0x10 (0x58AD80: +0x90, the next id)  // NAME 1: neutral (D10)
};

// ui_dynamicwindow.cpp's screen (0xAC bytes; tables 0xAF12DC / 0xAF12D0 / 0xAF12C4 / 0xAF12AC)
class UIDynamicWindowClass : public InterfaceClass, public UIDynamicWindowUnknown84Class  // NAME 3: class-from-file (D24) for app\Client\ui_dynamicwindow.cpp
{
public:
	UIDynamicWindowClass(LineDisplayClientClass *client, const char *name, int flags);		// 0x589D30 (InterfaceClass 0x540D30; a 'STANDARD_BLANK_WINDOW' without a name)  // NAME 1 [client, name, flags]: what it passes on
	virtual ~UIDynamicWindowClass();														// 0x58A0B0 (slot 0x58A080)
	virtual void Unknown_58();																// 0x58A2C0  // NAME 1: neutral (D10)
	virtual void Unknown_5C();																// 0x58A390  // NAME 1: neutral (D10)
	virtual void Unknown_60();																// 0x58A4B0  // NAME 1: neutral (D10)
	virtual bool Unknown_68(int key);														// 0x58A3C0  // NAME 1 [key]: InterfaceClass's
	virtual GadgetClass *Unknown84_04();														// 0x58AA40  // NAME 1: neutral (D10)
	virtual void Unknown84_08(GadgetClass *gadget, int arg1, int arg2);						// 0x58AA60  // NAME 1: neutral (D10)
	virtual void Unknown84_0C(GadgetClass *gadget);											// 0x58AD00  // NAME 1: neutral (D10)
	virtual int Unknown84_10();																// 0x58AD80  // NAME 1: neutral (D10)
	void Add_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index);	// 0x58A950 (FAIL_IF '!MainWindow'; MainWindow's Add_Element)  // NAME 1 [Add_Element, element, row_index]: DynamicWindowClass's
	void Remove_All(bool recursive);														// 0x58A9A0 (FAIL_IF '!MainWindow'; MainWindow's Remove_All)  // NAME 1 [Remove_All, recursive]: DynamicWindowClass's
	void Unknown_58A580(Vector2 position);													// 0x58A580 (ui_starmap.cpp's 0x7C8490)  // NAME 2 [position]: what the caller passes
	void Unknown_58ADA0();																	// 0x58ADA0 (FAIL_IF 'Is_Active()')  // NAME 1: neutral (D10)

	RefCountPtr<DynamicWindowClass> MainWindow;												// +0x88 (FAIL_IF '!MainWindow')  // NAME 3: the FAIL_IF's
	GadgetClass *Unknown8C;																	// +0x8C (slot +0x04)  // NAME 1: neutral (D10)
	int Unknown90;																			// +0x90 (0x457; slot +0x10 counts on)  // NAME 1: neutral (D10)
	std::vector<GadgetClass *> Unknown94;													// +0x94 (slot +0x0C searches it)  // NAME 1: neutral (D10)
	int UnknownA4;																			// +0xA4  // NAME 1: neutral (D10)
	bool UnknownA8;																			// +0xA8 (the constructor: true)  // NAME 1: neutral (D10)
	bool UnknownA9;																			// +0xA9 (the constructor: true)  // NAME 1: neutral (D10)
};

#endif
