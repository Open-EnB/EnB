// Earth & Beyond client (reconstruction): app\Client\ui_avsel.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "camera.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
#include "..\audio\soundlib.h"
#include "dx8renderer.h"
#include "vector4.h"
#include "tutorial_manager.h"
#include "ui_avsel.h"

// What a gadget's slot +0x08 returns, as far as 0x55E070 reads it
struct UIAvselGadgetPartStruct	// NAME 1: neutral (D10)
{
	char Unknown00[0x30];								// +0x00
	RenderObjClass *Unknown30;							// +0x30 (0x55E070 highlights it)
};

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04();
	virtual UIAvselGadgetPartStruct *Unknown_08();		// +0x08 (0x55E070)
	virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24();
	virtual void Unknown_28(LayerClass *layer);			// +0x28 (0x55F760: the window's layer)  // NAME 1 [layer]: as ui_cpit.cpp's
	virtual void Unknown_2C();
	virtual bool Unknown_30(float x, float y);			// +0x30 (0x55E070: the pointer's position, 0..1: over the gadget)  // NAME 1 [x, y]: neutral
	virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (0x55ED20: 1 lights a button, 0x55EF90: 0)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual bool Unknown_60();							// +0x60 (0x55EF90: the button is lit)
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(int arg);					// +0x74 (0x55F760: the edit box, 1)  // NAME 1 [arg]: neutral
	char Unknown04[0x20 - 0x04];						// +0x04
	void *Unknown20;									// +0x20 (0x55F6D0: the mouse-over sound only when set)
	char Unknown24[0x2C - 0x24];						// +0x24
	bool Is_Unknown2C_2() { return(Unknown2CBits.Unknown2C_2); }	// NAME 1: neutral (D10): inline (0x55F6D0)
	void Unknown_660070(int arg);						// 0x660070 (0x55E370 passes 1)  // NAME 1 [arg]: neutral
	union {
		unsigned int Unknown2C;							// +0x2C (0x55E370 sets 0x400)
		struct {
			unsigned int Unknown2C_0 : 2;
			unsigned int Unknown2C_2 : 1;				// bit 2 (0x55F6D0: the mouse-over sound)  // NAME 1: neutral (D10)
		} Unknown2CBits;								// NAME 1: neutral (D10)
	};
};
// gadget.cpp's button gadget (100 bytes, constructor 0x675240 over a render object and a layer)
class UIAvselButtonClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIAvselButtonClass(RenderObjClass *robj, LayerClass *layer, int a, int b);	// 0x675240 (the constructor passes 0x55, 3)  // NAME 1 [robj, layer, a, b]: as ui_manu.cpp's
	char Unknown30[0x64 - 0x30];						// +0x30
};
void Unknown_65EED0(GadgetClass *gadget);				// 0x65EED0 (cdecl, gadget.cpp): the gadget deleted  // NAME 1 [gadget]: neutral
void Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight (0, 0, 0 off: 0x55E070)  // NAME 1 [robj, a, b, c]: as ui_group.cpp's
void Unknown_496490(RenderObjClass *robj, int unknown);	// 0x496490 (ro_funcs.cpp, cdecl: 0x55E070 highlights the avatar under the pointer)  // NAME 1 [robj, unknown]: neutral
void Unknown_42ECF0(float *x, float *y);				// 0x42ECF0 (cdecl): the pointer's position, 0..1  // NAME 1 [x, y]: as scriptedmovie.cpp's
void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, unknown]: as eff_o.cpp's
// The FlashingGadgetEvent the window's callback gets (the client's RTTI name, 0xB7C068)
class FlashingGadgetEvent	// NAME 3: the client's RTTI (the callback's type name)
{
public:
	GadgetClass *Gadget;								// +0x00 (0x55F6D0's FAIL_IF '!gadget')  // NAME 1: neutral
};

// fontsys.cpp's text object and fontmanager.h's FontManagerClass at 0xBE67C4, fontdatabase.h's FontStringFilesClass at
// 0xBE67BC, as far as this file calls them (as ui_cpit.cpp declares them: neither header can be included here)
class FontSys : public RenderObjClass	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)
{
public:
	void Change_Text(const char *text, int unknown);	// 0x6CDBC0  // NAME 1 [text, unknown]: as loadingview.cpp's
	void Unknown_6CE910(bool on);						// 0x6CE910 (0x55E2E0: the selected slot's text)  // NAME 1 [on]: neutral
	void Set_Unknown2FC(float value) { Unknown2FC = value; Unknown4C5 = false; }	// NAME 1: neutral (D10): inline (0x55ED20 1.0, 0x55EF90 0.33)
	char Unknown_Pad[0x2FC - sizeof(RenderObjClass)];
	float Unknown2FC;									// +0x2FC (ui_cpit.cpp's)
	char Unknown300[0x4C5 - 0x300];
	bool Unknown4C5;									// +0x4C5
};
class UIAvselManagedFontClass { public: void Unknown_6D0A70(const char *name, const char *text); };	// NAME 1: neutral (D10): fontmanager.h's ManagedFontClass (0x6D0A70 reloads the styles named name with the text)  // NAME 1 [name, text]: fontmanager.h's
class UIAvselFontManagerClass { public: UIAvselManagedFontClass *Find(unsigned int id); void Add(RenderObjClass *object); void Add(const RefCountPtr<RenderObjClass> &object); void Remove(unsigned int id); FontSys *Get_Style(const char *name, RenderObjClass *object); };	// NAME 3: fontmanager.h's (0x6D1D80, 0x6D2220, 0x6D25D0)  // NAME 1 [object, id, name]: as ui_cpit.cpp's
extern UIAvselFontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's

// local_interface_sort.cpp's sort object (8 bytes; as ui_cpit.cpp declares it)
class LineDisplaySortClass  // NAME 1: neutral (D10)
{
public:
	LineDisplaySortClass(InterfaceClass *inter);		// 0x541980  // NAME 1 [inter]: view.cpp's
	~LineDisplaySortClass();							// 0x6E18B0
private:
	char Unknown00[8];
};

// The control keys event the constructor registers (0x10 bytes) and what InterfaceClass +0x4C is (as ui_manu.cpp's)
struct UIAvselKeyEventStruct	// NAME 1: neutral (D10): kernel.h's input event
{
	UIAvselKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B, 0x0D)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
class UIAvselKeysClass { public: void Unknown_721E60(const UIAvselKeyEventStruct *event, int id); void Unknown_721EF0(const UIAvselKeyEventStruct *event); };	// NAME 1: neutral (D10): InterfaceClass +0x4C's 0x721E60  // NAME 1 [event, id]: what the constructor passes (10, 7)

