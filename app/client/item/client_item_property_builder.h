// Earth & Beyond client (reconstruction): app\client\item\client_item_property_builder.h, the declarations of the
// client's client_item_property_builder.cpp (its FAIL_IFs' __FILE__ 'e:\build\beta\G\app\Client\item\client_item_property_builder.cpp';
// this header's name is assumed from it, NAME 1).
#ifndef CLIENT_ITEM_PROPERTY_BUILDER_H  // NAME 1 [client_item_property_builder.h]: header name assumed from the client's client_item_property_builder.cpp
#define CLIENT_ITEM_PROPERTY_BUILDER_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class SClient;
class UIItemExaminationPropertyRouterBaseClass;

// The callees in files the tree does not have yet (interim here, D137)

// One effect of an item (0x58 bytes; its copy 0x5293B0 and destructors 0x5295C0 / 0x5296D0 are the implicit ones;
// item_effect_description.cpp's, not rebuilt)
class ItemEffectClass  // NAME 1: neutral (D10)
{
public:
	std::string Unknown_833D00() const;																	// 0x833D00  // NAME 1: neutral (D10)
	std::string Unknown_833D40() const;																	// 0x833D40  // NAME 1: neutral (D10)

	std::string Unknown00;																				// +0x00  // NAME 1: neutral
	std::string Unknown10;																				// +0x10  // NAME 1: neutral
	std::string Unknown20;																				// +0x20  // NAME 1: neutral
	std::vector<float> Unknown30;																		// +0x30  // NAME 1: neutral
	std::vector<float> Unknown40;																		// +0x40  // NAME 1: neutral
	int Unknown50;																						// +0x50  // NAME 1: neutral
	int Unknown54;																						// +0x54  // NAME 1: neutral
};

// An item's effects (0x24 bytes; item_effect_description.cpp's, not rebuilt)
class ItemEffectDescriptionClass  // NAME 2: item_effect_description.cpp's 0x83E5F0
{
public:
	const std::vector<ItemEffectClass> &Get_Effects() const { return(Effects); }	// NAME 1: neutral
	void Unknown_83E5F0(const std::string &delta);														// 0x83E5F0  // NAME 1: neutral (D10); NAME 1 [delta]: what it gets

	std::vector<ItemEffectClass> Effects;																// +0x00  // NAME 2: what it holds
	int Unknown10;																						// +0x10  // NAME 1: neutral
	int Unknown14;																						// +0x14  // NAME 1: neutral
	int Unknown18;																						// +0x18  // NAME 1: neutral
	int Unknown1C;																						// +0x1C  // NAME 1: neutral
	int Unknown20;																						// +0x20  // NAME 1: neutral
};

