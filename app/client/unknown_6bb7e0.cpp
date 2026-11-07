// Earth & Beyond client (reconstruction): app\client\unknown_6bb7e0.cpp, PROVISIONAL file name (D137; notes at the end)
#include <list>
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\ww3d2\matpass.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\ww3d2\ww3d.h"

// co.cpp's render hook (vtable 0xB007A4, 0x10 bytes; declared in co.cpp too, where Set_Model 0x6F33F0 makes it)
struct UnknownB007A4PassStruct	// NAME 1: neutral (D10): co.cpp's
{
	MaterialPassClass *Pass;	// +0x00
	bool Unknown04;				// +0x04 (an override flag 4 when set; with a blending shader, the static sort)
};
class UnknownB007A4Class : public RenderHookClass	// NAME 1: neutral (D10), named by its vtable: co.cpp's
{
public:
	virtual ~UnknownB007A4Class() {}
	virtual bool Pre_Render(RenderObjClass *robj, RenderInfoClass &rinfo);
	virtual void Post_Render(RenderObjClass *robj, RenderInfoClass &rinfo);

	std::list<UnknownB007A4PassStruct> Passes;	// +0x04
};

// @0x6BB7E0
bool UnknownB007A4Class::Pre_Render(RenderObjClass *robj, RenderInfoClass &rinfo)
{
	if (Passes.size() != 0) {
		rinfo.Push_Material_Pass(Passes.front().Pass);
		int flags = Passes.front().Unknown04 ? 4 : 0;
		rinfo.Push_Override_Flags((RenderInfoClass::RINFO_OVERRIDE_FLAGS)(rinfo.Current_Override_Flags() | flags));
	}
	if (Passes.size() != 0 && Passes.front().Unknown04 && Passes.front().Pass->Peek_Shader().Get_Dst_Blend_Func() != ShaderClass::DSTBLEND_ZERO && WW3D::Is_Sorting_Enabled()) {
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

/*
** Reconstruction notes (WE5, 2026-11-07)
**
** The object: no __FILE__ or assert; client 0x6BB7E0-0x6BB8CB with std::list helpers after it (0x6BB8E0, 0x6BBA20)
** and font32bit.cpp from 0x6BBAC0; before it debris.cpp (0x6BB470). co.cpp's Set_Model 0x6F33F0 hooks every model
** with it: Pre_Render pushes the first pass's material pass and the override flags (4 when the pass's flag is set);
** a flagged pass with a blending shader goes to WW3D's static sort list while sorting is on and the model is not
** drawn now. Post_Render pops both.
*/
