// Earth & Beyond client (reconstruction): app\client\ui_ship_cust.h, the starbase's buy-ship window of ui_ship_cust.cpp
// (this header's name is provisional, NAME 1). Notes at the end of ui_ship_cust.cpp.
#ifndef UI_SHIP_CUST_H  // NAME 1 [ui_ship_cust.h]: provisional header name for ui_ship_cust.cpp
#define UI_SHIP_CUST_H

#include <map>
#include "sclient.h"
#include "ui_shipcust.h"
#include "matrix3d.h"
#include "vector2.h"
#include "..\shared\money.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\packets\pkrecustomize.h"

class SBaseViewClass;
class UIViewClass;
class ShipPipClass;
class LayerClass;
class UIAvatrPullClass;
class TextGadgetClass;
class TextureClass;
class GadgetClass;
class FlashingGadgetEvent;
class SClientUnknown1334Class;
class ShipCustDescClass;
class ShipRecustomizationCostClass;
class Vector3;

// What the window's gadget map holds for an id (0x18 bytes; 0x61ADA0 fills it): ui_shipcust.h's ShipCustGadgetStruct's
// layout, but its own type: the client's map templates of this file (0x618570, 0x61F760-0x620B30) use their own _Nil
// and _Nilrefs (0xBE5E28 / 0xBE5E24), apart from ui_shipcust.cpp's copies (0x622A60, 0x628A70-0x62A600)
struct StarbaseShipCustGadgetStruct	// NAME 1: neutral (D10)
{
	GadgetClass *Gadget;								// +0x00  // NAME 1: as ui_shipcust.h's
	int Id;												// +0x04 (the key again)  // NAME 1: as ui_shipcust.h's
	const char *Name;									// +0x08 the model's sub-object  // NAME 1: as ui_shipcust.h's
	const char *Style;									// +0x0C its text's font style  // NAME 1: as ui_shipcust.h's
	short Key;											// +0x10 its key  // NAME 1: as ui_shipcust.h's
	short Group;										// +0x12 a radio group (0: the tabs)  // NAME 1: as ui_shipcust.h's
	bool Selected;										// +0x14  // NAME 1: as ui_shipcust.h's
};

