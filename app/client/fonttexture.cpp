// Earth & Beyond client (reconstruction): app\client\fonttexture.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "font32bit.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\surfaceclass.h"
#include "..\..\ww3d2\pot.h"
#include "..\..\wwlib\nstrdup.h"
#include <math.h>
#include <string.h>

SurfaceClass *Unknown_BE67C8 = NULL;	// 0xBE67C8: the surface every text is printed to first (freed with the last holder)  // NAME 1 [Unknown_BE67C8]: neutral (D10)
int Unknown_BE67D0 = 0;					// 0xBE67D0: counts the surface's re-creations  // NAME 1 [Unknown_BE67D0]: neutral (D10)
int Unknown_BE67D4 = 0;					// 0xBE67D4: counts the textures 0x6D8DC0 re-creates  // NAME 1 [Unknown_BE67D4]: neutral (D10)

int Unknown_6D8B90Class::Count = 0;			// 0xBE67CC
int Unknown_6D8B90Class::Unknown_BE67D8 = 0;	// 0xBE67D8

// NOT EXACT: written from the client's code (0x6D8B90) for the POC, not yet matched
Unknown_6D8B90Class::Unknown_6D8B90Class() :
	Size(0x20),
	Unknown04(NULL),
	Unknown08(NULL),
	Texture(NULL)
{
	Texture = new TextureClass(Size, Size, WW3D_FORMAT_A4R4G4B4, MIP_LEVELS_1, TextureClass::POOL_MANAGED, false);
	Count++;
	Unknown_BE67D8++;
}

// NOT EXACT: written from the client's code (0x6D8C50) for the POC, not yet matched
Unknown_6D8B90Class::~Unknown_6D8B90Class()
{
	REF_PTR_RELEASE(Texture);
	Count--;
	if (Count == 0) {
		REF_PTR_RELEASE(Unknown_BE67C8);
	}
	if (Unknown04 != NULL) {
		delete [] Unknown04;		// +0x04: the text (nstrdup)
	}
	if (Unknown08 != NULL) {
		Unknown08->Release_Ref();	// +0x08: the font it was printed with
		Unknown08 = NULL;
	}
}






















































// 0x6D8DC0: prints text with *font into the shared surface, then copies it into Texture in strips of Size pixels,
// one font height apart (Update_Text draws one quad per strip). Nothing to do (false) when the text, the font and
// unknown are the same as last time. With unknown set the texture is POT(height * unknown) square, unknown strips.
// NOT EXACT: written from the client's code (0x6D8DC0) for the POC, not yet matched
bool Unknown_6D8B90Class::Unknown_6D8DC0(Font32Class **font, const char *str, int unknown)
{
	FAIL_IF(!str) str = "";
	if (Texture != NULL && Unknown04 != NULL && str != NULL) {
		if (strcmp(Unknown04, str) == 0 && Unknown08 == *font && unknown == 0) {
			return false;
		}
	}
	if (Unknown04 != NULL) {
		delete [] Unknown04;
	}
	if (*font != NULL) {
		(*font)->Add_Ref();
	}
	if (Unknown08 != NULL) {
		Unknown08->Release_Ref();
	}
	Unknown08 = *font;
	Unknown04 = nstrdup(str);
	int fw = (*font)->String_Pixel_Width(str);


	if (fw <= 0) {
		fw = (*font)->String_Pixel_Width(" ");
		FAIL_IF(fw <= 0) fw = 8; }
	int fh = (*font)->Get_Height();
	FAIL_IF(fh <= 0) fh = 8;
	if (Unknown_BE67C8 == NULL) {
		Unknown_BE67C8 = new SurfaceClass(fw, fh, WW3D_FORMAT_A4R4G4B4);
	} else {
		SurfaceClass::SurfaceDescription desc;
		Unknown_BE67C8->Get_Description(desc);
		if ((int)desc.Width < fw || (int)desc.Height < fh) {
			REF_PTR_RELEASE(Unknown_BE67C8);
			Unknown_BE67C8 = new SurfaceClass(fw, fh, WW3D_FORMAT_A4R4G4B4);
			Unknown_BE67D0++;
		}
	}
	Unknown_BE67C8->Clear();
	(*font)->Print_To_Surface(str, Unknown_BE67C8);
	SurfaceClass *surface = Texture->Get_Surface_Level(0);
	SurfaceClass::SurfaceDescription tdesc;
	surface->Get_Description(tdesc);
	int rows;
	if (unknown != 0) {
		Size = Find_POT(fh * unknown);
		if ((int)tdesc.Width != Size) {
			REF_PTR_RELEASE(surface);
			REF_PTR_RELEASE(Texture);
			Texture = new TextureClass(Size, Size, WW3D_FORMAT_A4R4G4B4, MIP_LEVELS_1, TextureClass::POOL_MANAGED, false);
			Unknown_BE67D4++;
			surface = Texture->Get_Surface_Level(0);
		}
		rows = unknown;
	} else {
		Size = Find_POT((int)ceil(sqrt((double)(fh * fw))));
		if ((Size / fh) * Size < fw) {
			Size *= 2;
		}
		while (Size < fh) {
			Size *= 2;
		}
		if ((int)tdesc.Width < Size) {
			REF_PTR_RELEASE(surface);
			REF_PTR_RELEASE(Texture);
			Texture = new TextureClass(Size, Size, WW3D_FORMAT_A4R4G4B4, MIP_LEVELS_1, TextureClass::POOL_MANAGED, false);
			Unknown_BE67D4++;
			surface = Texture->Get_Surface_Level(0);
		} else {
			Size = tdesc.Width;
		}
		rows = (fw & (Size - 1)) != 0 ? fw / Size + 1 : fw / Size;
	}
	for (int i = 0; i < rows; i++) {
		int x = Size * i;
		if (fw <= x) {
			break;
		}
		int w = fw - x;
		if (w >= Size) {
			w = Size;
		}
		surface->Copy(0, i * fh, x, 0, w, fh, Unknown_BE67C8);
	}
	REF_PTR_RELEASE(surface);
	return true;
}

/*
**	Reconstruction notes (fonttexture.cpp)
**
**	__FILE__ 'e:\build\beta\G\app\Client\fonttexture.cpp' (0xB92870): 0x6D8DC0's FAIL_IFs '!str' (line 92; the tree's
**	parameter name str, as the client's), 'fw <= 0' (114), 'fh <= 0' (116). The holder's statics 0xBE67C8-0xBE67D8 are this file's; the
**	constructor 0x6D8B90 and the statics Count 0xBE67CC / 0xBE67D8 are still in fontsys.cpp (interim, D137): this file
**	starts at 0x6D8B90 or before (not settled). Functional (D64) for the POC client (TXT1).
*/
