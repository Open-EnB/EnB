#include "ui_item_examination_attributes.h"
#include "..\shared\unknown_debug.h"
#include <algorithm>

void UIItemExaminationAttributesClass::Add_Attribute(ClientItemPropertyAttributeClass *attribute)
{
	RefCountPtr<ClientItemPropertyAttributeClass> ptr = Create_Peek(attribute);
	Attributes.push_back(ptr);
}

void UIItemExaminationAttributesClass::Rebuild()
{
	std::sort(Attributes.begin(), Attributes.end());
	for (std::vector< RefCountPtr<ClientItemPropertyAttributeClass> >::iterator it = Attributes.begin(); it != Attributes.end(); ++it) {
		RefCountPtr<UIItemExaminationAttributeClass> row = Create_NEW(new UIItemExaminationAttributeClass(*it, Listener));
		List->Add_Element(row);
	}
}

void UIItemExaminationAttributesClass::Clear()
{
	List->Remove_All_Children(true);
	Attributes.clear();
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: unknown, assumed e:\build\beta\G\app\Client\ui_item_examination_attributes.cpp (interim,
**	D137): the client holds no __FILE__ of it; its code runs from the panel's constructor 0x532BA0 (after
**	ui_item_examination.cpp's adjustor thunks 0x532B70-0x532B90) to the row's and the sort's copies before
**	the description panel's constructor 0x534CA0 ('ITEM_EXAMINATION_DESCRIPTION').
**
**	Partial (IE1, run 2026-11-05-night): Add_Attribute 0x5331B0, Rebuild 0x533250 and Clear 0x5333B0 exact, with
**	the copies they bring. Not yet here: the constructor 0x532BA0 (same length, the stack frame 4 bytes larger:
**	the temporaries' slots) and the destructor 0x533000 (the client calls RefCountPtr's assignment 0x534060 and
**	destructor 0x533C30 out of line in its erase, then sets List to null): defining either emits the table, and
**	the deleting destructor then pairs with the row's 0x533A90 until the destructor is exact.
*/
