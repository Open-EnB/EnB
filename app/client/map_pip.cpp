// Earth & Beyond client (reconstruction): the map picture-in-picture views' base (notes at the end)
#include "map_pip.h"
#include "sclient.h"
#include "intrface.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"

// starmap_pip_support.cpp's (not in the tree yet), as far as this file uses it. 0x118 bytes.
// PLACEMENT: provisional (D137): the client's starmap_pip_support.cpp gives it a file; move it there when that file comes in
class StarmapPipSupportClass : public RenderObjClass  // NAME 3: class-from-file (D24) for app\client\starmap_pip_support.cpp
{
public:
	StarmapPipSupportClass();							// 0x7C3AC0
	virtual RenderObjClass *Clone() const;
	virtual void Render(RenderInfoClass &rinfo);
	void Unknown_7C3C60(const Vector2 &arg1, float arg2);	// 0x7C3C60 (+0x10C, +0x114)  // NAME 1 [arg1, arg2]: neutral
	char Unknown_Pad[0x118 - sizeof(RenderObjClass)];
};

bool Unknown_4967F0(RenderObjClass *robj, int arg, RenderObjClass *model);	// 0x4967F0 (gadget.cpp declares it the same)  // NAME 1 [robj, arg, model]: neutral
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (gadget.cpp declares it the same)  // NAME 1 [robj]: as ui_avatr.cpp's

MapPipClass::MapPipClass(SClient *client, bool arg) :
	PipSceneClass(client),
	Client(client),
	ViewportGadget(NULL),
	Unknown40(false),
	Unknown44(0)
{
	Scene->Set_Ambient_Light(Vector3(1.0f, 1.0f, 1.0f));
	if (arg) {
		Unknown48 = Create_NEW(new StarmapPipSupportClass);
		Unknown4C = Create_NEW(new StarmapPipSupportClass);
	}
}

void MapPipClass::Unknown_1C(int arg)
{
}

void MapPipClass::Unknown_20(int arg)
{
}

void MapPipClass::Unknown_24(int arg)
{
}

MapPipClass::~MapPipClass()
{
}
















































bool MapPipClass::Unknown_04(const RefCountPtr<InterfaceClass> &iface, const RefCountPtr<RenderObjClass> &vp, int arg)
{
	FAIL_IF(Interface) return(false);
	InterfaceClass *iface_ptr = iface.Peek(); FAIL_IF(!iface_ptr) return(false);
	FAIL_IF(iface_ptr->Is_Active()) return(false);
	FAIL_IF(!vp) return(false);




	UIViewClass *view = iface_ptr->View;

	FAIL_IF(!view) return(false);




	ViewportGadget = (GadgetPipSceneClass *)Unknown_18();
	FAIL_IF(!ViewportGadget) return(false);
	Unknown_14();
	Unknown44 = vp->Get_Sort_Level() - 1;
	if (!Unknown_10(vp)) {
		return(false);
	}
	iface_ptr->Unknown64.Unknown_6870E0(ViewportGadget, arg, iface_ptr->View, true, true);
	ViewportGadget->Unknown_40(false);
	Interface = iface;
	if (Unknown48) {
		Unknown_4967F0(Unknown48.Peek(), -1, vp->Get_Container());
		Unknown4C->Set_Sort_Level(Unknown48->Get_Sort_Level());
	}
	return(true);
}




























bool MapPipClass::Unknown_08()
{
	if (!Interface) {
		FAIL_IF(ViewportGadget) {
			ViewportGadget = NULL;
		}
		return(false);
	}
	Unknown_0C(false);
	if (ViewportGadget) {
		Interface->Unknown64.Unknown_687150(ViewportGadget);
		GadgetClass::Destroy(ViewportGadget);
		ViewportGadget = NULL;
	}
	Interface = NULL;
	return(true);
}






















bool MapPipClass::Unknown_0C(bool on)
{
	FAIL_IF(!Interface) {
		return(false);
	}
	if (Unknown40 != on) {
		if (on) {
			if (!Unknown_7BB080(Interface->Get_View(), 100, Unknown44)) {
				return(false);
			}
			if (Unknown48) {
				Unknown48->Add(Interface->Peek_Scene());
			}
			if (Unknown4C) {
				Unknown4C->Add(Interface->Peek_Scene());
			}
		} else {
			if (Unknown48) {
				Unknown4C->Remove();
			}
			if (Unknown4C) {
				Unknown48->Remove();
			}
			Unknown_7BB110();
		}
		Unknown40 = on;
		ViewportGadget->Unknown_40(on);
		return(true);
	}
	return(false);
}

bool MapPipClass::Unknown_10(const RefCountPtr<RenderObjClass> &vp)
{
	if (!Unknown_7BACD0(Unknown_497300(vp->Get_Container()), true)) {
		return(false);
	}
	if (Unknown48) {
		Unknown48->Unknown_7C3C60(Unknown14, Unknown24);
	}
	if (Unknown4C) {
		Unknown4C->Unknown_7C3C60(Unknown1C, Unknown28);
	}
	return(true);
}

void MapPipClass::Unknown_14()
{
}

bool MapPipClass::Unknown_693020()
{
	return(ViewportGadget->Unknown49);
}

/*
**	Reconstruction notes.
**
**	Placed by the client's __FILE__ 0xB909E0 (the FAIL_IFs of 0x692B30, 0x692D40, 0x692E00). The object runs 0x6927C0-0x693060+:
**	the object before it ends before 0x6927C0 (the triage's lead is loadingview, unchecked) and RefCountPtr<StarmapPipSupportClass>'s Create_NEW (0x693060) closes it.
**	0x692E00's off branch tests +0x48 and removes +0x4C, then tests +0x4C and removes +0x48: the client's, kept.
*/
