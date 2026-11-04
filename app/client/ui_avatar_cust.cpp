// Earth & Beyond client (reconstruction): app\Client\ui_avatar_cust.cpp. Notes at the end of the file.
#include "sclient.h"
#include "sbaseview.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_template.h"
#include "..\shared\money.h"
#include "..\packets\pkrecustomize.h"
#include "..\packets\pkstarbasetow.h"
#include "ww3d.h"
#include "rendobj.h"
#include "matrix3d.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
#include "..\audio\soundlib.h"

class UIAvatarCustAvatarClass;
#include "aabox.h"
#include "random.h"
extern RandomClass Unknown_BDBB84;					// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's

// The +0xB8 gadget's +0x54 (ui_avatr.cpp's UIAvatrUnknown094Unknown054Struct), as far as 0x54CAE0 calls it
class UIAvatarCustPip054Class	// NAME 1: neutral (D10)
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
	virtual void Unknown_1C4();							// +0x1C4 (0x54CAE0, last)
};

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown; 0x551B20 passes its argument to every gadget)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C(); virtual void Unknown_50();
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60();
	virtual void Unknown_64(AABoxClass &box);			// +0x64 (gadget.h's: the box)  // NAME 1 [box]: gadget.h's
	virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(const RefCountPtr<RefCountClass> &object);	// +0x84 (the window's +0xB8: the avatar after a change, 0x54E690, through a converted temporary; the pointer's class not known)  // NAME 1 [object]: neutral
	virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C (0x54DC20: !on)  // NAME 1 [on]: as ui_avatr.cpp's

	bool Is_Unknown2C_02() { return(Unknown2C_02); }	// (gadget.h's inline)  // NAME 1: neutral (D10)
	static SignalMap &Get_Signal_Map();					// 0x65ECE0 (gadget.h's)
	void Unknown_6603A0(int arg, UIViewClass *view);	// 0x6603A0  // NAME 1 [arg, view]: neutral
	void Unknown_6607D0();								// 0x6607D0

	char Unknown04[0x20 - 0x04];						// +0x04
	void *Flash;										// +0x20 (gadget.h's GadgetFlashStruct *: 0x551320 tests it)  // NAME 3: gadget.h's
	char Unknown24[0x2C - 0x24];						// +0x24
	unsigned int Unknown2C_00 : 2;						// +0x2C
	unsigned int Unknown2C_02 : 1;						// +0x2C bit 2 (gadget.h's: under the pointer; 0x551320's mouse-over sound)
	unsigned int Unknown2C_03 : 7;						// +0x2C
	unsigned int Unknown2C400 : 1;						// +0x2C bit 0x400 (0x54D8A0 sets it on the colour pull's gadget)
	unsigned int Unknown2C800 : 1;						// +0x2C bit 0x800 (0x550670 sets it with 0x6603A0, clears it with 0x6607D0)
	RenderObjClass *Unknown30;							// +0x30 (0x550A10 hands it to 0x497300)
	void Unknown_6608F0();								// 0x6608F0 (0x550A10, after the model's turn)
};

// The window's +0xB8: ship_pip.cpp's gadget (ui_avatr.cpp's ShipPipClass, its +0x94), as far as this file calls it
class UIAvatarCustPipClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C(); virtual void Unknown_50();
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60();
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(const RefCountPtr<RefCountClass> &object);	// +0x84 (the avatar after a change, 0x54E690, through a converted temporary; the pointer's class not known)  // NAME 1 [object]: neutral
	virtual void Unknown_88();							// +0x88 (0x54DFE0, after +0xDC)
	virtual void Unknown_8C();							// +0x8C (0x54DFE0)
	void Unknown_80C180();								// 0x80C180 (the window's destructor, after its slot +0x80, as ui_avatr.cpp's)

	char Unknown04[0x54 - 0x04];						// +0x04
	UIAvatarCustPip054Class *Unknown54;					// +0x54 (the per-frame 0x54CAE0 calls its slot +0x1C4: ui_avatr.cpp's ShipPipClass +0x54)
	char Unknown58[0xB8 - 0x58];						// +0x58
	float UnknownB8, UnknownBC, UnknownC0, UnknownC4, UnknownC8;	// +0xB8 (its model's turn: 0x54E3C0 sets them while a turn button is down)
	char UnknownCC[0xDC - 0xCC];						// +0xCC
	bool UnknownDC;										// +0xDC (0x54DFE0: set in mode 1, the head)
};

// The pulls' gadget (0x80 bytes, constructor 0x671300), as ui_avatr.cpp's UIAvatrGadget671300Class
class UIAvatarCustGadget671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIAvatarCustGadget671300Class(RenderObjClass *area, int a, const LayerClass &layer, int b, int c, int d);	// 0x671300 (0x54D8A0: the palette, 0, the window's layer +0xAC, 0, 0, 0)  // NAME 1 [area, a, layer, b, c, d]: neutral
	void Unknown_671630();								// 0x671630 (the buttons' flags off, as ui_avatr.cpp's)
	char Unknown34[0x64 - 0x34];						// +0x34
	bool Unknown64;										// +0x64 (the left button held, as ui_avatr.cpp's)
	bool Unknown65;										// +0x65 (the right button held)
	char Unknown66[0x80 - 0x66];						// +0x66
};

// gadget.h's FlashingGadgetEvent (its header cannot be included next to this file's GadgetClass)
class FlashingGadgetEvent	// NAME 5: the client's RTTI .?AVFlashingGadgetEvent@@
{
public:
	GadgetClass *Gadget;								// +0x00  // NAME 1: as gadget.h's
};

void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (the pointer, 0..1)  // NAME 1 [x, y]: as gadget.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's

// The client at InterfaceClass +0x40, as this file calls it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp

	char Unknown04[0x112C - 0x04];						// +0x04
	unsigned AvatarID;									// +0x112C (0x54EC30)  // NAME 3: client.h's ClientClass::AvatarID
};

// An object whose first member is a gadget (the window's +0x2FC / +0x308 / +0x31C, as far as 0x551B20 reads them;
// ui_avatr.cpp's pull, UIAvatrPullClass: its destructor 0x585950)
struct UIAvatarCustGadgetHolderStruct	// NAME 1: neutral (D10)
{
	UIAvatarCustGadgetHolderStruct();					// 0x585920 (0x10 bytes)
	~UIAvatarCustGadgetHolderStruct();					// 0x585950 (the window's destructor)
	void Unknown_5859A0();								// 0x5859A0 (once the gadget is in)
	GadgetClass *Unknown00;								// +0x00
	RenderObjClass *Unknown04;							// +0x04 (the pull: 'PULL_COLOR')
	bool Unknown08;										// +0x08 (dragged: 0x54F390)
	char Unknown09[0x10 - 0x09];						// +0x09
};

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
void Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

// The asset manager (*0xBFAEE0)
#include "assetmgr.h"	// WW3DAssetManager: 0xBFAEE0 is WW3DAssetManager::TheInstance (its constructor 0x91C2C0 stores it)







extern GadgetClass *Unknown_BE5F24;					// NAME 1 [Unknown_BE5F24]: 0xBE5F24 (0x54E3C0: a gadget held down, as 0xBE5F28)
extern GadgetClass *Unknown_BE5F28;					// NAME 1 [Unknown_BE5F28]: 0xBE5F28


// fontmanager.h's FontManagerClass at 0xBE67C4 (its header cannot be included next to sclient.h: workflow.md 3.3)
class FontSys : public RenderObjClass { public: void Change_Text(const char *text, int unknown); };	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)  // NAME 1 [text, unknown]: as loadingview.cpp's
class FontManagerClass { public: void Remove(RenderObjClass *object); FontSys *Get_Style(const char *name, RenderObjClass *object); };	// NAME 1: neutral (D10): fontmanager.h's Get_Style  // NAME 1 [name, object]: as loadingview.cpp's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4
// fontdatabase.h's FontStringFilesClass at 0xBE67BC (sclient.h declares the pointer), as ui_avatr.cpp does
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's
extern bool Unknown_BB9654;							// NAME 1 [Unknown_BB9654]: neutral (D10) (tsk_login.cpp's); the constructor clears it, the destructor sets it

// The gadget events' callbacks (SignalBaseClass's, unknown_signalbase.h: vtable 0xAEDB58)
#include "unknown_signalbase.h"

// money.cpp's comparisons (cdecl, not rebuilt)
bool operator==(const MoneyClass &a, const MoneyClass &b);	// 0x4F0600  // NAME 1 [a, b]: neutral
bool operator>=(const MoneyClass &a, const MoneyClass &b);	// 0x4F0740  // NAME 1 [a, b]: neutral

