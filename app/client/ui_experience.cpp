// Earth & Beyond client (reconstruction): app\Client\ui_experience.cpp, the experience bars (combat, trade, explore) of
// the PDA and the character info window. Notes at the end of the file.
#include <stdio.h>
#include "ui_experience.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "intrface.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "..\shared\uvcontrl.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x58BF90)
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: what the helpers pass









// What a bar holds at +0x64: its texture's offset controller (gone by its destructor: the unwind actions of 0x58BFE0 /
// 0x58C280 call 0x58C220) and the value shown (the bar's -1 store sits before its unwind state 1, so it is this member's)
class UIExperienceGaugeClass	// NAME 1: neutral (D10)
{
public:
	UIExperienceGaugeClass() : Gauge(NULL), Value(-1.0f) {}













	~UIExperienceGaugeClass() { G_ASSERT(!Gauge); }		// 0x58C220

	UVControllerClass *Gauge;							// +0x00 (0x4A1240's, 0x0C bytes)  // NAME 5: the assert's
	float Value;										// +0x04 (-1 until set)  // NAME 1: neutral
};

// One bar (0x6C bytes, table 0xAF15EC): the mesh's sub-object of that name, its texture scrolled to the value
class UIExperienceBarClass : public Unknown_675120Class	// NAME 2: its sub-objects are the mesh's '*_bar'
{
public:
	UIExperienceBarClass(RenderObjClass *mesh, const char *name);	// 0x58BFE0  // NAME 1 [mesh, name]: what 0x58C450 passes
	virtual ~UIExperienceBarClass();					// +0x00 0x58C280 (deleting 0x58C250)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator) { if (!indicator) return(NULL); return(RObjGadgetClass::Unknown_24(position, frame, indicator)); }	// +0x24 0x58C1F0  // NAME 1 [position, frame, indicator]: the base's
	void Set_Value(float value);						// 0x58C300 (the offset 0.46 * value, when it differs)  // NAME 2: what it does
	void Set_Value_Clamped(float value);				// 0x58C390 (clamped to 0..1)  // NAME 2: what it does

	bool Exists;										// +0x60 (the sub-object found)  // NAME 5: the asserts'
	UIExperienceGaugeClass Controller;					// +0x64  // NAME 1: what it holds
};







// @0x58BFE0
UIExperienceBarClass::UIExperienceBarClass(RenderObjClass *mesh, const char *name) :
	Unknown_675120Class(NULL, 0),
	Exists(false)
{

	FAIL_IF(!name) return;
	Unknown_65FFA0(-1);

	RefCountPtr<RenderObjClass> robj = Create_Get(mesh->Get_Sub_Object_By_Name(name));


	FAIL_IF(!robj) return;
	Unknown_496AE0(robj.Peek(), true);
	Exists = true;
	Controller.Gauge = new UVControllerClass(robj.Peek(), 0, 0);
	Set_Render_Obj(robj.Peek());
	Set_Value(0.0f);
}

// @0x58C280
UIExperienceBarClass::~UIExperienceBarClass()
{
	delete Controller.Gauge;
	Controller.Gauge = NULL;
}

// @0x58C300
void UIExperienceBarClass::Set_Value(float value)
{



	FAIL_IF(!Exists) return;
	if (Controller.Value != value) {
		Controller.Gauge->Unknown_4A13A0(Vector2(0.0f, value * 0.46));
		Controller.Value = value;
	}
}

// @0x58C390
void UIExperienceBarClass::Set_Value_Clamped(float value)
{


	FAIL_IF(!Exists) return;
	if (value < 0.0f) {
		value = 0.0f;
	} else if (value > 1.0f) {
		value = 1.0f;
	}
	Controller.Gauge->Unknown_4A13A0(Vector2(0.0f, value * 0.46));
	Controller.Value = value;
}









// @0x58C450  // NOT EXACT: functional (D64): the client calls GenericNode::GenericNode out of line (an EH state for the node)
UIExperienceClass::UIExperienceClass(RenderObjClass *mesh, InterfaceClass *parent) :
	Parent(parent),
	CombatBar(NULL),
	TradeBar(NULL),
	ExploreBar(NULL)
{
	CombatBar = new UIExperienceBarClass(mesh, "combat_bar");
	GadgetClass::Set_Tooltip(CombatBar, "UI_EXPERIENCE", "COMBAT");
	if (CombatBar->Exists) {
		Parent->Unknown64.Unknown_6870E0(CombatBar, 0, Parent->View, true, true);
	}
	TradeBar = new UIExperienceBarClass(mesh, "trade_bar");
	GadgetClass::Set_Tooltip(TradeBar, "UI_EXPERIENCE", "TRADE");
	if (TradeBar->Exists) {
		Parent->Unknown64.Unknown_6870E0(TradeBar, 0, Parent->View, true, true);
	}
	ExploreBar = new UIExperienceBarClass(mesh, "explore_bar");
	GadgetClass::Set_Tooltip(ExploreBar, "UI_EXPERIENCE", "EXPLORE");
	if (ExploreBar->Exists) {
		Parent->Unknown64.Unknown_6870E0(ExploreBar, 0, Parent->View, true, true);
	}
}

