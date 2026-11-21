// Earth & Beyond client (reconstruction): app\client\unknown_6bb7e0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_6bb7e0.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\ww3d2\ww3d.h"

// @0x6BB7E0
bool UnknownB007A4Class::Pre_Render(RenderObjClass *robj, RenderInfoClass &rinfo)
{
	if (Passes.size() != 0) {
		rinfo.Push_Material_Pass(Passes.front().Pass.Peek());
		int flags = Passes.front().Unknown04 ? 4 : 0;
		rinfo.Push_Override_Flags((RenderInfoClass::RINFO_OVERRIDE_FLAGS)(rinfo.Current_Override_Flags() | flags));
	}
	if (Passes.size() != 0 && Passes.front().Unknown04 && Passes.front().Pass->Peek_Shader().Get_Dst_Blend_Func() != ShaderClass::DSTBLEND_ZERO && WW3D::Are_Static_Sort_Lists_Enabled()) {
		WW3D::Add_To_Static_Sort_List(robj, Passes.front().Pass->Peek_Shader().Guess_Sort_Level());
		return(false);
	}
	return(true);
}

// @0x6BB8B0
void UnknownB007A4Class::Post_Render(RenderObjClass *robj, RenderInfoClass &rinfo)
{
	if (Passes.size() != 0) {
		rinfo.Pop_Override_Flags();
		rinfo.Pop_Material_Pass();
	}
}

// 0x6BB8E0: the pass and its flag onto the list's end
void UnknownB007A4Class::Unknown_6BB8E0(const RefCountPtr<MaterialPassClass> &pass, bool flag)
{
	Passes.push_front(UnknownB007A4PassStruct(pass, flag));
}

// 0x6BBA20: the pass's first entry off the list
void UnknownB007A4Class::Unknown_6BBA20(const RefCountPtr<MaterialPassClass> &pass)
{
	for (std::list<UnknownB007A4PassStruct>::iterator it = Passes.begin(); it != Passes.end(); it++) {
		if ((*it).Pass == pass) {
			Passes.erase(it);
			return;
		}
	}
}

/*
** Reconstruction notes (WE5, 2026-11-07)
**
** The object: no __FILE__ or assert; client 0x6BB7E0-0x6BB8CB with std::list helpers after it (0x6BB8E0, 0x6BBA20)
** and font32bit.cpp from 0x6BBAC0; before it debris.cpp (0x6BB470). co.cpp's Set_Model 0x6F33F0 hooks every model
** with it: Pre_Render pushes the first pass's material pass and the override flags (4 when the pass's flag is set);
** a flagged pass with a blending shader goes to WW3D's static sort list while sorting is on and the model is not
** drawn now. Post_Render pops both.
**
** S1 (2026-11-14): the class and its pair struct moved to unknown_6bb7e0.h (co.cpp and eff_pass.cpp include it).
** The pair holds a RefCountPtr: its copy 0x6BBA90 and destructor 0x6BB9F0 (out of line here; co.cpp's list erase
** 0x701240 calls the destructor) add and release the reference. 0x6BB8E0 pushes onto the list's front, 0x6BBA20
** erases the first entry whose pass is the one given.
**	Pre_Render's sort test reads WW3D::AreStaticSortListsEnabled (0x6BB844 reads 0xBFAE50, the address
**	ww3d.obj's own functions read it at), not IsSortingEnabled (0xBB9644) (relocaudit.py, run 2026-11-21 PRESET).
*/
