// Earth & Beyond client (reconstruction): an unnamed INI-section object at 0x685E80 (PROVISIONAL). Notes at the end of the file.
#include <stdlib.h>
#include "unknown_685e80.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\texture.h"

// @0x685E80
Unknown_685E80Class::Unknown_685E80Class(int id, INIClass &ini, const char *section)
{
	char texture_name[256];

	Id = id;
	ToolTip = ini.Get_Alloc_String(section, "ToolTip", "");
	UserData = ini.Get_Int(section, "UserData", 0);
	ini.Get_String(section, "Texture", "", texture_name, 255);
	Texture = WW3DAssetManager::Get_Instance()->Get_Texture(texture_name, MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
}

// @0x685F90
Unknown_685E80Class::~Unknown_685E80Class()
{
	if (ToolTip) {
		free(ToolTip);
	}
	Texture->Release_Ref();
}

// @0x685FD0
TextureClass *Unknown_685E80Class::Get_Texture()
{
	Texture->Add_Ref();
	return Texture;
}

// @0x685FF0
TextureClass *Unknown_685E80Class::Peek_Texture()
{
	return Texture;
}

/*
Reconstruction notes:
The object 0x685E80-0x685FF4 (after fontsys_cursor.cpp, whose __FILE__ 0xB8FD0C its 'UserData' / 'ToolTip' follow, before
the unrelated constructor 0x686010) names no file (no assert): the class and file names are provisional. The table
0xAFC788 has one slot, the deleting destructor 0x685F40. The destructor releases the texture without a null test
(Release_Ref, not REF_PTR_RELEASE). Get_Texture 0x685FD0 and Peek_Texture 0x685FF0 have no caller in the client.
*/
