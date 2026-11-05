// Earth & Beyond client (reconstruction): app\client\decal.h, the decal library of decal.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\decal.cpp'; this header's name is provisional, NAME 1). sclient.cpp, scriptedmovie.cpp
// and ui_shipcust.cpp still declare the class locally (the same layout: their vectors are std::vector<int>).
#ifndef DECAL_H  // NAME 1 [decal.h]: provisional header name for decal.cpp
#define DECAL_H

#include <vector>
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\shader.h"

class TextureClass;
class VertexMaterialClass;
class INIClass;
class Vector4;

// One decal's material (0x44 bytes, constructor 0x619280, destructor 0x618C10, assignment 0x75DA20): the passes of
// a mesh, two texture stages, a shader and a vertex material each (0x80EAF0 puts them on a 'decalNN' mesh)
class Unknown619280Class		// NAME 1: neutral (D10): 0x44 bytes, constructor 0x619280
{
public:
	Unknown619280Class();								// 0x619280
	~Unknown619280Class();								// 0x618C10

	int Unknown00;										// +0x00 the pass count
	RefCountPtr<TextureClass> Unknown04[4][2];			// +0x04 [pass][stage]
	ShaderClass Unknown24[4];							// +0x24 [pass]
	RefCountPtr<VertexMaterialClass> Unknown34[4];		// +0x34 [pass]
};

// The decal library (0xB8 bytes): the insignia textures (decal_textures.ini), the secret ones (secret_decal_textures.ini,
// indices from 1000) and the swatch meshes (decal_swatches.ini, negative indices)
class SClientUnknown1334Class	// NAME 1: neutral (D10): 0xB8 bytes, destructor 0x618A50
{
public:
	~SClientUnknown1334Class();							// 0x618A50
	void Load(const char *textures, const char *swatches, const char *secret_textures);	// 0x75BE00  // NAME 2: what it does  // NAME 3 [textures, swatches, secret_textures]: the INI names
	RefCountPtr<TextureClass> Unknown_61DF40(int index);	// 0x61DF40 (the insignia's texture)  // NAME 1 [index]: neutral
	void Unknown_75CB10(int index, const Vector4 &color, Unknown619280Class *mat_desc);	// 0x75CB10 (decal index's material, tinted)  // NAME 3 [mat_desc]: the FAIL_IF's '!mat_desc'; NAME 1 [index, color]: neutral

	std::vector<RefCountPtr<TextureClass> > Unknown00;	// +0x00 the FAIL_IF's 'TextureArray'
	std::vector<RefCountPtr<TextureClass> > Unknown10;	// +0x10 the FAIL_IF's 'SecretTextureArray'
	std::vector<Unknown619280Class> Unknown20;			// +0x20 the FAIL_IF's 'SwatchArray'
	Unknown619280Class Unknown30;						// +0x30 the base of an insignia's material (alpha, fogged, lit)
	Unknown619280Class Unknown74;						// +0x74 an empty slot's material (opaque, no colour / depth write, untextured)
};

TextureClass *Unknown_82EF70(INIClass &ini, const char *section);	// 0x82EF70 (cdecl: 'File', 'TextureWrap')  // NAME 1 [all]: neutral

#endif
