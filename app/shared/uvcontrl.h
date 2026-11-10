// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim header (D137): the client holds no string of the header that declares uvcontrl.cpp's classes; named after the
**	.cpp (e:\build\beta\G\app\shared\uvcontrl.cpp, its FAIL_IFs) as E&B's own files pair them.
**
**	Controllers of one texture mapper of a render object's vertex material: the base finds the mapper (RObj, Mapper: the
**	FAIL_IFs' names), UVControllerClass drives a linear offset mapper, Unknown4A13F0Class a grid mapper.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UVCONTRL_H  // NAME 3 [uvcontrl.h]: header-from-file (D137) for app\shared\uvcontrl.cpp
#define UVCONTRL_H

class RenderObjClass;
class TextureMapperClass;
class Vector2;

// The controllers' base (table 0xAEAA18: its destructor only)
class Unknown4A0FB0Class	// NAME 1: neutral (D10)
{
public:
	Unknown4A0FB0Class(RenderObjClass *robj, int index, int stage, int mapper_id);	// 0x4A0FB0 (inlined into the derived constructors)  // NAME 1 [robj, index, stage]: what they select; [mapper_id]: the FAIL_IF's
	virtual ~Unknown4A0FB0Class();						// 0x4A1080 (deleting 0x4A1000)
	void Unknown_4A10D0(RenderObjClass *robj, int index, int stage, int mapper_id);	// 0x4A10D0 (the mapper of stage stage of vertex material index)  // NAME 1: neutral

protected:
	RenderObjClass *RObj;								// +0x04 (FAIL_IF '!RObj')
	TextureMapperClass *Mapper;							// +0x08 (FAIL_IF '!Mapper')
};

// A linear offset mapper's controller (table 0xAEAA20); Unknown_671300Class scrolls its texture with it
class UVControllerClass : public Unknown4A0FB0Class	// NAME 3: class-from-file (D24) for app\shared\uvcontrl.cpp
{
public:
	UVControllerClass(RenderObjClass *robj, int index, int stage);	// 0x4A1240 (MAPPER_ID_LINEAR_OFFSET)
	virtual ~UVControllerClass();						// 0x4A1350 (deleting 0x4A1320)
	void Unknown_4A13A0(const Vector2 &offset);			// 0x4A13A0 (the mapper's current UV offset)  // NAME 1 [offset]: what it sets
};

// A grid mapper's controller (table 0xAEAA24)
class Unknown4A13F0Class : public Unknown4A0FB0Class	// NAME 1: neutral (D10)
{
public:
	Unknown4A13F0Class(RenderObjClass *robj, int index, int stage);	// 0x4A13F0 (MAPPER_ID_GRID, 0 frames per second)
	virtual ~Unknown4A13F0Class();						// 0x4A1500 (deleting 0x4A14D0)
	void Unknown_4A1550(unsigned int frame);			// 0x4A1550 (the mapper's current frame)  // NAME 1 [frame]: what it sets
};

#endif
