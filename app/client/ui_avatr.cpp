// Earth & Beyond client (reconstruction): app\Client\ui_avatr.cpp. Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "refcount.h"
#include "random.h"
#include "camera.h"
#include "layer.h"
#include "dx8renderer.h"
#include "..\audio\soundlib.h"
#include "tutorial_manager.h"
#include "local_interface_sort.h"
#include <new>
#include "aabox.h"
#include "..\shared\avatar_desc.h"
#include "avatar.h"
class SignalMap; struct UIAvatrGadgetUnknown08Struct { char Unknown00[0x30]; RenderObjClass *Unknown30; };	// gadget.h's slot +0x08 return: the model at +0x30  // NAME 1: neutral (D10)
class AABoxClass;
// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual UIAvatrGadgetUnknown08Struct *Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60();
	virtual void Unknown_64(AABoxClass &box);			// +0x64 (gadget.h's: the box)  // NAME 1 [box]: gadget.h's
	virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral
	static void Destroy(GadgetClass *del_me);			// 0x65EED0 (gadget.h's)  // NAME 2 [del_me]: gadget.h's

	static SignalMap &Get_Signal_Map();					// 0x65ECE0 (gadget.h's)
	void Unknown_65FFC0(int arg, GadgetClass *gadget);	// 0x65FFC0 (gadget.h's)  // NAME 1 [arg, gadget]: gadget.h's
	char Unknown04[0x2C - 0x04];						// +0x04
	unsigned int Unknown2C;								// +0x2C (0x556290 sets 0x400)
};

// The pulls' gadget (0x80 bytes, constructor 0x671300), as far as 0x556290 makes it
class UIAvatrGadget671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_671630();	UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d);	// 0x671630 (the buttons' flags off); 0x671300 (0x556290: the area model, 0, the window's mesh, 0, 0, 0)  // NAME 1 [area, a, mesh, b, c, d]: neutral

	char Unknown30[0x64 - 0x30]; bool Unknown64; bool Unknown65; char Unknown66[0x80 - 0x66];	// +0x30; +0x64 / +0x65 the left / right button held (0x671570)
};

// A gadget with its pull model (0x10 bytes, constructor 0x585920; the window's +0x160 / +0x16C / +0x180)
class UIAvatrPullClass	// NAME 1: neutral (D10)
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950 (the window's destructor)
	void Unknown_5859A0();								// 0x5859A0 (0x556290, once the gadget is in)
	void Show(bool on)		// NAME 1: neutral (D10): inline (0x556630)
	{
		Gadget->Unknown_40(on);
		Gadget->Unknown_8C(!on);
		Pull->Set_Hidden(!on);
	}

	GadgetClass *Gadget;								// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04 ('BODY_PULL' / 'HEAD_PULL', 0x556290)  // NAME 2: what 0x556290 puts there
	bool Unknown08;										// +0x08
	float Unknown0C;									// +0x0C (0.5)
};

// What 0x5860B0 returns (avatar.cpp's neighbourhood: five blend weights from a point in 0 .. 1)
struct UIAvatrWeightsStruct	// NAME 1: neutral (D10)
{
	UIAvatrWeightsStruct() {}
	float Weight[5];									// NAME 1: neutral
};
UIAvatrWeightsStruct Unknown_5860B0(Vector2 uv);		// 0x5860B0 (cdecl)  // NAME 1 [Unknown_5860B0, uv]: neutral (D10)
extern RandomClass Unknown_BDBB84; void Unknown_42ECF0(float *x, float *y); void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color);	// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's; 0x42ECF0 (the pointer, 0..1), 0x496C00 (ro_funcs: the texture's colour at u, v)  // NAME 1 [x, y, robj, u, v, color]: as gadget.cpp's / ro_funcs.cpp's

// The +0x8C object's +0x194 is avatar.h's CharacterViewAvatarClass (the view's AvatarRObj)

































// The +0x8C object's +0x34, as far as 0x554D60 reads it
struct UIAvatrUnknown08CUnknown034Struct	// NAME 1: neutral (D10)
{
	char Unknown00[0x7C];								// +0x00
	bool Unknown7C;										// +0x7C (0x554D60)
};

// tsk_login.h's LoginTaskClass (the window's +0x90), as far as this file calls it (tsk_login.h includes charview.h,
// which this file declares as far as it uses it)
class UIAvatrLoginTaskClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50();
	virtual void Go_Next_Step();						// +0x54 (tsk_login.h's)
	virtual void Go_Back_Step();						// +0x58 (tsk_login.h's)
	char Unknown04[0x28 - 0x04];						// +0x04
	int Previous_Step;									// +0x28 (tsk_login.h's: 6 when the ship screen sent us back)
};

// charview.h's CharacterViewClass (the window's +0x8C), as far as this file uses it: the same class, declared here
// because charview.h changes this file's code (0x556630, 0x55AFD0, 0x55B110 lost their match with it)
class CharacterViewClass	// NAME 1 [CharacterViewClass]: charview.h's
{
public:
	void Unknown_67A9B0(int type);						// 0x67A9B0 (the window's destructor: 0)
	bool Unknown_67B2B0();								// 0x67B2B0 (0x554D60: true clears the window's +0x1C5)
	void Unknown_67A250();								// 0x67A250
	void Unknown_67ACE0(int type);						// 0x67ACE0
	void Unknown_67AB20(int type, const char *name);	// 0x67AB20
	void Unknown_67A290(const Vector3 &position, const Vector3 &rotation, float fov);	// 0x67A290
	void Unknown_67A3F0();								// 0x67A3F0 (+0x194 released)
	void Unknown_67A440(AvatarDescClass *desc, bool copy);	// 0x67A440
	void Unknown_67A670();								// 0x67A670 (+0x198 released)
	void Unknown_67B270();								// 0x67B270 ('Are you sure you want to quit?')
	void Unknown_67B4F0(bool on);						// 0x67B4F0
	void Unknown_67C200(Vector2 *offsets, int size, int which);	// 0x67C200
	void Unknown_67C2E0(Vector2 *out, int index);		// 0x67C2E0
	void Unknown_67C340(Vector3 *out, int index);		// 0x67C340
	void Unknown_67C270(const Vector2 *offsets, int size, int which);	// 0x67C270 (charview.h's: the offsets in)
	void Unknown_67C310(const Vector2 *in, int index);	// 0x67C310
	void Unknown_67C380(const Vector3 *in, int index);	// 0x67C380
	void Unknown_5594B0(AvatarDescClass desc);			// 0x5594B0 (out of line in this file: +0x88 = desc; ui_avsel.cpp's too)  // NAME 1 [desc]: neutral
	void Unknown_67C7B0();								// 0x67C7B0
	void Unknown_67C920(std::string name);				// 0x67C920
	char Unknown00[0x14];								// +0x00
	ClientTaskClass *Unknown14;							// +0x14 (UIViewClass's: the task)
	char Unknown18[0x34 - 0x18];						// +0x18
	UIAvatrUnknown08CUnknown034Struct *Unknown034;		// +0x34
	char Unknown38[0x68 - 0x38];						// +0x38
	CameraClass *Unknown068;							// +0x68
	char Unknown6C[0x88 - 0x6C];						// +0x6C
	AvatarDescClass Unknown088;							// +0x88
	CharacterViewAvatarClass *Peek_Unknown194() { return(Unknown194); }	// NAME 1: neutral (D10)
	CharacterViewAvatarClass *Unknown194;					// +0x194 (AvatarRObj)
	char Unknown198[0x1B8 - 0x198];						// +0x198
	RenderObjClass *Peek_Model() { return(Unknown1B8); }	// NAME 1: neutral (D10)
	RenderObjClass *Unknown1B8;							// +0x1B8 (ViewRObjs[0], the model: its texts by name)
	RenderObjClass *Unknown1BC;							// +0x1BC (ViewRObjs[1])
	char Unknown1C0[0x1CD - 0x1C0];						// +0x1C0
	bool Unknown1CD;									// +0x1CD (Unknown1CC[1])
	char Unknown1CE[0x3A4 - 0x1CE];						// +0x1CE
	EnvironmentMapManagerClass *Unknown3A4;				// +0x3A4 'envmaps.ini'
};

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

// The asset manager (*0xBFAEE0)
#include "assetmgr.h"	// WW3DAssetManager: 0xBFAEE0 is WW3DAssetManager::TheInstance (its constructor 0x91C2C0 stores it)









// The +0x94 object's +0x54, as far as 0x556B20 writes it
class UIAvatrUnknown094Unknown054Struct	// NAME 1: neutral (D10)
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
	virtual void Unknown_1C4();							// +0x1C4 (0x554D60, last)

	char Unknown004[0x214 - 0x004];						// +0x004
	bool Unknown214;									// +0x214 (0x556B20 sets it)
	char Unknown215[0x231 - 0x215];						// +0x215
	bool Unknown231;									// +0x231 (set on the head tab)
};

