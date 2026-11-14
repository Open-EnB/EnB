// Earth & Beyond client (reconstruction): app\client\bar2d.cpp. Notes at the end of the file.
#include "bar2d.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\wwmath\wwmath.h"
#include "..\shared\g_assert.h"
// NOT EXACT: 0.76, shape (inline budget): the client inlines Rotate_Z only in case 1 (2 and 3 call 0x577940) and Set_Dirty, and calls the last vertex's UV (0x6B71E0); ours inlines Rotate_Z in cases 1 and 2, all eight UVs and calls Set_Dirty
Bar2DClass::Bar2DClass(const Vector4 &color0, const Vector4 &color1, const Vector4 &color2, const Vector4 &color3,
	float height, float width, float x, float y, float percent, float rate, int orientation) :
	DynamicMeshClass(4, 8),
	Rate(rate * 0.001f),
	Color0(color0),
	Color1(color1),
	Color2(color2),
	Color3(color3)
{
	Bits |= IS_FORCE_VISIBLE;

	if (percent < 0) percent = 0; else if (percent > 1) percent = 1;
	if (x < 0) x = 0; else if (x > 1) x = 1;
	if (y < 0) y = 0; else if (y > 1) y = 1;
	if (height < 0) height = 0;
	if (width < 0) width = 0;
	Height = height <= 1 ? height : 1;
	Width = width <= 1 ? width : 1;




	x = x * 2 - 1.0f;
	y = 1.0f - y * 2;
	Height *= 2;
	Width *= 2;
	Percent.Set_Value(percent, true);

	VertexMaterialClass *material = new VertexMaterialClass;
	material->Set_Diffuse(0, 0, 0);
	material->Set_Specular(0, 0, 0);
	material->Set_Ambient(0, 0, 0);
	material->Set_Emissive(1, 1, 1);
	Set_Vertex_Material(material, true);
	material->Release_Ref();

	ShaderClass shader = ShaderClass::_PresetAlphaSolidShader;
	shader.Set_Depth_Compare(ShaderClass::PASS_ALWAYS);
	shader.Set_Depth_Mask(ShaderClass::DEPTH_WRITE_DISABLE);
	Set_Shader(shader);
	Enable_Sort();

	Begin_Tri_Strip();
	Set_Vertex_Color(Color0);
	Vertex(0, 0, 0, 0, 0);
	Vertex(0, -Height, 0, 0, 0);
	Vertex(0, 0, 0, 0, 0);
	Vertex(0, -Height, 0, 0, 0);
	End_Tri_Strip();

	Begin_Tri_Strip();
	Set_Vertex_Color(Color3);
	Vertex(0, 0, 0, 0, 0);
	Vertex(0, -Height, 0, 0, 0);
	Vertex(Width, 0, 0, 0, 0);
	Vertex(Width, -Height, 0, 0, 0);
	End_Tri_Strip();

	Matrix3D tm(true);
	switch (orientation) {
		case 1:
			tm.Rotate_Z(WWMATH_PI);
			y -= Height;
			x += Width;
			break;
		case 2:
			tm.Rotate_Z(WWMATH_PI / 2);
			y -= Width;
			break;
		case 3:
			tm.Rotate_Z(WWMATH_PI * 1.5f);
			x += Height;
			break;
	}
	tm.Set_Translation(Vector3(x, y, 0));
	Set_Transform(tm);
	Set_Dirty();
}




























































































Bar2DClass::Bar2DClass(const Bar2DClass &src) :
	DynamicMeshClass(src),
	Rate(src.Rate),
	Percent(src.Percent),
	Width(src.Width),
	Height(src.Height)
{
	G_ASSERT(0);
}














Bar2DClass &Bar2DClass::operator=(const Bar2DClass &that)
{
	if (this != &that) {
		DynamicMeshClass::operator=(that);
		G_ASSERT(0);
	}
	return *this;
}

Bar2DClass::~Bar2DClass()
{
}

