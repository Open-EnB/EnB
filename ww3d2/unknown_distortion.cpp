// Earth & Beyond client (reconstruction)
/*
**	E&B's screen-distortion effect (decision D19); see unknown_distortion.h. Names are neutral (D10).
*/


#include "unknown_distortion.h"
#include "vector.h"
#include "dx8wrapper.h"
#include "dx8caps.h"
#include "ww3d.h"
#include "camera.h"
#include "vector2.h"
#include "sphere.h"
#include "aabox.h"
#include "wwmath.h"
#include "matrix4.h"
#include "texture.h"
#include "surfaceclass.h"
#include "shader.h"
#include "vertmaterial.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "dx8fvf.h"
#include <math.h>


/*
** The objects to draw this frame (0xC0BAA8), and whether the hardware can draw them (0xC0BAD8).
** The list has external linkage: as a file static, VC6 loads the inlined Add's vtable before the
** growth sum (0x995E20, 0x996180 differ); SoundSceneObjClass's global list has the client's order.
*/
DynamicVectorClass<UnknownDistortionClass *>			DistortionList;
static bool														DistortionEnabled;


// Called when a render device is set (0x995DC0): the back buffer must fit in a texture
void Unknown_Reset_Distortion_List(void)
{
	DistortionList.Reset_Active();
	DistortionEnabled = false;

	const DX8Caps * caps = DX8Wrapper::Get_Current_Caps();
	if (caps->Support_Cubemaps()) {
		int width, height, bits;
		bool windowed;
		WW3D::Get_Device_Resolution(width, height, bits, windowed);
		if (width <= 1024 && (int)caps->Get_DX8_Caps().MaxTextureWidth >= width &&
			(int)caps->Get_DX8_Caps().MaxTextureWidth >= height) {
			DistortionEnabled = true;
		}
	}
}

// 0x995E20
void Unknown_Add_To_Distortion_List(UnknownDistortionClass * obj)
{
	DistortionList.Add(obj);
}

// Draws this frame's objects and empties the list (0x995E80)
void Unknown_Render_Distortion_List(CameraClass & camera)
{
	if (DistortionEnabled) {
		for (int i = 0; i < DistortionList.Count(); i++) {
			DistortionList[i]->Unknown_Render_Distortion(camera);
		}
	}
	DistortionList.Reset_Active();
}


/*
** UnknownDistortionClass
*/
UnknownDistortionClass::UnknownDistortionClass(void) :
	Unknown90(1.0f)
{
}

UnknownDistortionClass::UnknownDistortionClass(const UnknownDistortionClass & src) :
	RenderObjClass(src)
{
	Unknown90 = src.Unknown90;
	Unknown8C = src.Unknown8C;
}

UnknownDistortionClass::~UnknownDistortionClass(void)
{
	Unknown8C = NULL;
}

RenderObjClass * UnknownDistortionClass::Clone(void) const
{
	return NEW_REF(UnknownDistortionClass, (*this));
}

void UnknownDistortionClass::Render(RenderInfoClass & rinfo)
{
	Unknown_Add_To_Distortion_List(this);
}

