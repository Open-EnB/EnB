// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_interpolator.h (the client's
**	__FILE__ string, from Get_Value's assert on line 54, inlined in 0x4A67D0 and 0x4A6B30)
**
**	A value in [0, 1] moving at ChangePerTick per tick from StartValue until EndTime; the three
**	fields are children (the constructor's names). The client doesn't name the class (D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_INTERPOLATOR_H
#define __DELTA_INTERPOLATOR_H

#include "delta_template.h"
#include "..\duration.h"
#include "..\unknown_debug.h"

// vtable 0xAEAE54, size 0x250
class DeltaInterpolatorClass : public DeltaCompoundClass  // NAME 3: by convention (D24) from the client's file delta_interpolator.cpp (D59)
{
public:
	DeltaInterpolatorClass(const char *name);											// 0x4A6110
	virtual void Unknown_00(DeltaChildVisitorClass *visitor);					// +0x00 0x4A6930
	virtual void Unknown_3C(DeltaClass *delta);									// +0x3C 0x4A68E0

	float Get_Change() const;																// 0x4A6360  // NAME 2: describes what it does
	float Update(const DurationClass &time);											// 0x4A6380  // NAME 2: describes what it does
	void Set(float value, float change, const DurationClass &time);			// 0x4A6590  // NAME 2: describes what it does
	void Set(float value, const DurationClass &time);								// 0x4A67A0
	void Set_Change(float change, const DurationClass &time);					// 0x4A67D0  // NAME 2: describes what it does

	// the value at time (inlined in 0x4A67D0 and 0x4A6B30)
	float Get_Value(const DurationClass &time)  // NAME 2: describes what it does
	{













		G_ASSERT(time >= LastTime);
		if (m_Unknown24C || time == LastTime) {
			return LastValue;
		}
		return Update(time);
	}

	DeltaTemplateClass<DurationClass> EndTime;		// +0xA0
	DeltaTemplateClass<float> ChangePerTick;		// +0x130
	DeltaTemplateClass<float> StartValue;			// +0x1B8 (the value at LastTime when Set)
	DurationClass LastTime;								// +0x240
	float LastValue;										// +0x248  // NAME 2: after the assert's LastTime
	bool m_Unknown24C;									// +0x24C set when the value can't change (no change, or EndTime reached)
};

#endif //__DELTA_INTERPOLATOR_H
