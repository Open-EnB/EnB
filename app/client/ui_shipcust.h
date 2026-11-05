// Earth & Beyond client (reconstruction): app\client\ui_shipcust.h, the ship customisation window of character
// creation (the client's ui_shipcust.cpp per its asserts; this header's name is provisional, NAME 1). Notes at the end
// of ui_shipcust.cpp.
#ifndef UI_SHIPCUST_H  // NAME 1 [ui_shipcust.h]: provisional header name for ui_shipcust.cpp
#define UI_SHIPCUST_H

#include "intrface.h"
#include "..\shared\shipcust_desc.h"
#include "..\..\wwlib\ref_ptr.h"
#include <map>

class GadgetClass;
class LayerClass;
class LoginTaskClass;
class TextGadgetClass;
class TextureClass;
class RenderObjClass;
class CharacterViewClass;
class EnvironmentMapManagerClass;
class SClientUnknown1334Class;
class FlashingGadgetEvent;
class ShipPipClass;
class UIAvatrPullClass;

// The window's ship: its description with the models built from it (0x184 bytes; constructor 0x620D20, copy
// constructor 0x620E40, destructor 0x6210A0, all out of line in this file)
class Unknown620D20Class	// NAME 1: neutral (D10)
{
public:
	Unknown620D20Class();								// 0x620D20
	~Unknown620D20Class();								// 0x6210A0 (0x621E20, then the five models released)

	void Unknown_6211D0(EnvironmentMapManagerClass *environmentmapmanager);	// 0x6211D0 (G_ASSERT 'environmentmapmanager != NULL' l.174): the parts' colours onto the model  // NAME 3 [environmentmapmanager]: the assert's
	void Unknown_621390(int decal, SClientUnknown1334Class *decalmaterialmanager);	// 0x621390 (G_ASSERT 'decalmaterialmanager != NULL' l.237): the insignia onto the model  // NAME 3 [decalmaterialmanager]: the assert's; NAME 1 [decal]: neutral
	void Unknown_621500();								// 0x621500 (the name onto the model)
	char *Unknown_621530(int race, int part, int style, int variant);	// 0x621530 (a part's model name, 'S' race part style variant; this unused: 0x621790 sets ecx)  // NAME 1 [all]: neutral
	bool Unknown_621610(RenderObjClass *model, RenderObjClass *part, const char *format);	// 0x621610 (copies of part on the model's numbered bones; this unused)  // NAME 1 [all]: neutral
	void Unknown_6216D0(RenderObjClass *model, const char *format);	// 0x6216D0 (the numbered bones' sub-objects taken off; this unused)  // NAME 1 [all]: neutral
	void Unknown_621790(EnvironmentMapManagerClass *environmentmapmanager, int decal, SClientUnknown1334Class *decalmaterialmanager);	// 0x621790 (G_ASSERTs 'result' l.501, 505): the models built from the description  // NAME 1 [all]: as 0x6211D0's / 0x621390's
	void Unknown_621E20();								// 0x621E20 (the models taken apart and released)

	ShipCustDescClass Desc;								// +0x000 (a member: the ctor 0x620D20 and dtor 0x6210A0 store no table of their own)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Unknown160;				// +0x160 the hull ('S' race 'H' style variant) and the ship  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Unknown164;				// +0x164 the engine, on each part's '~03_nn' bones
	RefCountPtr<RenderObjClass> Unknown168;				// +0x168 the wing (on the hull's '~02')
	RefCountPtr<RenderObjClass> Unknown16C;				// +0x16C the profession part (on the hull's '~01')
	RefCountPtr<RenderObjClass> Unknown170;				// +0x170 'PU43' on its '~weap_nn' bones (race 2, profession 0)
	int Unknown174;										// +0x174 the hull's variant
	int Unknown178;										// +0x178 the wing's variant
	int Unknown17C;										// +0x17C the profession part's variant
	int Unknown180;										// +0x180 the engine's variant
};

