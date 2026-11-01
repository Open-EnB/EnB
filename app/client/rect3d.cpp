// Earth & Beyond client (reconstruction): app\client\rect3d.cpp. Notes at the end of the file.
#include "rect3d.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\shared\unknown_debug.h"

// NOT EXACT: 0.79, inline budget: the client calls Switch_To_Multi_Vertex_Color out of line (0x6B7230, its Convert_Color_Clamp inlined) and inlines all four Vertex calls; ours inlines Switch (calling Convert_Color_Clamp) and calls the last UV
Rect3DClass::Rect3DClass(const Vector4 &color, const Vector4 &rect) :
	DynamicMeshClass(2, 4)
{
	Bits |= IS_FORCE_VISIBLE;

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
	shader.Set_Cull_Mode(ShaderClass::CULL_MODE_DISABLE);
	Set_Shader(shader);
	Enable_Sort();

	Begin_Tri_Strip();
	Set_Vertex_Color(color);
	Vertex(rect.X, rect.Y, 0, 0, 0);
	Vertex(rect.Z + rect.X, rect.Y, 0, 0, 0);
	Vertex(rect.X, rect.Y - rect.W, 0, 0, 0);
	Vertex(rect.Z + rect.X, rect.Y - rect.W, 0, 0, 0);








	End_Tri_Strip();

	Set_Transform(Matrix3D(true));
	Set_Dirty();
}








































Rect3DClass::Rect3DClass(const Rect3DClass &src) :
	DynamicMeshClass(src)
{
	G_ASSERT(0);
}


Rect3DClass &Rect3DClass::operator=(const Rect3DClass &that)
{
	if (this != &that) {
		DynamicMeshClass::operator=(that);
		G_ASSERT(0);
	}
	return *this;
}

Rect3DClass::~Rect3DClass()
{
}

RenderObjClass *Rect3DClass::Clone() const
{
	return new Rect3DClass(*this);
}

void Rect3DClass::Notify_Added(SceneClass *scene)
{
	RenderObjClass::Notify_Added(scene);
	scene->Register(this, SceneClass::ON_FRAME_UPDATE);
}

void Rect3DClass::Notify_Removed(SceneClass *scene)
{
	scene->Unregister(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass::Notify_Removed(scene);
}

void Rect3DClass::Set_Color(const Vector4 &color)
{
	Change_Vertex_Color(0, color, 0);
	Change_Vertex_Color(1, color, 0);
	Change_Vertex_Color(2, color, 0);
	Change_Vertex_Color(3, color, 0);
}

void Rect3DClass::Set_Rect(const Vector4 &rect)
{
	Move_Vertex(0, rect.X, rect.Y, 0);
	Move_Vertex(1, rect.Z + rect.X, rect.Y, 0);
	Move_Vertex(2, rect.X, rect.Y - rect.W, 0);
	Move_Vertex(3, rect.Z + rect.X, rect.Y - rect.W, 0);
}

/*
**	Reconstruction notes (rect3d.cpp, client 0x6E2290-0x6E2B40; __FILE__ 0xB92CE8, G_ASSERT(0) on the
**	client's lines 92 (the copy constructor, inlined in Clone) and 100 (operator=); D25: blank lines keep
**	them there).
**
**	Rect3DClass: DynamicMeshClass(2, 4) with one vertex material (black, emissive white), the alpha solid
**	shader without depth test, depth writes or culling, sorted, one tri strip of four vertices in one
**	color (x, y), (x + w, y), (x, y - h), (x + w, y - h), an identity transform. Copying asserts.
**	Class_ID (0x1000A) is inline: the linker kept fontsys_cursor.obj's copy (0x685840), which the
**	cursor's table shares. 0x6E2B30 / 0x6E2B40 are the compiler's adjustor thunks for the second and
**	third tables.
*/
