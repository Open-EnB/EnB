// Earth & Beyond client (reconstruction): app\shared\spline.cpp (the client's __FILE__). Notes at the end.
#include "spline.h"
























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

/*
**	Notes
**
**	The client's spline.cpp is 0x483000-0x4845F0 (asserts 'size > 0' line 29, 'Size >= 2',
**	'spline_name != NULL'). Only SplineFloatArrayClass::Resize 0x483820 is rebuilt: spln_db.cpp's
**	Evaluate_Curve 0x485620 / Evaluate_Color 0x485790 call it. The rest of the file (the curve
**	classes it implements) is not rebuilt yet.
*/
