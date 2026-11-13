#include "client_item_property_builder.h"
#include "..\ui_item_examination_property_router.h"
#include "..\..\shared\g_assert.h"
class Unknown831340Class { public: class TextureClass *Unknown_831840(); };	// client.h's item template (item_template.cpp's 0x831840)
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
	std::string maker = Data->Unknown_688830()->Get_Builder_Name(Slot);	// NAME 1 [maker, name, maker2, value, quality, level, structure, tooltip]: neutral
	std::string name = Item->Name;
	std::string maker2 = Item->Unknown34;
	router.Route(ClientItemPropertyNameClass::Create(name));
	router.Route(ClientItemPropertyUnknown529810Class::Create(Item->Unknown08));
	router.Route(ClientItemPropertyIconClass::Create(Create_Get(((Unknown831340Class *)Item)->Unknown_831840())));
	int value = 0;
	if (Item->Get_Template_Info()->Get_Int(ITEM_INFO_TYPE, &value)) {
		router.Route(ClientItemPropertyTypeClass::Create(Item->Unknown48, value, Item->Unknown08));
	} else {
		router.Route(ClientItemPropertyTypeClass::Create(Item->Unknown48, Item->Unknown08));
	}
	router.Route(ClientItemPropertyDescriptionClass::Create(Item->Description));
	float quality = Data->Unknown_688830()->Get_Quality(Slot);
	if (quality != -1.0f) {
		int level;
		if (quality < 1.0f) {
			level = 0;
		} else if (quality > 1.0f) {
			level = 2;
		} else {
			level = 1;
		}
		RefCountPtr<ClientItemPropertyAttributeClass> quality_property = ClientItemPropertyAttributePercentClass::Create(0, "Quality", "Quality of this item compared to others of its kind", 0.0f, 1.0f, quality, level);	// NAME 1 [quality_property, structure_property]: neutral
		router.Route(quality_property);
		float structure = Data->Unknown_688830()->Get_Structure(Slot);
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
			RefCountPtr<ClientItemPropertyAttributeClass> structure_property = ClientItemPropertyAttributePercentClass::Create(6, "Structure", tooltip, 0.0f, 1.0f, structure, level);
			router.Route(structure_property);
		}
	}
	if (!maker.empty()) {
		router.Route(ClientItemPropertySpecialMakerClass::Create(maker, ClientItemPropertySpecialMakerClass::MAKER_PLAYER));
	} else if (!maker2.empty()) {
		router.Route(ClientItemPropertySpecialMakerClass::Create(maker2, ClientItemPropertySpecialMakerClass::MAKER_CORPORATION));
	}
}


