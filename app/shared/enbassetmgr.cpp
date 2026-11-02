// Earth & Beyond client (reconstruction): app\Shared\enbassetmgr.cpp (__FILE__ in the assert at 0xB721AC). Notes at the end of the file.
#include "enbassetmgr.h"
#include "rendobj.h"
#include "proto.h"
#include "wwprofile.h"
#include "unknown_debug.h"
#include <string.h>

#include "dx8wrapper.h"
#include "dx8caps.h"

void Unknown_48E9D0(RenderObjClass *robj);	// 0x48E9D0: Post_Process' walk of a model (recursive; not rebuilt)  // NAME 1 [Unknown_48E9D0, robj]: neutral (D10)
void Unknown_48E620(RenderObjClass *robj);	// 0x48E620: Post_Process' step for the 'plt' models (not rebuilt)  // NAME 1 [Unknown_48E620, robj]: neutral (D10)

// @0x48B070
UnknownBDADC0Class::UnknownBDADC0Class()
{
}

// @0x48B0D0
UnknownBDADC0Class::~UnknownBDADC0Class()
{
}

// 'ENBAssetManager::Post_Process' (0x48EB90, its FAIL_IF(!robj) on line 635; not rebuilt): Create_Render_Obj's copy
static inline RenderObjClass *Post_Process(RenderObjClass *robj)	// NAME 3 [Post_Process]: its profile string
{
	WWPROFILE("ENBAssetManager::Post_Process");
	FAIL_IF(!robj) return(NULL);
	Unknown_48E9D0(robj);
	const char *name = robj->Get_Name();
	if (!DX8Wrapper::Get_Current_Caps()->Support_TnL() && strnicmp(name, "plt", 3) == 0) {
		Unknown_48E620(robj);
	}
	return(robj);
}

// @0x48B0F0
RenderObjClass *UnknownBDADC0Class::Create_Render_Obj(const char *name)
{
	WWPROFILE("ENBAssetManager::Create_Render_Obj()");
	RenderObjClass *robj = WW3DAssetManager::Create_Render_Obj(name);
	if (robj != NULL) {
		robj = Post_Process(robj);
	} else {
		robj = NULL;
	}
	return(robj);
}

// @0x48F2C0
void UnknownBDADC0Class::Release_Unused_Assets(void)
{
	int count = Prototypes.Count();
	while (count-- > 0) {
		PrototypeClass *proto = Prototypes[count];
		Prototypes.Delete(count);
		if (proto != NULL) {
			delete proto;
		}
	}
	memset(PrototypeHashTable, 0, sizeof(PrototypeClass *) * PROTOTYPE_HASH_TABLE_SIZE);
	HAnimManager.Free_All_Anims();
	HTreeManager.Free_All_Trees();
	WW3DAssetManager::Release_Unused_Assets();
}

/*
**	Notes
**
**	0x48B070 is the constructor WinMain (0x526DE0) calls after new (0x408C bytes). The destructor 0x48B0D0 is declared (it stores
**	the table before the base's; the compiler's would only jump); the deleting destructor 0x48B0A0 is the compiler's.
**	0x48B0F0 (slot +0x14) inlines Post_Process (0x48EB90: its FAIL_IF(!robj) on line 635 drops out after the NULL test); the
**	if / else with robj = NULL is what puts the zero in ebx and joins both paths (a return NULL or a void Post_Process
**	duplicates the epilogue). 0x48F2C0 (slot +0x10) is Free_Assets' prototype loop without its profile, then the base's
**	Release_Unused_Assets. Not written: +0x38 0x48F1A0 (it loops until 0x48ECA0 has released every texture with one
**	reference, so a stand-in there would never return) and +0x78 0x48B220 (0x590 bytes; its HSV helpers 0x48D1F0 .. 0x48E0F0,
**	0x48BCF0, 0x48BE60 and the prototype class at table 0xAEA5A0 are not rebuilt).
*/
