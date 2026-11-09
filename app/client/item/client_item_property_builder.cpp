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


// NOT EXACT: 0x522860 (functional, 0.61: the same calls, strings and asserts in order; the frame is 0xC smaller, VC6 inlines string assigns in other cases, and case 35 releases its property through the RefCountPtr the flags use, 0x529130)
void ClientItemPropertyBuilderClass::Unknown_522860(UIItemExaminationPropertyRouterBaseClass &router)
{
	Data->Unknown_688830()->Unknown_68A010(Slot);
	const RefCountPtr<ItemTemplateInfoClass> &template_info = Item->Get_Template_Info();	// NAME 1 [info, count, i, ok, base, actual, level and the cases' locals]: neutral




































































	FAIL_IF(!template_info) return;
	RefCountPtr<ItemTemplateInfoClass> info = Create_NEW(new ItemTemplateInfoClass);
	info->Unknown_8417B0(Data->Unknown_688830()->Unknown_68A090(Slot));
	unsigned int count = template_info->Unknown_83FE00();
	for (unsigned int i = 0; i < count; i++) {
		int id = template_info->Unknown_840940(i);
		if (id == -1) {
			continue;
		}
		switch (id) {
		case 0:


			G_ASSERT(0);
			break;
		case 1: {
			std::string ammo;
			bool ok = template_info->Unknown_840980(ammo, i);


			G_ASSERT(ok);
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_521560(10, "Ammo", "", "Ammunition required for this weapon", 0, ammo);
			router.Route(property);
			break;
		}
		case 2:
			if (Data->Unknown50 == 2) {
				RefCountPtr<ClientItemPropertyOptionClass> property = Unknown_52AD00(SpaceClient, Data, Slot);
				router.Route(property);
			}
			break;
		case 3: {
			int profession = 0;
			template_info->Unknown_840A20(3, profession);
			if (profession != 0) {
				RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_52CF90(SpaceClient, profession);
				router.Route(property);
			}
			break;
		}
		case 4: {
			int combat_level_required = 0;
			template_info->Unknown_840A20(4, combat_level_required);












			FAIL_IF(combat_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_530530(SpaceClient, 0, combat_level_required);
			router.Route(property);
			break;
		}
		case 5: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(5, base);


			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Unknown_83FD60(5)) {
				ok = info->Unknown_840A20(5, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_520790(5, "Damage", "units", "Base damage inflicted by weapon", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 7: {
			int base;
			template_info->Unknown_840A20(7, base);
			int level = 1;
			int actual = base;
			if (info->Unknown_83FD60(7)) {
				info->Unknown_840A20(7, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_51FEA0(10, "Effect Range", "Range of effect.", 1, base, actual, level);
			router.Route(property);
			break;
		}
		case 8: {
			int base;
			template_info->Unknown_840A20(8, base);
			int level = 1;
			int actual = base;
			if (info->Unknown_83FD60(8)) {
				info->Unknown_840A20(8, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_51FEA0(10, "Effect Radius", "Radius of effect.", 1, base, actual, level);
			router.Route(property);
			break;
		}
		case 9: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(9, base);














			G_ASSERT(ok);
			int level = 1;
			float actual = base;
			if (info->Unknown_83FD60(9)) {
				ok = info->Unknown_840AC0(9, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Energy Use", "units", "Energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 10: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(10, base);



			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Unknown_83FD60(10)) {
				ok = info->Unknown_840AC0(10, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Energy Drain", "units/sec", "Energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 11: {
			std::string skill;
			int unknown = Item->Unknown48;
			template_info->Unknown_8409D0(11, skill);
			RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_52E760(SpaceClient, skill, unknown, false);
			router.Route(property);
			break;
		}
		case 12: {
			int explore_level_required = 0;
			template_info->Unknown_840A20(12, explore_level_required);







			FAIL_IF(explore_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_530530(SpaceClient, 1, explore_level_required);
			router.Route(property);
			break;
		}
		case 14: {
			int total_level_required = 0;
			template_info->Unknown_840A20(14, total_level_required);








			FAIL_IF(total_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_530530(SpaceClient, 3, total_level_required);
			router.Route(property);
			break;
		}
		case 15: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(15, base);


			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Unknown_83FD60(15)) {
				ok = info->Unknown_840A20(15, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Maneuverability", "units", "Ability to defeat anti-missile systems", 0, (float)base, (float)actual, level);
			router.Route(property);
			break;
		}
		case 16: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(16, base);



			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Unknown_83FD60(16)) {
				ok = info->Unknown_840A20(16, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Max Power", "units", "Maximum amount of power this reactor can hold", 0, (float)base, (float)actual, level);
			router.Route(property);
			break;
		}
		case 18: {
			int unknown = 0;
			int race = 0;
			template_info->Unknown_840A20(18, unknown);
			template_info->Unknown_840A20(17, race);
			if (race != 0 || unknown != 0) {
				RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_52DB60(SpaceClient, unknown, race);
				router.Route(property);
			}
			break;
		}
		case 19: {
			int base;
			template_info->Unknown_840A20(19, base);
			int level = 1;
			int actual = base;
			if (info->Unknown_83FD60(19)) {
				info->Unknown_840A20(19, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_51FEA0(6, "Range", "The maximum targeting range of the item.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 20: {
			float base;
			template_info->Unknown_840AC0(20, base);
			int level = 1;
			float actual = base;
			if (info->Unknown_83FD60(20)) {
				info->Unknown_840AC0(20, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Recharge", "units/sec", "The amount of energy regenerated per second.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 21: {
			float base;
			template_info->Unknown_840AC0(21, base);
			float actual = base;
			int level = 1;
			if (info->Unknown_83FD60(21)) {
				info->Unknown_840AC0(21, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_521050(6, "Reload Time", "The number of seconds that must elapse between activations.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 22: {
			int base;
			template_info->Unknown_840A20(22, base);
			int level = 1;
			int actual = base;
			if (info->Unknown_83FD60(22)) {
				info->Unknown_840A20(22, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_520790(6, "Rounds Fired", "rounds", "The number of rounds fired per burst.", 1, base, actual, level);
			router.Route(property);
			break;
		}
		case 23: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(23, base);



























			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Unknown_83FD60(23)) {
				ok = info->Unknown_840AC0(23, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Shield Use", "units", "Shield energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 24: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(24, base);



			G_ASSERT(ok);
			int level = 1;
			int actual = base;
			if (info->Unknown_83FD60(24)) {
				ok = info->Unknown_840A20(24, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_520790(6, "Capacity", "units", "Maximum amount of damage this shield can absorb when fully charged.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 25: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(25, base);



			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Unknown_83FD60(25)) {
				ok = info->Unknown_840AC0(25, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 0;
			} else if (actual < base) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Shield Drain", "units/sec", "Shield energy required to activate item", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 26: {
			float base;
			template_info->Unknown_840AC0(26, base);
			int level = 1;
			float actual = base;
			if (info->Unknown_83FD60(26)) {
				info->Unknown_840AC0(26, actual);
				if (base <= actual) {
					level = 2;
				} else {
					level = 0;
				}
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Recharge", "units/sec", "The amount of shield energy regenerated per second.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 28: {
			int unknown = 1;
			std::string skill;
			template_info->Unknown_8409D0(27, skill);
			template_info->Unknown_840A20(28, unknown);
			RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_52E760(SpaceClient, skill, unknown, true);
			router.Route(property);
			break;
		}
		case 29: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(29, base);















			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Unknown_83FD60(29)) {
				ok = info->Unknown_840A20(29, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 2;
			} else if (base < actual) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_51FEA0(6, "Signature", "Specifies an increased distance that an NPC will notice the player when engine is equipped.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 31: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(31, base);







			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Unknown_83FD60(31)) {
				ok = info->Unknown_840A20(31, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_520790(6, "Thrust", "units", "Higher thrust values will increase the ship's maximum speed.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 32: {
			int trade_level_required;
			template_info->Unknown_840A20(32, trade_level_required);





			FAIL_IF(trade_level_required <= 0) break;
			RefCountPtr<ClientItemPropertySpecialClass> property = Unknown_530530(SpaceClient, 2, trade_level_required);
			router.Route(property);
			break;
		}
		case 33: {
			int base = 0;
			bool ok = template_info->Unknown_840A20(33, base);


			G_ASSERT(ok);
			int actual = base;
			int level = 1;
			if (info->Unknown_83FD60(33)) {
				ok = info->Unknown_840A20(33, actual);

				G_ASSERT(ok);
			}
			if (actual < base) {
				level = 0;
			} else if (base < actual) {
				level = 2;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_520790(6, "Warp", "", "Shows how fast the ship will travel while warping with this engine equipped.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 34: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(34, base);



			G_ASSERT(ok);
			int level = 1;
			float actual = base;
			if (info->Unknown_83FD60(34)) {
				ok = info->Unknown_840AC0(34, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 2;
			} else if (actual < base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(6, "Warp Drain", "units/sec", "How fast engine will drain reactor when in free warp (warping with no target.)", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 35: {
			int mode_flags = 0;
			bool ok = template_info->Unknown_840A20(35, mode_flags);



			G_ASSERT(ok);
			G_ASSERT(mode_flags != MODE_NONE_BF);
			if (mode_flags != MODE_NONE_BF) {
				std::vector<std::string> modes;
				if (mode_flags & 1) modes.push_back("Manufacturing");
				if (mode_flags & 2) modes.push_back("Analyze");
				if (mode_flags & 4) modes.push_back("Dismantle");
				if (mode_flags & 8) modes.push_back("Refining");
				std::string text = Unknown_4FB5B0(modes, ", ");
				RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_521560(6, "Mode", "", "Which activities this Override item benefits", 0, text);
				router.Route(property);
			}
			break;
		}
		case 36: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(36, base);














			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Unknown_83FD60(36)) {
				ok = info->Unknown_840AC0(36, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 2;
			} else if (actual < base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_5203D0(7, "Build Value", "levels", "Improvement in Build/Refine ability when used.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		case 37: {
			float base = 0.0f;
			bool ok = template_info->Unknown_840AC0(37, base);



			G_ASSERT(ok);
			float actual = base;
			int level = 1;
			if (info->Unknown_83FD60(37)) {
				ok = info->Unknown_840AC0(37, actual);

				G_ASSERT(ok);
			}
			if (base < actual) {
				level = 2;
			} else if (actual < base) {
				level = 0;
			}
			RefCountPtr<ClientItemPropertyAttributeClass> property = Unknown_520B20(8, "Critical", "Improvement in chances for a Critical Success when used.", 0, base, actual, level);
			router.Route(property);
			break;
		}
		}
	}
}


// NOT EXACT: 0x527F40 (0.78: the same code; the client pushes esi at entry, ours after the FAIL_IF, and its frame is 8 bytes bigger)
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
	unsigned int count = effects.size();	// unused: an inline layer the client spends budget on (0x527F40 calls the RefCountPtr copies out of line)  // NAME 1 [count]: neutral
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
**	IP1, run 2026-11-08d: the constructor 0x521B80, destructor 0x521BB0, Build 0x521BD0, the effects' loop 0x528460
**	(a member that never reads this: ret 8, inlined twice into 0x527F40) and the flags' specials 0x5285A0 exact (the
**	flag tests are inline bool accessors: the client shifts each bit down and tests bit 0; a plain bitfield test or an
**	explicit shift folds to a mask test); 0x521C10 (the name, id, icon, level, description, quality, structure and
**	maker properties; stack slots), 0x522860 (the template's attributes by id, each compared with the slot's delta
**	copy: level 0 worse, 1 unchanged, 2 better; the requirements; the asserts on the client's lines 155-796) and
**	0x527F40 (inline budget) NOT EXACT. The callees in files the tree does not have yet are declared in
**	client_item_property_builder.h (interim, D137).
*/
