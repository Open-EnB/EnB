// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\projscen.h (name assumed from projscen.cpp's __FILE__)
**
**	A SimpleSceneClass that gives its render-state tracks (RsTrackClass, Class_ID 0x10001) projectors:
**	a ProjectorManagerClass and the static and dynamic culling systems of the projector receivers.
**	The main view's scene derives from it (0x75ECF0 calls the constructor and the three Create_ methods).
**
**	THE CLIENT'S LAYOUT (0xA8 bytes), from the constructor 0x494EE0 and the destructor 0x494F50; vtable 0xAEA790.
*/

#ifndef PROJSCEN_H
#define PROJSCEN_H

#include "scene.h"
#include "proj_man.h"

class CameraClass;
class RenderObjClass;

class ProjSceneClass : public SimpleSceneClass	// NAME 3: class-from-file (D24) for app\shared\projscen.cpp
{
public:
	ProjSceneClass(void);														// 0x494EE0
	virtual ~ProjSceneClass(void);											// 0x494F50 (deleting 0x494F20)

	virtual int		Get_Scene_ID(void) const	{ return 0x10002; }		// +0x08 0x495F60
	virtual void	Add_Render_Object(RenderObjClass * obj);				// +0x0C 0x495070
	virtual void	Remove_Render_Object(RenderObjClass * obj);			// +0x10 0x495160
	virtual void	Remove_All_Render_Objects(void);							// +0x60 0x495230
	virtual void	Apply_Projectors(CameraClass * camera);				// +0x68 0x495990  // NAME 2: what it calls

	void				Create_Projector_Manager(void);							// 0x495280 (-NOPROJECTORS: none)  // NAME 2: what it does
	void				Delete_Projector_Manager(void);							// 0x4953E0 (no caller)  // NAME 2: what it does
	void				Create_Projectors(RenderObjClass * obj);				// 0x495430  // NAME 2: what it does
	void				Create_Static_Culling_System(void);						// 0x495560  // NAME 2: what it does
	void				Create_Dynamic_Culling_System(const Vector3 & min,const Vector3 & max,float objdim);	// 0x495680 (ret 0xC, the arguments unused; the caller 0x75EDC8 passes a box and 10000)  // NAME 2: what it does [min, max, objdim]: GridCullSystemClass::Re_Partition's
	void				Add_Static_Receiver(RenderObjClass * obj);				// 0x4957A0  // NAME 2: what it does
	void				Add_Dynamic_Receiver(RenderObjClass * obj);			// 0x495870  // NAME 2: what it does
	void				Delete_Static_Culling_System(void);						// 0x495930 (no caller)  // NAME 2: what it does
	void				Delete_Dynamic_Culling_System(void);					// 0x495960 (no caller)  // NAME 2: what it does

	ProjectorManagerClass *				ProjectorManager;					// +0x98  // NAME 2: what it holds
	ProjectionDefClass *					ProjectionDef;						// +0x9C  // NAME 2: as ProjectorCasterClass's
	ReceiverAABTreeCullSystemClass *	StaticCullingSystem;				// +0xA0 (the assert's)
	ReceiverGridCullSystemClass *		DynamicCullingSystem;			// +0xA4 (the assert's)
};

#endif //PROJSCEN_H