class UIAvselEnvMapClass;
// An avatar's table as far as its slot +0x1C4 (0x55D9C0 calls it for the five every frame; ui_avatr.cpp's +0x94's +0x54)
class UIAvselAvatarVirtualsClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_000(); virtual void Unknown_004(); virtual void Unknown_008(); virtual void Unknown_00C(); virtual void Unknown_010(); virtual void Unknown_014(); virtual void Unknown_018(); virtual void Unknown_01C();
	virtual void Unknown_020(); virtual void Unknown_024(); virtual void Unknown_028(); virtual void Unknown_02C(); virtual void Unknown_030(); virtual void Unknown_034(); virtual void Unknown_038(); virtual void Unknown_03C();
	virtual void Unknown_040(); virtual void Unknown_044(); virtual void Unknown_048(); virtual void Unknown_04C(); virtual void Unknown_050(); virtual void Unknown_054(); virtual void Unknown_058(); virtual void Unknown_05C();
	virtual void Unknown_060(); virtual void Unknown_064(); virtual void Unknown_068(); virtual void Unknown_06C(); virtual void Unknown_070(); virtual void Unknown_074(); virtual void Unknown_078(); virtual void Unknown_07C();
	virtual void Unknown_080(); virtual void Unknown_084(); virtual void Unknown_088(); virtual void Unknown_08C(); virtual void Unknown_090(); virtual void Unknown_094(); virtual void Unknown_098(); virtual void Unknown_09C();
	virtual void Unknown_0A0(); virtual void Unknown_0A4(); virtual void Unknown_0A8(); virtual void Unknown_0AC(); virtual void Unknown_0B0(); virtual void Unknown_0B4(); virtual void Unknown_0B8(); virtual void Unknown_0BC();
	virtual void Unknown_0C0(); virtual void Unknown_0C4(); virtual void Unknown_0C8(); virtual void Unknown_0CC(); virtual void Unknown_0D0(); virtual void Unknown_0D4(); virtual void Unknown_0D8(); virtual void Unknown_0DC();
	virtual void Unknown_0E0(); virtual void Unknown_0E4(); virtual void Unknown_0E8(); virtual void Unknown_0EC(); virtual void Unknown_0F0(); virtual void Unknown_0F4(); virtual void Unknown_0F8(); virtual void Unknown_0FC();
	virtual void Unknown_100(); virtual void Unknown_104(); virtual void Unknown_108(); virtual void Unknown_10C(); virtual void Unknown_110(); virtual void Unknown_114(); virtual void Unknown_118(); virtual void Unknown_11C();
	virtual void Unknown_120(); virtual void Unknown_124(); virtual void Unknown_128(); virtual void Unknown_12C(); virtual void Unknown_130(); virtual void Unknown_134(); virtual void Unknown_138(); virtual void Unknown_13C();
	virtual void Unknown_140(); virtual void Unknown_144(); virtual void Unknown_148(); virtual void Unknown_14C(); virtual void Unknown_150(); virtual void Unknown_154(); virtual void Unknown_158(); virtual void Unknown_15C();
	virtual void Unknown_160(); virtual void Unknown_164(); virtual void Unknown_168(); virtual void Unknown_16C(); virtual void Unknown_170(); virtual void Unknown_174(); virtual void Unknown_178(); virtual void Unknown_17C();
	virtual void Unknown_180(); virtual void Unknown_184(); virtual void Unknown_188(); virtual void Unknown_18C(); virtual void Unknown_190(); virtual void Unknown_194(); virtual void Unknown_198(); virtual void Unknown_19C();
	virtual void Unknown_1A0(); virtual void Unknown_1A4(); virtual void Unknown_1A8(); virtual void Unknown_1AC(); virtual void Unknown_1B0(); virtual void Unknown_1B4(); virtual void Unknown_1B8(); virtual void Unknown_1BC();
	virtual void Unknown_1C0();
	virtual void Unknown_1C4();							// +0x1C4
};

// One of the five avatars on the screen (avatar.cpp's render object), as far as this file calls it
class UIAvselAvatarClass : public RenderObjClass	// NAME 1: neutral (D10)
{
public:
	UIAvselAvatarClass(AvatarDescClass *desc, int a, int b, UIAvselEnvMapClass *env);	// 0x77B870 (900 bytes; 0x55E370: a copy of the slot's descriptor, 1, 0, the view's +0x3A4)  // NAME 1 [desc, a, b, env]: neutral
	bool Unknown_7863E0();								// 0x7863E0 (slot +0x60: false starts it, 0x786370)
	void Unknown_786370(int on);						// 0x786370  // NAME 1 [on]: neutral
	void Unknown_77E3D0(int arg);						// 0x77E3D0 (0x55E370 passes 1)  // NAME 1 [arg]: neutral
	void Unknown_784A50(const char *emote, int a, int b);	// 0x784A50 ('/happy at your service', 1, 0)  // NAME 1 [emote, a, b]: neutral
	void Unknown_7896B0(int arg);						// 0x7896B0 (0x55D9C0: 1, the deleted character's)  // NAME 1 [arg]: neutral
	virtual RenderObjClass *Clone() const;				// (RenderObjClass's pure ones, avatar.cpp's)
	virtual void Render(RenderInfoClass &rinfo);
	char Unknown_Pad[0x108 - sizeof(RenderObjClass)];
	int Unknown108;										// +0x108 (0x55F140: the race, 0 .. 2)
	int Unknown10C;										// +0x10C (0x55F140: the profession, 0 .. 2)
	char Unknown110[0x2BC - 0x110];						// +0x110
	RenderObjClass *Get_Unknown2BC() { return(Unknown2BC); }	// NAME 1: neutral (D10)
	RenderObjClass *Unknown2BC;							// +0x2BC (0x55E370's button is made over it)
	char Unknown2C0[0x384 - 0x2C0];						// +0x2C0 (900 bytes)
};

// env_map_manager.cpp's object at the character view's +0x3A4 (as sclient.cpp declares it)
class UIAvselEnvMapClass { public: void Unknown_760770(int base_asset, bool a); };	// NAME 1: neutral (D10)  // NAME 1 [base_asset, a]: sclient.cpp's

// What the character view's +0x1BC points to, as far as 0x55E370 calls it
class UIAvselUnknown1BCClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(RenderObjClass *robj, const char *name);	// +0x80 (0x55E370: an avatar, 'DUDE%02d')  // NAME 1 [robj, name]: neutral
};

