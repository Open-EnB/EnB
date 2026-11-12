// Earth & Beyond client (reconstruction): app\client\fonttexture.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "font32bit.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\surfaceclass.h"
#include "..\..\ww3d2\pot.h"
#include "..\..\wwlib\nstrdup.h"
#include <math.h>
#include <string.h>

static SurfaceClass *Unknown_BE67C8 = NULL;	// 0xBE67C8: the surface every text is printed to first (freed with the last holder; file static: its release codegen, 0x6D8C50 / 0x6D8DC0)  // NAME 1 [Unknown_BE67C8]: neutral (D10)
int Unknown_BE67D0 = 0;					// 0xBE67D0: counts the surface's re-creations  // NAME 1 [Unknown_BE67D0]: neutral (D10)
int Unknown_BE67D4 = 0;					// 0xBE67D4: counts the textures 0x6D8DC0 re-creates  // NAME 1 [Unknown_BE67D4]: neutral (D10)

int Unknown_6D8B90Class::Count = 0;			// 0xBE67CC
int Unknown_6D8B90Class::Unknown_BE67D8 = 0;	// 0xBE67D8


Unknown_6D8B90Class::Unknown_6D8B90Class() :
	Size(0x20),
	Text(NULL),
	Texture(NULL)
{
	Texture = new TextureClass(Size, Size, WW3D_FORMAT_A4R4G4B4, MIP_LEVELS_1, TextureClass::POOL_MANAGED, false);
	Count++;
	Unknown_BE67D8++;
}


Unknown_6D8B90Class::~Unknown_6D8B90Class()
{
	REF_PTR_RELEASE(Texture);
	if (--Count == 0) {
		REF_PTR_RELEASE(Unknown_BE67C8);
	}
	if (Text != NULL) {
		delete [] Text;
	}
}


bool Unknown_6D8B90Class::Is_Printed(RefCountPtr<Font32Class> *font, const char *str)
{
	if (Texture != NULL && Text != NULL && str != NULL) {
		if (strcmp(Text, str) == 0 && Font == *font) {
			return true;
		}
	}
	return false;
}






































bool Unknown_6D8B90Class::Unknown_6D8DC0(RefCountPtr<Font32Class> *font, const char *str, int unknown)
{
	FAIL_IF(!str) str = "";
	if (Is_Printed(font, str) && unknown == 0) {
		return false;
	}
	if (Text != NULL) {
		delete [] Text;
	}
	Font = *font;
	Text = nstrdup(str);
	int fw = (*font)->String_Pixel_Width(str);










	if (fw <= 0) {
		fw = (*font)->String_Pixel_Width(" ");
		FAIL_IF(fw <= 0) fw = 8; }
	int fh = (*font)->Get_Height();
	FAIL_IF(fh <= 0) fh = 8;
	if (Unknown_BE67C8 != NULL) {
		SurfaceClass::SurfaceDescription desc;
		Unknown_BE67C8->Get_Description(desc);
		if ((int)desc.Width < fw || (int)desc.Height < fh) {
			REF_PTR_RELEASE(Unknown_BE67C8);
			Unknown_BE67C8 = new SurfaceClass(fw, fh, WW3D_FORMAT_A4R4G4B4);
			Unknown_BE67D0++;
		}
	} else {
		Unknown_BE67C8 = new SurfaceClass(fw, fh, WW3D_FORMAT_A4R4G4B4);
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
		surface->Copy(0, i * fh, x, 0, (fw - x < Size) ? fw - x : Size, fh, Unknown_BE67C8);
	}
	REF_PTR_RELEASE(surface);
	return true;
}

/*
**	Reconstruction notes (fonttexture.cpp)
**
**	__FILE__ 'e:\build\beta\G\app\Client\fonttexture.cpp' (0xB92870): 0x6D8DC0's FAIL_IFs '!str' (line 92; the tree's
**	parameter name str, as the client's), 'fw <= 0' (114), 'fh <= 0' (116). The holder's statics 0xBE67C8-0xBE67D8 are
**	this file's. The object starts at the constructor 0x6D8B90 (fontsys.cpp's last function before it) and ends with
**	0x6D8DC0 (gauge3d.cpp follows at 0x6D9360). Font +0x08 is a RefCountPtr<Font32Class>: the destructor releases it
**	after the EH state's end (a member's destructor) and 0x6D8DC0's assignment is RefCountPtr's operator=. Is_Printed
**	0x6D8D30 (no callers left; inlined in 0x6D8DC0) is the same-text test. The shared surface 0xBE67C8 is a file
**	static: as a global its REF_PTR_RELEASE compiles to the plain dec / jne, as a static to the client's
**	mov ecx, eax; add eax, 4; dec; cmp; jne (the destructor and 0x6D8DC0 match only so). The strip width is a
**	conditional expression in the Copy call (a separate w swaps ecx / edx).
*/
