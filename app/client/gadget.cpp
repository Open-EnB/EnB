// Earth & Beyond client (reconstruction): app\client\gadget.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "gadget.h"
#include "..\kernel\kernel.h"
#include "sclient.h"
#include "uiview.h"
#include "control_keys_manager.h"
#include "rendobj.h"
#include "ww3d.h"
#include "aabox.h"
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
	void Unknown_6B3F50(const SphereClass &sphere);		// 0x6B3F50 (each frame: the clone's bounding sphere, slot +0xF0)  // NAME 1 [sphere]: neutral

	char Unknown00[0x3C];
};

// the input (input.cpp's neighbourhood, cdecl)
bool Unknown_42EA80(KernelEventClass *event);						// 0x42EA80 (the queue's next event, kept)  // NAME 1 [event]: neutral
void Unknown_42E8A0(KernelEventClass *event);						// 0x42E8A0 (taken)  // NAME 1 [event]: neutral
void Unknown_42ECF0(float *x, float *y);							// 0x42ECF0 (the pointer's position, 0..1)  // NAME 1 [x, y]: as scriptedmovie.cpp's
bool Unknown_42EC60(int button, int arg);							// 0x42EC60 (a button down)  // NAME 1 [button, arg]: neutral
unsigned int Unknown_Get_Ticks();										// (unknown_root.h's)
static KernelEventClass Unknown_BE5EB0;									// 0xBE5EB0 (0x65F780: the last event)  // NAME 1: neutral (D10)

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

const char *GadgetClass::Get_Unknown14()
{
	return(Unknown14);
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

bool GadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Owner) {
		Owner->Unknown_44(mask, event, this);
	}
	event->Key = (unsigned short)Id;
	Unknown_BE5F34 = mask;
	Unknown_BE5F2C = this;
	return(true);
}

bool GadgetClass::Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask)
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

