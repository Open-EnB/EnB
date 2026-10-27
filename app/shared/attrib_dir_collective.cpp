// Earth & Beyond client (reconstruction): app\shared\attrib_dir_collective.cpp. Notes at the end of the file.
#include <algorithm>
#include "..\shared\unknown_debug.h"
#include "attrib_dir_collective.h"












RefCountPtr<AttribValueFunctionClass> AttribAverageFunctionClass::Create()
{
	return Create_NEW(new AttribAverageFunctionClass);
}












AttribAverageFunctionClass::AttribAverageFunctionClass() : Changed(false), Unknown1C(0)
{
}









void AttribAverageFunctionClass::Add(float value)
{
	Values.push_back(value);
	Changed = true;
}

void AttribAverageFunctionClass::Remove(float value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), value);
	FAIL_IF(find == Values.end()) return;
	Values.erase(find);
	Changed = true;
}


void AttribAverageFunctionClass::Replace(float old_value, float new_value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), old_value);
	FAIL_IF(find == Values.end()) return;
	*find = new_value;
	Changed = true;
}











float AttribAverageFunctionClass::Calculate()
{
	if (Values.size() == 0) {
		return 0;
	}
	float sum = 0;
	for (std::vector<float>::iterator it = Values.begin(); it != Values.end(); ++it) {
		sum += *it;
	}
	return sum / Values.size();
}

RefCountPtr<AttribValueFunctionClass> AttribMinFunctionClass::Create()
{
	return Create_NEW(new AttribMinFunctionClass);
}

AttribMinFunctionClass::AttribMinFunctionClass() : Sorted(true)
{
}

void AttribMinFunctionClass::Add(float value)
{
	Values.push_back(value);
	Sorted = false;
}

void AttribMinFunctionClass::Remove(float value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), value);
	FAIL_IF(find == Values.end()) return;
	Values.erase(find);
}


void AttribMinFunctionClass::Replace(float old_value, float new_value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), old_value);
	FAIL_IF(find == Values.end()) return;
	*find = new_value;
	Sorted = false;
}










float AttribMinFunctionClass::Calculate()
{
	if (Values.size() == 0) {
		return 0;
	}
	if (!Sorted) {
		std::sort(Values.begin(), Values.end());
	}
	return Values.front();
}

RefCountPtr<AttribValueFunctionClass> AttribMaxFunctionClass::Create()
{
	return Create_NEW(new AttribMaxFunctionClass);
}

AttribMaxFunctionClass::AttribMaxFunctionClass() : Sorted(true)
{
}

void AttribMaxFunctionClass::Add(float value)
{
	Values.push_back(value);
	Sorted = false;
}

void AttribMaxFunctionClass::Remove(float value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), value);
	FAIL_IF(find == Values.end()) return;
	Values.erase(find);
}


void AttribMaxFunctionClass::Replace(float old_value, float new_value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), old_value);
	FAIL_IF(find == Values.end()) return;
	*find = new_value;
	Sorted = false;
}







float AttribMaxFunctionClass::Calculate()
{
	if (Values.size() == 0) {
		return 0;
	}
	if (!Sorted) {
		std::sort(Values.begin(), Values.end());
	}
	return Values.back();
}

AttribFunctionModClass::AttribFunctionModClass(const RefCountPtr<AttribValueFunctionClass> &function, int type, void *owner) : ValueFunction(function), Type(type), Owner(owner)
{
	G_ASSERT(ValueFunction);
}






AttribFunctionModClass::~AttribFunctionModClass()
{
	ValueFunction = 0;
}

float AttribFunctionModClass::Get_Value()
{
	FAIL_IF(!ValueFunction) return 0;
	return ValueFunction->Calculate();
}


/*
**	Reconstruction notes
**
**	client.exe 0x4DA7F0-0x4DBBA0 rebuilt here: the three value functions (average 0xAEC2DC, smallest
**	0xAEC2F8, largest 0xAEC314: Create, the out-of-line constructor, Add, Remove and Replace with their
**	FAIL_IFs on the client's lines 54 / 63, 108 / 116, 159 / 167, Calculate) and the function mod
**	(0xAEC330: G_ASSERT(ValueFunction) on 191, Get_Value's FAIL_IF on 206), with the vector, sort and
**	RefCountPtr instances. Calculate sorts without setting Sorted (as the client does). Not rebuilt yet:
**	the collective itself, 0x4DBBC0-0x4DCFF0 (its constructor and destructor, the members and the
**	destination attributes in two SGI hash_maps, the member callback 0xAEC344, lines 225-401) and its
**	table instances to 0x4DF2B0.
*/
