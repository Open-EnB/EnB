// Earth & Beyond client (reconstruction): app\client\ui_mfdm.cpp (per the client's asserts), the display's items
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "option.h"
#include "rendobj.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "matinfo.h"
#include "sphere.h"
#include "aabox.h"
#include "avatar_pip.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "ui_mfd.h"
#include "ui_mfd_extended_context.h"

void Unknown_496E90(SphereClass &sphere, RenderObjClass *robj);	// 0x496E90 (ro_funcs.cpp's neighbourhood: a render object's bounding sphere)  // NAME 1: neutral (D10)

struct Unknown4973E0Point { Unknown4973E0Point(int x, int y) : X(x), Y(y) {} int X; int Y; };	// NAME 1: neutral (D10)
// An off-screen render of an object, 0x5C3150's (0x2C bytes; 0x4973E0, 0x497700, 0x4976B0: not rebuilt yet)
class Unknown4973E0Class	// NAME 1: neutral (D10)
{
public:
	Unknown4973E0Class(const Unknown4973E0Point &size, const Unknown4973E0Point &a, const Unknown4973E0Point &b);	// 0x4973E0  // NAME 1 [size, a, b]: neutral (D10)
	~Unknown4973E0Class();								// 0x4976B0
	TextureClass *Unknown_497700(RenderObjClass *robj, int arg);	// 0x497700  // NAME 1 [robj, arg]: neutral (D10)
	char Unknown00[0x2C];
};

static const char *UIMFDDirectionNames[] = {	// 0xB82554  // NAME 2: what it holds
	"MFD_X_DIRECTION", "MFD_X_VALUE", "MFD_Y_DIRECTION", "MFD_Y_VALUE", "MFD_Z_DIRECTION", "MFD_Z_VALUE", "MFD_HEADING", "MFD_PITCH"
};

// 0x5C1FE0
bool UIMFDItemClass::Unknown_14()
{
	return(false);
}

// 0x5C2000
UIMFDPIPItemClass::UIMFDPIPItemClass(UIMFDClass *owner) :
	UIMFDItemClass(owner),
	Transform(true),
	UnknownBool(false),
	PIPVP(NULL),
	Size(-1.0f, -1.0f),
	Light(new LightClass(LightClass::POINT))
{
	Scene = Create_NEW(new SimpleSceneClass());
	Scene->Set_Fog_Enable(false);
	Scene->Set_Ambient_Light(Vector3(0.2f, 0.2f, 0.2f));
	Camera = Create_NEW(new CameraClass());
	Camera->Set_Viewport(Vector2(0.3f, 0.68750006f), Vector2(0.46875f, 0.87500006f));
	Camera->Set_View_Plane(Vector2(-0.887f, -0.5f), Vector2(0.887f, 0.5f));
	Camera->Set_Clip_Planes(5.0f, 5000.0f);
	Light->Set_Intensity(1.0f);
	Light->Set_Force_Visible(true);
	Light->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Light->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	Light->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
	Light->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	Light->Set_Transform(Matrix3D::Identity);
	Light->Add(Scene.Peek());
	PIPVP = Owner->RObj->Get_Sub_Object_By_Name("scanner_click");
	AABoxClass box = PIPVP->Get_Bounding_Box();
	Size.X = box.Extent.X + box.Extent.X;
	Size.Y = box.Extent.Y + box.Extent.Y;
}
// NOT EXACT: 0x5C2470: a load scheduled before a store, one EH state store (order)
// 0x5C2470
UIMFDPIPItemClass::~UIMFDPIPItemClass()
{





























































	G_ASSERT(Light);
	REF_PTR_RELEASE(Light);




	G_ASSERT(PIPVP);
	REF_PTR_RELEASE(PIPVP);
}

