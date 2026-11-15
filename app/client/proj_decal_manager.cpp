// Earth & Beyond client (reconstruction): app\Client\proj_decal_manager.cpp (__FILE__ 0xB9B458). Notes at the end.
#include <stdio.h>
#include "proj_decal_manager.h"
#include "..\shared\g_assert.h"
#include "..\shared\enbassetmgr.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\decalsys.h"
#include "..\..\ww3d2\matpass.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\wwmath\sphere.h"
#include "..\..\wwmath\matrix3d.h"

struct ProjectedDecalClass { float F00; float F04; float F08; int ProjDecalAssetID; float F10; };

// @0x763050
void ProjDecalManagerClass::Load(const char *ini_filename)
{
	INIClass ini;
	ini.Load(ini_filename);
	ProjectedDecalAssetTable.clear();
	ProjectedDecalAssetTable.reserve(ini.Section_Count());
	int index = 0;
	while (true) {
		char section[256];
		sprintf(section, "Decal%03d", index);
		if (!ini.Find_Section(section))
			break;
		{ Unknown758F40Class asset;
		asset.BaseSize = ini.Get_Float(section, "BaseSize", 0.0f);
		char tex_name[256];
		ini.Get_String(section, "Texture", "", tex_name, 0xFF);
		asset.Texture = Create_Get(AssetManager->Get_Texture(tex_name));
		if (asset.Texture) {
			asset.Texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
			asset.Texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		}

		asset.Lighting = ini.Get_Bool(section, "Lighting", false);

		G_ASSERT(asset.BaseSize > 0.0f);
		G_ASSERT(asset.Texture);
		ProjectedDecalAssetTable.push_back(asset);
		} index++;
	}
}

// @0x7633E0
void ProjDecalManagerClass::Unknown_7633E0(DecalGeneratorClass *generator, const ProjectedDecalClass &proj_decal, RenderObjClass *model)
{



	FAIL_IF(proj_decal.ProjDecalAssetID >= (int)ProjectedDecalAssetTable.size()) return;

	FAIL_IF(!generator) return;
	Unknown758F40Class &asset = ProjectedDecalAssetTable[proj_decal.ProjDecalAssetID];
	const Vector3 &dir = *(const Vector3 *)&proj_decal.F00;
	Vector3 z = dir;
	Vector3 x(-z.Y, z.X, 0.0f);
	Vector3 y; Vector3::Cross_Product(z, x, &y);
	SphereClass sphere = model->Get_Bounding_Sphere();
	Vector3 pos = sphere.Center + (sphere.Radius * 2.0f) * dir;
	Matrix3D tm;
	tm.Set(x, y, z, pos);
	generator->Set_Transform(tm);
	float half_size = proj_decal.F10 * asset.BaseSize * 0.5f;
	generator->Set_Ortho_Projection(-half_size, half_size, -half_size, half_size, sphere.Radius * 0.5f, sphere.Radius * 2.0f);
	RefCountPtr<MaterialPassClass> pass = Create_Get(generator->Get_Material());
	pass->Set_Texture(asset.Texture.Peek(), 0);
	ShaderClass shader = ShaderClass::_PresetAlphaShader;
	if (asset.Lighting) {
		RefCountPtr<VertexMaterialClass> mat = Create_NEW(new VertexMaterialClass);
		mat->Set_Ambient(1.0f, 1.0f, 1.0f);
		mat->Set_Diffuse(1.0f, 1.0f, 1.0f);
		mat->Set_Specular(0.0f, 0.0f, 0.0f);
		mat->Set_Emissive(0.0f, 0.0f, 0.0f);
		pass->Set_Material(mat.Peek());
	} else {
		shader.Set_Primary_Gradient(ShaderClass::GRADIENT_DISABLE);
	}
	pass->Set_Shader(shader);
}

/*
** Reconstruction notes:
** - Object range in retail client.exe: 0x763050..0x763DFF (between unknown_761c80.cpp
**   and unknown_763e00.cpp). __FILE__ at 0xB9B458 is "e:\build\beta\G\app\Client\proj_decal_manager.cpp".
** - Lines 41, 42, 54, 56 match the retail G_ASSERT / FAIL_IF __LINE__ values.
** - ProjectedDecalClass is defined locally (rather than including aux_roots.h, which
**   pulls in <string> and emits a ctype<unsigned short>::id initializer absent here).
** - Load (0x763050), Unknown_7633E0 (0x7633E0), Create_Get<MaterialPassClass> (0x763850),
**   std::vector<Unknown758F40Class> helpers (0x763820..0x7639E0) and std::_Construct (0x763DC0)
**   all compile byte-exact.
*/