// The window's +0x94: ship_pip.cpp's gadget (0xF4 bytes, vtable 0xB07DF4), as far as this file calls it
class ShipPipClass	// NAME 3: class-from-file (D24) for app\client\ship_pip.cpp
{
public:
	ShipPipClass(int type);								// 0x80B2E0 (the constructor passes 1)  // NAME 1 [type]: neutral
	bool Unknown_80C220(ViewClass *view);		// 0x80C220 (FAIL_IF '!PIPView': its scene, camera and FloatParent)  // NAME 1 [view]: neutral
	void Unknown_80C540(const Vector2 &size, const Vector3 &offset, unsigned int sort_level);	// 0x80C540 (the constructor: (640, 480), (0, 0, 0), 0x1C)  // NAME 1 [size, offset, a]: neutral
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (0x554D60)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70();
	virtual void Unknown_74(int race);					// +0x74 0x80BAB0 (the lights by race)  // NAME 1 [race]: neutral
	virtual void Unknown_78();							// +0x78 (0x554D60)
	virtual void Unknown_7C(const RefCountPtr<CharacterViewAvatarClass> &avatar);	// +0x7C 0x80C640 (the avatar into +0x54)  // NAME 1 [avatar]: neutral
	virtual void Unknown_80();
	virtual void Unknown_84(const RefCountPtr<CharacterViewAvatarClass> &avatar);	// +0x84 0x80CA30  // NAME 1 [avatar]: neutral
	virtual void Unknown_88();							// +0x88 (0x556B20, after +0xDC)
	void Unknown_80C180();								// 0x80C180 (the window's destructor, after +0x80)
	virtual void Unknown_8C();							// +0x8C

	char Unknown04[0x54 - 0x04];						// +0x04
	UIAvatrUnknown094Unknown054Struct *Unknown054;		// +0x54
	char Unknown58[0xDC - 0x58];						// +0x58
	bool Unknown0DC;									// +0xDC (set on the head tab)
	char Unknown0DD[0xF4 - 0xDD];						// +0xDD (ship_pip.h's: 0xF4 bytes; the constructor writes +0xEC / +0xF0)
};

// fontmanager.h's FontManagerClass at 0xBE67C4 (its header cannot be included next to sclient.h: workflow.md 3.3)
class FontSys : public RenderObjClass { public: void Change_Text(const char *text, int unknown); };	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)  // NAME 1 [text, unknown]: as loadingview.cpp's
class FontManagerClass { public: void Add(RenderObjClass *object); void Remove(RenderObjClass *object); FontSys *Get_Style(const char *name, RenderObjClass *object); };	// NAME 1: neutral (D10): fontmanager.h's Get_Style  // NAME 1 [name, object]: as loadingview.cpp's
extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
// fontdatabase.h's FontStringFilesClass at 0xBE67BC (sclient.h declares the pointer), as ui_cpit.cpp does
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's

// The gadget events' callbacks (SignalBaseClass's, unknown_signalbase.h: vtable 0xAEDB58) and what 0x65ECE0 returns
#include "unknown_signalbase.h"
typedef SignalBaseClass::CallbackClass UIAvatrCallbackBaseClass;	// NAME 1 [UIAvatrCallbackBaseClass]: neutral (D10)
class FlashingGadgetEvent;	// the client's RTTI (.?AVFlashingGadgetEvent@@)



class UIAvatrNotifierClass		// NAME 1: neutral (D10)
{
public:
	void Remove(int id, UIAvatrCallbackBaseClass *callback);	// 0x4F98D0  // NAME 2: as sclient.cpp's
};
UIAvatrNotifierClass *Unknown_65ECE0();					// 0x65ECE0  // NAME 1 [Unknown_65ECE0]: neutral (D10)

// The window's buttons and their texts (0xB7AF84: 50 entries, the gadgets' order at +0x98)
struct UIAvatrPartStruct	// NAME 1: neutral (D10)
{
	const char *Button;		// NAME 1: neutral
	const char *Text;		// NAME 1: neutral
};
static UIAvatrPartStruct UIAvatrParts[50] = {	// NAME 1: neutral (D10)
	{ "gender_tab", "male_fe_avatar00" },
	{ "head_tab", "head_fe_avatar00" },
	{ "body_tab", "physique_fe_avatar00" },
	{ "random", "random_fe_avatar00" },
	{ "turn", NULL },
	{ "turn_left", NULL },
	{ "turn_right", NULL },
	{ "zoom_in", NULL },
	{ "zoom_out", NULL },
	{ "pan", NULL },
	{ "pan_up", NULL },
	{ "pan_down", NULL },
	{ "pan_left", NULL },
	{ "pan_right", NULL },
	{ "confirm", "confirm_fe_avatar00" },
	{ "exit", "exit_fe_avatar00" },
	{ "hair_left", "style_fe_avatar00" },			// 16: the head tab's
	{ "hair_right", "hair_title_fe_avatar00" },
	{ "beard_left", "beard_fe_avatar00" },
	{ "beard_right", "face_title_fe_avatar00" },
	{ "head_type_left", "head_type_fe_avatar00" },
	{ "head_type_right", "acc_title_fe_avatar00" },
	{ "face_left", "face_type_fe_avatar00" },
	{ "face_right", "face_type_fe_avatar00" },
	{ "tattoo_left", "details_fe_avatar00" },
	{ "tattoo_right", "details_fe_avatar00" },
	{ "eye_left", "glasses_fe_avatar00" },
	{ "eye_right", "glasses_fe_avatar00" },
	{ "ear_left", "ear_fe_avatar00" },
	{ "ear_right", "ear_fe_avatar00" },
	{ "hair_color", "hair_color_fe_avatar00" },
	{ "eye_color", "eye_color_fe_avatar00" },
	{ "skin_color", "skin_color_fe_avatar00" },
	{ "scale_in", "head_text_fe_avatar00" },
	{ "scale_out", NULL },
	{ "shirt_style_lef", "shirt_style_fe_avatar00" },	// 35: the body tab's
	{ "shirt_style_rig", "body_title_fe_avatar00" },
	{ "pant_style_left", "pant_style_fe_avatar00" },
	{ "pant_style_righ", "cloth_title_fe_avatar00" },
	{ "p_shirt_color", "p_shirt_color_fe_avatar00" },
	{ "s_shirt_color", "s_shirt_color_fe_avatar00" },
	{ "p_pant_color", "p_pant_color_fe_avatar00" },
	{ "s_pant_color", "s_pant_color_fe_avatar00" },
	{ "move_up", "height_text_fe_avatar00" },
	{ "move_down", "height_var_fe_avatar00" },
	{ "move_left", "weight_text_fe_avatar00" },
	{ "move_right", "weight_var_fe_avatar00" },
	{ "metal00", "metals_fe_avatar00" },
	{ "metal01", "body_text_fe_avatar00" },
	{ "metal02", NULL },
};

// The window's +0x2DC .. +0x2F4 (seven of them, destructor 0x585BF0: 0x559F30), as far as 0x558170 reads +0x2EC
class UIAvatrUnknown2ECStruct	// NAME 1: neutral (D10)
{
public:
	~UIAvatrUnknown2ECStruct();							// 0x585BF0
	UIAvatrUnknown2ECStruct(RenderObjClass *model, const char *name, const char *texture, const char *part, bool two_digits);	// 0x585B50 (0x24 bytes)  // NAME 1 [model, name, texture, part, two_digits]: what 0x559900 passes
	void Unknown_585C90();								// 0x585C90 (the next choice: the odd ids)
	void Unknown_585CC0();								// 0x585CC0 (the previous choice: the even ids)
	void Unknown_585CF0(int choice);					// 0x585CF0  // NAME 1 [choice]: neutral
	void Unknown_585D10();								// 0x585D10 (+0x20: how many choices exist)
	char Unknown00[0x18];								// +0x00
	int Unknown18;										// +0x18 (the choice)
	char Unknown1C[0x20 - 0x1C];						// +0x1C
	int Unknown20;										// +0x20 (how many head types 0x558170 picks from)
};