// tsk_login.h's CharacterViewClass (charview.cpp), as far as this file calls it
class UIAvselCharacterViewClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_67A250();								// 0x67A250
	void Unknown_67ACE0(int arg);						// 0x67ACE0 (the constructor: 0)  // NAME 1 [arg]: neutral
	void Unknown_67AB20(int slot, const char *name);	// 0x67AB20 (0 'FE12A', 1 'FE01C', 2 'FE_ROOM')  // NAME 1 [slot, name]: neutral
	void Unknown_67B7A0(int arg);						// 0x67B7A0 (the constructor: 1)  // NAME 1 [arg]: neutral
	void Unknown_67A290(const Vector3 &a, const Vector3 &b, float fov);	// 0x67A290 ((0, 0, 0), (0, -90, -90), 45)  // NAME 1 [a, b, fov]: neutral
	bool Unknown_67AD30(int slot, const char *name);	// 0x67AD30 (1, 'FE01C.FE01C-1')  // NAME 1 [slot, name]: neutral
	void Unknown_67AE20(int slot);						// 0x67AE20  // NAME 1 [slot]: neutral
	void Unknown_67AE60(int slot);						// 0x67AE60  // NAME 1 [slot]: neutral
	void Unknown_67A9B0(int arg);						// 0x67A9B0 (the destructor: 0)  // NAME 1 [arg]: neutral
	bool Unknown_67AEA0(int slot);						// 0x67AEA0 (0x55D9C0)  // NAME 1 [slot]: neutral
	void Unknown_67ADD0(int slot);						// 0x67ADD0 (0x55D9C0)  // NAME 1 [slot]: neutral
	bool Unknown_67B2B0();								// 0x67B2B0 (0x55D9C0; ui_avatr.cpp's)
	void Unknown_67B270();								// 0x67B270 (slot +0x68, key 10)
	void Unknown_5594B0(AvatarDescClass desc);			// 0x5594B0 (slot +0x68: the selected character's descriptor, by value)  // NAME 1 [desc]: neutral

	char Unknown000[0x14];								// +0x00
	LoginTaskClass *Task;								// +0x14 (its slot +0x08 returns itself: docs/kernel-map-step9.md 3.3)  // NAME 1: neutral
	char Unknown018[0x34 - 0x18];						// +0x18
	ViewUnknown34Class *Unknown034;						// +0x34 (the message box: 0x55D9C0's 'OK' and 'YES_NO')
	char Unknown038[0x68 - 0x38];						// +0x38
	CameraClass *Camera;								// +0x68 (the constructor's clip planes)  // NAME 2: what it holds
	char Unknown06C[0x70 - 0x6C];						// +0x6C
	LayerClass *Get_Unknown070() { return(Unknown070); }	// NAME 1: neutral (D10)
	LayerClass *Unknown070;								// +0x70 (0x55E370's avatar buttons are made over it)
	char Unknown074[0x1B8 - 0x74];						// +0x74
	RenderObjClass *Peek_Model() { return(Model); }		// NAME 1: neutral (D10)
	RenderObjClass *Model;								// +0x1B8 (the screen's model: its texts and buttons by name)  // NAME 2: what it holds
	UIAvselUnknown1BCClass *Unknown1BC;					// +0x1BC (0x55E370 hands it the avatars by name)
	char Unknown1C0[0x1CD - 0x1C0];						// +0x1C0
	bool Unknown1CD;										// +0x1CD (0x55D9C0; tsk_login.h's)
	bool Unknown1CE;										// +0x1CE
	char Unknown1CF[0x1E4 - 0x1CF];						// +0x1CF
	int Unknown1E4;										// +0x1E4 (slot +0x8C)
	char Unknown1E8[0x244 - 0x1E8];						// +0x1E8
	ShipCustDescClass Unknown244;						// +0x244 (slot +0x68 copies the selected character's into it; 0x160 bytes)
	UIAvselEnvMapClass *Get_Unknown3A4() { return(Unknown3A4); }	// NAME 1: neutral (D10)
	UIAvselEnvMapClass *Unknown3A4;						// +0x3A4
};