// The window's buttons and their texts (0xB7A324: 40 entries, the gadgets' order at +0x1CC / +0x204 / +0x240; the
// metal models' buttons are their bones, 0x550A10)
struct UIAvatarCustPartStruct	// NAME 1: neutral (D10)
{
	const char *Button;		// NAME 1: neutral (also the metal models' bone: 0x550A10 hands it to Get_Bone_Index)
	const char *Text;		// NAME 1: neutral
};
static UIAvatarCustPartStruct UIAvatarCustParts[40] = {	// NAME 1: neutral (D10)
	{ "head_tab", "head_buy_avatar00" },
	{ "body_tab", "physique_buy_avatar00" },
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
	{ "review_change", "accept_buy_avatar00" },
	{ "cancel", "cancel_buy_avatar00" },
	{ "hair_left", "style_buy_avatar00" },
	{ "hair_right", "hair_title_buy_avatar00" },
	{ "beard_left", "beard_buy_avatar00" },
	{ "beard_right", NULL },
	{ "tattoo_left", "details_buy_avatar00" },
	{ "tattoo_right", "details_buy_avatar00" },
	{ "eye_left", "glasses_buy_avatar00" },
	{ "eye_right", "glasses_buy_avatar00" },
	{ "ear_left", "ear_buy_avatar00" },
	{ "ear_right", "ear_buy_avatar00" },
	{ "hair_color", "hair_color_buy_avatar00" },
	{ "p_eye_color", "eye_color_buy_avatar00" },
	{ "p_skin_color", "skin_color_buy_avatar00" },
	{ "scale_in", NULL },
	{ "scale_out", "acc_title_buy_avatar00" },
	{ "shirt_style_left", "shirt_style_buy_avatar00" },
	{ "shirt_style_right", NULL },
	{ "pant_style_left", "pant_style_buy_avatar00" },
	{ "pant_style_right", "cloth_title_buy_avatar00" },
	{ "p_shirt_color", "p_shirt_color_buy_avatar00" },
	{ "s_shirt_color", "s_shirt_color_buy_avatar00" },
	{ "p_pant_color", "p_pant_color_buy_avatar00" },
	{ "s_pant_color", "s_pant_color_buy_avatar00" },
	{ "metal00", "metals_buy_avatar00" },
	{ "metal01", NULL },
	{ "metal02", NULL },
};
static const char *UIAvatarCustGenders[2] = { "AM", "AF" };	// 0xB7A464: the models' gender prefix (as ui_avatr.cpp's)  // NAME 1: neutral (D10)
static const char *UIAvatarCustRaces[3] = { "T", "J", "P" };	// 0xB7A46C: the models' race prefix  // NAME 1: neutral (D10)

// avatar.cpp's avatar, as far as this file calls it (the window's +0xBC)
class UIAvatarCustAvatarClass : public RefCountClass	// NAME 1: neutral (D10); a RefCountClass (0x54E690 releases it through slot 0, its count at +0x04)
{
public:
	void Unknown_77FC20(int value, bool unknown);		// 0x77FC20 (0x54E950: +0x368's value, 1)  // NAME 1 [value, unknown]: neutral
	void Unknown_77FD30(int value, bool unknown);		// 0x77FD30 (0x54EAD0: +0x36C's value, 1)  // NAME 1 [value, unknown]: neutral
	void Unknown_77E9D0(int body, int pants, bool unknown);	// 0x77E9D0 (0x54E690 / 0x54E7F0: Body_Type and Pants_Type, 1)  // NAME 1 [body, pants, unknown]: AvatarDescClass's fields
	void Unknown_780790(int part, int value);			// 0x780790 (0x550970: 1-4 for the window's parts 3-6)  // NAME 1 [part, value]: neutral
	void Unknown_785C00(const Vector3 &offset);		// 0x785C00 (avatar.cpp's: the tattoo offset; 0x54F6F0)  // NAME 1 [offset]: avatar.cpp's
	void Unknown_780810(EnvironmentMapManagerClass *unknown);	// 0x780810 (0x550970 passes the client's EnvironmentMapManager)  // NAME 1 [unknown]: neutral

	char Unknown08[0xF8 - 0x08];						// +0x08
	AvatarDescClass *Get_Desc() { return(&Unknown0F8); }	// NAME 1: neutral (D10)
	AvatarDescClass Unknown0F8;							// +0xF8 (the new look: 0x551BF0 prices it against the window's +0xC0)
	char Unknown204[0x214 - 0x204];						// +0x204
	bool Unknown214;									// +0x214 (0x54DFE0 sets it)
	char Unknown215[0x231 - 0x215];						// +0x215
	bool Unknown231;									// +0x231 (0x54DFE0: set in mode 1, the head)
};

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_Changed(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10)

// What the window holds at +0x368..+0x378 (its destructor 0x585BF0 not inlined; 0x54F2B0 deletes them)
class UIAvatarCustUnknown368Class	// NAME 1: neutral (D10)
{
public:
	~UIAvatarCustUnknown368Class();						// 0x585BF0
	UIAvatarCustUnknown368Class(RenderObjClass *model, const char *name, const char *texture, const char *part, bool two_digits);	// 0x585B50 (0x24 bytes)  // NAME 1 [model, name, texture, part, two_digits]: as ui_avatr.cpp's
	void Unknown_585CF0(int choice);					// 0x585CF0  // NAME 1 [choice]: as ui_avatr.cpp's
	void Unknown_585D10();								// 0x585D10 (how many choices exist)
	void Unknown_585C90();								// 0x585C90 (0x54EAD0: any direction but 1)
	void Unknown_585CC0();								// 0x585CC0 (0x54EAD0: direction 1)

	char Unknown00[0x18];								// +0x00
	int Unknown18;										// +0x18 (the value: 0x54EAD0 hands it to the avatar's 0x77FD30)
	char Unknown1C[0x24 - 0x1C];						// +0x1C (0x24 bytes: 0x54EEC0)
};

// Two floats the window keeps seven of (+0x320, 0.5 each: the constructor's loop)
struct UIAvatarCustPairStruct	// NAME 1: neutral (D10)
{
	void Set(float x, float y) { Unknown00 = x; Unknown04 = y; }	// NAME 1 [Set, x, y]: neutral
	float Unknown00;									// +0x00
	float Unknown04;									// +0x04
};

