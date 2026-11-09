#include "client_item_property_builder.h"
#include "..\ui_item_examination_property_router.h"
#include "..\..\shared\g_assert.h"

ClientItemPropertyBuilderClass::ClientItemPropertyBuilderClass(SClient *space_client, Unknown688830Item *item, const RefCountPtr<InvDataClass> &data, int slot) :
	SpaceClient(space_client),
	Item(item),
	Data(data),
	Slot(slot)
{
}

ClientItemPropertyBuilderClass::~ClientItemPropertyBuilderClass()
{
}

void ClientItemPropertyBuilderClass::Build(UIItemExaminationPropertyRouterBaseClass &router)
{
	Unknown_521C10(router);
	Unknown_522860(router);
	Unknown_527F40(router);
	Unknown_5285A0(router);
}


// NOT EXACT: 0x521C10 (frame layout: the same code, but four strings and three RefCountPtrs sit in other stack slots; and the level requirement reads &Item->TemplateInfo then its Referent where ours folds [eax+0x5C])
void ClientItemPropertyBuilderClass::Unknown_521C10(UIItemExaminationPropertyRouterBaseClass &router)
{
	std::string maker = Data->Unknown_688830()->Unknown_68A8C0(Slot);	// NAME 1 [maker, name, maker2, value, quality, level, structure, tooltip]: neutral
	std::string name = Item->Name;
	std::string maker2 = Item->Unknown34;
	router.Route(Unknown_52A360(name));
	router.Route(Unknown_529810(Item->Unknown08));
	router.Route(Unknown_529FC0(Create_Get(Item->Unknown_831840())));
	int value = 0;
	if (Item->Get_Template_Info()->Unknown_840A20(6, value)) {
		router.Route(Unknown_52FFB0(Item->Unknown48, value, Item->Unknown08));
	} else {
		router.Route(Unknown_530070(Item->Unknown48, Item->Unknown08));
	}
	router.Route(Unknown_529AE0(Item->Description));
	float quality = Data->Unknown_688830()->Unknown_68A010(Slot);
	if (quality != -1.0f) {
		int level;
		if (quality < 1.0f) {
			level = 0;
		} else if (quality > 1.0f) {
			level = 2;
		} else {
			level = 1;
		}
		RefCountPtr<ClientItemPropertyAttributeClass> quality_property = Unknown_520B20(0, "Quality", "Quality of this item compared to others of its kind", 0.0f, 1.0f, quality, level);	// NAME 1 [quality_property, structure_property]: neutral
		router.Route(quality_property);
		float structure = Data->Unknown_688830()->Unknown_689F90(Slot);
		if (structure != -1.0f) {
			std::string tooltip;
			if (structure >= 1.0f) {
				level = 2;
				tooltip = "This item is functioning perfectly.";
			} else if (structure > 0.5f) {
				level = 1;
				tooltip = "This item has taken damage but is still functioning.";
			} else if (structure > 0.0f) {
				level = 0;
				tooltip = "This item has been severely damaged but is still functioning.";
			} else {
				level = 0;
				tooltip = "This item is no longer operational and must be repaired.";
			}
			RefCountPtr<ClientItemPropertyAttributeClass> structure_property = Unknown_520B20(6, "Structure", tooltip, 0.0f, 1.0f, structure, level);
			router.Route(structure_property);
		}
	}
	if (!maker.empty()) {
		router.Route(Unknown_52C770(maker, false));
	} else if (!maker2.empty()) {
		router.Route(Unknown_52C770(maker2, true));
	}
}






























































































































































































































































































































































































































































































































































































































































































































































// NOT EXACT: 0x527F40 (inline budget: the client calls the RefCountPtr copies in the inlined 0x528460 out of line, two of which this inlines; frame 0x78 against 0x80)
void ClientItemPropertyBuilderClass::Unknown_527F40(UIItemExaminationPropertyRouterBaseClass &router)
{
	const RefCountPtr<ItemTemplateInfoClass> &template_info = Item->Get_Template_Info();
	FAIL_IF(!template_info) return;
	ItemEffectDescriptionClass effects = template_info->Get_Unknown40();	// NAME 1 [effects, delta, effects2, delta2]: neutral
	std::string delta = Data->Unknown_688830()->Unknown_68A270(Slot);
	if (!delta.empty()) {
		effects.Unknown_83E5F0(delta);
	}
	Unknown_528460(router, effects.Get_Effects());
	ItemEffectDescriptionClass effects2 = template_info->Get_Unknown68();
	std::string delta2 = Data->Unknown_688830()->Unknown_68A450(Slot);
	if (!delta2.empty()) {
		effects2.Unknown_83E5F0(delta2);
	}
	Unknown_528460(router, effects2.Get_Effects());
}

void ClientItemPropertyBuilderClass::Unknown_528460(UIItemExaminationPropertyRouterBaseClass &router, const std::vector<ItemEffectClass> &effects)
{
	for (std::vector<ItemEffectClass>::const_iterator it = effects.begin(); it != effects.end(); ++it) {	// NAME 1 [it, special]: neutral
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52B840(*it);
		router.Route(special);
	}
}


void ClientItemPropertyBuilderClass::Unknown_5285A0(UIItemExaminationPropertyRouterBaseClass &router)
{
	ItemFlagsStruct flags = Item->Flags;	// NAME 1 [flags, special]: neutral
	if (flags.Is_NotTradable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52F9E0(3, "Not Tradable", "This item cannot be traded or sold.", 0, true);
		router.Route(special);
	}
	if (flags.Is_Temporary()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52F9E0(3, "Temporary", "This item will disappear when player logs out.", 0, true);
		router.Route(special);
	}
	if (flags.Is_Unique()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52F9E0(3, "Unique", "Player cannot have more than one of this item.", 0, true);
		router.Route(special);
	}
	if (flags.Is_NotStorable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52F9E0(3, "Not Storable", "This item cannot be placed in storage.", 0, true);
		router.Route(special);
	}
	if (flags.Is_NotDestroyable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52F9E0(3, "Not Destroyable", "This item cannot be destroyed.", 0, true);
		router.Route(special);
	}
	if (flags.Is_NotManufacturable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = Unknown_52F9E0(3, "Not Manufacturable", "This item cannot be analyzed or manufactured.", 0, true);
		router.Route(special);
	}
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item\client_item_property_builder.cpp (its __FILE__: the FAIL_IFs
**	'!template_info' at lines 155 and 818, 0x522860 / 0x527F40).
**
**	Partial (IP1, run 2026-11-08d): the constructor 0x521B80, destructor 0x521BB0, Build 0x521BD0, the effects' loop
**	0x528460 (a member that never reads this: ret 8, inlined twice into 0x527F40) and the flags' specials 0x5285A0
**	exact (the flag tests are inline bool accessors: the client shifts each bit down and tests bit 0; a plain
**	bitfield test or an explicit shift folds to a mask test); 0x527F40 (inline budget) and 0x521C10 (the name, id,
**	icon, level, description, quality, structure and maker properties; stack slots) NOT EXACT. The callees in files
**	the tree does not have yet are declared in client_item_property_builder.h (interim, D137). Not yet here: 0x522860
**	(17,780 bytes, the asserts at lines 155-796: the item type's attributes and requirements).
*/
