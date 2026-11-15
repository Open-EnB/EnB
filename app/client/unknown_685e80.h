#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_685E80_H  // NAME 1: PROVISIONAL, the object's first address (no __FILE__ in it)
#define __UNKNOWN_685E80_H

class INIClass;
class TextureClass;

// An INI section's tooltip, user data and texture: the base of acticon.cpp's ActionClass (action.ini's
// 'Action%04d' sections, read by 0x6766E0). Table 0xAFC788 (one slot: the deleting destructor 0x685F40), 0x14 bytes.
class Unknown_685E80Class  // NAME 1: neutral (D10), the constructor's address
{
public:
	Unknown_685E80Class(int id, INIClass &ini, const char *section);	// 0x685E80  // NAME 1 [id, ini, section]: what 0x6766E0 passes
	virtual ~Unknown_685E80Class();						// 0x685F90 (deleting 0x685F40)

	TextureClass *Get_Texture();						// 0x685FD0 (no caller: Add_Ref'd)  // NAME 2: WW3D's Get_ / Peek_ convention for what it does
	TextureClass *Peek_Texture();						// 0x685FF0 (no caller)  // NAME 2: WW3D's Get_ / Peek_ convention for what it does

	char *ToolTip;										// +0x04 ('ToolTip', Get_Alloc_String; free'd)  // NAME 2: the INI entry
	int UserData;										// +0x08 ('UserData')  // NAME 2: the INI entry
	TextureClass *Texture;								// +0x0C ('Texture', the asset manager's, one mip level)  // NAME 2: the INI entry
	int Id;												// +0x10 (the constructor's first argument)  // NAME 1: 0x6766E0 passes the section's number
};

#endif