// The avatar customisation window (ui_avatar_cust.cpp's __FILE__), as far as this file's functions so far use it
class StarbaseAvatarCreationInterface : public Unknown5BB320Class	// NAME 3: the client's RTTI (0xB7AC88: SignalBaseClass::MemberCallback<GadgetClass,StarbaseAvatarCreationInterface,...>, the FlashingGadgetEvent handler 0x551320's)
{
public:
	StarbaseAvatarCreationInterface(UIViewClass *view, LayerClass *layer);	// 0x54B7B0 (0x3D0 bytes; the starbase view's 0x69F8F0 passes itself and its +0x7C)  // NAME 1 [view, layer]: what 0x69F8F0 passes
	~StarbaseAvatarCreationInterface();					// 0x54BAB0 (table 0xAEF6AC; the deleting one 0x54BA80)
	void Unknown_551320(FlashingGadgetEvent *event, GadgetClass *gadget);	// 0x551320 (FAIL_IFs '!event', '!gadget': the FlashingGadgetEvent handler)  // NAME 2 [event, gadget]: the FAIL_IFs'
	virtual void Unknown_54();							// +0x54 0x54BD30 (InterfaceClass's: Init calls it after construction)
	virtual void Unknown_60();							// +0x60 0x54CAE0 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x54CBC0 (the events: the gadgets' ids)  // NAME 1 [key]: InterfaceClass's
	void Set_Unknown30C(float u, float v) { Unknown30C = u; Unknown310 = v; }	// NAME 1: neutral (D10): inline (0x54F390 computes v first)  // NAME 1 [u, v]: neutral
	void Unknown_54F390(int arg);						// 0x54F390 (0x54CAE0 in mode 1: 1; the head pull dragged: its point +0x30C)  // NAME 1 [arg]: neutral
	void Unknown_54FAA0();								// 0x54FAA0 (0x54CAE0)
	void Unknown_54D720(int text);						// 0x54D720 (the palette's texture: 'text1' .. 'text7', as ui_avatr.cpp's 0x555F60)  // NAME 1 [text]: neutral
	void Unknown_54D8A0();								// 0x54D8A0 (the colour pull made, +0x31C; as ui_avatr.cpp's 0x5560E0)
	void Unknown_54DA50();								// 0x54DA50 (the head pull made, +0x308; as ui_avatr.cpp's 0x556290)
	void Unknown_54DFE0(int mode);						// 0x54DFE0 (the mode +0xB0: 1 the head, 2 the clothes; the palette helper 0x5513E0 inlined)  // NAME 1 [mode]: neutral
	void Unknown_54E3C0();								// 0x54E3C0
	void Unknown_54E610();								// 0x54E610 (0x54E3C0, then the gadgets hidden while the view's +0x34 is active)
	void Unknown_54F2B0();								// 0x54F2B0 (+0x368..+0x378 deleted)
	void Unknown_54EEC0();								// 0x54EEC0 (+0x368..+0x378 made: hair, beard, tattoo, goggle, ear)
	void Unknown_54F6F0(int unused, bool random, float du, float dv);	// 0x54F6F0 (the head pull's point +0x30C moved by du, dv in 0..1, or a random one; the avatar's tattoo offset from it; 0x550550: 1, 0, 0, 0)  // NAME 1 [unused, random, du, dv]: neutral
	void Unknown_5502F0();								// 0x5502F0 (the tattoo's FacialFeature: the head pull shown when it moves, the zoom's largest +0x318)
	void Unknown_550550(int dir);						// 0x550550 (the zoom +0x314 a thirtieth out (1) or in (2), between 0.5 and +0x318; the zoom buttons +0x238 / +0x23C)  // NAME 1 [dir]: neutral
	void Unknown_54E690(int dir);						// 0x54E690 (the body type one on (2) or back (1), of three)  // NAME 1 [dir]: as 0x550550's
	void Unknown_54E7F0(int dir);						// 0x54E7F0 (the pants type one on (2) or back (1), of three)  // NAME 1 [dir]: as 0x550550's
	void Unknown_54EAD0(int dir);						// 0x54EAD0 (+0x36C one on (1) or back, its value onto the avatar)  // NAME 1 [dir]: as 0x550550's
	void Unknown_54FFC0();								// 0x54FFC0
	void Unknown_550140(int id);						// 0x550140 (the part buttons 0x1A-0x1C, 0x24-0x27: the part +0x358, a metal's +0x35C)  // NAME 1 [id]: neutral
	void Unknown_54E950(int dir);						// 0x54E950 (+0x368 one on (1) or back, skipping 0 for Gender 1, its value onto the avatar)  // NAME 1 [dir]: as 0x550550's
	void Unknown_54DC20(int mode, bool on);				// 0x54DC20 (0x54EE40 / 0x54EE80: the mode +0xB0, shown or not)  // NAME 1 [mode, on]: neutral
	void Unknown_54EE40();								// 0x54EE40 (no direct caller: a table's; 0x54DC20 with the mode, on, and +0xB8 shown)
	void Unknown_54EE80();								// 0x54EE80 (as 0x54EE40, off)
	void Unknown_550670();								// 0x550670 (the mode and part buttons' state)
	void Unknown_550970(int value);						// 0x550970  // NAME 1 [value]: neutral
	void Unknown_551BF0();								// 0x551BF0 (the cost of the changes: recustomization_cost.cpp's 0x8493F0)
	void Unknown_550A10();								// 0x550A10 (the three metal models turn, the mesh's bones follow)
	void Unknown_54EC30();								// 0x54EC30 (0x551A80, on the confirmation's first button)
	void Unknown_5513E0(const char *name);				// 0x5513E0 (the texture of that name onto the mesh's COLOR_PALETTE)  // NAME 1 [name]: neutral
	void Unknown_551520(bool any);						// 0x551520 (the credits and cost texts)  // NAME 1 [any]: neutral
	void Unknown_551900();								// 0x551900 (the confirmation box)
	void Unknown_551A80();								// 0x551A80 (the boxes' answers)
	void Unknown_551B20(bool on);						// 0x551B20 (every gadget shown or hidden)  // NAME 1 [on]: neutral
	void Unknown_551C40(const RecustomizationCostClass &cost);	// 0x551C40 (the costs kept, +0x3CC set)  // NAME 1 [cost]: neutral
	void Unknown_551C80();								// 0x551C80 (STARBASE_REQUEST, action 10)

	LayerClass *Unknown0AC;								// +0xAC (the constructor's layer)
	int Unknown0B0;										// +0xB0 (the mode: 1, 2)
	SBaseViewClass *Unknown0B4;							// +0xB4 (its Client's EnvironmentMapManager; 0x551A80 calls 0x69F440)
	UIAvatarCustPipClass *Unknown0B8;					// +0xB8 (ship_pip.cpp's gadget)
	RefCountPtr<UIAvatarCustAvatarClass> Unknown0BC;	// +0xBC (a RefCountPtr: the destructor 0x54BAB0 releases it twice, by hand and as a member)
	AvatarDescClass Unknown0C0;							// +0xC0 (the old look: 0x551BF0)
	GadgetClass *Unknown1CC[14];						// +0x1CC ([0] on in mode 1, [1] in mode 2)
	GadgetClass *Unknown204[15];						// +0x204 ([10]-[12]: parts 0-2)
	GadgetClass *Unknown240[11];						// +0x240 ([4]-[7]: parts 3-6; [8]-[10] the metal models' gadgets)
	Matrix3D Unknown26C[3];								// +0x26C (the models' turning frames; the constructor's loop)
	UIAvatarCustGadgetHolderStruct *Unknown2FC;			// +0x2FC
	Vector2 Unknown300;									// +0x300 (0.5, 0.5)
	UIAvatarCustGadgetHolderStruct *Unknown308;			// +0x308
	float Unknown30C;									// +0x30C (0.5)
	float Unknown310;									// +0x310 (0.5)
	Vector2 Unknown314;									// +0x314 (X the zoom: 0x550550; Y its largest)
	UIAvatarCustGadgetHolderStruct *Unknown31C;			// +0x31C
	UIAvatarCustPairStruct Unknown320[7];	// +0x320 (0.5, 0.5 each: the constructor's loop)
	int Unknown358;										// +0x358 (the part, 0-6)
	int Unknown35C;										// +0x35C
	bool Unknown360;									// +0x360
	bool Unknown361;									// +0x361 (0x54E610: set while the gadgets are hidden)
	bool Unknown362;									// +0x362
	bool Unknown363;									// +0x363
	bool Unknown364;									// +0x364
	bool Unknown365;									// +0x365
	char Unknown366[0x368 - 0x366];						// +0x366
	UIAvatarCustUnknown368Class *Unknown368;			// +0x368
	UIAvatarCustUnknown368Class *Unknown36C;			// +0x36C
	UIAvatarCustUnknown368Class *Unknown370;			// +0x370
	UIAvatarCustUnknown368Class *Unknown374;			// +0x374
	UIAvatarCustUnknown368Class *Unknown378;			// +0x378
	char Unknown37C[0x380 - 0x37C];						// +0x37C
	MoneyClass Unknown380;								// +0x380 (the credits: the AuxData's Money)
	MoneyClass Unknown388;								// +0x388 (the changes' cost)
	RecustomizationCostClass Unknown390;				// +0x390 (the costs per slot: 0x551C40)
	bool Unknown3CC;									// +0x3CC (set once the costs are in)
};

inline void UIAvatarCust_Set(float &dst, float value) { dst = value; }	// NAME 1: neutral (D10): the non-zero turns go through a float argument (the client stores them with fld / fstp, the zeros from edi); a plain assignment copies them through eax (0.44)
inline bool UIAvatarCust_Held(GadgetClass *gadget) { return(Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget); }	// NAME 1: neutral (D10)

static const float UIAvatarCustTurn = 2.0f;				// 0xAEF670 (0x54E3C0: the model's turn, +0xB8)  // NAME 1: neutral (D10)
static const float UIAvatarCustTilt = 0.75f;			// 0xAEF674 (+0xC0 / +0xC4)  // NAME 1: neutral (D10)
static const float UIAvatarCustRise = 1.0f;				// 0xAEF678 (+0xC8)  // NAME 1: neutral (D10)

