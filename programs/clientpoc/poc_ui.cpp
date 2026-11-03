// Earth & Beyond client (reconstruction): the proof-of-concept client's view stand-ins (D179). NOT THE CLIENT'S CODE.
/*
**	The views' stand-ins on the login path: the avatar model and
**	Create_Peek 0x497300. (The 2-D scene stand-in went when interface_scene.cpp landed in app; the settings store
**	when settings_store.cpp's 0x7CA2D0 did.)
*/



#include <string.h>
#include "..\..\stubs\stub_log.h"

#include "..\..\app\client\sclient.h"

// 0x497300 (gadget.cpp, textgadg.cpp, ui_login.cpp declare it): Create_Peek<RenderObjClass> out of line; gen_stubs'
// stand-in cannot build the returned RefCountPtr
#include "rendobj.h"
#include "ref_ptr.h"
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj)
{
	return(Create_Peek(robj));
}
// mousetbl.cpp's cursor table and its loader / unloader 0x7228C0 / 0x723150: app\client\mousetbl.cpp (KRN, exact).