// What the window's gadget map holds for an id (0x18 bytes; 0x624B50 fills it)
struct ShipCustGadgetStruct	// NAME 1: neutral (D10)
{
	GadgetClass *Gadget;								// +0x00  // NAME 1: neutral (D10)
	int Id;												// +0x04 (the key again)  // NAME 1: what it holds
	const char *Name;									// +0x08 the model's sub-object  // NAME 1: what it holds
	const char *Style;									// +0x0C its text's font style (0x6249D0 dims it)  // NAME 1: what it holds
	short Key;											// +0x10 its key (0x0D for CONFIRM)  // NAME 1: what it holds
	short Group;										// +0x12 a radio group (0: the tabs)  // NAME 1: what it holds
	bool Selected;										// +0x14  // NAME 1: what it holds
};

// The ship customisation window (ui_shipcust.cpp's __FILE__ 0xB89DEC; the client's RTTI: its FlashingGadgetEvent
// callback's type name MemberCallback<GadgetClass,ShipCustomizeInterface,...>), 0x26C bytes, tables 0xAF79A4 /
// 0xAF7998 / 0xAF798C; the character view's mode 6 (LoginTaskClass step 6). Its pages FA_SHIP01-04: hull, paint,
// insignia, name.
// Its second base, at +0x084, is the description being edited (CONFIRM copies it to the view's +0x244): the
// destructor 0x622B90 null-checks this before running ~Unknown620D20Class inline, as for a base, not a member.
class ShipCustomizeInterface : public InterfaceClass, public Unknown620D20Class	// NAME 5: the client's RTTI
{
public:
	ShipCustomizeInterface(UIViewClass *view, LayerClass *layer, int unknown, int unknown2);	// 0x622180 (charview.cpp's 0x679160: the mode before was 5, 0)  // NAME 1 [view, layer, unknown, unknown2]: as charview.cpp's
	virtual ~ShipCustomizeInterface();				// 0x622B90 (0x622A30 the deleting one)
	virtual void Unknown_60();						// +0x60 0x622FC0 (every frame)
	virtual bool Unknown_68(int key);				// +0x68 0x623080 (the events)  // NAME 1 [key]: InterfaceClass's
	virtual bool Unknown_78(void *arg);				// +0x78 0x6288C0 (the global error packet: 'BAD SHIP NAME')  // NAME 1 [arg]: InterfaceClass's

