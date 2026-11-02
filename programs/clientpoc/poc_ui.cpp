// Earth & Beyond client (reconstruction): the proof-of-concept client's view stand-ins (D179). NOT THE CLIENT'S CODE.
/*
**	The views' stand-ins on the login path: the message box, the settings store, the avatar model and
**	Create_Peek 0x497300. (The 2-D scene stand-in went when interface_scene.cpp landed in app.)
*/



#include <string.h>
#include "..\..\stubs\stub_log.h"

// sclient.h's ViewUnknown34Class (constructor 0x5875C0, 0x1F8 bytes; every view's +0x34, the message box):
// here the InterfaceClass alone, no dialog gadgets (its +0x7C Active stays false, so LoginTaskClass::Run
// never asks it for a button)
#include "..\..\app\client\sclient.h"

// 0x5875C0
ViewUnknown34Class::ViewUnknown34Class(UIViewClass *view, const LayerClass &layer) : InterfaceClass(view, 0)
{
	STUB_CALLED("POC ViewUnknown34Class (0x5875C0, the views' message box): an empty InterfaceClass");
	memset(Unknown84, 0, sizeof(Unknown84));
	memset(Type, 0, sizeof(Type));
	memset(Unknown190, 0, sizeof(Unknown190));
}

// tsk_game.h's Unknown_7CA2D0 (0x7CA2D0): the settings store, a function-static object (0x1B4 bytes at
// 0xBE8890, constructor 0x7C97B0); ui_login.cpp clears its std::map at +0x60. Here zeroed, with the map
// constructed, so the readers see no settings.
#include <map>
#include <string>
#include "..\..\app\client\tasks\tsk_game.h"
struct PocSettingsStruct
{
	PocSettingsStruct() { memset(Unknown00, 0, sizeof(Unknown00)); memset(Unknown70, 0, sizeof(Unknown70)); }
	char Unknown00[0x60];
	std::map<std::string, std::string> Unknown60;
	char Unknown70[0x1B4 - 0x60 - sizeof(std::map<std::string, std::string>)];
};

Unknown_7CA2D0Class *Unknown_7CA2D0()
{
	static PocSettingsStruct settings;
	STUB_CALLED("POC Unknown_7CA2D0 (0x7CA2D0, the settings store): an empty one");
	return((Unknown_7CA2D0Class *)&settings);
}

// avatar.cpp's model (charview.h's CharacterViewAvatarClass, constructor 0x77B870, 0x384 bytes): P-POC-2's
// stand-in. The 3-D avatar is not built: the object is a ww3d2 Null3DObjClass (a render object that draws
// nothing) under the class's name, so the view can attach, remove and release it; +0x220 (the narrator's
// speech flag) stays false. Its other members are gen_stubs' no-ops.
#include <new.h>
#include "nullrobj.h"
#include "..\..\app\client\charview.h"

// 0x77B870
CharacterViewAvatarClass::CharacterViewAvatarClass(AvatarDescClass *desc, int a, int b, SClientUnknown1338Class *env_maps)
{
	STUB_CALLED("POC CharacterViewAvatarClass (0x77B870, avatar.cpp): a Null3DObjClass, no avatar");
	new ((void *)this) Null3DObjClass("AVATAR");
	Unknown220 = false;
}

// 0x497300 (gadget.cpp, textgadg.cpp, ui_login.cpp declare it): Create_Peek<RenderObjClass> out of line; gen_stubs'
// stand-in cannot build the returned RefCountPtr
#include "ref_ptr.h"
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj)
{
	return(Create_Peek(robj));
}
