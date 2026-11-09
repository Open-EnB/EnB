// Earth & Beyond client (reconstruction): app\client\ui_mount.cpp, the equipment mount points window
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\ui_mount.cpp' at 0xB82A4C). Notes at the end.
#define __DELTA_ARRAY_MEMBERS	// delta_array.h's Get_Value
#include <string.h>
#include <math.h>
#include "ui_mount.h"
#include "aux_roots.h"
#include "client.h"
#include "co.h"
#include "local_interface_sort.h"
#include "camera.h"
#include "assetmgr.h"
#include "fontmanager.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\message.h"
#include "..\shared\enbassetmgr.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (avatar.cpp's declaration)
const char *Unknown_843440(int mount_type);	// 0x843440: a mount's type as text  // NAME 1: neutral (D10); NAME 1 [mount_type]: what it is given

// The line from a button to its mount point (0xF4 bytes, constructor 0x6DF890; line2d.h's neighbourhood)
class Unknown6DF890Class	// NAME 1: neutral (D10)
{
public:
	Unknown6DF890Class(const Vector4 &color, const Vector2 &from, const Vector2 &to, float width);	// 0x6DF890  // NAME 1 [color, from, to, width]: what Build passes
	char Unknown00[0xF4];
};

extern Matrix3D UnknownBF9330;	// 0xBF9330 (the model's transform)  // NAME 1: neutral (D10)
extern Matrix3D UnknownBF92D0;	// 0xBF92D0 (the model's transform with +0x402)  // NAME 1: neutral (D10)

// 0x5C73B0
UIMountClass::UIMountClass(SClient *client, CameraClass *camera, int kind, int unused) :
	Unknown59F3D0Class(client, kind),
	Camera(camera),
	Model(0),
	Distance(-1.0f),
	Built(false),
	Unknown401(true),
	Unknown402(false),
	Shown(false)
{
	Camera->Add_Ref();
	Unknown401 = true;
	memset(Points, 0, sizeof(Points));
}

// 0x5C74A0
void UIMountClass::Unknown_54()
{
	Unknown59F3D0Class::Unknown_54();
	Build();
}

// 0x5C74D0
UIMountClass::~UIMountClass()
{
	Clear();
	Camera->Release_Ref();
	Clear();
}

// 0x5C7570
void UIMountClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	Build();
	if (Built) {
		Shown = false;
		Unknown59F3D0Class::Unknown_58();
	}
}

// 0x5C7600
void UIMountClass::Unknown_5C()
{
	Hide();
	Unknown59F3D0Class::Unknown_5C();
}


















































































































// 0x5C7630
void UIMountClass::Toggle()
{
	int i;
	if (!Shown) {
		FAIL_IF(!View) return;
		SceneClass *scene = (SceneClass *)View->Unknown_50();
		FAIL_IF(!scene) return;
		for (i = 0; i < (int)Unknown88.size(); i++) {
			UIMountPointStruct &p = Points[i];
			if (p.Button) p.Button->Add(scene);
			if (p.MntPoint) p.MntPoint->Add(scene);
			if (p.Line) p.Line->Add(scene);
		}
		Shown = true;
		return;
	}
	for (i = 0; i < (int)Unknown88.size(); i++) {
		UIMountPointStruct &p = Points[i];
		if (p.Button) p.Button->Remove();
		if (p.MntPoint) p.MntPoint->Remove();
		if (p.Line) p.Line->Remove();
	}
	Shown = false;
}











