// The avatar creation window (ui_avatr.cpp's __FILE__), as far as this file's functions so far use it
class AvatarCreationInterface : public InterfaceClass	// NAME 3: the client's RTTI (0xB7B848: SignalBaseClass::MemberCallback<GadgetClass,AvatarCreationInterface,...>)
{
public:
	AvatarCreationInterface(UIViewClass *view, LayerClass *layer, bool keep);	// 0x553AF0 (charview.cpp's 0x679160, mode 4: keep 0)  // NAME 1 [view, layer, keep]: neutral
	virtual ~AvatarCreationInterface();					// 0x554B50 (0x554B20 the deleting one)
	virtual void Unknown_60();							// +0x60 0x554D60 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x554E80 (the events: the gadgets' ids)  // NAME 1 [key]: InterfaceClass's
	void Unknown_557290();								// 0x557290 (id 4)
	void Unknown_557950(int a, bool b, bool c);			// 0x557950 (the constructor: 2, 1, 0 then 0 .. 6, 1, 0)  // NAME 1 [a, b, c]: neutral
	void Unknown_5582A0();								// 0x5582A0 (id 1)
	void Unknown_558D30(int dir);						// 0x558D30 (ids 0x26 / 0x27: 1, 2)  // NAME 1 [dir]: neutral
	void Unknown_558EA0(int dir);						// 0x558EA0 (ids 0x28 / 0x29: 1, 2)  // NAME 1 [dir]: neutral
	void Unknown_559010(int dir);						// 0x559010 (ids 0x12 / 0x13: 1, 2)  // NAME 1 [dir]: neutral
	void Unknown_5591E0(int dir);						// 0x5591E0 (ids 0x14 / 0x15: 1, 2)  // NAME 1 [dir]: neutral
	void Unknown_559350();								// 0x559350 (id 0xF, before the next step)
	void Unknown_559900();								// 0x559900 (the constructor)
	void Unknown_55B9A0(int arg);						// 0x55B9A0 (ids 0x32 .. 0x34: 0, 1, 2)  // NAME 1 [arg]: neutral
	void Unknown_555F60(int text);						// 0x555F60 (the palette's texture: 'text1' .. 'text7')  // NAME 1 [text]: neutral
	void Unknown_5560E0();								// 0x5560E0 (the colour pull made)
	void Unknown_556290();								// 0x556290 (the body and head pulls made)
	void Unknown_556630(int tab, bool on);				// 0x556630 (a tab's gadgets and texts shown or hidden: 0 none, 1 head, 2 body)  // NAME 1 [tab, on]: neutral
	void Unknown_556B20(int tab);						// 0x556B20 (the tab: 0 gender, 1 head, 2 body)  // NAME 1 [tab]: neutral
	void Unknown_55A550(int a, float b, float c, float d);	// 0x55A550 (ids 0x2E .. 0x31: +-0.05 in c or d) (0x556B20: 1, 0, 0, 0 on the head tab, 0, 0, 0, 0 on the body tab)  // NAME 1 [a, b, c, d]: neutral
	void Unknown_55B110(int arg);						// 0x55B110 (0x556B20: 0x20, 0x2A on the body tab)  // NAME 1 [arg]: neutral
	void Unknown_55B650();								// 0x55B650 (0x55B110, last)
	void Unknown_55B2D0();								// 0x55B2D0 (0x556B20, the head tab)
	void Unknown_556F80();								// 0x556F80
	void Unknown_558170();								// 0x558170 (a random head type and scale)
	void Unknown_558060();								// 0x558060 (a random point for the blend weights, kept at +0x164)
	void Unknown_5571D0();								// 0x5571D0 (0x554D60's first half: the +0x1C5 flag)
	void Unknown_5598A0();								// 0x5598A0 (the shared gadgets shown)
	void Unknown_5598D0();								// 0x5598D0 (the shared gadgets hidden)
	void Unknown_55A1F0(int arg);						// 0x55A1F0 (0x554D60: 0 on the body tab, 1 on the head tab)  // NAME 1 [arg]: neutral
	void Unknown_55A960();								// 0x55A960
	void Unknown_55AE50();								// 0x55AE50 (0x5560E0, last)
	void Unknown_55AFD0();								// 0x55AFD0 (the gender text)
	void Unknown_55B540(int zoom);						// 0x55B540 (the head's scale: 1 smaller, 2 larger)  // NAME 1 [zoom]: neutral
	void Unknown_55BAC0();								// 0x55BAC0
	void Unknown_559F30();								// 0x559F30 (the destructor)
	void Unknown_55BFB0();								// 0x55BFB0 (the height and weight texts)
	void Unknown_55C280(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x55C280 (FAIL_IFs '!event', '!gadget': the FlashingGadgetEvent handler)  // NAME 1 [sender]: as ui_avsel.h's
	void Unknown_Set_Palette(const char *name);		// 0x55C350 (the texture of that name onto the model's COLOR_PALETTE: ui_avatar_cust.cpp's 0x5513E0)  // NAME 1: neutral (D10); NAME 1 [name]: neutral

	UIMeshClass *Unknown084;							// +0x84 (the window's mesh)
	int Unknown088;										// +0x88 (the tab, 0x556B20)
	CharacterViewClass *Unknown08C;					// +0x8C
	UIAvatrLoginTaskClass *Unknown090;					// +0x90 (the view's task)
	ShipPipClass *Unknown094;					// +0x94
	GadgetClass *Unknown098[50];						// +0x98 (UIAvatrParts' gadgets)
	UIAvatrPullClass *Unknown160;						// +0x160 (the body's pull)
	Vector2 Unknown164;									// +0x164 (0x558060's point)
	UIAvatrPullClass *Unknown16C;						// +0x16C (the head's pull)
	float Unknown170;									// +0x170 (0x558170)
	float Unknown174;									// +0x174 (0x558170)
	float Unknown178;									// +0x178 (the head's scale, 0.5 .. +0x17C: 0x55B540)
	float Unknown17C;									// +0x17C
	UIAvatrPullClass *Unknown180;						// +0x180 (the colour pull, 0x5560E0)
	Vector2 Unknown184[7];								// +0x184 (0.5, 0.5 each; 0x67C200 fills them when the ship screen sent us back)
	int Unknown1BC;										// +0x1BC (0x55B110: 0 .. 6 from the button)
	int Unknown1C0;										// +0x1C0 (0x55B110: the +0x194 object's value for it)
	char Unknown1C4;									// +0x1C4
	bool Unknown1C5;									// +0x1C5 (0x554D60)
	bool Unknown1C6;									// +0x1C6
	bool Unknown1C7;									// +0x1C7
	bool Unknown1C8;									// +0x1C8
	bool Unknown1C9;									// +0x1C9
	char Unknown1CA[0x2C8 - 0x1CA];						// +0x1CA (+0x1CC an AvatarDescClass, 0x10C bytes: its string at +0x2C8)
	std::string Unknown2C8;								// +0x2C8 (the destructor's)
	bool Unknown2D8;									// +0x2D8
	UIAvatrUnknown2ECStruct *Unknown2DC;				// +0x2DC (0x559F30 deletes the seven)
	UIAvatrUnknown2ECStruct *Unknown2E0;				// +0x2E0
	UIAvatrUnknown2ECStruct *Unknown2E4;				// +0x2E4
	UIAvatrUnknown2ECStruct *Unknown2E8;				// +0x2E8
	UIAvatrUnknown2ECStruct *Unknown2EC;				// +0x2EC (0x558170)
	UIAvatrUnknown2ECStruct *Unknown2F0;				// +0x2F0
	UIAvatrUnknown2ECStruct *Unknown2F4;				// +0x2F4
};

// The window's FlashingGadgetEvent callback: SignalBaseClass::MemberCallback (vtable 0xAEF988 over 0xAEDB58; its
// members out of line after the window's: the destructor 0x55C4C0, 0x55C4E0, 0x55C500, 0x55C520, 0x55C5A0 and the
// deleting destructor 0x55C600)
typedef SignalBaseClass::MemberCallback<GadgetClass, AvatarCreationInterface, void (AvatarCreationInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> UIAvatrCallbackClass;	// NAME 1 [UIAvatrCallbackClass]: neutral (D10)









// env_map_manager.h's EnvironmentMapManagerClass (the view's +0x3A4), as ui_class.cpp declares it
class EnvironmentMapManagerClass { public: void Unknown_760770(int base_asset, bool a); };	// NAME 1: as ui_class.cpp's
struct UIAvatrKeyEventStruct	// NAME 1: neutral (D10): kernel.h's input event, as ui_class.cpp's UIClassKeyEventStruct
{
	UIAvatrKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B, 0x0D)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
struct ControlKeyComboStruct; class ControlKeyListClass { public: void Add(const ControlKeyComboStruct &combo, void *handler); void Remove(const ControlKeyComboStruct &combo); };	// control_keys_manager.h's (InterfaceClass +0x4C, 0x721E60 / 0x721EF0), as ui_login.cpp declares it
class ButtonGadgetClass : public GadgetClass	// NAME 1 [ButtonGadgetClass]: unknown_675120.h's (0x64 bytes), as far as this file makes it
{
public:
	ButtonGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask, unsigned int flags);	// 0x675240 (the screens pass 0x55, 3)
	char Unknown30[0x64 - 0x30];						// +0x30
};
static int UIAvatrEnvMaps[3] = { 100003, 100004, 100002 };	// 0xB7B128: the environment maps' base asset by race  // NAME 1: neutral (D10)

// NOT EXACT: functional (D64), from the client's 0x553AF0: the pip (+0x94, ship_pip.cpp) is ShipPipClass; the
// AvatarDescClass at +0x1CC is constructed in place over this file's char block
// 0x553AF0
AvatarCreationInterface::AvatarCreationInterface(UIViewClass *view, LayerClass *layer, bool keep) :
	InterfaceClass(view, (int)layer->Peek_Scene())
{
	int i;
	Unknown084 = (UIMeshClass *)layer;
	Unknown088 = 3;
	Unknown08C = NULL;
	Unknown090 = NULL;
	Unknown094 = NULL;
	Unknown160 = NULL;
	Unknown164.X = 0.5f;
	Unknown164.Y = 0.5f;
	Unknown16C = NULL;
	Unknown170 = 0.5f;
	Unknown174 = 0.5f;
	Unknown178 = 1.0f;
	Unknown17C = 1.0f;
	Unknown180 = NULL;
	Unknown1BC = 0;
	Unknown1C0 = -1;
	Unknown1C4 = 0;
	Unknown1C5 = false;
	Unknown1C6 = false;
	Unknown1C7 = false;
	Unknown1C8 = false;
	Unknown1C9 = false;
	::new ((void *)((char *)this + 0x1CC)) AvatarDescClass;
	Unknown2D8 = false;
	Unknown2DC = NULL;
	Unknown2E0 = NULL;
	Unknown2E4 = NULL;
	Unknown2E8 = NULL;
	Unknown2EC = NULL;
	Unknown2F0 = NULL;
	Unknown2F4 = NULL;
	for (i = 0; i < 50; i++) {
		Unknown098[i] = NULL;
	}
	for (i = 0; i < 7; i++) {
		Unknown184[i].X = 0.5f;
		Unknown184[i].Y = 0.5f;
	}
	Unknown08C = (CharacterViewClass *)View->Unknown_28();
	Unknown090 = (UIAvatrLoginTaskClass *)Unknown08C->Unknown14->Unknown_08();
	InterfaceSortScopeClass sort(this);
	Unknown08C->Unknown_67A250();
	Unknown08C->Unknown_67ACE0(0);
	Unknown08C->Unknown_67AB20(0, "fe_avatar00");
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	Unknown08C->Unknown068->Set_Clip_Planes(1.0f, 500.0f);
	if (Unknown08C->Unknown1BC == NULL) {
		Unknown08C->Unknown_67AB20(1, "FE01C");
		Unknown08C->Unknown_67A290(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -90.0f, -90.0f), 60.0f);
		Unknown08C->Unknown_67A440(&Unknown08C->Unknown088, false);
		Unknown08C->Peek_Unknown194()->Unknown232 = true;
	}
	Unknown08C->Unknown_67B4F0(false);
	Unknown_BE67BC->Load("fa_fe_avatar00.ini");
	Unknown_BE67C4->Add(model.Peek());