// The starbase's ship customisation window (the buy-ship pages 'fa_buy_ship01_ini' ..), as far as this file's
// functions so far use it. Its first base is the starbase windows' interface base (0xAC bytes, as
// ui_avatar_cust.cpp's StarbaseAvatarCreationInterface), its second the ship being edited (ui_shipcust.h's
// Unknown620D20Class, at +0xAC: these functions call its 0x6211D0 / 0x621390 with ecx = this + 0xAC).
class StarbaseShipCustomizeInterface : public Unknown5BB320Class, public Unknown620D20Class	// NAME 3: the client's RTTI (0xB89BB0, among this file's strings: SignalBaseClass::MemberCallback<GadgetClass,StarbaseShipCustomizeInterface,...>, the FlashingGadgetEvent handler's)
{
public:
	StarbaseShipCustomizeInterface(UIViewClass *view, LayerClass *layer);	// 0x6181B0 ('BUY_SHIP01'; sbaseview.cpp's 0x69F8F0)  // NAME 1 [view, layer]: as ui_avatar_cust.cpp's
	virtual ~StarbaseShipCustomizeInterface();			// 0x6186A0 (the deleting one 0x618540)
	virtual void Unknown_54();							// +0x54 0x618CA0 (after construction: the description from the avatar, the preview, the decal library, page 0)
	virtual void Unknown_60();							// 0x6193A0 (every frame, then Unknown5BB320Class's 0x5BB660)
	virtual bool Unknown_68(int key);					// +0x68 0x619470 (the events: the gadgets' ids)  // NAME 1 [key]: InterfaceClass's
	void Unknown_61A4D0();								// 0x61A4D0 (the frame: held buttons, a tab change, leaving; G_ASSERT l.712: as ui_shipcust.cpp's 0x624270)
	void Unknown_61AC20(bool on);						// 0x61AC20 (the gadgets on or off, their texts dimmed: as ui_shipcust.cpp's 0x6249D0)  // NAME 1 [on]: neutral
	void Unknown_61AD30(const char *style, bool on);	// 0x61AD30 (a style shown or hidden: as 0x624AE0)  // NAME 1 [style, on]: neutral
	void Unknown_61ADA0(int id, const char *name, const char *style, short key, short group);	// 0x61ADA0 (a button of the model's sub-object into the map: as 0x624B50)  // NAME 1 [all]: as ui_shipcust.h's
	void Unknown_61AFC0();								// 0x61AFC0 (the ship rebuilt and handed to the preview, then priced: as ui_shipcust.cpp's 0x624D70)
	void Unknown_61AF50();								// 0x61AF50 (the map emptied: as 0x624D00)
	void Unknown_61B0F0();								// 0x61B0F0 (the parts' colours onto the model, then 0x61F180)
	void Unknown_61B130();								// 0x61B130 (the insignia onto the model, then 0x61F180)
	void Unknown_61B510(Vector2 uv);					// 0x61B510 (the colour's palette position: as 0x6251F0)  // NAME 1 [uv]: neutral
	void Unknown_61B580();								// 0x61B580 (the pull to the colour's palette position: as ui_shipcust.cpp's 0x625260)
	void Unknown_61B760();								// 0x61B760 (the colour palette and its pull: as ui_shipcust.cpp's 0x625440; G_ASSERT 'ColorSliderTexture' l.1108)
	void Unknown_61BA80(bool on);						// 0x61BA80 (the pull shown: as 0x625760)  // NAME 1 [on]: neutral
	void Unknown_61B170();								// 0x61B170
	void Unknown_61BAC0(int id);						// 0x61BAC0 (a button selected in its group: as ui_shipcust.cpp's 0x6257A0)  // NAME 1 [id]: neutral
	int Unknown_61BC50(short group);					// 0x61BC50 (the selected id of a group: as 0x625930)  // NAME 1 [group]: neutral
	void Unknown_61BD80(int page);						// 0x61BD80 (a page's model, fonts and buttons: as ui_shipcust.cpp's 0x625A60)  // NAME 1 [page]: neutral
	void Unknown_61BCE0(short group);					// 0x61BCE0 (a group's buttons unselected: as 0x6259C0)  // NAME 1 [group]: neutral
	void Unknown_61D260();								// 0x61D260 (the page's buttons from the description: as ui_shipcust.cpp's 0x6277A0)
	float *Unknown_61D180();							// 0x61D180 (the colour's palette position: as ui_shipcust.cpp's 0x627110)
	Vector3 *Unknown_61D1C0();							// 0x61D1C0 (the colour: as 0x627160)
	bool *Unknown_61D200();								// 0x61D200 (the setting's matte flag: as 0x6271A0)
	ShipCustomizationMetal *Unknown_61D230();			// 0x61D230 (the setting's metal: as 0x6271D0)
	void Unknown_61DAB0();								// 0x61DAB0 (the insignia buttons: as ui_shipcust.cpp's 0x627FA0)
	void Unknown_61DBF0();								// 0x61DBF0 (the insignia list's textures: as 0x6280E0)
	void Unknown_61DF80();								// 0x61DF80 (the metal models: 'METAL00' ..)
	void Unknown_61E8B0();
	void Unknown_61E8E0(bool any);						// 0x61E8E0 (the credits and cost texts: as ui_avatar_cust.cpp's 0x551520)  // NAME 1 [any]: as ui_avatar_cust.cpp's
	void Unknown_61EF60();								// 0x61EF60 (the confirmation box: as ui_avatar_cust.cpp's 0x551900)
	void Unknown_61F0E0();								// 0x61F0E0								// 0x61E8B0 (the client's 0x74C960 for its avatar)
	void Unknown_61F180();								// 0x61F180 (after the model changes)
	void Unknown_61F690();								// 0x61F690 (RECUSTOMIZE_SHIP_DONE with the description)
	void Unknown_61F1D0();								// 0x61F1D0 (STARBASE_REQUEST, action 11: as ui_avatar_cust.cpp's 0x551C80)
	void Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d);	// 0x61F250 (RECUSTOMIZE_SHIP_START 129: cpacket.cpp)  // NAME 1 [desc, cost, a, b, c, d]: the packet's fields
	void Unknown_61E730(FlashingGadgetEvent *event, GadgetClass *gadget);	// 0x61E730 (FAIL_IFs '!event' l.2017, '!gadget' l.2020)  // NAME 3 [event, gadget]: the FAIL_IFs'

	SBaseViewClass *SBaseView;								// +0x230 (its Client's EnvironmentMapManager; not InterfaceClass's View, +0x48)  // NAME 1: neutral (D10)
	bool Unknown234;									// +0x234 (the constructor 0x6181B0 clears +0x234 .. +0x239)
	bool Unknown235;									// +0x235
	bool Unknown236;									// +0x236
	bool Unknown237;									// +0x237
	bool Unknown238;									// +0x238
	bool Unknown239;									// +0x239
	int Unknown23C;										// +0x23C
	RefCountPtr<TextureClass> Unknown240;				// +0x240 (no insignia: 'BLACK.TGA', as ui_shipcust.cpp's +0x21C)
	Matrix3D Unknown244[3];								// +0x244 (the metal models' frames: 0x61DF80; the constructor makes them identities)
	RenderObjClass *Unknown2D4;							// +0x2D4
	RenderObjClass *Unknown2D8;							// +0x2D8
	RenderObjClass *Unknown2DC;							// +0x2DC
	ShipPipClass *Unknown2E0;							// +0x2E0 the ship preview gadget (as ui_shipcust.cpp's +0x22C)
	LayerClass *Layer;									// +0x2E4 (the buttons are made over it)  // NAME 1: as ui_shipcust.h's
	int Page;											// +0x2E8 (1: the paint page, as ui_shipcust.cpp's +0x234)  // NAME 1: as ui_shipcust.h's
	std::map<int, StarbaseShipCustGadgetStruct> Gadgets;		// +0x2EC (as ui_shipcust.cpp's +0x238)  // NAME 1: as ui_shipcust.h's
	bool Unknown2FC;									// +0x2FC (the page's fonts loaded, as ui_shipcust.cpp's +0x248)
	UIAvatrPullClass *Unknown300;						// +0x300 the colour palette and its pull (as ui_shipcust.cpp's +0x24C)
	RefCountPtr<TextureClass> ColorSliderTexture;		// +0x304  // NAME 3: 0x61B760's G_ASSERT (l.1108)
	TextGadgetClass *NameGadget;						// +0x308 (the ship's name: 'NAME_buy_ship04')  // NAME 1: as ui_shipcust.h's
	int Unknown30C;										// +0x30C the part
	int Unknown310;										// +0x310 the part's setting
	int Unknown314;										// +0x314 the insignia list's first
	int Unknown318;										// +0x318 the insignia
	char Unknown31C[0x320 - 0x31C];						// +0x31C
	MoneyClass Unknown320;								// +0x320 (the destructor's EH states 5 / 6: 0x4A7780)
	MoneyClass Unknown328;								// +0x328
	SClientUnknown1334Class *Unknown330;				// +0x330 the decal library
	bool Unknown334;									// +0x334
	ShipRecustomizationCostClass Unknown338;			// +0x338 (constructor 0x8499C0; 0x61F250 copies the packet's costs into it)
	ShipCustDescClass Unknown36C;						// +0x36C the ship before the changes (0x61F250; 0x61F180 prices the description against it)
};

#endif // UI_SHIP_CUST_H
