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
#include "dx8renderer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"

#include "avatar.h"	// CharacterViewAvatarClass (the window's +0xBC)
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

struct UIAvatarCustGadgetUnknown08Struct { char Unknown00[0x30]; RenderObjClass *Unknown30; };	// gadget.h's slot +0x08 return: the model at +0x30 (as ui_avatr.cpp's)  // NAME 1: neutral (D10)
// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual UIAvatarCustGadgetUnknown08Struct *As_RObjGadget(); virtual void As_RObjProxyGadget();
	virtual void As_SlotDataGadget(); virtual void As_UIScutProxyGadget(); virtual void Unknown_18(); virtual void Unknown_1C();
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
	void Unknown_6603A0(bool indicator, UIViewClass *view);	// 0x6603A0  // NAME 1 [indicator, view]: gadget.h's
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
	bool UnknownDC;	void Set_UnknownDC(bool on) { UnknownDC = on; }	// +0xDC (0x54DFE0: set in mode 1, the head, through an inline setter: the client's eax for the store)  // NAME 1 [Set_UnknownDC]: neutral (D10)
};

// The pulls' gadget (0x80 bytes, constructor 0x671300), as ui_avatr.cpp's UIAvatrGadget671300Class
class Unknown_671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	Unknown_671300Class(RenderObjClass *area, int a, const LayerClass &layer, int b, int c, int d);	// 0x671300 (0x54D8A0: the palette, 0, the window's layer +0xAC, 0, 0, 0)  // NAME 1 [area, a, layer, b, c, d]: neutral
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

void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color);	// 0x496C00 (ro_funcs: the texture's colour at u, v)  // NAME 1 [robj, u, v, color]: as ro_funcs.cpp's
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
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

// The asset manager (*0xBFAEE0)
#include "assetmgr.h"	// WW3DAssetManager: 0xBFAEE0 is WW3DAssetManager::TheInstance (its constructor 0x91C2C0 stores it)







extern GadgetClass *Unknown_BE5F24;					// NAME 1 [Unknown_BE5F24]: 0xBE5F24 (0x54E3C0: a gadget held down, as 0xBE5F28)
extern GadgetClass *Unknown_BE5F28;					// NAME 1 [Unknown_BE5F28]: 0xBE5F28


#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)


extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4
// fontdatabase.h's FontStringFilesClass at 0xBE67BC (sclient.h declares the pointer), as ui_avatr.cpp does

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
	{ "shirt_style_lef", "shirt_style_buy_avatar00" },
	{ "shirt_style_rig", NULL },
	{ "pant_style_left", "pant_style_buy_avatar00" },
	{ "pant_style_righ", "cloth_title_buy_avatar00" },
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
	int Unknown18;	int Get_Unknown18() { return(Unknown18); }	// +0x18 (the value: 0x54EAD0 hands it to the avatar's 0x77FD30; 0x54CBC0's 0x14-0x19 and 0x54E950 read it through the inline getter)  // NAME 1 [Get_Unknown18]: neutral (D10)
	char Unknown1C[0x24 - 0x1C];						// +0x1C (0x24 bytes: 0x54EEC0)
};

#include "ui_avatar_cust.h"	// StarbaseAvatarCreationInterface and its UIAvatarCustPairStruct (moved there; the blank lines keep the asserts' lines)




























































































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
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Object());
	Unknown_BE67BC->Unload("fa_buy_avatar00.ini");
	GadgetClass::Get_Signal_Map().Remove((GadgetClass *)NULL, this, &StarbaseAvatarCreationInterface::Unknown_551320, FlashingGadgetEvent());
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

