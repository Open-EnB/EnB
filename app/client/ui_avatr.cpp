// Earth & Beyond client (reconstruction): app\Client\ui_avatr.cpp. Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "refcount.h"
#include "random.h"

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral

	char Unknown04[0x2C - 0x04];						// +0x04
	unsigned int Unknown2C;								// +0x2C (0x556290 sets 0x400)
};

// The pulls' gadget (0x80 bytes, constructor 0x671300), as far as 0x556290 makes it
class UIAvatrGadget671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d);	// 0x671300 (0x556290: the area model, 0, the window's mesh, 0, 0, 0)  // NAME 1 [area, a, mesh, b, c, d]: neutral

	char Unknown30[0x80 - 0x30];						// +0x30
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
	char Unknown08[0x10 - 0x08];						// +0x08
};

// What 0x5860B0 returns (avatar.cpp's neighbourhood: five blend weights from a point in 0 .. 1)
struct UIAvatrWeightsStruct	// NAME 1: neutral (D10)
{
	UIAvatrWeightsStruct() {}
	float Weight[5];									// NAME 1: neutral
};
UIAvatrWeightsStruct Unknown_5860B0(Vector2 uv);		// 0x5860B0 (cdecl)  // NAME 1 [Unknown_5860B0, uv]: neutral (D10)
extern RandomClass Unknown_BDBB84;						// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's

// The +0x8C object's +0x194 (refcounted; +0x110 is 1 when the beard is off, 0x556630)
class UIAvatrUnknown194Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_77E830(const UIAvatrWeightsStruct &weights);	// 0x77E830 (avatar.cpp's neighbourhood; 0x558060)  // NAME 1 [weights]: neutral
	void Unknown_7859F0(int type);						// 0x7859F0 (0x558170: a random one of the +0x2EC object's +0x20)  // NAME 1 [type]: neutral
	void Unknown_77E900(const UIAvatrWeightsStruct &weights);	// 0x77E900 (0x558060)  // NAME 1 [weights]: neutral
	char Unknown08[0x110 - 0x08];						// +0x08
	int Unknown110;										// +0x110
	char Unknown114[0x194 - 0x114];						// +0x114
	int Unknown194;										// +0x194 (0x55B110: the body tab's 0x2A .. 0x2D values)
	int Unknown198;										// +0x198
	int Unknown19C;										// +0x19C
	int Unknown1A0;										// +0x1A0
};

// The +0x8C object's +0x34, as far as 0x554D60 reads it
struct UIAvatrUnknown08CUnknown034Struct	// NAME 1: neutral (D10)
{
	char Unknown00[0x7C];								// +0x00
	bool Unknown7C;										// +0x7C (0x554D60)
};

// The window's +0x8C, as far as this file reads it
class UIAvatrUnknown08CClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_67A9B0(int arg);						// 0x67A9B0 (the window's destructor: 0)  // NAME 1 [arg]: neutral
	bool Unknown_67B2B0();								// 0x67B2B0 (0x554D60: true clears the window's +0x1C5)
	char Unknown00[0x34];								// +0x00
	UIAvatrUnknown08CUnknown034Struct *Unknown034;		// +0x34
	char Unknown38[0x194 - 0x38];						// +0x38
	UIAvatrUnknown194Class *Peek_Unknown194() { return(Unknown194); }	// NAME 1: neutral (D10)
	UIAvatrUnknown194Class *Unknown194;					// +0x194
	char Unknown198[0x1B8 - 0x198];						// +0x198
	RenderObjClass *Peek_Model() { return(Unknown1B8); }	// NAME 1: neutral (D10)
	RenderObjClass *Unknown1B8;							// +0x1B8 (the model: its texts by name)
};

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, unknown]: as eff_o.cpp's
void Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

// The object at *0xBFAEE0 (kernel.cpp's), as far as this file calls it: its slot +0x30 gives a texture by name
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual TextureClass *Get_Texture(const char *name, int a, int b, int c);	// +0x30 (the name, 0, 0, 1)  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

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

