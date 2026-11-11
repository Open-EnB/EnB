// Earth & Beyond client (reconstruction): app\Client\dynamic_window_icon.cpp (__FILE__ 0xB76BA0). Notes at the end of the file.
#include "dynamic_window_icon.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\textureloader.h"
#include "..\..\ww3d2\dynamesh.h"
#include "..\..\ww3d2\shader.h"
#include "..\shared\g_assert.h"

















RefCountPtr<DynamicWindowIconClass> DynamicWindowIconClass::Create(const std::string &texture_name, const Vector2 &size, unsigned int flags)
{
	FAIL_IF(texture_name.empty()) return NULL;
	FAIL_IF(size.X <= 0 || size.Y <= 0) return NULL;

	RefCountPtr<RenderObjClass> render_object = Create_Render_Object(texture_name, size);
	FAIL_IF(!render_object) return NULL;

	return Create_NEW(new DynamicWindowIconClass(render_object, size, flags));
}

RefCountPtr<DynamicWindowIconClass> DynamicWindowIconClass::Create(const RefCountPtr<TextureClass> &texture, const Vector2 &size, unsigned int flags)
{
	FAIL_IF(!texture) return NULL;
	FAIL_IF(size.X <= 0 || size.Y <= 0) return NULL;

	RefCountPtr<RenderObjClass> render_object = Create_Render_Object(texture, size);
	FAIL_IF(!render_object) return NULL;

	return Create_NEW(new DynamicWindowIconClass(render_object, size, flags));
}

DynamicWindowIconClass::DynamicWindowIconClass(const RefCountPtr<RenderObjClass> &render_object, const Vector2 &size, unsigned int flags) :
	DynamicWindowRenderObjectClass(flags),
	IconSize(size)
{
	Set_Render_Object(render_object);
}

DynamicWindowIconClass::~DynamicWindowIconClass()
{
}

void DynamicWindowIconClass::Set_Texture(const std::string &texture_name)
{
	RefCountPtr<RenderObjClass> render_object = Create_Render_Object(texture_name, IconSize);
	Set_Render_Object(render_object);
}

void DynamicWindowIconClass::Set_Texture(const RefCountPtr<TextureClass> &texture)
{
	RefCountPtr<RenderObjClass> render_object = Create_Render_Object(texture, IconSize);
	Replace_Render_Object(render_object);
}

RefCountPtr<RenderObjClass> DynamicWindowIconClass::Create_Render_Object(const std::string &texture_name, const Vector2 &size)
{
	RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(texture_name.c_str(), MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (!texture) {
		return NULL;
	}
	return Create_Render_Object(texture, size);
}

RefCountPtr<RenderObjClass> DynamicWindowIconClass::Create_Render_Object(const RefCountPtr<TextureClass> &texture, const Vector2 &size)
{

	FAIL_IF(!texture) return NULL;

	if (!texture->Is_Initialized()) {
		TextureLoader::Request_Foreground_Loading(texture.Peek());
	}
	RefCountPtr<DynamicMeshClass> mesh = Create_NEW(new DynamicMeshClass(2, 4));
	mesh->Set_Texture(texture.Peek());
	mesh->Set_Shader(ShaderClass::_PresetAlpha2DShader);

	int width = size.X * 0.5f;
	int height = size.Y * 0.5f;
	mesh->Begin_Tri_Strip();
	mesh->Vertex(-width, -height, 0, 0, 1);
	mesh->Vertex(width, -height, 0, 1, 1);
	mesh->Vertex(-width, height, 0, 0, 0);
	mesh->Vertex(width, height, 0, 1, 0);
	mesh->End_Tri_Strip();
	return mesh;
}

/*
**	Reconstruction notes (O1, 2026-11-10)
**
**	Client 0x519A20-0x51A7AF (__FILE__ 0xB76BA0; FAIL_IFs on the client's lines 28-83, D25): the two Create
**	0x519A20 (a texture name) / 0x519C40 (a texture), both with the constructor inlined; the constructor 0x519E60,
**	the deleting destructor 0x519EF0, the destructor 0x519F20, the two Set_Texture 0x519F40 (a texture name, through
**	the base's Set_Render_Object slot) / 0x519FE0 (a texture, through Replace_Render_Object), the two
**	Create_Render_Object 0x51A080 / 0x51A190 (as dynamic_window_toggle.cpp's: a two-triangle strip of the texture,
**	size wide and high, centred), then the header and template instances: MeshMatDescClass::Get_UV_Array_By_Index
**	0x51A4F0, UVBufferClass's destructors 0x51A5E0 / 0x51A610, DynamicMeshModel's Set_Single_Shader 0x51A650,
**	Create_NEW<DynamicWindowIconClass> 0x51A690, Create_Get<TextureClass> 0x51A6F0, Create_NEW<DynamicMeshClass>
**	0x51A720 and ShareBufferClass<Vector2>'s destructors to 0x51A7AF. The class name is the file's (D24; no RTTI).
*/