// 0x5C25A0
bool UIMFDPIPItemClass::Unknown_14()
{
	return(Get_RObj() != NULL);
}
// NOT EXACT: 0x5C25C0: one pop scheduled after a load (order)
// 0x5C25C0
void UIMFDPIPItemClass::Unknown_08()
{
	RenderObjClass *robj = Get_RObj();
	if (robj == NULL) {
		Camera->Set_Clip_Planes(0.0001f, 0.099999994f);
		return;
	}
	if (robj->Peek_Scene() == NULL) {
		robj->Add(Scene.Peek());
	}
	int level = Owner->RObj->Get_Sort_Level();
	if (level == 0) {
		RenderObjClass *frame = Owner->RObj->Get_Sub_Object_By_Name("FRAME_SELECT");
		if (frame != NULL) {
			level = frame->Get_Sort_Level();
			frame->Release_Ref();
		}
		if (level == 0) {
			level = 9;
		}
	}
	int poly;
	if (OptionClass::Instance->Get_Bool("Graphics_Default")) {
		poly = (int)((float)((SClient *)((ClientClass *)Owner->Client)->Unknown_18())->PolyBudget * 0.1f);
	} else {
		poly = (int)((float)(int)(8000.0f + (OptionClass::Instance->Get_Float("Graphics_Space_Poly") * 0.01f) * (40000.0f - 8000.0f)) * 0.1f);
	}
	((UIViewClass *)Owner->View)->Unknown_65B7A0(Scene, Camera, poly, level);
	Vector3 pos = PIPVP->Get_Position();
	Vector2 p(pos.X - Size.X * 0.5f, pos.Y + Size.Y * 0.5f);
	p.X = p.X + 320.0f;
	p.Y = 240.0f - p.Y;
	Camera->Set_Viewport(Vector2(p.X * (1.0f / 640.0f), p.Y * (1.0f / 480.0f)), Vector2((p.X + Size.X) * (1.0f / 640.0f), (p.Y + Size.Y) * (1.0f / 480.0f)));
	float a = 0.5 / Size.Y * Size.X;
	Camera->Set_View_Plane(Vector2(-a, -0.5f), Vector2(a, 0.5f));
	MainViewClass *view = ((SClient *)((ClientClass *)Owner->Client)->Unknown_18())->MainView;
	OrbitalManager *orbitals = view->Orbitals;
	if (orbitals != NULL) {
		Vector4 color = orbitals->Unknown_4F50D0(0);
		Light->Set_Diffuse(Vector3(color.X, color.Y, color.Z));
		Light->Set_Intensity(color.W);
	}
}

// 0x5C28D0
void UIMFDPIPItemClass::Unknown_0C()
{
	RenderObjClass *robj = Get_RObj();
	if (robj != NULL) {
		if (robj->Peek_Scene() != NULL) {
			robj->Remove();
		}
	}
	((UIViewClass *)Owner->View)->Unknown_65B7E0(Scene);
}

// 0x5C2920
void UIMFDPIPItemClass::Unknown_10()
{
	RenderObjClass *robj = Get_RObj();
	if (robj == NULL) {
		Camera->Set_Clip_Planes(0.0001f, 0.099999994f);
		return;
	}
	Matrix3D tm = robj->Get_Transform();
	Vector3 pos = tm.Get_Translation();
	SphereClass sphere = robj->Get_Bounding_Sphere();
	if (sphere.Radius <= 0.0f) {
		Unknown_496E90(sphere, robj);
	}
	Matrix3D camera(true);
	if (UnknownBool) {
		camera = Transform;
	} else {
		camera.Look_At(Transform.Get_Translation(), sphere.Center, 0.0f);
		Vector3 z = camera.Get_Z_Vector();
		float d = sphere.Radius * 1.8;
		camera.Set_Translation(sphere.Center + z * d);
	}
	Camera->Set_Transform(camera);
	float dist = (camera.Get_Translation() - pos).Length();
	float diameter = sphere.Radius + sphere.Radius;
	float zfar = diameter + dist;
	if (fabs(Camera->Get_Depth() - zfar) > 0.0001) {
		float znear = dist - diameter;
		Camera->Set_Clip_Planes(WWMath::Max(1.0f, znear), zfar);
	}
	Matrix3D light_tm = camera;
	Light->Set_Transform(light_tm);
}

// 0x5C2CD0
Unknown5C2CD0Class::Unknown5C2CD0Class(UIMFDClass *owner) :
	UIMFDPIPItemClass(owner),
	Carrier(NULL)
{
	UnknownBool = false;
	Carrier = new Unknown47FA70Class(true);
}

// 0x5C2DC0
Unknown5C2CD0Class::~Unknown5C2CD0Class()
{
	Carrier->Release_Ref();
}

// 0x5C2E40
RenderObjClass *Unknown5C2CD0Class::Get_RObj()
{
	return((Carrier->Model != NULL) ? Carrier : NULL);
}

// 0x5C2E60
void Unknown5C2CD0Class::Unknown_04(ClientObjectClass *object)
{
	if (object != NULL) {
		RenderObjClass *targ_robj = object->RenderObject;













































		G_ASSERT(targ_robj);
		Carrier->Unknown_47FF70(targ_robj);
	} else {
		Carrier->Unknown_47FF70(NULL);
	}
}

// 0x5C2ED0
void Unknown5C2CD0Class::Unknown_08()
{
	if (Get_RObj() != NULL) {
		UIMFDPIPItemClass::Unknown_08();
	}
}

// 0x5C2F00
void Unknown5C2CD0Class::Unknown_10()
{
	ClientObjectClass *object = Owner->Unknown_5BD520();
	if (object != NULL) {
		if (Carrier->Model != object->RenderObject) {
			Unknown_0C();
			Unknown_08();
		}
		Carrier->Set_Transform(object->Unknown_28(true));
		ClientObjectClass *source = Owner->Unknown_5BD500();
		if (source == NULL) {
			source = object;
		}
		Transform = source->Unknown_28(true);
	} else if (Carrier->Model != NULL) {
		Unknown_0C();
		Unknown_08();
	}
	UIMFDPIPItemClass::Unknown_10();
}

