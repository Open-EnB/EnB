// Earth & Beyond client (reconstruction): app\shared\ro_funcs.cpp (__FILE__ 0xB724F0). Notes at the end of the file.
#include "rendobj.h"
#include "mesh.h"
#include "meshmdl.h"

// 0x497240
void Unknown_497240(RenderObjClass *robj, bool on)	// NAME 1 [Unknown_497240, robj, on]: neutral (D10)
{
	if (robj == NULL) {
		return;
	}
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		((MeshClass *)robj)->Peek_Model()->Set_Flag(MeshGeometryClass::ALLOW_NPATCHES, on);
		return;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		Unknown_497240(sub, on);
		if (sub) {
			sub->Release_Ref();
		}
	}
}

/*
**	Reconstruction notes (MSG2, 2026-11-02)
**
**	client.exe: ro_funcs.cpp's window is 0x495F60-0x497330 (appfile.py show: asserts on lines 55-411, the
**	model helpers 0x496110 / 0x496240 / ... the pick 0x496960). Rebuilt so far: only 0x497240, the
**	ALLOW_NPATCHES flag set or cleared on every mesh's model through the sub-objects (the message box's
**	windows clear it). It follows the file's last assert function 0x496E90 (line 411) and precedes the
**	file's COMDATs (0x497300, ShaderClass's constructor 0x497330), so it is ro_funcs.cpp's own function.
**	The rest of the file stays to be rebuilt; its asserts' lines (55-411) leave room above for them, so
**	this function moves down when they come (it has no assert of its own).
*/
