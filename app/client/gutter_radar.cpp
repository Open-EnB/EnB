// gutter_radar.cpp: the gutter icons and the two reticles (0x7B6BD0-0x7B7790); notes at the end
#include <string>
#include <math.h>
#include "gutter_radar.h"
#include "target_reticle.h"
#include "gutter_data.h"
#include "mouse_targeting.h"
#include "radar_system_data_cache.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "scene.h"
#include "camera.h"
#include "layer.h"
#include "wwmath.h"
#include "option.h"
#include "tasks\tsk_login.h"
#include "..\shared\g_assert.h"

float Unknown_831E90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius);	// 0x831E90 (cdecl): the distance between two spheres  // NAME 1: neutral (D10)












// @0x7B6BD0
GutterRadarClass::GutterRadarClass(SClient *client, const RefCountPtr<SimpleSceneClass> &scene) :
	Client(client),
	Scene(scene),
	Reticle08(NULL),
	Reticle0C(NULL),
	Position(0.0f, 0.0f, 0.0f),

	Size(0.0f),
	MouseTarget(NULL)
{
	Reticle08 = new TargetReticleClass(Client, scene, 0x1C, false);
	Reticle0C = new TargetReticleClass(Client, scene, 0x1A, true);
}

// @0x7B6D80
GutterRadarClass::~GutterRadarClass()
{
	delete Reticle08;
	delete Reticle0C;
}











































// @0x7B6E40
void GutterRadarClass::Set_Targets(ClientObjectClass *first, ClientObjectClass *second, MouseTargetClass *mouse_target)
{
	FAIL_IF(!Client) return;
	FAIL_IF(!Client->MainView) return;
	FAIL_IF(!Client->Mouse) return;

	G_ASSERT(!MouseTarget);
	MouseTarget = mouse_target;
	if (MouseTarget) {
		MouseTarget->Set(Client->Mouse->Get_X(), Client->Mouse->Get_Y(), 0.01875f, 0.025f,
			LayerClass(Client->MainView->Unknown_64(), (CameraClass *)Client->MainView->Unknown_7C()));
	}
	Reticle08->Set_Client_Object(first);
	Reticle0C->Set_Client_Object(second);
}


















// @0x7B7040
void GutterRadarClass::Update(RadarSystemDataCacheClass *entry)
{
	GutterDataClass *gdata = entry->Get_Gutter_Data(true);


	FAIL_IF(!gdata) return;

	ClientObjectClass *co = gdata->Peek_Client_Object();
	FAIL_IF(!co) return;

	Vector3 pos = co->Unknown_24();
	float radius = co->Unknown_30();
	bool on_screen = Project(pos, radius);
	float icon_scale = Get_Icon_Scale(pos, radius); float scale = gdata->Update(Position, Size, on_screen, Scene, icon_scale);
	if (co == Reticle08->Peek_Client_Object()) {
		Reticle08->Update(Position, Size);
	} else if (co == Reticle0C->Peek_Client_Object()) {
		Reticle0C->Update(Position, Size);
	}
	if (MouseTarget) {
		MouseTarget->Test(gdata, Position, Size, scale);
	}
	gdata->Update_Indicator(Position, Size);
}

// @0x7B71B0
void GutterRadarClass::Clear_Mouse_Target()
{
	MouseTarget = NULL;
}

// @0x7B71D0
// NOT EXACT: FPU order (the client keeps 1/z on the stack past the size's last use, fmul st(1) and a later fstp st(0); ours fmulp)
bool GutterRadarClass::Project(const Vector3 &pos, float radius)
{
	bool on_screen = false;
	CameraClass *camera = (CameraClass *)Client->MainView->Unknown_7C();
	Vector2 view_min, view_max;
	camera->Get_View_Plane(view_min, view_max);
	double width = view_max.X - view_min.X;
	double height = view_max.Y - view_min.Y;
	double x_scale = 1.0 / width;
	double y_scale = 1.0 / height;
	Vector3 view;
	camera->Transform_To_View_Space(view, pos);
	float z = (WWMath::Fabs(view.Z) > 0.0001f) ? view.Z : 0.0001f;
	double inv_z = 1.0 / z;
	float x = (float)(-(view.X * inv_z)) * x_scale;
	float y = view.Y * inv_z * y_scale;
	float abs_x = WWMath::Fabs(x);
	float abs_y = WWMath::Fabs(y);
	if ((abs_x < 0.490625f) && (abs_y < 0.4875f) && (z < 0.0f)) {
		Position.X = x;
		Position.Y = y;
		Position.Z = z;
		on_screen = true;
	} else {
		if (abs_x > abs_y) {
			Position.Y = ((float)(z * -0.490625f * width) * view.Y / WWMath::Fabs(view.X)) * inv_z * y_scale;
			float edge_x;
			if (view.X > 0.0f) {
				edge_x = 0.490625f;
			} else {
				edge_x = -0.490625f;
			}
			Position.X = edge_x;
		} else {
			Position.X = (float)(-(((float)(-(z * 0.4875f) * height) * view.X / WWMath::Fabs(view.Y)) * inv_z)) * x_scale;
			Position.Y = (view.Y > 0.0f) ? -0.4875f : 0.4875f;
		}
		Position.Z = z;
	}
	Position.X += 0.5f;
	Position.Y += 0.5f;
	float size = WWMath::Fabs((float)(-((view.X + radius) * inv_z)) * x_scale - x);
	Size = size;
	if (!on_screen) {
		float over_x = 0.0f;
		if (x < 0.0f) {
			over_x = -x;
		} else if (x > 1.0f) {
			over_x = x - 1.0f;
		}
		float over_y = 0.0f;
		if (y < 0.0f) {
			over_y = -y;
		} else if (y > 1.0f) {
			over_y = y - 1.0f;
		}
		float inset = (((over_y > over_x) ? over_y : over_x) < (size - 0.0125f)) ? ((over_y > over_x) ? over_y : over_x) : (size - 0.0125f);
		if (inset > 0.0f) {
			Size = size - inset;
		}
	}
	return(on_screen);
}

// @0x7B75E0
// NOT EXACT: tail merge (the client keeps the first return(1.0f)'s own epilogue; ours jumps to the last one)
float GutterRadarClass::Get_Icon_Scale(const Vector3 &pos, float radius)
{
	float min_distance = OptionClass::Instance->Get_Float("Gutter_Icon_Min_Distance");
	float max_distance = OptionClass::Instance->Get_Float("Gutter_Icon_Max_Distance");
	float min_scale = OptionClass::Instance->Get_Float("Gutter_Icon_Min_Scale");
	if ((min_scale >= 0.0f) && (min_distance > 0.0f) && (max_distance > 0.0f) && (max_distance > min_distance)) {
		CameraClass *camera = (CameraClass *)Client->MainView->Unknown_7C();
		if (camera) {
			Vector3 camera_pos = camera->Get_Transform().Get_Translation();
			float distance = Unknown_831E90(camera_pos, 1.0f, pos, radius);
			if (distance < min_distance) {
				return(1.0f);
			} else if (distance > max_distance) {
				return(min_scale);
			} else return(1.0f - ((distance - min_distance) / (max_distance - min_distance)) * (1.0f - min_scale));
		}
	}
	return(1.0f);
}

/*
**	Notes
**
**	The object: 0x7B6BD0 (the constructor of the class 0x7B6E40's asserts belong to) to 0x7B7790, <string>'s
**	guarded initialiser, which ends it. mouse_targeting.cpp's 0x7B77C0 opens the next.
*/