	void Unknown_624270();							// 0x624270 (the frame: held buttons, a tab change, leaving; G_ASSERT l.1223)
	void Unknown_6249D0(bool on);					// 0x6249D0 (the gadgets on or off, their texts dimmed)  // NAME 1 [on]: neutral
	void Unknown_624AE0(const char *style, bool on);	// 0x624AE0 (a style shown or hidden)  // NAME 1 [style, on]: neutral
	void Unknown_624B50(int id, const char *name, const char *style, short key, short group);	// 0x624B50 (a button of the model's sub-object into the map)  // NAME 1 [all]: what it does with them
	void Unknown_624D00();							// 0x624D00 (the map emptied)
	void Unknown_624D70();							// 0x624D70 (the ship rebuilt and handed to the preview)
	void Unknown_624DF0();							// 0x624DF0
	void Unknown_624E20();							// 0x624E20
	void Unknown_624E50();							// 0x624E50 (the colour pull dragged)
	void Unknown_6251F0(float u, float v);			// 0x6251F0 (the colour's palette position)  // NAME 1 [u, v]: neutral
	void Unknown_625260();							// 0x625260 (the pull to the colour's palette position)
	void Unknown_625440();							// 0x625440 (the colour palette and its pull; G_ASSERT 'ColorSliderTexture' l.1600)
	void Unknown_625760(bool on);					// 0x625760 (the pull shown)  // NAME 1 [on]: neutral
	void Unknown_6257A0(int id);					// 0x6257A0 (a button selected in its group)  // NAME 1 [id]: neutral
	int Unknown_625930(short group);				// 0x625930 (the selected id of a group)  // NAME 1 [group]: neutral
	void Unknown_6259C0(short group);				// 0x6259C0 (a group's buttons unselected)  // NAME 1 [group]: neutral
	void Unknown_625A60(int page);					// 0x625A60 (a page's model, fonts and buttons)  // NAME 1 [page]: neutral
	float *Unknown_627110();						// 0x627110 (the colour's palette position)
	Vector3 *Unknown_627160();						// 0x627160 (the colour)
	bool *Unknown_6271A0();							// 0x6271A0 (the setting's matte flag)
	ShipCustomizationMetal *Unknown_6271D0();		// 0x6271D0 (the setting's metal)
	void Unknown_627200();							// 0x627200 (RANDOM)
	void Unknown_627730(float r, float g, float b);	// 0x627730 (the colour)  // NAME 1 [r, g, b]: neutral
	void Unknown_6277A0();							// 0x6277A0 (the page's buttons from the description)
	void Unknown_627FA0();							// 0x627FA0 (the insignia buttons; inlined in 0x625A60)
	void Unknown_6280E0();							// 0x6280E0 (the insignia list's textures)
	void Unknown_628420();							// 0x628420 (the metal models turned)
	void Unknown_628900(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x628900 (FAIL_IFs '!event', '!gadget')  // NAME 3 [event]: the FAIL_IF's; NAME 1 [sender]: the callback's type

	CharacterViewClass *CharacterView;				// +0x208 (the view's slot +0x28)  // NAME 1: as ui_class.h's
	LoginTaskClass *LoginTask;						// +0x20C  // NAME 1: as ui_class.h's
	bool Unknown210;								// +0x210
	bool Unknown211;								// +0x211 (the quit box is up)
	bool Unknown212;								// +0x212
	bool Unknown213;								// +0x213 (a tab clicked: the page changes in the frame)
	bool Unknown214;								// +0x214
	bool Unknown215;								// +0x215 (leaving)
	int Unknown218;									// +0x218 (how it leaves: 5 the next step, else back)
	RefCountPtr<TextureClass> Unknown21C;			// +0x21C 'BLACK.TGA' (no insignia)
	RenderObjClass *Unknown220[3];					// +0x220 the metal buttons' models (page 2)
	ShipPipClass *Unknown22C;					// +0x22C the ship preview gadget (id 0x34)
	LayerClass *Layer;								// +0x230  // NAME 1: as ui_login.cpp's
	int Page;										// +0x234 (0 hull, 1 paint, 2 insignia, 3 name)  // NAME 1: what it holds
	std::map<int, ShipCustGadgetStruct> Gadgets;	// +0x238  // NAME 1: what it holds
	bool Unknown248;								// +0x248 (the page's fonts loaded)
	UIAvatrPullClass *Unknown24C;					// +0x24C the colour palette and its pull (pages 1 and 3)
	RefCountPtr<TextureClass> ColorSliderTexture;	// +0x250  // NAME 3: the G_ASSERT's
	TextGadgetClass *NameGadget;					// +0x254 ('NAME_fa_ship04', id 0x36)  // NAME 1: as ui_login.cpp's
	int Unknown258;									// +0x258 the part (0 hull .. 3 engine)
	int Unknown25C;									// +0x25C the part's setting (primary, secondary)
	int Unknown260;									// +0x260 the insignia list's first
	int Unknown264;									// +0x264 the insignia (0 none)
	SClientUnknown1334Class *Unknown268;			// +0x268 the decal library (decal_textures.ini ...)
};

typedef ShipCustomizeInterface Unknown622180InterfaceClass;	// NAME 1 [Unknown622180InterfaceClass]: transition alias (charview.cpp)

#endif // UI_SHIPCUST_H
