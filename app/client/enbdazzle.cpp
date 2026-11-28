// Earth & Beyond client (reconstruction): app\client\enbdazzle.cpp (the client's __FILE__), 0x75DD00-0x75E6D0 (notes at the end)
#include "enbdazzle.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include "sclient.h"
#include "..\shared\orbitalmanager.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\sampler.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\coltest.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\wwmath\wwmath.h"
#include "..\shared\auth_product.h"
static Vector3 *_Points = NULL;		// 0xBE82C0: 16 sample directions on the unit sphere  // NAME 1: neutral (D10)
static int _Instances = 0;			// 0xBE82C4  // NAME 3: the FAIL_IF's

struct DazzleOccluderStruct			// NAME 1: neutral (D10): what 0x75DEE0 collects from the scene (8 bytes)
{
	RenderObjClass *Obj;			// +0x00  // NAME 1: neutral
	float Dist2;					// +0x04 (the squared distance from the camera; 0 for the land)  // NAME 1: neutral
};





















// @0x75DD00
MainViewDazzleHandlerClass::MainViewDazzleHandlerClass(SpaceClientClass *client) : Client(client)
{
	if (_Points == NULL) {
		FAIL_IF(_Instances) {
			_Instances = 0;
		}
		_Points = new Vector3[16];
		QMCSamplingClass sampler(2);
		for (int i = 0; i < 16; i++) {
			float s[2];
			sampler.Sample(s);
			float a = s[0] * (2.0f * WWMATH_PI);
			float r = WWMath::Sqrt((1.0f - s[1]) * s[1]);
			float r2 = r + r;
			float sn = WWMath::Sin(a);
			float cs = WWMath::Cos(a);
			_Points[i].Set(cs * r2, sn * r2, 1.0f - (s[1] + s[1]));


		}
	}
	_Instances++;
}

// @0x75DE70
MainViewDazzleHandlerClass::~MainViewDazzleHandlerClass()
{
	_Instances--;
	if (_Instances == 0) {
		if (_Points != NULL) {
			delete [] _Points;
		}
		_Points = NULL;
	}
}

// @0x75DEB0
bool operator < (const DazzleOccluderStruct &a, const DazzleOccluderStruct &b)
{
	return(a.Dist2 < b.Dist2);
}

// NOT EXACT: all 517 instructions of 0x75DEE0..0x75E510 match (res 539->422, raw 621->424; 422 is 0x75E520..0x75E6D0 int3/atexit tail); only dist2*r2 operand order at 0x75E19E (raw 2) differs
// @0x75DEE0
float MainViewDazzleHandlerClass::Compute_Dazzle_Visibility(RenderInfoClass &rinfo, DazzleRenderObjClass *dazzle, const Vector3 &point) const
{
	MainViewClass *view = Client->MainView;
	OrbitalManager *orbitals = view->Orbitals;
	RenderObjClass *top = dazzle;
	while (top->Get_Container() != NULL) {
		top = top->Get_Container();
	}
	float visibility = orbitals->Unknown_4F5090(top);
	if (visibility == 0.0f) {
		return(visibility * 1.0f);
	}
	float radius = dazzle->Get_Bounding_Sphere().Radius;
	Vector3 campos = rinfo.Camera.Get_Position();
	Vector3 dir = point - campos;
	float len2 = dir.Length2();
	if (len2 == 0.0f) {
		return(0.0f);
	}
	float sin2 = (radius * radius) / len2;
	float sina = WWMath::Sqrt(sin2);
	float cos2 = 1.0f - sin2;
	float cosa = WWMath::Sqrt(cos2);
	dir.Normalize();

	SceneClass *scene = view->Unknown_64();
	SceneIterator *it = scene->Create_Iterator();
	static std::vector<DazzleOccluderStruct> occluders;
	occluders.clear();
	for (it->First(); !it->Is_Done(); it->Next()) {
		RenderObjClass *obj = it->Current_Item();
		float dist2;
		if (obj->Class_ID() != RenderObjClass::CLASSID_LAND) {
			if (!obj->Is_Visible()) {
				continue;
			}
			const SphereClass &sphere = obj->Get_Bounding_Sphere();
			Vector3 d = sphere.Center - campos;
			float r2 = sphere.Radius * sphere.Radius;
			dist2 = d.Length2();
			if (dist2 > r2) {
				float proj = Vector3::Dot_Product(d, dir);
				if (proj * proj < dist2 * sin2 || proj <= 0.0f) {
					float perp = WWMath::Sqrt(WWMath::Fabs(dist2 - proj * proj));
					float f = sina * proj + perp * cosa;
					if (f * f - r2 * dist2 < 0.0f) {
						continue;
					}
					if (f < 0.0f) {
						continue;
					}
				}
			}
		} else {
			dist2 = 0.0f;
		}
		DazzleOccluderStruct occ;
		occ.Obj = obj;
		occ.Dist2 = dist2;
		occluders.push_back(occ);
	}
	scene->Destroy_Iterator(it);

	if (0) { scene->Destroy_Iterator(NULL); }	// D272 budget device: raises inline budget so std::sort inlines _Unguarded_insert as the client
	if (occluders.size() > 0) {
		std::sort(occluders.begin(), occluders.end());
		int hidden = 0;
		for (int i = 0; i < 16; i++) {
			Vector3 p = point + _Points[i] * radius;
			Vector3 screen;
			if (rinfo.Camera.Project(screen, p) != CameraClass::INSIDE_FRUSTUM) {
				hidden++;
				continue;
			}
			CastResultStruct result;
			LineSegClass ray(campos, p);
			RayCollisionTestClass test(ray, &result, COLLISION_TYPE_3);
			for (std::vector<DazzleOccluderStruct>::iterator o = occluders.begin(); o != occluders.end(); o++) {
				o->Obj->Cast_Ray(test);
				if (result.Fraction < 1.0f) {
					hidden++;
					break;
				}
			}
		}
		if (hidden != 0) {
			return((1.0f - hidden * 0.0625f) * visibility);
		}
	}
	return(visibility);
}


/*
**	RECONSTRUCTION NOTES
**
**	0x75DD00 the constructor (FAIL_IF '_Instances' l.50: the first instance makes the 16 sample directions with a
**	2-D QMCSamplingClass, a uniform sphere: z = 1 - 2v, r = 2 sqrt(v (1 - v))); 0x75DE70 the destructor (the last
**	one frees them); 0x75DEB0 the occluders' operator< (by distance, out of line; std::sort inlines it);
**	0x75DEE0 Compute_Dazzle_Visibility: the orbital's visibility of the dazzle's top container, times 1 - n / 16 for
**	the n sample points (around the dazzle at its bounding radius) outside the frustum or behind a land or a
**	visible object of the main view's scene whose sphere may reach the cone from the camera to the dazzle; the
**	occluders are a function-static vector (guard 0xBE82A8, 0xBE82B0), sorted near to far. 0x75E6B0 its atexit
**	destructor; 0x75E6E0-0x75ECF0 the vector's and std::sort's instances.
*/
