// Earth & Beyond client (reconstruction): app\Client\gd_robj.cpp (__FILE__ 0xB8D374). Notes at the end of the file.
#include "gd_robj.h"
#include "rendobj.h"
#include "scene.h"
#include "fontmanager.h"
#include "aabox.h"
#include "..\shared\unknown_debug.h"

// ro_funcs.cpp (app\shared, cdecl), as far as this file calls it
void Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110: a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.h
bool Unknown_496240(RenderObjClass *robj, int arg);	// 0x496240 (over the sub-objects)  // NAME 1 [robj, arg]: neutral
int Unknown_495FD0(RenderObjClass *robj, int arg, int index);	// 0x495FD0 (into the material's table at index)  // NAME 1 [robj, arg, index]: neutral
int Unknown_4960C0(RenderObjClass *robj, int index);	// 0x4960C0 (out of the material's table at index)  // NAME 1 [robj, index]: neutral
bool Unknown_496960(RenderObjClass *robj, float x, float y, const LayerClass &layer);	// 0x496960 (FAIL_IFs on the layer's scene and camera: a pick)  // NAME 1 [robj, x, y, layer]: neutral

// NOT EXACT: scheduling (similarity 0.93): the client stores the table after the flags byte; 0x66BEC0, the same source, is exact
RObjGadgetClass::RObjGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask) :
	GadgetClass(mask),
	RObj(NULL),
	Unknown34(NULL),
	Layer(layer)
{
	Unknown5C_00 = false;
	Unknown5C_01 = false;
	Unknown5C_02 = false;
	Unknown5C_03 = true;
	Set_Render_Obj(robj);
}

RObjGadgetClass::RObjGadgetClass(RenderObjClass *robj, unsigned int mask) :
	GadgetClass(mask),
	RObj(NULL),
	Unknown34(NULL)
{
	Unknown5C_00 = false;
	Unknown5C_01 = false;
	Unknown5C_02 = false;
	Unknown5C_03 = true;
	Set_Render_Obj(robj);
}

RObjGadgetClass::~RObjGadgetClass()
{
	Set_Render_Obj(NULL);
	if (Unknown34) {
		Unknown34->Release_Ref();
	}
	Unknown34 = NULL;
}

void RObjGadgetClass::Set_Render_Obj(RenderObjClass *robj)
{
	if (robj == RObj) {
		return;
	}
	bool highlighted = Unknown5C_00;
	bool inscene = Unknown5C_01;
	bool visible = Unknown5C_02;
	if (RObj) {
		if (highlighted) {
			Unknown_66C360();
		}
		if (inscene) {
			Unknown_78();
		}
		if (visible) {
			Unknown_8C(false);
		}
		if (RObj) {
			RObj->Release_Ref();
		}
		RObj = NULL;
	}
	if (robj) {
		RObj = robj;
		RObj->Add_Ref();
		if (highlighted) {
			Unknown_66C2D0();
		}
		if (inscene) {
			Unknown_74();
		}
		if (visible) {
			Unknown_8C(true);
		}
	}
}

void RObjGadgetClass::Unknown_98(FontStyleClass *style)
{
	if (style) {
		style->Add_Ref();
	}
	if (Unknown34) {
		Unknown34->Release_Ref();
	}
	Unknown34 = style;
}

void RObjGadgetClass::Unknown_66C220(float r, float g, float b)
{
	if (!(bool)Unknown2C_0A) {
		Unknown_496110(RObj, r, g, b);
	}
}

void RObjGadgetClass::Unknown_66C260(const Vector3 &color)
{
	if (!(bool)Unknown2C_0A) {
		Unknown_496110(RObj, color.X, color.Y, color.Z);
	}
}

void RObjGadgetClass::Unknown_66C2A0(int arg)
{
	Unknown_496240(RObj, arg);
}

void RObjGadgetClass::Unknown_66C2D0()
{
	if (!(bool)Unknown2C_0A && !Unknown5C_00) {
		Vector3 color = Unknown_9C();
		Unknown_66C220(color.X, color.Y, color.Z);
		if (Unknown34) {
			Unknown34->Unknown_6CE910(true);
		}
		Unknown5C_00 = true;
	}
}

