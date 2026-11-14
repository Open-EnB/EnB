// Earth & Beyond client (reconstruction): app\client\gauge3d.cpp. Notes at the end of the file.
#include "gauge3d.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\wwmath\wwmath.h"
#include "..\shared\g_assert.h"


Gauge3DClass::Gauge3DClass(const Vector4 &color0, const Vector4 &color1, const Vector4 &color2, const Vector4 &rect,
	float value, float speed, int flags, int direction) :
	DynamicMeshClass(4, 8),
	Direction(direction),
	Flags(flags),
	Dirty(true),
	Speed(speed * 0.001f),
	Value(),
	Position(&rect.X),
	Size(&rect.Z),


	Color0(color0),
	Color1(color1),
	Color2(color2)
{
	Bits |= IS_FORCE_VISIBLE;
	Value.Set_Value(WWMath::Clamp(value, 0.0f, 1.0f), true);

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
	Set_Vertex_Color(Color0);
	Vertex(Position.X, Position.Y, 0, 0, 0);
	Vertex(Position.X + Size.X, Position.Y, 0, 0, 0);
	Vertex(Position.X, Position.Y - Size.Y, 0, 0, 0);
	Vertex(Position.X + Size.X, Position.Y - Size.Y, 0, 0, 0);
	End_Tri_Strip();

	Set_Transform(Matrix3D(true));
	Set_Dirty();
}










































































































// NOT EXACT: 0.83, order-only: the client pushes both of G_ASSERT's strings before storing Size.Y; ours stores between them
Gauge3DClass::Gauge3DClass(const Gauge3DClass &src) :
	DynamicMeshClass(src),
	Speed(src.Speed),
	Size(src.Size)
{
	G_ASSERT(0);
}














Gauge3DClass &Gauge3DClass::operator=(const Gauge3DClass &that)
{
	if (this != &that) {
		DynamicMeshClass::operator=(that);
		G_ASSERT(0);
	}
	return *this;
}


Gauge3DClass::~Gauge3DClass()
{
}


RenderObjClass *Gauge3DClass::Clone() const
{
	return new Gauge3DClass(*this);
}


void Gauge3DClass::Render(RenderInfoClass &rinfo)
{
	if (Dirty) {
		float value = Value.Get_Current();
		Vector4 color;
		if (Flags & FLAG_FADE) {
			color = Lerp(Color0, Color1, value);
		} else {
			color = Color1;
		}

		switch (Direction) {
		case 0:
		{
			float width = value * Size.X;
			Move_Vertex(1, width + Position.X, Position.Y, 0);
			Move_Vertex(3, width + Position.X, Position.Y - Size.Y, 0);
			Change_Vertex_Color(1, color, 0);
			Change_Vertex_Color(3, color, 0);
			if (!(Flags & FLAG_KEEP_EMPTY)) {
				Change_Vertex_Color(0, color, 0);
				Change_Vertex_Color(2, color, 0);
			}
			break;
		}
		case 1:
		{
			float width = value * Size.X;
			Move_Vertex(0, Position.X + Size.X - width, Position.Y, 0);
			Move_Vertex(2, Position.X + Size.X - width, Position.Y - Size.Y, 0);
			Change_Vertex_Color(0, color, 0);
			Change_Vertex_Color(2, color, 0);
			if (!(Flags & FLAG_KEEP_EMPTY)) {
				Change_Vertex_Color(1, color, 0);
				Change_Vertex_Color(3, color, 0);
			}
			break;
		}
		case 2:
		{
			float height = value * Size.Y;
			Move_Vertex(0, Position.X, Position.Y - Size.Y + height, 0);
			Move_Vertex(1, Size.X + Position.X, Position.Y - Size.Y + height, 0);
			Change_Vertex_Color(0, color, 0);
			Change_Vertex_Color(1, color, 0);
			if (!(Flags & FLAG_KEEP_EMPTY)) {
				Change_Vertex_Color(2, color, 0);
				Change_Vertex_Color(3, color, 0);
			}
			break;
		}
		case 3:
		{
			float height = value * Size.Y;
			Move_Vertex(2, Position.X, Position.Y - height, 0);
			Move_Vertex(3, Size.X + Position.X, Position.Y - height, 0);
			Change_Vertex_Color(2, color, 0);
			Change_Vertex_Color(3, color, 0);
			if (!(Flags & FLAG_KEEP_EMPTY)) {
				Change_Vertex_Color(0, color, 0);
				Change_Vertex_Color(1, color, 0);
			}
			break;
		}
		}
		Set_Dirty();
	}
	DynamicMeshClass::Render(rinfo);
}


void Gauge3DClass::On_Frame_Update()
{
	float old = Value.Get_Current();
	Value.Get_Value(WW3D::Get_Frame_Time());
	if (Value.Get_Current() != old) {
		Dirty = true;
	}
}


void Gauge3DClass::Notify_Added(SceneClass *scene)
{
	RenderObjClass::Notify_Added(scene);
	scene->Register(this, SceneClass::ON_FRAME_UPDATE);
	Dirty = true;
}


void Gauge3DClass::Notify_Removed(SceneClass *scene)
{
	scene->Unregister(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass::Notify_Removed(scene);
}


void Gauge3DClass::Set_Value(float value)
{
	value = WWMath::Clamp(value, 0.0f, 1.0f);
	if (Value.Get_Current() != value) {
		Value.Set_Value(value, true);
		Dirty = true;
	}
}


void Gauge3DClass::Set_Target(float value)
{
	Value.Set_Target(WWMath::Clamp(value, 0.0f, 1.0f), Speed);
}


void Gauge3DClass::Set_Colors(const Vector4 &color0, const Vector4 &color1)
{
	Dirty = true;
	Color0 = color0;
	Color1 = color1;
	if (Flags & FLAG_KEEP_EMPTY) {
		Change_Vertex_Color(0, Color0, 0);
		Change_Vertex_Color(1, Color0, 0);
		Change_Vertex_Color(2, Color0, 0);
		Change_Vertex_Color(3, Color0, 0);
	}
}

/*
**	Reconstruction notes (gauge3d.cpp, client 0x6D9360-0x6DA4D0; __FILE__ 0xB928A4, G_ASSERT(0) on the
**	client's lines 169 (the copy constructor 0x6D9980) and 189 (operator= 0x6D9A50); D25: blank lines keep
**	them there). fonttexture.cpp ends before 0x6D9360 (its notes); InterfaceSceneClass's constructor
**	0x6DA4D0 opens the next object.
**
**	Gauge3DClass: Rect3DClass's quad (the same vertex material, shader, sort and tri strip), but
**	DynamicMeshClass(4, 8), and the two vertices on the moving edge follow Value (an AccelClass, 0..1)
**	across the rectangle from the edge Direction names, in Color1, or with FLAG_FADE in Color0 faded to
**	Color1 by the value; without FLAG_KEEP_EMPTY the other two vertices take the same color. Render moves
**	them only when Dirty (set by On_Frame_Update when the value moved, by the setters and Notify_Added;
**	nothing clears it). Speed is the constructor's rate per second, kept per millisecond (WW3D's frame
**	time). The rectangle is two Vector2s: the constructor copies them as floats (Vector2(const float *))
**	and the copy constructor copies only Size, which Clone keeps out of line (with float members it
**	inlines the copy). Class_ID 0x1000B (0x6D9930) and the deleting destructor (0x6D9950) are inline;
**	0x6DA4B0 / 0x6DA4C0 are the compiler's adjustor thunks for the second and third tables. Nothing in
**	the client constructs one.
*/
