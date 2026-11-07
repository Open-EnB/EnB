// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\inv_manu_render.h (name assumed from inv_manu_render.cpp, whose __FILE__ is
**	at 0xB900CC; the client holds no string of this header)
**
**	An inventory slot on the manufacturing screen: table 0xAFCAF0, 0x30 bytes (0x6A90A0 makes it for items of kinds 12 and 13).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef INV_MANU_RENDER_H
#define INV_MANU_RENDER_H

#include <string>
#include "slot_data_render.h"

class FontSys;
class InvDataClass;

class InvManuRenderClass : public SlotDataRenderClass	// NAME 3: class-from-file (D24) for app\Client\inv_manu_render.cpp
{
public:
	InvManuRenderClass();													// 0x68B370
	virtual ~InvManuRenderClass();											// +0x04 0x68B540 (deleting 0x68B510)
	virtual void *Unknown_1C() { return(this); }							// +0x1C 0x68B4F0
	virtual const char *Unknown_28(std::string *string);					// +0x28 0x68B980
	virtual void Unknown_2C(bool force);									// +0x2C 0x68B5E0
	virtual int Unknown_30(int level);										// +0x30 0x68BD60
	virtual TextureClass *Unknown_34();										// +0x34 0x68B860

	InvDataClass *Peek_Inv_Data();											// 0x68BCF0  // NAME 3: the asserts '!sd', 'id', as inv_render.cpp's
	void Unknown_68BDA0(int value);											// 0x68BDA0 (the level's text)  // NAME 1 [value]: neutral

	FontSys *LevelText;														// +0x2C ('INVENTORY_LEVEL')  // NAME 2: its string template
};

#endif