void RObjGadgetClass::Unknown_66C360()
{
	if (!(bool)Unknown2C_0A && Unknown5C_00) {
		Vector3 color = Unknown_A0();
		Unknown_66C220(color.X, color.Y, color.Z);
		if (Unknown34) {
			Unknown34->Unknown_6CE910(false);
		}
		Unknown5C_00 = false;
	}
}

void RObjGadgetClass::Unknown_8C(bool on)
{
	if (Unknown5C_02 != on) {
		if (RObj) {
			RObj->Set_Hidden(on);
		}
		if (Unknown34) {
			Unknown34->Set_Hidden(on);
		}
		Unknown5C_02 = on;
	}
}

void RObjGadgetClass::Unknown_90(int arg)
{
	Unknown_495FD0(RObj, arg, 0);
}

void RObjGadgetClass::Unknown_66C4A0()
{
	Unknown_4960C0(RObj, 0);
}

bool RObjGadgetClass::Unknown_30(float x, float y)
{
	if (RObj && !RObj->Is_Hidden() && RObj->Peek_Scene()) {
		return(Unknown_496960(RObj, x, y, Layer));
	}
	return(false);
}

RenderObjClass *RObjGadgetClass::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	if (!Unknown5C_03) {
		return(NULL);
	}
	if (RObj) {
		*position = RObj->Get_Position();
	} else {
		position->Set(0.0f, 0.0f, 0.0f);
	}
	return(RObj);
}

void RObjGadgetClass::Unknown_28(LayerClass *layer)
{
	bool inscene = Unknown5C_01;
	if (inscene) {
		Unknown_78();
	}
	Layer.Set(*layer);
	if (inscene) {
		Unknown_74();
	}
}

void RObjGadgetClass::Unknown_74()
{
	if (RObj) {
		Unknown5C_01 = true;
		RObj->Add(Layer.Peek_Scene());
	}
}

void RObjGadgetClass::Unknown_78()
{
	Unknown_6607D0();
	if (RObj) {
		Unknown5C_01 = false;
		RObj->Remove();
	}
}

void RObjGadgetClass::Unknown_54(GadgetListManagerClass *manager, InterfaceClass *iface)
{
	Unknown_74();
	GadgetClass::Unknown_54(manager, iface);
}

void RObjGadgetClass::Unknown_58(GadgetListManagerClass *manager)
{
	Unknown_78();
	GadgetClass::Unknown_58(manager);
}

const Matrix3D &RObjGadgetClass::Unknown_66C6C0()
{
	if (RObj) {
		return(RObj->Get_Transform());
	}
	static Matrix3D identity(true);
	return(identity);
}












void RObjGadgetClass::Unknown_64(AABoxClass &box)
{
	G_ASSERT(RObj);
	if (RObj) {
		box = RObj->Get_Bounding_Box();
		return;
	}
	GadgetClass::Unknown_64(box);
}

Vector3 RObjGadgetClass::Unknown_9C()
{
	return(Vector3(0.35f, 0.35f, 0.35f));
}

Vector3 RObjGadgetClass::Unknown_A0()
{
	return(Vector3(1.0f, 1.0f, 1.0f));
}



/*
**	Reconstruction notes (GAD4, 2026-11-01). Client 0x66BDF0-0x66C8A1 (table 0xAFAAA0, 41 slots; __FILE__ 0xB8D374,
**	G_ASSERT 'RObj' on the client's line 250 in 0x66C7D0). gd_resize.cpp's FAIL_IF (0x66BB10, line 64) is the code
**	just before; gd_selct.cpp's 'GD_SELECT_FONT' is first read at 0x66CC00. Between them 0x66C8F0-0x66C9A0 (a
**	640x480 gadget, table 0xAFAB64) belong to one of the two files; not placed.
**
**	Slot +0x08's one-line inlines and slots +0x7C-+0x88 (0x51AD20-0x51ADA0) are emitted beside smouse.cpp's, as
**	gadget.h's. The highlight (0x66C2D0 / 0x66C360) takes slot +0x9C / +0xA0's colour into a local and passes it
**	member by member to 0x66C220 (inlined), which is what the client's fld / fstp pushes show.
**
**	NOT EXACT: the constructor 0x66BDF0 (the table store is scheduled after the flags byte in the client, before
**	it here; the twin 0x66BEC0 is exact with the same source).
*/
