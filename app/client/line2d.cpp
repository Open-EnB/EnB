// Earth & Beyond client (reconstruction): app\client\line2d.cpp. Notes at the end of the file.
#include "line2d.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwmath\matrix3d.h"
#include <math.h>

// @0x6DF890
Line2DClass::Line2DClass(const Vector4 &color, const Vector2 &from, const Vector2 &to, float width) :
	Unknown6E1AB0Class(color, Vector2(0, 0), Vector2(0.1f, 0.1f))
{
	Set(from, to, width);
}

// @0x6DFA50
// NOT EXACT: 0.64, stack layout: the client spills end.Y, the width and the angle into dead parameter slots (frame 0x58, ours 0x60) and copies the translation with integer moves
void Line2DClass::Set(const Vector2 &from, const Vector2 &to, float width)
{
	Vector2 start(from.X * 2 - 1, 1 - from.Y * 2);
	Vector2 end(to.X * 2 - 1, 1 - to.Y * 2);

	float dx = end.X - start.X;
	float adx = (float)fabs(dx);
	float dy = end.Y - start.Y;
	float ady = (float)fabs(dy);
	float length = (float)sqrt(ady * ady + adx * adx);

	float thickness;
	if (width != 0) {
		int w, h, bits;
		bool windowed;
		WW3D::Get_Device_Resolution(w, h, bits, windowed);
		if (adx == 0) {
			thickness = width / h;
		} else if (ady == 0) {
			thickness = width / w;
		} else {
			thickness = (width / h * ady) / (ady + adx) + (width / w * adx) / (ady + adx);
		}
	} else {
		Get_Vertex(1, thickness, width, width);
	}

	Move_Vertex(0, -thickness, 0, 0);
	Move_Vertex(1, thickness, 0, 0);
	Move_Vertex(2, -thickness, length, 0);
	Move_Vertex(3, thickness, length, 0);

	Matrix3D tm(Vector3(start.X, start.Y, 0));
	tm.Rotate_Z((float)-atan2(dx, dy));
	Set_Transform(tm);
	Set_Dirty();
}

/*
**	Reconstruction notes (line2d.cpp, client 0x6DF890-0x6DFD80; PROVISIONAL file: the client holds no string
**	of it; the object links after interface_static_sort_list.obj (its guard 0x6DF820, its COMDATs to 0x6DF880)
**	and before linebox.obj (0x6DFD80), and holds Line2DClass's code and the inlines line2d.h emitted).
**
**	0x6DF960 (Unknown6E1AB0Class::Class_ID) and 0x6DFA10 (its destructor) are the base's inlines, kept from
**	this object because it links before the base's own; 0x6DF980 / 0x6DF9B0 are line2d.h's asserting
**	setters, 0x6DF9E0 the scalar deleting destructor, 0x6DFA30 the destructor, 0x6DFD60 / 0x6DFD70 the
**	compiler's adjustor thunks for the second and third tables.
**
**	Set: the points are 0..1 screen coordinates (y down) turned into -1..1 view space; the quad (vertices
**	0, 1 at the start, 2, 3 at the length) is half a width either side, the width in pixels over the
**	device's resolution weighted by the line's slope; a width of 0 keeps vertex 1's x.
*/
