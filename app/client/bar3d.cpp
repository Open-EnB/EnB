// Earth & Beyond client (reconstruction): app\client\bar3d.cpp. Notes at the end of the file.
#include "bar3d.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\wwmath\wwmath.h"
#include "..\shared\g_assert.h"

Bar3DClass::Bar3DClass(float r0, float g0, float b0, float a0, float r1, float g1, float b1, float a1,
	float height, float width, float height_scale, float width_scale, float offset, float offset_scale,
	float percent, float rate) :
	DynamicMeshClass(4, 8),
	Target(NULL),
	Offset(offset),
	OffsetScale(offset_scale),
	Height(height),
	Width(width),
	HeightScale(height_scale),
	WidthScale(width_scale),
	Rate(rate * 0.001f)
{
	if (percent < 0) percent = 0; else if (percent > 1) percent = 1;
	Percent.Set_Value(percent, true);

	VertexMaterialClass *material0 = new VertexMaterialClass;
	material0->Set_Diffuse(0, 0, 0);
	material0->Set_Specular(0, 0, 0);
	material0->Set_Ambient(0, 0, 0);
	material0->Set_Emissive(r0, g0, b0);
	if (a0 < 1.0f) {
		material0->Set_Opacity(a0);
	}

	VertexMaterialClass *material1 = new VertexMaterialClass;
	material1->Set_Diffuse(0, 0, 0);
	material1->Set_Specular(0, 0, 0);
	material1->Set_Ambient(0, 0, 0);
	material1->Set_Emissive(r1, g1, b1);
	if (a1 < 1.0f) {
		material1->Set_Opacity(a1);
	}

	Set_Shader(ShaderClass::_PresetAlphaSolidShader);
	if (a0 < 1.0f || a1 < 1.0f) {
		Enable_Sort();
	} else {
		Disable_Sort();
	}

	Begin_Tri_Strip();
	Set_Vertex_Material(material0, true);
	Vertex(Width * -0.5, Height * 0.5f, 0, 0, 0);
	Vertex(Width * -0.5, Height * -0.5f, 0, 0, 0);
	Vertex(Width * -0.5, Height * 0.5f, 0, 0, 0);
	Vertex(Width * -0.5, Height * -0.5f, 0, 0, 0);
	End_Tri_Strip();

	Begin_Tri_Strip();
	Set_Vertex_Material(material1, true);
	Vertex(Width * -0.5, Height * 0.5f, 0, 0, 0);
	Vertex(Width * -0.5, Height * -0.5f, 0, 0, 0);
	Vertex(Width * 0.5, Height * 0.5f, 0, 0, 0);
	Vertex(Width * 0.5, Height * -0.5f, 0, 0, 0);
	End_Tri_Strip();

	material0->Release_Ref();
	material1->Release_Ref();
	Set_Dirty();
}





































































































Bar3DClass::Bar3DClass(const Bar3DClass &src) :
	DynamicMeshClass(src),
	Target(src.Target),
	Offset(src.Offset),
	OffsetScale(src.OffsetScale),
	Height(src.Height),
	Width(src.Width),
	HeightScale(src.HeightScale),
	WidthScale(src.WidthScale),
	Rate(src.Rate),
	Percent(src.Percent)
{
	if (Target) {
		Target->Add_Ref();
	}
}


Bar3DClass &Bar3DClass::operator=(const Bar3DClass &that)
{
	DynamicMeshClass::operator=(that);
	if (this != &that) {
		G_ASSERT(0);
	}
	return *this;
}

Bar3DClass::~Bar3DClass()
{
	if (Target) {
		Target->Release_Ref();
	}
}

