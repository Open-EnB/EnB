#include "ui_item_examination_attributes.h"
#include "dynamic_window_text.h"
#include "gd_dynamicelement.h"
#include "gadget_collection_interface.h"
#include "..\shared\g_assert.h"
#include <algorithm>

UIItemExaminationAttributesClass::UIItemExaminationAttributesClass(GadgetCollectionInterface *listener) :
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

UIItemExaminationAttributeClass::UIItemExaminationAttributeClass(const RefCountPtr<ClientItemPropertyAttributeClass> &attribute, GadgetCollectionInterface *listener) :
	DynamicWindowHorizontalClass(6),
	Listener(listener),
	Gadget(NULL)
{
	RefCountPtr<DynamicWindowTextClass> label = DynamicWindowTextClass::Create("ATTRIBUTE00_IEI00", attribute->Label, 0x12);
	Add_Element(label);
	static Vector2 value_size(65.0f, 0.0f);
	RefCountPtr<DynamicWindowHorizontalClass> value = Create_NEW(new DynamicWindowHorizontalClass(0));
	value->Set_Min_Size(value_size);
	RefCountPtr<DynamicWindowTextClass> amount = DynamicWindowTextClass::Create("ATTRIBUTE00_VAR_IEI00", attribute->Unknown_14() + " ", 0x10);
	value->Add_Element(amount);
	RefCountPtr<DynamicWindowTextClass> units = DynamicWindowTextClass::Create("ATT00_UNIT_IEI00", attribute->Unknown1C, 0x10);
	value->Add_Element(units);
	Add_Element(value);
	std::string tooltip = attribute->Unknown_0C();
	if (!tooltip.empty()) {
		Gadget = new DynamicElementGadgetClass(this);
		Gadget->Set_Unknown14(tooltip.c_str());
		Gadget->Unknown2C_06 = false;
		Listener->Unknown84_08(Gadget, Listener->Unknown84_10(), -2);
	}
}

UIItemExaminationAttributeClass::~UIItemExaminationAttributeClass()
{
	if (Gadget != NULL) {
		Listener->Unknown84_0C(Gadget);
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	Remove_All_Children(true);
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
**	inlines in its erase). IE3 (run 2026-11-10): the row UIItemExaminationAttributeClass, constructor 0x533450
**	(its value column's minimum size a function-local static, guard 0xBE5A60), destructor 0x533AC0 and deleting
**	0x533A90 exact: label, value and units texts, the tooltip on a DynamicElementGadgetClass (0x6686F0).
*/
