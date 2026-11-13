// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\color_common.h (name assumed from color_common.cpp, whose __FILE__ is
**	0xBA93DC)
**
**	color_common.cpp's functions: the chat colours by type, the chat type by name (the 24 names of 0xBA928C) and the
**	decal index check against decal_textures.ini / decal_swatches.ini / secret_decal_textures.ini. None has a caller
**	in the retail client.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef COLOR_COMMON_H  // NAME 3 [color_common.h]: header name assumed from the client's color_common.cpp
#define COLOR_COMMON_H

class Vector3;

void Unknown_831A70(int type, Vector3 &color);		// 0x831A70: the colour of a chat type (0..15; G_ASSERT 'false' l.47, white)  // NAME 1 [type, color]: neutral (D10)
bool Unknown_831C80(const char *name, int &type);	// 0x831C80: the chat type of a name (0xBA928C, stricmp; G_ASSERT '0' l.92)  // NAME 1 [name, type]: neutral (D10)
int Unknown_831CF0(int index, bool special);		// 0x831CF0: the decal index, or 0x7FFFFFF when no ini has it  // NAME 1 [index, special]: neutral (D10): special also allows the swatches (< 0) and the secret textures (1000..)

#endif
