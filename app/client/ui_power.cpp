// Earth & Beyond client (reconstruction): app\client\ui_power.cpp, the power gauges (energy, shields, hull) of the
// cockpit, the group window's members and the MFD. The file name is provisional (NAME 1). Notes at the end of the file.
#include <stdio.h>
#include "ui_power.h"
#include "gd_gauge.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "intrface.h"
#include "gadget.h"
#include "..\audio\soundlib.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x5DBF70)
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_numbering.h"
#include "..\shared\auxdata\delta_interpolator.h"
#include "..\shared\g_assert.h"
#include "..\shared\auth_product.h"
// One gauge of the three (0x8C bytes, table 0xAF5CA0): gd_gauge.cpp's with the sound name of its low warning
class UIPowerGaugeClass : public TrailGaugeGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIPowerGaugeClass(RenderObjClass *mesh, const char *format, const char *sound);	// 0x5DCB50 (inlined into 0x5DBFC0)  // NAME 1 [mesh, format, sound]: what 0x5DBFC0 passes
	virtual ~UIPowerGaugeClass();						// +0x00 0x5DCBC0 (deleting 0x5DCB90)
	virtual bool Set_Value(float value);				// +0xA4 0x5DCBE0 (the base's; 'TALK_%s_ENERGY_LOW_1.wav' when it drops under 2)  // NAME 1 [value]: the base's

	const char *Sound;									// +0x88 ('PROPULSION', 'SHIELD')  // NAME 2: what the warning's name takes
};