// The window's +0x94, as far as 0x556B20 calls it
class UIAvatrUnknown094Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (0x554D60)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74();
	virtual void Unknown_78();							// +0x78 (0x554D60)
	virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84();
	virtual void Unknown_88();							// +0x88 (0x556B20, after +0xDC)
	void Unknown_80C180();								// 0x80C180 (the window's destructor, after +0x80)
	virtual void Unknown_8C();							// +0x8C

	char Unknown04[0x54 - 0x04];						// +0x04
	UIAvatrUnknown094Unknown054Struct *Unknown054;		// +0x54
	char Unknown58[0xDC - 0x58];						// +0x58
	bool Unknown0DC;									// +0xDC (set on the head tab)
};

// fontmanager.h's FontManagerClass at 0xBE67C4 (its header cannot be included next to sclient.h: workflow.md 3.3)
class FontSys : public RenderObjClass { public: void Change_Text(const char *text, int unknown); };	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)  // NAME 1 [text, unknown]: as loadingview.cpp's
class UIAvatrFontManagerClass { public: void Remove(RenderObjClass *object); FontSys *Unknown_6D25D0(const char *name, RenderObjClass *object); };	// NAME 1: neutral (D10): fontmanager.h's Get_Style  // NAME 1 [name, object]: as loadingview.cpp's
extern UIAvatrFontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
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
	char Unknown00[0x20];								// +0x00
	int Unknown20;										// +0x20 (how many head types 0x558170 picks from)
};

