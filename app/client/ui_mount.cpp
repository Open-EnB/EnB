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
#include "..\shared\auxdata\delta_numbering.h"
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (avatar.cpp's declaration)
#include "..\common\itemtype.h"	// Unknown_843440 (0x843440): a mount's type as text

#include "line2d.h"	// Line2DClass (0x6DF890): the line from a button to its mount point







// 0xBF9330 is Matrix3D::Identity (relocaudit's data map: 11 of 11 exact readers; R6, run 2026-11-24-night)
// 0xBF92D0 is Matrix3D::RotateZ90 (matrix3d.obj's 8th initialiser 0x8C2380 fills it with 0 -1 0 / 1 0 0 / 0 0 1; P3, run 2026-11-24-night)

// 0x5C73B0
UIMountClass::UIMountClass(SClient *client, CameraClass *camera, int kind, int unused) :
	UIInvClass(client, kind),
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
	UIInvClass::Unknown_54();
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
		UIInvClass::Unknown_58();
	}
}

// 0x5C7600
void UIMountClass::Unknown_5C()
{
	Hide();
	UIInvClass::Unknown_5C();
}


















































































































// 0x5C7630
void UIMountClass::Toggle()
{
	int i;
	if (!Shown) {
		FAIL_IF(!View) return;
		SceneClass *scene = (SceneClass *)View->Unknown_50();
		FAIL_IF(!scene) return;
		for (i = 0; i < (int)Buttons.size(); i++) {
			UIMountPointStruct &p = Points[i];
			if (p.Button) p.Button->Add(scene);
			if (p.MntPoint) p.MntPoint->Add(scene);
			if (p.Line) p.Line->Add(scene);
		}
		Shown = true;
		return;
	}
	for (i = 0; i < (int)Buttons.size(); i++) {
		UIMountPointStruct &p = Points[i];
		if (p.Button) p.Button->Remove();
		if (p.MntPoint) p.MntPoint->Remove();
		if (p.Line) p.Line->Remove();
	}
	Shown = false;
}











