// (two inline layers, AuxDataName and the root copy in the inner one: ui_mfd.cpp's form, the find 0x5BF2B0 out of line)
template<class C> inline C *Unknown_Power_Field(DeltaContainer *const &root, const char *name) { AuxDataName n(name); DeltaContainer *r = root; C *t = Unknown_Find_Field<C>(r, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Power_Find(DeltaContainer *const &root, const char *name) { return(Unknown_Power_Field<C>(root, name)); }	// NAME 1: neutral (D10)

// @0x5DBFC0  (Parent through the plain base first; GenericNode() out of line by the inline_depth at the file's end)
UIPowerClass::UIPowerClass(RenderObjClass *mesh, LayerClass *layer, InterfaceClass *parent, int arg) :
	UnknownPowerParentClass(parent),
	EnergyGauge(NULL),
	ShieldGauge(NULL),
	HullGauge(NULL)
{
	EnergyGauge = new UIPowerGaugeClass(mesh, "reactor_bar%d", "PROPULSION");
	EnergyGauge->Unknown_28(layer);
	GadgetClass::Set_Tooltip(EnergyGauge, "UI_POWER", "ENERGY", 0, 0);
	EnergyGauge->Unknown_65FFA0(arg);
	if (EnergyGauge->Exists) {
		Parent->Unknown64.Unknown_6870E0(EnergyGauge, 0, Parent->Get_View(), true, true);
	}
	ShieldGauge = new UIPowerGaugeClass(mesh, "shield_bar%d", "SHIELD");
	ShieldGauge->Unknown_28(layer);
	GadgetClass::Set_Tooltip(ShieldGauge, "UI_POWER", "SHIELDS", 0, 0);
	ShieldGauge->Unknown_65FFA0(arg);
	if (ShieldGauge->Exists) {
		Parent->Unknown64.Unknown_6870E0(ShieldGauge, 0, Parent->View, true, true);
	}
	HullGauge = new UIPowerGaugeClass(mesh, "hull_bar%d", "SHIELD");
	HullGauge->Unknown_28(layer);
	GadgetClass::Set_Tooltip(HullGauge, "UI_POWER", "HULL", 0, 0);
	HullGauge->Unknown_65FFA0(arg);
	if (HullGauge->Exists) {
		Parent->Unknown64.Unknown_6870E0(HullGauge, 0, Parent->View, true, true);
	}
}

// @0x5DC2C0
UIPowerClass::~UIPowerClass()
{
	GadgetClass::Destroy(ShieldGauge);
	GadgetClass::Destroy(EnergyGauge);
	GadgetClass::Destroy(HullGauge);
}

// @0x5DC390
void UIPowerClass::Unknown_5DC390()
{
	if (ShieldGauge->Exists) {
		ShieldGauge->Force_Value(0.0f);
		GadgetClass::Set_Tooltip(ShieldGauge, "UI_POWER", "NO_SHIELDS");
	}
	if (EnergyGauge->Exists) {
		EnergyGauge->Force_Value(0.0f);
		GadgetClass::Set_Tooltip(EnergyGauge, "UI_POWER", "NO_ENERGY");
	}
	if (HullGauge->Exists) {
		HullGauge->Force_Value(0.0f);
		GadgetClass::Set_Tooltip(HullGauge, "UI_POWER", "NO_HULL");
	}
}

// @0x5DC450
void UIPowerClass::Unknown_Watch_0C()
{
	Unknown_5DC390();
}

// @0x5DC470
void UIPowerClass::Unknown_08(ClientObjectClass *object)
{
	Unknown_5DC390();
	UnknownObjectWatchClass::Unknown_08(object);
}

// @0x5DC4A0
void UIPowerClass::Unknown_10()
{
	if (!Object) return;
	DeltaContainer *aux = (DeltaContainer *)Object->AuxData;
	DurationClass time((unsigned)Object->Client->Unknown10BC);
	if (EnergyGauge->Exists) {
		DeltaInterpolator *percent = Unknown_Power_Find<DeltaInterpolator>(aux, "EnergyPercent");
		float value = percent ? percent->Get_Value(time) : 0.0f;
		EnergyGauge->Set_Value(value);
		if (EnergyGauge->Is_Unknown2C_08()) {
			DeltaTemplateClass<float> *field = Unknown_Power_Find<DeltaTemplateClass<float> >(aux, "MaxEnergyPower");
			float max = field ? field->Value : 0.0;
			float cur = max * value;
			if (cur < 0.0f) {
				cur = 0.0f;
			} else if (cur > max) {
				cur = max;
			}
			GadgetClass::Set_Tooltip(EnergyGauge, "UI_POWER", "ENERGY", (int)cur, (int)max);
		}
	}
	if (ShieldGauge->Exists) {
		DeltaInterpolator *percent = Unknown_Power_Find<DeltaInterpolator>(aux, "ShieldPercent");
		float value = percent ? percent->Get_Value(time) : 0.0f;
		ShieldGauge->Set_Value(value);
		if (ShieldGauge->Is_Unknown2C_08()) {
			DeltaTemplateClass<float> *field = Unknown_Power_Field<DeltaTemplateClass<float> >(aux, "MaxShieldPower");
			float max; if (field && (max = field->Value) > 0.0f) {

				float cur = max * value;
				if (cur < 0.0f) {
					cur = 0.0f;
				} else if (cur > max) {
					cur = max;
				}
				GadgetClass::Set_Tooltip(ShieldGauge, "UI_POWER", "SHIELDS", (int)cur, (int)max);
			} else {
				GadgetClass::Set_Tooltip(ShieldGauge, "UI_POWER", "NO_SHIELDS");
			}
		}
	}
	if (HullGauge->Exists) {
		DeltaTemplateClass<float> *max_field = Unknown_Power_Field<DeltaTemplateClass<float> >(aux, "MaxHullPoints");
		DeltaTemplateClass<float> *field = Unknown_Power_Field<DeltaTemplateClass<float> >(aux, "HullPoints");
		float max = max_field ? max_field->Value : 0.0f;
		float cur = field ? field->Value : 0.0f;
		if (max < 1.0f) {
			max = 0.0f;
			cur = 0.0f;
			HullGauge->Set_Value(0.0f);
		} else {
			if (cur < 0.0f) {
				cur = 0.0f;
			} else if (cur > max) {
				cur = max;
			}
			HullGauge->Set_Value(cur / max);
		}
		if (HullGauge->Is_Unknown2C_08()) {
			GadgetClass::Set_Tooltip(HullGauge, "UI_POWER", "HULL", (int)cur, (int)max);
		}
	}
}

// @0x5DCAE0
void UIPowerClass::Unknown_5DCAE0()
{
	if (EnergyGauge->Exists) Parent->Unknown64.Unknown_687150(EnergyGauge);
	if (ShieldGauge->Exists) Parent->Unknown64.Unknown_687150(ShieldGauge);
	if (HullGauge->Exists) Parent->Unknown64.Unknown_687150(HullGauge);
}

// @0x5DCB50
UIPowerGaugeClass::UIPowerGaugeClass(RenderObjClass *mesh, const char *format, const char *sound) :
	TrailGaugeGadgetClass(mesh, format),
	Sound(sound)
{
}

// @0x5DCBC0
UIPowerGaugeClass::~UIPowerGaugeClass()
{
}

// @0x5DCBE0
bool UIPowerGaugeClass::Set_Value(float value)
{
	float old = Get_Value();
	if (TrailGaugeGadgetClass::Set_Value(value)) {
		if (Current.Value < 2.0f && old >= 2.0f) {
			char name[256];
			sprintf(name, "TALK_%s_ENERGY_LOW_1.wav", Sound);
			SoundLibrary::Play_2D_Sound(name, 2);
		}
		return(false);
	}
	return(true);
}

/*
**	Notes (O4, 2026-11-11)
**
**	The client's object 0x5DBF70-0x5DCCDF: permstr.h's PermStringInit static 0x5DBF70 (destructor 0x5DBFA0) opens it,
**	after the adjustor thunks 0x5DBF50 (-4) / 0x5DBF60 (-0x38) of Unknown5DB670InterfaceClass's deleting destructor;
**	UIProcessClass's 0x5DCCE0 follows. No __FILE__ of its own (only the headers' asserts: delta_interpolator.h l.54,
**	auxdata.h l.155): the file name is NAME 1, from the tooltips' section UI_POWER and the link order. Its twin is
**	ui_experience.cpp (the same watcher shape, the same three-bar constructor).
**	0x5DC4A0 is exact: use the existing two-layer lookup for MaxEnergyPower and one-layer field
**	lookups for later fields; capture the shield maximum once at its comparison, as retail does.
*/
#pragma inline_depth(2)	// templates take the file end's inline state: the watcher's DataNode<> calls GenericNode() 0x48A060 out of line in 0x5DBFC0, as the client