// Copies the back buffer under the object's screen square to a texture and draws it back through a
// grid of 8-pixel cells whose texture coordinates the distortion function displaces (0x9961E0)
void UnknownDistortionClass::Unknown_Render_Distortion(CameraClass & camera)
{
	if (!Unknown8C) return;

	// The square facing the camera, to whole pixels outwards
	Vector3 corner0;
	Vector3 corner1;
	Transform.Get_Translation(&corner0);
	Transform.Get_Translation(&corner1);
	Vector3 left = -camera.Get_Transform().Get_X_Vector() * Unknown90;
	Vector3 up = camera.Get_Transform().Get_Y_Vector() * Unknown90;
	corner0 += left + up;
	corner1 += -left - up;

	int width, height, bits;
	bool windowed;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);

	Vector3 screen0, screen1;
	camera.Project(screen0, corner0);
	int x0 = (int)floor((screen0.X + 1.0f) * width * 0.5f);
	int y0 = (int)floor((1.0f - screen0.Y) * height * 0.5f);
	screen0.X = (float)(x0 * 2) / width - 1.0f;
	screen0.Y = 1.0f - (float)(y0 * 2) / height;
	camera.Project(screen1, corner1);
	int x1 = (int)ceil((screen1.X + 1.0f) * width * 0.5f);
	int y1 = (int)ceil((1.0f - screen1.Y) * height * 0.5f);
	screen1.X = (x1 + 0.5f) * 2.0f / width - 1.0f;
	screen1.Y = 1.0f - (y1 + 0.5f) * 2.0f / height;
	screen0.Z = 0.0f;
	screen1.Z = 0.0f;
	float dx = screen1.X - screen0.X;
	float dy = screen1.Y - screen0.Y;

	// The part of it on screen, and where that lies in the square's texture coordinates
	int cx0 = x0 < 0 ? 0 : x0;
	int cy0 = y0 < 0 ? 0 : y0;
	int cx1 = x1 >= width ? width - 1 : x1;
	int cy1 = y1 >= height ? height - 1 : y1;
	int sw = x1 - x0;
	int sh = y1 - y0;
	int w = cx1 - cx0;
	int h = cy1 - cy0;
	Vector2 uv0 = Vector2(cx0 - x0, cy0 - y0) / (float)sw;
	Vector2 uv1 = Vector2(cx1 - x0, cy1 - y0) / (float)sh;	// (sic: uv0 is scaled by the width alone, uv1 by the height alone)
	float su = (float)sw / w;
	float sv = (float)sh / h;

	if (w <= 8 || h <= 8) return;

	SurfaceClass * back_buffer = DX8Wrapper::_Get_DX8_Back_Buffer();
	if (back_buffer == NULL) return;
	SurfaceClass::SurfaceDescription desc;
	back_buffer->Get_Description(desc);
	TextureClass * texture = NEW_REF(TextureClass, (w, h, desc.Format, MIP_LEVELS_1, TextureClass::POOL_DEFAULT, false));
	if (texture == NULL) {
		back_buffer->Release_Ref();
		return;
	}
	texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	texture->Set_Min_Filter(TextureClass::FILTER_TYPE_NONE);
	texture->Set_Mag_Filter(TextureClass::FILTER_TYPE_NONE);
	SurfaceClass * surface = texture->Get_Surface_Level();
	surface->Copy(0, 0, cx0, cy0, w, h, back_buffer);

	// Drawn in screen space
	Matrix4 identity(true);
	Matrix4 view;
	Matrix4 projection;
	DX8Wrapper::Get_Transform(D3DTS_VIEW, view);
	DX8Wrapper::Get_Transform(D3DTS_PROJECTION, projection);

	ShaderClass shader = ShaderClass::_PresetOpaque2DShader;
	shader.Set_Depth_Compare(ShaderClass::PASS_ALWAYS);
	DX8Wrapper::Set_Shader(shader);
	VertexMaterialClass * material = VertexMaterialClass::Get_Preset(VertexMaterialClass::PRELIT_NODIFFUSE);
	DX8Wrapper::Set_Material(material);
	REF_PTR_RELEASE(material);
	DX8Wrapper::Set_Transform(D3DTS_WORLD, identity);
	DX8Wrapper::Set_Transform(D3DTS_VIEW, identity);
	DX8Wrapper::Set_Transform(D3DTS_PROJECTION, identity);
	DX8Wrapper::Set_Texture(0, texture);

	// (these early returns leave the view and projection unrestored)
	int nx = sw >> 3;
	int ny = sh >> 3;
	if (nx < 2) {
		REF_PTR_RELEASE(surface);
		texture->Release_Ref();
		back_buffer->Release_Ref();
		return;
	}
	if (ny < 2) {
		REF_PTR_RELEASE(surface);
		texture->Release_Ref();
		back_buffer->Release_Ref();
		return;
	}

	static DynamicVectorClass<Vector3> verts;
	static DynamicVectorClass<Vector2> uvs;
	static DynamicVectorClass<unsigned short> indices;
	int vertex_count = nx * ny;
	int index_count = (nx - 1) * 6 * (ny - 1);
	verts.Reset_Active();
	uvs.Reset_Active();
	indices.Reset_Active();

	int i, j;
	for (j = 0; j < ny; j++) {
		for (i = 0; i < nx; i++) {
			Vector2 uv(i / (nx - 1.0f), j / (ny - 1.0f));
			Vector2 offset(uv.X * dx, uv.Y * dy);
			verts.Add(Vector3(screen0.X + offset.X, screen0.Y + offset.Y, screen0.Z));
			uvs.Add(uv);
		}
	}
	for (j = 0; j < ny - 1; j++) {
		for (i = 0; i < nx - 1; i++) {
			int i0 = j * nx + i;
			int i1 = (j + 1) * nx + i;
			indices.Add(i0);
			indices.Add(i1);
			indices.Add(i0 + 1);
			indices.Add(i0 + 1);
			indices.Add(i1);
			indices.Add(i1 + 1);
		}
	}

	// Distort, then map from the square to the copied part (outside it: off the texture)
	for (i = 0; i < vertex_count; i++) {
		Vector2 uv = uvs[i];
		Unknown8C->Unknown_Distort(uv, this);
		float u = uv.X < uv0.X ? -1.0f : (uv.X > uv1.X ? 2.0f : (uv.X - uv0.X) * su);
		float v = uv.Y < uv0.Y ? -1.0f : (uv.Y > uv1.Y ? 2.0f : (uv.Y - uv0.Y) * sv);
		uvs[i].X = u;
		uvs[i].Y = v;
	}

	{
		DynamicVBAccessClass vb_access(BUFFER_TYPE_DYNAMIC_DX8, dynamic_fvf_type, vertex_count);
		{
			DynamicVBAccessClass::WriteLockClass lock(&vb_access);
			unsigned char * vb = (unsigned char *)lock.Get_Formatted_Vertex_Array();
			const FVFInfoClass & fvf_info = vb_access.FVF_Info();
			for (i = 0; i < vertex_count; i++) {
				*(Vector3 *)(vb + fvf_info.Get_Location_Offset()) = verts[i];
				*(Vector2 *)(vb + fvf_info.Get_Tex_Offset(0)) = uvs[i];
				vb += fvf_info.Get_FVF_Size();
			}
		}
		DynamicIBAccessClass ib_access(BUFFER_TYPE_DYNAMIC_DX8, index_count);
		{
			DynamicIBAccessClass::WriteLockClass lock(&ib_access);
			unsigned short * ib = lock.Get_Index_Array();
			for (i = 0; i < index_count; i++) {
				ib[i] = indices[i];
			}
		}
		DX8Wrapper::Set_Vertex_Buffer(vb_access);
		DX8Wrapper::Set_Index_Buffer(ib_access, 0);
		DX8Wrapper::Draw_Triangles(0, index_count / 3, 0, vertex_count);
		DX8Wrapper::Set_Transform(D3DTS_VIEW, view);
		DX8Wrapper::Set_Transform(D3DTS_PROJECTION, projection);
	}
	REF_PTR_RELEASE(surface);
	texture->Release_Ref();
	back_buffer->Release_Ref();
}