// The window's FlashingGadgetEvent callback: SignalBaseClass::MemberCallback (vtable 0xAEFAEC over 0xAEDB58; its
// members out of line after the galaxy list's: the destructor 0x560250, 0x560270, 0x560290, 0x5602B0, 0x560330 and
// the deleting destructor 0x560390)
#include "unknown_signalbase.h"
typedef SignalBaseClass::MemberCallback<GadgetClass, AvatarSelectionInterface, void (AvatarSelectionInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> UIAvselCallbackClass;	// NAME 1 [UIAvselCallbackClass]: neutral (D10)
typedef SignalBaseClass::CallbackClass UIAvselCallbackBaseClass;	// NAME 1 [UIAvselCallbackBaseClass]: neutral (D10)

class UIAvselNotifierClass		// NAME 1: neutral (D10)
{
public:
	void Add(int id, UIAvselCallbackBaseClass *callback);	// 0x4F9740  // NAME 2: as sclient.cpp's
	void Remove(int id, UIAvselCallbackBaseClass *callback);	// 0x4F98D0  // NAME 2: as sclient.cpp's
};
UIAvselNotifierClass *Unknown_65ECE0();					// 0x65ECE0  // NAME 1 [Unknown_65ECE0]: neutral (D10)

// The object at *0xBFAEE0 (kernel.cpp's): its slot +0x14 makes a render object by name (as ui_login.cpp declares it)
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10();
	virtual RenderObjClass *Create_Render_Obj(const char *name);	// +0x14 ('BUDDYREF1')  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

extern unsigned int Unknown_BFAE3C; extern unsigned int Unknown_BFAE40;	// NAME 1: neutral (D10): the time now and at the last frame in ms (as scriptedmovie.cpp declares them)

// The button models and their texts (0xB7B8E0, 0xB7B904: the five slots, delete, continue / confirm, cancel / exit)
static const char *Unknown_B7B8E0[9] = { "name00", "name01", "name02", "name03", "name04", "delete", "continue", "cancel", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7B904[9] = { "fe12_Name_00", "fe12_Name_01", "fe12_Name_02", "fe12_Name_03", "fe12_Name_04", "fe12_Delete", "fe12_confirm", "fe12_exit", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7B928[3] = { "Race: Terran", "Race: Jenquai", "Race: Progen" };	// NAME 1: neutral (D10)
static const char *Unknown_B7B934[9] = { "Class: Enforcer", "Class: Tradesman", "Class: Scout", "Class: Defender", "Class: Seeker", "Class: Explorer", "Class: Warrior", "Class: Privateer", "Class: Sentinel" };	// NAME 1: neutral (D10)
static const char *Unknown_B7B958[2] = { "CANCEL_BUTTON", "OK_BUTTON" };	// NAME 1: neutral (D10)
static const char *Unknown_B7B960[2] = { "CANCEL_BUTTON_BUDDYREF1", "OK_BUTTON_BUDDYREF1" };	// NAME 1: neutral (D10)

class LoginTaskClass;
// LoginTaskClass's two accessors slot +0x68 calls out of line (0x55D860, 0x55D880; in this file's window)
class UIAvselTaskClass	// NAME 1: neutral (D10): tsk_login.h's LoginTaskClass
{
public:
	int Get_Selected() { return(((LoginTaskClass *)this)->Selected); }					// 0x55D860  // NAME 1: neutral (D10)
	AvatarDescClass *Get_Descs() { return(((LoginTaskClass *)this)->Unknown0350); }	// 0x55D880  // NAME 1: neutral (D10)
};
// What the edit box's +0x4C points to, as far as 0x55F760 calls it
class UIAvselEditUnknown4CClass { public: void Unknown_6E2A20(const Vector4 &color); };	// NAME 1: neutral (D10)  // NAME 1 [color]: neutral (1, 1, 1, 1)
// textgadg.cpp's text gadget (0x80 bytes, constructor 0x6AA810; as ui_login.cpp declares it): the referral's name
class UIAvselEditClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIAvselEditClass(InterfaceClass *iface, int length, const char *name, RenderObjClass *model, int a, int b);	// 0x6AA810  // NAME 1 [iface, length, name, model, a, b]: as ui_login.cpp's
	void Set_Text(const char *text, int unknown);		// 0x6AA570  // NAME 1: as ui_login.cpp's
	char *Get_Text();									// 0x6AA6C0 (a copy, deleted by the caller)  // NAME 1: as ui_login.cpp's
	void Unknown_6AAD10();								// 0x6AAD10 (every frame)
	char Unknown30[0x3B - 0x30];						// +0x30
	bool Unknown3B;										// +0x3B (0x55F760 sets it)
	char Unknown3C[0x4C - 0x3C];						// +0x3C
	UIAvselEditUnknown4CClass *Unknown4C;				// +0x4C
	char Unknown50[0x5C - 0x50];						// +0x50
	bool Unknown5C;										// +0x5C (0x55F760 clears it; ui_login.cpp: the password's hidden)
	char Unknown5D[0x80 - 0x5D];						// +0x5D
};
extern const int Unknown_B08CAC; extern const int Unknown_B08CBC; extern const int Unknown_B08CC0;	// NAME 1: neutral (D10): what 0x55F760's edit box gets (or'ed)












// 0x55C650
AvatarSelectionInterface::AvatarSelectionInterface(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	Unknown084(NULL), Unknown088(NULL), Unknown08C(0), Unknown090(-1),
	Unknown094(false), Unknown095(false), Unknown096(false), Unknown097(false), Unknown098(false),
	Unknown0EC(0), Unknown0F0(0), Unknown0F4(-1),
	Unknown0F8(false), Unknown0F9(false), Unknown0FA(false), Unknown0FB(false), Unknown0FC(false), Unknown0FD(false),
	Unknown100(-1), Layer(layer), Unknown118(NULL), Unknown120(false)
{
	LineDisplaySortClass sort(this);
	memset(Unknown09C, 0, sizeof(Unknown09C));
	int i;
	for (i = 0; i < 2; i++) {
		Unknown10C[i] = NULL;
	}
	Unknown114 = NULL;
	for (i = 0; i < 5; i++) {
		Unknown0BC[i] = NULL;
		Unknown0D0[i] = NULL;
		Unknown0E4[i] = false;
	}
	Unknown084 = (UIAvselCharacterViewClass *)View->Unknown_28();
	Unknown088 = (LoginTaskClass *)Unknown084->Task->Unknown_08();
	Unknown084->Unknown_67A250();
	Unknown084->Unknown_67ACE0(0);
	Unknown084->Unknown_67AB20(0, "FE12A");
	RenderObjClass *model = Unknown084->Model;
	Unknown084->Unknown_67AB20(1, "FE01C");
	Unknown084->Unknown_67AB20(2, "FE_ROOM");
	Unknown084->Unknown_67B7A0(1);
	Unknown084->Unknown_67A290(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -90.0f, -90.0f), 45.0f);
	Unknown084->Camera->Set_Clip_Planes(1.0f, 500.0f);
	((FontStringFilesClass *)Unknown_BE67BC)->Load("FA_FE12A.ini");
	Unknown_BE67C4->Add(model);
	for (i = 0; i < 8; i++) {
		char name[64];
		strcpy(name, Unknown_B7B8E0[i]);
		RenderObjClass *robj = model->Get_Sub_Object_By_Name(name);
		Unknown_496AE0(robj, 0);
		GadgetClass *gadget = new UIAvselButtonClass(robj, layer, 0x55, 3);
		Unknown09C[i] = gadget;
		Unknown64.Unknown_6870E0(gadget, i + 1, View, 1, 1);
		robj->Release_Ref();
	}
	((UIAvselKeysClass *)Unknown4C)->Unknown_721E60(&UIAvselKeyEventStruct(0x1B), 10);
	((UIAvselKeysClass *)Unknown4C)->Unknown_721E60(&UIAvselKeyEventStruct(0x0D), 7);
	if (Unknown088->Unknown1138) {
		RenderObjClass *robj = model->Get_Sub_Object_By_Name("click_me");
		Unknown_496AE0(robj, 0);
		Unknown118 = new UIAvselButtonClass(robj, layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown118, 0x11, View, 1, 1);
		robj->Release_Ref();
	} else {
		Unknown118 = NULL;
		Unknown_55FE50(false);
	}
	Unknown_55E370(1);
	if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
		Unknown084->Unknown_67AE20(1);
	}
	Unknown0FC = true;
	Unknown_55EF90();
	Unknown084->Unknown3A4->Unknown_760770(100001, true);
	void (AvatarSelectionInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarSelectionInterface::Unknown_55F6D0;
	UIAvselNotifierClass *notifier = Unknown_65ECE0();
	notifier->Add(0, new UIAvselCallbackClass(this, function));
}

// 0x55CCC0
AvatarSelectionInterface::~AvatarSelectionInterface()
{
	Unknown_5C();
	int i;
	for (i = 0; i < 8; i++) {
		if (Unknown09C[i]) {
			Unknown_65EED0(Unknown09C[i]);
			Unknown09C[i] = NULL;
		}
	}
	if (Unknown118) {
		Unknown_65EED0(Unknown118);
		Unknown118 = NULL;
	}
	for (i = 0; i < 5; i++) {
		if (Unknown0D0[i]) {
			Unknown_65EED0(Unknown0D0[i]);
			Unknown0D0[i] = NULL;
		}
		if (Unknown0BC[i]) {
			Unknown0BC[i]->Remove();
			Unknown0BC[i]->Release_Ref();
			Unknown0BC[i] = NULL;
		}
	}
	Unknown_BE67C4->Remove((unsigned int)Unknown084->Peek_Model());
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("FA_FE12A.ini");
	Unknown084->Unknown_67A9B0(0);
	void (AvatarSelectionInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarSelectionInterface::Unknown_55F6D0;
	UIAvselNotifierClass *notifier = Unknown_65ECE0();
	UIAvselCallbackClass callback(this, function);
	notifier->Remove(0, &callback);
}

// 0x55CEA0
void AvatarSelectionInterface::Unknown_60()
{
	Unknown_55E070();
	Unknown_55D9C0();
	if (Unknown114) {
		Unknown114->Unknown_6AAD10();
	}
	float elapsed = (Unknown_BFAE3C - Unknown_BFAE40) * 0.001f;
	if (Unknown0F0 > 0.0f && (Unknown0F0 -= elapsed) < 0.0f) {
		Unknown0F0 = 0.0f;
		if (!Unknown0F9) {
			Unknown_55ED20();
		}
	}
	if (Unknown0EC > 0.0f) {
		Unknown0EC -= elapsed;
		if (Unknown0EC < 0.0f) {
			Unknown0EC = 0.0f;
			if (Unknown0F4 > -1) {
				Unknown088->Selected = Unknown0F4;
				Unknown088->Unknown_50();
				Unknown0F4 = -1;
				Unknown0F0 = 30.0f;
				Unknown_55EF90();
				return;
			}
		} else if (Unknown0EC < 0.7f && Unknown0F4 > -1) {
			if (!Unknown0BC[Unknown0F4]->Unknown_7863E0()) {
				Unknown0BC[Unknown0F4]->Unknown_786370(1);
			}
		}
		Unknown_55EF90();
	}
}

// NOT EXACT: inline budget (0.49): shipcust_desc.h's written-out operator= (P-P7-1) inlines here; the client calls the implicit one out of line (0x55D8A0)
// 0x55D060
bool AvatarSelectionInterface::Unknown_68(int key)
{
	if (Unknown0F4 > -1 || Unknown0F0 != 0.0f) {
		Unknown_55EF90();
		return true;
	}
	if (Unknown0F8 || Unknown0F9 || Unknown0FA || Unknown0FB || Unknown0FC || Unknown0FD || Unknown095 || Unknown094) {
		return false;
	}
	if (!BuddyReferralUI) {
		int previous;
		switch (key) {
			case 17:
				Unknown_55EF90();
				Unknown_55F760();
				return false;
			case 1: case 2: case 3: case 4: case 5:
				SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
				if (Unknown096) {
					Unknown0BC[Unknown090]->Unknown_784A50("/normal", 0, 0);
				}
				previous = Unknown090;
				Unknown090 = key - 1;
				break;
			case 11: case 12: case 13: case 14: case 15:
				SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
				if (Unknown096) {
					Unknown0BC[Unknown090]->Unknown_784A50("/normal", 0, 0);
				}
				previous = Unknown090;
				Unknown090 = key - 11;
				break;
			case 6:
				SoundLibraryClass::Play_2D_Sound("Avatar_Delete_Character", 2);
				if (Unknown090 >= 0) {
					Unknown0BC[Unknown090]->Unknown_784A50("/angry please no", 1, 0);
					Unknown084->Unknown034->Unknown_588A70("YES_NO", 0, "Confirm deletion of the character?", 0x140, 0, 0);
					Unknown097 = true;
					Unknown0F9 = true;
					Unknown_55EF90();
				}
				return true;
			case 7:
				SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
				if (Unknown090 >= 0) {
					Unknown08C = 7;
					Unknown088->Selected = Unknown090;
					if (Unknown0E4[Unknown090]) {
						LoginCharacterClass *character = &Unknown088->Characters[Unknown088->Selected];
						if (character->ID != 0 && character->Galaxy_ID >= 0) {
							TutorialManagerClass::Instance->Write_Completed_File();
							Unknown088->Unknown_5C();
							return true;
						}
						UIAvselTaskClass *task = (UIAvselTaskClass *)Unknown088;
						Unknown084->Unknown_5594B0(task->Get_Descs()[task->Get_Selected()]);
						Unknown084->Unknown244 = Unknown088->Unknown088C[Unknown088->Selected];
						Unknown084->Unknown_67ACE0(1);
						Unknown084->Unknown1CD = true;
						Unknown084->Unknown1CE = true;
						Unknown088->Go_Next_Step();
						return true;
					}
					if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
						Unknown084->Unknown_67AE60(1);
					}
					Unknown0FD = true;
					Unknown_55EF90();
				}
				return true;
			case 8:
				SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
				if (Unknown096) {
					if (Unknown0BC[Unknown090]) {
						Unknown0BC[Unknown090]->Unknown_784A50("/normal", 0, 0);
					}
					Unknown090 = -1;
					Unknown_8C();
					return true;
				}
				Unknown08C = 8;
				if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
					Unknown084->Unknown_67AE60(1);
				}
				Unknown0FD = true;
				Unknown_55EF90();
				return true;
			case 10:
				SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
				Unknown084->Unknown_67B270();
				Unknown0F9 = true;
				Unknown_55EF90();
				return true;
			default:
				return false;
		}
		Unknown_55ED20();
		if (Unknown090 != previous) {
			Unknown_55E8E0();
		}
		return true;
	} else {
		SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
		switch (key) {
			case 1: case 8:
				Unknown11C = "";
				Unknown_55FBB0();
				Unknown_55ED20();
				return false;
			case 4:
				Unknown114->Unknown_74(1);
				return false;
			case 2:
				{
					char *text = Unknown114->Get_Text();
					Unknown11C = text;
					Unknown11C.Trim();
					delete text;
				}
				if (strcmp(Unknown11C, "") == 0) {
					Unknown084->Unknown034->Unknown_588A70("OK", 0, "Please enter a valid EA Member Name.", 0x140, 0, 0);
					return false;
				}
				if (stricmp(Unknown11C, Unknown088->Ticket_Info.EA_Customer.c_str()) == 0) {
					Unknown084->Unknown034->Unknown_588A70("OK", 0, "Sorry, you cannot refer yourself.", 0x140, 0, 0);
					return false;
				}
				Unknown_55FBB0();
				Unknown088->Send_Referrer(Unknown11C);
				Unknown120 = true;
				return false;
		}
		return false;
	}
}

// 0x55D9C0
void AvatarSelectionInterface::Unknown_55D9C0()
{
	if (Unknown120 && Unknown088->Referrer_Result) {
		switch (Unknown088->Referrer_Result) {
			case 2:
				Unknown084->Unknown034->Unknown_588A70("OK", 0, "Sorry, you have already been referred by someone.  You cannot be referred more than once.", 0x140, 0, 0);
				break;
			case 3:
				Unknown084->Unknown034->Unknown_588A70("OK", 0, "Sorry, that account does not exist.  Please check the name and try again.", 0x140, 0, 0);
				break;
			case 4:
				Unknown084->Unknown034->Unknown_588A70("OK", 0, "A system error occurred.  Please try again later.", 0x140, 0, 0);
				break;
			case 5:
				Unknown084->Unknown034->Unknown_588A70("OK", 0, "Sorry, your account is about to expire.", 0x140, 0, 0);
				break;
			case 6:
				Unknown084->Unknown034->Unknown_588A70("OK", 0, "Sorry, you cannot refer yourself.", 0x140, 0, 0);
				break;
			case 1:
				Unknown084->Unknown034->Unknown_588A70("OK", 0, "Your referral was registered successfully.  The person who referred you has received credit for your referral.", 0x140, 0, 0);
				break;
		}
		if (!Unknown088->Unknown1138) {
			Unknown118 = NULL;
			Unknown_55FE50(false);
		}
		Unknown_55ED20();
		Unknown120 = false;
	}
	if (Unknown095 || Unknown094) {
		Unknown_55EF90();
	}
	for (int i = 0; i < 5; i++) {
		if (Unknown0BC[i]) {
			((UIAvselAvatarVirtualsClass *)Unknown0BC[i])->Unknown_1C4();
		}
	}
	if (Unknown095 && Unknown084->Unknown_67AEA0(1)) {
		Unknown095 = false;
		Unknown096 = true;
		Unknown0BC[Unknown090]->Unknown_784A50("/happy at your service", 1, 0);
		Unknown_55ED20();
	}
	if (Unknown094 && Unknown084->Unknown_67AEA0(1)) {
		Unknown094 = false;
		Unknown_55FE50(true);
		if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-10")) {
			Unknown084->Unknown_67ADD0(1);
		}
		if (Unknown090 < 0) {
			Unknown_55ED20();
			Unknown_55E8E0();
		} else if (Unknown08C != 7 && Unknown08C != 8) {
			if (Unknown0E4[Unknown090]) {
				Unknown_88();
				Unknown_55EF90();
			} else if (Unknown090 < 5 && Unknown098) {
				Unknown08C = 7;
				if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
					Unknown084->Unknown_67AE60(1);
				}
				Unknown0FD = true;
				Unknown_55EF90();
			} else {
				Unknown090 = -1;
				Unknown08C = 0;
				Unknown_55ED20();
			}
		} else {
			if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
				Unknown084->Unknown_67AE60(1);
			}
			Unknown0FD = true;
			Unknown_55EF90();
		}
	}
	if (Unknown088->Unknown001D) {
		Unknown_55E370(false);
		Unknown088->Unknown001D = false;
		Unknown_55EF90();
		Unknown0F0 = 3.0f;
	}
	if (Unknown0FA && Unknown084->Unknown_67AEA0(0)) {
		Unknown0FA = false;
	}
	if (Unknown0FB && Unknown084->Unknown_67AEA0(0)) {
		Unknown0FB = false;
	}
	if (!Unknown0FA && Unknown0FC && Unknown084->Unknown_67AEA0(1)) {
		Unknown0FC = false;
		Unknown_55ED20();
		if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-10")) {
			Unknown084->Unknown_67ADD0(1);
		}
	}
	if (!Unknown0FB && Unknown0FD && Unknown084->Unknown_67AEA0(1)) {
		Unknown084->Unknown_67ACE0(1);
		Unknown084->Unknown1CD = true;
		Unknown084->Unknown1CE = true;
		if (Unknown0F4 > -1) {
			Unknown088->Selected = Unknown0F4;
			Unknown088->Unknown_50();
			Unknown0F4 = -1;
		}
		if (Unknown08C == 7) {
			Unknown088->Go_Next_Step();
		} else {
			Unknown088->Go_Back_Step();
		}
	}
	if (Unknown084->Unknown_67B2B0()) {
		Unknown0F9 = false;
		if (!Unknown095 && !Unknown094) {
			Unknown_55ED20();
		}
	}
	if (Unknown097) {
		int answer = Unknown084->Unknown034->Unknown_5890B0("YES_NO");
		if (answer) {
			Unknown084->Unknown034->Unknown_5C();
			Unknown0F9 = false;
			Unknown097 = false;
			if (answer == 1) {
				Unknown0E4[Unknown090] = false;
				Unknown0BC[Unknown090]->Unknown_784A50("you'll pay for this", 1, 0);
				Unknown0BC[Unknown090]->Unknown_7896B0(1);
				Unknown0EC = 3.2f;
				Unknown0F4 = Unknown090;
				Unknown_8C();
			} else {
				Unknown0BC[Unknown090]->Unknown_784A50("/happy yeah", 1, 0);
				Unknown_55ED20();
			}
		}
	}
	if (Unknown084->Unknown034->Active && !Unknown0F9) {
		Unknown0F9 = true;
		Unknown_55EF90();
	} else if (!Unknown084->Unknown034->Active && Unknown0F9) {
		Unknown0F9 = false;
		if (!Unknown095 && !Unknown094) {
			Unknown_55ED20();
		}
		if (Unknown08C == 7) {
			Unknown08C = 0;
		}
	}
}

