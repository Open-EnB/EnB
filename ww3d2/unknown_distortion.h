// Earth & Beyond client (reconstruction)
/*
**	E&B's screen-distortion effect (decision D19). The client keeps no name for this module or its
**	classes; every name here is neutral (D10) until E&B's own turn up. Client range 0x995D20-0x998200,
**	between dx8texman.cpp and boxrobj.cpp.
**
**	Distortion render objects add themselves to a per-frame list when rendered; once a frame the list
**	is drawn: each object copies the back buffer to a texture and draws it back over the object's
**	projected screen area through a grid whose texture coordinates a distortion function (a damped
**	ripple) displaces. The list is enabled when a render device is set, if the hardware can do it.
*/


#ifndef UNKNOWN_DISTORTION_H
#define UNKNOWN_DISTORTION_H

#include "always.h"
#include "rendobj.h"
#include "refcount.h"
#include "ref_ptr.h"

class CameraClass;
class Vector2;
class UnknownDistortionClass;


/*
** A distortion function: displaces a texture coordinate (0..1 over the distorted area).
*/
class UnknownRippleClass : public RefCountClass
{
public:
	UnknownRippleClass(void);

	virtual void			Unknown_Distort(Vector2 & uv, UnknownDistortionClass * owner);

protected:
	float						Unknown08;	// decay with the squared distance from the centre (20)
	float						Unknown0C;	// phase speed per millisecond (0.01)
	float						Unknown10;	// amplitude (0.01)
};


/*
** The distortion render object: a sphere of radius Unknown90 around its position.
*/
class UnknownDistortionClass : public RenderObjClass
{
public:
	UnknownDistortionClass(void);
	UnknownDistortionClass(const UnknownDistortionClass & src);
	virtual ~UnknownDistortionClass(void);

	virtual RenderObjClass *	Clone(void) const;
	virtual void					Render(RenderInfoClass & rinfo);
	virtual void		 			Get_Obj_Space_Bounding_Sphere(SphereClass & sphere) const;
	virtual void					Get_Obj_Space_Bounding_Box(AABoxClass & box) const;

	// Draws the distortion (0x9961E0; called once a frame through the list)
	virtual void					Unknown_Render_Distortion(CameraClass & camera);

protected:
	RefCountPtr<UnknownRippleClass>	Unknown8C;	// the distortion function: a smart pointer (the destructor
															// 0x996040 releases it twice: its own NULL assignment, then ~RefCountPtr)
	float								Unknown90;	// radius (1 by default)
};


// The per-frame list (0x995DC0, 0x995E20, 0x995E80)
void Unknown_Reset_Distortion_List(void);
void Unknown_Add_To_Distortion_List(UnknownDistortionClass * obj);
void Unknown_Render_Distortion_List(CameraClass & camera);

#endif // UNKNOWN_DISTORTION_H
