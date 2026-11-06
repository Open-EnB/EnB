// Earth & Beyond client (reconstruction): app\client\ui_item_examination.h, the item examination window (the client's
// ui_item_examination.cpp, 'e:\build\beta\G\app\Client\ui_item_examination.cpp' at 0xB786DC; this header's name is
// provisional, NAME 1). Laid out from the constructor 0x531660 and the destructor 0x5321C0.
#ifndef UI_ITEM_EXAMINATION_H  // NAME 1 [ui_item_examination.h]: provisional header name for ui_item_examination.cpp
#define UI_ITEM_EXAMINATION_H

#include "ui_slot_info.h"
#include "dynamic_window_class.h"
#include "dynamic_window_horizontal.h"
#include "dynamic_window_background.h"
#include "control_keys_manager.h"

class UIItemExaminationAttributesClass;
class UIItemExaminationOptionsClass;

// The window's panels from files the tree does not have yet (no strings name them; interim here, D137)
class UIItemExaminationTitleClass : public DynamicWindowHorizontalClass  // NAME 2: its text 'TITLE_IEI00'
{
public:
	UIItemExaminationTitleClass();															// 0x539980 (0x70 bytes, table 0xAEE818)
	void Clear();																			// 0x539F70  // NAME 1 [Clear]: as the options panel's

	int Unknown64;																			// +0x64  // NAME 1: neutral (D10)
	int Unknown68;																			// +0x68  // NAME 1: neutral (D10)
	int Unknown6C;																			// +0x6C  // NAME 1: neutral (D10)
};

class UIItemExaminationDescriptionClass : public DynamicWindowBackgroundClass  // NAME 2: its section 'ITEM_EXAMINATION_DESCRIPTION'
{
public:
	UIItemExaminationDescriptionClass();													// 0x534CA0 (0x78 bytes, table 0xAEE490)
	void Clear();																			// 0x535060  // NAME 1 [Clear]: as the options panel's

	char Unknown64[0x78 - 0x64];															// +0x64  // NAME 1: neutral (D10)
};

class UIItemExaminationSpecialsClass : public DynamicWindowBackgroundClass  // NAME 2: its section 'ITEM_EXAMINATION_SPECIALS'
{
public:
	UIItemExaminationSpecialsClass(UIDynamicWindowUnknown84Class *listener);				// 0x537AD0 (0x7C bytes, table 0xAEE718)  // NAME 1 [listener]: as the options panel's
	void Rebuild();																			// 0x538150  // NAME 1 [Rebuild]: as the options panel's
	void Clear();																			// 0x5382B0  // NAME 1 [Clear]: as the options panel's

	char Unknown64[0x7C - 0x64];															// +0x64  // NAME 1: neutral (D10)
};

// dynamic_window_icon.cpp's factory (0x519A20, FAIL_IF 'texture_name.empty()'): a 0x54-byte element
class DynamicWindowIconClass : public DynamicWindowClass  // NAME 3: class-from-file (D24) for dynamic_window_icon.cpp (base interim: what DynamicWindowGadgetClass takes)
{
public:
	static RefCountPtr<DynamicWindowIconClass> Create(const std::string &texture_name, const Vector2 &size, unsigned int flags);	// 0x519A20  // NAME 2 [texture_name]: the FAIL_IF's; NAME 1 [Create, size, flags]: as DynamicWindowTextClass's
};

GadgetClass *Unknown_51A9C0(UIDynamicWindowClass *window);									// 0x51A9C0 (dynamic_window_move.cpp: the window's move gadget)  // NAME 1 [window]: neutral

// The item examination window (0xDC bytes; tables 0xAEE2C8 / 0xAEE2BC / 0xAEE2B0 / 0xAEE298; ui_slot_info.cpp's
// 0x630280 makes it)
class UIItemExaminationClass : public UISlotInfoClass  // NAME 3: class-from-file (D24) for app\Client\ui_item_examination.cpp
{
public:
	UIItemExaminationClass(LineDisplayClientClass *client, const RefCountPtr<UISlotInfoItemClass> &item);	// 0x531660  // NAME 1 [client, item]: neutral
	virtual ~UIItemExaminationClass();														// 0x5321C0 (deleting 0x532190)
	virtual void Unknown_54();																// 0x532480 (Escape into the key list)
	virtual void Unknown_58();																// 0x5324D0
	virtual void Unknown_5C();																// 0x532500 (the panels cleared)
	virtual void Unknown_60();																// 0x5325C0 (the base's)
	virtual bool Unknown_68(int key);														// 0x5325E0 (key 1 closes)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);									// 0x532610 (the base's)  // NAME 1 [view, arg]: InterfaceClass's
	virtual UISlotInfoItemClass *Unknown_90();												// 0x532460 (Item)
	virtual bool Unknown_94();																// 0x532640 (FAIL_IF 'State != UI_CLOSED', '!client', '!space_client')

	enum { UI_CLOSED = 0 };																	// (the FAIL_IF 'State != UI_CLOSED')  // NAME 3: the FAIL_IF's

	RefCountPtr<UISlotInfoItemClass> Item;													// +0xB0  // NAME 1: neutral (D10)
	ControlKeyListClass Keys;																// +0xB4  // NAME 1: neutral (D10)
	int State;																				// +0xC0 (FAIL_IF 'State != UI_CLOSED')  // NAME 3: the FAIL_IF's
	RefCountPtr<UIItemExaminationTitleClass> Title;											// +0xC4  // NAME 1: neutral (D10)
	RefCountPtr<DynamicWindowIconClass> CloseButton;										// +0xC8 (G_ASSERT 'CloseButton')  // NAME 3: the assert's
	RefCountPtr<UIItemExaminationAttributesClass> Attributes;								// +0xCC  // NAME 1: neutral (D10)
	RefCountPtr<UIItemExaminationSpecialsClass> Specials;									// +0xD0  // NAME 1: neutral (D10)
	RefCountPtr<UIItemExaminationOptionsClass> Options;										// +0xD4  // NAME 1: neutral (D10)
	RefCountPtr<UIItemExaminationDescriptionClass> Description;								// +0xD8  // NAME 1: neutral (D10)
};

#endif
