// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ro_offset.cpp (inferred, notes at the end); client
**	0x47F560-0x47FA56.
*/
#include "ro_offset.h"
#include "ww3d.h"

// 0x47F560
OffsetRenderObjClass::OffsetRenderObjClass(void) :
	ProxyRObj(false),
	Offset(0.0f, 0.0f, 0.0f)
{
}

// 0x47F5E0
OffsetRenderObjClass::~OffsetRenderObjClass(void)
{
}

// 0x47F610
void OffsetRenderObjClass::Set_Transform(const Matrix3D &m)
{
	RenderObjClass::Set_Transform(m);
	if (Proxied != NULL) {
		Matrix3D tm(Transform);
		tm.Translate(Offset);
		Proxied->Set_Transform(tm);
	}
}

// 0x47F750
void OffsetRenderObjClass::Set_Position(const Vector3 &v)
{
	RenderObjClass::Set_Position(v);
	if (Proxied != NULL) {
		Vector3 pos(v);
		pos += Offset;
		Proxied->Set_Position(pos);
	}
}

// 0x47F7E0
void OffsetRenderObjClass::Render(RenderInfoClass &rinfo)
{
	if (Proxied == NULL || !Is_Not_Hidden_At_All()) {
		return;
	}
	if (WW3D::Are_Static_Sort_Lists_Enabled() && SortLevel != 0) {
		WW3D::Add_To_Static_Sort_List(this, SortLevel);
	} else {
		Proxied->Render(rinfo);
	}
}

// 0x47F860
void OffsetRenderObjClass::Set_Offset(const Vector3 &offset)
{
	Offset = offset;
	if (Proxied != NULL) {
		Vector3 pos(offset);
		pos += Transform.Get_Translation();
		Proxied->Set_Position(pos);
	}
}

/*
**	Notes
**
**	Placement (interim, D137): the client names only the header, ro_offset.h (__FILE__ 0xB71AA0, lines 49, 50, 54);
**	no assert sits in the out-of-line code, so this file's name is inferred from ro_proxy.cpp / ro_proxy.h beside it.
**	That the code is its own object, and in app\shared:
**	- 0xB71AA0 is absolute, where headers included from other directories read '..\shared/...' (workflow.md 2.1):
**	  the translation unit that included the header sits in the header's own directory.
**	- 0x47F560-0x47F8DD are out-of-line bodies (all of this class), then the adjustors 0x47F900 / 0x47F910 and the
**	  inlines to 0x47FA56; ProxyRObj's object repeats that shape from 0x47FA70 to its adjustors 0x480AD0 / 0x480AE0
**	  and Class_ID 0x480AF0. Out-of-line bodies after a run of inlines start a new object, so this is neither one
**	  object with ro_proxy.cpp nor part of the land.cpp before it (triage's low hint: its inlines end at 0x47F520).
**	- Get_Sort_Level / Set_Sort_Level (0x47F920 / 0x47F940) are ro_proxy.h's inlines, kept here as this object's copies.
**
**	Nothing in the client calls the constructor or Set_Offset (0 callers through their thunks 0x410F69, 0x408481).
**	Transform is read directly, without the Validate_Transform of Get_Transform (0x47F610, 0x47F860); Set_Offset's
**	copy-then-add is the only form tried that was exact (Get_Translation(&pos), operator+ were not).
*/
