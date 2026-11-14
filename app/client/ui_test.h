// Earth & Beyond client (reconstruction): app\client\ui_test.h, the UI test window (the [UITEST] section of
// fontcode.ini / fontart.ini). No string names the file: ui_test.cpp and this header are provisional names (NAME 1,
// D137), from its 'UITEST' strings and its object between ui_talkt_space.cpp and ui_trade.cpp.
#ifndef UI_TEST_H  // NAME 1 [ui_test.h]: provisional header name
#define UI_TEST_H

#include "intrface.h"
#include "layer.h"
#include "..\..\wwlib\vector.h"
#include "..\..\wwlib\ini.h"

class UIViewClass;			// sclient.h (uiview.cpp's view)
class Unknown5BB320Class;	// unknown_5bb250.h (the mesh windows' interface base)

// The test window (0xE8 bytes, tables 0xAF8DF4 / 0xAF8DE8 / 0xAF8DDC): on activation it reads the ini named by
// '-UITEST_CODE' (fontcode.ini, else fontart.ini), loads the string files 'load0', 'load1', ... of the section named
// by [UITEST] Section, and shows the meshes 'mesh0', 'mesh1', ... as mesh windows over its layer. Nothing calls
// its constructor (0x64F790, through its incremental thunk only).
class UITestClass : public InterfaceClass	// NAME 1: neutral, after its 'UITEST' strings
{
public:
	UITestClass(UIViewClass *view, const LayerClass &layer);	// 0x64F790 (the view and the layer's scene to 0x540A90, the layer copied)  // NAME 1 [view, layer]: Unknown5BB320Class's
	virtual ~UITestClass();							// +0x00 0x64F990 (the deleting destructor 0x64F920)
	virtual void Unknown_54();						// +0x54 0x64FB10 (nothing)
	virtual void Unknown_58();						// +0x58 0x64FB30 (activates: the ini, the string files, the mesh windows)
	virtual void Unknown_5C();						// +0x5C 0x64FF10 (deactivates: the ini cleared, the windows and string files released)
	virtual void Unknown_60();						// +0x60 0x650040 (every frame: each window's +0x60)
	virtual bool Unknown_68(int key);				// +0x68 0x650080 (false)  // NAME 1 [key]: InterfaceClass's

	DynamicVectorClass<Unknown5BB320Class *> Windows;	// +0x84 (tables 0xAF8E98 / 0xAF8EB4: this object's instances)  // NAME 1: what it holds
	DynamicVectorClass<char *> Files;				// +0x9C (the string files' names, nstrdup'd)  // NAME 1: what it holds
	INIClass Ini;									// +0xB4  // NAME 1: what it is
	LayerClass Layer;								// +0xC4 (0x24 bytes, to +0xE8; the windows are made over it)  // NAME 2: what it is
};

#endif // UI_TEST_H
