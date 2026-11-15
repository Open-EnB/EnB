// Earth & Beyond client (reconstruction): RObj2Texture, a render object drawn into a texture (notes in robj2texture.cpp)
#ifndef ROBJ2TEXTURE_H
#define ROBJ2TEXTURE_H

#include "vector2i.h"
#include "ww3dformat.h"

class CameraClass;
class LightClass;
class RenderObjClass;
class SphereClass;
class TextureClass;

class RObj2Texture
{
public:
	RObj2Texture(const Vector2i &size, const Vector2i &x_border, const Vector2i &y_border);	// 0x4973E0  // NAME 2 [x_border, y_border]: the left / right and top / bottom margins of the camera's viewport
	virtual ~RObj2Texture();																// 0x4976B0 (deleting 0x497630, the only slot of 0xAEA8A4)

	TextureClass *Render_TextureMap(RenderObjClass *robj, const SphereClass *sphere = NULL);	// 0x497700 (the client's profile name)

	bool Orthographic;			// +0x04: true from the constructor; the camera's projection  // NAME 2: what Render_TextureMap uses it for
	WW3DFormat Format;			// +0x08: WW3D_FORMAT_A8R8G8B8 from the constructor; the texture's format  // NAME 2: what Render_TextureMap uses it for
	Vector2i XBorder;			// +0x0C  // NAME 2: the viewport's left (X) and right (Y) margin in pixels
	Vector2i YBorder;			// +0x14  // NAME 2: the viewport's top (X) and bottom (Y) margin in pixels
	Vector2i Size;				// +0x1C: the texture's size in pixels  // NAME 2: what Render_TextureMap uses it for
	CameraClass *Camera;		// +0x24
	LightClass *Light;			// +0x28
};

// The projector render target in the first 8 or 16 bit format the device renders to (0x498120; proj_man.cpp's
// shadow textures)
TextureClass *Create_Projector_Render_Target(int width, int height);	// NAME 2: Renegade's pscene_projectors.cpp helper

#endif
