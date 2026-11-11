// Earth & Beyond client (reconstruction): app\client\item\client_item_property_builder.h, the declarations of the
// client's client_item_property_builder.cpp (its FAIL_IFs' __FILE__ 'e:\build\beta\G\app\Client\item\client_item_property_builder.cpp';
// this header's name is assumed from it, NAME 1).
#ifndef CLIENT_ITEM_PROPERTY_BUILDER_H  // NAME 1 [client_item_property_builder.h]: header name assumed from the client's client_item_property_builder.cpp
#define CLIENT_ITEM_PROPERTY_BUILDER_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include "client_item_property_special_effect.h"
#include "client_item_property_special_maker.h"
#include "client_item_property_special_profession_requirement.h"
#include "client_item_property_attribute.h"
#include "client_item_property_attribute_distance.h"
#include "client_item_property_attribute_string.h"
#include "client_item_property_attribute_seconds.h"
#include "client_item_property_attribute_percent.h"
#include "client_item_property_attribute_integer.h"
#include "client_item_property_attribute_float.h"
#include "client_item_property_option.h"
#include "client_item_property_icon.h"
#include "client_item_property_name.h"
#include "client_item_property_description.h"
#include "client_item_property_unknown_529810.h"
#include "client_item_property_option_auto_fire.h"
#include "..\..\ww3d2\texture.h"
#include <string>
#include <vector>
#include "..\..\common\item_info.h"	// ItemBaseInfoClass, ItemEffectDescriptionClass, ItemEffectClass
#include "..\inv_data_peek.h"	// InvDataPeekClass (InvDataClass::Unknown_688830)

class SClient;
class UIItemExaminationPropertyRouterBaseClass;

// The callees in files the tree does not have yet (interim here, D137)

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
	const RefCountPtr<ItemBaseInfoClass> &Get_Template_Info() const { return(TemplateInfo); }	// NAME 1: neutral (D10); an inline layer the client spends budget on
	// (0x831840 is client.h's item template's Unknown_831840: callers cast)																		// 0x831840 (its +0x10, with a reference)  // NAME 1: neutral (D10)
	char Unknown00[0x08];
	int Unknown08;																						// +0x08 (0x529810's)  // NAME 1: neutral
	char Unknown0C[0x14 - 0x0C];
	std::string Name;																					// +0x14 (0x52A360's)  // NAME 2: inv_render.cpp's Get_Name
	std::string Description;																			// +0x24 (0x529AE0's)  // NAME 2: what 0x521C10 makes of it
	std::string Unknown34;																				// +0x34 (0x52C770's, flag true)  // NAME 1: neutral
	char Unknown44[0x48 - 0x44];
	int Unknown48;																						// +0x48 (0x52FFB0 / 0x530070's)  // NAME 1: neutral
	char Unknown4C[0x58 - 0x4C];
	ItemFlagsStruct Flags;																				// +0x58  // NAME 2: the specials 0x5285A0 makes of them
	RefCountPtr<ItemBaseInfoClass> TemplateInfo;													// +0x5C  // NAME 2: the FAIL_IFs '!template_info'
};

class MoneyClass;
// inv_data.h's Unknown_688870 argument (libs/sgistl/hash_set.h's hash_set<int>), declared as kv_list.h declares the class
namespace std { template <class _Key> struct hash; template <class _Value, class _HashFcn, class _EqualKey, class _Alloc> class hash_set; }
class InvDataClass : public RefCountClass  // NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004
{
public:
	InvDataPeekClass *Unknown_688830();																	// 0x688830 (+0x54)  // NAME 1: neutral (D10)
	void Unknown_688870(unsigned int mask, const std::hash_set<int, std::hash<int>, std::equal_to<int>, std::allocator<int> > *ids);	// 0x688870 (ui_trade.cpp: a category's bit, 0 for all)  // NAME 1 [mask, ids]: neutral
	char Unknown08[0x50 - 0x08];
	int Unknown50;																						// +0x50 (0x522860 routes 0x52AD00's property when 2)  // NAME 1: neutral
};

// An effect as a special property: ClientItemPropertySpecialEffectClass, client_item_property_special_effect.h
// (two lines for three here and the #include above: the inline FAIL_IFs below keep their lines)

// A flag as a special property (its file is not rebuilt)
class ClientItemPropertySpecialFlagClass : public ClientItemPropertySpecialClass {};  // NAME 1: neutral (D10)
RefCountPtr<ClientItemPropertySpecialFlagClass> Unknown_52F9E0(int sort, const std::string &label, const std::string &tooltip, int unknown, bool flag);	// 0x52F9E0  // NAME 1: neutral (D10); NAME 1 [sort, label, tooltip, unknown, flag]

// The properties 0x521C10 makes (their files are not rebuilt; each Create allocates the property and hands it to Create_NEW)
class ClientItemPropertySpecialUnknown52FFB0Class : public ClientItemPropertySpecialClass {};			// NAME 1: neutral (D10)
class ClientItemPropertySpecialUnknown530070Class : public ClientItemPropertySpecialClass {};			// NAME 1: neutral (D10)
RefCountPtr<ClientItemPropertySpecialUnknown52FFB0Class> Unknown_52FFB0(int unknown, int value, int id);	// 0x52FFB0  // NAME 1: neutral (D10); NAME 1 [unknown, value, id]
RefCountPtr<ClientItemPropertySpecialUnknown530070Class> Unknown_530070(int unknown, int id);			// 0x530070  // NAME 1: neutral (D10); NAME 1 [unknown, id]
// The maker: ClientItemPropertySpecialMakerClass, client_item_property_special_maker.h (one line for two here and the #include)

// The attributes and requirements 0x522860 makes (their files are not rebuilt)
enum { MODE_NONE_BF = 0 };																				// the mode flags' none (G_ASSERT 'mode_flags != MODE_NONE_BF')  // NAME 3: the client's assert
class ClientItemPropertySpecialLevelRequirementClass : public ClientItemPropertySpecialClass {};		// NAME 3: client_item_property_special_level_requirement.cpp's
// ClientItemPropertySpecialProfessionRequirementClass: client_item_property_special_profession_requirement.h (one line for two here and the #include)
class ClientItemPropertySpecialRaceRequirementClass : public ClientItemPropertySpecialClass {};		// NAME 3: client_item_property_special_race_requirement.cpp's
class ClientItemPropertySpecialSkillRequirementClass : public ClientItemPropertySpecialClass {};		// NAME 3: client_item_property_special_skill_requirement.cpp's
RefCountPtr<ClientItemPropertySpecialLevelRequirementClass> Unknown_530530(SClient *space_client, int kind, int level);	// 0x530530 (kind 0 combat, 1 explore, 2 trade, 3 total)  // NAME 1: neutral (D10); NAME 1 [kind, level]
RefCountPtr<ClientItemPropertySpecialRaceRequirementClass> Unknown_52DB60(SClient *space_client, int unknown, int race);	// 0x52DB60 (attributes 18, 17)  // NAME 1: neutral (D10); NAME 1 [race]
RefCountPtr<ClientItemPropertySpecialSkillRequirementClass> Unknown_52E760(SClient *space_client, const std::string &skill, int level, bool flag);	// 0x52E760  // NAME 1: neutral (D10); NAME 1 [skill, flag]
std::string Unknown_4FB5B0(const std::vector<std::string> &strings, const std::string &separator);	// 0x4FB5B0 (joins: the mode names with ", ")  // NAME 1: neutral (D10); NAME 1 [strings, separator]

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