	for (i = 0; i < 16; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name(UIAvatrParts[i].Button));
		Unknown_496AE0(sub.Peek(), 0);
		Unknown098[i] = new ButtonGadgetClass(sub.Peek(), *layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown098[i], i + 1, View, 1, 0);
	}
	for (i = 16; i < 35; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name(UIAvatrParts[i].Button));
		Unknown_496AE0(sub.Peek(), 0);
		Unknown098[i] = new ButtonGadgetClass(sub.Peek(), *layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown098[i], i + 2, View, 1, 0);
	}
	for (i = 35; i < 50; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name(UIAvatrParts[i].Button));
		Unknown_496AE0(sub.Peek(), 0);
		Unknown098[i] = new ButtonGadgetClass(sub.Peek(), *layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown098[i], i + 3, View, 1, 0);
	}
	{
		RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name("turn_down"));
		if (sub) {
			sub->Set_Hidden(1);
		}
		sub = Create_Get(model->Get_Sub_Object_By_Name("turn_up"));
		if (sub) {
			sub->Set_Hidden(1);
		}
	}

	{
		RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
		Unknown094 = new ShipPipClass(1);
		Unknown094->Unknown_74(desc->Desc.Race);
		Unknown094->Unknown_80C220((ViewClass *)Unknown08C);
		((GadgetClass *)Unknown094)->Unknown_65FFC0(1, NULL);
		Unknown094->Unknown_80C540(Vector2(640.0f, 480.0f), Vector3(0.0f, 0.0f, 0.0f), 0x1C);
		Unknown64.Unknown_6870E0((GadgetClass *)Unknown094, 0x36, View, 1, 0);
		Unknown094->Unknown_7C(Create_Peek(Unknown08C->Peek_Unknown194()));
	}

	Unknown_559900();
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIAvatrKeyEventStruct(0x1B), (void *)0x3A);
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIAvatrKeyEventStruct(0x0D), (void *)0xF);
	Unknown_556290();
	Unknown_5560E0();
	bool back = (Unknown090->Previous_Step == 6);
	if (back) {
		Unknown08C->Unknown_67C200(Unknown184, 7, Unknown08C->Peek_Unknown194()->Desc.Gender);
		Unknown08C->Unknown_67C2E0(&Unknown164, Unknown08C->Peek_Unknown194()->Desc.Gender);
		Unknown08C->Unknown_67C340((Vector3 *)&Unknown170, Unknown08C->Peek_Unknown194()->Desc.Gender);
	}
	if (!keep && !back) {
		Unknown_Set_Palette("fa_skin_color.tga");
		Unknown_557950(2, 1, 0);
		Unknown_Set_Palette("fa_avatar_color.tga");
		Unknown_557950(0, 1, 0);
		Unknown_557950(1, 1, 0);
		Unknown_557950(3, 1, 0);
		Unknown_557950(4, 1, 0);
		Unknown_557950(5, 1, 0);
		Unknown_557950(6, 1, 0);
	}
	{
		RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
		Unknown08C->Unknown3A4->Unknown_760770(UIAvatrEnvMaps[desc->Desc.Race], 1);
		Unknown08C->Peek_Unknown194()->Unknown_780810(Unknown08C->Unknown3A4);
		Unknown08C->Peek_Unknown194()->Unknown232 = true;
		Unknown094->Unknown_84(Create_Peek(Unknown08C->Peek_Unknown194()));
	}
	Unknown_556630(0, true);
	if (Unknown094) {
		Unknown094->Unknown_40(true);
	}
	Unknown_55AFD0();
	Unknown_556B20(0);
	Unknown08C->Unknown_67C7B0();
	if (!TutorialManagerClass::Instance->Is_Completed("AvatarCreationIntro")) {
		Unknown08C->Unknown_67C920("1492_00_347se.mp3");
		TutorialManagerClass::Instance->Set_Completed("AvatarCreationIntro");
	}
	void (AvatarCreationInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarCreationInterface::Unknown_55C280;
	GadgetClass::Get_Signal_Map().connect(NULL, new UIAvatrCallbackClass(this, function));
}

