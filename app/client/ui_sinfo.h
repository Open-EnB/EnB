// Earth & Beyond client (reconstruction): app\client\ui_sinfo.h, the MFD's corner texts (the client's ui_sinfo.cpp,
// 'e:\build\beta\G\app\Client\ui_sinfo.cpp' at 0xB8A2CC; this header's name is provisional, NAME 1). The object is
// 0x62B020-0x62B8CF; nothing in the client calls it (no caller of the constructor 0x62B020 or of Add_Corner 0x62B200).
#ifndef UI_SINFO_H  // NAME 1 [ui_sinfo.h]: provisional header name for ui_sinfo.cpp
#define UI_SINFO_H

#include "..\..\wwlib\vector.h"

class FontSys;
class RenderObjClass;

// One corner's pair of texts, 'MESSAGE_MFD_<tl|tr|br|bl>_TITLE' and '..._VAL' (0xC bytes)
class UISInfoCornerClass  // NAME 2: the corner names 'tl' / 'tr' / 'br' / 'bl' it builds its strings from
{
public:
	UISInfoCornerClass(int corner, RenderObjClass *object);	// 0x62B2E0 (only from Add_Corner)  // NAME 2 [corner, object]: the switch's 0..3, FontSys's model
	~UISInfoCornerClass();										// 0x62B450 (no caller; inline in ~UISInfoClass)

	int Unknown0;												// +0x0 (0; nothing reads it)  // NAME 1: neutral (D10)
	FontSys *Title;												// +0x4 ('MESSAGE_MFD_%s_TITLE'; the constructor stores the '_VAL' text here too)  // NAME 2: its string
	FontSys *Value;												// +0x8 ('MESSAGE_MFD_%s_VAL' by the destructor's pairing; never stored)  // NAME 2: its string
};

// The corners of one model (0x20 bytes)
class UISInfoClass  // NAME 3: class-from-file (D24) for app\Client\ui_sinfo.cpp
{
public:
	UISInfoClass(RenderObjClass *object);						// 0x62B020  // NAME 2 [object]: the model the texts go on
	~UISInfoClass();											// 0x62B0B0
	int Add_Corner(int corner);									// 0x62B200 (returns the new corner's index)  // NAME 2: what it does

	int Unknown0;												// +0x0 (0; nothing reads it)  // NAME 1: neutral (D10)
	RenderObjClass *Model;										// +0x4 (a reference held)  // NAME 2: FontSys's model
	DynamicVectorClass<UISInfoCornerClass *> Corners;			// +0x8 (tables 0xAF7B74, VectorClass's 0xAF7B90)  // NAME 2: what it holds
};

#endif