RenderObjClass *Bar3DClass::Clone() const
{
	return new Bar3DClass(*this);
}
// NOT EXACT: 0.93, shape: the client stores distance * OffsetScale and compares Offset against it in memory (fcom [esp+0x10], Offset kept on je); every Max / MAX / max / if / ternary form tried keeps it on the FPU stack (fcomp st(1)); std::_cpp_max stores it but selects through a pointer
void Bar3DClass::Render(RenderInfoClass &rinfo)
{
	SphereClass sphere = Target->Get_Bounding_Sphere();
	Matrix3D inv;
	rinfo.Camera.Get_Transform().Get_Inverse(inv);
	Vector3 cpos;
	Matrix3D::Transform_Vector(inv, sphere.Center, &cpos);
	Set_Position(Target->Get_Position());
	float scale = (float)fabs(cpos.Z);
	if (scale <= 1.0f) {
		scale = 1.0f;
	}
	float height = (scale * HeightScale < Height) ? Height : scale * HeightScale;
	float width = (scale * WidthScale < Width) ? Width : scale * WidthScale;
	float y = WWMath::Max(Offset, scale * OffsetScale) + sphere.Radius;


	Vector3 bottom_left(width * -0.5f, y - height * 0.5f, 0);
	Vector3 top_left(width * -0.5f, y + height * 0.5f, 0);
	Vector3 bottom_right(width * 0.5f, y - height * 0.5f, 0);
	Vector3 top_right(width * 0.5f, y + height * 0.5f, 0);





	Vector3 mid;
	Vector3::Lerp(bottom_left, bottom_right, Percent.Get_Current(), &mid);

	Move_Vertex(0, bottom_left.X, bottom_left.Y, bottom_left.Z);
	Move_Vertex(1, top_left.X, top_left.Y, top_left.Z);
	Move_Vertex(2, mid.X, mid.Y, mid.Z);
	Move_Vertex(3, mid.X, top_left.Y, top_left.Z);
	Move_Vertex(4, bottom_right.X, bottom_right.Y, bottom_right.Z);
	Move_Vertex(5, top_right.X, top_right.Y, top_right.Z);	Set_Dirty();

	DynamicMeshClass::Render(rinfo);
}

void Bar3DClass::On_Frame_Update()
{
	if (Percent.IsActive) {
		Percent.Update(WW3D::Get_Frame_Time());
	}
	Invalidate_Cached_Bounding_Volumes();
}

void Bar3DClass::Notify_Added(SceneClass *scene)
{
	RenderObjClass::Notify_Added(scene);
	scene->Register(this, SceneClass::ON_FRAME_UPDATE);
}

void Bar3DClass::Notify_Removed(SceneClass *scene)
{
	scene->Unregister(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass::Notify_Removed(scene);
}

void Bar3DClass::Update_Cached_Bounding_Volumes() const
{
	CachedBoundingSphere = Target->Get_Bounding_Sphere();
	CachedBoundingBox = Target->Get_Bounding_Box();
	Validate_Cached_Bounding_Volumes();
}

void Bar3DClass::Set_Target(RenderObjClass *target)
{
	if (Target) {
		Target->Release_Ref();
	}
	Target = target;
	Target->Add_Ref();
	Set_Position(Target->Get_Position());
}

void Bar3DClass::Set_Colors(float r0, float g0, float b0, float a0, float r1, float g1, float b1, float a1)
{
	MaterialInfoClass *info = Get_Material_Info();
	VertexMaterialClass *material = info->Peek_Vertex_Material(0);
	material->Set_Emissive(r0, g0, b0);
	if (a0 < 1.0f) {
		material->Set_Opacity(a0);
	}
	material = info->Peek_Vertex_Material(1);
	material->Set_Emissive(r1, g1, b1);
	if (a1 < 1.0f) {
		material->Set_Opacity(a1);
	}
	if (a0 < 1.0f || a1 < 1.0f) {
		Enable_Sort();
	} else {
		Disable_Sort();
	}
	info->Release_Ref();
}

/*
**	Reconstruction notes (bar3d.cpp, client 0x6B7DE0-0x6B8F20; __FILE__ 0xB92054, G_ASSERT(0) on the client's
**	line 198 (operator=); D25: blank lines keep it there).
**
**	Bar3DClass: DynamicMeshClass(4, 8) with two vertex materials (black, the given emissive colors and
**	opacities; sorted when either is translucent), the alpha solid shader, two tri strips of four vertices
**	(the left edge and the fill edge in the first material, the fill edge and the right edge in the second),
**	width x height centred on the origin. It has no transform of its own: Set_Target stores a ref-counted
**	render object whose bounding volumes it reports (Get_Obj_Space_Bounding_Sphere / _Box,
**	Update_Cached_Bounding_Volumes; On_Frame_Update invalidates them each frame). Render scales the bar by
**	the target's camera-space distance (at least 1), never below the minimum sizes, lifts it by the target's
**	bounding-sphere radius plus max(Offset, distance * OffsetScale) and moves the fill edge (vertices 2-5) to
**	the percent's current value. Both call Set_Position(Target->Get_Position()). The copy constructor copies
**	every member and takes a reference on the target; operator= asserts. The rate is per second: AccelClass
**	counts milliseconds (WW3D::Get_Frame_Time()). Class_ID (0x10005) and the two bounding forwarders are
**	inline: bar3d.obj emitted the copies (0x6B84C0, 0x6B84E0, 0x6B8510). 0x6B8F00 / 0x6B8F10 are the
**	compiler's adjustor thunks. Nothing in the client constructs one (no callers of 0x6B7DE0), and no percent
**	setter survives (none in the window).
*/