void UnknownDistortionClass::Get_Obj_Space_Bounding_Sphere(SphereClass & sphere) const
{
	sphere.Center.Set(0, 0, 0);
	sphere.Radius = Unknown90;
}

void UnknownDistortionClass::Get_Obj_Space_Bounding_Box(AABoxClass & box) const
{
	box.Center.Set(0, 0, 0);
	box.Extent.Set(Unknown90, Unknown90, Unknown90);
}


/*
** UnknownRippleClass
*/
UnknownRippleClass::UnknownRippleClass(void) :
	Unknown08(20.0f),
	Unknown0C(0.01f),
	Unknown10(0.01f)
{
}

void UnknownRippleClass::Unknown_Distort(Vector2 & uv, UnknownDistortionClass * owner)
{
	float dy = uv.Y - 0.5f;
	float dx = uv.X - 0.5f;
	float r2 = dx * dx + dy * dy;
	float angle = (float)WW3D::Get_Sync_Time() * Unknown0C + r2;
	// Both offsets are computed before either is added (Y's sine and exponential first), and the two
	// exponentials are not merged: math.h's inline expf in Y does both (0x997DE0)
	uv += Vector2((float)exp(-r2 * Unknown08) * WWMath::Sin(angle) * Unknown10,
		WWMath::Sin(angle) * Unknown10 * expf(-r2 * Unknown08));
}
