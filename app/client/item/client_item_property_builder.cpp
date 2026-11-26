#include "..\aux_item.h"	// the item family's constructors (0x529580 numbers them)
#include "client_item_property_builder.h"	// Unknown831340Class (0x831840): client.h's item template, through inv_data.h
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

static inline int Unknown_Item_Unknown08(const Unknown688830Item *item) { return(item->Unknown08); }	// NAME 1 [Unknown_Item_Unknown08, item]: neutral (D10); inline accessor layer for 0x521C10
// 0x521C10
void ClientItemPropertyBuilderClass::Unknown_521C10(UIItemExaminationPropertyRouterBaseClass &router)
{
	std::string maker = Data->Unknown_688830()->Get_Builder_Name(Slot);	// NAME 1 [maker, name, maker2, value, quality, level, structure, tooltip]: neutral
	std::string name = Item->Name;
	std::string maker2 = Item->Unknown34;
	router.Route(ClientItemPropertyNameClass::Create(name));
	router.Route(ClientItemPropertyUnknown529810Class::Create(Unknown_Item_Unknown08(Item)));
	router.Route(ClientItemPropertyIconClass::Create(Create_Get(((Unknown831340Class *)Item)->Unknown_831840())));
	int value = 0; const RefCountPtr<ItemInfoClass> &template_info = Item->Get_Template_Info();
	if (template_info->Get_Int(ITEM_INFO_TYPE, &value)) {
		int id = Item->Unknown08; int type = Item->Unknown48; router.Route(ClientItemPropertyTypeClass::Create(type, value, id));	// NAME 1 [id, type]: neutral; both branches start with the same loads, which VC6 hoists above the branch as the client has them
	} else {
		int id = Item->Unknown08; int type = Item->Unknown48; router.Route(ClientItemPropertyTypeClass::Create(type, id));
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
		RefCountPtr<ClientItemPropertyClass> quality_property = ClientItemPropertyAttributePercentClass::Create(0, "Quality", "Quality of this item compared to others of its kind", 0.0f, 1.0f, quality, level);	// NAME 1 [quality_property, structure_property]: neutral
		router.Route(RefCountPtr<const ClientItemPropertyClass>(quality_property));
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
			RefCountPtr<ClientItemPropertyClass> structure_property = ClientItemPropertyAttributePercentClass::Create(6, "Structure", tooltip, 0.0f, 1.0f, structure, level);
			router.Route(RefCountPtr<const ClientItemPropertyClass>(structure_property));
		}
	}
	if (!maker.empty()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialMakerClass::Create(maker, ClientItemPropertySpecialMakerClass::MAKER_PLAYER); router.Route(RefCountPtr<const ClientItemPropertyClass>(special));	// NAME 1 [special]: neutral
	} else if (!maker2.empty()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialMakerClass::Create(maker2, ClientItemPropertySpecialMakerClass::MAKER_CORPORATION); router.Route(RefCountPtr<const ClientItemPropertyClass>(special));
	}
}