// The avatar creation window (ui_avatr.cpp's __FILE__), as far as this file's functions so far use it
class AvatarCreationInterface : public InterfaceClass	// NAME 3: the client's RTTI (0xB7B848: SignalBaseClass::MemberCallback<GadgetClass,AvatarCreationInterface,...>)
{
public:
	virtual ~AvatarCreationInterface();					// 0x554B50 (0x554B20 the deleting one)
	void Unknown_554D60();								// 0x554D60
	void Unknown_555F60(int text);						// 0x555F60 (the palette's texture: 'text1' .. 'text7')  // NAME 1 [text]: neutral
	void Unknown_5560E0();								// 0x5560E0 (the colour pull made)
	void Unknown_556290();								// 0x556290 (the body and head pulls made)
	void Unknown_556630(int tab, bool on);				// 0x556630 (a tab's gadgets and texts shown or hidden: 0 none, 1 head, 2 body)  // NAME 1 [tab, on]: neutral
	void Unknown_556B20(int tab);						// 0x556B20 (the tab: 0 gender, 1 head, 2 body)  // NAME 1 [tab]: neutral
	void Unknown_55A550(int a, int b, int c, int d);	// 0x55A550 (0x556B20: 1, 0, 0, 0 on the head tab, 0, 0, 0, 0 on the body tab)  // NAME 1 [a, b, c, d]: neutral
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
	UIAvatrUnknown08CClass *Unknown08C;					// +0x8C
	char Unknown90[0x94 - 0x90];						// +0x90
	UIAvatrUnknown094Class *Unknown094;					// +0x94
	GadgetClass *Unknown098[50];						// +0x98 (UIAvatrParts' gadgets)
	UIAvatrPullClass *Unknown160;						// +0x160 (the body's pull)
	Vector2 Unknown164;									// +0x164 (0x558060's point)
	UIAvatrPullClass *Unknown16C;						// +0x16C (the head's pull)
	float Unknown170;									// +0x170 (0x558170)
	float Unknown174;									// +0x174 (0x558170)
	float Unknown178;									// +0x178 (the head's scale, 0.5 .. +0x17C: 0x55B540)
	float Unknown17C;									// +0x17C
	UIAvatrPullClass *Unknown180;						// +0x180 (the colour pull, 0x5560E0)
	char Unknown184[0x1BC - 0x184];						// +0x184
	int Unknown1BC;										// +0x1BC (0x55B110: 0 .. 6 from the button)
	int Unknown1C0;										// +0x1C0 (0x55B110: the +0x194 object's value for it)
	char Unknown1C4;									// +0x1C4
	bool Unknown1C5;									// +0x1C5 (0x554D60)
	char Unknown1C6[0x2C8 - 0x1C6];						// +0x1C6
	std::string Unknown2C8;								// +0x2C8 (the destructor's)
	char Unknown2D8[0x2DC - 0x2D8];						// +0x2D8
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
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fa_fe_avatar00.ini");
	Unknown08C->Unknown_67A9B0(0);
	void (AvatarCreationInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &AvatarCreationInterface::Unknown_55C280;
	UIAvatrNotifierClass *notifier = Unknown_65ECE0();
	UIAvatrCallbackClass callback(this, function);
	notifier->Remove(0, &callback);
}

// 0x554D60
void AvatarCreationInterface::Unknown_554D60()
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

// 0x555F60
void AvatarCreationInterface::Unknown_555F60(int text)
{
	const char *names[] = { "text1", "text2", "Text3", "text4", "Text5", "Text6", "text7" };
	RefCountPtr<RenderObjClass> palette = Create_Get(Unknown08C->Peek_Model()->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	if (palette) {
		RefCountPtr<TextureClass> texture = Create_Get(Unknown_BFAEE0->Get_Texture(names[text], 0, 0, 1));
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
						FontSys *text = Unknown_BE67C4->Unknown_6D25D0(UIAvatrParts[i].Text, model.Peek());
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
				RefCountPtr<UIAvatrUnknown194Class> desc = Create_Peek(Unknown08C->Peek_Unknown194());
				if (desc->Unknown110 == 1) {
					Unknown098[18]->Unknown_40(false);
					Unknown098[18]->Unknown_8C(true);
					Unknown098[19]->Unknown_40(false);
					Unknown098[19]->Unknown_8C(true);
					FontSys *text = Unknown_BE67C4->Unknown_6D25D0("beard_fe_avatar00", model.Peek());
					if (text) text->Set_Hidden(true);
				}
			}
			break;
		case 2:
			for (i = 35; i < 50; i++) {
				Unknown098[i]->Unknown_40(on);
				Unknown098[i]->Unknown_8C(!on);
				if (UIAvatrParts[i].Text) {
					FontSys *text = Unknown_BE67C4->Unknown_6D25D0(UIAvatrParts[i].Text, model.Peek());
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
	Unknown08C->Peek_Unknown194()->Unknown_77E900(body);
	Unknown08C->Peek_Unknown194()->Unknown_77E830(face);
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

// 0x55AFD0
void AvatarCreationInterface::Unknown_55AFD0()
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown08C->Peek_Model());
	RefCountPtr<UIAvatrUnknown194Class> desc = Create_Peek(Unknown08C->Peek_Unknown194());
	FontSys *text = Unknown_BE67C4->Unknown_6D25D0("male_fe_avatar00", model.Peek());
	if (Unknown08C->Peek_Unknown194()->Unknown110 == 1) {
		text->Change_Text("Gender: Female", 0);
	} else {
		text->Change_Text("Gender: Male", 0);
	}
}

// 0x55B110
void AvatarCreationInterface::Unknown_55B110(int arg)
{
	RefCountPtr<UIAvatrUnknown194Class> desc = Create_Peek(Unknown08C->Peek_Unknown194());
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
			Unknown1C0 = desc->Unknown194;
			break;
		case 0x2B:
			Unknown1BC = 4;
			Unknown1C0 = desc->Unknown198;
			break;
		case 0x2C:
			Unknown1BC = 5;
			Unknown1C0 = desc->Unknown19C;
			break;
		case 0x2D:
			Unknown1BC = 6;
			Unknown1C0 = desc->Unknown1A0;
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
	TextureClass *texture = Unknown_BFAEE0->Get_Texture(name, 0, 0, 1);
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
