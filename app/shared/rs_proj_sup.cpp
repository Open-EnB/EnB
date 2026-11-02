// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\rs_proj_sup.cpp (its assert, line 58); client
**	0x45D9E0-0x45E0B6 and the inline copies after it. Notes at the end.
**
**	The projector support of the render-state tracks (RsTrackClass): a projector caster that casts
**	its track's current model as a shadow, and a projector receiver the projectors land on, which
**	pushes their material passes around the track's render. Both are declared in rs_track.h for now
**	(interim, D137); their bases are proj_man.h's ProjectorCasterClass / ProjectorReceiverClass.
*/
#include "rs_track.h"
#include "rinfo.h"
#include "colmath.h"
#include "..\shared\unknown_debug.h"

// @0x45D9E0
Unknown45D9E0Class::Unknown45D9E0Class(RsTrackClass *owner, int def, int manager) :
	ProjectorCasterClass((owner->Unknown8C & 0x80) != 0, (ProjectionDefClass *)def, (ProjectorManagerClass *)manager),
	Owner(owner)
{
	Create_Projection();
}

// @0x45DBB0
RenderObjClass *Unknown45D9E0Class::Peek_Render_Obj(void)
{
	return(Owner->Peek_Render_Object());
}

// @0x45DBE0
Matrix3D Unknown45D9E0Class::Get_Transform(void)
{
	return(Owner->Get_Transform());
}

// @0x45DC60
uint32 Unknown45D9E0Class::Get_Type_ID(void)
{
	return((uint32)Owner);
}

// @0x45DC80
Unknown45DC80Class::Unknown45DC80Class(RsTrackClass *owner, CullableClass *cull, int a) :
	ProjectorReceiverClass(cull, a),
	Owner(owner)
{
}

// @0x45DDF0
Unknown45DC80Class::~Unknown45DC80Class()
{
}

// @0x45DF20
void Unknown45DC80Class::Add_Projector(TexProjectClass *texProject)
{
	G_ASSERT(texProject != NULL);
	ProjectorList.Add(texProject);
}

// @0x45DF90
bool Unknown45DC80Class::Intersects(const OBBoxClass &box)
{
	return(CollisionMath::Intersection_Test(Owner->Get_Bounding_Box(), box));
}

// @0x45DFC0
void Unknown45DC80Class::Unknown_45DFC0(RenderInfoClass &rinfo)
{
	if (!ProjectorList.Is_Empty()) {
		TexProjListIterator it(&ProjectorList);
		for (it.First(); !it.Is_Done(); it.Next()) {
			rinfo.Push_Material_Pass(it.Peek_Obj()->Peek_Material_Pass());
		}
	}
}

// @0x45E030
void Unknown45DC80Class::Unknown_45E030(RenderInfoClass &rinfo)
{
	while (!ProjectorList.Is_Empty()) {
		ProjectorList.Release_Head();
		rinfo.Pop_Material_Pass();
	}
}

// @0x45E0B0
void *Unknown45DC80Class::Get_Projection_Object_ID(void)
{
	return(Owner);
}
/*
**	Reconstruction notes
**
**	client.exe 0x45D9E0-0x45E0B6, appfile.py match 11 of 11 exact, with the compiler's copies after it:
**	the deleting destructors 0x45DAB0 / 0x45DD90, the caster's implicit destructor 0x45DAE0 (the same
**	bytes as ProjectorCasterClass's 0x45E200), RefMultiListClass<TexProjectClass>'s 0x45DDC0 / 0x45E0D0,
**	the bases' destructors 0x45E200 / 0x45E300 (deleting 0x45E2D0 / 0x45E3A0) and the MultiListObjectClass
**	adjustor thunks 0x45E190 / 0x45E1A0 / 0x45E3D0 / 0x45E3E0.
**	The bases' inline constructors live in proj_man.h; the receiver's asserts there at line 207 (the client's
**	push 0xCF). The caster's base constructor releases ProjectionDef before taking def: the client's
**	optimizer drops that release (the member is NULL) but keeps the unwind states it adds (state 2 before
**	Create_Projection). The receiver's destructor is declared (the client stores its vtables first).
**	UseBlob is bit 7 of RsTrackClass::Unknown8C ((x & 0x80) != 0 gives the client's shr al, 7).
*/
