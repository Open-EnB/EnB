// Earth & Beyond client (reconstruction): the starbase lights manager, 0x764480-0x764FCF (PROVISIONAL file, D137): starbase_lights.ini's
// sections STARBASE-a-b-c (0x640D80's three values) read and written for the starbase interface and its lights panel (B1)
#include "starbase_lights.h"
#include <stdio.h>
#include "point.h"
#include "..\..\ww3d2\light.h"

Unknown_764480Class *Unknown_BE82D0;	// 0xBE82D0

// 0x764480
Unknown_764480Class::Unknown_764480Class()
{
	INI.Load("starbase_lights.ini");
}

// 0x7644F0
Unknown_764480Class::~Unknown_764480Class()
{
}

// 0x764510
void Unknown_764510(Unknown_764480Class *manager)
{
	Unknown_BE82D0 = manager;
}

// 0x764530
void Unknown_764480Class::Unknown_764530()
{
	INI.Load("starbase_lights.ini");
}

// 0x764550
void Unknown_764480Class::Unknown_764550()
{
	INI.Save("starbase_lights.ini");
}

// 0x764570
LightClass *Unknown_764480Class::Unknown_764570(int a, int b, int c, int index)
{
	LightClass *light = new LightClass;
	light->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	light->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	light->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
	char entry[64];
	char section[64];
	TPoint3D<float> zero(0.0f, 0.0f, 0.0f);
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	if (!INI.Find_Section(section)) {
		return(light);
	}
	_snprintf(entry, sizeof(entry), "LightRGB%d", index);
	if (INI.Find_Entry(section, entry)) {
		TPoint3D<float> rgb = INI.Get_Point(section, entry, zero);
		light->Set_Diffuse(*(Vector3 *)&rgb);
	}
	_snprintf(entry, sizeof(entry), "AmbientRGB%d", index);
	if (INI.Find_Entry(section, entry)) {
		TPoint3D<float> rgb = INI.Get_Point(section, entry, zero);
		light->Set_Ambient(*(Vector3 *)&rgb);
	}
	_snprintf(entry, sizeof(entry), "Near%d", index);
	float near_range = INI.Get_Float(section, entry, 0.0f);
	_snprintf(entry, sizeof(entry), "Far%d", index);
	float far_range = INI.Get_Float(section, entry, 50.0f);
	light->Set_Far_Attenuation_Range(near_range, far_range);
	_snprintf(entry, sizeof(entry), "Intensity%d", index);
	light->Set_Intensity(INI.Get_Float(section, entry, 0.0f));
	return(light);
}

// 0x764880
void Unknown_764480Class::Unknown_764880(int a, int b, int c, int index, LightClass *light)
{
	char section[64];
	char entry[64];
	Vector3 color;
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	_snprintf(entry, sizeof(entry), "LightRGB%d", index);
	light->Get_Diffuse(&color);
	INI.Put_Point(section, entry, TPoint3D<float>(color.X, color.Y, color.Z));
	_snprintf(entry, sizeof(entry), "AmbientRGB%d", index);
	light->Get_Ambient(&color);
	INI.Put_Point(section, entry, TPoint3D<float>(color.X, color.Y, color.Z));
	float intensity = light->Get_Intensity();
	_snprintf(entry, sizeof(entry), "Intensity%d", index);
	INI.Put_Float(section, entry, intensity);
	float near_range, far_range;
	light->Get_Far_Attenuation_Range(near_range, far_range);
	_snprintf(entry, sizeof(entry), "Near%d", index);
	INI.Put_Float(section, entry, near_range);
	_snprintf(entry, sizeof(entry), "Far%d", index);
	INI.Put_Float(section, entry, far_range);
}

// NOT EXACT: 0x764AB0, functional (register / order: the client keeps the offset's y on the FPU across the branch and copies x / z as integers; B1)
// 0x764AB0
Vector3 Unknown_764480Class::Unknown_764AB0(int a, int b, int c, int index)
{
	char section[64];
	char entry[64];
	TPoint3D<float> offset(0.5f, 0.5f, 0.0f);
	TPoint3D<float> center(0.5f, 0.5f, 0.0f);
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	if (!INI.Find_Section(section)) {
		return(Vector3(0.5f, 0.5f, 0.0f));
	}
	_snprintf(entry, sizeof(entry), "Offset%d", index);
	if (INI.Find_Entry(section, entry)) {
		TPoint3D<float> point = INI.Get_Point(section, entry, center);
		offset = point;
	}
	return(*(Vector3 *)&offset);
}

// 0x764C30
void Unknown_764480Class::Unknown_764C30(int a, int b, int c, int index, const Vector3 &point)
{
	char section[64];
	char entry[64];
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	_snprintf(entry, sizeof(entry), "Offset%d", index);
	INI.Put_Point(section, entry, TPoint3D<float>(point.X, point.Y, point.Z));
}

// NOT EXACT: 0x764CF0, functional (register / order: the client keeps the colour's y on the FPU across the branch and copies x / z as integers; B1)
// 0x764CF0
Vector3 Unknown_764480Class::Unknown_764CF0(int a, int b, int c)
{
	char section[64];
	TPoint3D<float> color(0.0f, 0.0f, 0.0f);
	TPoint3D<float> black(0.0f, 0.0f, 0.0f);
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	if (!INI.Find_Section(section)) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	if (INI.Find_Entry(section, "Ambient")) {
		TPoint3D<float> point = INI.Get_Point(section, "Ambient", black);
		color = point;
	}
	return(*(Vector3 *)&color);
}

// 0x764E20
void Unknown_764480Class::Unknown_764E20(int a, int b, int c, const Vector3 &color)
{
	char section[64];
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	INI.Put_Point(section, "Ambient", TPoint3D<float>(color.X, color.Y, color.Z));
}

// 0x764EA0
float Unknown_764480Class::Unknown_764EA0(int a, int b, int c)
{
	char section[64];
	float fill = 0.75f;
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	if (!INI.Find_Section(section)) {
		return(fill);
	}
	if (INI.Find_Entry(section, "Fill")) {
		fill = INI.Get_Float(section, "Fill", 0.75f);
	}
	return(fill);
}

// 0x764F60
void Unknown_764480Class::Unknown_764F60(int a, int b, int c, float fill)
{
	char section[64];
	_snprintf(section, sizeof(section), "STARBASE-%d-%d-%d", a, b, c);
	INI.Put_Float(section, "Fill", fill);
}
