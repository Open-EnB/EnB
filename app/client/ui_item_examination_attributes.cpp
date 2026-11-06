#include "ui_item_examination_attributes.h"
#include "dynamic_window_text.h"
#include "..\shared\unknown_debug.h"
#include <algorithm>

UIItemExaminationAttributesClass::UIItemExaminationAttributesClass(UIItemExaminationOptionListenerClass *listener) :
	DynamicWindowBackgroundClass("ITEM_EXAMINATION_ATTRIBUTES", 6),
	Listener(listener)
{
	RefCountPtr<DynamicWindowVerticalClass> contents = Create_NEW(new DynamicWindowVerticalClass(6));
	Vector2 min_size(120.0f, 40.0f);
	contents->Set_Min_Size(min_size);
	RefCountPtr<DynamicWindowTextClass> title = DynamicWindowTextClass::Create("ATTRIBUTES_IEI00", "Attributes", 6);
	contents->Add_Element(title);
	List = Create_NEW(new DynamicWindowVerticalClass(6));
	contents->Add_Element(List);
	Set_Contents(contents);
}

UIItemExaminationAttributesClass::~UIItemExaminationAttributesClass()
{
	List->Remove_All_Children(true);
	Set_Contents(NULL);
	Attributes.clear();
	List = NULL;
}

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
**	Partial (IE1, run 2026-11-05-night): the constructor 0x532BA0, the destructor 0x533000 (deleting 0x532FD0),
**	Add_Attribute 0x5331B0, Rebuild 0x533250 and Clear 0x5333B0 exact, with the copies they bring. The constructor
**	needs the minimum size as a named Vector2 (a temporary gives a frame 4 bytes larger); the destructor needs
**	Set_Contents(NULL), clear() and List = NULL (erase(begin, end) or RefCountPtr<>() temporaries change what VC6
**	inlines in its erase). Not yet here: the row class UIItemExaminationAttributeClass (constructor 0x533450,
**	destructor 0x533AC0, deleting 0x533A90).
*/