// NOT EXACT: register (0.62, 2402 bytes against 0x86D + the table): the client's own case per key (0x54EAD0 inlined
// for 0x12 / 0x13, the palette helper 0x5513E0 for 0x1A-0x1C, 0x551BF0 in the 0x14-0x19 tail VC6 cross-jumps), the
// view's Client read once at the top (edi) for 0x16-0x19, and the choosers' value through an inline getter there (the
// client's ecx); left: its converted temporary in the 0x54EAD0 copies through the out-of-line 0x551D40 with the
// destructor inline (ours the reverse), ebp / ebx roles in the palette copies, and eax where VC6 here takes edx
// (attempts.py show 0x54CBC0)
// 0x54CBC0
bool StarbaseAvatarCreationInterface::Unknown_68(int key)
{
	SClient *client = Unknown0B4->Client;
	if (!Unknown3CC) {
		return(true);
	}
	if (Unknown360 || Unknown361 || Unknown362 || Unknown363 || Unknown364 || Unknown365) {
		return(false);
	}
	switch (key) {
		case 1:
			Unknown_54DFE0(1);
			Unknown_550670();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		case 2:
			Unknown_54DFE0(2);
			Unknown_550670();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		case 3:
			Unknown0B8->Unknown_8C();
			break;
		case 8:
			Unknown0B8->Unknown_88();
			break;
		case 0xD:
			Unknown_551900();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		case 0xE:
			Unknown0B4->Unknown_69F440();
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			return(true);
		case 0x10:
			Unknown_54E950(1);
			break;
		case 0x11:
			Unknown_54E950(2);
			break;
		case 0x12:
			Unknown_54EAD0(1);
			break;
		case 0x13:
			Unknown_54EAD0(2);
			break;
		case 0x14:
			Unknown370->Unknown_585CC0(); Unknown0BC->Unknown_7859F0(Unknown370->Get_Unknown18()); Unknown_5502F0();
			Unknown0B8->Unknown_84(Unknown0BC);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2); Unknown_551BF0(); return(true);
		case 0x15:
			Unknown370->Unknown_585C90(); Unknown0BC->Unknown_7859F0(Unknown370->Get_Unknown18()); Unknown_5502F0();
			Unknown0B8->Unknown_84(Unknown0BC);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2); Unknown_551BF0(); return(true);
		case 0x16:
			Unknown374->Unknown_585CC0(); Unknown0BC->Unknown_77FCE0(Unknown374->Get_Unknown18(), true); TheDX8MeshRenderer.Invalidate();
			Unknown0BC->Unknown_780810(client->EnvironmentMapManager); Unknown0B8->Unknown_84(Unknown0BC);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2); Unknown_551BF0(); return(true);
		case 0x17:
			Unknown374->Unknown_585C90(); Unknown0BC->Unknown_77FCE0(Unknown374->Get_Unknown18(), true); TheDX8MeshRenderer.Invalidate();
			Unknown0BC->Unknown_780810(client->EnvironmentMapManager); Unknown0B8->Unknown_84(Unknown0BC);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2); Unknown_551BF0(); return(true);
		case 0x18:
			Unknown378->Unknown_585CC0(); Unknown0BC->Unknown_77FC90(Unknown378->Get_Unknown18(), true); TheDX8MeshRenderer.Invalidate();
			Unknown0BC->Unknown_780810(client->EnvironmentMapManager); Unknown0B8->Unknown_84(Unknown0BC);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2); Unknown_551BF0(); return(true);
		case 0x19:
			Unknown378->Unknown_585C90(); Unknown0BC->Unknown_77FC90(Unknown378->Get_Unknown18(), true); TheDX8MeshRenderer.Invalidate();
			Unknown0BC->Unknown_780810(client->EnvironmentMapManager); Unknown0B8->Unknown_84(Unknown0BC);
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2); Unknown_551BF0(); return(true);
		case 0x1A:
		case 0x1B:
			Unknown_5513E0("fa_avatar_color.tga");
			Unknown_550140(key);
			break;
		case 0x1C:
			Unknown_5513E0("fa_skin_color.tga");
			Unknown_550140(key);
			break;
		case 0x20:
			Unknown_54E690(1);
			break;
		case 0x21:
			Unknown_54E690(2);
			break;
		case 0x22:
			Unknown_54E7F0(1);
			break;
		case 0x23:
			Unknown_54E7F0(2);
			break;
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			Unknown_550140(key);
			break;
		case 0x28:
			Unknown_550970(0);
			break;
		case 0x29:
			Unknown_550970(1);
			break;
		case 0x2A:
			Unknown_550970(2);
			break;
		default:
			return(false);
	}
	SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
	return(true);
}