// 0x5C3030
Unknown5C3030Class::Unknown5C3030Class(UIMFDClass *owner) :
	UIMFDItemClass(owner),
	Texture(NULL)
{
}

// 0x5C30B0
Unknown5C3030Class::~Unknown5C3030Class()
{
	if (Texture != NULL) {
		Texture->Release_Ref();
	}
	Texture = NULL;
}

// 0x5C3130
bool Unknown5C3030Class::Unknown_14()
{
	return(Owner->Unknown_5BD520() != NULL);
}

// 0x5C3150
void Unknown5C3030Class::Unknown_08()
{
	ClientObjectClass *object = Owner->Unknown_5BD520();
	if (object == NULL) {
		return;
	}
	RenderObjClass *robj = object->RenderObject;
	Matrix3D saved = robj->Get_Transform();
	Matrix3D tm(true);
	tm.Rotate_Z(1.5707964f);
	robj->Set_Transform(tm);
	bool hidden = object->UnknownB0;
	object->Unknown_6FCB20(false);
	Unknown4973E0Class render(Unknown4973E0Point(0x80, 0x80), Unknown4973E0Point(0, 0), Unknown4973E0Point(0, 0));
	TextureClass *texture = render.Unknown_497700(robj, 0);
	robj->Set_Transform(saved);
	object->Unknown_6FCB20(!hidden);
	MeshClass *mesh = (MeshClass *)Owner->RObj->Get_Sub_Object_By_Name("scanner_click");
	MaterialInfoClass *minfo = mesh->Get_Material_Info();




























































































































	G_ASSERT(minfo);
	TextureClass *tex = minfo->Peek_Texture(0);

	G_ASSERT(tex);
	if (Texture == NULL) {
		Texture = tex;
		Texture->Add_Ref();
	}
	mesh->Replace_Texture(tex, texture);
	minfo->Replace_Texture(0, texture);
	REF_PTR_RELEASE(texture);
	minfo->Release_Ref();
	mesh->Release_Ref();
}

// 0x5C34C0
void Unknown5C3030Class::Unknown_0C()
{
	MeshClass *mesh = (MeshClass *)Owner->RObj->Get_Sub_Object_By_Name("scanner_click");
	MaterialInfoClass *minfo = mesh->Get_Material_Info();







	G_ASSERT(minfo);
	TextureClass *tex = minfo->Peek_Texture(0);
	G_ASSERT(tex);
	if (Texture == NULL) {
		Texture = tex;
		Texture->Add_Ref();
	}
	mesh->Replace_Texture(tex, Texture);
	minfo->Replace_Texture(0, Texture);
	minfo->Release_Ref();
	mesh->Release_Ref();
}

// 0x5C35D0
void Unknown5C3030Class::Unknown_10()
{
	ClientObjectClass *object = Owner->Unknown_5BD520();
	if (object != NULL) {
		NameClass name("Inventory.Hull");
		if (((DeltaCompoundClass *)object->AuxData)->Get_Lookup()->Unknown_Test_70_Above_1(name)) {
			Unknown_0C();
			Unknown_08();
		}
	}
}

// 0x5C3650
Unknown5C3650Class::Unknown5C3650Class(UIMFDClass *owner) :
	UIMFDItemClass(owner)
{
}

// 0x5C36D0
Unknown5C3650Class::~Unknown5C3650Class()
{
	if (Context != NULL) {
		Context = NULL;
	}
}

// 0x5C3790
void Unknown5C3650Class::Unknown_04(ClientObjectClass *object)
{
	ClientObjectClass *self = Owner->Unknown_5BD500();
	if (self == NULL) {
		Context = NULL;
		return;
	}
	Context = UIMFDExtendedContextClass::Create(self, object, Owner);
}

// 0x5C38A0
void Unknown5C3650Class::Unknown_08()
{
	if (Context != NULL) {
		Context->Unknown_08();
	}
}

// 0x5C38C0
void Unknown5C3650Class::Unknown_0C()
{
	if (Context != NULL) {
		Context->Unknown_0C();
	}
}

// 0x5C38E0
void Unknown5C3650Class::Unknown_10()
{
	if (Context != NULL) {
		Context->Unknown_10();
	}
}

// 0x5C3900
bool Unknown5C3650Class::Unknown_14()
{
	return(Context != NULL);
}