// NOT EXACT: 0x522860 (functional, masked residue 56 rows, frame 0x66C as the client's: 25 dead Data.Peek() reads in the gap above FAIL_IF (D272) and a copy-initialised RefCountPtr<const ClientItemPropertyClass> local at the Mode Route (the client re-reads it from its slot) spend the inline budget, so the Modes Create's three string constructors and the separator's destructor go out of line and case 26's three assigns expand as the client's; left: the Modes case's inline profile (0x5266A7, ours 28 rows to the client's 60) and the case tail at 0x5237F0 not merged into 0x525D60 (any bare dead statement splits it); CB, 2026-11-20-night)
void ClientItemPropertyBuilderClass::Unknown_522860(UIItemExaminationPropertyRouterBaseClass &router)
{
	Data->Unknown_688830()->Get_Quality(Slot);
	const RefCountPtr<ItemInfoClass> &template_info = Item->Get_Template_Info();	// NAME 1 [info, count, i, ok, base, actual, level and the cases' locals]: neutral
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();
	Data.Peek();











































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
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeStringClass::Create(10, "Ammo", "", "Ammunition required for this weapon", 0, ammo);
			router.Route(property);
			break;
		}
		case ITEM_INFO_AUTO_FIRE:
			if (Data->Unknown50 == 2) {
				RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyOptionAutoFireClass::Create(SpaceClient, Data, Slot);
				router.Route(property);
			}
			break;
		case ITEM_INFO_PROFESSION: {
			int profession = 0;
			template_info->Get_Int(ITEM_INFO_PROFESSION, &profession);
			if (profession != 0) {
				RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialProfessionRequirementClass::Create(SpaceClient, profession);
				router.Route(property);
			}
			break;
		}
		case ITEM_INFO_COMBAT_LEVEL: {
			int combat_level_required = 0;
			template_info->Get_Int(ITEM_INFO_COMBAT_LEVEL, &combat_level_required);












			FAIL_IF(combat_level_required <= 0) break;
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 0, combat_level_required);
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
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeIntegerClass::Create(5, "Damage", "units", "Base damage inflicted by weapon", 0, base, actual, level);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeDistanceClass::Create(10, "Effect Range", "Range of effect.", 1, base, actual, level);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeDistanceClass::Create(10, "Effect Radius", "Radius of effect.", 1, base, actual, level);
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
			if (actual < base) {
				level = 2;
			} else if (actual > base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Energy Use", "units", "Energy required to activate item", 0, base, actual, level);
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
			if (actual < base) {
				level = 2;
			} else if (actual > base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Energy Drain", "units/sec", "Energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_EQUIP_SKILL: {
			std::string skill;
			int unknown = Item->Unknown48;
			template_info->Get_String(ITEM_INFO_EQUIP_SKILL, skill);
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialSkillRequirementClass::Create(SpaceClient, skill, unknown, ClientItemPropertySpecialSkillRequirementClass::REQUIRED_TO_EQUIP);
			router.Route(property);
			break;
		}
		case ITEM_INFO_EXPLORE_LEVEL: {
			int explore_level_required = 0;
			template_info->Get_Int(ITEM_INFO_EXPLORE_LEVEL, &explore_level_required);







			FAIL_IF(explore_level_required <= 0) break;
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 1, explore_level_required);
			router.Route(property);
			break;
		}
		case ITEM_INFO_OVERALL_LEVEL: {
			int total_level_required = 0;
			template_info->Get_Int(ITEM_INFO_OVERALL_LEVEL, &total_level_required);








			FAIL_IF(total_level_required <= 0) break;
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 3, total_level_required);
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
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Maneuverability", "units", "Ability to defeat anti-missile systems", 0, (float)base, (float)actual, level);
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
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Max Power", "units", "Maximum amount of power this reactor can hold", 0, (float)base, (float)actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_RACES: {
			int unknown = 0;
			int race = 0;
			template_info->Get_Int(ITEM_INFO_RACES, &unknown);
			template_info->Get_Int(ITEM_INFO_RACE, &race);
			if (race != 0 || unknown != 0) {
				RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialRaceRequirementClass::Create(SpaceClient, unknown, race);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeDistanceClass::Create(6, "Range", "The maximum targeting range of the item.", 0, base, actual, level);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Recharge", "units/sec", "The amount of energy regenerated per second.", 0, base, actual, level);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeSecondsClass::Create(6, "Reload Time", "The number of seconds that must elapse between activations.", 0, base, actual, level);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Rounds Fired", "rounds", "The number of rounds fired per burst.", 1, base, actual, level);
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
			if (actual < base) {
				level = 2;
			} else if (actual > base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Shield Use", "units", "Shield energy required to activate item", 0, base, actual, level);
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
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Capacity", "units", "Maximum amount of damage this shield can absorb when fully charged.", 0, base, actual, level);
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
			if (actual < base) {
				level = 2;
			} else if (actual > base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Shield Drain", "units/sec", "Shield energy required to activate item", 0, base, actual, level);
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
				level = (actual < base) ? 0 : 2;




			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Recharge", "units/sec", "The amount of shield energy regenerated per second.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_ACTIVATE_SKILL_LEVEL: {
			std::string skill;
			int unknown = 1;
			template_info->Get_String(ITEM_INFO_ACTIVATE_SKILL, skill);
			template_info->Get_Int(ITEM_INFO_ACTIVATE_SKILL_LEVEL, &unknown);
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialSkillRequirementClass::Create(SpaceClient, skill, unknown, ClientItemPropertySpecialSkillRequirementClass::REQUIRED_TO_ACTIVATE);
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
			} else if (actual > base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeDistanceClass::Create(6, "Signature", "Specifies an increased distance that an NPC will notice the player when engine is equipped.", 0, base, actual, level);
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
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Thrust", "units", "Higher thrust values will increase the ship's maximum speed.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_TRADE_LEVEL: {
			int trade_level_required = 0;
			template_info->Get_Int(ITEM_INFO_TRADE_LEVEL, &trade_level_required);





			FAIL_IF(trade_level_required <= 0) break;
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertySpecialLevelRequirementClass::Create(SpaceClient, 2, trade_level_required);
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
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeIntegerClass::Create(6, "Warp", "", "Shows how fast the ship will travel while warping with this engine equipped.", 0, base, actual, level);
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
			if (actual < base) {
				level = 0;
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(6, "Warp Drain", "units/sec", "How fast engine will drain reactor when in free warp (warping with no target.)", 0, base, actual, level);
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
				RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeStringClass::Create(6, "Mode", "", "Which activities this Override item benefits", 0, text);
				RefCountPtr<const ClientItemPropertyClass> route = property; router.Route(route);
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
			if (actual < base) {
				level = 0;
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributeFloatClass::Create(7, "Build Value", "levels", "Improvement in Build/Refine ability when used.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case ITEM_INFO_CRITICAL: {
			float base = 0.0f;
			bool ok = template_info->Get_Float(ITEM_INFO_CRITICAL, &base);



			G_ASSERT(ok);
			int level = 1;
			float actual = base;
			if (info->Has_Field(ITEM_INFO_CRITICAL)) {
				ok = info->Get_Float(ITEM_INFO_CRITICAL, &actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (actual > base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyClass> property = ClientItemPropertyAttributePercentClass::Create(8, "Critical", "Improvement in chances for a Critical Success when used.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		}
	}
}


// 0x527F40
void ClientItemPropertyBuilderClass::Unknown_527F40(UIItemExaminationPropertyRouterBaseClass &router)
{
	const RefCountPtr<ItemInfoClass> &template_info = Item->Get_Template_Info();


	FAIL_IF(!template_info) return;
	ItemEffectDescriptionClass effects = template_info->Get_Activated_Effects();	// NAME 1 [effects, delta, effects2, delta2]: neutral
	std::string delta = Data->Unknown_688830()->Get_Instance_Activated_Effect_Info(Slot);
	if (!delta.empty()) {
		effects.Set_Summary(delta);
	}
	Unknown_528460(router, effects);
	ItemEffectDescriptionClass effects2 = template_info->Get_Equip_Effects();
	std::string delta2 = Data->Unknown_688830()->Get_Instance_Equip_Effect_Info(Slot);
	if (!delta2.empty()) {
		effects2.Set_Summary(delta2);
	}
	Unknown_528460(router, effects2);
}

void ClientItemPropertyBuilderClass::Unknown_528460(UIItemExaminationPropertyRouterBaseClass &router, const ItemEffectDescriptionClass &effects)
{
	unsigned int count = effects.Get_Effects().size();	// unused: an inline layer the client spends budget on (0x527F40 calls the RefCountPtr copies and both strings' _Tidy out of line)  // NAME 1 [count]: neutral
	for (std::vector<ItemEffectClass>::const_iterator it = effects.Get_Effects().begin(); it != effects.Get_Effects().end(); ++it) {	// NAME 1 [it, special]: neutral
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialEffectClass::Create(*it);
		router.Route(special);
	}
}


void ClientItemPropertyBuilderClass::Unknown_5285A0(UIItemExaminationPropertyRouterBaseClass &router)
{
	ItemFlagsStruct flags = Item->Flags;	// NAME 1 [flags, special]: neutral
	if (flags.Is_NotTradable()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Tradable", "This item cannot be traded or sold.", 0, true);
		router.Route(RefCountPtr<const ClientItemPropertyClass>(special));
	}
	if (flags.Is_Temporary()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Temporary", "This item will disappear when player logs out.", 0, true);
		router.Route(RefCountPtr<const ClientItemPropertyClass>(special));
	}
	if (flags.Is_Unique()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Unique", "Player cannot have more than one of this item.", 0, true);
		router.Route(RefCountPtr<const ClientItemPropertyClass>(special));
	}
	if (flags.Is_NotStorable()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Storable", "This item cannot be placed in storage.", 0, true);
		router.Route(RefCountPtr<const ClientItemPropertyClass>(special));
	}
	if (flags.Is_NotDestroyable()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Destroyable", "This item cannot be destroyed.", 0, true);
		router.Route(RefCountPtr<const ClientItemPropertyClass>(special));
	}
	if (flags.Is_NotManufacturable()) {
		RefCountPtr<ClientItemPropertyClass> special = ClientItemPropertySpecialFlagClass::Create(3, "Not Manufacturable", "This item cannot be analyzed or manufactured.", 0, true);
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
**	CIPB, run 2026-11-17b: 0x527F40 exact once 0x528460 takes the ItemEffectDescriptionClass (its vector at +0) and
**	reads Get_Effects() in the loop: the extra inline accessor layer is the budget that sends both strings' _Tidy
**	out of line and puts delta above delta2 in the frame. 0x521C10 0.70 -> 0.85: Get_Int through a template_info
**	reference (the client does not fold &Item->TemplateInfo), the maker properties through a named
**	RefCountPtr<ClientItemPropertySpecialClass> (the client converts into a local at 0x20, overlaying the structure
**	property: this one change put every string and RefCountPtr in the client's slots), the type branch's loads per branch.
**	FU3, run 2026-11-19b: 0x522860 masked residue 235 -> 66. Its 32 property locals are one type, RefCountPtr<ClientItemPropertyClass>
**	(the client's unwind table 0xAABB20-0xAAC133: 32 funclets jmp 0x529130, 32 jmp 0x529040; the Creates' results
**	convert into it through 0x529290 / 0x5292F0 / 0x529320, and each Route temporary from it through 0x5292C0), so
**	Route takes another instantiation: const RefCountPtr<const ClientItemPropertyClass> & (router.h). With that type
**	the dead reads give the client's level-1 cut and case 26's nested assigns together; without it they cannot.
**	0x528460's effect local is the same type (0x527F40 calls 0x5292C0 / 0x529040 / 0x529130 too): with it the object
**	emits the client's RefCountPtr copies again (0x529160 / 0x529190 / 0x5291C0 credited).
**	CB, run 2026-11-20-night: 0x5285A0's and 0x521C10's property locals are the same RefCountPtr<ClientItemPropertyClass>
**	(the client's unwind funclets: 0x5285A0's 0xAAC3C0-0xAAC483 and 0x521C10's 0xAAB9B0-0xAABAC8 release them through
**	0x529130, their Route temporaries through 0x529040; the flags' Creates convert through 0x529380). The retype alone
**	frees inline budget (0x5285A0: the last Create temporary's destructor 0x529220 inlines, residue 20; 0x521C10: the
**	makers' string _Tidy inlines, 8 -> 31); explicit RefCountPtr<const ClientItemPropertyClass>(property) Route
**	temporaries spend it back: 0x5285A0 exact with them on its first five Routes (an explicit one on the sixth pushes the
**	out-of-line converter's eax where the client re-takes the temporary's address), 0x521C10 back at 8 with them on the
**	quality / structure / maker Routes (the direct Create Routes take none: no effect).
*/
