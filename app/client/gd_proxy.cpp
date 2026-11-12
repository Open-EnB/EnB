
// Earth & Beyond client (reconstruction): gd_proxy.cpp. Notes at the end of the file.
#include "gd_proxy.h"
#include "rendobj.h"
#include "vector3.h"
#include "..\shared\g_assert.h"


ProxyGadgetClass::ProxyGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask) :
	ButtonGadgetClass(robj, layer, mask, 3),
	Target(NULL),
	ProxyMask(mask),
	Pressed(false),
	StickySave(NULL)
{
}


ProxyGadgetClass::~ProxyGadgetClass()
{
}


void ProxyGadgetClass::Set_Target(RObjGadgetClass *target)
{
	Target = target;
}


bool ProxyGadgetClass::Unknown_30(float x, float y)
{
	if (!Target && Peek_Render_Obj()->Is_Hidden()) {
		return(false);
	}
	bool over = ButtonGadgetClass::Unknown_30(x, y);
	if (Target) {
		Target->Unknown2C_02 = over;
	}
	return(over);
}


bool ProxyGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Target) {
		return(Target->Unknown_2C(event, x, y, mask));
	}
	return(ButtonGadgetClass::Unknown_2C(event, x, y, mask));
}


int ProxyGadgetClass::Get_Proxy_Id()
{
	return(Id);
}

















































void ProxyGadgetClass::Save_Sticky(bool on)
{
	G_ASSERT(!StickySave);
	GadgetClass *gadget = on ? (Target ? (GadgetClass *)Target : this) : NULL;
	GadgetClass *sticky = Unknown_BE5F24;
	Unknown_BE5F24 = gadget;
	StickySave = sticky;
}


void ProxyGadgetClass::Restore_Sticky()
{
	Unknown_BE5F24 = StickySave;
	StickySave = NULL;
}


RObjProxyGadgetClass::RObjProxyGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask) :
	ProxyGadgetClass(robj, layer, mask),
	TargetModel(NULL)
{
}


RObjProxyGadgetClass::~RObjProxyGadgetClass()
{
	if (TargetModel) {
		TargetModel->Release_Ref();
	}
	TargetModel = NULL;
}


RenderObjClass *RObjProxyGadgetClass::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	if (Target) {
		RenderObjClass *model = Target->Unknown_24(position, frame, indicator);
		*position = Peek_Render_Obj()->Get_Position();
		return(model);
	}
	if (indicator) {
		return(ProxyGadgetClass::Unknown_24(position, frame, indicator));
	}
	return(NULL);
}


void RObjProxyGadgetClass::Set_Target(RObjGadgetClass *target)
{
	ProxyGadgetClass::Set_Target(target);
	if (TargetModel) {
		TargetModel->Remove();
		TargetModel->Release_Ref();
		TargetModel = NULL;
		Mask = ProxyMask;
	}
	if (Target) {
		TargetModel = Target->Peek_Render_Obj();
		if (TargetModel) {
			while (TargetModel->Get_Container()) {
				TargetModel = TargetModel->Get_Container();
			}
			TargetModel->Add_Ref();
			TargetModel->Set_Transform(Peek_Render_Obj()->Get_Transform());
			TargetModel->Add(Peek_Render_Obj()->Peek_Scene());
		}
		Mask = Target->Mask;
	}
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_proxy.cpp (the client's own string: the assert '!StickySave'
**	l.107 of 0x66B420)
**
**	The proxy buttons (gd_proxy.h): 0x66B240-0x66B740, tables 0xAFA6E8 (ProxyGadgetClass) and 0xAFA7BC
**	(RObjProxyGadgetClass). The class's inline slots sit between its functions: 0x66B290 (slot +0xA8) after the
**	first constructor, 0x66B510 (slot +0x0C) after the second. The shortcut bar's proxy button (table 0xAFAC78,
**	constructor 0x66C9C0, gd_scut_proxy.cpp after gd_robj.cpp) derives from RObjProxyGadgetClass.
*/
