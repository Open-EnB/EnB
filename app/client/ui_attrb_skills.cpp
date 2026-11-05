// Earth & Beyond client (reconstruction): app\client\ui_attrb_skills.cpp, the character info window's attributes tab (the
// client's file per its asserts, 'e:\build\beta\G\app\Client\ui_attrb_skills.cpp'). Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "..\shared\unknown_debug.h"
#include "ui_attrb_skills.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// intrlist.cpp's list (InterfaceClass +0x58; intrface.h declares it only under __INTRFACE_MEMBERS), as far as this file calls it
class InterfaceUnknown58Class  // NAME 1: neutral (D10)
{
public:
	void Unknown_686DD0(RenderObjClass *robj);			// 0x686DD0
};

// 0x549640
UIAttrbSkillsClass::UIAttrbSkillsClass(SClient *client) :
	Unknown62D1F0Class(client),
	UnknownD4(NULL),
	UnknownD8(NULL),
	UnknownDC(NULL),
	UnknownE0(NULL),
	UnknownE4(false),
	UnknownE8(0),
	UnknownEC(-1),
	UnknownF0(0),
	UnknownF4(0),
	UnknownF8(0)
{
}

// 0x549830
UIAttrbSkillsClass::~UIAttrbSkillsClass()
{
	((InterfaceUnknown58Class *)&Unknown4C[0x58 - 0x4C])->Unknown_686DD0(UnknownBC.Peek());	// (InterfaceClass +0x58)
	Unknown_54B050();
	UnknownFC = NULL;
	if (UnknownD4) {
		UnknownD4->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
		UnknownD4 = NULL;
	}
	if (UnknownD8) {
		UnknownD8->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
		UnknownD8 = NULL;
	}
	UnknownDC = NULL;
	UnknownE0 = NULL;
	Unknown_BE67C4->Remove(UnknownBC.Peek());
	Unknown_BE67BC->Unload("fa_att.ini");
}

// 0x54A3C0
void UIAttrbSkillsClass::Unknown_5C()
{
	if (Active) {
		UnknownFC->Unknown_5C();
		UnknownC0->Unknown_5C();
		Unknown62D1F0Class::Unknown_5C();
	}
}

// 0x54A400
void UIAttrbSkillsClass::Unknown_60()
{
	Unknown62D1F0Class::Unknown_60();
	Unknown_54B140();
	int value = UnknownE4 ? UnknownD4->Value : UnknownD8->Value;
	if (UnknownE8 != value) {
		UnknownE8 = value;
		Unknown_54A900();
	}
	Unknown_54A640(false);
	Unknown_54AB20();
}

// 0x54A5B0
void UIAttrbSkillsClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (view) {
		Layer.Set_Scene((SceneClass *)view->Unknown_50());
		Layer.Set_Camera((CameraClass *)view->Unknown_54());
	} else {
		Layer.Set_Scene(NULL);
		Layer.Set_Camera(NULL);
	}
	UnknownFC->Unknown_7C(view, false);
	UnknownC0->Unknown_7C(view, false);
	Unknown62D1F0Class::Unknown_7C(view, arg);
}

/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x549640-~0x54B75F: the constructor 0x549640 first, right after ui_attrb.cpp's PermStringInitClass
**	static (0x5495F0 / atexit 0x549620, the end of ui_attrb.obj); its destructor 0x549830 (deleting 0x549800) and the
**	slots 0x549AA0.. carry ui_attrb_skills.cpp's asserts; the adjustor thunks 0x54B740 (-4) / 0x54B750 (-0x38) of the
**	deleting destructor close it. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's first tab. The
**	constructor's byte store to +0xC4 is the vector's allocator copied from an uninitialised temporary (VC6's
**	vector(const _A& = _A())), not an argument.
*/
