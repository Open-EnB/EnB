// Earth & Beyond client (reconstruction): app\client\ui_item_examination_title.h, the item examination window's title
// panel ('TITLE_IEI00'; 0x70 bytes, table 0xAEE818, its container side 0xAEE808 at +0x08; constructor 0x539980,
// destructor 0x539E80, deleting 0x539E50; made by the examination window 0x531660 at its +0xC4). The client's
// ui_item_examination_title.cpp (its __FILE__ in 0x53A2F0's FAIL_IFs '!text') follows the panel's code; this header's
// name is assumed from it (NAME 3).
#ifndef UI_ITEM_EXAMINATION_TITLE_H  // NAME 3 [ui_item_examination_title.h]: header name assumed from the client's ui_item_examination_title.cpp
#define UI_ITEM_EXAMINATION_TITLE_H

#include "dynamic_window_horizontal.h"
#include "dynamic_window_vertical.h"
#include "dynamic_window_text.h"
#include "dynamic_window_icon.h"
#include <string>

// What the title's slots get (the filler 0x537830's slots +0x08 / +0x0C pass them on)
class UIItemExaminationTitleNameClass { public: std::string Unknown_53A140() const; };	// NAME 1: neutral (D10): 0x53A140 (the name's text, by value)
struct UIItemExaminationTitleIconStruct { char Unknown00[8]; std::string Unknown08; };	// NAME 1: neutral (D10): +0x08 the icon's texture name

class UIItemExaminationTitleClass : public DynamicWindowHorizontalClass  // NAME 2: its text 'TITLE_IEI00' and ui_item_examination_title.cpp
{
public:
	UIItemExaminationTitleClass();															// 0x539980
	virtual ~UIItemExaminationTitleClass();													// +0x04 0x539E80 (deleting 0x539E50)

	void Clear();																			// 0x539F70 (the name emptied, the subtitles removed)  // NAME 1 [Clear]: as the options panel's
	void Set_Name(const UIItemExaminationTitleNameClass &name);								// 0x53A0A0  // NAME 1 [Set_Name, name]: what it does
	void Set_Icon(const UIItemExaminationTitleIconStruct &icon);							// 0x53A2C0  // NAME 1 [Set_Icon, icon]: what it does

protected:
	RefCountPtr<DynamicWindowIconClass> Icon;												// +0x64 (the item's icon, 32 x 32)  // NAME 1 [Icon]: what it holds
	RefCountPtr<DynamicWindowTextClass> Text;												// +0x68 ('TITLE_IEI00')  // NAME 1 [Text]: what it holds
	RefCountPtr<DynamicWindowHorizontalClass> Subtitles;									// +0x6C (0x53A2F0's 'SUBTITLE_..._IEI00' texts)  // NAME 1 [Subtitles]: what it holds
};

std::string Unknown_7EA190(int id);														// 0x7EA190 (an icon's texture name by id; the title asks for 100)  // NAME 1: neutral (D10); NAME 1 [id]: neutral

#endif