// @0x58C6E0
UIExperienceClass::~UIExperienceClass()
{
	GadgetClass::Destroy(CombatBar);
	GadgetClass::Destroy(TradeBar);
	GadgetClass::Destroy(ExploreBar);
}

// @0x58C7B0
void UIExperienceClass::Unknown_58C7B0()
{
	if (CombatBar->Exists) CombatBar->Set_Value_Clamped(0.0f);
	if (TradeBar->Exists) TradeBar->Set_Value_Clamped(0.0f);
	if (ExploreBar->Exists) ExploreBar->Set_Value_Clamped(0.0f);
}

// @0x58C8E0
void UIExperienceClass::Unknown_Watch_0C()
{
	Unknown_58C7B0();
}

// @0x58CA10
void UIExperienceClass::Unknown_08(ClientObjectClass *object)
{
	Unknown_58C7B0();
	UnknownObjectWatchClass::Unknown_08(object);
}

// @0x58CB50  // NOT EXACT: functional (D64): the client calls Unknown_Find_Field out of line for CombatExp only
void UIExperienceClass::Unknown_10()
{
	if (!Object) return;

	ClientClass *client = Object->Client;
	FAIL_IF(!client) return;
	SClient *sclient = (SClient *)client->As_Space_Client();
	FAIL_IF(!sclient) return;
	DeltaCompoundClass *aux = (DeltaCompoundClass *)sclient->AuxData;
	char text[32];
	if (CombatBar->Exists) {
		DeltaTemplateClass<float> *field = Unknown_Find_Field<DeltaTemplateClass<float> >(aux, NameClass("RPGInfo.CombatExp"));
		float value = (field && field->Unknown70) ? field->Value : 0.0f;
		CombatBar->Set_Value(value);
		_snprintf(text, sizeof(text), "Progress: %1.2f%%", value * 100.0f);
		CombatBar->Set_Unknown14(text);
	}
	if (TradeBar->Exists) {
		DeltaTemplateClass<float> *field = Unknown_Find_Field<DeltaTemplateClass<float> >(aux, NameClass("RPGInfo.TradeExp"));
		float value = (field && field->Unknown70) ? field->Value : 0.0f;
		TradeBar->Set_Value(value);
		_snprintf(text, sizeof(text), "Progress: %1.2f%%", value * 100.0f);
		TradeBar->Set_Unknown14(text);
	}
	if (ExploreBar->Exists) {
		DeltaTemplateClass<float> *field = Unknown_Find_Field<DeltaTemplateClass<float> >(aux, NameClass("RPGInfo.ExploreExp"));
		float value = (field && field->Unknown70) ? field->Value : 0.0f;
		ExploreBar->Set_Value(value);
		_snprintf(text, sizeof(text), "Progress: %1.2f%%", value * 100.0f);
		ExploreBar->Set_Unknown14(text);
	}
}

// @0x58CF60
void UIExperienceClass::Unknown_58CF60()
{
	if (CombatBar->Exists) Parent->Unknown64.Unknown_687150(CombatBar);
	if (TradeBar->Exists) Parent->Unknown64.Unknown_687150(TradeBar);
	if (ExploreBar->Exists) Parent->Unknown64.Unknown_687150(ExploreBar);
}

/*
**	Notes (O4, 2026-11-10)
**
**	The client's object 0x58BF90-0x58D010: permstr.h's PermStringInitClass static 0x58BF90 (its destructor 0x58BFC0) opens
**	it, after the adjustor thunks 0x58BF70 (-4) / 0x58BF80 (-0x38) of the previous object's deleting destructor 0x58B7F0;
**	<string>'s guarded statics 0x58CFD0 / 0x58D000 end it, before ui_faction.cpp's PermStringInitClass static 0x58D030.
**	Every __FILE__ in 0x58BFE0-0x58CF60 is 'e:\build\beta\G\app\Client\ui_experience.cpp' (0xB7EFBC). The asserts:
**	'!Gauge' l.47, '!name' l.79, '!robj' l.85, '!Exists' l.106 / l.118, '!client' l.195, '!sclient' l.197.
*/
