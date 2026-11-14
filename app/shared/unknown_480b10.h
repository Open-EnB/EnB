// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no assert or __FILE__ names it;
**	the object links between ro_proxy.cpp and stars.cpp)
**
**	A light whose colour, ambient, diffuse, specular and intensity follow splines read from an ini
**	('%s\COLOR' .. '%s\INTENSITY' sections, spline.h's arrays). 0xFC bytes: LightClass (0xE8) and five
**	spline pointers. Vtables 0xAE9FE0 (RenderObjClass), 0xAE9FC8, 0xAE9FC4 (LightClass's other bases).
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 1 [unknown_480b10.h]: no assert or __FILE__ names the file
#ifndef __UNKNOWN_480B10_H
#define __UNKNOWN_480B10_H

#include "light.h"

class INIClass;
class SplineFloatArrayClass;
class SplineColorArrayClass;

// NAME 1: a LightClass driven by splines (descriptive, D10; no RTTI names it)
class SplineLightClass : public LightClass
{
public:
	SplineLightClass(LightType type, INIClass &ini, const char *name);	// 0x480B10  // NAME 2 [type, ini, name]: what they are
	virtual ~SplineLightClass(void);	// +0x04 0x480F40 (scalar deleting 0x480F10)

	void					Save(INIClass &ini, const char *name);	// 0x481020  // NAME 2: what it does
	void					Update(float t, LightClass *light);	// 0x4811A0 (the constructor calls it with 0, this)  // NAME 2: what it does

protected:
	SplineColorArrayClass *	ColorSpline;		// +0xE8 '%s\COLOR'
	SplineFloatArrayClass *	AmbientSpline;		// +0xEC '%s\AMBIENT'
	SplineFloatArrayClass *	DiffuseSpline;		// +0xF0 '%s\DIFFUSE'
	SplineFloatArrayClass *	SpecularSpline;		// +0xF4 '%s\SPECULAR'
	SplineFloatArrayClass *	IntensitySpline;	// +0xF8 '%s\INTENSITY'
};

#endif //__UNKNOWN_480B10_H
