/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\stars.h (name assumed from stars.cpp; no assert
**	names it)
**
**	The star field: a render object holding layers of point sprites read from stars.ini, drawn
**	around the camera at radius 100 (the points are transformed by the view rotation only). The
**	client names neither class (D24); StarFieldClass / StarLayerClass and the method names are
**	descriptive (D10), the ini keys are the client's.
**
**	StarLayerClass, 0x20 bytes (client 0x481A20 ctor, 0x481A50 dtor):
**	  +0x00 PointGroup, +0x04 Points (world, unit sphere x 100), +0x08 TransformedPoints (camera
**	  space, rebuilt each frame), +0x0C Sizes, +0x10 Orientations, +0x14 Frames (RefCountPtr, E&B's
**	  ref_ptr.h), +0x18 NumStars, +0x1C Quads.
**	StarFieldClass : RenderObjClass, 0xA4 bytes (vtable 0xAEA258; ctor 0x4826E0):
**	  +0x8C Layers (std::vector: allocator, first +0x90, last +0x94, end +0x98), +0x9C TotalPolys,
**	  +0xA0 Budget. Overrides: Clone (slot 2, 0x483740: NULL), Get_Num_Polys (slot 8, 0x482E00),
**	  Render (slot 9, 0x482D80), Get_Cost (slot 69, 0x482E20).
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 3 [stars.h]: header of the client's app\shared\stars.cpp (assert __FILE__)
#ifndef __STARS_H
#define __STARS_H

#include <vector>
#include "rendobj.h"
#include "ref_ptr.h"
#include "sharebuf.h"
#include "vector3.h"

class INIClass;
class PointGroupClass;
class RenderInfoClass;

// NAME 2: one stars.ini layer (Texture, NumStars, ...); the client names no class
class StarLayerClass
{
public:
	StarLayerClass(void);
	~StarLayerClass(void);

// NAME 2: draws the first max_polys polygons (0x481B70)
	void		Render(RenderInfoClass &rinfo, unsigned int max_polys);
// NAME 2: reads one ini section (0x481DB0, assert 'section')
	bool		Load(const INIClass &ini, const char *section);
// NAME 2: polygons of all its stars (inlined in 0x4829E0 / 0x482D80)
	int		Get_Num_Polys(void) const		{ return Quads ? NumStars << 1 : NumStars; }

// NAME 2: members after their WW3D types and the ini keys
	PointGroupClass *									PointGroup;
	RefCountPtr<ShareBufferClass<Vector3> >			Points;
	RefCountPtr<ShareBufferClass<Vector3> >			TransformedPoints;
	RefCountPtr<ShareBufferClass<float> >				Sizes;
	RefCountPtr<ShareBufferClass<unsigned char> >	Orientations;
	RefCountPtr<ShareBufferClass<unsigned char> >	Frames;
	int													NumStars;
	bool													Quads;
};

// NAME 2: the RenderObjClass holding the layers (vtable 0xAEA258); the client names no class
class StarFieldClass : public RenderObjClass
{
public:
	StarFieldClass(unsigned int budget);
	virtual ~StarFieldClass(void);

	virtual RenderObjClass *	Clone(void) const				{ return NULL; }
	virtual int					Get_Num_Polys(void) const;
	virtual void				Render(RenderInfoClass &rinfo);
	virtual float				Get_Cost(void) const;

// NAME 2: loads stars.ini for a system number (0x4829E0)
	void							Init(int system);
// NAME 2: deletes the layers (0x482920)
	void							Free_Layers(void);
// NAME 2: the polygon budget at +0xA0
	void							Set_Budget(unsigned int budget)	{ Budget = budget; }
	unsigned int				Get_Budget(void) const				{ return Budget; }

protected:
// NAME 2: Layers (+0x8C), TotalPolys (+0x9C), Budget (+0xA0)
	std::vector<StarLayerClass *>	Layers;
	int									TotalPolys;
	unsigned int						Budget;
};

#endif //__STARS_H