// NOT EXACT (1.00, attempts.py 0x5C77A0): r53/r54 p.Y floor intermediate store (3 masked); 60/62 regions and 0x164 frame match
// 0x5C77A0
void UIMountClass::Build()
{
	if (Built) return;
	Clear();
	ClientObjectClass *cobj = ((ClientClass *)Client)->Get_Avatar();


	FAIL_IF(!cobj) return;

	DeltaCompoundClass *auxdata = (DeltaCompoundClass *)cobj->AuxData;
	FAIL_IF(!auxdata) return;
	DeltaStringClass *model_name;
	{ NameClass model_field("Inventory.EquipMountModel"); model_name = Unknown_Find_Set_Field<DeltaStringClass>(auxdata, model_field); }
	if (!model_name) return;
	const char *model_file = model_name->Get();
	DeltaArrayClass<InvSlotStruct> *mounts;
	{ NameClass mounts_field("Inventory.Mounts"); mounts = Unknown_Find_Set_Field<DeltaArrayClass<InvSlotStruct> >(auxdata, mounts_field); }
	if (!mounts) return;
	NameClass bones_field("Inventory.MountBoneNames");
	DeltaItemArrayClass<DeltaStringClass> *bone_names = Unknown_Find_Set_Field<DeltaItemArrayClass<DeltaStringClass> >(auxdata, bones_field);
	if (!bone_names) return;

	Model = AssetManager->Create_Render_Obj(model_file);
	if (!Model) {
		AssetManager->Load_3D_Assets(model_file);
		Model = AssetManager->Create_Render_Obj(model_file);
		if (!Model) return;
	}
	if (Unknown402) {
		Model->Set_Transform(Matrix3D::RotateZ90);
	} else {
		Model->Set_Transform(Matrix3D::Identity);
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
	while (fabs(step) > 0.5) {
		Vector3 dest;
		bool inside = false;
		tm.Translate_Z(step);
		Camera->Set_Transform(tm);
		if (Camera->Project(dest, tl) == CameraClass::INSIDE_FRUSTUM && Camera->Project(dest, br) == CameraClass::INSIDE_FRUSTUM) {
			inside = true;
		}
		if ((step > 0.0f) == inside) {
			step *= -0.5f;
		}
	}

	int count = mounts->Children.size();
	float button_w = -1.0f;
	float button_h = -1.0f;
	const ViewportClass &vp = Camera->Get_Viewport();
	Vector2 vmin = vp.Min;
	Vector2 vmax = vp.Max;
	float min_x = 0.0f;
	float min_y = 0.0f;
	float max_x = 0.0f;
	float max_y = 0.0f;





















































	for (int i = 0; i < count; i++) {
		InvSlotStruct *mount = &mounts->Get_Value(i);
		if (!mount->I00) continue;
		DeltaStringClass *ds = (DeltaStringClass *)bone_names->Children[i]; const char *bone_name = ds->Get();
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
		Vector3 point; float px = (vmax.X - vmin.X) * screen.X + vmin.X;
		point.Y = (vmax.Y - vmin.Y) * screen.Y + vmin.Y;
		point.X = px * 640.0f - 320.0f;
		point.X = (float)floor(point.X);
		point.Y = 240.0f - point.Y * 480.0f;
		point.Y = (float)floor(point.Y);
		((Vector2 *)&p.PointX)->Set(point.X, point.Y);
		
		point.Z = 0.0f;
		screen.X = screen.X + screen.X - 1.0f;
		screen.Y = screen.Y + screen.Y - 1.0f;
		((Vector2 *)&p.X)->Set(screen.X, screen.Y);
		
		if (min_x > screen.X) min_x = screen.X; else if (max_x < screen.X) max_x = screen.X;
		if (min_y > screen.Y) min_y = screen.Y; else if (max_y < screen.Y) max_y = screen.Y;
		p.Button = WW3DAssetManager::Get_Instance()->Create_Render_Obj("Equipbtn");
		Unknown_496AE0(p.Button, false);
		p.Bttn = p.Button->Get_Sub_Object_By_Name("bttn");
		p.Unknown0C = p.Button->Get_Sub_Object_Bone_Index(p.Bttn);
		p.MntPoint = WW3DAssetManager::Get_Instance()->Create_Render_Obj("MntPoint");
		p.MntPoint->Set_Position(point);
		p.Name = new FontSys("MOUNT_NAME", 0, Unknown_843440(mount->I00));
		p.Button->Add_Sub_Object_To_Bone(p.Name, "frame");
		if (button_w == -1.0f) {
			AABoxClass box;
			p.Button->Get_Obj_Space_Bounding_Box(box);
			float w2 = box.Extent.X + box.Extent.X; float h2 = box.Extent.Y + box.Extent.Y;
			button_w = w2 + 4.0; button_h = h2 + 4.0;
		}
	}

	// spread the buttons so the outermost fit the view
	button_w = button_w * (1.0f / 640.0f);
	button_h = button_h * (1.0f / 480.0f); float w = button_w + button_w;
	float d = 1.0f - (float)((1.0 - -min_x) - w);
	if (d <= 0.01f) d = 0.01f;
	float scale_left = 1.0f / d;
	double dd = 1.0 - ((1.0f - max_x) - w);
	if (dd <= 0.01f) dd = 0.01f;
	float scale_right = (float)(1.0 / dd);
	dd = 1.0 - ((1.0 - -min_y) - button_h);
	if (dd <= 0.01f) dd = 0.01f;
	float scale_down = (float)(1.0 / dd);
	dd = 1.0 - ((1.0f - max_y) - button_h);
	if (dd <= 0.01f) dd = 0.01f;
	float scale_up = (float)(1.0 / dd);
	int j;
	for (j = 0; j < count; j++) {
		UIMountPointStruct &p = Points[j]; if (p.Button) {
			p.X *= (p.X < 0.0f) ? scale_left : scale_right;
			p.Y *= (p.Y < 0.0f) ? scale_down : scale_up;
		}
	}
	for (j = 0; j < count; j++) {
		UIMountPointStruct &p = Points[j];
		if (p.Button) {
			p.Y = (p.Y + 1.0f) * (vmax.Y - vmin.Y) * 0.5f + vmin.Y;
			p.X = ((p.X + 1.0f) * (vmax.X - vmin.X) * 0.5f + vmin.X) * 640.0f - 320.0f;
			p.X = (float)floor(p.X);
			
			p.Y = (float)floor(240.0f - p.Y * 480.0f);
			p.Button->Set_Position(Vector3(p.X, p.Y, 0.0f));
			p.Line = new Line2DClass(Vector4(1.0f, 0.0f, 0.0f, 0.5f), *(Vector2 *)&p.X, *(Vector2 *)&p.PointX, 1.0f);
		}
	}
	Camera->Set_Transform(saved);
	Distance = tm.Get_Translation().Length();
	Model->Release_Ref();
	Model = 0;
	Unknown_BC();
	for (j = 0; j < count; j++) {
		UIMountPointStruct *p = &Points[j]; if (p->Button != 0) Unknown_AC(p->Bttn, 0);
	}
	Built = true;
}

// 0x5C85D0
void UIMountClass::Clear()
{
	Hide();
	for (int i = 0; i < (int)Buttons.size(); i++) {
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


// A mount's bits as the two-digit name of its art (no caller in the client)
struct UnknownB829E0Struct { const char *Name; int Bits; };	// NAME 1: neutral (D10)
UnknownB829E0Struct UnknownB829E0[] = {	// 0xB829E0  // NAME 1: neutral (D10)
	{ "00", 0 }, { "02", 0x20809 }, { "01", 0x1C002 }, { "03", 0x1C022 }, { "07", 0x40 }, { "06", 0x80 }, { "05", 0x100 },
	{ 0, 0 }
};


// 0x5C8710
const char *Unknown_5C8710(int bits) {
	UnknownB829E0Struct *e;
	const char *name = UnknownB829E0[0].Name;
	if (name == 0) return(0);
	for (e = UnknownB829E0; name; name = (++e)->Name) {
		if (bits == e->Bits) return(name);
		if (!bits) break;
	} return(0);
}

/*
**	Notes.
**	Object 0x5C7360-0x5C885F: opened by permstr.h's PermStringInit (0x5C7360, its atexit 0x5C7390), closed by the
**	adjustor thunks 0x5C8850 / 0x5C8860 of the second and third tables. 0x5C8780 and 0x5C87D0 are rtti.h's Cast for
**	the Mounts and MountBoneNames arrays, out of line. 0x5C8710 has no caller (the table 0xB829E0, "00",
**	"02" 0x20809, ...). The inline Is_Shown / Show / Hide are emitted with 0x58B680's file.
*/
