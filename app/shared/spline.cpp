// Earth & Beyond client (reconstruction): app\shared\spline.cpp (the client's __FILE__). Notes at the end.
#include "spline.h"
#include "ini.h"
#include <stdio.h>

SplineFloatArrayClass::SplineFloatArrayClass(int size)
{
	Size = size;
	Last = size - 1;
	Data = new float[size];
}

SplineFloatArrayClass::~SplineFloatArrayClass(void)
{
	if (Data != NULL) {
		delete [] Data;
	}
}








void SplineFloatArrayClass::Resize(unsigned int size, float value)
{
	G_ASSERT(size > 0);
	if (Data != NULL) {
		delete [] Data;
	}
	Size = size;
	Last = size - 1;
	Data = new float[size];
	while (size--) {
		Data[size] = value;
	}
}






















void SplineFloatArrayClass::Read(INIClass &ini, const char *section, const char *prefix)
{
	char entry[256];

	Last = Size - 1;
	G_ASSERT(Size >= 2);
	Data = new float[Size];
	int index = Size;
	while (index--) {
		sprintf(entry, "%s%d", prefix, index);
		Data[index] = ini.Get_Float(section, entry);
	}
}

void SplineFloatArrayClass::Load(INIClass &ini, const char *section, const char *prefix)
{
	char entry[256];

	if (Data != NULL) {
		delete [] Data;
	}
	sprintf(entry, "%sC", prefix);
	Size = ini.Get_Int(section, entry);
	if (Size == 0) {
		Size = 2;
		Last = 1;
		Data = new float[2];
		Data[0] = 1.0f;
		Data[1] = 0.0f;
	} else {
		Read(ini, section, prefix);
	}
}

void SplineFloatArrayClass::Save(INIClass &ini, const char *section, const char *prefix)
{
	char entry[256];

	sprintf(entry, "%sC", prefix);
	ini.Put_Int(section, entry, Size);
	int index = Size;
	while (index--) {
		sprintf(entry, "%s%d", prefix, index);
		ini.Put_Float(section, entry, Data[index]);
	}
}

SplineFloatArrayClass *SplineFloatArrayClass::Create(INIClass &ini, const char *section, const char *prefix)
{
	char entry[256];

	sprintf(entry, "%sC", prefix);
	int size = ini.Get_Int(section, entry);
	if (size <= 0) {
		return NULL;
	}
	SplineFloatArrayClass *array = new SplineFloatArrayClass;
	array->Size = size;
	array->Read(ini, section, prefix);
	return array;
}

float SplineFloatArrayOffsetClass::Get_Value(float t)
{
	return SplineFloatArrayClass::Get_Value(t + Offset);
}

void SplineColorArrayClass::Load(INIClass &ini, const char *section, const char *prefix)
{
	char name[256];

	if (prefix != NULL && *prefix != 0) {
		sprintf(name, "%sX", prefix);
		Red.Load(ini, section, name);
		sprintf(name, "%sY", prefix);
		Green.Load(ini, section, name);
		sprintf(name, "%sZ", prefix);
		Blue.Load(ini, section, name);
	} else {
		Red.Load(ini, section, "X");
		Green.Load(ini, section, "Y");
		Blue.Load(ini, section, "Z");
	}
}

void SplineColorArrayClass::Save(INIClass &ini, const char *section, const char *prefix)
{
	char name[256];

	if (prefix != NULL && *prefix != 0) {
		sprintf(name, "%sX", prefix);
		Red.Save(ini, section, name);
		sprintf(name, "%sY", prefix);
		Green.Save(ini, section, name);
		sprintf(name, "%sZ", prefix);
		Blue.Save(ini, section, name);
	} else {
		Red.Save(ini, section, "X");
		Green.Save(ini, section, "Y");
		Blue.Save(ini, section, "Z");
	}
}

SplineColorArrayClass *SplineColorArrayClass::Create(INIClass &ini, const char *section)
{
	int size = ini.Get_Int(section, "XC");
	if (size <= 0) {
		return NULL;
	}
	SplineColorArrayClass *colors = new SplineColorArrayClass(size);
	colors->Red.Read(ini, section, "X");
	colors->Green.Read(ini, section, "Y");
	colors->Blue.Read(ini, section, "Z");
	return colors;
}

/*
**	Notes
**
**	The client's spline.cpp is 0x4837B0-0x4845F0 (asserts 'size > 0' line 29, 'Size >= 2' line 67):
**	the float array's int constructor 0x4837B0, destructor 0x4837F0, Resize 0x483820, Load 0x4838B0,
**	Read 0x483A20, Save 0x483AF0, Create 0x483BB0; the offset array's Get_Value 0x483CF0 (spline.inl's
**	Find_Index inlined: the client's only copy reading +0x10); the colour array's Load 0x483DB0, Save
**	0x483EE0, Create 0x4842E0. Read, Create and the offset Get_Value have no caller in the client; Load
**	and both Creates inline Read ('Size >= 2' sits in all three on line 67), the colour Create inlines the
**	int constructor three times. Create's count entries are '%sC' (the float array) and 'XC' (the colour,
**	whose channels are 'X', 'Y', 'Z' or the prefix plus 'X', 'Y', 'Z').
*/
