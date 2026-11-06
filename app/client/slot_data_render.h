// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\slot_data_render.h (name assumed from slot_data_render.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\slot_data_render.cpp' at 0xB91684; the client holds no string of this header)
**
**	The base of what draws a slot's item: table 0xAFD810, constructor 0x6A7B60, destructor 0x6A80D0, 0x2C bytes (the
**	factory 0x6A90A0 makes its derived classes), with the one-line virtuals its table shares with the other derived classes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SLOT_DATA_RENDER_H
#define SLOT_DATA_RENDER_H

#include <string>
#include "always.h"
#include "refcount.h"
#include "..\..\wwlib\ref_ptr.h"

class RenderObjClass;
class SceneClass;
class TextureClass;
class Vector3;
class SlotDataClass;
class SkillRenderClass;

class SlotDataRenderClass : public RefCountClass	// NAME 3: class-from-file (D24) for app\Client\slot_data_render.cpp
{
public:
	SlotDataRenderClass();													// 0x6A7B60
	virtual ~SlotDataRenderClass();											// +0x04 0x6A80D0 (deleting 0x6A7BD0)
	virtual bool Unknown_08(SlotDataClass *sd, RenderObjClass *robj);		// +0x08 0x6A7C00  // NAME 3 [sd]: 0x6A7310's FAIL_IF; NAME 1 [robj]: neutral
	virtual void Unknown_0C(std::string &string);							// +0x0C 0x6A82F0 (appends its state)  // NAME 1 [string]: neutral
	virtual const char *Unknown_10(const char *state);						// +0x10 0x6A8430 (reads it back)  // NAME 3 [state]: its FAIL_IF
	virtual SlotDataRenderClass *Unknown_14() { return(this); }				// +0x14 0x68B410
	virtual void *Unknown_18() { return(0); }								// +0x18 0x68B430
	virtual void *Unknown_1C() { return(0); }								// +0x1C 0x68C0F0
	virtual SkillRenderClass *Unknown_20() { return(0); }					// +0x20 0x68B450 (SkillRenderClass: itself)
	virtual void *Unknown_24() { return(0); }								// +0x24 0x68B470
	virtual const char *Unknown_28(std::string *string);					// +0x28 0x6A8610  // NAME 1 [string]: neutral
	virtual void Unknown_2C(bool force);									// +0x2C 0x6A87A0  // NAME 1 [force]: neutral
	virtual int Unknown_30(int level);										// +0x30 0x6A89A0 (the sort level, one below)  // NAME 1 [level]: neutral
	virtual TextureClass *Unknown_34() { return(0); }						// +0x34 0x6A7BB0 (the slot's texture)
	virtual bool Unknown_38(Vector3 &color) { return(false); }				// +0x38 0x68B490  // NAME 1 [color]: neutral
	virtual void Unknown_3C() {}											// +0x3C 0x68B4B0
	virtual void Unknown_40(int slot);										// +0x40 0x6A8730  // NAME 1 [slot]: neutral
	virtual void Unknown_44(SlotDataRenderClass *render);					// +0x44 0x6A8760  // NAME 3 [render]: its FAIL_IF
	virtual const char *Unknown_48() { return("InvItem"); }					// +0x48 0x68B4D0 (the model's name)
	virtual const char *Unknown_4C();										// +0x4C 0x6A8980 (the empty slot's texture)

	int Get_Slot() { return(Unknown20); }									// (0x6A6EA0's register)  // NAME 2: the accessor of Unknown20, the slot
	void Unknown_6A89D0(TextureClass *texture);								// 0x6A89D0  // NAME 1 [texture]: neutral
	void Unknown_6A8A40(TextureClass *texture);								// 0x6A8A40 (Unknown10's)  // NAME 1 [texture]: neutral
	void Unknown_6A8AB0(bool on);											// 0x6A8AB0 (Mask on or off the container; inv_render.cpp's)  // NAME 1 [on]: neutral
	int Unknown_6A8B30();													// 0x6A8B30 (the sort level, or 8)
	void Unknown_6A8B60(SceneClass *scene);									// 0x6A8B60 (AddedToScene)  // NAME 3 [scene]: its FAIL_IF
	void Unknown_6A8C40();													// 0x6A8C40 (off the scene)
	void Unknown_6A8CC0(const Vector3 &position);							// 0x6A8CC0 (the container's position)  // NAME 2 [position]: RenderObjClass::Set_Position
	void Unknown_6A8D00(SlotDataRenderClass *icon);							// 0x6A8D00 (IsSubIcon on)  // NAME 3 [icon]: its FAIL_IF
	void Unknown_6A8DF0(SlotDataRenderClass *icon);							// 0x6A8DF0 (IsSubIcon off)  // NAME 3 [icon]: its FAIL_IF
	void Unknown_6A8EC0();													// 0x6A8EC0 (Unknown10 off the scene)
	void Unknown_6A8F00(RenderObjClass *robj);								// 0x6A8F00 (onto the container)  // NAME 3 [robj]: its FAIL_IF
	void Unknown_6A8FE0(RenderObjClass *robj);								// 0x6A8FE0 (off it)  // NAME 3 [robj]: its FAIL_IF

	SlotDataClass *SlotData;												// +0x08  // NAME 3: 0x6A7C00's FAIL_IF
	RenderObjClass *Mesh;													// +0x0C  // NAME 3: 0x6A7C00's FAIL_IF
	RefCountPtr<RenderObjClass> Unknown10;									// +0x10 ('inv_blank01')
	RefCountPtr<RenderObjClass> Mask;										// +0x14 ('inv_blank01', 'icon_mask.tga')  // NAME 3: 0x6A8AB0's FAIL_IF
	RenderObjClass *Container;												// +0x18  // NAME 3: 0x6A7C00's assert
	int Unknown1C;															// +0x1C (the bone)
	int Unknown20;															// +0x20 (the slot; -1)
	TextureClass *Unknown24;												// +0x24 (the texture)
	bool AddedToScene : 1;													// +0x28 bit 0  // NAME 3: 0x6A80D0's FAIL_IF
	bool IsSubIcon : 1;														// +0x28 bit 1  // NAME 3: 0x6A8D00's assert
};

#endif