// NOT EXACT: functional, from the pseudocode (D64)
// 0x5C77A0
void UIMountClass::Build()
{
	if (Built) return;
	Clear();
	ClientObjectClass *cobj = ((ClientClass *)Client)->Get_Avatar();


	FAIL_IF(!cobj) return;


	FAIL_IF(!cobj->AuxData) return;
	DeltaCompoundClass *auxdata = (DeltaCompoundClass *)cobj->AuxData;
	DeltaStringClass *model_name = Unknown_Find_Field<DeltaStringClass>(auxdata, NameClass("Inventory.EquipMountModel"));
	if (!model_name || !model_name->Unknown70) return;
	DeltaArrayClass<UnknownB016DCType> *mounts = Unknown_Find_Field<DeltaArrayClass<UnknownB016DCType> >(auxdata, NameClass("Inventory.Mounts"));
	if (!mounts || !mounts->Unknown70) return;
	DeltaItemArrayClass<DeltaStringClass> *bone_names = Unknown_Find_Field<DeltaItemArrayClass<DeltaStringClass> >(auxdata, NameClass("Inventory.MountBoneNames"));
	if (!bone_names || !bone_names->Unknown70) return;

	WW3DAssetManager *assets = WW3DAssetManager::Get_Instance();
	Model = AssetManager->Create_Render_Obj(model_name->Get());
	if (!Model) {
		AssetManager->Load_3D_Assets(model_name->Get());
		Model = AssetManager->Create_Render_Obj(model_name->Get());
		if (!Model) return;
	}
	if (!Unknown402) {
		Model->Set_Transform(UnknownBF9330);
	} else {
		Model->Set_Transform(UnknownBF92D0);
	}
	int bone = Model->Get_Bone_Index("VP_TL");
	if (!bone) return;
	Vector3 tl = Model->Get_Bone_Transform(bone).Get_Translation();
	bone = Model->Get_Bone_Index("VP_BR");
	if (!bone) return;
	Vector3 br = Model->Get_Bone_Transform(bone).Get_Translation();

	// back the camera off until both corners project inside the view, halving the step at each crossing
	Matrix3D saved = Camera->Get_Transform();
	float step = 256.0f;
	Matrix3D tm(1);
	Vector3 dest;
	do {
		bool inside = false;
		tm.Translate_Z(step);
		Camera->Set_Transform(tm);
		if (Camera->Project(dest, tl) == CameraClass::INSIDE_FRUSTUM && Camera->Project(dest, br) == CameraClass::INSIDE_FRUSTUM) {
			inside = true;
		}
		if ((step > 0.0f) == inside) {
			step *= -0.5f;
		}
	} while (fabs(step) > 0.5f);

	int count = mounts->Children.size();
	float button_w = -1.0f;
	float button_h = -1.0f;
	Vector2 vmin, vmax;
	Camera->Get_Viewport(vmin, vmax);
	float min_x = 0.0f;
	float min_y = 0.0f;
	float max_x = 0.0f;
	float max_y = 0.0f;
























































	for (int i = 0; i < count; i++) {
		UnknownB016DCType *mount = &mounts->Get_Value(i);
		if (!mount->I00) continue;
		const char *bone_name = ((DeltaStringClass *)bone_names->Children[i])->Get();
		if (!bone_name) continue;
		int boneidx = Model->Get_Bone_Index(bone_name);
		if (strlen(bone_name) != 0) {
			FAIL_IF(0 == boneidx) {
				Message_Error("Equipment Mount Points", "", __FILE__, __LINE__, "Bone Name %s not found in player ship mesh", bone_name);
				continue;
			}
		} else if (!boneidx) {
			continue;
		}
		UIMountPointStruct &p = Points[i];
		const Matrix3D &bt = Model->Get_Bone_Transform(boneidx);
		Vector3 pos(bt[0][3], bt[1][3], 0.0f);
		Vector3 screen;
		Camera->Project(screen, pos);
		CameraClass::Convert_Old(screen);
		Vector3 point;
		point.Y = (vmax.Y - vmin.Y) * screen.Y + vmin.Y;
		point.X = ((vmax.X - vmin.X) * screen.X + vmin.X) * 640.0f - 320.0f;
		point.X = (float)floor(point.X);
		point.Y = 240.0f - point.Y * 480.0f;
		point.Y = (float)floor(point.Y);
		p.PointX = point.X;
		p.PointY = point.Y;
		point.Z = 0.0f;
		screen.X = screen.X + screen.X - 1.0f;
		screen.Y = screen.Y + screen.Y - 1.0f;
		p.X = screen.X;
		p.Y = screen.Y;
		if (screen.X < min_x) min_x = screen.X; else if (screen.X > max_x) max_x = screen.X;
		if (screen.Y < min_y) min_y = screen.Y; else if (screen.Y > max_y) max_y = screen.Y;
		p.Button = assets->Create_Render_Obj("Equipbtn");
		Unknown_496AE0(p.Button, false);
		p.Bttn = p.Button->Get_Sub_Object_By_Name("bttn");
		p.Unknown0C = p.Button->Get_Sub_Object_Bone_Index(p.Bttn);
		p.MntPoint = assets->Create_Render_Obj("MntPoint");
		p.MntPoint->Set_Position(point);
		p.Name = new FontSys("MOUNT_NAME", 0, Unknown_843440(mount->I00));
		p.Button->Add_Sub_Object_To_Bone(p.Name, "frame");
		if (button_w == -1.0f) {
			AABoxClass box;
			p.Button->Get_Obj_Space_Bounding_Box(box);
			button_w = box.Extent.X + box.Extent.X + 4.0f;
			button_h = box.Extent.Y + box.Extent.Y + 4.0f;
		}
	}

	// spread the buttons so the outermost fit the view
	button_h = button_h * (1.0f / 480.0f);
	float w = button_w * (1.0f / 640.0f) + button_w * (1.0f / 640.0f);
	float d = 1.0f - ((1.0f - -min_x) - w);
	if (d <= 0.01f) d = 0.01f;
	float scale_left = 1.0f / d;
	d = 1.0f - ((1.0f - max_x) - w);
	if (d <= 0.01f) d = 0.01f;
	float scale_right = 1.0f / d;
	d = 1.0f - ((1.0f - -min_y) - button_h);
	if (d <= 0.01f) d = 0.01f;
	float scale_down = 1.0f / d;
	d = 1.0f - ((1.0f - max_y) - button_h);
	if (d <= 0.01f) d = 0.01f;
	float scale_up = 1.0f / d;
	int j;
	for (j = 0; j < count; j++) {
		if (Points[j].Button) {
			Points[j].X *= (Points[j].X < 0.0f) ? scale_left : scale_right;
			Points[j].Y *= (Points[j].Y < 0.0f) ? scale_down : scale_up;
		}
	}
	for (j = 0; j < count; j++) {
		UIMountPointStruct &p = Points[j];
		if (p.Button) {
			p.Y = (p.Y + 1.0f) * (vmax.Y - vmin.Y) * 0.5f + vmin.Y;
			p.X = ((p.X + 1.0f) * (vmax.X - vmin.X) * 0.5f + vmin.X) * 640.0f - 320.0f;
			p.X = (float)floor(p.X);
			p.Y = 240.0f - p.Y * 480.0f;
			p.Y = (float)floor(p.Y);
			p.Button->Set_Position(Vector3(p.X, p.Y, 0.0f));
			p.Line = (RenderObjClass *)new Unknown6DF890Class(Vector4(1.0f, 0.0f, 0.0f, 0.5f), *(Vector2 *)&p.X, *(Vector2 *)&p.PointX, 1.0f);
		}
	}
	Camera->Set_Transform(saved);
	Distance = tm.Get_Translation().Length();
	Model->Release_Ref();
	Model = 0;
	Unknown_BC();
	for (j = 0; j < count; j++) {
		if (Points[j].Button) Unknown_AC(Points[j].Bttn, 0);
	}
	Built = true;
}