// NOT EXACT: register (0.78; the else branch's vtable load in eax, the client's in edx)
// 0x55E070
void AvatarSelectionInterface::Unknown_55E070()
{
	bool highlighted = false;
	int highlight = -1;
	RenderObjClass *robj;
	if (!Unknown0F8 && !Unknown0F9 && !Unknown0FA && !Unknown0FB && !Unknown0FC && !Unknown0FD && Unknown0F4 == -1 &&
		Unknown0F0 == 0.0f && !Unknown095 && !Unknown094 && !BuddyReferralUI) {
		float x, y;
		Unknown_42ECF0(&x, &y);
		for (int i = 0; i < 5; i++) {
			if (Unknown0D0[i] && Unknown0D0[i]->Unknown_30(x, y) && Unknown090 != i && y < 0.85f) {
				highlighted = true;
				for (int j = 0; j < 5; j++) {
					GadgetClass *gadget = Unknown0D0[j];
					if (j == i) {
						if (gadget) {
							robj = gadget->Unknown_08()->Unknown30;
							Unknown_496490(robj, 0);
							highlight = j;
						}
					} else if (gadget) {
						robj = gadget->Unknown_08()->Unknown30;
						Unknown_496110(robj, 0.0f, 0.0f, 0.0f);
					}
				}
			}
		}
	}
	if (!highlighted) {
		Unknown100 = -1;
		for (int i = 0; i < 5; i++) {
			if (Unknown0D0[i]) {
				robj = Unknown0D0[i]->Unknown_08()->Unknown30;
				Unknown_496110(robj, 0.0f, 0.0f, 0.0f);
			}
		}
	} else if (Unknown100 != highlight) {
		Unknown100 = highlight;
		SoundLibraryClass::Play_2D_Sound("Avatar_Highlight", 2);
	}
}

