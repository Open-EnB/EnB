// Earth & Beyond client (reconstruction): app\client\unknown_6bb7e0.h, PROVISIONAL file name (D137): the render hook
// co.cpp's Set_Model 0x6F33F0 gives every model (vtable 0xB007A4, 0x10 bytes, over RenderHookClass's 0xB007B4);
// co.cpp, eff_pass.cpp and unknown_6bb7e0.cpp share it
#ifndef UNKNOWN_6BB7E0_H
#define UNKNOWN_6BB7E0_H

#include <list>
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\matpass.h"
#include "ref_ptr.h"

class RenderInfoClass;

// a (material pass, flag) pair on the hook's list: its copy 0x6BBA90 adds a reference, its destructor 0x6BB9F0
// releases it (both out of line in unknown_6bb7e0.cpp; co.cpp's list erase 0x701240 calls the destructor)
struct UnknownB007A4PassStruct	// NAME 1: neutral (D10)
{
	UnknownB007A4PassStruct(const RefCountPtr<MaterialPassClass> &pass, bool flag) : Pass(pass), Unknown04(flag) {}	// (inline in 0x6BB8E0)  // NAME 1 [pass, flag]: what Unknown_6BB8E0 gets

	RefCountPtr<MaterialPassClass> Pass;	// +0x00 (0x6BB7E0 pushes it)  // NAME 2: what it holds
	bool Unknown04;							// +0x04 (0x6BB7E0: an override flag 4 when set; with a blending shader, the static sort)
};

class UnknownB007A4Class : public RenderHookClass	// NAME 1: neutral (D10), named by its vtable
{
public:
	virtual ~UnknownB007A4Class() {}										// 0x6F3770 (scalar deleting 0x6F3740; co.cpp)
	virtual bool Pre_Render(RenderObjClass *robj, RenderInfoClass &rinfo);	// 0x6BB7E0
	virtual void Post_Render(RenderObjClass *robj, RenderInfoClass &rinfo);	// 0x6BB8B0
	void Unknown_6BB8E0(const RefCountPtr<MaterialPassClass> &pass, bool flag);	// 0x6BB8E0 (the pass and its flag onto the list's front; eff_pass.cpp's Unknown_18)  // NAME 1 [pass, flag]: what Unknown_18 passes
	void Unknown_6BBA20(const RefCountPtr<MaterialPassClass> &pass);			// 0x6BBA20 (the pass's first entry off the list; eff_pass.cpp's Unknown_20)  // NAME 1 [pass]: what Unknown_20 passes

	std::list<UnknownB007A4PassStruct> Passes;	// +0x04  // NAME 2: what it holds
};

#endif