// What an item's +0x5C points at (item_info.cpp's, not rebuilt)
class ItemTemplateInfoClass : public RefCountClass  // NAME 2: the FAIL_IFs '!template_info'
{
public:
	const ItemEffectDescriptionClass &Get_Unknown40() const { return(Unknown40); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	const ItemEffectDescriptionClass &Get_Unknown68() const { return(Unknown68); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	char Unknown08[0x40 - 0x08];
	ItemEffectDescriptionClass Unknown40;																// +0x40  // NAME 1: neutral
	char Unknown64[0x68 - 0x64];
	ItemEffectDescriptionClass Unknown68;																// +0x68  // NAME 1: neutral
};

// An item's flags (the item's +0x58; the labels and tooltips 0x5285A0 gives them)
struct ItemFlagsStruct  // NAME 1: neutral (D10)
{
	bool Is_NotTradable() const { return(NotTradable); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	bool Is_Temporary() const { return(Temporary); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	bool Is_Unique() const { return(Unique); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	bool Is_NotStorable() const { return(NotStorable); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	bool Is_NotDestroyable() const { return(NotDestroyable); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	bool Is_NotManufacturable() const { return(NotManufacturable); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	unsigned int NotTradable : 1;																		// bit 0 'Not Tradable'  // NAME 2: its label
	unsigned int Temporary : 1;																			// bit 1 'Temporary'  // NAME 2: its label
	unsigned int Unique : 1;																			// bit 2 'Unique'  // NAME 2: its label
	unsigned int NotStorable : 1;																		// bit 3 'Not Storable'  // NAME 2: its label
	unsigned int NotDestroyable : 1;																	// bit 4 'Not Destroyable'  // NAME 2: its label
	unsigned int Unknown5 : 1;																			// NAME 1: neutral
	unsigned int Unknown6 : 1;																			// NAME 1: neutral
	unsigned int NotManufacturable : 1;																	// bit 7 'Not Manufacturable'  // NAME 2: its label
};

// The slot's item (inv_data_peek.cpp's, not rebuilt; inv_render.cpp's name)
class Unknown688830Item  // NAME 1: neutral (D10)
{
public:
	const RefCountPtr<ItemTemplateInfoClass> &Get_Template_Info() const { return(TemplateInfo); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	char Unknown00[0x58];
	ItemFlagsStruct Flags;																				// +0x58  // NAME 2: the specials 0x5285A0 makes of them
	RefCountPtr<ItemTemplateInfoClass> TemplateInfo;													// +0x5C  // NAME 2: the FAIL_IFs '!template_info'
};

// inv_data_peek.cpp's (not rebuilt)
class InvDataPeekClass  // NAME 3: D24 class-from-file for inv_data_peek.cpp
{
public:
	std::string Unknown_68A270(int slot);																// 0x68A270  // NAME 1: neutral (D10); NAME 1 [slot]
	std::string Unknown_68A450(int slot);																// 0x68A450  // NAME 1: neutral (D10)
};

class InvDataClass : public RefCountClass  // NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004
{
public:
	InvDataPeekClass *Unknown_688830();																	// 0x688830 (+0x54)  // NAME 1: neutral (D10)
};

// An effect as a special property (0x34 bytes, table 0xAEDEDC; its file is not rebuilt)
class ClientItemPropertySpecialEffectClass : public ClientItemPropertySpecialClass {};  // NAME 1: neutral (D10)
RefCountPtr<ClientItemPropertySpecialEffectClass> Unknown_52B840(const ItemEffectClass &effect);		// 0x52B840  // NAME 1: neutral (D10); NAME 1 [effect]

// A flag as a special property (its file is not rebuilt)
class ClientItemPropertySpecialFlagClass : public ClientItemPropertySpecialClass {};  // NAME 1: neutral (D10)
RefCountPtr<ClientItemPropertySpecialFlagClass> Unknown_52F9E0(int sort, const std::string &label, const std::string &tooltip, int unknown, bool flag);	// 0x52F9E0  // NAME 1: neutral (D10); NAME 1 [sort, label, tooltip, unknown, flag]

// What hands an item's properties to a router (16 bytes on the caller's stack: the item examination window's slot +0x94
// 0x532640, inv_render.cpp's 0x68C3E0 and 0x68C6B0)
class ClientItemPropertyBuilderClass  // NAME 3: class-from-file (D24) for client_item_property_builder.cpp
{
public:
	ClientItemPropertyBuilderClass(SClient *space_client, Unknown688830Item *item, const RefCountPtr<InvDataClass> &data, int slot);	// 0x521B80  // NAME 1 [space_client, item, data, slot]: neutral
	~ClientItemPropertyBuilderClass();																	// 0x521BB0
	void Build(UIItemExaminationPropertyRouterBaseClass &router);										// 0x521BD0  // NAME 2 [Build]: what it does; NAME 1 [router]: what it gets

private:
	void Unknown_521C10(UIItemExaminationPropertyRouterBaseClass &router);								// the name, icon, description, quality and structure  // NAME 1: neutral (D10)
	void Unknown_522860(UIItemExaminationPropertyRouterBaseClass &router);								// the attributes and requirements  // NAME 1: neutral (D10)
	void Unknown_527F40(UIItemExaminationPropertyRouterBaseClass &router);								// the effects  // NAME 1: neutral (D10)
	void Unknown_5285A0(UIItemExaminationPropertyRouterBaseClass &router);								// the flags  // NAME 1: neutral (D10)
	void Unknown_528460(UIItemExaminationPropertyRouterBaseClass &router, const std::vector<ItemEffectClass> &effects);	// the effects to specials  // NAME 1: neutral (D10); NAME 1 [effects]

	SClient *SpaceClient;																				// +0x00  // NAME 1: neutral
	Unknown688830Item *Item;																			// +0x04  // NAME 1: neutral
	const RefCountPtr<InvDataClass> &Data;																// +0x08 (a reference: the constructor stores its address)  // NAME 1: neutral
	int Slot;																							// +0x0C  // NAME 1: neutral
};

#endif
