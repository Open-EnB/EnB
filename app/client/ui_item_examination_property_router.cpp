#include "..\shared\g_assert.h"
#include "ui_item_examination_property_router.h"
#include "ui_item_examination.h"
#include "ui_item_examination_attributes.h"
#include "ui_item_examination_options.h"

#include "item\client_item_property_description.h"


UIItemExaminationPropertyRouterClass::UIItemExaminationPropertyRouterClass(UIItemExaminationTitleClass &title, UIItemExaminationAttributesClass &attributes, UIItemExaminationSpecialsClass &specials, UIItemExaminationOptionsClass &options, UIItemExaminationDescriptionClass &description) :
	Title(title),
	Attributes(attributes),
	Specials(specials),
	Options(options),
	Description(description)
{
}

UIItemExaminationPropertyRouterClass::~UIItemExaminationPropertyRouterClass()
{
}

void UIItemExaminationPropertyRouterClass::Clear()
{
	Title.Clear();
	Description.Clear();
}


























void UIItemExaminationPropertyRouterClass::Route(const RefCountPtr<ClientItemPropertyClass> &property)
{
	FAIL_IF(!property) return;
	property->Accept(this);
}



void UIItemExaminationPropertyRouterClass::Unknown_20(void *property)
{
	G_ASSERT(0);
}

void UIItemExaminationPropertyRouterClass::Unknown_1C(ClientItemPropertyNameClass *property)
{
	Title.Set_Name(*property);
}

void UIItemExaminationPropertyRouterClass::Unknown_14(ClientItemPropertyUnknown529810Class *property)
{
}

void UIItemExaminationPropertyRouterClass::Unknown_18(ClientItemPropertyIconClass *property)
{
	Title.Set_Icon(*property);
}

void UIItemExaminationPropertyRouterClass::Unknown_10(ClientItemPropertyTypeClass *property)
{
	Title.Set_Subtitles(*property);
}

void UIItemExaminationPropertyRouterClass::Unknown_0C(ClientItemPropertyDescriptionClass *property)
{
	Description.Set_Description(property->Description);
}

void UIItemExaminationPropertyRouterClass::Unknown_08(ClientItemPropertyAttributeClass *property)
{
	Attributes.Add_Attribute(property);
}

void UIItemExaminationPropertyRouterClass::Unknown_04(ClientItemPropertySpecialClass *property)
{
	Specials.Add_Special(property);
}

void UIItemExaminationPropertyRouterClass::Unknown_00(ClientItemPropertyOptionClass *property)
{
	Options.Add_Option(property);
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_item_examination_property_router.cpp (its __FILE__ 0xB78AB0:
**	FAIL_IF '!property' line 56, G_ASSERT '0' line 64).
**
**	IE2 (run 2026-11-05-night): the object 0x537830-0x537ABF, all exact: the constructor 0x537830, the destructor
**	0x5378C0 (deleting 0x537880), Clear 0x5378E0, Route 0x537910 and the visitor slots 0x537960-0x537A80. Its own side
**	has no destructor of its own (the client's deleting destructor inlines only this class's vptr stores); the
**	visitor's default slots 0x530390-0x530490 belong to the item property files. ui_item_examination.cpp's
**	Unknown_94 still declares this class locally as Unknown537830Class.
*/
