// Earth & Beyond client (reconstruction): a Line3DClass with a sort level, client 0x466920-0x466BC0 (notes at the end)
#include "unknown_466920.h"
#include "..\..\ww3d2\ww3d.h"


// 0x466920 (the destructor 0x4669D0, deleting 0x4669A0, the adjustor thunks 0x466B60 / 0x466B70)
Unknown_466920Class::Unknown_466920Class(const Vector3 &start, const Vector3 &end, float width, float r, float g, float b, float opacity) :
	Line3DClass(start, end, width, r, g, b, opacity),
	SortLevel(0)
{
}

// 0x4669F0
Unknown_466920Class::Unknown_466920Class(const Unknown_466920Class &src) :
	Line3DClass(src),
	SortLevel(src.SortLevel)
{
}

// 0x466A40
Unknown_466920Class &Unknown_466920Class::operator=(const Unknown_466920Class &that)
{
	Line3DClass::operator=(that);
	SortLevel = that.SortLevel;
	return(*this);
}

// 0x466A80
RenderObjClass *Unknown_466920Class::Clone() const
{
	return(new Unknown_466920Class(*this));
}

// 0x466B20: on the static sort list at its level while static sort lists are on, else drawn as a plain line
void Unknown_466920Class::Render(RenderInfoClass &rinfo)
{
	if (WW3D::Are_Static_Sort_Lists_Enabled() && SortLevel) {
		WW3D::Add_To_Static_Sort_List(this, SortLevel);
		return;
	}
	Line3DClass::Render(rinfo);
}


/*
**	Reconstruction notes (unknown_466920.cpp, client 0x466920-0x466BC0).
**
**	- The object between heightfield.cpp (to 0x4668F0) and landdmap.cpp (from 0x466BE0): the constructor, the
**	  compiler's destructor (a jump to ~Line3DClass) and deleting destructor, the copy constructor and operator=
**	  (neither called: written out of line here, Clone inlines the copy), Clone, Render, the two adjustor thunks
**	  of the deleting destructor (tables at +0x08 / +0x10), then the inline Class_ID / Get_Sort_Level /
**	  Set_Sort_Level COMDATs 0x466B80-0x466BC0.
**	- Users: StarmapPipSupportClass (starmap_pip_support.h, 0x7C3AC0) and nav_list_line_draw.cpp's line (0x7B99C0).
**	- PLACEMENT: provisional (D137): file name and directory unknown (app/shared beside its land neighbours).
*/
