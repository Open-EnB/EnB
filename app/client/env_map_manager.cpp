// Earth & Beyond client (reconstruction): app\Client\env_map_manager.cpp (__FILE__ 0xB9B1D4). Notes at the end.
#include <stdio.h>
#include "env_map_manager.h"
#include "..\shared\g_assert.h"

// 0x760360
void EnvironmentMapManagerClass::Load(const char *env_map_ini_filename)
{
	FAIL_IF(!env_map_ini_filename || !env_map_ini_filename[0]) return;

	if (INI.Is_Loaded()) {
		return;
	}
	INI.Load(env_map_ini_filename);

	char section[256];
	int index = 0;
	sprintf(section, "PROCTEX-%02d", index);
	while (INI.Find_Section(section)) {
		{
			BaseGridTextures.push_back(Unknown67D360Class());

			char name[256];
			INI.Get_String(section, "BaseTexture", "missing_texture.tga", name, 0xFF);
			BaseGridTextures.back().Name = name;

			RefCountPtr<TextureClass> texture = Create_NEW(new TextureClass(name, NULL, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
			if (!texture->Is_Initialized()) {
				TextureLoader::Request_Foreground_Loading(texture.Peek());
			}
			BaseGridTextures.back().Surface = Create_Get(texture->Get_Surface_Level(0));










			FAIL_IF(!BaseGridTextures.back().Surface) {
				BaseGridTextures.back().Surface = Create_NEW(new SurfaceClass(32, 32, Get_Valid_Texture_Format(WW3D_FORMAT_R8G8B8, true)));
			}
		}
		index++;
		sprintf(section, "PROCTEX-%02d", index);
	}
}
// NOT EXACT: relocation (explain.py: our DX8Wrapper::_Create_DX8_Texture takes MipCountType, the client's TextureClass::MipCountType), then frame layout (similarity 0.69, its calls one for one the client's): functional for POC (D64)
// 0x760770
void EnvironmentMapManagerClass::Unknown_760770(int base_asset, bool a)
{
	char section[256]; char name[256]; sprintf(section, "BACK-%i", base_asset); INI.Get_String(section, "Texture", "", name, 0xFE);
	if (!name[0]) INI.Get_String("BACK-DEFAULT", "Texture", "", name, 0xFE);
	FAIL_IF(!name[0]) return;

	char axis[2];
	INI.Get_String(section, "Axis", "Z", axis, 2);
	switch (axis[0]) {
		case 'X':
		case 'x':
			Unknown10 = 0;
			break;
		case 'Y':
		case 'y':
			Unknown10 = 1;
			break;
		default:
			Unknown10 = 2;
			break;
	}

	RefCountPtr<TextureClass> reflection_texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (!reflection_texture->Is_Initialized()) {
		TextureLoader::Request_Foreground_Loading(reflection_texture.Peek());
	}
	RefCountPtr<SurfaceClass> reflection_surface = Create_Get(reflection_texture->Get_Surface_Level(0));













	FAIL_IF(!reflection_surface) return;

	SurfaceClass::SurfaceDescription reflection_desc;
	reflection_surface->Get_Description(reflection_desc);
	RefCountPtr<SurfaceClass> reflection_copy = Create_NEW(new SurfaceClass(reflection_desc.Width, reflection_desc.Height, WW3D_FORMAT_R8G8B8));
	reflection_copy->Copy(0, 0, 0, 0, reflection_desc.Width, reflection_desc.Height, reflection_surface.Peek());
	RefCountPtr<SurfaceClass> reflection_half = Create_NEW(new SurfaceClass(reflection_desc.Width / 2, reflection_desc.Height / 2, WW3D_FORMAT_R8G8B8));
	reflection_half->Stretch_Copy(0, 0, reflection_desc.Width / 2, reflection_desc.Height / 2, 0, 0, reflection_desc.Width, reflection_desc.Height, reflection_surface.Peek());

	int width;
	int height;
	int size;
	unsigned char *reflection = reflection_copy->CreateCopy(&width, &height, &size);



	FAIL_IF(width != (int)reflection_desc.Width || height != (int)reflection_desc.Height || size != 3) { delete [] reflection; return; }

	unsigned char *reflection_small = reflection_half->CreateCopy(&width, &height, &size);
	FAIL_IF(width != (int)reflection_desc.Width / 2 || height != (int)reflection_desc.Height / 2 || size != 3) {
		delete [] reflection;
		delete [] reflection_small;
		return;
	}

	for (std::vector<Unknown67D360Class>::iterator it = BaseGridTextures.begin(); it != BaseGridTextures.end(); ++it) {
		RefCountPtr<TextureClass> grid_texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(it->Name.c_str(), MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, false));




		FAIL_IF(!grid_texture) continue;

		SurfaceClass::SurfaceDescription base_desc;
		it->Surface->Get_Description(base_desc);


		FAIL_IF(reflection_desc.Width * 2 != base_desc.Width || reflection_desc.Height * 2 != base_desc.Height) continue;

		RefCountPtr<SurfaceClass> new_surface = Create_NEW(new SurfaceClass(base_desc.Width, base_desc.Height, WW3D_FORMAT_R8G8B8));
		FAIL_IF(!new_surface) continue;

		new_surface->Copy(0, 0, 0, 0, base_desc.Width, base_desc.Height, it->Surface.Peek());
		unsigned char *pixels = new_surface->CreateCopy(&width, &height, &size);

		FAIL_IF(width != (int)base_desc.Width || height != (int)base_desc.Height || size != 3) {
			delete [] pixels;
			continue;
		}

		// the reflection added over the bottom right quarter, its half size copy over each other cell of a 4 x 4 grid
		int scale = a + 1;
		int pitch = base_desc.Width * 3;
		unsigned int half_height = base_desc.Height >> 1;
		int half_pitch = (base_desc.Width >> 1) * 3;
		unsigned char *src_row = reflection;
		unsigned char *dst_row = pixels + pitch * half_height + half_pitch;
		for (unsigned int y = half_height; y != 0; y--) {
			unsigned char *src = src_row;
			unsigned char *dst = dst_row;
			for (int x = half_pitch; x > 0; x--) {
				unsigned int value = *src * scale + *dst;
				if (value > 0xFF) {
					value = 0xFF;
				}
				*dst = (unsigned char)value;
				dst++;
				src++;
			}
			dst_row += pitch;
			src_row += half_pitch;
		}

		unsigned int quarter_height = base_desc.Height >> 2;
		int quarter_pitch = (base_desc.Width >> 2) * 3;
		for (int row = 0; row < (int)base_desc.Height; row += quarter_height) {
			for (int col = 0; col < pitch; col += quarter_pitch) {
				if ((col < half_pitch || row < (int)half_height) && quarter_height != 0) {
					unsigned char *cell_src = reflection_small;
					unsigned char *cell_dst = pixels + row * pitch + col;
					for (unsigned int y = quarter_height; y != 0; y--) {
						unsigned char *src = cell_src;
						unsigned char *dst = cell_dst;
						for (int x = quarter_pitch; x > 0; x--) {
							unsigned int value = *src * scale + *dst;
							if (value > 0xFF) {
								value = 0xFF;
							}
							*dst = (unsigned char)value;
							dst++;
							src++;
						}
						cell_dst += pitch;
						cell_src += quarter_pitch;
					}
				}
			}
		}

		new_surface->Copy(pixels);
		IDirect3DTexture8 *d3d_texture = DX8Wrapper::_Create_DX8_Texture(new_surface->Peek_D3D_Surface(), MIP_LEVELS_ALL);
		grid_texture->Apply_New_Surface(d3d_texture, true, true);
		((bool *)grid_texture.Peek())[0x0F] = true;	// TextureBaseClass +0x0F: IsProcedural (protected in texture.h)
		d3d_texture->Release();
		delete [] pixels;
	}

	delete [] reflection;
	delete [] reflection_small;
}

/*
**	Reconstruction notes
**
**	client.exe: env_map_manager.cpp 0x7602C0-0x7617C0 (asserts on lines 9, 42, 56-137).
**	0x760360 Load: envmaps.ini's PROCTEX-00, PROCTEX-01, ... sections, each one's BaseTexture loaded and its
**	level 0 surface kept (a 32 x 32 surface when it has none). 0x761540 is the vector's insert, 0x51A6F0 /
**	0x761510 / 0x6BCF40 the RefCountPtr Create_NEW / Create_Get friends, 0x6BCF70 its destructor.
**	0x760770: the BACK-<base asset> section's Texture (else BACK-DEFAULT's) and Axis (X 0, Y 1, else 2 into
**	Unknown10); the reflection and a half size copy blended (times 1, or 2 when a) into each base grid texture's
**	level 0 surface: the full one over the bottom right quarter, the half one over the other cells of a 4 x 4 grid;
**	the result applied to the asset manager's texture of that name as a new D3D texture (IsProcedural set).
**	The asset manager through WW3DAssetManager::Get_Instance (the client reads 0xBFAEE0, the other app files'
**	Unknown_BFAEE0, which nothing defines yet). Functional NOT EXACT (D64).
*/
