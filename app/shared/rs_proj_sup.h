// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\shared\rs_proj_sup.h (PROVISIONAL, D137: the header of rs_proj_sup.cpp)
**
**	The render-state tracks' projector caster and receiver (rs_proj_sup.cpp, 0x45D9E0-0x45E0B6), moved out of
**	rs_track.h; their bases are proj_man.h's ProjectorCasterClass / ProjectorReceiverClass.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RS_PROJ_SUP_H  // NAME 1 [rs_proj_sup.h]: provisional header name for rs_proj_sup.cpp
#define RS_PROJ_SUP_H

#include "proj_man.h"

class RsTrackClass;
class CullableClass;


// rs_proj_sup.cpp's projector caster (0x45D9E0, vtable 0xAE80B0) and receiver (0x45DC80, vtable 0xAE80EC) RsTrackClass
// makes (0x462470, 0x462530)
class Unknown45D9E0Class : public ProjectorCasterClass	// NAME 1: neutral (D10)
{
public:
	Unknown45D9E0Class(RsTrackClass *owner, ProjectionDefClass *def, ProjectorManagerClass *manager);	// 0x45D9E0 (0x24 bytes)  // NAME 1 [owner]: neutral; NAME 2 [def, manager]: the base's

	virtual RenderObjClass *Peek_Render_Obj(void);					// 0x45DBB0
	virtual Matrix3D Get_Transform(void);							// 0x45DBE0
	virtual uint32 Get_Type_ID(void);								// 0x45DC60

	RsTrackClass *Owner;											// +0x20  // NAME 2: what it points at
};
class Unknown45DC80Class : public ProjectorReceiverClass	// NAME 1: neutral (D10)
{
public:
	Unknown45DC80Class(RsTrackClass *owner, CullableClass *cull, int a);	// 0x45DC80 (0x34 bytes)  // NAME 1 [owner, cull, a]: neutral

	virtual ~Unknown45DC80Class();									// 0x45DDF0 (deleting 0x45DD90)
	virtual void Add_Projector(TexProjectClass *texProject);		// 0x45DF20 (the assert's texProject)
	virtual bool Intersects(const OBBoxClass &box);					// 0x45DF90
	virtual void *Get_Projection_Object_ID(void);					// 0x45E0B0

	void Unknown_45DFC0(RenderInfoClass &rinfo);					// 0x45DFC0: pushes the projectors' material passes  // NAME 1 [rinfo]: the Render argument
	void Unknown_45E030(RenderInfoClass &rinfo);					// 0x45E030: releases them, popping the passes  // NAME 1 [rinfo]: the Render argument

	TexProjListClass ProjectorList;									// +0x18  // NAME 2: what it holds
	RsTrackClass *Owner;											// +0x30  // NAME 2: what it points at
};

#endif
