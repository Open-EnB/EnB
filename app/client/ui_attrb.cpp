// Earth & Beyond client (reconstruction): app\Client\ui_attrb.cpp, the character info window's attributes and skill panels. Notes at the
// end of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30)
#include <stdio.h>
#include <string.h>
#include <string>
#include "pdaint.h"			// (before sclient.h: its PDAInterfaceClass)
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "..\packets\pkskill.h"
#include "..\audio\soundlib.h"
#include "ui_attrb.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x5495F0)
#include "..\shared\auxdata\delta_numbering.h"	// 0x548210 numbers the int field's global
extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
#include "avatar.h"
#include "gd_uiback.h"		// UIBackGadget (0x674260)
#include "assetmgr.h"
#include "avatar_pip.h"
#include "co.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\money.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\g_assert.h"
#include "..\common\race.h"		// RaceNames (0xBB06E0)
#include "ui_experience.h"	// UIExperienceClass (+0x8C)
// (the skill panel's declarations left with it for ui_attrb_skill_panel.cpp; the blank lines below keep the asserts
// on the client's lines 237 and 382)














































// skillnam.cpp's, as far as this file calls them (cdecl)
class SkillNamesDatabase { public: static int Max_Skill();	// 0x818590 (skillnam.h's)
	static const char *Unknown_8184D0(int skill);	// 0x8184D0: 'Combat', 'Trade' or 'Explore'  // NAME 1: neutral (D10)
	static const char *Unknown_818200(int skill);					// 0x818200: the skill's name  // NAME 1: neutral (D10)
	static const char *Unknown_818250(int skill);					// 0x818250: its description  // NAME 1: neutral (D10)
	static const char *Unknown_8182A0(int skill, int level);		// 0x8182A0: a level's detail  // NAME 1: neutral (D10)
	static const char *Unknown_818640(int skill, int index); };	// 0x818640: not yet trained  // NAME 1: neutral (D10)


// What InterfaceClass's +0x40 points to, as far as this file calls it (as pdaint.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
	virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24();
	virtual ClientObjectClass *Get_Avatar();			// +0x28 (client.h's)  // NAME 2: ClientClass's slot +0x28
};

int Unknown_82F350(int race, int prof);				// chartemplate.cpp's 0x82F350 (race and profession to the class index)  // NAME 1: neutral (D10), as chartemplate.cpp's
const char *Unknown_82F3C0(int index);				// chartemplate.cpp's 0x82F3C0 (the class's name)  // NAME 1: neutral (D10), as chartemplate.cpp's

