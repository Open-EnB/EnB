// Earth & Beyond client (reconstruction): app\client\ui_avatar_cust.h, the starbase's avatar customisation window of
// ui_avatar_cust.cpp (this header's name is provisional, NAME 1). Notes at the end of ui_avatar_cust.cpp.
#ifndef UI_AVATAR_CUST_H  // NAME 1 [ui_avatar_cust.h]: provisional header name for ui_avatar_cust.cpp
#define UI_AVATAR_CUST_H

#include "sclient.h"
#include "matrix3d.h"
#include "vector2.h"
#include "..\shared\money.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\packets\pkrecustomize.h"

class SBaseViewClass;
class UIViewClass;
class LayerClass;
class GadgetClass;
class FlashingGadgetEvent;
class UIAvatarCustPipClass;
class CharacterViewAvatarClass;	// avatar.h
class UIAvatarCustUnknown368Class;
struct UIAvatarCustGadgetHolderStruct;

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
	void Unknown_54FAA0();								// 0x54FAA0 (0x54CAE0: the colour pull dragged, the part's colour from the palette)
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
	RefCountPtr<CharacterViewAvatarClass> Unknown0BC;	// +0xBC (a RefCountPtr: the destructor 0x54BAB0 releases it twice, by hand and as a member)
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

#endif // UI_AVATAR_CUST_H
