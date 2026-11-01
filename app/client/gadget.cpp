// Earth & Beyond client (reconstruction): app\client\gadget.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "gadget.h"
#include "sclient.h"
#include "uiview.h"
#include "control_keys_manager.h"
#include "rendobj.h"
#include "ww3d.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"

GadgetClass *Unknown_BE5F24 = NULL;
GadgetClass *Unknown_BE5F28 = NULL;
GadgetClass *Unknown_BE5F2C = NULL;		// 0xBE5F2C (slot +0x2C: the last gadget an event went to)  // NAME 1: neutral (D10)
GadgetClass *Unknown_BE5F30 = NULL;		// 0xBE5F30 (0x65F780: the gadget under the pointer)  // NAME 1: neutral (D10)
unsigned int Unknown_BE5F34 = 0;		// 0xBE5F34 (slot +0x2C: its mask)  // NAME 1: neutral (D10)
static char Unknown_BE5EC4[32];			// 0xBE5EC4 (0x65F340: the last tooltip name)  // NAME 1: neutral (D10)
static int Unknown_BE5F38;				// 0xBE5F38 (0x65F340: its count)  // NAME 1: neutral (D10)
static int Unknown_B8C9E0 = 0;			// 0xB8C9E0 (the gadgets' ids)  // NAME 1: neutral (D10)

void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line

// user_click_indicator.cpp's indicator (0x3C bytes), as far as the flash uses it
class UserClickIndicatorClass  // NAME 1: neutral (D10)
{
public:
	UserClickIndicatorClass();							// 0x6B3A90
	~UserClickIndicatorClass();							// 0x6B3C20
	bool Unknown_6B3D00(UIViewClass *view);				// 0x6B3D00 (the flash's view)  // NAME 1 [view]: neutral
	void Unknown_6B3F50(int arg);						// 0x6B3F50 (each frame: the clone's slot +0xF0)  // NAME 1 [arg]: neutral

	char Unknown00[0x3C];
};

