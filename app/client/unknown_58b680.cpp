// Earth & Beyond client (reconstruction): app\client\unknown_58b680.cpp, the hull mount points window.
// No string names the file (no asserts): the name is an interim one (D137). Notes at the end.
#include <string.h>
#include <math.h>
#include "unknown_58b680.h"
#include "client.h"
#include "co.h"
#include "local_interface_sort.h"
#include "camera.h"
#include "rendobj.h"
#include "matrix3d.h"
#include "vector2.h"
#include "vector2i.h"
#include "texture.h"
#include "wwmath.h"
#include "..\shared\ro_proxy.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\lookup.h"

bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);	// 0x495FD0 (ro_funcs.cpp): the texture onto the model  // NAME 1 [all]: as ui_shipcust.cpp's

// RObj2Texture (the client's profile name for 0x497700's Render_TextureMap; not rebuilt): renders an object into a
// texture, as starmap_data.cpp's (here its second and third arguments are pushed as ints: 9, 9 and 0, 0)
class RObj2Texture
{
public:
	RObj2Texture(const Vector2i &size, const Vector2i &a, const Vector2i &b);	// 0x4973E0  // NAME 1 [size, a, b]: as starmap_data.cpp's
	~RObj2Texture();														// 0x4976B0
	TextureClass *Render_TextureMap(RenderObjClass *robj, int arg);	// 0x497700  // NAME 1 [arg]: neutral
	char Unknown00[0x2C];									// (0x2C bytes, ui_mfdm.cpp's)
};

// 0x58B680
UIMountHullClass::UIMountHullClass(SClient *client, const LayerClass &layer, RenderObjClass *model, int kind) :
	UIMountClass(client, layer.Camera, kind, 0),
	Layer(layer),
	Model(model)
{
	Unknown402 = true;
	Proxy = new ProxyRObj(false);
}

// 0x58B9B0
UIMountHullClass::~UIMountHullClass()
{
	if (Proxy) {
		Proxy->Release_Ref();
		Proxy = NULL;
	}
}

// 0x58B820
void UIMountHullClass::Unknown_54()
{
	Unknown_58B850();
	UIMountClass::Unknown_54();
}

// 0x58B850
void UIMountHullClass::Unknown_58B850()
{
	Vector3 tl = Model->Get_Bone_Transform("vp_tl").Get_Translation();
	Vector3 br = Model->Get_Bone_Transform("vp_br").Get_Translation();
	Vector2 vmin((tl.X + 320.0f) / 640.0f, (480.0f - (tl.Y + 240.0f)) / 480.0f);
	Vector2 vmax((br.X + 320.0f) / 640.0f, (480.0f - (br.Y + 240.0f)) / 480.0f);
	Camera->Set_Viewport(vmin, vmax);
	Camera->Set_View_Plane((vmax.X - vmin.X) * (WWMATH_PI / 4.0), (vmax.Y - vmin.Y) * (WWMATH_PI / 4.0));
	Camera->Set_Clip_Planes(5.0f, 5000.0f);
}

// 0x58BA70
void UIMountHullClass::Unknown_60()
{
	ClientObjectClass *ship = ((ClientClass *)Client)->Get_Ship();
	if (ship) {
		DeltaCompoundClass *auxdata = (DeltaCompoundClass *)ship->AuxData;
		NameClass hull("Inventory.Hull");
		if (auxdata->Get_Lookup()->Unknown_Test_70_Above_1(hull)) {
			bool shown = Shown;
			Unknown_5C();
			Clear();
			Unknown_58();
			if (shown) {
				Toggle();
			}
		}
	}
	UIMountClass::Unknown_60();
}

// 0x58BB10
void UIMountHullClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	UIMountClass::Unknown_58();
	ClientObjectClass *ship = ((ClientClass *)Client)->Get_Ship();
	if (ship) {
		RenderObjClass *robj = ship->RenderObject;
		Proxy->Unknown_47FF70(robj);
	}
}

// 0x58BBB0
void UIMountHullClass::Unknown_5C()
{
	UIMountClass::Unknown_5C();
	if (Proxy->Peek_Scene()) {
		Proxy->Remove();
	}
}

// 0x58BBF0
void UIMountHullClass::Toggle()
{
	UIMountClass::Toggle();
	if (Shown) {
		RenderObjClass *robj = Proxy->Proxied;
		if (robj) {
			RenderObjClass *ship_model = Model->Get_Sub_Object_By_Name("ship");
			if (ship_model) {
				Matrix3D tm = robj->Get_Transform();
				Matrix3D rot(true);
				if (Unknown402) {
					rot.Rotate_Z(DEG_TO_RADF(90.0f));
				}
				robj->Set_Transform(rot);
				bool visible = false;
				ClientObjectClass *ship = ((ClientClass *)Client)->Get_Ship();
				if (ship) {
					visible = ship->UnknownB0;
					ship->Unknown_6FCB20(false);
				}
				RObj2Texture r2t(Vector2i(0x100, 0x100), Vector2i(9, 9), Vector2i(0, 0));
				TextureClass *texture = r2t.Render_TextureMap(robj, 0);
				if (ship) {
					ship->Unknown_6FCB20(!visible);
				}
				robj->Set_Transform(tm);
				Unknown_495FD0(ship_model, texture, 0);
				if (texture) {
					texture->Release_Ref();
				}
				ship_model->Release_Ref();
			}
		} else {
			Shown = false;
		}
	} else {
		if (Proxy->Peek_Scene()) {
			Proxy->Remove();
		}
	}
}

/*
**	Reconstruction notes (unknown_58b680.cpp, client 0x58B630-0x58BF8F)
**
**	The object: a PermStringInitClass static at 0x58B630 (its atexit destructor 0x58B660) opens it, after
**	ui_dynamicwindow.cpp's guarded <string> id 0x58B5D0 and its three deleting-destructor thunks 0x58B600-0x58B620;
**	the guarded <string> id 0x58BF40 and this class's two thunks 0x58BF70 / 0x58BF80 close it, before the next
**	object's PermStringInitClass static 0x58BF90. No string names the file and no caller of the constructor exists.
**	ui_mount.h's inline Is_Shown / Show / Hide (0x58B770 / 0x58B790 / 0x58B7C0) are emitted here, with the table.
**
**	Its data: Model's bones 'vp_tl' / 'vp_br' (RenderObjClass slot +0xB8, Get_Bone_Transform) give the camera's
**	viewport in screen units (640 x 480) and a view plane of a quarter pi per unit; 'ship' (slot +0x70) takes the
**	texture RObj2Texture renders of the ship's model (Proxy's Proxied, 256 x 256, turned a quarter turn about Z when
**	Unknown402 is set), the ship's own model hidden meanwhile (ClientObjectClass 0x6FCB20). The client's Get_Ship
**	(ClientClass slot +0x30) and its auxdata's Inventory.Hull (LookupClass 0x4ACB20) drive the rebuild.
**
**	Forms that matter: the base gets layer.Camera read directly (no Peek_Camera call); 0x58B850 copies the bones'
**	translations into Vector3 locals (only X, Y used: the frame is the client's 0x20); WWMATH_PI / 4.0 makes the
**	client's double constant; 0x58BA70 loads the auxdata before the NameClass; 0x58BB10 holds the ship's render
**	object in a local.
*/