// 0x54D720
void StarbaseAvatarCreationInterface::Unknown_54D720(int text)
{
	const char *names[] = { "text1", "text2", "Text3", "text4", "Text5", "Text6", "text7" };
	RefCountPtr<RenderObjClass> palette = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("COLOR_PALETTE"));
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
	RenderObjClass *palette = InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("COLOR_PALETTE");
	Unknown_496AE0(palette, 0);
	Unknown31C->Unknown00 = new Unknown_671300Class(palette, 0, *Unknown0AC, 0, 0, 0);
	Unknown31C->Unknown00->Unknown2C400 = true;
	Unknown31C->Unknown_5859A0();
	palette->Release_Ref();
	Unknown31C->Unknown04 = InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("PULL_COLOR");
	Unknown_496AE0(Unknown31C->Unknown04, 1);
	Unknown64.Unknown_6870E0(Unknown31C->Unknown00, 0x2F, View, 1, 1);
	Unknown_54FFC0();
}

// 0x54DA50
void StarbaseAvatarCreationInterface::Unknown_54DA50()
{
	Unknown308 = new UIAvatarCustGadgetHolderStruct;
	RefCountPtr<RenderObjClass> area = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("HEAD_AREA"));
	Unknown_496AE0(area.Peek(), 0);
	Unknown308->Unknown00 = new Unknown_671300Class(area.Peek(), 0, *Unknown0AC, 0, 0, 0);
	Unknown308->Unknown00->Unknown2C400 = true;
	Unknown308->Unknown_5859A0();
	Unknown308->Unknown04 = InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("HEAD_PULL");
	Unknown_496AE0(Unknown308->Unknown04, 1);
	Unknown64.Unknown_6870E0(Unknown308->Unknown00, 0x2E, View, 1, 1);
}

// 0x54DC20
void StarbaseAvatarCreationInterface::Unknown_54DC20(int mode, bool on)
{
	RefCountPtr<RenderObjClass> model = Unknown_497300(InterfaceMesh->Peek_Object());
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

// The tab switch: the palette helper 0x5513E0 inlined twice, its two ~RefCountPtr out of line (0x4740B0) in both
// copies; the pip's +0xDC through an inline setter and the avatar through one pointer per tab (the client's single
// load of +0xBC); two dead Peek()s spend the inline budget the client spent (D130; not the client's)
// 0x54DFE0
void StarbaseAvatarCreationInterface::Unknown_54DFE0(int mode)
{
	if (Unknown0B0 == mode && (mode == 1 || mode == 2)) {
		return;
	}
	Unknown0B0 = mode;
	RefCountPtr<RenderObjClass> model = Unknown_497300(InterfaceMesh->Peek_Object());
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
			Unknown0B8->Set_UnknownDC(true);
			Unknown0B8->Unknown_88();
			Unknown0B8->Unknown_8C();
			{ CharacterViewAvatarClass *avatar = Unknown0BC.Peek(); avatar->Unknown231 = true;
			avatar->Unknown214 = true; }
			break;
		case 2:
			Unknown_54DC20(1, false);
			Unknown_54DC20(2, true);
			if (frame) frame->Set_Hidden(false);
			Unknown_550140(0x24);
			Unknown_5513E0("fa_avatar_color.tga");
			Unknown0B8->Set_UnknownDC(false);
			Unknown0B8->Unknown_88();
			Unknown0B8->Unknown_8C();
			{ CharacterViewAvatarClass *avatar = Unknown0BC.Peek(); avatar->Unknown231 = false;
			avatar->Unknown214 = true; } Unknown0BC.Peek(); Unknown0BC.Peek();	// dead Peek()s (D130; not the client's): they spend VC6's inline budget, so the helper's second copy calls its ~RefCountPtr out of line
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

// The view's client through Get_Client() and its envmaps through sclient.h's inline Peek_Environment_Map_Manager() (the client's eax)
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
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Peek_Environment_Map_Manager());
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// As 0x54E690's, its twin for Pants_Type
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
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Peek_Environment_Map_Manager());
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// NOT EXACT: register (0.73 with the value through Get_Unknown18(): dir in ebp where the client keeps edi, then as 0x54E690's; attempts.py show 0x54E950)
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
	Unknown0BC->Unknown_77FC20(Unknown368->Get_Unknown18(), true);
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Peek_Environment_Map_Manager());
	Unknown0B8->Unknown_84(Unknown0BC);
	Unknown_551BF0();
}

