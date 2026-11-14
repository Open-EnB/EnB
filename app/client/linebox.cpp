// Earth & Beyond client (reconstruction): app\client\linebox.cpp. Notes at the end of the file.
#include "linebox.h"
#include "..\..\ww3d2\line3d.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\shared\g_assert.h"
#include <math.h>

// @0x6DFD80
LineBoxClass::LineBoxClass(float r, float g, float b, float width, float min_size) :
	Extent(0, 0, 0),
	NumLines(12),
	Width(width),
	MinSize(min_size)
{
	Lines = new Line3DClass *[NumLines];
	Target = NULL;
	for (int i = 0; i < NumLines; i++) {
		Lines[i] = new Line3DClass(Vector3(0, 0, 0), Vector3(1000, 1000, 1000), 10.0f, r, g, b);
	}
}



















// @0x6DFFD0
LineBoxClass::LineBoxClass(const LineBoxClass &src) :
	RenderObjClass(src),
	Extent(src.Extent),
	NumLines(src.NumLines),
	Width(src.Width),
	MinSize(src.MinSize)
{
	G_ASSERT(0);
}


// @0x6E00C0
LineBoxClass &LineBoxClass::operator=(const LineBoxClass &that)
{
	RenderObjClass::operator=(that);
	if (this != &that) {
		G_ASSERT(0);
	}
	return *this;
}

// @0x6E0100
// NOT EXACT: objdiff identical with relocations, stack offsets and targets masked (explain.py: identical); objmatch does not pair it (0.80), cause not found
LineBoxClass::~LineBoxClass()
{
	for (int i = 0; i < NumLines; i++) {
		Lines[i]->Release_Ref();
	}
	delete [] Lines;
	REF_PTR_RELEASE(Target);
}

// @0x6E0240
RenderObjClass *LineBoxClass::Clone() const
{
	return new LineBoxClass(*this);
}

// @0x6E0350
// NOT EXACT: 0.93, scheduling: the client copies -extent.Z for the lower corners later, and loads NumLines before clearing the last loop's counter
void LineBoxClass::Render(RenderInfoClass &rinfo)
{
	Vector3 center;
	Vector3 extent;
	if (Target != NULL) {
		SphereClass sphere = Target->Get_Bounding_Sphere();
		float radius = sphere.Radius;
		if (Target->Class_ID() == CLASSID_IMAGE3D) {
			radius = sphere.Radius * 0.75f;
		}
		center = sphere.Center;
		extent.Set(radius, radius, radius);
		Set_Position(center);
	} else {
		center = Get_Position();
		extent = Extent;
	}

	Matrix3D inv;
	rinfo.Camera.Get_Transform().Get_Inverse(inv);
	float dist = (float)fabs((inv * center).Z);
	if (dist <= 1.0f) {
		dist = 1.0f;
	}

	int i;
	if (Target != NULL) {
		float min = dist * MinSize * 0.5f;
		for (i = 0; i < 3; i++) {
			extent[i] = (min < extent[i]) ? extent[i] : min;
		}
	}

	Vector3 p0 = Vector3(-extent.X, -extent.Y, extent.Z) + center;
	Vector3 p1 = Vector3(extent.X, -extent.Y, extent.Z) + center;
	Vector3 p2 = Vector3(extent.X, extent.Y, extent.Z) + center;
	Vector3 p3 = Vector3(-extent.X, extent.Y, extent.Z) + center;
	Vector3 p4 = Vector3(-extent.X, -extent.Y, -extent.Z) + center;
	Vector3 p5 = Vector3(extent.X, -extent.Y, -extent.Z) + center;
	Vector3 p6 = Vector3(extent.X, extent.Y, -extent.Z) + center;
	Vector3 p7 = Vector3(-extent.X, extent.Y, -extent.Z) + center;

	float width = dist * Width;
	Lines[0]->Reset(p0, p1, width);
	Lines[1]->Reset(p1, p2, width);
	Lines[2]->Reset(p2, p3, width);
	Lines[3]->Reset(p3, p0, width);
	Lines[4]->Reset(p4, p5, width);
	Lines[5]->Reset(p5, p6, width);
	Lines[6]->Reset(p6, p7, width);
	Lines[7]->Reset(p7, p4, width);
	Lines[8]->Reset(p4, p0, width);
	Lines[9]->Reset(p7, p3, width);
	Lines[10]->Reset(p6, p2, width);
	Lines[11]->Reset(p5, p1, width);

	for (i = 0; i < NumLines; i++) {
		Lines[i]->Render(rinfo);
	}
}

