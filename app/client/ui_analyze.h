// Earth & Beyond client (reconstruction): app\client\ui_analyze.h, the analysis window (the client's ui_analyze.cpp
// per its asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x5435D0 (AN1, 2026-11-06).
#ifndef UI_ANALYZE_H  // NAME 1 [ui_analyze.h]: provisional header name for ui_analyze.cpp
#define UI_ANALYZE_H

#include "sclient.h"
#include "layer.h"
#include "ui_inv.h"		// UIInvRobjStripClass, the strips of slots
#include "..\..\wwlib\ref_ptr.h"

class SBaseViewClass;		// sbaseview.cpp's starbase view (+0xD0)
class ButtonGadgetClass;	// unknown_675120.h's (+0xE0)
class DeltaCompoundClass;	// the item's aux data (+0x10C)

enum
{
	UI_CLOSED = 0,										// (the destructor's G_ASSERT 'State == UI_CLOSED'; slot +0x5C sets it)  // NAME 3: the assert's
	UI_OPEN = 2											// (slot +0x58 sets it)  // NAME 1: neutral (D10)
};

// ui_analyze.cpp's analysis window (0x11C bytes, tables 0xAEF120 / 0xAEF114 / 0xAEF108; constructor 0x5435D0, made by
// sbaseview.cpp's 0x69F8F0 case 6; laid out from the constructor; as far as this file's rebuilt functions use it)
class UIAnalyzeClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_analyze.cpp
{
public:
	UIAnalyzeClass(SBaseViewClass *view, const LayerClass &layer);	// 0x5435D0 (0x11C bytes: sbaseview.cpp's 0x69F8F0 case 6)  // NAME 1 [view, layer]: as ui_manu.cpp's
	virtual ~UIAnalyzeClass();							// 0x5437C0 (G_ASSERT 'State == UI_CLOSED', l.116; deleting 0x543790)
	virtual void Unknown_54();							// 0x5439F0 (the buttons, the three sub-windows, the icons)
	virtual void Unknown_58();							// 0x544090
	virtual void Unknown_5C();							// 0x544270
	virtual void Unknown_60();							// 0x5443F0 (the result shown when the attempt ends)
	virtual bool Unknown_68(int key);					// 0x544BB0  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x544020 (InterfaceClass's, then the three sub-windows')  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_544D80();								// 0x544D80: the cost and probability texts
	void Unknown_545190();								// 0x545190: the cost and probability texts as they change, the 'Validity' message
	void Unknown_545610();								// 0x545610 ('Validity', 'Mode')
	void Unknown_5458B0();								// 0x5458B0: the analysis started (key 5)
	void Unknown_544C50(int index, bool show, const char *text);	// 0x544C50 (FAIL_IF '!font', l.483): a text set and shown (as ui_manu.cpp's 0x5B1AF0)  // NAME 1 [index, show, text]: neutral
	void Unknown_545D30();								// 0x545D30: MANUFACTURE_ACTION 126 of 0 sent, the category columns hidden
	void Unknown_545E40();								// 0x545E40: the category icons set to the item's 'CurrentItemCategory'
	void Unknown_546000();								// 0x546000: the category columns and their icons hidden
	void Unknown_546090();								// 0x546090: the item's 'FailureMessage' shown
	void Unknown_5461C0(int level, int a, int b, int c);	// 0x5461C0: the icon of a category column (as ui_manu.cpp's 0x5B42F0)  // NAME 1 [level, a, b, c]: neutral

	LayerClass Unknown0AC;								// +0x0AC (the constructor's layer)
	SBaseViewClass *Unknown0D0;							// +0x0D0 (the constructor's view: sbaseview.cpp's starbase view)
	RefCountPtr<UIInvRobjStripClass> Unknown0D4;			// +0x0D4 'TSM02A.E%02d'
	RefCountPtr<UIInvRobjStripClass> Unknown0D8;			// +0x0D8 'TSM02A.RESULT'
	RefCountPtr<UIInvRobjStripClass> Unknown0DC;			// +0x0DC 'TSM02A.OVER'
	ButtonGadgetClass *Unknown0E0[7];					// +0x0E0 (+0xE8..+0xF0: the category columns)
	RefCountPtr<RenderObjClass> Unknown0FC[3];			// +0x0FC the category columns' icons
	int State;											// +0x108 (UI_CLOSED when shut)  // NAME 3: the destructor's G_ASSERT 'State == UI_CLOSED'
	DeltaCompoundClass *Unknown10C;						// +0x10C: the item's aux data
	bool Unknown110;									// +0x110
	float Unknown114;									// +0x114 (0x546090: 2.0f)
	bool Unknown118;									// +0x118
};

#endif // UI_ANALYZE_H
