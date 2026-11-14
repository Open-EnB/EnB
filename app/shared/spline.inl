// Earth & Beyond client (reconstruction): app\shared\spline.inl (the client's __FILE__). Notes at the end.
#include <math.h>
#include "wwmath.h"




















inline int SplineFloatArrayClass::Find_Index(float &t)
{
	float x = fmod(t * Last, Last);
	float fl = floor(x);
	t = x - fl;
	int array_index = WWMath::Float_To_Long(fl);
	if (array_index < 0) {
		array_index += Last;
	}
	G_ASSERT((array_index >= 0) && ((array_index + 1) < Size));
	return array_index;
}

inline float SplineFloatArrayClass::Get_Value(float t)
{
	int index = Find_Index(t);
	return (Data[index + 1] - Data[index]) * t + Data[index];
}

inline float SplineFloatArrayClass::Evaluate(float t)
{
	return Get_Value(t);
}

inline Vector3 SplineColorArrayClass::Get_Value(float t)
{
	return Vector3(Red.Get_Value(t), Green.Get_Value(t), Blue.Get_Value(t));
}

inline Vector3 SplineColorArrayClass::Evaluate(float t)
{
	return Get_Value(t);
}

/*
**	Notes
**
**	The client's spline.inl holds one assert, line 33 '(array_index >= 0) && ((array_index + 1) < Size)'
**	(0xB71B98): Find_Index's. Its out-of-line copies sit in unknown_480b10.cpp's object (0x481520 the float
**	array's Evaluate, 0x481610 the colour array's, 0x4818C0 Find_Index, 0x481960 Get_Value) and in
**	spline.cpp's (0x483CF0). The position t runs 0..1 over the points (fmod(t * Last, Last)); the
**	fraction is the integer part's remainder; the index goes through WWMath::Float_To_Long (fld / fistp).
*/
