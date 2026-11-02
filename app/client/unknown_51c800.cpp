// Earth & Beyond client (reconstruction): a dynamic window element over a text (client 0x51C800-0x51C91B).
// File PROVISIONAL (D137): no string; between dynamic_window_multi_line_text.cpp (to 0x51C780) and
// dynamic_window_render_object.cpp (0x51C930), so a dynamic_window_*.cpp named between them alphabetically.
#include "unknown_51c800.h"
#include "fontmanager.h"

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's

// 0x51C800 (the deleting destructor 0x51C8C0, the destructor 0x51C8F0 and slot +0x58 0x51C910 follow: the header's)
Unknown51C800Class::Unknown51C800Class(FontStyleClass *text, unsigned int flags) :
	DynamicWindowRenderObjectClass(flags)
{
	Set_Render_Object(Unknown_497300(text));
}