// 0x5C85D0
void UIMountClass::Clear()
{
	Hide();
	for (int i = 0; i < (int)Unknown88.size(); i++) {
		UIMountPointStruct &p = Points[i];
		if (p.Name) {
			p.Bttn->Remove_Sub_Object(p.Name);
			p.Name->Release_Ref();
			p.Name = 0;
		}
		if (p.Bttn) {
			p.Bttn->Release_Ref();
			p.Bttn = 0;
		}
		if (p.Button) {
			p.Button->Release_Ref();
			p.Button = 0;
		}
		if (p.MntPoint) {
			p.MntPoint->Release_Ref();
			p.MntPoint = 0;
		}
		if (p.Line) {
			p.Line->Release_Ref();
			p.Line = 0;
		}
	}
	if (Model) Model->Release_Ref();
	Unknown_BC();
	Built = false;
	Distance = -1.0f;
}

/*
**	Notes.
**	Object 0x5C7360-0x5C885F: opened by permstr.h's PermStringInit (0x5C7360, its atexit 0x5C7390), closed by the
**	adjustor thunks 0x5C8850 / 0x5C8860 of the second and third tables. 0x5C8780 and 0x5C87D0 are rtti.h's Cast for
**	the Mounts and MountBoneNames arrays, out of line. 0x5C8710 (no caller: a name-to-bits table at 0xB829E0, "00",
**	"02" 0x20809, ...) is not rebuilt. The inline Is_Shown / Show / Hide are emitted with 0x58B680's file.
*/
