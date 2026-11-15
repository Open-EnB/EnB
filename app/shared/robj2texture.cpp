// Earth & Beyond client (reconstruction): RObj2Texture, a render object drawn into a texture (notes at the end)
#include "robj2texture.h"
#include "ro_proxy.h"
#include "camera.h"
#include "light.h"
#include "scene.h"
#include "rendobj.h"
#include "texture.h"
#include "surfaceclass.h"
#include "dx8wrapper.h"
#include "dx8caps.h"
#include "ww3d.h"
#include "matrix3d.h"
#include "sphere.h"
#include "wwprofile.h"
#include "wwmath.h"
#include <math.h>

// @0x4973E0
RObj2Texture::RObj2Texture(const Vector2i &size, const Vector2i &x_border, const Vector2i &y_border) :
	Orthographic(true),
	Format(WW3D_FORMAT_A8R8G8B8),
	XBorder(x_border),
	YBorder(y_border),
	Size(size),
	Camera(NULL),
	Light(NULL)
{
	Camera = new CameraClass;
	Camera->Set_Clip_Planes(0.2f, 500.0f);
	Camera->Set_View_Plane(DEG_TO_RADF(90.0f), DEG_TO_RADF(90.0f));
	Camera->Set_Viewport(
		Vector2((float)XBorder.I / (float)Size.I, (float)YBorder.I / (float)Size.J),
		Vector2(1.0f - (float)XBorder.J / (float)Size.I, 1.0f - (float)YBorder.J / (float)Size.J));

	Light = new LightClass(LightClass::POINT);
	Light->Set_Intensity(1.0f);
	Light->Set_Force_Visible(true);
	Light->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Light->Set_Ambient(Vector3(0.3f, 0.3f, 0.3f));
	Light->Set_Specular(Vector3(1.0f, 1.0f, 1.0f));
	Light->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	Light->Set_Transform(Matrix3D::Identity);
}

// @0x4976B0
RObj2Texture::~RObj2Texture()
{
	REF_PTR_RELEASE(Light);
	REF_PTR_RELEASE(Camera);
}

// @0x497700
// NOT EXACT (0.63, attempts.py 0x497700): frame 0xC4 vs the client's 0xA4; the client keeps a 0.0f in memory for the camera translation's X and Y adds, ours folds them
TextureClass *RObj2Texture::Render_TextureMap(RenderObjClass *robj, const SphereClass *sphere)
{
	WWPROFILE("RObj2Texture::Render_TextureMap");

	int width = Size.I;
	int height = Size.J;

	int lod = robj->Get_LOD_Level();
	robj->Set_LOD_Level(robj->Get_LOD_Count() - 1);
	int hidden = robj->Is_Hidden();
	robj->Set_Hidden(false);

	if (sphere == NULL) {
		sphere = &robj->Get_Bounding_Sphere();
	}
	SphereClass bsphere = *sphere;

	if (Orthographic) {
		Camera->Set_Projection_Type(CameraClass::ORTHO);
		Camera->Set_View_Plane(Vector2(-bsphere.Radius, -bsphere.Radius), Vector2(bsphere.Radius, bsphere.Radius));
	} else {
		Camera->Set_Projection_Type(CameraClass::PERSPECTIVE);
		Camera->Set_View_Plane(DEG_TO_RADF(90.0f), DEG_TO_RADF(90.0f));
	}

	Matrix3D camera_tm(bsphere.Center);
	camera_tm.Adjust_Translation(Vector3(0.0f, 0.0f, bsphere.Radius));
	Camera->Set_Transform(camera_tm);

	Matrix3D light_tm = camera_tm;
	light_tm.Translate_X(bsphere.Radius * 0.5f);
	light_tm.Translate_Y(bsphere.Radius * 0.5f);
	Light->Set_Transform(light_tm);

	float zfar = bsphere.Radius * 10.0f;
	if (fabs(Camera->Get_Depth() - zfar) > 0.0001f) {
		Camera->Set_Clip_Planes(bsphere.Radius * 0.01f, zfar);
	}

	SimpleSceneClass *scene = new SimpleSceneClass;
	ProxyRObj *proxy = new ProxyRObj(false);
	proxy->Set_Transform(robj->Get_Transform());
	proxy->Unknown_47FF70(robj);
	scene->Add_Render_Object(proxy);
	scene->Add_Render_Object(Light);
	scene->Set_Ambient_Light(Vector3(0.2f, 0.2f, 0.2f));

	float fx, fy;
	const ViewportClass &vp = Camera->Get_Viewport();
	WW3D::Normalize_Coordinates(width, height, fx, fy);
	Vector2 vmin = vp.Min;
	vmin.Scale(fx, fy);
	Vector2 vmax = vp.Max;
	vmax.Scale(fx, fy);
	Camera->Set_Viewport(vmin, vmax);
	WW3D::Begin_Render(true, true, Vector3(0, 0, 0));
	WW3D::Render(scene, Camera, false, false, Vector3(0, 0, 0));
	WW3D::End_Render(false);
	Camera->Set_Viewport(vp.Min, vp.Max);

	REF_PTR_RELEASE(scene);
	REF_PTR_RELEASE(proxy);

	robj->Set_LOD_Level(lod);
	robj->Set_Hidden(hidden);

	SurfaceClass *back = DX8Wrapper::_Get_DX8_Back_Buffer(0);
	SurfaceClass::SurfaceDescription desc;
	back->Get_Description(desc);
	SurfaceClass *copy = new SurfaceClass(width, height, desc.Format);
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	DX8Wrapper::_Copy_DX8_Rects(back->Peek_D3D_Surface(), &rect, 1, copy->Peek_D3D_Surface(), NULL);

	SurfaceClass *surface = new SurfaceClass(width, height, WW3D_FORMAT_A8R8G8B8);
	surface->Copy(0, 0, 0, 0, width, height, copy);
	int pitch;
	unsigned char *bits = (unsigned char *)surface->Lock(&pitch);
	for (int y = 0; y < height; y++) {
		unsigned char *pixel = bits;
		for (int x = 0; x < width; x++) {
			if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
				pixel[3] = 0;
			} else {
				pixel[3] = 0xFF;
			}
			pixel += 4;
		}
		bits += pitch;
	}
	surface->Unlock();

	TextureClass *texture = new TextureClass(surface);
	REF_PTR_RELEASE(back);
	REF_PTR_RELEASE(copy);
	REF_PTR_RELEASE(surface);

	if (texture != NULL) {
		if (Format != WW3D_FORMAT_A8R8G8B8) {
			SurfaceClass *level = texture->Get_Surface_Level(0);
			texture->Release_Ref();
			WW3DFormat format = Get_Valid_Texture_Format(Format, false);
			SurfaceClass *converted = new SurfaceClass(width, height, format);
			converted->Copy(0, 0, 0, 0, width, height, level);
			texture = new TextureClass(converted);
			REF_PTR_RELEASE(level);
			REF_PTR_RELEASE(converted);
		}
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_Mip_Mapping(TextureClass::FILTER_TYPE_NONE);
	}
	return texture;
}

