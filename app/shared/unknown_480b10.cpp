// Earth & Beyond client (reconstruction): app\shared\ (file name unknown). Notes at the end of the file.
#include "unknown_480b10.h"
#include "spline.h"
#include "ini.h"
#include <stdio.h>

SplineLightClass::SplineLightClass(LightType type, INIClass &ini, const char *name) :
	LightClass(type)
{
	char section[256];

	sprintf(section, "%s\\COLOR", name);
	if (ini.Is_Present(section)) {
		ColorSpline = new SplineColorArrayClass;
		ColorSpline->Load(ini, section, "");
	} else {
		ColorSpline = NULL;
	}

	sprintf(section, "%s\\AMBIENT", name);
	if (ini.Is_Present(section)) {
		AmbientSpline = new SplineFloatArrayClass;
		AmbientSpline->Load(ini, section, "");
	} else {
		Ambient.Set(0.0f, 0.0f, 0.0f);
		AmbientSpline = NULL;
	}

	sprintf(section, "%s\\DIFFUSE", name);
	if (ini.Is_Present(section)) {
		DiffuseSpline = new SplineFloatArrayClass;
		DiffuseSpline->Load(ini, section, "");
	} else {
		DiffuseSpline = NULL;
		Diffuse.Set(1.0f, 1.0f, 1.0f);
	}

	sprintf(section, "%s\\SPECULAR", name);
	if (ini.Is_Present(section)) {
		SpecularSpline = new SplineFloatArrayClass;
		SpecularSpline->Load(ini, section, "");
	} else {
		SpecularSpline = NULL;
		Specular.Set(1.0f, 1.0f, 1.0f);
	}

	sprintf(section, "%s\\INTENSITY", name);
	if (ini.Is_Present(section)) {
		IntensitySpline = new SplineFloatArrayClass;
		IntensitySpline->Load(ini, section, "");
	} else {
		IntensitySpline = NULL;
		Intensity = 1.0f;
	}

	FarAttenStart = 1000000.0f;
	FarAttenEnd = 1000000.0f;
	Update(0.0f, this);
}

SplineLightClass::~SplineLightClass(void)
{
	delete ColorSpline;
	delete AmbientSpline;
	delete DiffuseSpline;
	delete SpecularSpline;
	delete IntensitySpline;
}

void SplineLightClass::Save(INIClass &ini, const char *name)
{
	char section[256];

	if (ColorSpline != NULL) {
		sprintf(section, "%s\\COLOR", name);
		ColorSpline->Save(ini, section, "");
	}
	if (AmbientSpline != NULL) {
		sprintf(section, "%s\\AMBIENT", name);
		AmbientSpline->Save(ini, section, "");
	}
	if (DiffuseSpline != NULL) {
		sprintf(section, "%s\\DIFFUSE", name);
		DiffuseSpline->Save(ini, section, "");
	}
	if (SpecularSpline != NULL) {
		sprintf(section, "%s\\SPECULAR", name);
		SpecularSpline->Save(ini, section, "");
	}
	if (IntensitySpline != NULL) {
		sprintf(section, "%s\\INTENSITY", name);
		IntensitySpline->Save(ini, section, "");
	}
}

void SplineLightClass::Update(float t, LightClass *light)
{
	Vector3 color;
	if (ColorSpline != NULL) {
		color = ColorSpline->Get_Value(t);
	} else {
		color.Set(1.0f, 1.0f, 1.0f);
	}

	if (DiffuseSpline != NULL) {
		light->Set_Diffuse(DiffuseSpline->Get_Value(t) * color);
	} else {
		light->Set_Diffuse(color);
	}

	if (AmbientSpline != NULL) {
		light->Set_Ambient(color * AmbientSpline->Get_Value(t));
	} else {
		light->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	}

	if (SpecularSpline != NULL) {
		light->Set_Specular(color * SpecularSpline->Get_Value(t));
	} else {
		light->Set_Specular(color);
	}

	if (IntensitySpline != NULL) {
		light->Set_Intensity(IntensitySpline->Get_Value(t));
	} else {
		light->Set_Intensity(1.0f);
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x480B10-0x481A20 (after ro_proxy.cpp's ProxyRObj 0x480AF0, before stars.cpp 0x481A20; no
**	guarded static initialiser in between). The object's own functions:
**
**	  0x480B10 SplineLightClass::SplineLightClass      0x481020 Save
**	  0x480F10 / 0x480F40 scalar deleting dtor / dtor  0x4811A0 Update
**
**	and what it emits of its headers: LightClass's adjustor thunks 0x4814A0 / 0x4814B0 and inline
**	virtuals 0x4814C0 Class_ID, 0x4814E0 Render, 0x481500 Is_Vertex_Processor; spline.inl's 0x481520
**	SplineFloatArrayClass::Evaluate, 0x481610 SplineColorArrayClass::Evaluate, 0x4818C0 Find_Index,
**	0x481960 Get_Value; spline.h's 0x4815E0 / 0x481810 scalar deleting dtors, 0x481840
**	~SplineColorArrayClass.
**
**	The constructor has no caller in the client (its only reference is an unused incremental-link
**	thunk 0x409A1B). The pooled empty string 0xBDA908 is the prefix every spline Load / Save gets.
*/