// 0x554B50
AvatarCreationInterface::~AvatarCreationInterface()
{
	Unknown_5C();
	if (Unknown180) {
		delete Unknown180;
		Unknown180 = NULL;
	}
	if (Unknown16C) {
		delete Unknown16C;
		Unknown16C = NULL;
	}
	if (Unknown160) {
		delete Unknown160;
		Unknown160 = NULL;
	}
	Unknown094->Unknown_80();
	Unknown094->Unknown_80C180();
	Unknown64.Unknown_687150((GadgetClass *)Unknown094);
	Unknown_559F30();
	Unknown_BE67C4->Remove(Unknown08C->Peek_Model());
	Unknown_BE67BC->Unload("fa_fe_avatar00.ini");
	Unknown08C->Unknown_67A9B0(0);
	void (AvatarCreationInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarCreationInterface::Unknown_55C280;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	UIAvatrCallbackClass callback(this, function);
	notifier->disconnect(0, callback);
}

// 0x554D60
void AvatarCreationInterface::Unknown_60()
{
	Unknown_556F80();
	if (Unknown08C->Unknown_67B2B0()) {
		Unknown1C5 = false;
	}
	if (Unknown08C->Unknown034->Unknown7C && !Unknown1C5) {
		Unknown1C5 = true;
		Unknown_556630(0, false);
		if (Unknown094) Unknown094->Unknown_40(false);
	} else if (!Unknown08C->Unknown034->Unknown7C && Unknown1C5) {
		Unknown1C5 = false;
		Unknown_556630(0, true);
		if (Unknown094) Unknown094->Unknown_40(true);
	}
	if (Unknown088 == 2) {
		Unknown_55A1F0(0);
	} else if (Unknown088 == 1) {
		Unknown_55A1F0(1);
	}
	Unknown_55BFB0();
	Unknown_55A960();
	Unknown_55BAC0();
	Unknown094->Unknown_78();
	Unknown094->Unknown054->Unknown_1C4();
}

// NOT EXACT: functional (D64), from the client's 0x554E80 (the client's switch shares its sound tails)
// 0x554E80
bool AvatarCreationInterface::Unknown_68(int key)
{
	if (Unknown1C4 || Unknown1C5 || Unknown1C6 || Unknown1C7 || Unknown1C8 || Unknown1C9) {
		return(false);
	}
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	switch (key) {
		case 1:
			Unknown_5582A0();
			Unknown_556B20(0);
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		case 2:
			Unknown_556B20(1);
			Unknown_55B650();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			Unknown08C->Unknown_67C7B0();
			if (!TutorialManagerClass::Instance->Is_Completed("AvatarCreationHead")) {
				Unknown08C->Unknown_67C920("1492_00_351se.mp3");
				Unknown08C->Unknown_67C920("1492_00_352se.mp3");
				Unknown08C->Unknown_67C920("1492_00_353se.mp3");
				TutorialManagerClass::Instance->Set_Completed("AvatarCreationHead");
			}
			return(true);
		case 3:
			Unknown_556B20(2);
			Unknown_55B650();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			Unknown08C->Unknown_67C7B0();
			if (!TutorialManagerClass::Instance->Is_Completed("AvatarCreationBody")) {
				Unknown08C->Unknown_67C920("1492_00_355se.mp3");
				Unknown08C->Unknown_67C920("1492_00_356se.mp3");
				Unknown08C->Unknown_67C920("1492_00_357se.mp3");
				TutorialManagerClass::Instance->Set_Completed("AvatarCreationBody");
			}
			return(true);
		case 4:
			Unknown_557290();
			Unknown094->Unknown_84(desc);
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		case 5:
			Unknown094->Unknown_8C();
			break;
		case 0xA:
			Unknown094->Unknown_88();
			break;
		case 0xF:
			Unknown_559350();
			Unknown08C->Unknown_67A3F0();
			Unknown08C->Unknown_67ACE0(1);
			Unknown08C->Unknown1CD = true;
			Unknown090->Go_Next_Step();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			Unknown08C->Unknown_67C7B0();
			return(true);
		case 0x10:
			Unknown08C->Unknown_67A3F0();
			Unknown08C->Unknown_67ACE0(1);
			Unknown08C->Unknown_67ACE0(4);
			Unknown08C->Unknown_67A670();
			Unknown08C->Unknown1CD = true;
			Unknown090->Go_Back_Step();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			Unknown08C->Unknown_67C7B0();
			return(true);
		case 0x12:
			Unknown_559010(1);
			break;
		case 0x13:
			Unknown_559010(2);
			break;
		case 0x14:
			Unknown_5591E0(1);
			break;
		case 0x15:
			Unknown_5591E0(2);
			break;
		case 0x16:
		case 0x17:
			if (key == 0x16) Unknown2DC->Unknown_585CC0(); else Unknown2DC->Unknown_585C90();
			desc->Unknown_77F000(Unknown2DC->Unknown18);
			desc->Unknown_780810(Unknown08C->Unknown3A4);
			Unknown094->Unknown_84(desc);
			break;
		case 0x18:
		case 0x19:
			if (key == 0x18) Unknown2E8->Unknown_585CC0(); else Unknown2E8->Unknown_585C90();
			desc->Unknown_7857D0(Unknown2E8->Unknown18);
			Unknown094->Unknown_84(desc);
			break;
		case 0x1A:
		case 0x1B:
			if (key == 0x1A) Unknown2EC->Unknown_585CC0(); else Unknown2EC->Unknown_585C90();
			desc->Unknown_7859F0(Unknown2EC->Unknown18);
			Unknown_55B2D0();
			Unknown094->Unknown_84(desc);
			break;
		case 0x1C:
		case 0x1D:
			if (key == 0x1C) Unknown2F0->Unknown_585CC0(); else Unknown2F0->Unknown_585C90();
			desc->Unknown_77FCE0(Unknown2F0->Unknown18, 1);
			TheDX8MeshRenderer.Invalidate();
			desc->Unknown_780810(Unknown08C->Unknown3A4);
			Unknown094->Unknown_84(desc);
			break;
		case 0x1E:
		case 0x1F:
			if (key == 0x1E) Unknown2F4->Unknown_585CC0(); else Unknown2F4->Unknown_585C90();
			desc->Unknown_77FC90(Unknown2F4->Unknown18, 1);
			TheDX8MeshRenderer.Invalidate();
			desc->Unknown_780810(Unknown08C->Unknown3A4);
			Unknown094->Unknown_84(desc);
			break;
		case 0x20:
		case 0x21:
			Unknown_Set_Palette("fa_avatar_color.tga");
			Unknown_55B110(key);
			break;
		case 0x22:
			Unknown_Set_Palette("fa_skin_color.tga");
			Unknown_55B110(key);
			break;
		case 0x26:
			Unknown_558D30(1);
			break;
		case 0x27:
			Unknown_558D30(2);
			break;
		case 0x28:
			Unknown_558EA0(1);
			break;
		case 0x29:
			Unknown_558EA0(2);
			break;
		case 0x2A:
		case 0x2B:
		case 0x2C:
		case 0x2D:
			Unknown_55B110(key);
			break;
		case 0x2E:
			Unknown_55A550(0, 0.0f, 0.0f, 0.05f);
			break;
		case 0x2F:
			Unknown_55A550(0, 0.0f, 0.0f, -0.05f);
			break;
		case 0x30:
			Unknown_55A550(0, 0.0f, -0.05f, 0.0f);
			break;
		case 0x31:
			Unknown_55A550(0, 0.0f, 0.05f, 0.0f);
			break;
		case 0x32:
			Unknown_55B9A0(0);
			break;
		case 0x33:
			Unknown_55B9A0(1);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			return(true);
		case 0x34:
			Unknown_55B9A0(2);
			break;
		case 0x3A:
			Unknown08C->Unknown_67B270();
			Unknown1C5 = true;
			Unknown08C->Unknown_67C7B0();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		default:
			return(false);
	}
	SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
	return(true);
}

// 0x555F60
void AvatarCreationInterface::Unknown_555F60(int text)
{
	const char *names[] = { "text1", "text2", "Text3", "text4", "Text5", "Text6", "text7" };
	RefCountPtr<RenderObjClass> palette = Create_Get(Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	if (palette) {
		RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(names[text], MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
		if (texture) {
			Unknown_495FD0(palette.Peek(), texture.Peek(), 0);
		}
	}
}

// 0x5560E0
void AvatarCreationInterface::Unknown_5560E0()
{
	Unknown180 = new UIAvatrPullClass;
	RenderObjClass *palette = Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("COLOR_PALETTE");
	Unknown_496AE0(palette, 0);
	Unknown180->Gadget = new UIAvatrGadget671300Class(palette, 0, Unknown084, 0, 0, 0);
	Unknown180->Gadget->Unknown2C |= 0x400;
	Unknown180->Unknown_5859A0();
	palette->Release_Ref();
	Unknown180->Pull = Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("PULL_COLOR");
	Unknown_496AE0(Unknown180->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown180->Gadget, 0x39, View, 1, 1);
	Unknown_55AE50();
}

// 0x556290
void AvatarCreationInterface::Unknown_556290()
{
	Unknown160 = new UIAvatrPullClass;
	Unknown16C = new UIAvatrPullClass;
	RefCountPtr<RenderObjClass> area = Create_Get(Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("BODY_AREA"));
	Unknown_496AE0(area.Peek(), 0);
	Unknown160->Gadget = new UIAvatrGadget671300Class(area.Peek(), 0, Unknown084, 0, 0, 0);
	Unknown160->Gadget->Unknown2C |= 0x400;
	Unknown160->Unknown_5859A0();
	Unknown160->Pull = Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("BODY_PULL");
	Unknown_496AE0(Unknown160->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown160->Gadget, 0x37, View, 1, 1);
	area = Create_Get(Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("HEAD_AREA"));
	Unknown_496AE0(area.Peek(), 0);
	Unknown16C->Gadget = new UIAvatrGadget671300Class(area.Peek(), 0, Unknown084, 0, 0, 0);
	Unknown16C->Gadget->Unknown2C |= 0x400;
	Unknown16C->Unknown_5859A0();
	Unknown16C->Pull = Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("HEAD_PULL");
	Unknown_496AE0(Unknown16C->Pull, 1);
	Unknown64.Unknown_6870E0(Unknown16C->Gadget, 0x38, View, 1, 1);
}

// 0x556630
void AvatarCreationInterface::Unknown_556630(int tab, bool on)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	int i;
	switch (tab) {
		case 0:
			if (Unknown180) {
				Unknown180->Gadget->Unknown_40(false);
				Unknown180->Gadget->Unknown_8C(true);
				Unknown180->Pull->Set_Hidden(true);
			}
			if (Unknown160) {
				Unknown160->Gadget->Unknown_40(false);
				Unknown160->Gadget->Unknown_8C(true);
				Unknown160->Pull->Set_Hidden(true);
			}
			if (Unknown16C) {
				Unknown16C->Gadget->Unknown_40(false);
				Unknown16C->Gadget->Unknown_8C(true);
				Unknown16C->Pull->Set_Hidden(true);
			}
			break;
		case 1:
			{
				for (i = 16; i < 35; i++) {
					Unknown098[i]->Unknown_40(on);
					Unknown098[i]->Unknown_8C(!on);
					if (UIAvatrParts[i].Text) {
						FontSys *text = Unknown_BE67C4->Get_Style(UIAvatrParts[i].Text, model.Peek());
						if (text) text->Set_Hidden(!on);
					}
				}
				if (Unknown180) {
					Unknown180->Gadget->Unknown_40(on);
					Unknown180->Gadget->Unknown_8C(!on);
					Unknown180->Pull->Set_Hidden(!on);
				}
				if (Unknown160) {
					Unknown160->Gadget->Unknown_40(false);
					Unknown160->Gadget->Unknown_8C(true);
					Unknown160->Pull->Set_Hidden(true);
				}
				RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
				if (desc->Desc.Gender == 1) {
					Unknown098[18]->Unknown_40(false);
					Unknown098[18]->Unknown_8C(true);
					Unknown098[19]->Unknown_40(false);
					Unknown098[19]->Unknown_8C(true);
					FontSys *text = Unknown_BE67C4->Get_Style("beard_fe_avatar00", model.Peek());
					if (text) text->Set_Hidden(true);
				}
			}
			break;
		case 2:
			for (i = 35; i < 50; i++) {
				Unknown098[i]->Unknown_40(on);
				Unknown098[i]->Unknown_8C(!on);
				if (UIAvatrParts[i].Text) {
					FontSys *text = Unknown_BE67C4->Get_Style(UIAvatrParts[i].Text, model.Peek());
					if (text) text->Set_Hidden(!on);
				}
			}
			if (Unknown180) {
				Unknown180->Gadget->Unknown_40(on);
				Unknown180->Gadget->Unknown_8C(!on);
				Unknown180->Pull->Set_Hidden(!on);
			}
			if (Unknown160) {
				Unknown160->Gadget->Unknown_40(on);
				Unknown160->Gadget->Unknown_8C(!on);
				Unknown160->Pull->Set_Hidden(!on);
			}
			if (Unknown16C) {
				Unknown16C->Gadget->Unknown_40(false);
				Unknown16C->Gadget->Unknown_8C(true);
				Unknown16C->Pull->Set_Hidden(true);
			}
			break;
	}
}

// NOT EXACT: inline budget (0.50, 972 bytes against 0x379: the client calls the palette helper's two ~RefCountPtr out of line, 0x4740B0, in both inlined copies, and keeps 1 in ebx; ours inlines them and keeps 0; attempts.py show 0x556B20)
// 0x556B20
void AvatarCreationInterface::Unknown_556B20(int tab)
{
	if (Unknown088 == tab && (tab == 1 || tab == 2)) {
		return;
	}
	Unknown088 = tab;
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<RenderObjClass> frame = Create_Get(model->Get_Sub_Object_By_Name("FRAME_COLOR"));
	switch (tab) {
		case 0:
			Unknown_556630(1, false);
			Unknown_556630(2, false);
			Unknown_556630(0, true);
			if (frame) frame->Set_Hidden(true);
			Unknown_55B110(0x20);
			Unknown094->Unknown0DC = false;
			Unknown094->Unknown_88();
			Unknown094->Unknown_8C();
			Unknown094->Unknown054->Unknown231 = false;
			Unknown094->Unknown054->Unknown214 = true;
			break;
		case 1:
			Unknown_556630(0, false);
			Unknown_556630(2, false);
			Unknown_556630(1, true);
			Unknown_55B2D0();
			Unknown_55A550(1, 0, 0, 0);
			if (frame) frame->Set_Hidden(false);
			Unknown_55B110(0x20);
			Unknown_Set_Palette("fa_avatar_color.tga");
			Unknown094->Unknown0DC = true;
			Unknown094->Unknown_88();
			Unknown094->Unknown_8C();
			Unknown094->Unknown054->Unknown231 = true;
			Unknown094->Unknown054->Unknown214 = true;
			break;
		case 2:
			Unknown_556630(0, false);
			Unknown_556630(1, false);
			Unknown_556630(2, true);
			Unknown_55A550(0, 0, 0, 0);
			if (frame) frame->Set_Hidden(false);
			Unknown_55B110(0x2A);
			Unknown_Set_Palette("fa_avatar_color.tga");
			Unknown094->Unknown0DC = false;
			Unknown094->Unknown_88();
			Unknown094->Unknown_8C();
			Unknown094->Unknown054->Unknown231 = false;
			Unknown094->Unknown054->Unknown214 = true;
			break;
	}
}

// 0x5571D0
void AvatarCreationInterface::Unknown_5571D0()
{
	Unknown_556F80();
	if (Unknown08C->Unknown_67B2B0()) {
		Unknown1C5 = false;
	}
	if (Unknown08C->Unknown034->Unknown7C && !Unknown1C5) {
		Unknown1C5 = true;
		Unknown_556630(0, false);
		if (Unknown094) Unknown094->Unknown_40(false);
	} else if (!Unknown08C->Unknown034->Unknown7C && Unknown1C5) {
		Unknown1C5 = false;
		Unknown_556630(0, true);
		if (Unknown094) Unknown094->Unknown_40(true);
	}
}

// 0x558060
void AvatarCreationInterface::Unknown_558060()
{
	Vector2 uv;
	uv.X = Unknown_BDBB84(0, 100) * 0.01f;
	uv.Y = Unknown_BDBB84(0, 100) * 0.01f;
	UIAvatrWeightsStruct body = Unknown_5860B0(uv);
	UIAvatrWeightsStruct face = Unknown_5860B0(uv);
	Unknown08C->Peek_Unknown194()->Unknown_77E900(body.Weight);
	Unknown08C->Peek_Unknown194()->Unknown_77E830(face.Weight);
	Unknown164 = uv;
}

// 0x558170
void AvatarCreationInterface::Unknown_558170()
{
	int count = Unknown2EC->Unknown20;
	int type = Unknown_BDBB84(0, count);
	Unknown08C->Peek_Unknown194()->Unknown_7859F0(type);
	Unknown_55B2D0();
	if (type == 3) {
		Unknown170 = 0.5f;
		Unknown174 = 0.5f;
		Unknown178 = 1.0f;
		Unknown_55B540(0);
		return;
	}
	Unknown170 = Unknown_BDBB84(0, 100) * 0.01f;
	Unknown174 = Unknown_BDBB84(0, 100) * 0.01f;
	Unknown178 = Unknown_BDBB84(5, (int)(Unknown17C * 10.0f)) * 0.1f;
	Unknown_55B540(0);
}

// 0x5598A0
void AvatarCreationInterface::Unknown_5598A0()
{
	Unknown_556630(0, true);
	if (Unknown094) Unknown094->Unknown_40(true);
}

// 0x5598D0
void AvatarCreationInterface::Unknown_5598D0()
{
	Unknown_556630(0, false);
	if (Unknown094) Unknown094->Unknown_40(false);
}

// NOT EXACT: functional (D64), from the client's 0x559350 (the avatar's Desc at +0xF8, avatar.h's)
// 0x559350
void AvatarCreationInterface::Unknown_559350()
{
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	Unknown08C->Unknown_5594B0(*(AvatarDescClass *)((char *)desc.Peek() + 0xF8));
	Unknown08C->Unknown_67C270(Unknown184, 7, desc->Desc.Gender);
	Unknown08C->Unknown_67C310(&Unknown164, desc->Desc.Gender);
	Unknown08C->Unknown_67C380((Vector3 *)&Unknown170, desc->Desc.Gender);
}

// NOT EXACT: functional (D64), from the client's 0x5594B0 (AvatarDescClass's assignment)
// 0x5594B0
void CharacterViewClass::Unknown_5594B0(AvatarDescClass desc)
{
	Unknown088 = desc;
}

// 0x559F30
void AvatarCreationInterface::Unknown_559F30()
{
	if (Unknown2DC) {
		delete Unknown2DC;
		Unknown2DC = NULL;
	}
	if (Unknown2E0) {
		delete Unknown2E0;
		Unknown2E0 = NULL;
	}
	if (Unknown2E4) {
		delete Unknown2E4;
		Unknown2E4 = NULL;
	}
	if (Unknown2E8) {
		delete Unknown2E8;
		Unknown2E8 = NULL;
	}
	if (Unknown2EC) {
		delete Unknown2EC;
		Unknown2EC = NULL;
	}
	if (Unknown2F0) {
		delete Unknown2F0;
		Unknown2F0 = NULL;
	}
	if (Unknown2F4) {
		delete Unknown2F4;
		Unknown2F4 = NULL;
	}
}

static const char *UIAvatrGenders[2] = { "AM", "AF" };	// 0xB7B114: the models' gender prefix  // NAME 1: neutral (D10)
static const char *UIAvatrRaces[3] = { "T", "J", "P" };		// 0xB7B11C: the models' race prefix  // NAME 1: neutral (D10)

// NOT EXACT: functional (D64), from the client's 0x559900
// 0x559900
void AvatarCreationInterface::Unknown_559900()
{
	char name[64];
	char texture[64];
	Unknown_559F30();
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	const char *gender = UIAvatrGenders[desc->Desc.Gender];
	const char *race = UIAvatrRaces[desc->Desc.Race];
	_snprintf(name, sizeof(name), "%s%shead", gender, race);
	Unknown2DC = new UIAvatrUnknown2ECStruct(model.Peek(), name, NULL, "HEAD", false);
	Unknown2DC->Unknown_585CF0(desc->Desc.Head_Type);
	Unknown2DC->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%shair", gender, race);
	Unknown2E0 = new UIAvatrUnknown2ECStruct(model.Peek(), name, NULL, "HAIR", false);
	Unknown2E0->Unknown_585CF0(desc->Desc.Hair_Num);
	Unknown2E0->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%sbeard", gender, race);
	Unknown2E4 = new UIAvatrUnknown2ECStruct(model.Peek(), name, NULL, "BEARD", false);
	Unknown2E4->Unknown_585CF0(desc->Desc.Beard_Num);
	Unknown2E4->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%shead%02d", gender, race, desc->Desc.Head_Type);
	_snprintf(texture, sizeof(texture), "%s%sFACETEX", gender, race);
	Unknown2E8 = new UIAvatrUnknown2ECStruct(model.Peek(), name, texture, "FACE", false);
	Unknown2E8->Unknown_585CF0(desc->Desc.Head_Texture_Num);
	Unknown2E8->Unknown_585D10();
	Unknown2EC = new UIAvatrUnknown2ECStruct(model.Peek(), NULL, "atattoo", "TATTOO", false);
	Unknown2EC->Unknown_585CF0(desc->Desc.Tattoo_Texture_Num);
	Unknown2EC->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%sGOGGLE", gender, race);
	Unknown2F0 = new UIAvatrUnknown2ECStruct(model.Peek(), name, NULL, "EYE", false);
	Unknown2F0->Unknown_585CF0(desc->Desc.Goggle_Num);
	Unknown2F0->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%sEAR", gender, race);
	Unknown2F4 = new UIAvatrUnknown2ECStruct(model.Peek(), name, NULL, "EAR", false);
	Unknown2F4->Unknown_585CF0(desc->Desc.Ear_Num);
	Unknown2F4->Unknown_585D10();
}

// 0x55AFD0
void AvatarCreationInterface::Unknown_55AFD0()
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	FontSys *text = Unknown_BE67C4->Get_Style("male_fe_avatar00", model.Peek());
	if (Unknown08C->Peek_Unknown194()->Desc.Gender == 1) {
		text->Change_Text("Gender: Female", 0);
	} else {
		text->Change_Text("Gender: Male", 0);
	}
}

// 0x55B110
void AvatarCreationInterface::Unknown_55B110(int arg)
{
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	switch (arg) {
		case 0x20:
			Unknown1BC = 0;
			break;
		case 0x21:
			Unknown1BC = 1;
			break;
		case 0x22:
			Unknown1BC = 2;
			break;
		case 0x2A:
			Unknown1BC = 3;
			Unknown1C0 = desc->Desc.Shirt_Primary_Metal;
			break;
		case 0x2B:
			Unknown1BC = 4;
			Unknown1C0 = desc->Desc.Shirt_Secondary_Metal;
			break;
		case 0x2C:
			Unknown1BC = 5;
			Unknown1C0 = desc->Desc.Pants_Primary_Metal;
			break;
		case 0x2D:
			Unknown1BC = 6;
			Unknown1C0 = desc->Desc.Pants_Secondary_Metal;
			break;
	}
	Unknown_55AE50();
	Unknown_55B650();
}

// 0x55B540
void AvatarCreationInterface::Unknown_55B540(int zoom)
{
	float step = Unknown178 * 0.033333335f;
	if (zoom == 1) {
		Unknown178 = WWMath::Max(Unknown178 - step, 0.5f);
		if (Unknown178 == 0.5f) {
			Unknown098[33]->Unknown_40(false);
		}
		Unknown098[34]->Unknown_40(true);
	} else if (zoom == 2) {
		Unknown178 = WWMath::Min(step + Unknown178, Unknown17C);
		if (Unknown178 == Unknown17C) {
			Unknown098[34]->Unknown_40(false);
		}
		Unknown098[33]->Unknown_40(true);
	}
	Unknown_55A550(1, 0, 0, 0);
}

// 0x55C350 (after the FAIL_IFs of 0x55C280, lines 2207 / 2211, in the client; no caller: 0x556B20 inlines it)
void AvatarCreationInterface::Unknown_Set_Palette(const char *name)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<RenderObjClass> palette = Create_Get(model->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	TextureClass *texture = WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
	if (texture) {
		Unknown_495FD0(palette.Peek(), texture, 0);
		texture->Release_Ref();
	}
}



/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avatr.cpp (its __FILE__, 0xB7B80C: 0x55C280's FAIL_IFs on lines 2207 / 2211)
**
**	The avatar creation window, AvatarCreationInterface (the client's RTTI: the type name of its
**	FlashingGadgetEvent callback, 0xB7B848, sits in this file's literals). The file's evidence: its
**	code runs from 0x553AF0 (after ui_avatar_dialog.cpp, whose last asserts are 0x5537A0's) past the
**	FAIL_IFs of 0x55C280; 0x553AF0 loads 'fa_fe_avatar00.ini', and the literals of the file's own
**	(the '*_fe_avatar00' names, 'BODY_PULL' 0xB7B76C, 'AvatarCreationHead' ...) lie between
**	ui_avatar_dialog.cpp's __FILE__ (0xB7AEBC) and this file's (0xB7B80C). It is ui_avatar_cust.cpp's
**	sibling: the literals both use ('COLOR_PALETTE', 'HEAD_PULL', 'FRAME_COLOR', 'fa_avatar_color.tga')
**	are merged into ui_avatar_cust.cpp's, which links first.
**
**	So far (TK6, claim #88): 0x556290 (the body and head pulls made) and 0x556630 (a tab's gadgets and
**	texts shown or hidden) exact, 0x556B20 (the
**	tab chosen; NOT EXACT) and the palette helper 0x55C350 it inlines (exact; defined at the end here
**	for now, it belongs after the FAIL_IFs of line 2207). The gadgets at +0x98 follow UIAvatrParts
**	(0xB7AF84, 50 entries: 16 shared, 19 for the head tab, 15 for the body tab). The rest of the file
**	is not rebuilt; the class is declared only as far as these use it.
**	TK14 (claim #118): 0x554D60 (the per-frame update: the +0x1C5 flag follows +0x8C's +0x34's +0x7C;
**	the client re-reads +0x1C5 in the second test, so the condition is written twice), 0x555F60 (the
**	palette's texture 'text1' .. 'text7': a local array of names) and 0x5560E0 (the colour pull over
**	COLOR_PALETTE, list id 0x39) exact. The destructor 0x554B50 exact (0x554B20 its deleting one, an
**	identical copy): the FlashingGadgetEvent callback 0x55C280 taken off 0x65ECE0's notifier, its member
**	function pointer held in a local first (the client loads it before the call); the callback's base
**	needs its own empty destructor (the client resets the base vtable 0xAEDB58 after Remove).
**	TK14 (claim #122): 0x5571D0 (0x554D60's first half, written out again in 0x554D60: a call from
**	0x554D60 is not inlined, 0.29), 0x5598A0 and 0x5598D0 (the shared gadgets shown / hidden) exact.
**	The window's vtable 0xAEF9BC: +0x60 is 0x554D60 and +0x68 0x554E80 (InterfaceClass's Unknown_60 /
**	Unknown_68 overridden; declared plain members here so far).
**	0x55AFD0 (the gender text: an if / else of two Change_Text calls, the ?: form loses 0.78) and
**	0x55B540 (the head's scale, WWMath::Max / Min) exact.
**	0x558060 (a random point for avatar.cpp's blend weights, 0x5860B0: the point built in a Vector2
**	member by member, the weights struct needs a constructor so the returns land in the locals) and
**	0x558170 (a random head type and scale; the type count read into a local first) exact.
**	0x559F30 (the seven +0x2DC .. +0x2F4 objects deleted, destructor 0x585BF0) exact.
**	0x55B110 (the button 0x20 .. 0x22 / 0x2A .. 0x2D into +0x1BC, +0x1C0 from the +0x194 object) exact.
*/

// NOT EXACT: functional (D64), from the client's 0x558D30 (inline-budget: the client's RefCountPtr temporaries)
// 0x558D30
void AvatarCreationInterface::Unknown_558D30(int dir)
{
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	int num = desc->Desc.Body_Type;
	if (dir == 2) {
		num = (num + 1) % 3;
	} else if (dir == 1) {
		num--;
		if (num < 0) {
			num = 2;
		}
	}
	desc->Unknown_77E9D0(num, desc->Desc.Pants_Type, true);
	desc->Unknown_780810(Unknown08C->Unknown3A4);
	Unknown094->Unknown_84(desc);
}

// NOT EXACT: functional (D64), from the client's 0x558EA0 (inline-budget: the client's RefCountPtr temporaries)
// 0x558EA0
void AvatarCreationInterface::Unknown_558EA0(int dir)
{
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	int num = desc->Desc.Pants_Type;
	if (dir == 2) {
		num = (num + 1) % 3;
	} else if (dir == 1) {
		num--;
		if (num < 0) {
			num = 2;
		}
	}
	desc->Unknown_77E9D0(desc->Desc.Body_Type, num, true);
	desc->Unknown_780810(Unknown08C->Unknown3A4);
	Unknown094->Unknown_84(desc);
}

// NOT EXACT: functional (D64), from the client's 0x559010 (a female avatar skips hair 0: the client calls itself again)
// 0x559010
void AvatarCreationInterface::Unknown_559010(int dir)
{
	if (dir == 1) {
		Unknown2E0->Unknown_585CC0();
	} else {
		Unknown2E0->Unknown_585C90();
	}
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	if (desc->Desc.Gender == 1 && Unknown2E0->Unknown18 == 0) {
		Unknown_559010(dir);
		return;
	}
	desc->Unknown_77FC20(Unknown2E0->Unknown18, true);
	desc->Unknown_780810(Unknown08C->Unknown3A4);
	Unknown094->Unknown_84(desc);
}

// NOT EXACT: functional (D64), from the client's 0x5591E0 (inline-budget: the client's RefCountPtr temporaries)
// 0x5591E0
void AvatarCreationInterface::Unknown_5591E0(int dir)
{
	if (dir == 1) {
		Unknown2E4->Unknown_585CC0();
	} else {
		Unknown2E4->Unknown_585C90();
	}
	Unknown08C->Peek_Unknown194()->Unknown_77FD30(Unknown2E4->Unknown18, true);
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	desc->Unknown_780810(Unknown08C->Unknown3A4);
	Unknown094->Unknown_84(desc);
}

// NOT EXACT: functional (D64), from the client's 0x55A960 (the colour pull: dragged, it moves its marker on the
// window model's bone and takes the palette's colour under it into the chosen part, +0x1BC)
// 0x55A960
void AvatarCreationInterface::Unknown_55A960()
{
	UIAvatrGadget671300Class *gadget = (UIAvatrGadget671300Class *)Unknown180->Gadget;
	if (gadget->Unknown64) {
		Unknown180->Unknown08 = true;
	}
	if ((gadget->Unknown64 || gadget->Unknown65) && Unknown180->Unknown08) {
		AABoxClass box;
		gadget->Unknown_64(box);
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		if (x >= box.Center.X + box.Extent.X) x = box.Center.X + box.Extent.X;
		if (x <= box.Center.X - box.Extent.X) x = box.Center.X - box.Extent.X;
		if (y >= box.Center.Y + box.Extent.Y) y = box.Center.Y + box.Extent.Y;
		if (y <= box.Center.Y - box.Extent.Y) y = box.Center.Y - box.Extent.Y;
		RenderObjClass *model = Unknown08C->Unknown1B8;
		int bone = model->Get_Sub_Object_Bone_Index(Unknown180->Pull);
		model->Capture_Bone(bone);
		Matrix3D tm = Unknown180->Pull->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
		model->Control_Bone(bone, tm, false);
		float u = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		float v = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		if (u < 0.01f) u = 0.01f; else if (u > 0.99f) u = 0.99f;
		if (v < 0.01f) v = 0.01f; else if (v > 0.99f) v = 0.99f;
		Vector3 color;
		Unknown_496C00(gadget->Unknown_08()->Unknown30, u, 1.0f - v, &color);
		Unknown184[Unknown1BC].X = u;
		Unknown184[Unknown1BC].Y = v;
		CharacterViewAvatarClass *avatar = Unknown08C->Peek_Unknown194();
		switch (Unknown1BC) {
			case 0: avatar->Unknown_77FFA0(color, true); break;
			case 1: avatar->Unknown_7804A0(color); break;
			case 2: avatar->Unknown_780680(0, color); break;
			case 3: avatar->Unknown_780680(1, color); Unknown_55B9A0(-1); break;
			case 4: avatar->Unknown_780680(2, color); Unknown_55B9A0(-1); break;
			case 5: avatar->Unknown_780680(3, color); Unknown_55B9A0(-1); break;
			case 6: avatar->Unknown_780680(4, color); Unknown_55B9A0(-1); break;
		}
		avatar->Unknown_780810(Unknown08C->Unknown3A4);
	} else {
		Unknown180->Unknown08 = false;
	}
	gadget->Unknown_671630();
}

// NOT EXACT: functional (D64), from the client's 0x55B9A0 (the body tab's metal, -1: none, into +0x1C0 and the chosen part)
// 0x55B9A0
void AvatarCreationInterface::Unknown_55B9A0(int arg)
{
	if (Unknown088 != 2) {
		return;
	}
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	Unknown1C0 = arg;
	switch (Unknown1BC) {
		case 3: desc->Unknown_780790(1, arg); break;
		case 4: desc->Unknown_780790(2, arg); break;
		case 5: desc->Unknown_780790(3, arg); break;
		case 6: desc->Unknown_780790(4, arg); break;
	}
	desc->Unknown_780810(Unknown08C->Unknown3A4);
}

// NOT EXACT: functional (D64), from the client's 0x5582A0 (the gender button: the offsets saved for the old
// gender; the other gender's avatar kept at +0x1CC comes back (+0x2D8), or the first time a copy of this one
// in the other gender with the palettes' default colours; the old description is then kept at +0x1CC)
// 0x5582A0
void AvatarCreationInterface::Unknown_5582A0()
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<CharacterViewAvatarClass> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	AvatarDescClass old(desc->Desc);
	Unknown08C->Unknown_67C270(Unknown184, 7, old.Gender);
	Unknown08C->Unknown_67C310(&Unknown164, old.Gender);
	Unknown08C->Unknown_67C380((Vector3 *)&Unknown170, old.Gender);
	desc->Desc.Gender = (desc->Desc.Gender == 0);
	Unknown_55AFD0();
	AvatarDescClass *kept = (AvatarDescClass *)((char *)this + 0x1CC);
	if (Unknown2D8) {
		Unknown08C->Unknown_67A3F0();
		Unknown08C->Unknown_67A440(kept, false);
		Unknown_559900();
		Unknown08C->Unknown_67C200(Unknown184, 7, kept->Gender);
		Unknown08C->Unknown_67C2E0(&Unknown164, kept->Gender);
		Unknown08C->Unknown_67C340((Vector3 *)&Unknown170, kept->Gender);
	} else {
		AvatarDescClass copy(desc->Desc);
		Unknown08C->Unknown_67A3F0();
		Unknown08C->Unknown_67A440(&copy, true);
		Unknown_559900();
		Unknown_Set_Palette("fa_skin_color.tga");
		Unknown_557950(2, 1, 0);
		Unknown_Set_Palette("fa_avatar_color.tga");
		Unknown_557950(0, 1, 0);
		Unknown_557950(1, 1, 0);
		Unknown_557950(3, 1, 0);
		Unknown_557950(4, 1, 0);
		Unknown_557950(5, 1, 0);
		Unknown_557950(6, 1, 0);
		Unknown164.X = 0.5f;
		Unknown164.Y = 0.5f;
		Unknown170 = 0.5f;
		Unknown174 = 0.5f;
		Unknown178 = 1.0f;
	}
	*kept = old;
	Unknown2D8 = true;
	Unknown_559350();
	RefCountPtr<CharacterViewAvatarClass> avatar = Create_Peek(Unknown08C->Peek_Unknown194());
	avatar->Unknown_780810(Unknown08C->Unknown3A4);
	avatar->Unknown232 = true;
	Unknown094->Unknown_84(avatar);
	Unknown08C->Unknown_67C7B0();
	if (avatar->Desc.Gender == 1) {
		Unknown08C->Unknown_67C920("1492_00_350se.mp3");
	} else {
		Unknown08C->Unknown_67C920("1492_00_349se.mp3");
	}
}

// NOT EXACT: functional (D64), from the client's 0x557950 (a colour from the palette at a point: the description's
// default (a = 1, b = 0), the one kept for the part (b = 1) or a random one (a = 0, then a random metal too))
// 0x557950
void AvatarCreationInterface::Unknown_557950(int which, bool keep, bool stored)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<CharacterViewAvatarClass> avatar = Create_Peek(Unknown08C->Peek_Unknown194());
	if (avatar.Peek() == NULL) {
		return;
	}
	Vector2 uv(0.0f, 0.0f);
	if (!keep) {
		uv.X = Unknown_BDBB84(0, 100) * 0.01f;
		uv.Y = Unknown_BDBB84(0, 100) * 0.01f;
	} else if (stored) {
		uv = Unknown184[which];
	} else {
		switch (which) {
			case 0: uv = avatar->Desc.Get_Hair_Offset(); break;
			case 1: uv = avatar->Desc.Get_Eye_Offset(); break;
			case 2: uv = avatar->Desc.Get_Skin_Offset(); break;
			case 3: uv = avatar->Desc.Get_Shirt_Primary_Offset(); break;
			case 4: uv = avatar->Desc.Get_Shirt_Secondary_Offset(); break;
			case 5: uv = avatar->Desc.Get_Pants_Primary_Offset(); break;
			case 6: uv = avatar->Desc.Get_Pants_Secondary_Offset(); break;
		}
	}
	RefCountPtr<RenderObjClass> palette = Create_Get(model->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	Vector3 color;
	Unknown_496C00(palette.Peek(), uv.X, stored ? 1.0f - uv.Y : uv.Y, &color);
	switch (which) {
		case 0: avatar->Unknown_77FFA0(color, true); break;
		case 1: avatar->Unknown_7804A0(color); break;
		case 2: avatar->Unknown_780680(0, color); break;
		case 3: avatar->Unknown_780680(1, color); if (!keep) avatar->Unknown_780790(1, Unknown_BDBB84(-1, 2)); break;
		case 4: avatar->Unknown_780680(2, color); if (!keep) avatar->Unknown_780790(2, Unknown_BDBB84(-1, 2)); break;
		case 5: avatar->Unknown_780680(3, color); if (!keep) avatar->Unknown_780790(3, Unknown_BDBB84(-1, 2)); break;
		case 6: avatar->Unknown_780680(4, color); if (!keep) avatar->Unknown_780790(4, Unknown_BDBB84(-1, 2)); break;
	}
	Unknown184[which].X = uv.X;
	Unknown184[which].Y = stored ? uv.Y : 1.0f - uv.Y;
	Unknown_55AE50();
}