// As 0x54E690's, the chooser's value through Get_Unknown18() (the client's eax)
// 0x54EAD0
void StarbaseAvatarCreationInterface::Unknown_54EAD0(int dir)
{
	if (dir == 1) {
		Unknown36C->Unknown_585CC0();
	} else {
		Unknown36C->Unknown_585C90();
	}
	Unknown0BC->Unknown_77FD30(Unknown36C->Get_Unknown18(), true);
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Peek_Environment_Map_Manager());
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
	RefCountPtr<RenderObjClass> model = Unknown_497300(InterfaceMesh->Peek_Object());
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
	if (((Unknown_671300Class *)pull->Unknown00)->Unknown64) {
		pull->Unknown08 = true;
	}
	if ((((Unknown_671300Class *)pull->Unknown00)->Unknown64 || ((Unknown_671300Class *)pull->Unknown00)->Unknown65) && pull->Unknown08) {
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
		RefCountPtr<RenderObjClass> model = Unknown_497300(InterfaceMesh->Peek_Object());
		int bone = model->Get_Sub_Object_Bone_Index(pull->Unknown04);
		model->Capture_Bone(bone);
		Matrix3D tm = pull->Unknown04->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
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
	((Unknown_671300Class *)pull->Unknown00)->Unknown_671630();
}

// A Vector2 point set member by member, tm[2][3] = 0, the edges declared up front, the tattoo offset built from the
// members, and each translation's product parenthesised ((width * point.X) + left: without the parentheses VC6
// schedules the adds otherwise)
// 0x54F6F0
void StarbaseAvatarCreationInterface::Unknown_54F6F0(int unused, bool random, float du, float dv)
{
	UIAvatarCustGadgetHolderStruct *pull = Unknown308;
	Vector2 point;
	point.X = Unknown30C;
	point.Y = Unknown310;
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
	float top, left, width, height;
	if (random) {
		point.X = Unknown_BDBB84(0, 100) * 0.01f;
		point.Y = Unknown_BDBB84(0, 100) * 0.01f;
	}
	top = box.Center.Y - box.Extent.Y;
	left = box.Center.X - box.Extent.X;
	width = box.Extent.X * 2.0f;
	height = box.Extent.Y * 2.0f;
	RefCountPtr<RenderObjClass> model = Unknown_497300(InterfaceMesh->Peek_Object());
	int bone = model->Get_Sub_Object_Bone_Index(pull->Unknown04);
	model->Capture_Bone(bone);
	Matrix3D tm = pull->Unknown04->Get_Transform();
	tm.Set_X_Translation((width * point.X) + left - box.Center.X);
	tm.Set_Y_Translation((height * point.Y) + top - box.Center.Y);
	tm[2][3] = 0.0f;
	model->Control_Bone(bone, tm, false);
	Unknown30C = point.X;
	Unknown310 = point.Y;
	Unknown0BC->Unknown_785C00(Vector3(Unknown30C, 1.0f - Unknown310, Unknown314.X));
}