// @0x498120
TextureClass *Create_Projector_Render_Target(int width, int height)
{
	WW3DFormat format;
	if (DX8Wrapper::Get_Current_Caps()->Support_Render_To_Texture_Format(WW3D_FORMAT_R3G3B2)) format = WW3D_FORMAT_R3G3B2;
	else if (DX8Wrapper::Get_Current_Caps()->Support_Render_To_Texture_Format(WW3D_FORMAT_R5G6B5)) format = WW3D_FORMAT_R5G6B5;
	else if (DX8Wrapper::Get_Current_Caps()->Support_Render_To_Texture_Format(WW3D_FORMAT_A4R4G4B4)) format = WW3D_FORMAT_A4R4G4B4;
	else if (DX8Wrapper::Get_Current_Caps()->Support_Render_To_Texture_Format(WW3D_FORMAT_X1R5G5B5)) format = WW3D_FORMAT_X1R5G5B5;
	else format = WW3D_FORMAT_UNKNOWN;
	return DX8Wrapper::Create_Render_Target(width, height, format);
}

/*
** Reconstruction notes (E&B's own file; no __FILE__ in the client: no asserts).
** - The name: RObj2Texture is the client's own, the profile string "RObj2Texture::Render_TextureMap" (0xB72594) that
**   0x497700 pushes; the file is named after the class (an interim placement, D137): its object sits between
**   ro_funcs.cpp's (0x495F60-0x497330 and its COMDATs, IntersectionClass's destructors 0x497390 / 0x4973B0 among them:
**   0xAA50F0, ro_funcs' unwind, destroys one) and segline_create.cpp's (0x4981E0), and robj2texture sorts between
**   them.
** - The object: 0x4973E0-0x497F06 and 0x498120-0x4981AF; its vtable 0xAEA8A4 has one slot (the deleting destructor
**   0x497630) and the 10.0f after it (0xAEA8A8) is the file's own literal.
** - The class: the constructor sets the camera (0.2 to 500, 90 degrees both ways) with its viewport inset by the
**   two border pairs, and a point light (as avatar_pip.cpp's, ambient 0.3). Render_TextureMap renders the object
**   (lowest LOD, unhidden) through a ProxyRObj in a SimpleSceneClass, with the camera at the bounding sphere's
**   top (orthographic over the radius when Orthographic), copies the back buffer, makes black pixels transparent
**   and returns a clamped texture without mipmaps (converted when Format is not A8R8G8B8). The viewport it
**   "restores" is read through the reference it scaled, so the restore writes the scaled one back (the client's
**   code: 0x497BB5 copies from the camera's own viewport).
** - Create_Projector_Render_Target (0x498120): Renegade's wwphys/pscene_projectors.cpp helper without its
**   last-resort retry; proj_man.cpp's two callers (0x493620, 0x493D80).
** - NOT EXACT: Render_TextureMap (the frame is 0x20 bytes larger than the client's; the camera transform's
**   Adjust_Translation: the client adds a 0.0f kept in memory to the X and Y translation, VC6 folds ours).
*/