// 0x5C3920
Unknown5C3920Class::Unknown5C3920Class(UIMFDClass *owner) :
	UIMFDItemClass(owner),
	Pitch(-1),
	Heading(-1),
	Unknown30(-1),
	DirectionY(-1),
	DirectionX(-1),
	DirectionZ(-1),
	Axis(-1),
	Position(0.0f, 0.0f, 0.0f)
{
	FontSys *fontbase = Owner->Lines[0].Font;
	for (int i = 0; i < 8; i++) {
		FontSys *font = new FontSys(UIMFDDirectionNames[i], 0, 0);
		Lines[i] = font;
		font->Set_Sort_Level(fontbase->Get_Sort_Level());
	}
}

// 0x5C3AB0
Unknown5C3920Class::~Unknown5C3920Class()
{
	for (int i = 0; i < 8; i++) {
		Lines[i]->Release_Ref();
	}
}

// 0x5C3B40
bool Unknown5C3920Class::Unknown_14()
{
	if (Owner->Unknown_5BD520() == NULL && Owner->Unknown_5BD500() == NULL) {
		return(false);
	}
	return(true);
}

// 0x5C3B80
void Unknown5C3920Class::Unknown_08()
{
	Vector3 position = Owner->Lines[0].Font->Get_Position();
	for (int i = 0; i < 8; i++) {
		if (Lines[i]->Peek_Scene() == NULL) {
			Lines[i]->Add(Owner->Layer.Scene);
		}
		Lines[i]->Set_Position(position);
	}
}

// 0x5C3C00
void Unknown5C3920Class::Unknown_0C()
{
	for (int i = 0; i < 8; i++) {
		if (Lines[i]->Peek_Scene() != NULL) {
			Lines[i]->Remove();
		}
	}
}

// 0x5C3C40
void Unknown5C3920Class::Unknown_10()
{
	ClientObjectClass *object = Owner->Unknown_5BD520();
	if (object == NULL) {
		object = Owner->Unknown_5BD500();
		if (object == NULL) {
			return;
		}
	}
	Vector3 position = object->Unknown_24();
	if (Position.X != position.X) {
		Lines[1]->Unknown_6CE420(0, "%d", (int)position.X);
	}
	if (Position.Y != position.Y) {
		Lines[3]->Unknown_6CE420(0, "%d", (int)position.Y);
	}
	if (Position.Z != position.Z) {
		Lines[5]->Unknown_6CE420(0, "%d", (int)position.Z);
	}
	Position = position;
	Matrix3D tm = object->Unknown_28(true);
	Vector3 x(tm[0][0], tm[1][0], tm[2][0]);
	Vector3 y(tm[0][2], tm[1][2], tm[2][2]);
	int heading = (int)(atan2(x.X, x.Y) * 57.29577791868204 + 360.0) % 360;
	if (Heading != heading) {
		Lines[6]->Unknown_6CE420(0, "Heading:%d", heading);
		Heading = heading;
	}
	int pitch = (int)(90.0 - y.Z * 90.0) % 360;
	if (x.Z < 0.0f) {
		pitch = -pitch;
	}
	if (Pitch != pitch) {
		Lines[7]->Unknown_6CE420(0, "Pitch:%d", pitch);
		Pitch = pitch;
	}
	bool changed = false;
	char c;
	if (x.X > 0.0f) {
		c = '+';
	} else if (x.X < 0.0f) {
		c = '-';
	} else {
		c = '=';
	}
	if (DirectionX != c) {
		DirectionX = c;
		changed = true;
	}
	if (x.Y > 0.0f) {
		c = '+';
	} else if (x.Y < 0.0f) {
		c = '-';
	} else {
		c = '=';
	}
	if (DirectionY != c) {
		DirectionY = c;
		changed = true;
	}
	char z_direction;
	if (x.Z > 0.0f) {
		z_direction = '+';
	} else if (x.Z < 0.0f) {
		z_direction = '-';
	} else {
		z_direction = '=';
	}
	if (DirectionZ != z_direction) {
		DirectionZ = z_direction;
		changed = true;
	}
	char axis;
	float ax = fabs(x.X);
	float ay = fabs(x.Y);
	float az = fabs(x.Z);
	if (ax > ay) {
		if (ax > az) {
			axis = 'x';
		} else {
			axis = 'z';
		}
	} else {
		axis = 'y';
		if (ay <= az) {
			axis = 'z';
		}
	}
	if (Axis != axis) {
		Axis = axis;
	} else if (!changed) {
		return;
	}
	Lines[0]->Unknown_6CE420(0, "%cX:%c", (char)((axis == 'x') ? '>' : ' '), DirectionX);
	Lines[2]->Unknown_6CE420(0, "%cY:%c", (char)((axis == 'y') ? '>' : ' '), DirectionY);
	Lines[4]->Unknown_6CE420(0, "%cZ:%c", (char)((axis == 'z') ? '>' : ' '), DirectionZ);
	DirectionZ = z_direction;
}
