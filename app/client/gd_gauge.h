// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim header (D137): a view of gd_gauge.cpp's gauge gadget (its asserts: 'e:\build\beta\G\app\Client\gd_gauge.cpp',
**	'!obj_name[size]' l.157), as far as ui_power.cpp derives from it. Its code (0x669570-0x669F..) is not rebuilt yet: the
**	constructor 0x669A30 builds on 0x669570 (a class of its own in the client, folded in here); fields and slots are typed
**	from what 0x669A30 and ui_power.cpp's 0x5DBFC0 / 0x5DC4A0 / 0x5DCBE0 do with them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_GAUGE_H  // NAME 3 [gd_gauge.h]: header name assumed from the client's gd_gauge.cpp
#define GD_GAUGE_H

#include "unknown_675120.h"

class RenderObjClass;
class Vector3;

// A gauge (0x88 bytes, table 0xAFA280): the mesh's sub-object named by format and 1, its texture scrolled to the value
class GaugeGadgetClass : public Unknown_675120Class	// NAME 3: class-from-file (D24) for app\client\gd_gauge.cpp
{
public:
	GaugeGadgetClass(RenderObjClass *mesh, const char *format);	// 0x669A30 (0x669570 first; sprintf(format, 1), the UV controller 0x4A1240)  // NAME 1 [mesh, format]: what ui_power.cpp passes
	virtual ~GaugeGadgetClass();						// +0x00 0x669BD0 (deleting 0x669BA0)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x669760  // NAME 1 [position, frame, indicator]: the base's
	virtual void Set_Render_Obj(RenderObjClass *robj);	// +0x94 0x669790  // NAME 1 [robj]: the base's
	virtual bool Unknown_A4(float value);				// +0xA4 0x669C70 (false when it changed nothing; sets +0x68)  // NAME 1 [value]: neutral
	float Get_Value() { return(Value); }				// (inline: ui_power.cpp's 0x5DCBE0 loads it through the FPU)  // NAME 2: what it returns
	virtual float Unknown_A8(float value);				// +0xA8 0x669E70 (ui_power.cpp's 0x5DC390 passes 0)  // NAME 1 [value]: neutral

	bool Exists;										// +0x60 (the sub-object found: 0x669A30 tests it)  // NAME 2: as ui_experience.cpp's bars'
	char Unknown61[0x68 - 0x61];						// +0x61
	float Value;										// +0x68 (0x5DCBE0 reads it before and after the base's slot +0xA4)  // NAME 1: neutral
	char Unknown6C[0x88 - 0x6C];						// +0x6C (+0x6C the UV controller, +0x70 -1, +0x74 an AccelClass, +0x84)
};

#endif