// 0x55E2E0
void AvatarSelectionInterface::Unknown_55E2E0(int text, const char *string, bool selected)
{
	FontSys *textptr = Unknown_BE67C4->Get_Style(Unknown_B7B904[text], Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Change_Text(string, 0);
	textptr->Unknown_6CE910(selected);
}




// 0x55E370
void AvatarSelectionInterface::Unknown_55E370(bool active)
{
	LoginCharacterClass *chars = Unknown088->Characters;
	AvatarDescClass *descs = Unknown088->Unknown0350;
	G_ASSERT(chars);
	TheDX8MeshRenderer.Invalidate();
	if (!active) {
		InterfaceClass::Unknown_5C();
	}
	for (int i = 0; i < 5; i++) {
		if (!Unknown0E4[i]) {
			if (Unknown0BC[i]) {
				Unknown0BC[i]->Remove();
				Unknown0BC[i]->Release_Ref();
				Unknown0BC[i] = NULL;
			}
			if (Unknown0D0[i]) {
				Unknown64.Unknown_687150(Unknown0D0[i]);
				Unknown_65EED0(Unknown0D0[i]);
				Unknown0D0[i] = NULL;
			}
			if (chars[i].ID != 0) {
				Unknown_55E2E0(i, descs[i].Get_Full_Name().c_str(), i == Unknown090);
				Unknown0E4[i] = true;
				AvatarDescClass *desc = new AvatarDescClass(descs[i]);
				Unknown0BC[i] = new UIAvselAvatarClass(desc, 1, 0, Unknown084->Get_Unknown3A4());
				if (Unknown0BC[i]) {
					char name[64];
					sprintf(name, "DUDE%02d", i);
					Unknown084->Unknown1BC->Unknown_80(Unknown0BC[i], name);
					Unknown0BC[i]->Unknown_77E3D0(1);
					Unknown0D0[i] = new UIAvselButtonClass(Unknown0BC[i]->Get_Unknown2BC(), Unknown084->Get_Unknown070(), 0x55, 0);
					Unknown0D0[i]->Unknown2C |= 0x400;
					Unknown0D0[i]->Unknown_660070(1);
					Unknown64.Unknown_6870E0(Unknown0D0[i], i + 11, View, 1, 1);
				}
				delete desc;
			} else {
				Unknown_55E2E0(i, "New Character", i == Unknown090);
				Unknown0E4[i] = false;
			}
		}
	}
	if (!active) {
		InterfaceClass::Unknown_58();
	}
}

// 0x55E8E0
void AvatarSelectionInterface::Unknown_55E8E0()
{
	AvatarDescClass *descs = Unknown088->Unknown0350;
	Unknown_55ED20();
	for (int i = 0; i < 5; i++) {
		if (Unknown0E4[i]) {
			if (i == Unknown090) {
				if (Unknown096) {
					Unknown_8C();
				} else {
					Unknown_88();
				}
			}
			Unknown_55E2E0(i, descs[i].Get_Full_Name().c_str(), i == Unknown090);
		} else {
			if (i == Unknown090) {
				Unknown08C = 7;
				Unknown088->Selected = Unknown090;
				if (Unknown096) {
					Unknown098 = true;
					Unknown_8C();
				} else {
					if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
						Unknown084->Unknown_67AE60(1);
					}
					Unknown0FD = true;
					Unknown_55EF90();
				}
			}
			Unknown_55E2E0(i, "New Character", i == Unknown090);
		}
	}
}

// 0x55EBD0
void AvatarSelectionInterface::Unknown_88()
{
	if (Unknown090 >= 0 && Unknown0E4[Unknown090]) {
		Unknown_55FE50(false);
		char name[32];
		sprintf(name, "FE01C.FE01C-%d", Unknown090 + 5);
		if (Unknown084->Unknown_67AD30(1, name)) {
			Unknown084->Unknown_67AE20(1);
			Unknown095 = true;
			Unknown_55EF90();
			Unknown0F9 = true;
			Unknown084->Unknown_67AD30(0, "FE12A.FE12A-1");
			Unknown084->Unknown_67AE20(0);
		}
		Unknown_55F140();
	}
}

