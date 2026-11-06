// Earth & Beyond client (reconstruction): app\client\ui_slot_info.h, the slot information screens' base (the client's
// ui_slot_info.cpp, 'e:\build\beta\G\app\Client\ui_slot_info.cpp' at 0xB8A6DC; this header's name is provisional,
// NAME 1). Declarations only, laid out from the constructor 0x62FDF0 (its code is not in the tree yet).
#ifndef UI_SLOT_INFO_H  // NAME 1 [ui_slot_info.h]: provisional header name for ui_slot_info.cpp
#define UI_SLOT_INFO_H

#include "ui_dynamicwindow.h"

// What the screens show (0x630280 gets it from its argument's slot +0x10): reference counted at +0x18
struct UISlotInfoItemBaseStruct { virtual void Unknown_00(); char Unknown04[0x18 - 4]; };								// NAME 1: neutral (D10)
class UISlotInfoItemUnknown54Class;
class UISlotInfoItemClass : public UISlotInfoItemBaseStruct, public RefCountClass			// NAME 1: neutral (D10)
{
public:
	UISlotInfoItemUnknown54Class *Unknown_688830();										// 0x688830 (+0x54)  // NAME 1: neutral (D10)
};

// ui_slot_info.cpp's screen (0xB0 bytes; tables 0xAF814C / 0xAF8140 / 0xAF8134 / 0xAF811C; 0x630280 makes the
// item examination window or 0x62B930's)
class UISlotInfoClass : public UIDynamicWindowClass  // NAME 3: class-from-file (D24) for app\Client\ui_slot_info.cpp
{
public:
	UISlotInfoClass(LineDisplayClientClass *client, const char *name);						// 0x62FDF0 (flags 8)  // NAME 1 [client, name]: the base's
	virtual ~UISlotInfoClass();																// 0x62FEC0 (slot 0x62FE90)
	virtual void Unknown_58();																// 0x62FFB0 (G_ASSERT '_ActiveInterface != this')  // NAME 1: neutral (D10)
	virtual void Unknown_5C();																// 0x630020 (G_ASSERT 'this == _ActiveInterface')  // NAME 1: neutral (D10)
	virtual int Unknown_88() { return(0); }													// +0x88 0x532150 (inline: emitted in ui_item_examination.cpp)  // NAME 1: neutral (D10)
	virtual int Unknown_8C() { return(0); }													// +0x8C 0x532170 (inline: emitted in ui_item_examination.cpp)  // NAME 1: neutral (D10)
	virtual UISlotInfoItemClass *Unknown_90() = 0;											// +0x90  // NAME 1: neutral (D10)
	virtual bool Unknown_94() = 0;															// +0x94  // NAME 1: neutral (D10)

	int UnknownAC;																			// +0xAC (0x68AAA0's slot)  // NAME 1: neutral (D10)
};

#endif
