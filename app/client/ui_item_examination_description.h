// Earth & Beyond client (reconstruction): app\client\ui_item_examination_description.h, the item examination window's
// description panel ('ITEM_EXAMINATION_DESCRIPTION', 'DETAILS00_IEI00'; 0x78 bytes, table 0xAEE490, its container side
// 0xAEE480 at +0x4C; constructor 0x534CA0, destructor 0x534F80, deleting 0x534F50). The client holds no string of this
// file or its .cpp: both names are assumed from the window section, as ui_item_examination_attributes.h's (interim, D137).
#ifndef UI_ITEM_EXAMINATION_DESCRIPTION_H  // NAME 1 [ui_item_examination_description.h]: assumed from 'ITEM_EXAMINATION_DESCRIPTION' (interim, D137)
#define UI_ITEM_EXAMINATION_DESCRIPTION_H

#include "dynamic_window_background.h"
#include "dynamic_window_multi_line_text.h"
#include <string>

class UIItemExaminationDescriptionClass : public DynamicWindowBackgroundClass  // NAME 2: its section 'ITEM_EXAMINATION_DESCRIPTION'
{
public:
	UIItemExaminationDescriptionClass();													// 0x534CA0 (made by the examination window 0x531660)
	virtual ~UIItemExaminationDescriptionClass();											// +0x04 0x534F80 (deleting 0x534F50)

	void Clear();																			// 0x535060 (the text emptied)  // NAME 1 [Clear]: as the options panel's
	void Set_Description(const std::string &text);											// 0x5351D0 (a changed text shown)  // NAME 1 [Set_Description, text]: what it does

protected:
	RefCountPtr<DynamicWindowMultiLineTextClass> Text;										// +0x64 ('DETAILS00_IEI00', 150 x 60)  // NAME 1 [Text]: what it holds
	std::string Description;																// +0x68  // NAME 1 [Description]: what it holds
};

#endif