// 0x55ECB0
void AvatarSelectionInterface::Unknown_8C()
{
	if (Unknown084->Unknown1E4) {
		Unknown084->Unknown_67AE60(1);
		Unknown094 = true;
		Unknown096 = false;
		Unknown_55EF90();
		Unknown0F9 = true;
		Unknown084->Unknown_67AE60(0);
	}
}

// 0x55ED20
void AvatarSelectionInterface::Unknown_55ED20()
{
	int i;
	FontSys *text;
	if (BuddyReferralUI) {
		for (i = 0; i < 2; i++) {
			if (Unknown10C[i]) {
				Unknown10C[i]->Unknown_40(true);
				text = Unknown_BE67C4->Get_Style(Unknown_B7B960[i], BuddyReferralUI.Peek());
				if (text) {
					text->Set_Unknown2FC(1.0f);
				}
			}
		}
		return;
	}
	for (i = 0; i < 8; i++) {
		if (Unknown09C[i] && !Unknown09C[i]->Unknown_60()) {
			Unknown09C[i]->Unknown_40(true);
			text = Unknown_BE67C4->Get_Style(Unknown_B7B904[i], Unknown084->Peek_Model());
			if (text) {
				text->Set_Unknown2FC(1.0f);
			}
		}
	}
	if (Unknown118 && !Unknown118->Unknown_60()) {
		Unknown118->Unknown_40(true);
		text = Unknown_BE67C4->Get_Style("fe12_ReferralButton", Unknown084->Peek_Model());
		if (text) {
			text->Set_Unknown2FC(1.0f);
		}
		text = Unknown_BE67C4->Get_Style("fe12_ReferralText", Unknown084->Peek_Model());
		if (text) {
			text->Set_Unknown2FC(1.0f);
		}
	}
	if (Unknown090 < 0 || !Unknown0E4[Unknown090]) {
		Unknown09C[5]->Unknown_40(false);
		text = Unknown_BE67C4->Get_Style(Unknown_B7B904[5], Unknown084->Peek_Model());
		if (text) {
			text->Set_Unknown2FC(0.33f);
		}
	}
	if (Unknown090 < 0) {
		Unknown09C[6]->Unknown_40(false);
		text = Unknown_BE67C4->Get_Style(Unknown_B7B904[6], Unknown084->Peek_Model());
		if (text) {
			text->Set_Unknown2FC(0.33f);
		}
	}
	Unknown0F9 = false;
}

// 0x55EF90
void AvatarSelectionInterface::Unknown_55EF90()
{
	int i;
	FontSys *text;
	if (BuddyReferralUI) {
		for (i = 0; i < 2; i++) {
			if (Unknown10C[i]) {
				Unknown10C[i]->Unknown_40(false);
				text = Unknown_BE67C4->Get_Style(Unknown_B7B960[i], BuddyReferralUI.Peek());
				if (text) {
					text->Set_Unknown2FC(0.33f);
				}
			}
		}
		return;
	}
	for (i = 0; i < 8; i++) {
		if (Unknown09C[i] && Unknown09C[i]->Unknown_60()) {
			Unknown09C[i]->Unknown_40(false);
			text = Unknown_BE67C4->Get_Style(Unknown_B7B904[i], Unknown084->Peek_Model());
			if (text) {
				text->Set_Unknown2FC(0.33f);
			}
		}
	}
	if (Unknown118 && Unknown118->Unknown_60()) {
		Unknown118->Unknown_40(false);
		text = Unknown_BE67C4->Get_Style("fe12_ReferralButton", Unknown084->Peek_Model());
		if (text) {
			text->Set_Unknown2FC(0.33f);
		}
		text = Unknown_BE67C4->Get_Style("fe12_ReferralText", Unknown084->Peek_Model());
		if (text) {
			text->Set_Unknown2FC(0.33f);
		}
	}
}





















































