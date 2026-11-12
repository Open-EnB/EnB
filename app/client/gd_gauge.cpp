
// Earth & Beyond client (reconstruction): gd_gauge.cpp. Notes at the end of the file.
#include <stdio.h>
#include "gd_gauge.h"
#include "rendobj.h"
#include "vector2.h"
#include "vector3.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x669F60)

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp; declared as avatar.cpp does)

// The longest sub-object name a gauge's format may give
const int GAUGE_OBJ_NAME_SIZE = 32;	// NAME 1: neutral (D10): the asserts' 'size'

// The texture's V offset at the value 1
#define GAUGE_FULL_OFFSET 0.46f	// NAME 1: neutral (D10)

// How fast the second bar follows
#define GAUGE_TRAIL_ACCEL 0.0001f	// NAME 1: neutral (D10)


































GaugeGadgetClass::GaugeGadgetClass(RenderObjClass *mesh, const char *format) :
	Unknown_675120Class(NULL, 0),
	Exists(false)
{
	if (mesh) {
		if (format && *format) {
			const int size = GAUGE_OBJ_NAME_SIZE;
			char obj_name[size + 1];
			obj_name[size] = 0;
			sprintf(obj_name, format, 0);
			G_ASSERT(!obj_name[size]);
			mesh = mesh->Get_Sub_Object_By_Name(obj_name);
		} else {
			mesh->Add_Ref();
		}
		if (mesh) {
			Set_Render_Obj(mesh);
			mesh->Release_Ref();
		}
	}
}


GaugeGadgetClass::~GaugeGadgetClass()
{
	delete Current.Gauge;
	Current.Gauge = NULL;
}


RenderObjClass *GaugeGadgetClass::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	if (!indicator) {
		return(NULL);
	}
	return(Unknown_675120Class::Unknown_24(position, frame, indicator));
}



void GaugeGadgetClass::Set_Render_Obj(RenderObjClass *robj)
{
	Unknown_675120Class::Set_Render_Obj(robj);
	if (robj) {
		Unknown_496AE0(robj, true);
		Exists = true;
		G_ASSERT(!Current.Gauge);
		delete Current.Gauge;
		Current.Gauge = new UVControllerClass(robj, 0, 0);
		Set_Value(0.0f);
	} else {
		Exists = false;
		delete Current.Gauge;
		Current.Gauge = NULL;
	}
}


bool GaugeGadgetClass::Set_Value(float value)
{
	if (!Exists) {
		return(false);
	}
	if (value < 0.0f) {
		value = 0.0f;
	} else if (value > 1.0f) {
		value = 1.0f;
	}
	if (Current.Value == value) {
		return(false);
	}
	Current.Gauge->Unknown_4A13A0(Vector2(0.0f, value * GAUGE_FULL_OFFSET));
	Current.Value = value;
	return(true);
}


float GaugeGadgetClass::Force_Value(float value)
{
	if (value < 0.0f) {
		value = 0.0f;
	} else if (value > 1.0f) {
		value = 1.0f;
	}
	if (Exists) {
		Current.Gauge->Unknown_4A13A0(Vector2(0.0f, value * GAUGE_FULL_OFFSET));
	}
	Current.Value = value;
	return(value);
}



TrailGaugeGadgetClass::TrailGaugeGadgetClass(RenderObjClass *mesh, const char *format) :
	GaugeGadgetClass(mesh, format),
	TrailOffset(0.0f)
{
	if (Exists) {
		const int size = GAUGE_OBJ_NAME_SIZE;
		char obj_name[size + 1];
		obj_name[size] = 0;
		sprintf(obj_name, format, 1);
		G_ASSERT(!obj_name[size]);
		RenderObjClass *robj = mesh->Get_Sub_Object_By_Name(obj_name);
		if (robj) {
			Unknown_496AE0(robj, true);
			Trail.Gauge = new UVControllerClass(robj, 0, 0);
			robj->Release_Ref();
			TrailAccel.Set(0.0f, 0.0f, GAUGE_TRAIL_ACCEL);
		}
		Set_Value(0.0f);
	}
}


TrailGaugeGadgetClass::~TrailGaugeGadgetClass()
{
	delete Trail.Gauge;
	Trail.Gauge = NULL;
}


bool TrailGaugeGadgetClass::Set_Value(float value)
{
	bool changed = GaugeGadgetClass::Set_Value(value);
	float current = Get_Value();
	if (changed) {
		if (current > Trail.Value) {
			TrailAccel.Set(current, current, GAUGE_TRAIL_ACCEL);
		} else {
			TrailAccel.Set_Target(current, GAUGE_TRAIL_ACCEL);
		}
	}
	if (Trail.Gauge) {
		float trail = TrailAccel.Get_Value((float)WW3D::Get_Frame_Time()) + TrailOffset;
		if (trail < 0.0f) {
			trail = 0.0f;
		} else if (trail > 1.0f) {
			trail = 1.0f;
		}
		if (trail != Trail.Value) {
			Trail.Gauge->Unknown_4A13A0(Vector2(0.0f, trail * GAUGE_FULL_OFFSET));
			Trail.Value = trail;
		}
	}
	return(changed);
}


float TrailGaugeGadgetClass::Force_Value(float value)
{
	value = GaugeGadgetClass::Force_Value(value);
	if (Trail.Gauge) {
		TrailAccel.Set(value, value, GAUGE_TRAIL_ACCEL);
		Trail.Gauge->Unknown_4A13A0(Vector2(0.0f, value * GAUGE_FULL_OFFSET));
		Trail.Value = value;
	}
	return(value);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_gauge.cpp (the client's own string: the asserts
**	'!obj_name[size]' l.65 / l.157 and '!Current.Gauge' l.101)
**
**	The gauge gadgets (gd_gauge.h): 0x669570-0x669FA0, tables 0xAFA1AC (GaugeGadgetClass) and 0xAFA280
**	(TrailGaugeGadgetClass). 0x669680 between the first constructor and its destructor is gd_gauge.h's
**	~GaugeBarStruct (the assert '!Gauge', gd_gauge.h l.69), emitted here for the constructors' unwinding.
*/
