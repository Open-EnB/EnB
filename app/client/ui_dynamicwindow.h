// Earth & Beyond client (reconstruction): app\client\ui_dynamicwindow.h, the screen that shows a dynamic window (the
// client's ui_dynamicwindow.cpp, 'e:\build\beta\G\app\Client\ui_dynamicwindow.cpp' at 0xB7EF24; this header's name is
// provisional, NAME 1). The object is 0x589D30-0x58B62F (the constructor first; the adjustor thunks 0x58B600 / 0x58B610 /
// 0x58B620 last; the next object opens with a PermStringInitClass static at 0x58B630).
#ifndef UI_DYNAMICWINDOW_H  // NAME 1 [ui_dynamicwindow.h]: provisional header name for ui_dynamicwindow.cpp
#define UI_DYNAMICWINDOW_H

#include "intrface.h"
#include "..\..\wwlib\ref_ptr.h"
#include <vector>

class GadgetClass;
class DynamicWindowClass;
class Vector2;
class DynamicWindowElementClass;
class DynamicWindowRenderObjectClass;
class DynamicWindowGadgetClass;
class RenderObjClass;

// The window's gadget side at +0x84 (table 0xAF1380): GadgetCollectionInterface
#include "gadget_collection_interface.h"

// ui_dynamicwindow.cpp's screen (0xAC bytes; tables 0xAF12DC / 0xAF12D0 / 0xAF12C4 / 0xAF12AC)
class UIDynamicWindowClass : public InterfaceClass, public GadgetCollectionInterface  // NAME 3: class-from-file (D24) for app\Client\ui_dynamicwindow.cpp
{
public:
	UIDynamicWindowClass(LineDisplayClientClass *client, const char *name, int level);		// 0x589D30 (InterfaceClass 0x540D30; a 'STANDARD_BLANK_WINDOW' without a name; MainWindow's Set_Level(level))  // NAME 1 [client, name]: what it passes on; NAME 2 [level]: Set_Level's
	virtual ~UIDynamicWindowClass();														// 0x58A0B0 (slot 0x58A080)
	virtual void Unknown_58();																// 0x58A2C0 (activates: MainWindow laid out and into the view's scene)  // NAME 1: neutral (D10)
	virtual void Unknown_5C();																// 0x58A390 (MainWindow out of the scene)  // NAME 1: neutral (D10)
	virtual void Unknown_60();																// 0x58A4B0 (UnknownA8: deactivates 1000 ms after the pointer leaves +0x8C)  // NAME 1: neutral (D10)
	virtual bool Unknown_68(int key);														// 0x58A3C0 (a GadgetEvent with the id; 0xFFFFFF: Unknown_5C when UnknownA9)  // NAME 1 [key]: InterfaceClass's
	virtual GadgetClass *Unknown84_04();														// 0x58AA40  // NAME 1: neutral (D10)
	virtual void Unknown84_08(GadgetClass *gadget, int arg1, int arg2);						// 0x58AA60  // NAME 1: neutral (D10)
	virtual void Unknown84_0C(GadgetClass *gadget);											// 0x58AD00  // NAME 1: neutral (D10)
	virtual int Unknown84_10();																// 0x58AD80  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Get_Render_Object();										// 0x58A530 (MainWindow's +0x08, or NULL; dynamic_window_move.cpp's 0x51A9C0)  // NAME 1 [Get_Render_Object]: DynamicWindowRenderObjectClass's
	void Add_Object(DynamicWindowElementClass *object, int row_index);						// 0x58A670 (FAIL_IF '!object'; MainWindow's)  // NAME 1 [Add_Object, object, row_index]: DynamicWindowClass's
	void Remove_Object(DynamicWindowElementClass *object);									// 0x58A6C0 (FAIL_IF '!object'; MainWindow's Remove_Child)  // NAME 1 [Remove_Object, object]: DynamicWindowClass's
	void Add_Object(DynamicWindowRenderObjectClass *object, int row_index);					// 0x58A7E0 (FAIL_IF '!object'; MainWindow's)  // NAME 1 [the overload's type]: DynamicWindowClass's
	void Remove_Object(DynamicWindowRenderObjectClass *object);								// 0x58A830 (FAIL_IF '!object'; MainWindow's Remove_Child)  // NAME 1 [the overload's type]: DynamicWindowClass's
	void Add_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index);	// 0x58A950 (FAIL_IF '!MainWindow'; MainWindow's Add_Element)  // NAME 1 [Add_Element, element, row_index]: DynamicWindowClass's
	void Remove_All(bool recursive);														// 0x58A9A0 (FAIL_IF '!MainWindow'; MainWindow's Remove_All)  // NAME 1 [Remove_All, recursive]: DynamicWindowClass's
	void Remove_Element(const RefCountPtr<DynamicWindowElementClass> &element);				// 0x58A9F0 (FAIL_IF '!MainWindow'; MainWindow's, row -1)  // NAME 1 [Remove_Element, element]: DynamicWindowClass's
	void Unknown_58A580(Vector2 position);													// 0x58A580 (MainWindow's Set_Center; ui_starmap.cpp's 0x7C8490)  // NAME 2 [position]: what the caller passes
	void Get_Center(Vector2 &center);														// 0x58A640 (MainWindow's; ui_slot_info.cpp's 0x630070)  // NAME 2 [Get_Center, center]: DynamicWindowClass's
	void Unknown_58ADA0();																	// 0x58ADA0 (FAIL_IF 'Is_Active()'; the gadgets of Unknown94 destroyed)  // NAME 1: neutral (D10)
	void Update_Layout();																	// 0x58AE70 (MainWindow's)  // NAME 1 [Update_Layout]: DynamicWindowClass's
	static void Add_Text(DynamicWindowClass *window, const char *font_name, int row_index, unsigned int flags, const char *format, ...);	// 0x58AE90 (FAIL_IF '!window'; DynamicWindowTextClass::Create(font_name, text, flags | 6))  // NAME 1 [Add_Text, row_index, flags, format]: what it does; NAME 2 [window, font_name]: the FAIL_IF's, Create's
	void Add_Text(const char *font_name, int row_index, unsigned int flags, const char *format, ...);	// 0x58B160 (FAIL_IF '!MainWindow'; cdecl; ui_starmap.cpp's 'STARMAP_HOW_TO' lines)  // NAME 1 [Add_Text, row_index, flags, format]: what it does; NAME 2 [font_name]: Create's

	RefCountPtr<DynamicWindowClass> MainWindow;												// +0x88 (FAIL_IF '!MainWindow')  // NAME 3: the FAIL_IF's
	DynamicWindowGadgetClass *Unknown8C;													// +0x8C (slot +0x04; the constructor's DynamicWindowGadgetClass over MainWindow, id 0xFFFFFF)  // NAME 1: neutral (D10)
	int Unknown90;																			// +0x90 (0x457; slot +0x10 counts on)  // NAME 1: neutral (D10)
	std::vector<GadgetClass *> Unknown94;													// +0x94 (slot +0x08 adds, slot +0x0C removes; Unknown_58ADA0 destroys them)  // NAME 1: neutral (D10)
	unsigned int UnknownA4;																	// +0xA4 (ms since the pointer left +0x8C: slot +0x60)  // NAME 1: neutral (D10)
	bool UnknownA8;																			// +0xA8 (the constructor: true; slot +0x60 closes on the timer)  // NAME 1: neutral (D10)
	bool UnknownA9;																			// +0xA9 (the constructor: true; slot +0x68 closes on key 0xFFFFFF)  // NAME 1: neutral (D10)
};

#endif
