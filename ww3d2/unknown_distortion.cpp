/*
**	Earth & Beyond client (reconstruction)
**
**	E&B's screen-distortion effect (decision D19); see unknown_distortion.h. Names are neutral (D10).
*/

#include "unknown_distortion.h"
#include "vector.h"
#include "dx8wrapper.h"
#include "dx8caps.h"
#include "ww3d.h"
#include "camera.h"
#include "vector2.h"
#include "sphere.h"
#include "aabox.h"
#include "wwmath.h"
#include <math.h>


/*
** The objects to draw this frame (0xC0BAA8), and whether the hardware can draw them (0xC0BAD8)
*/
static DynamicVectorClass<UnknownDistortionClass *>	DistortionList;
static bool														DistortionEnabled;


// Called when a render device is set (0x995DC0): the back buffer must fit in a texture
void Unknown_Reset_Distortion_List(void)
{
	DistortionList.Reset_Active();
	DistortionEnabled = false;

	const DX8Caps * caps = DX8Wrapper::Get_Current_Caps();
	if (caps->Support_Cubemaps()) {
		int width, height, bits;
		bool windowed;
		WW3D::Get_Device_Resolution(width, height, bits, windowed);
		if (width <= 1024 && (int)caps->Get_DX8_Caps().MaxTextureWidth >= width &&
			(int)caps->Get_DX8_Caps().MaxTextureWidth >= height) {
			DistortionEnabled = true;
		}
	}
}

// 0x995E20
void Unknown_Add_To_Distortion_List(UnknownDistortionClass * obj)
{
	DistortionList.Add(obj);
}

// Draws this frame's objects and empties the list (0x995E80)
void Unknown_Render_Distortion_List(CameraClass & camera)
{
	if (DistortionEnabled) {
		for (int i = 0; i < DistortionList.Count(); i++) {
			DistortionList[i]->Unknown_Render_Distortion(camera);
		}
	}
	DistortionList.Reset_Active();
}


/*
** UnknownDistortionClass
*/
UnknownDistortionClass::UnknownDistortionClass(void) :
	m_Unknown90(1.0f)
{
}

UnknownDistortionClass::UnknownDistortionClass(const UnknownDistortionClass & src) :
	RenderObjClass(src)
{
	m_Unknown90 = src.m_Unknown90;
	m_Unknown8C = src.m_Unknown8C;
}

UnknownDistortionClass::~UnknownDistortionClass(void)
{
	m_Unknown8C = NULL;
}

RenderObjClass * UnknownDistortionClass::Clone(void) const
{
	return NEW_REF(UnknownDistortionClass, (*this));
}

void UnknownDistortionClass::Render(RenderInfoClass & rinfo)
{
	Unknown_Add_To_Distortion_List(this);
}

void UnknownDistortionClass::Unknown_Render_Distortion(CameraClass & camera)
{
	// not rebuilt yet (0x9961E0)
}

void UnknownDistortionClass::Get_Obj_Space_Bounding_Sphere(SphereClass & sphere) const
{
	sphere.Center.Set(0, 0, 0);
	sphere.Radius = m_Unknown90;
}

void UnknownDistortionClass::Get_Obj_Space_Bounding_Box(AABoxClass & box) const
{
	box.Center.Set(0, 0, 0);
	box.Extent.Set(m_Unknown90, m_Unknown90, m_Unknown90);
}


/*
** UnknownRippleClass
*/
UnknownRippleClass::UnknownRippleClass(void) :
	m_Unknown08(20.0f),
	m_Unknown0C(0.01f),
	m_Unknown10(0.01f)
{
}

void UnknownRippleClass::Unknown_Distort(Vector2 & uv, UnknownDistortionClass * owner)
{
	float dy = uv.Y - 0.5f;
	float dx = uv.X - 0.5f;
	float r2 = dx * dx + dy * dy;
	float angle = (float)WW3D::Get_Sync_Time() * m_Unknown0C + r2;
	uv.X += (float)exp(-r2 * m_Unknown08) * WWMath::Sin(angle) * m_Unknown10;
	uv.Y += WWMath::Sin(angle) * m_Unknown10 * (float)exp(-r2 * m_Unknown08);
}