// @0x6E0910
void LineBoxClass::Notify_Added(SceneClass *scene)
{
	RenderObjClass::Notify_Added(scene);
	scene->Register(this, SceneClass::ON_FRAME_UPDATE);
}

// @0x6E0940
void LineBoxClass::Notify_Removed(SceneClass *scene)
{
	scene->Unregister(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass::Notify_Removed(scene);
}

// @0x6E0970
void LineBoxClass::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	if (Target != NULL) {
		Target->Get_Obj_Space_Bounding_Sphere(sphere);
		return;
	}
	sphere.Center = Get_Position();
	sphere.Radius = Extent.Length();
}

// @0x6E0A30
void LineBoxClass::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	if (Target != NULL) {
		Target->Get_Obj_Space_Bounding_Box(box);
		return;
	}
	box.Center = Get_Position();
	box.Extent = Extent;
}

// @0x6E0AC0
void LineBoxClass::Update_Cached_Bounding_Volumes() const
{
	if (Target != NULL) {
		CachedBoundingBox = Target->Get_Bounding_Box();
		CachedBoundingSphere = Target->Get_Bounding_Sphere();
		Validate_Cached_Bounding_Volumes();
		return;
	}
	Vector3 pos = Get_Position();
	CachedBoundingBox.Center = pos;
	CachedBoundingBox.Extent = Extent;
	CachedBoundingSphere.Init(pos, Extent.Length());
	Validate_Cached_Bounding_Volumes();
}

// @0x6E0C20
int LineBoxClass::Get_Num_Polys() const
{
	return Lines[0]->Get_Num_Polys() * NumLines;
}



































































// @0x6E0C50
Line3DClass *LineBoxClass::Get_Line(int index)
{
	G_ASSERT(index >= 0);
	G_ASSERT(index < NumLines);
	Lines[index]->Add_Ref();
	return Lines[index];
}

// @0x6E0CD0
void LineBoxClass::Set_Target(RenderObjClass *target)
{
	if (Target != NULL) {
		Target->Release_Ref();
	}
	Target = target;
	Target->Add_Ref();
	Set_Position(Target->Get_Bounding_Box().Center);
}

// @0x6E0D40
void LineBoxClass::Set_Box(Vector3 max, Vector3 min)
{
	if (Target != NULL) {
		Target->Release_Ref();
		Target = NULL;
	}
	Extent = (max - min) * 0.5f;
	Set_Position((min + max) * 0.5f);
}

/*
**	Reconstruction notes (linebox.cpp, client 0x6DFD80-0x6E0E80; __FILE__ 0xB92BAC: G_ASSERT(0) on the
**	client's lines 51 (the copy constructor, also inlined in Clone) and 60 (operator=), 'index >= 0' /
**	'index < NumLines' on 272 / 273 (Get_Line); D25: blank lines keep them there).
**
**	LineBoxClass: twelve Line3DClass edges ((0, 0, 0) to (1000, 1000, 1000), width 10, the box's color).
**	Render places them each frame: around the target's bounding sphere (its radius three quarters for a
**	class id 5 object, each half size at least MinSize times half the camera distance) or around the
**	box's own position and Extent; the edges' width is Width times the camera distance (at least 1).
**	Class_ID (0x10007), On_Frame_Update and Get_Num_Lines are inline, emitted here; 0x6DFFA0 is the
**	scalar deleting destructor, 0x6E0E60 / 0x6E0E70 the compiler's adjustor thunks for the second and
**	third tables.
*/