// NOT EXACT: 0x522860 (functional, 0.61: the same calls, strings and asserts in order; the frame is 0xC smaller, VC6 inlines string assigns in other cases, and case 35 releases its property through the RefCountPtr the flags use, 0x529130)
void ClientItemPropertyBuilderClass::Unknown_522860(UIItemExaminationPropertyRouterBaseClass &router)
{
	Data->Unknown_688830()->Get_Quality(Slot);
	const RefCountPtr<ItemInfoClass> &template_info = Item->Get_Template_Info();	// NAME 1 [info, count, i, ok, base, actual, level and the cases' locals]: neutral




































































	FAIL_IF(!template_info) return;
	RefCountPtr<ItemInfoClass> info = Create_NEW(new ItemInfoClass);
	info->Apply_Delta(Data->Unknown_688830()->Get_Instance_Info(Slot));
	unsigned int count = template_info->Get_Field_Count();
	for (unsigned int i = 0; i < count; i++) {
		int id = template_info->Get_Field_Info(i);
		if (id == -1) {
			continue;
		}
		switch (id) {
		case ITEM_INFO_00:


			G_ASSERT(0);
			break;
		case ITEM_INFO_AMMO: {
			std::string ammo;
			bool ok = template_info->Get_String(ammo, i);


			G_ASSERT(ok);
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeStringClass::Create(10, "Ammo", "", "Ammunition required for this weapon", 0, ammo);
			router.Route(property);
			break;
		}
		case ITEM_INFO_AUTO_FIRE:
			if (Data->Unknown50 == 2) {
				RefCountPtr<ClientItemPropertyOptionClass> property = ClientItemPropertyOptionAutoFireClass::Create(SpaceClient, Data, Slot);
				router.Route(property);
			}
			break;
		case ITEM_INFO_PROFESSION: {
			int profession = 0;
			template_info->Get_Int(ITEM_INFO_PROFESSION, &profession);
			if (profession != 0) {
				RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialProfessionRequirementClass::Create(SpaceClient, profession);
				router.Route(property);
			}
			break;
		}
		case ITEM_INFO_COMBAT_LEVEL: {
			int combat_level_required = 0;
			template_info->Get_Int(ITEM_INFO_COMBAT_LEVEL, &combat_level_required);












			FAIL_IF(combat_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 0, combat_level_required);
			router.Route(property);
			break;
		}
		case ITEM_INFO_DAMAGE: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_DAMAGE, &base);


			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_DAMAGE)) {
				ok = info->Get_Int(ITEM_INFO_DAMAGE, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeIntegerClass::Create(5, "Damage", "units", "Base damage inflicted by weapon", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_EFFECT_RANGE: {
			int base;
			template_info->Get_Int(ITEM_INFO_EFFECT_RANGE, &base);
			int level = 1;
			int actual = base;
			if (info->Has_Field(ITEM_INFO_EFFECT_RANGE)) {
				info->Get_Int(ITEM_INFO_EFFECT_RANGE, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeDistanceClass::Create(10, "Effect Range", "Range of effect.", 1, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_EFFECT_RADIUS: {
			int base;
			template_info->Get_Int(ITEM_INFO_EFFECT_RADIUS, &base);
			int level = 1;
			int actual = base;
			if (info->Has_Field(ITEM_INFO_EFFECT_RADIUS)) {
				info->Get_Int(ITEM_INFO_EFFECT_RADIUS, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeDistanceClass::Create(10, "Effect Radius", "Radius of effect.", 1, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_ENERGY_USE: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_ENERGY_USE, &base);














			G_ASSERT(ok);
			int level = 1;
			float actual = base;
			if (info->Has_Field(ITEM_INFO_ENERGY_USE)) {
				ok = info->Get_Float(ITEM_INFO_ENERGY_USE, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Energy Use", "units", "Energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_ENERGY_DRAIN: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_ENERGY_DRAIN, &base);



			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_ENERGY_DRAIN)) {
				ok = info->Get_Float(ITEM_INFO_ENERGY_DRAIN, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Energy Drain", "units/sec", "Energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_EQUIP_SKILL: {
			std::string skill;
			int unknown = Item->Unknown48;
			template_info->Get_String(ITEM_INFO_EQUIP_SKILL, skill);
			RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialSkillRequirementClass::Create(SpaceClient, skill, unknown, ClientItemPropertySpecialSkillRequirementClass::REQUIRED_TO_EQUIP);
			router.Route(property);
			break;
		}
		case ITEM_INFO_EXPLORE_LEVEL: {
			int explore_level_required = 0;
			template_info->Get_Int(ITEM_INFO_EXPLORE_LEVEL, &explore_level_required);







			FAIL_IF(explore_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 1, explore_level_required);
			router.Route(property);
			break;
		}
		case ITEM_INFO_OVERALL_LEVEL: {
			int total_level_required = 0;
			template_info->Get_Int(ITEM_INFO_OVERALL_LEVEL, &total_level_required);








			FAIL_IF(total_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 3, total_level_required);
			router.Route(property);
			break;
		}
		case ITEM_INFO_MANEUVERABILITY: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_MANEUVERABILITY, &base);


			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_MANEUVERABILITY)) {
				ok = info->Get_Int(ITEM_INFO_MANEUVERABILITY, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Maneuverability", "units", "Ability to defeat anti-missile systems", 0, (float)base, (float)actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_MAX_POWER: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_MAX_POWER, &base);



			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_MAX_POWER)) {
				ok = info->Get_Int(ITEM_INFO_MAX_POWER, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Max Power", "units", "Maximum amount of power this reactor can hold", 0, (float)base, (float)actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_RACES: {
			int unknown = 0;
			int race = 0;
			template_info->Get_Int(ITEM_INFO_RACES, &unknown);
			template_info->Get_Int(ITEM_INFO_RACE, &race);
			if (race != 0 || unknown != 0) {
				RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialRaceRequirementClass::Create(SpaceClient, unknown, race);
				router.Route(property);
			}
			break;
		}
		case ITEM_INFO_RANGE: {
			int base;
			template_info->Get_Int(ITEM_INFO_RANGE, &base);
			int level = 1;
			int actual = base;
			if (info->Has_Field(ITEM_INFO_RANGE)) {
				info->Get_Int(ITEM_INFO_RANGE, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeDistanceClass::Create(6, "Range", "The maximum targeting range of the item.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_RECHARGE: {
			float base;
			template_info->Get_Float(ITEM_INFO_RECHARGE, &base);
			int level = 1;
			float actual = base;
			if (info->Has_Field(ITEM_INFO_RECHARGE)) {
				info->Get_Float(ITEM_INFO_RECHARGE, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Recharge", "units/sec", "The amount of energy regenerated per second.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_RELOAD_TIME: {
			float base;
			template_info->Get_Float(ITEM_INFO_RELOAD_TIME, &base);
			float actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_RELOAD_TIME)) {
				info->Get_Float(ITEM_INFO_RELOAD_TIME, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeSecondsClass::Create(6, "Reload Time", "The number of seconds that must elapse between activations.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_ROUNDS_FIRED: {
			int base;
			template_info->Get_Int(ITEM_INFO_ROUNDS_FIRED, &base);
			int level = 1;
			int actual = base;
			if (info->Has_Field(ITEM_INFO_ROUNDS_FIRED)) {
				info->Get_Int(ITEM_INFO_ROUNDS_FIRED, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Rounds Fired", "rounds", "The number of rounds fired per burst.", 1, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_SHIELD_USE: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_SHIELD_USE, &base);



























			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_SHIELD_USE)) {
				ok = info->Get_Float(ITEM_INFO_SHIELD_USE, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Shield Use", "units", "Shield energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_CAPACITY: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_CAPACITY, &base);



			G_ASSERT(ok);
			int level = 1;
			int actual = base;
			if (info->Has_Field(ITEM_INFO_CAPACITY)) {
				ok = info->Get_Int(ITEM_INFO_CAPACITY, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Capacity", "units", "Maximum amount of damage this shield can absorb when fully charged.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_SHIELD_DRAIN: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_SHIELD_DRAIN, &base);



			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_SHIELD_DRAIN)) {
				ok = info->Get_Float(ITEM_INFO_SHIELD_DRAIN, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Shield Drain", "units/sec", "Shield energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_SHIELD_RECHARGE: {
			float base;
			template_info->Get_Float(ITEM_INFO_SHIELD_RECHARGE, &base);
			int level = 1;
			float actual = base;
			if (info->Has_Field(ITEM_INFO_SHIELD_RECHARGE)) {
				info->Get_Float(ITEM_INFO_SHIELD_RECHARGE, &actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Recharge", "units/sec", "The amount of shield energy regenerated per second.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_ACTIVATE_SKILL_LEVEL: {
			int unknown = 1;
			std::string skill;
			template_info->Get_String(ITEM_INFO_ACTIVATE_SKILL, skill);
			template_info->Get_Int(ITEM_INFO_ACTIVATE_SKILL_LEVEL, &unknown);
			RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialSkillRequirementClass::Create(SpaceClient, skill, unknown, ClientItemPropertySpecialSkillRequirementClass::REQUIRED_TO_ACTIVATE);
			router.Route(property);
			break;
		}
		case ITEM_INFO_SIGNATURE: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_SIGNATURE, &base);















			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_SIGNATURE)) {
				ok = info->Get_Int(ITEM_INFO_SIGNATURE, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 2;
			} else if (base < actual) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeDistanceClass::Create(6, "Signature", "Specifies an increased distance that an NPC will notice the player when engine is equipped.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_THRUST: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_THRUST, &base);







			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_THRUST)) {
				ok = info->Get_Int(ITEM_INFO_THRUST, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Thrust", "units", "Higher thrust values will increase the ship's maximum speed.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_TRADE_LEVEL: {
			int trade_level_required;
			template_info->Get_Int(ITEM_INFO_TRADE_LEVEL, &trade_level_required);





			FAIL_IF(trade_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 2, trade_level_required);
			router.Route(property);
			break;
		}
		case ITEM_INFO_WARP: {
			int base = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_WARP, &base);


			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_WARP)) {
				ok = info->Get_Int(ITEM_INFO_WARP, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Warp", "", "Shows how fast the ship will travel while warping with this engine equipped.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_WARP_DRAIN: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_WARP_DRAIN, &base);



			G_ASSERT(ok);
			int level = 1;
			float actual = base;
			if (info->Has_Field(ITEM_INFO_WARP_DRAIN)) {
				ok = info->Get_Float(ITEM_INFO_WARP_DRAIN, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 2;
			} else if (actual < base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Warp Drain", "units/sec", "How fast engine will drain reactor when in free warp (warping with no target.)", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_MODES: {
			int mode_flags = 0;
			bool ok = template_info->Get_Int(ITEM_INFO_MODES, &mode_flags);



			G_ASSERT(ok);
			G_ASSERT(mode_flags != MODE_NONE_BF);
			if (mode_flags != MODE_NONE_BF) {
				std::vector<std::string> modes;
				if (mode_flags & 1) modes.push_back("Manufacturing");
				if (mode_flags & 2) modes.push_back("Analyze");
				if (mode_flags & 4) modes.push_back("Dismantle");
				if (mode_flags & 8) modes.push_back("Refining");
				std::string text = String_Join(modes, ", ");
				RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeStringClass::Create(6, "Mode", "", "Which activities this Override item benefits", 0, text);
				router.Route(property);
			}
			break;
		}
		case ITEM_INFO_BUILD_VALUE: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_BUILD_VALUE, &base);














			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_BUILD_VALUE)) {
				ok = info->Get_Float(ITEM_INFO_BUILD_VALUE, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 2;
			} else if (actual < base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributeFloatClass::Create(7, "Build Value", "levels", "Improvement in Build/Refine ability when used.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_CRITICAL: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_CRITICAL, &base);



			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Has_Field(ITEM_INFO_CRITICAL)) {
				ok = info->Get_Float(ITEM_INFO_CRITICAL, &actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 2;
			} else if (actual < base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = ClientItemPropertyAttributePercentClass::Create(8, "Critical", "Improvement in chances for a Critical Success when used.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		}
	}
}


// NOT EXACT: 0x527F40 (0.78: the same code; the client pushes esi at entry, ours after the FAIL_IF, and its frame is 8 bytes bigger)
void ClientItemPropertyBuilderClass::Unknown_527F40(UIItemExaminationPropertyRouterBaseClass &router)
{
	const RefCountPtr<ItemInfoClass> &template_info = Item->Get_Template_Info();


	FAIL_IF(!template_info) return;
	ItemEffectDescriptionClass effects = template_info->Get_Activated_Effects();	// NAME 1 [effects, delta, effects2, delta2]: neutral
	std::string delta = Data->Unknown_688830()->Get_Instance_Activated_Effect_Info(Slot);
	if (!delta.empty()) {
		effects.Set_Summary(delta);
	}
	Unknown_528460(router, effects.Get_Effects());
	ItemEffectDescriptionClass effects2 = template_info->Get_Equip_Effects();
	std::string delta2 = Data->Unknown_688830()->Get_Instance_Equip_Effect_Info(Slot);
	if (!delta2.empty()) {
		effects2.Set_Summary(delta2);
	}
	Unknown_528460(router, effects2.Get_Effects());
}

void ClientItemPropertyBuilderClass::Unknown_528460(UIItemExaminationPropertyRouterBaseClass &router, const std::vector<ItemEffectClass> &effects)
{
	unsigned int count = effects.size();	// unused: an inline layer the client spends budget on (0x527F40 calls the RefCountPtr copies out of line)  // NAME 1 [count]: neutral
	for (std::vector<ItemEffectClass>::const_iterator it = effects.begin(); it != effects.end(); ++it) {	// NAME 1 [it, special]: neutral
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialEffectClass::Create(*it);
		router.Route(special);
	}
}


void ClientItemPropertyBuilderClass::Unknown_5285A0(UIItemExaminationPropertyRouterBaseClass &router)
{
	ItemFlagsStruct flags = Item->Flags;	// NAME 1 [flags, special]: neutral
	if (flags.Is_NotTradable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Tradable", "This item cannot be traded or sold.", 0, true);
		router.Route(special);
	}
	if (flags.Is_Temporary()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Temporary", "This item will disappear when player logs out.", 0, true);
		router.Route(special);
	}
	if (flags.Is_Unique()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Unique", "Player cannot have more than one of this item.", 0, true);
		router.Route(special);
	}
	if (flags.Is_NotStorable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Storable", "This item cannot be placed in storage.", 0, true);
		router.Route(special);
	}
	if (flags.Is_NotDestroyable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Destroyable", "This item cannot be destroyed.", 0, true);
		router.Route(special);
	}
	if (flags.Is_NotManufacturable()) {
		RefCountPtr<ClientItemPropertySpecialClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Manufacturable", "This item cannot be analyzed or manufactured.", 0, true);
		router.Route(special);
	}
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item\client_item_property_builder.cpp (its __FILE__: the FAIL_IFs
**	'!template_info' at lines 155 and 818, 0x522860 / 0x527F40).
**
**	IP1, run 2026-11-08d: the constructor 0x521B80, destructor 0x521BB0, Build 0x521BD0, the effects' loop 0x528460
**	(a member that never reads this: ret 8, inlined twice into 0x527F40) and the flags' specials 0x5285A0 exact (the
**	flag tests are inline bool accessors: the client shifts each bit down and tests bit 0; a plain bitfield test or an
**	explicit shift folds to a mask test); 0x521C10 (the name, id, icon, level, description, quality, structure and
**	maker properties; stack slots), 0x522860 (the template's attributes by id, each compared with the slot's delta
**	copy: level 0 worse, 1 unchanged, 2 better; the requirements; the asserts on the client's lines 155-796) and
**	0x527F40 (inline budget) NOT EXACT. The callees in files the tree does not have yet are declared in
**	client_item_property_builder.h (interim, D137).
*/
