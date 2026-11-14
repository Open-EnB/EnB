// Earth & Beyond client (reconstruction): app\client\ui_interrupt.cpp, the scanner's interrupt progress display.
// Notes at the end of the file.
#include "..\shared\auxdata\permstr.h"
#include <string>
#include "ui_interrupt.h"
#include "ui_scan.h"
#include "gd_gauge.h"
#include "gadget.h"
#include "intrface.h"
#include "client.h"
#include "co.h"
#include "..\..\wwmath\wwmath.h"
#include "..\shared\g_assert.h"

// (duration.h's, not rebuilt there yet: interim placement, D137)
inline bool operator > (const DurationClass &a, const DurationClass &b) { return(a.Time > b.Time); }
inline DurationClass Millisecond(int ms) { return(DurationClass((__int64)ms)); }	// NAME 3: the assert's

// NOT EXACT: the client loads the bar into edx and the view into ecx for 0x6870E0 (ours: ecx / edx), twice
// @0x59E2C0
Unknown59E2C0Class::Unknown59E2C0Class(RenderObjClass *robj, LayerClass *layer, UIScanClass *owner, ClientObjectClass *object, int unknown) :
	Unknown59E2C0OwnerClass(owner),
	Unknown1C(NULL),
	Unknown20(NULL),
	StartTime(0),
	ActivationTime(0),
	Unknown38((InterruptStateType)0),
	Unknown3C(0),
	Unknown40(0)
{
	Unknown1C = new TrailGaugeGadgetClass(robj, "inter_bar%d");
	Unknown1C->Unknown_28(layer);
	GadgetClass::Set_Tooltip(Unknown1C, "UI_INTERRUPT", "INT_PROGRESS", 0);
	Unknown1C->Unknown_65FFA0(unknown);
	Owner->Unknown64.Unknown_6870E0(Unknown1C, 0, Owner->View, true, true);
	Unknown20 = new TrailGaugeGadgetClass(robj, "inter2_bar%d");
	Unknown20->Unknown_28(layer);
	GadgetClass::Set_Tooltip(Unknown20, "UI_INTERRUPT", "SKILL_PROGRESS", 0);
	Unknown20->Unknown_65FFA0(unknown);
	Owner->Unknown64.Unknown_6870E0(Unknown20, 0, Owner->View, true, true);
	Unknown_08(object);
}

// @0x59E540
Unknown59E2C0Class::~Unknown59E2C0Class()
{
	GadgetClass::Destroy(Unknown20);
	GadgetClass::Destroy(Unknown1C);
}

// @0x59E600
void Unknown59E2C0Class::Unknown_59E600()
{
	Unknown20->Force_Value(0);
	GadgetClass::Set_Tooltip(Unknown20, "UI_INTERRUPT", "SKILL_PROGRESS", 0);
	Unknown1C->Force_Value(0);
	GadgetClass::Set_Tooltip(Unknown1C, "UI_INTERRUPT", "INT_PROGRESS", 0);
}

// @0x59E680
void Unknown59E2C0Class::Unknown_Watch_0C()
{
	Unknown_59E600();
}

// @0x59E700
void Unknown59E2C0Class::Unknown_08(ClientObjectClass *object)
{
	Unknown_59E600();
	UnknownObjectWatchClass::Unknown_08(object);
}











































// @0x59E790
void Unknown59E2C0Class::Unknown_10()
{
	if (Object) {
		DurationClass now((unsigned int)Object->Client->Unknown10BC);
		Unknown1C->Set_Value(WWMath::Clamp(Unknown40, 0.0f, 1.0f));
		if (Unknown1C->Is_Unknown2C_08()) {
			GadgetClass::Set_Tooltip(Unknown1C, "UI_INTERRUPT", "INT_PROGRESS", (int)(Unknown40 * 100.0f));
		}
		if (Unknown38 == 1 || Unknown38 == 3) {
			G_ASSERT((ActivationTime - StartTime) > Millisecond(0));
			Unknown3C = WWMath::Clamp((float)((double)(now - StartTime).Time / (double)(ActivationTime - StartTime).Time), 0.0f, 1.0f);
		}
		Unknown20->Set_Value(Unknown3C);
		if (Unknown20->Is_Unknown2C_08()) {
			GadgetClass::Set_Tooltip(Unknown20, "UI_INTERRUPT", "SKILL_PROGRESS", (int)(Unknown3C * 100.0f));
		}
	}
}

// @0x59E970
void Unknown59E2C0Class::Unknown_59E970()
{
	Owner->Unknown64.Unknown_687150(Unknown1C);
	Owner->Unknown64.Unknown_687150(Unknown20);
}

/*
**	Notes (O15, 2026-11-13)
**
**	The client's object 0x59E2C0-0x59EA1A: the constructor 0x59E2C0, the deleting destructor 0x59E510, the destructor
**	0x59E540, 0x59E600, the slots +0x0C 0x59E680 / +0x08 0x59E700 / +0x10 0x59E790 (G_ASSERT l.125), 0x59E970, then the
**	guarded <string> initialiser 0x59E9B0 and permstr.h's PermStringInitClass static 0x59E9E0 (destructor 0x59EA10);
**	ui_scan.cpp's two time setters 0x60EC70 / 0x60ECA0 stay there. The owner (+0x18) is a plain base declared first: the
**	constructor stores it before the watcher base (ui_experience.cpp 0x58C450's finding). The skill's share is a double
**	division ((double) both times: two fild qword and fdivp; (float) casts give fidiv). The constructor's residue: the
**	client loads the bar into edx and the view into ecx for 0x6870E0 (ours: ecx / edx), twice.
*/