// The window's FlashingGadgetEvent callback: SignalBaseClass::MemberCallback (vtable 0xAEF758 over 0xAEDB58), as ui_avatr.cpp's
typedef SignalBaseClass::MemberCallback<GadgetClass, StarbaseAvatarCreationInterface, void (StarbaseAvatarCreationInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> UIAvatarCustCallbackClass;	// NAME 1 [UIAvatarCustCallbackClass]: neutral (D10)

// 0x54B7B0
StarbaseAvatarCreationInterface::StarbaseAvatarCreationInterface(UIViewClass *view, LayerClass *layer) :
	Unknown5BB320Class(new UIMeshClass("buy_avatar00", -1), view, *layer),
	Unknown0AC(layer),
	Unknown0B0(3),
	Unknown0B4((SBaseViewClass *)view->Unknown_30()),
	Unknown0B8(NULL),
	Unknown2FC(NULL),
	Unknown300(0.5f, 0.5f),
	Unknown308(NULL),
	Unknown30C(0.5f),
	Unknown310(0.5f),
	Unknown314(1.0f, 1.0f),
	Unknown31C(NULL),
	Unknown358(0),
	Unknown35C(-1),
	Unknown360(false),
	Unknown361(false),
	Unknown362(false),
	Unknown363(false),
	Unknown364(false),
	Unknown365(false),
	Unknown368(NULL),
	Unknown36C(NULL),
	Unknown370(NULL),
	Unknown374(NULL),
	Unknown378(NULL),
	Unknown3CC(false)
{
	int i;
	for (i = 0; i < 14; i++) {
		Unknown1CC[i] = NULL;
	}
	for (i = 0; i < 15; i++) {
		Unknown204[i] = NULL;
	}
	for (i = 0; i < 11; i++) {
		Unknown240[i] = NULL;
	}
	for (i = 0; i < 7; i++) {
		Unknown320[i].Set(0.5f, 0.5f);
	}
	for (int j = 0; j < 3; j++) {
		Unknown26C[j].Make_Identity();
	}
	Unknown_BB9654 = false;
}

// NOT EXACT: inline budget (482 bytes against 0x1F1: the client calls its callback's base constructor out of line,
// 0x551EE0, which stores vtable 0xAEF788: an intermediate class over SignalBaseClass::CallbackClass with Event_Type
// 0x53D130 and Is_Equal 0x551F00 (Type() == that.Type()); a template EventCallbackClass<EVENT> between them in
// unknown_signalbase.h still inlines, with 6 or 20 dead inline reads before it and with a temporary callback;
// attempts.py show 0x54BAB0)
// 0x54BAB0
StarbaseAvatarCreationInterface::~StarbaseAvatarCreationInterface()
{
	Unknown_BB9654 = true;
	Unknown_5C();
	Unknown0B4->Unknown_69DC60();
	if (Unknown31C) {
		delete Unknown31C;
		Unknown31C = NULL;
	}
	if (Unknown308) {
		delete Unknown308;
		Unknown308 = NULL;
	}
	if (Unknown2FC) {
		delete Unknown2FC;
		Unknown2FC = NULL;
	}
	Unknown0B8->Unknown_80();
	Unknown0B8->Unknown_80C180();
	if (Unknown0BC) {
		Unknown0BC = NULL;
	}
	Unknown64.Unknown_687150((GadgetClass *)Unknown0B8);
	Unknown_54F2B0();
	Unknown_BE67C4->Remove(Mesh->Peek_Object());
	Unknown_BE67BC->Unload("fa_buy_avatar00.ini");
	void (StarbaseAvatarCreationInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &StarbaseAvatarCreationInterface::Unknown_551320;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	UIAvatarCustCallbackClass callback(this, function);
	notifier->disconnect(0, callback);
}

// 0x54CAE0
void StarbaseAvatarCreationInterface::Unknown_60()
{
	Unknown_54E610();
	Unknown_551A80();
	if (Unknown0B0 == 1) {
		Unknown_54F390(1);
	}
	Unknown_551520(false);
	Unknown_54FAA0();
	Unknown_550A10();
	Unknown0B8->Unknown_78();
	Unknown0B8->Unknown54->Unknown_1C4();
	Unknown5BB320Class::Unknown_60();
}

// 0x54D720
void StarbaseAvatarCreationInterface::Unknown_54D720(int text)
{
	const char *names[] = { "text1", "text2", "Text3", "text4", "Text5", "Text6", "text7" };
	RefCountPtr<RenderObjClass> palette = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	if (palette) {
		RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(names[text], MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
		if (texture) {
			Unknown_495FD0(palette.Peek(), texture.Peek(), 0);
		}
	}
}

// 0x54D8A0
void StarbaseAvatarCreationInterface::Unknown_54D8A0()
{
	Unknown31C = new UIAvatarCustGadgetHolderStruct;
	RenderObjClass *palette = Mesh->Peek_Object()->Get_Sub_Object_By_Name("COLOR_PALETTE");
	Unknown_496AE0(palette, 0);
	Unknown31C->Unknown00 = new UIAvatarCustGadget671300Class(palette, 0, *Unknown0AC, 0, 0, 0);
	Unknown31C->Unknown00->Unknown2C400 = true;
	Unknown31C->Unknown_5859A0();
	palette->Release_Ref();
	Unknown31C->Unknown04 = Mesh->Peek_Object()->Get_Sub_Object_By_Name("PULL_COLOR");
	Unknown_496AE0(Unknown31C->Unknown04, 1);
	Unknown64.Unknown_6870E0(Unknown31C->Unknown00, 0x2F, View, 1, 1);
	Unknown_54FFC0();
}

// 0x54DA50
void StarbaseAvatarCreationInterface::Unknown_54DA50()
{
	Unknown308 = new UIAvatarCustGadgetHolderStruct;
	RefCountPtr<RenderObjClass> area = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name("HEAD_AREA"));
	Unknown_496AE0(area.Peek(), 0);
	Unknown308->Unknown00 = new UIAvatarCustGadget671300Class(area.Peek(), 0, *Unknown0AC, 0, 0, 0);
	Unknown308->Unknown00->Unknown2C400 = true;
	Unknown308->Unknown_5859A0();
	Unknown308->Unknown04 = Mesh->Peek_Object()->Get_Sub_Object_By_Name("HEAD_PULL");
	Unknown_496AE0(Unknown308->Unknown04, 1);
	Unknown64.Unknown_6870E0(Unknown308->Unknown00, 0x2E, View, 1, 1);
}

// 0x54DC20
void StarbaseAvatarCreationInterface::Unknown_54DC20(int mode, bool on)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(Mesh->Peek_Object());
	int i;
	switch (mode) {
		case 1:
			for (i = 14; i < 29; i++) {
				Unknown204[i - 14]->Unknown_40(on);
				Unknown204[i - 14]->Unknown_8C(!on);
				if (UIAvatarCustParts[i].Text) {
					FontSys *text = Unknown_BE67C4->Get_Style(UIAvatarCustParts[i].Text, model.Peek());
					if (text) text->Set_Hidden(!on);
				}
			}
			if (Unknown31C) {
				Unknown31C->Unknown00->Unknown_40(on);
				Unknown31C->Unknown00->Unknown_8C(!on);
				Unknown31C->Unknown04->Set_Hidden(!on);
			}
			if (Unknown2FC) {
				Unknown2FC->Unknown00->Unknown_40(false);
				Unknown2FC->Unknown00->Unknown_8C(true);
				Unknown2FC->Unknown04->Set_Hidden(true);
			}
			if (Unknown0BC->Get_Desc()->Gender == 1) {
				Unknown204[2]->Unknown_40(false);
				Unknown204[2]->Unknown_8C(true);
				Unknown204[3]->Unknown_40(false);
				Unknown204[3]->Unknown_8C(true);
				FontSys *text = Unknown_BE67C4->Get_Style("beard_buy_avatar00", model.Peek());
				if (text) text->Set_Hidden(true);
			}
			break;
		case 2:
			for (i = 29; i < 40; i++) {
				Unknown240[i - 29]->Unknown_40(on);
				Unknown240[i - 29]->Unknown_8C(!on);
				if (UIAvatarCustParts[i].Text) {
					FontSys *text = Unknown_BE67C4->Get_Style(UIAvatarCustParts[i].Text, model.Peek());
					if (text) text->Set_Hidden(!on);
				}
			}
			if (Unknown31C) {
				Unknown31C->Unknown00->Unknown_40(on);
				Unknown31C->Unknown00->Unknown_8C(!on);
				Unknown31C->Unknown04->Set_Hidden(!on);
			}
			if (Unknown2FC) {
				Unknown2FC->Unknown00->Unknown_40(false);
				Unknown2FC->Unknown00->Unknown_8C(true);
				Unknown2FC->Unknown04->Set_Hidden(true);
			}
			if (Unknown308) {
				Unknown308->Unknown00->Unknown_40(false);
				Unknown308->Unknown00->Unknown_8C(true);
				Unknown308->Unknown04->Set_Hidden(true);
			}
			break;
	}
}

// NOT EXACT: inline budget (248 instructions against 235: the client calls the palette helper's two ~RefCountPtr out
// of line, 0x4740B0, in both inlined copies of 0x5513E0, as ui_avatr.cpp's 0x556B20; dead reads before the helper or
// in it (1-2) change nothing, 3 keep the helper out of line)
// 0x54DFE0
void StarbaseAvatarCreationInterface::Unknown_54DFE0(int mode)
{
	if (Unknown0B0 == mode && (mode == 1 || mode == 2)) {
		return;
	}
	Unknown0B0 = mode;
	RefCountPtr<RenderObjClass> model = Unknown_497300(Mesh->Peek_Object());
	RefCountPtr<RenderObjClass> frame = Create_Get(model->Get_Sub_Object_By_Name("FRAME_COLOR"));
	switch (mode) {
		case 1:
			Unknown_54DC20(2, false);
			Unknown_54DC20(1, true);
			Unknown_5502F0();
			Unknown_54F6F0(1, false, 0, 0);
			if (frame) frame->Set_Hidden(false);
			Unknown_550140(0x1A);
			Unknown_5513E0("fa_avatar_color.tga");
			Unknown0B8->UnknownDC = true;
			Unknown0B8->Unknown_88();
			Unknown0B8->Unknown_8C();
			Unknown0BC->Unknown231 = true;
			Unknown0BC->Unknown214 = true;
			break;
		case 2:
			Unknown_54DC20(1, false);
			Unknown_54DC20(2, true);
			if (frame) frame->Set_Hidden(false);
			Unknown_550140(0x24);
			Unknown_5513E0("fa_avatar_color.tga");
			Unknown0B8->UnknownDC = false;
			Unknown0B8->Unknown_88();
			Unknown0B8->Unknown_8C();
			Unknown0BC->Unknown231 = false;
			Unknown0BC->Unknown214 = true;
			break;
	}
}

// 0x54E3C0
void StarbaseAvatarCreationInterface::Unknown_54E3C0()
{
	if (UIAvatarCust_Held(Unknown1CC[3])) {
		UIAvatarCust_Set(Unknown0B8->UnknownB8, -UIAvatarCustTurn);
	} else if (UIAvatarCust_Held(Unknown1CC[4])) {
		UIAvatarCust_Set(Unknown0B8->UnknownB8, UIAvatarCustTurn);
	} else {
		Unknown0B8->UnknownB8 = 0;
		Unknown0B8->UnknownBC = 0;
	}
	if (UIAvatarCust_Held(Unknown1CC[11])) {
		UIAvatarCust_Set(Unknown0B8->UnknownC0, -UIAvatarCustTilt);
	} else if (UIAvatarCust_Held(Unknown1CC[10])) {
		UIAvatarCust_Set(Unknown0B8->UnknownC0, UIAvatarCustTilt);
	} else if (UIAvatarCust_Held(Unknown1CC[8])) {
		UIAvatarCust_Set(Unknown0B8->UnknownC4, -UIAvatarCustTilt);
	} else if (UIAvatarCust_Held(Unknown1CC[9])) {
		UIAvatarCust_Set(Unknown0B8->UnknownC4, UIAvatarCustTilt);
	} else {
		Unknown0B8->UnknownC0 = 0;
		Unknown0B8->UnknownC4 = 0;
	}
	if (UIAvatarCust_Held(Unknown1CC[5])) {
		UIAvatarCust_Set(Unknown0B8->UnknownC8, -UIAvatarCustRise);
	} else if (UIAvatarCust_Held(Unknown1CC[6])) {
		UIAvatarCust_Set(Unknown0B8->UnknownC8, UIAvatarCustRise);
	} else {
		Unknown0B8->UnknownC8 = 0;
	}
	if (Unknown0B0 == 1) {
		if (UIAvatarCust_Held(Unknown204[13])) {
			Unknown_550550(1);
			return;
		}
		if (UIAvatarCust_Held(Unknown204[14])) {
			Unknown_550550(2);
		}
	}
}

// 0x54E610
void StarbaseAvatarCreationInterface::Unknown_54E610()
{
	Unknown_54E3C0();
	if (Unknown0B4->Unknown34->Active) {
		if (!Unknown361) {
			Unknown361 = true;
			Unknown_551B20(false);
			return;
		}
	}
	if (!Unknown0B4->Unknown34->Active && Unknown361) {
		Unknown361 = false;
		Unknown_551B20(true);
		Unknown_550670();
	}
}

// NOT EXACT: register (0.85, 271 bytes against 0x10F: the view's Client into edx where the client keeps eax; Get_Client() instead of Client 0.74, and a local, Peek() or the slot's object in a named pointer don't change it; attempts.py show 0x54E690)
// 0x54E690
void StarbaseAvatarCreationInterface::Unknown_54E690(int dir)
{
	int body = Unknown0BC->Get_Desc()->Body_Type;
	int pants = Unknown0BC->Get_Desc()->Pants_Type;
	if (dir == 2) {
		body = (body + 1) % 3;
	} else if (dir == 1) {
		if (--body < 0) {
			body = 2;
		}
	}
	Unknown0BC->Unknown_77E9D0(body, pants, true);
	Unknown0BC->Unknown_780810(Unknown0B4->Client->EnvironmentMapManager);
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// NOT EXACT: register (0.85: as 0x54E690's, its twin for Pants_Type)
// 0x54E7F0
void StarbaseAvatarCreationInterface::Unknown_54E7F0(int dir)
{
	int body = Unknown0BC->Get_Desc()->Body_Type;
	int pants = Unknown0BC->Get_Desc()->Pants_Type;
	if (dir == 2) {
		pants = (pants + 1) % 3;
	} else if (dir == 1) {
		if (--pants < 0) {
			pants = 2;
		}
	}
	Unknown0BC->Unknown_77E9D0(body, pants, true);
	Unknown0BC->Unknown_780810(Unknown0B4->Client->EnvironmentMapManager);
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// NOT EXACT: register (0.69: dir in ebp where the client keeps edi, then as 0x54E690's; attempts.py show 0x54E950)
// 0x54E950
void StarbaseAvatarCreationInterface::Unknown_54E950(int dir)
{
	do {
		if (dir == 1) {
			Unknown368->Unknown_585CC0();
		} else {
			Unknown368->Unknown_585C90();
		}
	} while (Unknown0BC->Get_Desc()->Gender == 1 && Unknown368->Unknown18 == 0);
	Unknown0BC->Unknown_77FC20(Unknown368->Unknown18, true);
	Unknown0BC->Unknown_780810(Unknown0B4->Client->EnvironmentMapManager);
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// NOT EXACT: register (0.81: as 0x54E690's, the view's Client chain)
// 0x54EAD0
void StarbaseAvatarCreationInterface::Unknown_54EAD0(int dir)
{
	if (dir == 1) {
		Unknown36C->Unknown_585CC0();
	} else {
		Unknown36C->Unknown_585C90();
	}
	Unknown0BC->Unknown_77FD30(Unknown36C->Unknown18, true);
	Unknown0BC->Unknown_780810(Unknown0B4->Client->EnvironmentMapManager);
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// NOT EXACT: register (0.77, 273 bytes against 0x112: the client loads Client's AvatarID through eax / ecx where VC6 here takes edx, and the packet's look into esi; attempts.py show 0x54EC30)
// 0x54EC30
void StarbaseAvatarCreationInterface::Unknown_54EC30()
{
	SClient *client = Client->As_Space_Client();
	client->Unknown_74C530(Client->AvatarID);
	client->Unknown_74C3B0(Client->AvatarID, *Unknown0BC->Get_Desc());
	UIStarbaseClass *starbase = ((SBaseViewClass *)client->Unknown135C)->Get_Starbase().Peek();
	if (starbase) {
		starbase->Unknown_6435E0();
	}
	client->Send_Packet(&RecustomizeAvatarDonePacketClass(client->Unknown1138, *Unknown0BC->Get_Desc()));
}

// 0x54EE40
void StarbaseAvatarCreationInterface::Unknown_54EE40()
{
	Unknown_54DC20(Unknown0B0, true);
	if (Unknown0B8) {
		Unknown0B8->Unknown_40(true);
	}
}

// 0x54EE80
void StarbaseAvatarCreationInterface::Unknown_54EE80()
{
	Unknown_54DC20(Unknown0B0, false);
	if (Unknown0B8) {
		Unknown0B8->Unknown_40(false);
	}
}

// 0x54EEC0
void StarbaseAvatarCreationInterface::Unknown_54EEC0()
{
	char name[64];
	Unknown_54F2B0();
	RefCountPtr<RenderObjClass> model = Unknown_497300(Mesh->Peek_Object());
	_snprintf(name, sizeof(name), "%s%shair", UIAvatarCustGenders[Unknown0BC->Get_Desc()->Gender], UIAvatarCustRaces[Unknown0BC->Get_Desc()->Race]);
	Unknown368 = new UIAvatarCustUnknown368Class(model.Peek(), name, NULL, "HAIR", false);
	Unknown368->Unknown_585CF0(Unknown0BC->Get_Desc()->Hair_Num);
	Unknown368->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%sbeard", UIAvatarCustGenders[Unknown0BC->Get_Desc()->Gender], UIAvatarCustRaces[Unknown0BC->Get_Desc()->Race]);
	Unknown36C = new UIAvatarCustUnknown368Class(model.Peek(), name, NULL, "BEARD", false);
	Unknown36C->Unknown_585CF0(Unknown0BC->Get_Desc()->Beard_Num);
	Unknown36C->Unknown_585D10();
	Unknown370 = new UIAvatarCustUnknown368Class(model.Peek(), NULL, "atattoo", "TATTOO", false);
	Unknown370->Unknown_585CF0(Unknown0BC->Get_Desc()->Tattoo_Texture_Num);
	Unknown370->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%sGOGGLE", UIAvatarCustGenders[Unknown0BC->Get_Desc()->Gender], UIAvatarCustRaces[Unknown0BC->Get_Desc()->Race]);
	Unknown374 = new UIAvatarCustUnknown368Class(model.Peek(), name, NULL, "EYE", false);
	Unknown374->Unknown_585CF0(Unknown0BC->Get_Desc()->Goggle_Num);
	Unknown374->Unknown_585D10();
	_snprintf(name, sizeof(name), "%s%sEAR", UIAvatarCustGenders[Unknown0BC->Get_Desc()->Gender], UIAvatarCustRaces[Unknown0BC->Get_Desc()->Race]);
	Unknown378 = new UIAvatarCustUnknown368Class(model.Peek(), name, NULL, "EAR", false);
	Unknown378->Unknown_585CF0(Unknown0BC->Get_Desc()->Ear_Num);
	Unknown378->Unknown_585D10();
}

// 0x54F2B0
void StarbaseAvatarCreationInterface::Unknown_54F2B0()
{
	if (Unknown368) {
		delete Unknown368;
		Unknown368 = NULL;
	}
	if (Unknown36C) {
		delete Unknown36C;
		Unknown36C = NULL;
	}
	if (Unknown370) {
		delete Unknown370;
		Unknown370 = NULL;
	}
	if (Unknown374) {
		delete Unknown374;
		Unknown374 = NULL;
	}
	if (Unknown378) {
		delete Unknown378;
		Unknown378 = NULL;
	}
}

// NOT EXACT: shape (one byte: when +0x64 is clear our first je skips straight to the +0x65 test, the client's re-tests
// +0x64; == true, an inline grab helper and a named gadget local tried. tm[2][3] = 0, the edges declared up front and
// the point stored through an inline setter (v computed first) made the rest the client's)
// 0x54F390
void StarbaseAvatarCreationInterface::Unknown_54F390(int arg)
{
	UIAvatarCustGadgetHolderStruct *pull = Unknown308;
	if (((UIAvatarCustGadget671300Class *)pull->Unknown00)->Unknown64) {
		pull->Unknown08 = true;
	}
	if ((((UIAvatarCustGadget671300Class *)pull->Unknown00)->Unknown64 || ((UIAvatarCustGadget671300Class *)pull->Unknown00)->Unknown65) && pull->Unknown08) {
		AABoxClass box;
		pull->Unknown00->Unknown_64(box);
		float x;
		float y;
		float top, left, width, height;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		x = WWMath::Min(x, box.Center.X + box.Extent.X);
		x = WWMath::Max(x, box.Center.X - box.Extent.X);
		y = WWMath::Min(y, box.Center.Y + box.Extent.Y);
		y = WWMath::Max(y, box.Center.Y - box.Extent.Y);
		RefCountPtr<RenderObjClass> model = Unknown_497300(Mesh->Peek_Object());
		int bone = model->Get_Sub_Object_Bone_Index(pull->Unknown04);
		model->Capture_Bone(bone);
		Matrix3D tm = pull->Unknown04->Get_Transform();
		tm[0][3] = x - box.Center.X;
		tm[1][3] = y - box.Center.Y;
		tm[2][3] = 0.0f;
		model->Control_Bone(bone, tm, false);
		top = box.Center.Y - box.Extent.Y;
		left = box.Center.X - box.Extent.X;
		width = box.Extent.X * 2.0f;
		height = box.Extent.Y * 2.0f;
		Set_Unknown30C((x - left) / width, (y - top) / height);
		Unknown_550550(0);
		Unknown_551BF0();
	} else {
		pull->Unknown08 = false;
	}
	((UIAvatarCustGadget671300Class *)pull->Unknown00)->Unknown_671630();
}

// NOT EXACT: shape (194 instructions against 196: the client copies +0x310 into its local with integer moves before the
// first add, ours loads it onto the FPU; locals u / v, a Vector2 from the two floats and a Vector2 member at +0x30C
// tried, the last breaks the constructor's scheduling; tm[2][3] = 0 is the client's, as 0x54F390's)
// 0x54F6F0
void StarbaseAvatarCreationInterface::Unknown_54F6F0(int unused, bool random, float du, float dv)
{
	UIAvatarCustGadgetHolderStruct *pull = Unknown308;
	Vector2 point(Unknown30C, Unknown310);
	if (du > 0.0f) {
		point.X = WWMath::Min(point.X + du, 1.0f);
	} else {
		point.X = WWMath::Max(point.X + du, 0.0f);
	}
	if (dv > 0.0f) {
		point.Y = WWMath::Min(point.Y + dv, 1.0f);
	} else {
		point.Y = WWMath::Max(point.Y + dv, 0.0f);
	}
	AABoxClass box;
	pull->Unknown00->Unknown_64(box);
	if (random) {
		point.X = Unknown_BDBB84(0, 100) * 0.01f;
		point.Y = Unknown_BDBB84(0, 100) * 0.01f;
	}
	float top = box.Center.Y - box.Extent.Y;
	float left = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f;
	float height = box.Extent.Y * 2.0f;
	RefCountPtr<RenderObjClass> model = Unknown_497300(Mesh->Peek_Object());
	int bone = model->Get_Sub_Object_Bone_Index(pull->Unknown04);
	model->Capture_Bone(bone);
	Matrix3D tm = pull->Unknown04->Get_Transform();
	tm[0][3] = width * point.X + left - box.Center.X;
	tm[1][3] = height * point.Y + top - box.Center.Y;
	tm[2][3] = 0.0f;
	model->Control_Bone(bone, tm, false);
	Unknown30C = point.X;
	Unknown310 = point.Y;
	Unknown0BC->Unknown_785C00(Vector3(point.X, 1.0f - point.Y, Unknown314.X));
}

// NOT EXACT: order-only (292 bytes, the same 91 instructions: Control_Bone's `push 0` comes before the matrix copy, the
// client's after the two translations; the default argument and Set_X/Y_Translation give the same)
// 0x54FFC0
void StarbaseAvatarCreationInterface::Unknown_54FFC0()
{
	AABoxClass box;
	Unknown31C->Unknown00->Unknown_64(box);
	float top = box.Center.Y - box.Extent.Y;
	float left = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f;
	float height = box.Extent.Y * 2.0f;
	RenderObjClass *model = Mesh->Peek_Object();
	int bone = model->Get_Sub_Object_Bone_Index(Unknown31C->Unknown04);
	model->Capture_Bone(bone);
	Matrix3D tm = Unknown31C->Unknown04->Get_Transform();
	tm[0][3] = width * Unknown320[Unknown358].Unknown00 + left - box.Center.X;
	tm[1][3] = height * Unknown320[Unknown358].Unknown04 + top - box.Center.Y;
	model->Control_Bone(bone, tm, false);
}

// 0x550140
void StarbaseAvatarCreationInterface::Unknown_550140(int id)
{
	switch (id) {
		case 0x1A:
			Unknown358 = 0;
			break;
		case 0x1B:
			Unknown358 = 1;
			break;
		case 0x1C:
			Unknown358 = 2;
			break;
		case 0x24:
			Unknown358 = 3;
			Unknown35C = Unknown0BC->Get_Desc()->Shirt_Primary_Metal;
			break;
		case 0x25:
			Unknown358 = 4;
			Unknown35C = Unknown0BC->Get_Desc()->Shirt_Secondary_Metal;
			break;
		case 0x26:
			Unknown358 = 5;
			Unknown35C = Unknown0BC->Get_Desc()->Pants_Primary_Metal;
			break;
		case 0x27:
			Unknown358 = 6;
			Unknown35C = Unknown0BC->Get_Desc()->Pants_Secondary_Metal;
			break;
	}
	Unknown_54FFC0();
	Unknown_550670();
}

// NOT EXACT: shape (131 instructions against 132: the client keeps the two bools the other way round on the stack,
// movable at +0xC and zoom at +8, and writes zoom's 0 as an immediate; `zoom = true; if (!(...)) zoom = false;` gives 132
// with the slots still swapped, and declaring zoom first or movable after scale changes nothing)
// 0x5502F0
void StarbaseAvatarCreationInterface::Unknown_5502F0()
{
	AvatarDescClass desc(*Unknown0BC->Get_Desc());
	bool movable;
	int scale;
	desc.Get_Facial_Feature(movable, scale);
	Unknown314.Y = (float)scale;
	if (Unknown0B0 == 1) {
		Unknown308->Unknown00->Unknown_40(movable);
		Unknown308->Unknown00->Unknown_8C(!movable);
		Unknown308->Unknown04->Set_Hidden(!movable);
		if (!movable) {
			Unknown30C = 0.5f;
			Unknown310 = 0.5f;
		}
		bool zoom = Unknown314.Y > 1.0f;
		Unknown204[13]->Unknown_40(zoom);
		Unknown204[13]->Unknown_8C(!zoom);
		Unknown204[14]->Unknown_40(zoom);
		Unknown204[14]->Unknown_8C(!zoom);
		if (!movable) {
			Unknown314.X = 1.0f;
		}
	}
	Unknown314.X = WWMath::Min(Unknown314.X, Unknown314.Y);
	Unknown314.X = WWMath::Max(Unknown314.X, 0.5f);
	Unknown_550550(0);
}

// 0x550550
void StarbaseAvatarCreationInterface::Unknown_550550(int dir)
{
	float step = Unknown314.X * 0.033333335f;
	if (dir == 1) {
		Unknown314.X = WWMath::Max(Unknown314.X - step, 0.5f);
		if (Unknown314.X == 0.5f) {
			Unknown204[13]->Unknown_40(false);
		}
		Unknown204[14]->Unknown_40(true);
	} else if (dir == 2) {
		Unknown314.X = WWMath::Min(step + Unknown314.X, Unknown314.Y);
		if (Unknown314.X == Unknown314.Y) {
			Unknown204[14]->Unknown_40(false);
		}
		Unknown204[13]->Unknown_40(true);
	}
	Unknown_54F6F0(1, false, 0, 0);
	Unknown_551BF0();
}

// 0x550670
void StarbaseAvatarCreationInterface::Unknown_550670()
{
	if (Unknown0B0 == 1) {
		Unknown1CC[0]->Unknown2C800 = true;
		Unknown1CC[0]->Unknown_6603A0(0, NULL);
	} else {
		Unknown1CC[0]->Unknown2C800 = false;
		Unknown1CC[0]->Unknown_6607D0();
	}
	if (Unknown0B0 == 2) {
		Unknown1CC[1]->Unknown2C800 = true;
		Unknown1CC[1]->Unknown_6603A0(0, NULL);
	} else {
		Unknown1CC[1]->Unknown2C800 = false;
		Unknown1CC[1]->Unknown_6607D0();
	}
	if (Unknown358 == 0) {
		Unknown204[10]->Unknown_6607D0();
		Unknown204[10]->Unknown2C800 = true;
		Unknown204[10]->Unknown_6603A0(0, NULL);
	} else {
		Unknown204[10]->Unknown2C800 = false;
		Unknown204[10]->Unknown_6607D0();
	}
	if (Unknown358 == 1) {
		Unknown204[11]->Unknown2C800 = true;
		Unknown204[11]->Unknown_6603A0(0, NULL);
	} else {
		Unknown204[11]->Unknown2C800 = false;
		Unknown204[11]->Unknown_6607D0();
	}
	if (Unknown358 == 2) {
		Unknown204[12]->Unknown2C800 = true;
		Unknown204[12]->Unknown_6603A0(0, NULL);
	} else {
		Unknown204[12]->Unknown2C800 = false;
		Unknown204[12]->Unknown_6607D0();
	}
	if (Unknown358 == 3) {
		Unknown240[4]->Unknown_6607D0();
		Unknown240[4]->Unknown2C800 = true;
		Unknown240[4]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[4]->Unknown2C800 = false;
		Unknown240[4]->Unknown_6607D0();
	}
	if (Unknown358 == 4) {
		Unknown240[5]->Unknown2C800 = true;
		Unknown240[5]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[5]->Unknown2C800 = false;
		Unknown240[5]->Unknown_6607D0();
	}
	if (Unknown358 == 5) {
		Unknown240[6]->Unknown2C800 = true;
		Unknown240[6]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[6]->Unknown2C800 = false;
		Unknown240[6]->Unknown_6607D0();
	}
	if (Unknown358 == 6) {
		Unknown240[7]->Unknown2C800 = true;
		Unknown240[7]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[7]->Unknown2C800 = false;
		Unknown240[7]->Unknown_6607D0();
	}
}

// 0x550970
void StarbaseAvatarCreationInterface::Unknown_550970(int value)
{
	if (Unknown0B0 != 2) {
		return;
	}
	Unknown35C = value;
	switch (Unknown358) {
		case 3: Unknown0BC->Unknown_780790(1, value); break;
		case 4: Unknown0BC->Unknown_780790(2, value); break;
		case 5: Unknown0BC->Unknown_780790(3, value); break;
		case 6: Unknown0BC->Unknown_780790(4, value); break;
	}
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->EnvironmentMapManager);
	Unknown_551BF0();
}

// NOT EXACT: register (0.71, 1713 bytes against 0x6A8: the frame time's second global loads into ebp, pushed early, and the copies' registers rotate from there; attempts.py show 0x550A10)
// 0x550A10
void StarbaseAvatarCreationInterface::Unknown_550A10()
{
	Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this);	// dead finds (D130; not the client's): they spend VC6's inline budget, so the third Rotate_Y stays out of line as the client's (matrix3d.h's copy 0x551270; four were enough before 0x5513E0-0x551C80 joined the file, with four now it is inlined)
	if (Unknown0B0 != 2) {
		return;
	}
	float time = WW3D::Get_Frame_Time() * 0.001f;
	float slow = time * 0.3f;
	float fast = time + time;
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown240[8]->Unknown30);
	if (model) {
		Matrix3D tm = model->Get_Transform();
		if (Unknown35C == 0) {
			Unknown26C[0].Rotate_Y(fast);
		} else {
			Unknown26C[0].Rotate_Y(slow);
		}
		int bone = Mesh->Peek_Object()->Get_Bone_Index(UIAvatarCustParts[37].Button);
		Mesh->Peek_Object()->Capture_Bone(bone);
		Mesh->Peek_Object()->Control_Bone(bone, Unknown26C[0]);
		Matrix3D newtm(Unknown26C[0]);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		model->Set_Transform(newtm);
		Unknown240[8]->Unknown_6608F0();
	}
	model = Unknown_497300(Unknown240[9]->Unknown30);
	if (model) {
		Matrix3D tm = model->Get_Transform();
		if (Unknown35C == 1) {
			Unknown26C[1].Rotate_Y(fast);
		} else {
			Unknown26C[1].Rotate_Y(slow);
		}
		int bone = Mesh->Peek_Object()->Get_Bone_Index(UIAvatarCustParts[38].Button);
		Mesh->Peek_Object()->Capture_Bone(bone);
		Mesh->Peek_Object()->Control_Bone(bone, Unknown26C[1]);
		Matrix3D newtm(Unknown26C[1]);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		model->Set_Transform(newtm);
		Unknown240[9]->Unknown_6608F0();
	}
	model = Unknown_497300(Unknown240[10]->Unknown30);
	if (model) {
		Matrix3D tm = model->Get_Transform();
		float angle = slow;
		if (Unknown35C == 2) {
			angle = fast;
		}
		Unknown26C[2].Rotate_Y(angle);
		int bone = Mesh->Peek_Object()->Get_Bone_Index(UIAvatarCustParts[39].Button);
		Mesh->Peek_Object()->Capture_Bone(bone);
		Mesh->Peek_Object()->Control_Bone(bone, Unknown26C[2]);
		Matrix3D newtm(Unknown26C[2]);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		model->Set_Transform(newtm);
		Unknown240[10]->Unknown_6608F0();
	}
}






























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// 0x551320
void StarbaseAvatarCreationInterface::Unknown_551320(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event)
		return;

	GadgetClass *gadget = event->Gadget;
	FAIL_IF(!gadget)
		return;
	if (gadget->Flash && gadget->Is_Unknown2C_02()) {
		if (gadget == Unknown1CC[0] || gadget == Unknown1CC[1] || gadget == Unknown1CC[12] || gadget == Unknown1CC[13]) {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
		}
	}
}

// 0x5513E0
void StarbaseAvatarCreationInterface::Unknown_5513E0(const char *name)
{
	RefCountPtr<RenderObjClass> mesh = Unknown_497300(Mesh->Peek_Object());
	RefCountPtr<RenderObjClass> palette = Create_Get(mesh->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	TextureClass *texture = WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
	if (texture) {
		Unknown_495FD0(palette.Peek(), texture, 0);
		texture->Release_Ref();
	}
}

// 0x551520
void StarbaseAvatarCreationInterface::Unknown_551520(bool any)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<MoneyClass> *money;
	if (any) {
		money = Unknown_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	} else {
		money = Unknown_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	}
	if (money) {
		Unknown380 = money->Value;
		FontSys *text = Unknown_BE67C4->Get_Style("credits_buy_avatar00", Mesh->Peek_Object());
		if (text) {
			std::string line("Credits: ");
			line += Unknown380.Unknown_4F00C0(true);
			text->Change_Text(line.c_str(), 0);
		}
	}
	FontSys *text = Unknown_BE67C4->Get_Style("cost_buy_avatar00", Mesh->Peek_Object());
	if (text) {
		std::string line("Cost: ");
		line += Unknown388.Unknown_4F00C0(true);
		text->Change_Text(line.c_str(), 0);
	}
}

// 0x551900
void StarbaseAvatarCreationInterface::Unknown_551900()
{
	if (Unknown388 == MoneyClass()) {
		View->Get_Unknown34()->Unknown_588A70("NO_CHANGES", 0, NULL, 0x140, 0, 0);
	} else if (Unknown380 >= Unknown388) {
		char text[128];
		_snprintf(text, sizeof(text), "Total changes cost %s credits. Proceed with update?", Unknown388.Unknown_4F00C0(false).c_str());
		View->Get_Unknown34()->Unknown_588A70("AVATAR_CONFIRMATION", 0, text, 0x140, 0, 0);
	} else {
		View->Get_Unknown34()->Unknown_588A70("NEED_FUNDS", 0, NULL, 0x140, 0, 0);
	}
}

// 0x551A80
void StarbaseAvatarCreationInterface::Unknown_551A80()
{
	if (View->Get_Unknown34()->Is_Active()) {
		int button = View->Get_Unknown34()->Unknown_5890B0("AVATAR_CONFIRMATION");
		if (button) {
			if (button == 1) {
				Unknown_54EC30();
				Unknown0B4->Unknown_69F440();
			}
			View->Get_Unknown34()->Unknown_5C();
		}
		if (View->Get_Unknown34()->Unknown_5890B0("NEED_FUNDS")) {
			View->Get_Unknown34()->Unknown_5C();
		}
		if (View->Get_Unknown34()->Unknown_5890B0("NO_CHANGES")) {
			View->Get_Unknown34()->Unknown_5C();
		}
	}
}

// 0x551B20
void StarbaseAvatarCreationInterface::Unknown_551B20(bool on)
{
	int i;
	for (i = 0; i < 14; i++) {
		if (Unknown1CC[i]) Unknown1CC[i]->Unknown_40(on);
	}
	for (i = 0; i < 15; i++) {
		if (Unknown204[i]) Unknown204[i]->Unknown_40(on);
	}
	for (i = 0; i < 11; i++) {
		if (Unknown240[i]) Unknown240[i]->Unknown_40(on);
	}
	if (Unknown0B8) Unknown0B8->Unknown_40(on);
	if (Unknown31C) Unknown31C->Unknown00->Unknown_40(on);
	if (Unknown308) Unknown308->Unknown00->Unknown_40(on);
}

// 0x551BF0
void StarbaseAvatarCreationInterface::Unknown_551BF0()
{
	if (Unknown3CC) {
		AvatarDescClass *newavatar = &Unknown0BC->Unknown0F8;
		Unknown388 = Unknown390.Unknown_8493F0(&Unknown0C0, newavatar);
	}
}

// 0x551C40
void StarbaseAvatarCreationInterface::Unknown_551C40(const RecustomizationCostClass &cost)
{
	Unknown390.Data = cost.Data;
	Unknown3CC = true;
}

// 0x551C80
void StarbaseAvatarCreationInterface::Unknown_551C80()
{
	SClient *client = Client->As_Space_Client();
	int id = client->Unknown1138;
	client->Send_Packet(&StarbaseRequestPacketClass(id, 10, 0));
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avatar_cust.cpp (its __FILE__, 0x551320's FAIL_IFs on lines 2210 / 2214)
**
**	The avatar customisation window, StarbaseAvatarCreationInterface (the client's RTTI: the type
**	descriptor of its FlashingGadgetEvent callback, 0xB7AC88, sits in this file's literals): so far
**	0x550670, 0x550970 and 0x550A10 (NOT EXACT; matrix3d.h's out-of-line Rotate_Y 0x551270 comes with
**	it) (TG4, claim #63), and 0x5513E0-0x551C80 after the FAIL_IFs of 0x551320 (TK6, claim #87): the
**	palette texture, the credits and cost texts, the confirmation box and its answers, the gadgets
**	shown, the cost of the changes, the costs kept, the STARBASE_REQUEST. The file's evidence: they sit
**	between ui_attrb_skills.cpp's last functions (0x54B370) and ui_avatar_dialog.cpp's first (0x551FB0,
**	its asserts from line 85); the literals of 0x5513E0-0x551C80 ('COLOR_PALETTE' 0xB7AA2C, 'Cost: '
**	0xB7ABC0 to 'NO_CHANGES' 0xB7AC70) lie in this file's block of literals, around its __FILE__
**	(0xB7AB88), before the callback's type name and ui_avatar_dialog.cpp's first literal (0xB7AD0C).
**	Before 0x550670 (TK15, claims #119 / #120): 0x54E610 (the gadgets hidden while the view's +0x34 is
**	active), 0x54EC30 (NOT EXACT: the new look sent, RECUSTOMIZE_AVATAR_DONE; it brings SBaseViewClass's
**	Get_Starbase 0x54EDA0 and the packet's implicit destructor 0x54EDD0 out of line), 0x54F2B0 (+0x368..+0x378
**	deleted), 0x550140 (the part buttons) and 0x550550 (the zoom); NOT EXACT (register) 0x54E690 / 0x54E7F0 (the
**	body and pants types), 0x54E950 (+0x368's value) and 0x54EAD0 (+0x36C's value); 0x54E3C0 (the model's
**	turn while a turn button is held: 0x54E610 calls it), 0x54EE40 / 0x54EE80 (reached only through a table)
**	exact. +0xBC is a RefCountPtr (0x54E690 hands it to +0xB8's
**	slot +0x84 through a converted temporary). The file starts at 0x54B760, its PermStringInitClass static (0xBE5B34,
**	atexit 0x54B790): 0x54B370 is ui_attrb_skills.cpp's (its FAIL_IF, line 577), and 0x54B740 / 0x54B750 are
**	adjustor thunks to 0x549800 (ui_attrb_skills.cpp's class); the constructor 0x54B7B0 ('buy_avatar00') and
**	the destructor 0x54BAB0 follow.
**	The class is declared only as far as these use it; the rest of the file (about 2,200 lines before
**	the FAIL_IFs, 0x54B7B0-0x550670) is not rebuilt. +0xC0 is the old look (an AvatarDescClass, 0x10C
**	bytes, up to the gadgets at +0x1CC), the avatar's +0xF8 the new one.
*/
