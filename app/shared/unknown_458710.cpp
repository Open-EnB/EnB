// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (D10, interim placement D137): the object after rs.cpp's, client
**	0x458710-0x458BFA, has no __FILE__ and no assert; its calls into rs.cpp (0x457E10, 0x457E90) are not
**	inlined, so it is a separate object, linked between rs.cpp and the next rs_*.cpp.
**
**	A render-object state whose model goes onto the owner's bone path while it is active
**	(Unknown458710Class over rs_robj.cpp's Unknown45E850Class; both declared in rs_track.h for now).
**	Notes at the end.
*/



#include <string.h>
#include "rs_track.h"
#include "..\..\wwlib\nstrdup.h"

// 0x458710 (deleting destructor 0x4587E0)
Unknown458710Class::Unknown458710Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *name, int next_id, float duration, const char *bone_path, int arg) :
	Unknown45E850Class(list, id, link_id, flags, name, next_id, duration),
	Unknown48(arg)
{
	if (bone_path != NULL && *bone_path != 0) {
		Unknown44 = nstrdup(bone_path);
	} else {
		Unknown44 = NULL;
	}
}

// 0x458810
Unknown458710Class::Unknown458710Class(RenderStateListClass *list, const Unknown458710Class &source) :
	Unknown45E850Class(list, source)
{
	if (source.Unknown44 != NULL && *source.Unknown44 != 0) {
		Unknown44 = nstrdup(source.Unknown44);
	} else {
		Unknown44 = NULL;
	}
	Unknown48 = source.Unknown48;
}

// 0x4588A0
Unknown458710Class::Unknown458710Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, RenderObjClass *robj, int next_id, float duration, const char *bone_path, int arg) :
	Unknown45E850Class(list, id, link_id, flags, robj, next_id, duration),
	Unknown48(arg)
{
	if (bone_path != NULL && *bone_path != 0) {
		Unknown44 = nstrdup(bone_path);
	} else {
		Unknown44 = NULL;
	}
}

// 0x458970: the copy into another list
RenderStateClass *Unknown458710Class::Unknown_04(RenderStateListClass *list)
{
	return(new Unknown458710Class(list, *this));
}

// 0x458A60
Unknown458710Class::~Unknown458710Class()
{
	if (Unknown44 != NULL) {
		delete Unknown44;
	}
}

// 0x458AE0: the bone path (the old copy is not freed: the client's code)
void Unknown458710Class::Unknown_458AE0(const char *bone_path)
{
	if (bone_path != NULL && *bone_path != 0) {
		Unknown44 = nstrdup(bone_path);
	} else {
		Unknown44 = NULL;
	}
}

// 0x458B20: activation: the model onto the owner's bone path
void Unknown458710Class::Unknown_14(RsTrackClass *owner)
{
	Unknown_457E10();
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		robj->Set_Hidden(false);
	}
	owner->Unknown_1D0((int)this, Unknown44, robj, Unknown48);	// the state as Unknown_1D0's first argument (declared int there)
}

// 0x458B70: deactivation: hidden, off the bone path
void Unknown458710Class::Unknown_18(RsTrackClass *owner)
{
	Unknown_457E90();
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		robj->Set_Hidden(true);
		owner->Unknown_461D60(robj, Unknown44);
	}
}

// 0x458BC0
void Unknown458710Class::Unknown_1C(RsTrackClass *owner)
{
	Unknown_457E10();
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		robj->Set_Hidden(false);
	}
	owner->Unknown_1D0((int)this, Unknown44, robj, Unknown48);	// the state as Unknown_1D0's first argument (declared int there)
}
/*
**	Reconstruction notes
**
**	client.exe: 0x458710-0x458BFA, all exact (appfile.py match 9 of 9 markers, and the deleting
**	destructor 0x4587E0): the two constructors 0x458710 / 0x4588A0 (byte-identical but for the base
**	constructor they call: by model name 0x45E5F0, by render object 0x45E880), the copy constructor
**	0x458810 and the clone 0x458970 (vtable +0x04, new 0x4C bytes), the destructor 0x458A60, 0x458AE0, and
**	the vtable 0xAE7D58's +0x14 / +0x18 / +0x1C (0x458B20 / 0x458B70 / 0x458BC0): activation shows the
**	model (Set_Hidden(false)) and hands it to RsTrackClass::Unknown_1D0 with the bone path +0x44 and
**	+0x48; deactivation hides it and takes it off the path (RsTrackClass::Unknown_461D60).
*/