// 0x54FAA0
void StarbaseAvatarCreationInterface::Unknown_54FAA0()
{
	if (((Unknown_671300Class *)Unknown31C->Unknown00)->Unknown64) {
		Unknown31C->Unknown08 = true;
	}
	if ((((Unknown_671300Class *)Unknown31C->Unknown00)->Unknown64 || ((Unknown_671300Class *)Unknown31C->Unknown00)->Unknown65) && Unknown31C->Unknown08) {
		AABoxClass box;
		Unknown31C->Unknown00->Unknown_64(box);
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		x = WWMath::Min(x, box.Center.X + box.Extent.X);
		x = WWMath::Max(x, box.Center.X - box.Extent.X);
		y = WWMath::Min(y, box.Center.Y + box.Extent.Y);
		y = WWMath::Max(y, box.Center.Y - box.Extent.Y);
		RenderObjClass *model = InterfaceMesh->Peek_Object();
		int bone = model->Get_Sub_Object_Bone_Index(Unknown31C->Unknown04);
		model->Capture_Bone(bone);
		Matrix3D tm = Unknown31C->Unknown04->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
		model->Control_Bone(bone, tm, false);
		Vector2 point;
		point.X = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		point.Y = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		float cu;
		if (point.X < 0.01f) {
			cu = 0.01f;
		} else if (point.X > 0.99f) {
			cu = 0.99f;
		} else {
			cu = point.X;
		}
		float cv;
		if (point.Y < 0.01f) {
			cv = 0.01f;
		} else if (point.Y > 0.99f) {
			cv = 0.99f;
		} else {
			cv = point.Y;
		}
		Vector3 color;
		UIAvatarCustGadgetUnknown08Struct *area = Unknown31C->Unknown00->As_RObjGadget();
		Unknown_496C00(area->Unknown30, cu, 1.0f - cv, &color);
		Unknown320[Unknown358].Set(cu, cv);
		switch (Unknown358) {
			case 0:
				Unknown0BC->Unknown_77FFA0(color, true);
				break;
			case 1:
				Unknown0BC->Unknown_7804A0(color);
				break;
			case 2:
				Unknown0BC->Unknown_780680(0, color);
				break;
			case 3:
				Unknown0BC->Unknown_780680(1, color);
				Unknown_550970(-1);
				break;
			case 4:
				Unknown0BC->Unknown_780680(2, color);
				Unknown_550970(-1);
				break;
			case 5:
				Unknown0BC->Unknown_780680(3, color);
				Unknown_550970(-1);
				break;
			case 6:
				Unknown0BC->Unknown_780680(4, color);
				Unknown_550970(-1);
				break;
		}
		Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Peek_Environment_Map_Manager());
		Unknown_551BF0();
	} else {
		Unknown31C->Unknown08 = false;
	}
	((Unknown_671300Class *)Unknown31C->Unknown00)->Unknown_671630();
}

