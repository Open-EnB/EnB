// Earth & Beyond client (reconstruction): the proof-of-concept client's view stand-ins (D179). NOT THE CLIENT'S CODE.
/*
**	The views' stand-ins on the login path: the avatar model and
**	Create_Peek 0x497300. (The 2-D scene stand-in went when interface_scene.cpp landed in app; the settings store
**	when settings_store.cpp's 0x7CA2D0 did.)
*/



#include <string.h>
#include <new.h>
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

// ---------------------------------------------------------------- ui_avsel.cpp forwarders (charview, avatar, signal map)

#include "..\..\app\client\charview.h"
#include "..\..\app\client\avatar.h"
#include "..\..\app\client\control_keys_manager.h"
#include "..\..\app\client\unknown_signalbase.h"

class SClientUnknown1338Class
{
public:
	void Unknown_760770(int base_asset, bool a);
};

class UIAvselCharacterViewClass
{
public:
	void Unknown_67A250();
	void Unknown_67ACE0(int slot);
	void Unknown_67AB20(int slot, const char *name);
	void Unknown_67B7A0(int arg);
	void Unknown_67A290(const Vector3 &a, const Vector3 &b, float fov);
	bool Unknown_67AD30(int slot, const char *name);
	void Unknown_67AE20(int slot);
	void Unknown_67AE60(int slot);
	void Unknown_67A9B0(int arg);
	bool Unknown_67AEA0(int slot);
	void Unknown_67ADD0(int slot);
	bool Unknown_67B2B0();
	void Unknown_67B270();
	void Unknown_5594B0(AvatarDescClass desc);
};

void UIAvselCharacterViewClass::Unknown_67A250() { ((CharacterViewClass *)this)->Unknown_67A250(); }
void UIAvselCharacterViewClass::Unknown_67ACE0(int slot) { ((CharacterViewClass *)this)->Unknown_67ACE0(slot); }
void UIAvselCharacterViewClass::Unknown_67AB20(int slot, const char *name) { ((CharacterViewClass *)this)->Unknown_67AB20(slot, name); }
void UIAvselCharacterViewClass::Unknown_67B7A0(int arg) { ((CharacterViewClass *)this)->Unknown_67B7A0(arg); }
void UIAvselCharacterViewClass::Unknown_67A290(const Vector3 &a, const Vector3 &b, float fov) { ((CharacterViewClass *)this)->Unknown_67A290(a, b, fov); }
bool UIAvselCharacterViewClass::Unknown_67AD30(int slot, const char *name) { return ((CharacterViewClass *)this)->Unknown_67AD30(slot, name); }
void UIAvselCharacterViewClass::Unknown_67AE20(int slot) { ((CharacterViewClass *)this)->Unknown_67AE20(slot); }
void UIAvselCharacterViewClass::Unknown_67AE60(int slot) { ((CharacterViewClass *)this)->Unknown_67AE60(slot); }
void UIAvselCharacterViewClass::Unknown_67A9B0(int arg) { ((CharacterViewClass *)this)->Unknown_67A9B0(arg); }
bool UIAvselCharacterViewClass::Unknown_67AEA0(int slot) { return ((CharacterViewClass *)this)->Unknown_67AEA0(slot); }
void UIAvselCharacterViewClass::Unknown_67ADD0(int slot) { ((CharacterViewClass *)this)->Unknown_67ADD0(slot); }
bool UIAvselCharacterViewClass::Unknown_67B2B0() { return ((CharacterViewClass *)this)->Unknown_67B2B0(); }
void UIAvselCharacterViewClass::Unknown_67B270() { ((CharacterViewClass *)this)->Unknown_67B270(); }
void UIAvselCharacterViewClass::Unknown_5594B0(AvatarDescClass desc) { ((CharacterViewClass *)this)->Unknown088 = desc; }

class UIAvselEnvMapClass
{
public:
	void Unknown_760770(int base_asset, bool a);
};

void UIAvselEnvMapClass::Unknown_760770(int base_asset, bool a)
{
	((SClientUnknown1338Class *)this)->Unknown_760770(base_asset, a);
}

struct UIAvselKeyEventStruct
{
	int Key;
	float Unknown04;
	float Unknown08;
};

class UIAvselKeysClass
{
public:
	void Unknown_721E60(const UIAvselKeyEventStruct *event, int id);
	void Unknown_721EF0(const UIAvselKeyEventStruct *event);
};

void UIAvselKeysClass::Unknown_721E60(const UIAvselKeyEventStruct *event, int id)
{
	ControlKeyComboStruct combo((unsigned short)event->Key, 0);
	((InterfaceKeyListClass *)this)->Add(combo, (void *)id);
}

void UIAvselKeysClass::Unknown_721EF0(const UIAvselKeyEventStruct *event)
{
	ControlKeyComboStruct combo((unsigned short)event->Key, 0);
	((InterfaceKeyListClass *)this)->Remove(combo);
}

class UIAvselNotifierClass
{
public:
	void Add(int id, SignalBaseClass::CallbackClass *callback);
	void Remove(int id, SignalBaseClass::CallbackClass *callback);
};

void UIAvselNotifierClass::Add(int id, SignalBaseClass::CallbackClass *callback)
{
	((SignalMap *)this)->connect((void *)id, callback);
}

void UIAvselNotifierClass::Remove(int id, SignalBaseClass::CallbackClass *callback)
{
	if (callback != NULL) {
		((SignalMap *)this)->disconnect((void *)id, *callback);
	}
}

class UIAvselAvatarClass
{
public:
	UIAvselAvatarClass(AvatarDescClass *desc, int a, int b, UIAvselEnvMapClass *env);
	bool Unknown_7863E0();
	void Unknown_786370(int on);
	void Unknown_77E3D0(int arg);
	void Unknown_784A50(const char *emote, int a, int b);
	void Unknown_7896B0(int arg);
};

UIAvselAvatarClass::UIAvselAvatarClass(AvatarDescClass *desc, int a, int b, UIAvselEnvMapClass *env)
{
	new (this) CharacterViewAvatarClass(desc, a, b, (SClientUnknown1338Class *)env);
}

bool UIAvselAvatarClass::Unknown_7863E0() { return ((CharacterViewAvatarClass *)this)->Unknown_7863E0(); }
void UIAvselAvatarClass::Unknown_786370(int on) { ((CharacterViewAvatarClass *)this)->Unknown_786370(on != 0); }
void UIAvselAvatarClass::Unknown_77E3D0(int arg) { ((CharacterViewAvatarClass *)this)->Unknown_77E3D0(arg); }
void UIAvselAvatarClass::Unknown_784A50(const char *emote, int a, int b) { ((CharacterViewAvatarClass *)this)->Unknown_784A50(emote, a, b); }
void UIAvselAvatarClass::Unknown_7896B0(int arg) { ((CharacterViewAvatarClass *)this)->Unknown_7896B0(arg); }
