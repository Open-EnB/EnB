// Earth & Beyond client (reconstruction): the proof-of-concept client's 2-D view scene (D179). NOT THE CLIENT'S CODE.
/*
**	uiview.h's ViewUnknown1CClass (constructor 0x6DA4D0, 0xA0 bytes, table 0xAFF1F4; the views' +0x1C):
**	the client's is a SimpleSceneClass with a static sort list at +0x9C (0x6DA930, 0x74 bytes) that its
**	render 0x6DA760 draws after the scene. Here a plain SimpleSceneClass: the interfaces' gadgets are
**	render objects in it, drawn by WW3D::Render without the sort list (+0x9C stays NULL, so the
**	interfaces' FAIL_IF '!ssl' paths skip it). Delete this file when the real class lands in app/.
*/



#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\uiview.h"
#include "camera.h"
#include "ww3d.h"

// 0x6DA4D0
ViewUnknown1CClass::ViewUnknown1CClass()
{
	STUB_CALLED("POC ViewUnknown1CClass (0x6DA4D0, the views' 2-D scene): a SimpleSceneClass, no static sort list");
	memset(Unknown98, 0, sizeof(Unknown98));
	*(void **)&Unknown98[4] = NULL;	// +0x9C the static sort list: none
}

// 0x6DA760: the scene with the view's camera (the client then draws the static sort list)
void ViewUnknown1CClass::Unknown_6DA760(RefCountPtr<CameraClass> &camera)
{
	STUB_CALLED("POC ViewUnknown1CClass::Unknown_6DA760 (0x6DA760): WW3D::Render(this, camera)");
	WW3D::Render(this, camera.Peek(), false, false, Vector3(0.0f, 0.0f, 0.0f));
}

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