// The translations through Set_X / Set_Y_Translation with the products parenthesised (either alone leaves
// Control_Bone's `push 0` hoisted above the matrix copy)
// 0x54FFC0
void StarbaseAvatarCreationInterface::Unknown_54FFC0()
{
	AABoxClass box;
	Unknown31C->Unknown00->Unknown_64(box);
	float top = box.Center.Y - box.Extent.Y;
	float left = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f;
	float height = box.Extent.Y * 2.0f;
	RenderObjClass *model = InterfaceMesh->Peek_Object();
	int bone = model->Get_Sub_Object_Bone_Index(Unknown31C->Unknown04);
	model->Capture_Bone(bone);
	Matrix3D tm = Unknown31C->Unknown04->Get_Transform();
	tm.Set_X_Translation((width * Unknown320[Unknown358].Unknown00) + left - box.Center.X);
	tm.Set_Y_Translation((height * Unknown320[Unknown358].Unknown04) + top - box.Center.Y);
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

// The step's last test reads zoom, not movable (the client's reload of zoom's slot before pop edi): with it the
// two bools take the client's slots (zoom set then cleared in memory, zoom's 1 from the ebx kept for the
// Unknown0B0 == 1 compare)
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
		if (!zoom) {
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
		int bone = InterfaceMesh->Peek_Object()->Get_Bone_Index(UIAvatarCustParts[37].Button);
		InterfaceMesh->Peek_Object()->Capture_Bone(bone);
		InterfaceMesh->Peek_Object()->Control_Bone(bone, Unknown26C[0]);
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
		int bone = InterfaceMesh->Peek_Object()->Get_Bone_Index(UIAvatarCustParts[38].Button);
		InterfaceMesh->Peek_Object()->Capture_Bone(bone);
		InterfaceMesh->Peek_Object()->Control_Bone(bone, Unknown26C[1]);
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
		int bone = InterfaceMesh->Peek_Object()->Get_Bone_Index(UIAvatarCustParts[39].Button);
		InterfaceMesh->Peek_Object()->Capture_Bone(bone);
		InterfaceMesh->Peek_Object()->Control_Bone(bone, Unknown26C[2]);
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
	RefCountPtr<RenderObjClass> mesh = Unknown_497300(InterfaceMesh->Peek_Object());
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
		FontSys *text = Unknown_BE67C4->Get_Style("credits_buy_avatar00", InterfaceMesh->Peek_Object());
		if (text) {
			std::string line("Credits: ");
			line += Unknown380.Amount_As_Text(true);
			text->Change_Text(line.c_str(), 0);
		}
	}
	FontSys *text = Unknown_BE67C4->Get_Style("cost_buy_avatar00", InterfaceMesh->Peek_Object());
	if (text) {
		std::string line("Cost: ");
		line += Unknown388.Amount_As_Text(true);
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
		_snprintf(text, sizeof(text), "Total changes cost %s credits. Proceed with update?", Unknown388.Amount_As_Text(false).c_str());
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
		AvatarDescClass *newavatar = &Unknown0BC->Desc;
		Unknown388 = Unknown390.Unknown_8493F0(&Unknown0C0, newavatar);
	}
}

// 0x551C40
void StarbaseAvatarCreationInterface::Unknown_551C40(const RecustomizationCostClass &cost)
{
	Unknown390.Costs = cost.Costs;
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
**	2026-11-04 (UAC): the window's base is Unknown5BB320Class (the mesh at +0x84, the layer at +0xAC); exact
**	since: the constructor 0x54B7B0, the deleting destructor 0x54BA80 and its thunks 0x551F90 / 0x551FA0, the
**	per-frame 0x54CAE0 (0x54E610 inlined), 0x54D720 (the palette texture), 0x54D8A0 / 0x54DA50 (the colour and
**	head pulls made), 0x54DC20 (a mode's gadgets shown), 0x54EEC0 (the choosers made), 0x54FAA0 (the colour pull
**	dragged), 0x551320 (the FlashingGadgetEvent handler) and the destructor 0x54BAB0 (the callback taken off
**	through SignalMap::Remove, its base SignalBaseClass::EventCallbackClass<FlashingGadgetEvent>'s constructor
**	0x551EE0 out of line as the client's; 0x551EE0-0x551F70 exact with it). NOT EXACT: 0x54DFE0 (the tab
**	switch: inline budget, as ui_avatr.cpp's 0x556B20), 0x54F390 (the head pull dragged: one byte), 0x54F6F0 (the
**	tattoo pull: order-only), 0x54FFC0 (order-only) and 0x5502F0 (two bools' stack slots). Not rebuilt: Init
**	0x54BD30 (slot +0x54). The key handler 0x54CBC0 (slot +0x68) functional (CS1, 2026-11-04; NOT EXACT 0.42).
**	+0xC0 is the old look (an AvatarDescClass, 0x10C bytes, up to the gadgets at +0x1CC), the avatar's +0xF8
**	the new one; the +0xB8 gadget is ship_pip.cpp's (ui_avatr.cpp's ShipPipClass).
*/