// NOT EXACT: inline budget (0.59; the client erases the galaxies' list out of line on the FAIL_IF's return, and keeps other registers)
// 0x55F140
void AvatarSelectionInterface::Unknown_55F140()
{
	LoginCharacterClass *chars = Unknown088->Characters;
	if (Unknown090 == -1 || !Unknown0E4[Unknown090]) {
		return;
	}
	FontSys *textptr = Unknown_BE67C4->Get_Style("fe12_Race_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	int race = Unknown0BC[Unknown090]->Unknown108;
	textptr->Change_Text(Unknown_B7B928[race], 0);


	textptr = Unknown_BE67C4->Get_Style("fe12_Prof_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Change_Text(Unknown_B7B934[Unknown0BC[Unknown090]->Unknown10C + race * 3], 0);



	textptr = Unknown_BE67C4->Get_Style("fe12_Level_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	char level[64];
	sprintf(level, "Level: %d", chars[Unknown090].Level_Combat + chars[Unknown090].Level_Explore + chars[Unknown090].Level_Trade);
	textptr->Change_Text(level, 0);

	UIAvselManagedFontClass *font = Unknown_BE67C4->Find((unsigned int)Unknown084->Peek_Model());
	if (font) {
		const char *sector = chars[Unknown090].Sector_Name;
		if (strlen(sector)) {
			char location[0x78];
			_snprintf(location, sizeof(location), "Location: %s", sector);
			font->Unknown_6D0A70("WRAP_fe12_Location_Title", location);
		} else {
			font->Unknown_6D0A70("WRAP_fe12_Location_Title", "");
		}
	}











	std::list<LoginGalaxyClass> galaxies = Unknown088->Galaxies;
	textptr = Unknown_BE67C4->Get_Style("fe12_Server_Title", Unknown084->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Change_Text("", 0);
	for (std::list<LoginGalaxyClass>::iterator it = galaxies.begin(); it != galaxies.end(); it++) {
		if ((*it).ID == chars[Unknown090].Galaxy_ID && strlen((*it).Name)) {
			float capacity = (unsigned int)(*it).Unknown58 * 0.9f;
			if (capacity == 0.0f) {
				capacity = 1.0f;
			}
			float full = (unsigned int)(*it).Unknown54 / capacity * 100.0f;
			char galaxy[0x68];
			if (full >= 100.0f) {
				_snprintf(galaxy, sizeof(galaxy), "Galaxy: %s (FULL)", (*it).Name);
			} else {
				_snprintf(galaxy, sizeof(galaxy), "Galaxy: %s (%.0f%% full)", (*it).Name, full);
			}
			textptr->Change_Text(galaxy, 0);
		}
	}
}











// 0x55F680
void AvatarSelectionInterface::Unknown_55F680()
{
	if (Unknown084->Unknown_67AD30(1, "FE01C.FE01C-1")) {
		Unknown084->Unknown_67AE60(1);
	}
	Unknown0FD = true;
	Unknown_55EF90();
}

// 0x55F6D0
void AvatarSelectionInterface::Unknown_55F6D0(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event)
		return;

	GadgetClass *gadget = event->Gadget;
	FAIL_IF(!gadget)
		return;
	if (gadget->Unknown20 && gadget->Is_Unknown2C_2()) {
		SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
	}
}












// 0x55F760
void AvatarSelectionInterface::Unknown_55F760()
{
	Unknown_5C();
	Unknown11C = "";
	RenderObjClass *model = Unknown084->Peek_Model();
	BuddyReferralUI = Create_Get(Unknown_BFAEE0->Create_Render_Obj("BUDDYREF1"));
	G_ASSERT(BuddyReferralUI);
	model->Add_Sub_Object(BuddyReferralUI.Peek());
	((FontStringFilesClass *)Unknown_BE67BC)->Load("fc_buddyref1.ini");
	Unknown_BE67C4->Add(BuddyReferralUI);
	for (int i = 0; i < 2; i++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(BuddyReferralUI->Get_Sub_Object_By_Name(Unknown_B7B958[i]));
		Unknown_496AE0(robj.Peek(), 0);
		Unknown10C[i] = new UIAvselButtonClass(robj.Peek(), Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown10C[i], i + 1, View, 1, 0);
	}
	Unknown114 = new UIAvselEditClass(this, 20, "ENTRY_NAME_BUDDYREF1", BuddyReferralUI.Peek(), Unknown_B08CC0 | Unknown_B08CBC | Unknown_B08CAC, 0);
	Unknown114->Unknown_28(Layer);
	Unknown114->Unknown4C->Unknown_6E2A20(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	Unknown114->Set_Text("", 0);
	Unknown114->Unknown5C = false;
	Unknown114->Unknown3B = true;
	Unknown114->Unknown_74(1);
	Unknown64.Unknown_6870E0(Unknown114, 4, View, 1, 0);
	((UIAvselKeysClass *)Unknown4C)->Unknown_721E60(&UIAvselKeyEventStruct(0x0D), 2);
	((UIAvselKeysClass *)Unknown4C)->Unknown_721E60(&UIAvselKeyEventStruct(0x1B), 1);
	InterfaceClass::Unknown_58();
	Unknown_58();
}



















// 0x55FBB0
void AvatarSelectionInterface::Unknown_55FBB0()
{
	Unknown_5C();
	Unknown64.Unknown_687150(Unknown114);
	Unknown114 = NULL;
	for (int i = 0; i < 2; i++) {
		Unknown64.Unknown_687150(Unknown10C[i]);
		Unknown10C[i] = NULL;
	}
	Unknown_BE67C4->Remove((unsigned int)BuddyReferralUI.Peek());
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fc_buddyref1.ini");
	RenderObjClass *model = Unknown084->Peek_Model();
	G_ASSERT(BuddyReferralUI);
	model->Remove_Sub_Object(BuddyReferralUI.Peek());
	BuddyReferralUI = NULL;
	((UIAvselKeysClass *)Unknown4C)->Unknown_721EF0(&UIAvselKeyEventStruct(0x0D));
	((UIAvselKeysClass *)Unknown4C)->Unknown_721EF0(&UIAvselKeyEventStruct(0x1B));
	((UIAvselKeysClass *)Unknown4C)->Unknown_721E60(&UIAvselKeyEventStruct(0x0D), 7);
	((UIAvselKeysClass *)Unknown4C)->Unknown_721E60(&UIAvselKeyEventStruct(0x1B), 10);
	InterfaceClass::Unknown_58();
	Unknown_58();
}









// 0x55FDC0
void AvatarSelectionInterface::Unknown_55FDC0(bool on)
{
	for (int i = 0; i < 2; i++) {
		if (Unknown10C[i]) {
			Unknown10C[i]->Unknown_40(on);
			FontSys *text = Unknown_BE67C4->Get_Style(Unknown_B7B960[i], BuddyReferralUI.Peek());
			if (text) {
				text->Set_Unknown2FC(on ? 1.0f : 0.33f);
			}
		}
	}
}

// 0x55FE50
void AvatarSelectionInterface::Unknown_55FE50(bool on)
{
	int hidden = !on;
	if (!Unknown088->Unknown1138) hidden = true;
	RefCountPtr<RenderObjClass> robj = Create_Get(Unknown084->Peek_Model()->Get_Sub_Object_By_Name("click_me"));
	G_ASSERT(robj);
	robj->Set_Hidden(hidden);

	FontSys *textptr = Unknown_BE67C4->Get_Style("fe12_ReferralText", Unknown084->Peek_Model());
	G_ASSERT(textptr);
	textptr->Set_Hidden(hidden);

	textptr = Unknown_BE67C4->Get_Style("fe12_ReferralButton", Unknown084->Peek_Model());
	G_ASSERT(textptr);
	textptr->Set_Hidden(hidden);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avsel.cpp (its __FILE__, 0xB7BEA0: 15 asserts, lines
**	769-1262)
**
**	The character select window, AvatarSelectionInterface (the client's RTTI: the type name of its
**	FlashingGadgetEvent callback, 0xB7C068, sits in this file's literals), charview.cpp's step 2 (0x679160).
**	Its base is intrface.cpp's InterfaceClass (the constructor calls 0x540A90 itself and puts no mesh at
**	+0x84, so not sclient.h's Unknown5BB320Class). The file's code starts at the constructor 0x55C650:
**	0x55C490-0x55C640 before it are ui_avatr.cpp's (0x55C630 / 0x55C640 the adjustor thunks into
**	AvatarCreationInterface's deleting destructor).
**
**	So far (LG2, claim in run PR #139): the constructor 0x55C650 (the 2-entry +0x10C cleared in a loop,
**	the camera's two vectors as temporaries, the key events' +4 / +8 floats), the destructor 0x55CCC0
**	(0x55CC90 its deleting one), slot +0x60 0x55CEA0 (every frame), +0x88 0x55EBD0, +0x8C 0x55ECB0,
**	0x55E2E0 (a text of Unknown_B7B904 changed: FAIL_IF line 769; inlined in its callers), 0x55F680, the
**	FlashingGadgetEvent callback 0x55F6D0 (the gadget's +0x2C bit 2 read through a bool getter of a
**	bitfield: the client shifts and tests), 0x55FDC0 and 0x55FE50 (the referral button; the hidden flag
**	set to !on, then to true without the task's +0x1138) exact; 0x55E8E0 (the five names, 0x55E2E0
**	inlined), 0x55ED20 / 0x55EF90 (the buttons lit / dimmed), 0x55F760 (the referral dialog opened:
**	G_ASSERT line 1160; its edit box is textgadg.cpp's) and 0x55FBB0 (closed: line 1215) exact. NOT
**	EXACT (register): 0x55E070 (the avatar under the pointer highlighted, 0.78). 0x55E370 (the
**	slots' avatars made, G_ASSERT 'chars' line 782) exact through inline accessors (the view's +0x3A4 and
**	+0x70, the avatar's +0x2BC): read directly, the arguments land in other registers. 0x55D9C0 (every frame: the referral's
**	result boxes, the emotes, the delete question's answer) exact; slot +0x68 0x55D060 (the buttons
**	1-17 and the referral dialog's) NOT EXACT (0.49): it assigns the selected character's ship
**	descriptor, and the client calls ShipCustDescClass's implicit operator= out of line (0x55D8A0)
**	where shipcust_desc.h's written-out one (P-P7-1) inlines; 0x55F140 (the selected character's
**	texts: race, profession, level, location, galaxy; FAIL_IFs 1047 / 1053 / 1059 / 1088; the
**	galaxies' list copied) NOT EXACT (inline budget, 0.59). The class is declared in ui_avsel.h (a
**	provisional name: with it in this file the functions before line 769 do not fit above it), the
**	other types only as far as these functions use them.
*/