// NOT EXACT: shape (written from the client's 0x65F780 for the proof-of-concept client: the input's next event to the
// manager's gadgets: the held gadget first, then every gadget under the pointer)
KernelEventClass GadgetClass::Process_Input(GadgetListManagerClass *manager)
{
	KernelEventClass event;
	Unknown_BE5F2C = NULL;
	Unknown_BE5F34 = 0;
	bool got = Unknown_42EA80(&event);
	while (got) {
		Unknown_42E8A0(&event);
		if (event.Key != 0x10 && event.Key != 0x11 && event.Key != 0x12) {
			break;
		}
		got = Unknown_42EA80(&event);
	}
	if (event.Key == 0) {
		event.Key = 0;
		event.Flags = 0;
		Unknown_42ECF0(&event.Unknown04, &event.Unknown08);
		event.Unknown0C = 0;
	}
	Unknown_BE5EB0 = event;
	float x = (event.Unknown04 < 0.0f) ? 0.0f : ((event.Unknown04 > 1.0f) ? 1.0f : event.Unknown04);
	float y = (event.Unknown08 < 0.0f) ? 0.0f : ((event.Unknown08 > 1.0f) ? 1.0f : event.Unknown08);

	unsigned int mask = 0;
	bool buttons = false;
	if (event.Key == 0) {
		buttons = true;
	} else {
		if (event.Key == 1) {
			mask = (event.Flags & 0x1000) ? 4 : 1;
		} else if (event.Key == 2) {
			mask = (event.Flags & 0x1000) ? 0x40 : 0x10;
		} else if (event.Key == 0x101 || event.Key == 0x102) {
			mask = 0x1000;
		}
		if (mask == 0) {
			mask = 0x100;
		}
		if (event.Flags & 0x100) {
			mask |= 0x200;
		}
		if (event.Flags & 0x200) {
			mask |= 0x400;
		}
		if (event.Flags & 0x400) {
			mask |= 0x800;
		}
		if (event.Key == 1 || event.Key == 2 || event.Key == 0x10 || event.Key == 0x11 || event.Key == 0x12) {
			buttons = true;
		}
	}
	if (buttons) {
		mask |= Unknown_42EC60(1, 0) ? 2 : 8;
		mask |= Unknown_42EC60(2, 0) ? 0x20 : 0x80;
	}

	GadgetClass *handled = NULL;
	GadgetClass *held = NULL;
	bool over = false;
	bool front = false;
	GadgetClass **sticky = NULL;
	if ((mask & 5) && Unknown_BE5F24) {
		sticky = &Unknown_BE5F24;
	} else if ((mask & 0x50) && Unknown_BE5F28) {
		sticky = &Unknown_BE5F28;
	} else if ((mask & 2) && Unknown_BE5F24) {
		sticky = &Unknown_BE5F24;
	} else if ((mask & 0x20) && Unknown_BE5F28) {
		sticky = &Unknown_BE5F28;
	} else if ((mask & 8) && Unknown_BE5F24) {
		sticky = &Unknown_BE5F24;
	} else if ((mask & 0x80) && Unknown_BE5F28) {
		sticky = &Unknown_BE5F28;
	}
	if (sticky && *sticky) {
		held = *sticky;
		held->Unknown_30(x, y);
		(*sticky)->Unknown_34(&event, x, y, mask);
		if (*sticky) {
			handled = *sticky;
			front = true;
			over = true;
		}
	}

	static std::vector<GadgetClass *> _under;
	_under.clear();
	GadgetClass *last_over = Unknown_BE5F30;
	Unknown_BE5F30 = NULL;
	GadgetListIteratorClass it(manager);
	GadgetClass *gadget;
	while ((gadget = it.Get()) != NULL) {
		++it;
		if (gadget->Flash) {
			gadget->Unknown_6609A0();
		}
		if (gadget == held) {
			continue;
		}
		if (gadget->Unknown_30(x, y)) {
			_under.push_back(gadget);
			gadget->Unknown2C_02 = true;
			if (!over) {
				over = true;
				gadget->Unknown2C_04 = true;
				Unknown_BE5F30 = gadget;
			} else {
				gadget->Unknown2C_04 = false;
			}
			gadget->Unknown2C_03 = false;
			if (!handled && gadget->Unknown2C_05) {
				if (!front) {
					gadget->Unknown2C_03 = true;
					front = true;
					if (last_over == gadget && !gadget->Flash) {
						gadget->Unknown_6603A0(false, NULL);
					}
				}
				if (gadget->Unknown_34(&event, x, y, mask)) {
					handled = gadget;
					if (gadget->Flash) {
						gadget->Unknown_6607D0();
					} else {
						gadget->Unknown_6603A0(false, NULL);
					}
				}
			}
		} else {
			gadget->Unknown2C_02 = false;
			if (!handled && (gadget->Mask & mask & 0x100) && gadget->Unknown2C_05 && gadget->Unknown_34(&event, x, y, mask)) {
				handled = gadget;
			}
		}
	}
	manager->Unknown3C.swap(_under);
	if (handled && (mask & 0x1111) && manager->Unknown_661610(handled->Unknown08)) {
		InterfaceClass *iface = handled->Unknown08;
		Get_Signal_Map().emit((void *)iface, GadgetGroupToFrontEvent(iface));
	}
	return(event);
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

void GadgetClass::Unknown_64(AABoxClass &box)
{
	box.Center.X = 0; box.Center.Y = 0; box.Center.Z = 0; box.Extent.X = 0; box.Extent.Y = 0; box.Extent.Z = 0;
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















// NOT EXACT: shape (written from the client's 0x6603A0 for the proof-of-concept client: the flash's clone of the model
// slot +0x24 gives, in the model's scene, with the click indicator when asked)
void GadgetClass::Unknown_6603A0(bool indicator, UIViewClass *view)
{
	float frame = 0.0f;
	Vector3 position;
	RenderObjClass *model = Unknown2C_0A ? NULL : Unknown_24(&position, &frame, indicator);
	if (Flash) {
		if (model == Flash->Model) {
			return;
		}
		Flash->Clone->Remove();
		delete Flash;
		Flash = NULL;
		Get_Signal_Map().emit((GadgetClass *)NULL, FlashingGadgetEvent(this));
		G_ASSERT(!Flash);
	}
	if (model) {
		SceneClass *scene = model->Peek_Scene();
		GadgetSortClass sort(Unknown_497300(model));
		Flash = new GadgetFlashStruct;
		Flash->Clone = model->Clone();
		model->Add_Ref();
		if (Flash->Model) {
			Flash->Model->Release_Ref();
		}
		Flash->Model = model;
		if (!Unknown_496620(Flash->Clone) && !indicator) {
			Unknown_6607D0();
		} else {
			Unknown_4967F0(Flash->Clone, -1, Flash->Model);
			Unknown_496AE0(Flash->Clone, 0);
			HAnimClass *anim = model->Peek_Animation();
			if (anim) {
				Flash->Clone->Set_Animation(anim, frame, 0);
			}
			Flash->Clone->Add(scene);
			if (indicator) {
				Flash->Indicator = new UserClickIndicatorClass;
				if (!Flash->Indicator->Unknown_6B3D00(view)) {
					delete Flash->Indicator;
					Flash->Indicator = NULL;
				}
			}
			Get_Signal_Map().emit((GadgetClass *)NULL, FlashingGadgetEvent(this));
			Unknown_6609A0();
		}
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

bool GadgetClass::Is_Unknown2C_00()
{
	return(Unknown2C_00);
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


































































































// NOT EXACT: shape (written from the client's 0x6609A0 for the proof-of-concept client: the flash's frame, a
// one-second pulse unless the gadget is held under the pointer)
void GadgetClass::Unknown_6609A0()
{
	FAIL_IF(!Flash) return;
	float alpha = 1.0f;
	bool pulse = true;
	if (!Unknown2C_02 || !Unknown2C_03) {
		if (Flash->Unknown0C) {
			if (!Unknown2C_0B) {
				Unknown_6607D0();
				return;
			}
			Flash->Unknown0C = false;
			return;
		}
		float t = (float)(Unknown_Get_Ticks() % 1000) * 0.001f - 0.5f;
		alpha = (t >= 0.0f) ? (t + t) : (t * -2.0f);
	} else if (!Flash->Unknown0C) {
		Flash->Unknown0C = true;
	} else {
		pulse = false;
	}
	float frame = 0.0f;
	Vector3 position;
	RenderObjClass *model = Unknown_24(&position, &frame, Flash->Indicator != NULL);
	if (model != Flash->Model || Unknown_4960C0(model, 0) != Unknown_4960C0(Flash->Clone, 0)) {
		Unknown_6607D0();
		if (!model) {
			return;
		}
		Unknown_6603A0(false, NULL);
		if (!Flash) {
			return;
		}
	}
	Vector3 current = Flash->Clone->Get_Position();
	if (position.X != current.X || position.Y != current.Y || position.Z != current.Z) {
		Flash->Clone->Set_Position(position);
	}
	if (Flash->Indicator) {
		Flash->Indicator->Unknown_6B3F50(Flash->Clone->Get_Bounding_Sphere());
	}
	if (pulse && !Unknown_496110(Flash->Clone, alpha, alpha, alpha)) {
		Flash->Clone->Set_Hidden(alpha < 0.5f);
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
**	Set_Tooltip_Name's buffer is 36 bytes (the client's frame), 31 of them used. The flash's RenderObjClass slots, as
**	the client calls them: +0x9C Set_Animation, +0xA4 Peek_Animation, +0xF0 Get_Bounding_Sphere, +0x184 Set_Hidden.
*/
