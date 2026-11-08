// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\enbscene.h (name assumed from enbscene.cpp's __FILE__)
**
**	The main view's scene (mainview.cpp's +0x10C): a ProjSceneClass that also sorts what is added to it into
**	the 'plt' models (their day / night), the 'clouds' emitters (their colour) and the land and the meshes
**	(+0xDC; the land also in Land, the assert's name).
**
**	THE CLIENT'S LAYOUT (0xF4 bytes), from the constructor 0x75ECF0 and the destructor 0x75EED0; vtable 0xB03D3C.
*/

#ifndef ENBSCENE_H
#define ENBSCENE_H

#include "..\shared\projscen.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\multilist.h"

class LandClass;
class CameraClass;
class OBBoxClass;
class Vector3;
struct CastResultStruct;

class MainViewSceneClass : public ProjSceneClass	// NAME 1: neutral (D10): mainview.cpp's scene
{
public:
	MainViewSceneClass(void);												// 0x75ECF0
	virtual ~MainViewSceneClass(void);										// 0x75EED0 (deleting 0x75EE70)

	virtual void	Add_Render_Object(RenderObjClass * obj);				// +0x0C 0x75F180
	virtual void	Remove_Render_Object(RenderObjClass * obj);			// +0x10 0x75F440
	virtual void	Remove_All_Render_Objects(void);							// +0x60 0x75F560
	virtual void	Apply_Projectors(CameraClass * camera);				// +0x68 0x75F8B0 (ProjSceneClass's, then the land's projectors)

	void Unknown_75F690(const Vector3 &sun, const Vector3 &camera);		// 0x75F690 (mainview.cpp's Update, space mode)  // NAME 1 [sun, camera]: what Update passes
	void Unknown_75F700(const Vector3 &color);								// 0x75F700 (mainview.cpp's Update, planet mode)  // NAME 1 [color]: what Update passes
	void Unknown_75FB00(const OBBoxClass &to, const OBBoxClass &from, const Vector3 &move, CastResultStruct *result, RenderObjClass *ignore, bool ship);	// 0x75FB00 (camera_manager.cpp's 0x7998B0 and camctrl_takeoff.cpp's 0x796920: casts the box through the +0xDC objects)  // NAME 1 [to, from, move, result, ignore, ship]: what 0x7998B0 passes

	RefMultiListClass<RenderObjClass>	UnknownA8;		// +0xA8 the 'plt' models  // NAME 1: neutral (D10)
	RefMultiListClass<RenderObjClass>	UnknownC0;		// +0xC0 the 'clouds'  // NAME 1: neutral (D10)
	RefCountPtr<LandClass>				Land;				// +0xD8 (the assert's)
	RefMultiListClass<RenderObjClass>	UnknownDC;		// +0xDC the land, meshes, HLODs and tracks over them  // NAME 1: neutral (D10)
};

#endif //ENBSCENE_H