// auxdata.h's find, set (+0x70) or changed since (+0x70 above 1), else NULL
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10), as ui_avatar_cust.cpp's
template<class C> inline C *Unknown_Find_Changed(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10), as ui_avatar_cust.cpp's
template<class C> inline C *Unknown_Find(DeltaCompoundClass *root, const char *name, bool all) { return(all ? Unknown_Find_Valid<C>(root, name) : Unknown_Find_Changed<C>(root, name)); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_ValidR(DeltaCompoundClass *const &root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): the root by reference: the root by reference (0x547E40 reads it again after the NameClass)
template<class C> inline C *Unknown_Find_ChangedR(DeltaCompoundClass *const &root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10): the root by reference: the root by reference (0x547E40 reads it again after the NameClass)
template<class C> inline C *Unknown_FindR(DeltaCompoundClass *const &root, const char *name, bool all) { return(all ? Unknown_Find_ValidR<C>(root, name) : Unknown_Find_ChangedR<C>(root, name)); }	// NAME 1: neutral (D10): the root by reference

// @0x546A80
Unknown546A80Class::Unknown546A80Class(SClient *client, RefCountPtr<RenderObjClass> &mesh) :
	InterfaceClass((LineDisplayClientClass *)client),
	Unknown84(mesh),
	Experience(NULL),
	AvatarPip(NULL),
	UnknownAC(false)
{
}

// @0x546B50
Unknown546A80Class::~Unknown546A80Class()
{
	if (Active) {
		InterfaceMesh->Remove();
		AvatarPip->Unknown_7E4350();
		InterfaceClass::Unknown_5C();
	}
	if (Experience) {
		Experience->Unknown_58CF60();
		delete Experience;
		Experience = NULL;
	}
	if (AvatarPip) {
		delete AvatarPip;
		AvatarPip = NULL;
	}
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload("fa_att.ini");
}

// @0x546DD0
void Unknown546A80Class::Unknown_54()
{
	InterfaceMesh = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("TSI12A"));
	Unknown_BE67BC->Load("fa_att.ini");
	Unknown_BE67C4->Add(InterfaceMesh.Peek());
	Experience = new UIExperienceClass(InterfaceMesh.Peek(), this);
	if (Experience) {
		Experience->Unknown_08(Client->Get_Avatar());
	}
	AvatarPip = new AvatarPipClass;
	Unknown90 = Create_Peek(Unknown_BE67C4->Get_Style("CHARACTER_NAME_ATT", InterfaceMesh.Peek()));
	Unknown94 = Create_Peek(Unknown_BE67C4->Get_Style("CHARACTER_LEVEL_ATT", InterfaceMesh.Peek()));
	Unknown98 = Create_Peek(Unknown_BE67C4->Get_Style("CHARACTER_CLASS_NAME_ATT", InterfaceMesh.Peek()));
	Unknown9C = Create_Peek(Unknown_BE67C4->Get_Style("CHARACTER_RANK_ATT", InterfaceMesh.Peek()));
	UnknownA0 = Create_Peek(Unknown_BE67C4->Get_Style("CHARACTER_CREDITS_ATT", InterfaceMesh.Peek()));
	UnknownA4 = Create_Peek(Unknown_BE67C4->Get_Style("EXP_DEBT_ATT", InterfaceMesh.Peek()));
	Unknown_10();
}

// @0x5472C0
void Unknown546A80Class::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
}

// @0x5472F0
void Unknown546A80Class::Unknown_58()
{
	if (!Active) {
		InterfaceClass::Unknown_58();
		Unknown84->Add_Sub_Object_To_Bone(InterfaceMesh.Peek(), "CHAR_ATTRIB");
		Unknown_547570();
		Unknown_5477C0(true);
		Unknown_547C20(true);
		Unknown_547FD0(true);
		UnknownAC = true;
	}
}

// @0x547370
void Unknown546A80Class::Unknown_5C()
{
	if (Active) {
		InterfaceMesh->Remove();
		AvatarPip->Unknown_7E4350();
		InterfaceClass::Unknown_5C();
	}
}

// @0x5473B0
void Unknown546A80Class::Unknown_5473B0()
{
	if (!UnknownAC) {
		AvatarPip->Unknown_7E3A20(NULL, ((ClientClass *)Client)->AvatarID, NULL);
	}
}

// @0x5473F0
void Unknown546A80Class::Unknown_60()
{
	if (UnknownAC) {
		UnknownAC = false;
		AvatarPip->Unknown_7E36B0(Client->As_Space_Client(), Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("PICTURE")), true);
		Unknown_5473B0();
	} else if (!Client->As_Space_Client()->Unknown_744B60()) {
		Unknown_5473B0();
	}
	Unknown_547C20(false);
	Unknown_5477C0(false);
	Unknown_547E40(false);
	Unknown_547FD0(false);
}

// @0x547550
bool Unknown546A80Class::Unknown_68(int key)
{
	return(false);
}

// @0x547570
void Unknown546A80Class::Unknown_547570()
{
	if (SkillNamesDatabase::Max_Skill()) {
		CharacterViewAvatarClass *avatar = Client->As_Space_Client()->Unknown_74C840(((ClientClass *)Client)->Get_Avatar_ID());









		G_ASSERT(avatar);
		std::string name("Not Found");
		int race = 0;
		int profession = 0;
		if (avatar) {
			AvatarDescClass *desc = &avatar->Desc;
			desc->Get_Full_Name(name);
			race = desc->Race;
			profession = desc->Profession;
		}
		Unknown90->Change_Text(name.c_str(), 0);
		char text[64];
		_snprintf(text, sizeof(text), "%s %s", RaceNames[race], Unknown_82F3C0(Unknown_82F350(race, profession)));
		Unknown98->Change_Text(text, 0);
		Unknown_547E40(true);
	}
}