// ro_funcs.cpp's (cdecl)
int Unknown_4960C0(RenderObjClass *robj, int arg);					// 0x4960C0  // NAME 1 [robj, arg]: neutral
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (a model's highlight)  // NAME 1 [robj, a, b, c]: as ui_avsel.cpp's
bool Unknown_496620(RenderObjClass *robj);							// 0x496620  // NAME 1 [robj]: neutral
void Unknown_4967F0(RenderObjClass *robj, int arg, RenderObjClass *model);	// 0x4967F0  // NAME 1 [robj, arg, model]: neutral
void Unknown_496AE0(RenderObjClass *robj, int unknown);				// 0x496AE0 (effect.ini's ResetMappers)  // NAME 1 [robj, unknown]: as eff_o.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300  // NAME 1 [robj]: as ui_avatr.cpp's

// local_interface_sort.cpp's sort object over a model (constructor 0x6E1730, destructor 0x6E18B0)
class GadgetSortClass  // NAME 1: neutral (D10)
{
public:
	GadgetSortClass(const RefCountPtr<RenderObjClass> &robj);	// 0x6E1730  // NAME 1 [robj]: neutral
	~GadgetSortClass();									// 0x6E18B0

	char Unknown00[8];
};

SignalMap &GadgetClass::Get_Signal_Map()
{
	static SignalMap _map;
	return(_map);
}

// NOT EXACT: order (0.77; the client stores +0x10 and the table between the +0x2C load and its and / or)
GadgetClass::GadgetClass(unsigned int mask) :
	Id(Unknown_B8C9E0++),
	Unknown08(NULL),
	Unknown0C(0),
	Unknown14(NULL),
	Owner(NULL),
	Unknown1C(0),
	Flash(NULL),
	Unknown24(NULL),
	Unknown28(0)
{
	Mask = mask;
	Unknown2C_00 = false;
	InList = false;
	Unknown2C_02 = false;
	Unknown2C_03 = false;
	Unknown2C_04 = false;
	Unknown2C_05 = true;
	Unknown2C_06 = false;
	Unknown2C_07 = false;
	Unknown2C_08 = false;
	Destroyed = false;
	Unknown2C_0A = false;
	Unknown2C_0B = false;
	FirstSortGroup = false;
	LastSortGroup = false;
}

























































GadgetClass::~GadgetClass()
{
	G_ASSERT(Destroyed);
	delete Unknown14;
	Unknown14 = NULL;
	delete Unknown24;
	Unknown24 = NULL;
}



void GadgetClass::Destroy(GadgetClass *del_me)
{
	FAIL_IF(!del_me) return;
	Get_Signal_Map().emit(del_me, DestroyGadgetEvent());
	del_me->Unknown_40(false);
	del_me->Destroyed = true;
	del_me->Unknown_04();
	delete del_me;
}

void GadgetClass::Unknown_04()
{
}































void GadgetClass::Set_Tooltip(GadgetClass *gadget, const char *section, const char *entry, ...)
{
	if (!section) {
		if (gadget->Unknown14) {
			delete gadget->Unknown14;
			gadget->Unknown14 = NULL;
			gadget->Unknown2C_07 = true;
		}
		return;
	}
	if (!gadget->Unknown24) {
		Set_Tooltip_Name(gadget, "%s_%s", section, entry);
	}
	static INIClass _ini;
	if (!_ini.Is_Loaded()) {
		_ini.Load("tooltip.ini");
	}
	enum { _max = 1023 };
	char def[_max + 1];
	def[_max] = '\0';
	sprintf(def, "ToolTip [%s]:%s not found!", section, entry);
	FAIL_IF(def[_max]) {
		def[_max] = '\0';
	}
	char *text;
	char *newtip = _ini.Get_Alloc_String(section, entry, def);
	if (newtip) {
		va_list args;
		va_start(args, entry);
		_vsnprintf(def, _max, newtip, args);




		G_ASSERT(!def[_max]);
		if (def[0] != '\0' && strcmp(def, newtip) != 0) {
			text = nstrdup(def);
		} else {
			text = nstrdup(newtip);
		}
		free(newtip);
	} else {



		G_ASSERT(newtip);
		text = nstrdup(def);
	}
	if (gadget->Unknown14 && strcmp(gadget->Unknown14, text) == 0) {
		delete text;
		return;
	}
	delete gadget->Unknown14;
	gadget->Unknown14 = text;
	gadget->Unknown2C_07 = true;
}

void GadgetClass::Set_Unknown14(const char *text)
{
	if (Unknown14) {
		delete Unknown14;
		Unknown14 = NULL;
	}
	if (text && *text) {
		Unknown14 = nstrdup(text);
	}
	Unknown2C_07 = true;
}

void GadgetClass::Set_Tooltip_Name(GadgetClass *gadget, const char *format, ...)
{
	if (format && *format) {
		if (gadget->Unknown24) {
			delete gadget->Unknown24;
		}
		char name[36];
		va_list args;
		va_start(args, format);
		_vsnprintf(name, 31, format, args);
		name[31] = '\0';
		if (strcmp(Unknown_BE5EC4, name) != 0) {
			Unknown_BE5F38 = 0;
			strcpy(Unknown_BE5EC4, name);
		}
		char suffix[8];
		sprintf(suffix, "_%d", Unknown_BE5F38++);
		strcat(name, suffix);
		gadget->Unknown24 = nstrdup(name);
	}
}

bool GadgetClass::Unknown_2C(GadgetEventStruct *event, float x, float y, unsigned int mask)
{
	if (Owner) {
		Owner->Unknown_44(mask, event, this);
	}
	event->Type = (short)Id;
	Unknown_BE5F34 = mask;
	Unknown_BE5F2C = this;
	return(true);
}

bool GadgetClass::Unknown_34(GadgetEventStruct *event, float x, float y, unsigned int mask)
{
	mask &= Mask;
	if (Unknown28 > 0.0f) {
		Unknown28 -= WW3D::Get_Frame_Time() * 0.001f;
		Unknown28 = (0.0f > Unknown28) ? 0.0f : Unknown28;
	}
	if (mask & 1) {
		Toggle_Unknown2C_00();
	}
	if (mask) {
		return(Unknown_2C(event, x, y, mask));
	}
	return(false);
}

































































































GadgetClass *GadgetClass::Unknown_65F5F0(GadgetListManagerClass *manager, bool a, bool b)
{
	FAIL_IF(!manager) return(NULL);
	return(manager->Unknown_661AA0(a, b));
}

void GadgetClass::Unknown_40(bool on)
{
	Unknown2C_05 = on;
	if (!on) {
		if (Unknown_BE5F24 == this) {
			Unknown_BE5F24 = NULL;
		}
		if (Unknown_BE5F28 == this) {
			Unknown_BE5F28 = NULL;
		}
		Unknown_6607D0();
	}
}

void GadgetClass::Unknown_65FFA0(int arg)
{
	Unknown0C = arg;
}

void GadgetClass::Unknown_65FFC0(int arg, GadgetClass *gadget)
{
	if (!gadget) {
		gadget = this;
	}
	Unknown0C = gadget->Unknown0C + arg;
}


















































































































































































































































































void GadgetClass::Set_First_Sort_Group(bool on)
{
	FAIL_IF(InList) return;
	FAIL_IF(LastSortGroup) return;
	FirstSortGroup = on;
}

void GadgetClass::Set_Last_Sort_Group(bool on)
{
	FAIL_IF(InList) return;
	FAIL_IF(FirstSortGroup) return;
	LastSortGroup = on;
}

void GadgetClass::Unknown_54(GadgetListManagerClass *manager, InterfaceClass *iface)
{
	FAIL_IF(!manager) return;
	Unknown08 = iface;
	manager->Unknown_661700(this);
	InList = true;
}

void GadgetClass::Unknown_58(GadgetListManagerClass *manager)
{
	FAIL_IF(!manager) return;
	manager->Unknown_6617E0(this);
	Unknown08 = NULL;
	InList = false;
	if (Unknown_BE5F24 == this) {
		Unknown_BE5F24 = NULL;
	}
	if (Unknown_BE5F28 == this) {
		Unknown_BE5F28 = NULL;
	}
	if (Unknown_BE5F2C == this) {
		Unknown_BE5F2C = NULL;
		Unknown_BE5F34 = 0;
	}
}

bool GadgetClass::Unknown_68()
{
	GadgetClass **sticky;
	if (Unknown_BE5F34 & 0x10) {
		sticky = &Unknown_BE5F28;
	} else if (Unknown_BE5F34 & 0x0F) {
		sticky = &Unknown_BE5F24;
	} else if (Unknown_BE5F34 & 0xF0) {
		sticky = &Unknown_BE5F28;
	} else {
		return(false);
	}
	if (*sticky != NULL) {
		G_ASSERT(*sticky == NULL);
		*sticky = NULL;
		return(false);
	}
	*sticky = this;
	return(true);
}






void GadgetClass::Unknown_6C()
{
	GadgetClass **sticky;
	if (Unknown_BE5F34 & 0x40) {
		sticky = &Unknown_BE5F28;
	} else if (Unknown_BE5F34 & 0x0F) {
		sticky = &Unknown_BE5F24;
	} else if (Unknown_BE5F34 & 0xF0) {
		sticky = &Unknown_BE5F28;
	} else {
		return;
	}
	if (*sticky == this) {
		*sticky = NULL;
	} else if (*sticky != NULL) {
		G_ASSERT(*sticky == this);
		Unknown_Debug_Print("warning: tried to release sticky, but belonged to another gadget.");
	}
}

void GadgetClass::Unknown_70()
{
	if (Unknown_BE5F24) {
		Unknown_BE5F24->Unknown_6C();
	}
	if (Unknown_BE5F28) {
		Unknown_BE5F28->Unknown_6C();
	}
}

GadgetUnknown64Struct GadgetClass::Unknown_64()
{
	return(GadgetUnknown64Struct());
}

GadgetFlashStruct::~GadgetFlashStruct()
{
	REF_PTR_RELEASE(Model);
	REF_PTR_RELEASE(Clone);
	if (Indicator) {
		delete Indicator;
		Indicator = NULL;
	}
}

void GadgetClass::Unknown_6607D0()
{
	if (Flash) {
		Flash->Clone->Remove();
		delete Flash;
		Flash = NULL;
		Get_Signal_Map().emit((GadgetClass *)NULL, FlashingGadgetEvent(this));
	}
}

void GadgetClass::Unknown_6608F0()
{
	if (Flash && Flash->Clone) {
		RenderObjClass *model = Flash->Model;
		if (model) {
			Flash->Clone->Set_Transform(model->Get_Transform());
		}
	}
}

void GadgetClass::Toggle_Unknown2C_00()
{
	if (Unknown28 > 0.0f) {
		Unknown2C_00 = true;
		Unknown28 = 0.0f;
	} else {
		Unknown2C_00 = false;
		Unknown28 = 0.5f;
	}
}


/*
**	Reconstruction notes
**
**	The client's window (app-functions.json, __FILE__ 0xB8C9F4): the function-local SignalMap 0x65ECE0 and the
**	constructor 0x65ED50 first (no assert; the gadgets' id counter 0xB8C9E0), the destructor 0x65EE60 (line 151,
**	its deleting destructor 0x65EDD0 inlines it), Destroy 0x65EED0 (162), slot +0x04 0x65EF90, the tooltips
**	0x65EFB0 (tooltip.ini, 225-249), 0x65F2D0, 0x65F320, 0x65F340, slots +0x2C 0x65F4B0, +0x34 0x65F500, 0x65F5F0
**	(420), slot +0x40 0x65F640, the input 0x65F780, 0x65FFA0-0x660070 (the sort groups, 726-734), slots +0x54 /
**	+0x58 (740 / 748), the held gadgets +0x68 / +0x6C (777 / 805), +0x70, +0x64, the flash 0x660320-0x6609A0
**	(864, 1036). Functions VC6 inlines into callers earlier in the file (0x6607D0 into 0x65F640, 0x660960 into
**	0x65F500) are ordinary functions defined later: VC6's /Ob2 inlines forward within the file.
**	Set_Tooltip_Name's buffer is 36 bytes (the client's frame), 31 of them used.
*/
