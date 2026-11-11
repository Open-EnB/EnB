// Earth & Beyond client (reconstruction): app\client\ui_item_examination_property_router.h, what hands an item's
// properties to the item examination window's panels (the client's ui_item_examination_property_router.cpp, __FILE__
// 0xB78AB0; this header's name is assumed from it, NAME 3). 0x1C bytes, tables 0xAEE6CC / 0xAEE6A0 (+0x04); the window's
// slot +0x94 0x532640 makes one on the stack.
#ifndef UI_ITEM_EXAMINATION_PROPERTY_ROUTER_H  // NAME 3 [ui_item_examination_property_router.h]: header name assumed from the client's ui_item_examination_property_router.cpp
#define UI_ITEM_EXAMINATION_PROPERTY_ROUTER_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include <string>

class ClientItemPropertyOptionClass;
class ClientItemPropertyAttributeClass;
class UIItemExaminationTitleClass;
class UIItemExaminationAttributesClass;
class UIItemExaminationSpecialsClass;
class UIItemExaminationOptionsClass;
class UIItemExaminationDescriptionClass;
class ClientItemPropertyNameClass;
class ClientItemPropertyIconClass;
class ClientItemPropertyTypeClass;
class ClientItemPropertySpecialClass;
class ClientItemPropertyDescriptionClass;
class ClientItemPropertyUnknown529810Class;
class ClientItemPropertyVisitorClass;

// An item's property as the router sees it: slot +0x10 hands it to a visitor (interim, D137)
class ClientItemPropertyClass : public RefCountClass  // NAME 1: neutral (D10): the item property files' common base, interim
{
public:
	ClientItemPropertyClass();															// 0x51F900 (table 0xAEDBF0)
	virtual ~ClientItemPropertyClass();													// 0x51F950 (deleting 0x51F920)
	virtual int Unknown_08() = 0;														// +0x08 (pure in 0xAEDBF0)  // NAME 1: neutral (D10)
	virtual std::string Unknown_0C() = 0;												// +0x0C (pure in 0xAEDBF0; the tooltip's key: 'category tooltip', 'description', 'name tooltip')  // NAME 1: neutral (D10)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10  // NAME 1 [Accept, visitor]: what the router passes
};

// The property visitor (table 0xAEE6DC: its default slots 0x530490-0x530390 are in item\client_item_property_visitor.cpp)
class ClientItemPropertyVisitorClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(ClientItemPropertyOptionClass *property);						// +0x00 (0x530490)  // NAME 1: neutral (D10); NAME 1 [property]: neutral
	virtual void Unknown_04(ClientItemPropertySpecialClass *property);				// +0x04 (0x530470)  // NAME 1: neutral (D10)
	virtual void Unknown_08(ClientItemPropertyAttributeClass *property);					// +0x08 (0x530450)  // NAME 1: neutral (D10)
	virtual void Unknown_0C(ClientItemPropertyDescriptionClass *property);				// +0x0C (0x530430)  // NAME 1: neutral (D10)
	virtual void Unknown_10(ClientItemPropertyTypeClass *property);				// +0x10 (0x530410)  // NAME 1: neutral (D10)
	virtual void Unknown_14(ClientItemPropertyUnknown529810Class *property);				// +0x14 (0x5303F0)  // NAME 1: neutral (D10)
	virtual void Unknown_18(ClientItemPropertyIconClass *property);					// +0x18 (0x5303D0)  // NAME 1: neutral (D10)
	virtual void Unknown_1C(ClientItemPropertyNameClass *property);						// +0x1C (0x5303B0)  // NAME 1: neutral (D10)
	virtual void Unknown_20(void *property);												// +0x20 (0x530390)  // NAME 1: neutral (D10)
};

// The router's own side (+0x00)
class UIItemExaminationPropertyRouterBaseClass  // NAME 1: neutral (D10)
{
public:
	virtual void Clear() = 0;																// +0x00  // NAME 1 [Clear]: what it does
	virtual void Route(const RefCountPtr<ClientItemPropertyClass> &property) = 0;			// +0x04 (FAIL_IF '!property')  // NAME 3 [property]: the FAIL_IF's; NAME 1 [Route]: the file's
};

class UIItemExaminationPropertyRouterClass : public UIItemExaminationPropertyRouterBaseClass, public ClientItemPropertyVisitorClass  // NAME 3: class-from-file (D24) for ui_item_examination_property_router.cpp
{
public:
	UIItemExaminationPropertyRouterClass(UIItemExaminationTitleClass &title, UIItemExaminationAttributesClass &attributes, UIItemExaminationSpecialsClass &specials, UIItemExaminationOptionsClass &options, UIItemExaminationDescriptionClass &description);	// 0x537830  // NAME 1 [title, attributes, specials, options, description]: the panels
	virtual ~UIItemExaminationPropertyRouterClass();										// +0x08 0x5378C0 (deleting 0x537880)
	virtual void Clear();																	// 0x5378E0 (the title and description cleared)
	virtual void Route(const RefCountPtr<ClientItemPropertyClass> &property);				// 0x537910
	virtual void Unknown_00(ClientItemPropertyOptionClass *property);						// 0x537A80 (the options panel)  // NAME 1: neutral (D10)
	virtual void Unknown_04(ClientItemPropertySpecialClass *property);				// 0x537A60 (the specials panel)  // NAME 1: neutral (D10)
	virtual void Unknown_08(ClientItemPropertyAttributeClass *property);					// 0x537A40 (the attributes panel)  // NAME 1: neutral (D10)
	virtual void Unknown_0C(ClientItemPropertyDescriptionClass *property);				// 0x537A10 (the description)  // NAME 1: neutral (D10)
	virtual void Unknown_10(ClientItemPropertyTypeClass *property);				// 0x5379F0 (the title's subtitles)  // NAME 1: neutral (D10)
	virtual void Unknown_14(ClientItemPropertyUnknown529810Class *property);				// 0x5379B0 (nothing)  // NAME 1: neutral (D10)
	virtual void Unknown_18(ClientItemPropertyIconClass *property);					// 0x5379D0 (the title's icon)  // NAME 1: neutral (D10)
	virtual void Unknown_1C(ClientItemPropertyNameClass *property);						// 0x537990 (the title's name)  // NAME 1: neutral (D10)
	virtual void Unknown_20(void *property);												// 0x537960 (G_ASSERT '0')  // NAME 1: neutral (D10)

protected:
	UIItemExaminationTitleClass &Title;														// +0x08  // NAME 1: the panel
	UIItemExaminationAttributesClass &Attributes;											// +0x0C  // NAME 1: the panel
	UIItemExaminationSpecialsClass &Specials;												// +0x10  // NAME 1: the panel
	UIItemExaminationOptionsClass &Options;													// +0x14  // NAME 1: the panel
	UIItemExaminationDescriptionClass &Description;										// +0x18  // NAME 1: the panel
};

#endif