// @0x547730
int Unknown546A80Class::Unknown_547730(DeltaTemplateClass<int> *field, const char *style)
{
	int value = field ? field->Value : -1;
	char text[256];
	_snprintf(text, sizeof(text), "%d", value);
	FontSys *font = Unknown_BE67C4->Get_Style(style, InterfaceMesh.Peek());
	font->Change_Text(text, 0);
	return(value);
}

// @0x5477C0
void Unknown546A80Class::Unknown_5477C0(bool all)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	bool changed = false;
	DeltaTemplateClass<int> *field;
	if (all) {
		field = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "RPGInfo.CombatLevel");
	} else {
		field = Unknown_Find_Changed<DeltaTemplateClass<int> >(aux, "RPGInfo.CombatLevel");
	}
	if (field || all) {
		Unknown_547730(field, "Combat_val");
		changed = true;
	}
	if (all) {
		field = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "RPGInfo.TradeLevel");
	} else {
		field = Unknown_Find_Changed<DeltaTemplateClass<int> >(aux, "RPGInfo.TradeLevel");
	}
	if (field || all) {
		Unknown_547730(field, "Trade_val");
		changed = true;
	}
	if (all) {
		field = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "RPGInfo.ExploreLevel");
	} else {
		field = Unknown_Find_Changed<DeltaTemplateClass<int> >(aux, "RPGInfo.ExploreLevel");
	}
	if (field || all) {
		Unknown_547730(field, "Exploration_val");
		changed = true;
	}
	if (changed || all) {
		int level = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "RPGInfo.CombatLevel")->Value;
		level += Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "RPGInfo.TradeLevel")->Value;
		char text[32];
		_snprintf(text, sizeof(text), "Level: %d", Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "RPGInfo.ExploreLevel")->Value + level);
		Unknown94->Change_Text(text, 0);
	}
	if (Experience) {
		int value = (int)Experience->Object;
		if (value != (int)Client->Get_Avatar()) {
			Experience->Unknown_08(Client->Get_Avatar());
		}
		Experience->Unknown_10();
	}
}

// NOT EXACT: layout (similarity 0.61: the client keeps the valid branch's null merge, xor eax / test eax, where ours threads it; the logic is the client's; attempts.csv)
// @0x547C20
void Unknown546A80Class::Unknown_547C20(bool all)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	MoneyClass money;
	DeltaTemplateClass<MoneyClass> *field;
	if (all) {
		field = Unknown_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	} else {
		field = Unknown_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money");
		if (!field) return;
	}
	if (field) {
		money = field->Value;
	}
	char text[32];
	_snprintf(text, sizeof(text), "Credits: %s", money.Amount_As_Text(true).c_str());
	UnknownA0->Change_Text(text, 0);
}










































#pragma auto_inline(off)
// @0x547E40
void Unknown546A80Class::Unknown_547E40(bool all)
{
	ClientObjectClass *avatar = Client->Get_Avatar();
	G_ASSERT(avatar);
	if (avatar && avatar->AuxData) {
		DeltaStringClass *rank = Unknown_FindR<DeltaStringClass>(*(DeltaCompoundClass **)&avatar->AuxData, "Rank", all);
		if (rank) {
			Unknown9C->Change_Text(rank->String, 0);
			return;
		}
	}
	if (all) {
		Unknown9C->Change_Text("", 0);
	}
}

