// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_gauge.h (the client's own string: the assert '!Gauge' of
**	0x669680 names it, line 69; gd_gauge.cpp's asserts are '!obj_name[size]' l.65 / l.157 and '!Current.Gauge' l.101)
**
**	The gauge gadgets: a bar of a screen's model whose texture is scrolled to a value of 0..1.
**	GaugeGadgetClass (table 0xAFA1AC, 43 slots, 0x6C bytes, constructor 0x669570): the model gadget without a scene
**	(unknown_675120.h) over the mesh's sub-object named by sprintf(format, 0), with slots +0xA4 / +0xA8 that set the
**	value. TrailGaugeGadgetClass (table 0xAFA280, 0x88 bytes, constructor 0x669A30) adds the sub-object
**	sprintf(format, 1): a second bar that runs after the first one's value (an AccelClass).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_GAUGE_H
#define GD_GAUGE_H

#include "unknown_675120.h"
#include "..\shared\accel.h"
#include "..\shared\uvcontrl.h"
#include "..\shared\g_assert.h"

class RenderObjClass;
class Vector3;






























// One bar (8 bytes): the UV controller of its texture and the value it shows (-1: none yet). Whoever owns it
// deletes the controller before the bar goes (the assert).
struct GaugeBarStruct  // NAME 1: neutral (D10): what it holds
{
	GaugeBarStruct() : Gauge(NULL), Value(-1.0f) {}




	~GaugeBarStruct()
	{
		G_ASSERT(!Gauge);
	}

	UVControllerClass *Gauge;							// +0x00 (G_ASSERT '!Gauge' l.69, '!Current.Gauge')  // NAME 3: the asserts'
	float Value;										// +0x04 (0..1; the texture's V offset is 0.46 of it)  // NAME 1: what slots +0xA4 / +0xA8 store
};

// A gauge (0x6C bytes, table 0xAFA1AC): ui_faction.cpp's love / hate bars, skill_render.cpp's 'REACTOR_BAR1'
class GaugeGadgetClass : public Unknown_675120Class	// NAME 3: class-from-file (D24) for app\Client\gd_gauge.cpp
{
public:
	GaugeGadgetClass(RenderObjClass *mesh, const char *format);	// 0x669570 (the sub-object sprintf(format, 0), or the mesh itself without a format)  // NAME 1 [mesh, format]: what it does with them
	virtual ~GaugeGadgetClass();						// +0x00 0x6696E0 (deleting 0x6696B0)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x669760 (the base's, only with the indicator)  // NAME 1 [position, frame, indicator]: the base's
	virtual void Set_Render_Obj(RenderObjClass *robj);	// +0x94 0x669790 (G_ASSERT '!Current.Gauge' l.101: the bar's UV controller made, the value 0)  // NAME 1 [robj]: the base's
	virtual bool Set_Value(float value);				// +0xA4 0x6698C0 (clamped to 0..1; false without a bar or when it is the value already)  // NAME 2: what it does; NAME 1 [value]: what it stores
	virtual float Force_Value(float value);				// +0xA8 0x669990 (clamped to 0..1 and stored, shown or not; returns it)  // NAME 2: what it does; NAME 1 [value]: what it stores
	float Get_Value() { return(Current.Value); }		// (inline: ui_power.cpp's 0x5DCBE0 loads it through the FPU)  // NAME 2: what it returns

	bool Exists;										// +0x60 (the sub-object found: Set_Render_Obj sets it, the slots test it)  // NAME 2: as ui_experience.cpp's bars'
	GaugeBarStruct Current;								// +0x64 (G_ASSERT '!Current.Gauge')  // NAME 3: the assert's
};

// A gauge with a second bar that follows the value (0x88 bytes, table 0xAFA280): ui_power.cpp's three
class TrailGaugeGadgetClass : public GaugeGadgetClass	// NAME 1: neutral (D10): what the second bar does
{
public:
	TrailGaugeGadgetClass(RenderObjClass *mesh, const char *format);	// 0x669A30 (G_ASSERT '!obj_name[size]' l.157: the sub-object sprintf(format, 1))  // NAME 1 [mesh, format]: the base's
	virtual ~TrailGaugeGadgetClass();					// +0x00 0x669BD0 (deleting 0x669BA0)
	virtual bool Set_Value(float value);				// +0xA4 0x669C70 (the base's, then the second bar a step after it: every frame)  // NAME 1 [value]: the base's
	virtual float Force_Value(float value);				// +0xA8 0x669E70 (both bars at once; ui_power.cpp's 0x5DC390 passes 0)  // NAME 1 [value]: the base's

	GaugeBarStruct Trail;								// +0x6C (the second bar)  // NAME 1: neutral (D10): what it does
	AccelClass TrailAccel;								// +0x74 (0x10 bytes: the second bar's value on its way to the first's)  // NAME 1: its type and whose it is
	float TrailOffset;									// +0x84 (zero; added to the second bar's value)  // NAME 1: what 0x669C70 does with it
};

#endif
