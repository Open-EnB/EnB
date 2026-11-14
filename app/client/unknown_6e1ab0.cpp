// Earth & Beyond client (reconstruction): app\client\unknown_6e1ab0.cpp (PROVISIONAL name). Notes at the end of the file.
#include "unknown_6e1ab0.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\wwmath\wwmath.h"

Unknown6E1AB0Class::Unknown6E1AB0Class(const Vector4 &color, const Vector2 &position, const Vector2 &size) :
	DynamicMeshClass(4, 8)
{
	float x = WWMath::Clamp(position.X);
	float y = WWMath::Clamp(position.Y);
	float w = WWMath::Clamp(size.X);
	float h = WWMath::Clamp(size.Y);
	x = x * 2 - 1;
	y = 1 - y * 2;
	h = h * 2;
	w = w * 2;

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
	Set_Vertex_Color(color);
	Vertex(0, 0, 0, 0, 0);
	Vertex(0, -h, 0, 0, 0);
	Vertex(w, 0, 0, 0, 0);
	Vertex(w, -h, 0, 0, 0);
	End_Tri_Strip();

	Matrix3D tm(true);
	tm.Set_Translation(Vector3(x, y, 0));
	Set_Transform(tm);
	Set_Dirty();
}

void Unknown6E1AB0Class::Set_Screen_Position(const Vector2 &position)
{
	float px = WWMath::Clamp(position.X);
	float py = WWMath::Clamp(position.Y);
	float x = px * 2 - 1;
	float y = 1 - py * 2;
	Set_Transform(Matrix3D(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, 0));
}

void Unknown6E1AB0Class::Set_Size(const Vector2 &size)
{
	float w = WWMath::Clamp(size.X) * 2;
	float h = WWMath::Clamp(size.Y) * 2;
	Move_Vertex(1, 0, -h, 0);
	Move_Vertex(2, w, 0, 0);
	Move_Vertex(3, w, -h, 0);
	Set_Dirty();
}

void Unknown6E1AB0Class::Set_Color(const Vector4 &color)
{
	Change_Vertex_Color(0, color, 0);
	Change_Vertex_Color(1, color, 0);
	Change_Vertex_Color(2, color, 0);
	Change_Vertex_Color(3, color, 0);
	Set_Dirty();
}

/*
**	Reconstruction notes (unknown_6e1ab0.cpp, client 0x6E1AB0-0x6E2290; PROVISIONAL file: the client holds no
**	string of it and no assert; the object links after local_interface_sort.obj (its <string> guard 0x6E1A80)
**	and before rect3d.obj (0x6E2290), so its name sorts between the two).
**
**	The constructor (0x6E1AB0) maps the position from 0..1 screen space to -1..1 (y down) and the size to twice
**	itself, builds one tri strip of four vertices (0, 0), (0, -h), (w, 0), (w, -h) in one color with the same
**	material and depth-less, sorted shader as Rect3DClass (culling left on), and places it by an identity
**	transform given the translation. Its form is the client's: `Matrix3D tm(true)` + Set_Translation (the
**	identity stores, then x, y and z last) spends the inline budget that leaves the last three vertices' UV
**	out of line (0x406ACD); x / y clamped and rescaled in place keep the client's parameter-slot reuse.
**	Set_Screen_Position (0x6E1F60) builds its matrix with the 12-float constructor (no Vector3 temporary:
**	frame 0x30). The destructor and Class_ID are inline (line2d.obj's copies 0x6DFA10, 0x6DF960); 0x6E1F30 is
**	the scalar deleting destructor, 0x6E2270 / 0x6E2280 the adjustor thunks for the second and third tables.
**	0x6E1E40-0x6E1F30 is int3 fill (240 bytes).
*/