template<class C> static inline C *Unknown_Find_Valid_547FD0(DeltaCompoundClass *r, const char *s) { return(Unknown_Find_Valid<C>(r, s)); }	// NAME 1: neutral (D10): extra inline layer keeps 0x547FD0's valid null merge unthreaded
// @0x547FD0
void Unknown546A80Class::Unknown_547FD0(bool all)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<int> *debt;
	if (all) {
		debt = Unknown_Find_Valid_547FD0<DeltaTemplateClass<int> >(aux, "XPDebt");
	} else {
		debt = Unknown_Find_Changed<DeltaTemplateClass<int> >(aux, "XPDebt");
		if (!debt) return;
	}
	char text[64];
	if (debt && debt->Value > 0) {
		_snprintf(text, sizeof(text), "XP Debt: %d", debt->Value);
	} else {
		_snprintf(text, sizeof(text), " ");
	}
	UnknownA4->Change_Text(text, 0); UnknownA4.Peek(); UnknownA4.Peek();	// D272: two dead reads push only the valid branch's Cast out of line
}
#pragma auto_inline(on)

/*
**	Notes (AT2, 2026-11-06)
**
**	The client's object ui_attrb.obj runs 0x546A80-0x54962F (after ui_analyze.obj, before ui_attrb_skills.obj 0x549640):
**	two classes. First the attributes panel at ui_attrb_skills.cpp's +0xC0 (Unknown546A80Class, 0xB0 bytes; AT3,
**	2026-11-08): the constructor 0x546A80 (InterfaceClass through the __INTRFACE_MEMBERS constructor 0x540D30), the
**	deleting destructor 0x546B20, the destructor 0x546B50 (the slot +0x5C body written out: the client does not call it),
**	slots +0x54 0x546DD0, +0x7C 0x5472C0, +0x58 0x5472F0, +0x5C 0x547370, +0x60 0x5473F0, +0x68 0x547550, 0x5473B0, 0x547570
**	(G_ASSERT 'avatar' line 237; the avatar's AvatarDescClass is its +0xF8), 0x547730 (inlined in 0x5477C0, emitted for
**	nobody), 0x5477C0 and 0x547E40 (G_ASSERT 'avatar' line 382) exact; RefCountPtr<FontSys>::operator= 0x548150 (the
**	sixth style's assignment goes out of line), the Cast instance 0x5481C0 and the thunks 0x548240 / 0x548250 come with
**	them. The forms that matter: the root by reference for 0x547E40 (it reads the avatar's +0x88 again after each
**	NameClass), plain roots elsewhere; 0x547570 reads the id through ClientClass::Get_Avatar_ID(); 0x5477C0 takes the
**	experience display's +0x04 into a local before the compare (the inline getter costs the third find's inlining);
**	auto_inline(off) around 0x547E40 / 0x547FD0 keeps them out of 0x5472F0 / 0x5473F0 as the client has them. NOT EXACT
**	0x547C20 (layout) and 0x547FD0 (inline budget): attempts.csv. PS (run 2026-11-18): the object ends at 0x54825F (CRT
**	entries 223-225: permstr.h's static, <string>'s initialiser 0x548190, the numbering 0x548210); permstr.h's static
**	0x548260 opens the next object, so the skill panel below now lives in ui_attrb_skill_panel.cpp. Then the skill panel at ui_attrb_skills.cpp's
**	+0xFC (Unknown548D60Class): the constructor 0x5482B0, the deleting destructor 0x548410, the destructor 0x548440, slots
**	+0x54 0x548610 and +0x68 0x548C50, 0x5494E0 exact (auto_inline(off) around it: 0x548D60 calls it out of line); 0x548D60
**	(the selected skill's texts, called by ui_attrb_skills.cpp) NOT EXACT: the title's colour stores (tried: Vector3
**	assignment, Set, a FontSys pointer helper, a reference to +0x494, a local pointer, memcpy, a chained Set returning the
**	struct; attempts.csv). Its flags are bit fields read through bool accessors (the client's word load, shr, test 1;
**	plain shifts or bare bit fields give a byte test), the last call's argument an if / else of two calls. pkskill.h's inline ~SkillPacketClass 0x548D40 is emitted here between them. After the slots: the adjustor
**	thunks 0x5495D0 (-4) / 0x5495E0 (-0x38) of the deleting destructor and permstr.h's PermStringInitClass static
**	(0x5495A0 here; ui_attrb_skills.cpp's notes name 0x5495F0 / 0x549620).
*/