RenderObjClass *Bar2DClass::Clone() const
{
	return new Bar2DClass(*this);
}

void Bar2DClass::Render(RenderInfoClass &rinfo)
{
	float percent = Percent.Get_Current();
	float x = percent * Width;
	Vector4 filled = Vector4::Lerp(Color0, Color1, percent);
	Vector4 empty = Vector4::Lerp(Color2, Color3, percent);

	Move_Vertex(2, x, 0, 0);
	Move_Vertex(3, x, -Height, 0);
	Move_Vertex(4, x, 0, 0);
	Move_Vertex(5, x, -Height, 0);
	Change_Vertex_Color(2, filled, 0);
	Change_Vertex_Color(3, filled, 0);
	Change_Vertex_Color(4, empty, 0);
	Change_Vertex_Color(5, empty, 0);
	Set_Dirty();

	DynamicMeshClass::Render(rinfo);
}

void Bar2DClass::On_Frame_Update()
{
	if (Percent.IsActive) {
		Percent.Update(WW3D::Get_Frame_Time());
	}
}

void Bar2DClass::Notify_Added(SceneClass *scene)
{
	RenderObjClass::Notify_Added(scene);
	scene->Register(this, SceneClass::ON_FRAME_UPDATE);
}

void Bar2DClass::Notify_Removed(SceneClass *scene)
{
	scene->Unregister(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass::Notify_Removed(scene);
}



















































































void Bar2DClass::Set_Percent(float percent)
{
	G_ASSERT(percent >= 0.0f && percent <= 1.0f);
	percent = WWMath::Clamp(percent);
	Percent.Set_Target(percent, Rate);
}

void Bar2DClass::Set_Percent_Now(float percent)
{
	G_ASSERT(percent >= 0.0f && percent <= 1.0f);
	percent = WWMath::Clamp(percent);
	Percent.Set_Value(percent, true);
}

void Bar2DClass::Set_Colors(const Vector4 &color0, const Vector4 &color1, const Vector4 &color2, const Vector4 &color3)
{
	Color0 = color0;
	Color1 = color1;
	Color2 = color2;
	Color3 = color3;
	Change_Vertex_Color(0, Color0, 0);
	Change_Vertex_Color(1, Color0, 0);
	Change_Vertex_Color(6, Color3, 0);
	Change_Vertex_Color(7, Color3, 0);
}

/*
**	Reconstruction notes (bar2d.cpp, client 0x6B6900-0x6B7DE0; __FILE__ 0xB91FFC, G_ASSERT(0) on the client's
**	lines 188 (the copy constructor) and 208 (operator=), the percent asserts on 345 and 352; D25: blank lines
**	keep them there).
**
**	Bar2DClass: DynamicMeshClass(4, 8) with one vertex material (black, emissive white), the alpha solid shader
**	without depth test or depth writes, sorted, two tri strips of four vertices: the left edge and the fill
**	edge in the first color, the fill edge and the right edge in the fourth. Render moves the fill edge
**	(vertices 2-5) to the percent's current value and shades it between the first and second colors (the
**	filled side) and the third and fourth (the empty side). The constructor clamps x, y and the percent to
**	0..1, maps x, y from 0..1 to -1..1 (y down), doubles the clamped width and height, and turns the bar by
**	180, 90 or 270 degrees for orientation 1, 2 or 3 (moved so it covers the same corner). The rate is per
**	second: AccelClass counts milliseconds (WW3D::Get_Frame_Time()). Copying asserts.
**	Class_ID (0x10004) is inline: bar2d.obj emitted the copy (0x6B7480). The constructor emitted
**	DynamicMeshClass::UV (0x6B71E0) and Switch_To_Multi_Vertex_Color (0x6B7230) out of line before it;
**	0x6B7DC0 / 0x6B7DD0 are the compiler's adjustor thunks for the second and third tables. Nothing in the
**	client constructs one (no callers of 0x6B6900).
*/
