// Earth & Beyond client (reconstruction): app\client\ship_stats_display.cpp, the ship's statistics on the PDA's equipment
// window (the client's __FILE__ 'e:\build\beta\G\app\Client\ship_stats_display.cpp', 0x6A2D50-0x6A3D50). Notes at the end.
#include <stdio.h>
#include <string>
#include "ship_stats_display.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "texture.h"
#include "fontdatabase.h"
#include "fontmanager.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\g_assert.h"

class TextureClass *Unknown_4960C0(RenderObjClass *robj, int arg);		// 0x4960C0 (ro_funcs.cpp)  // NAME 1 [robj, arg]: as gadget.cpp's
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);	// 0x495FD0 (ro_funcs.cpp): the texture onto the mesh's stage index  // NAME 1 [robj, tex, index]: as avatar.cpp's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The field named string under root when it holds a value (its Unknown70 set), else NULL (the name a local of its own:
// the two finds of each pair share the caller's two name slots)
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const char *string) { NameClass name(string); C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1 [Unknown_Find_Valid, string]: neutral (D10), as co.cpp's

// The fields on the display: each one's name (the font style on the mesh, after 'BaseStats.' / 'CurrentStats.' the
// ship's field) and whether a higher value is the better one (0xBE65E0 / 0xBE661C; its constructor is the static
// initialiser 0x6A2DA0)
class SSDC	// NAME 5: the client's FAIL_IF text 'i >= SSDC::NUM_SHIP_DISPLAY_FIELDS'
{
public:
	enum { NUM_SHIP_DISPLAY_FIELDS = ShipStatsDisplayClass::NUM_FIELDS };	// NAME 5: the FAIL_IF text
























	SSDC()
	{
		memset(HigherIsBetter, true, sizeof(HigherIsBetter));
		String[0] = "HullStrength";
		String[1] = "Defense";
		String[2] = "MissileDefense";
		String[3] = "Speed";
		String[4] = "WarpSpeed";
		String[5] = "TurnRate";
		String[6] = "ScanRange";
		String[7] = "Visibility";
		String[8] = "ResistImpact";
		String[9] = "ResistExplosive";
		String[10] = "ResistPlasma";
		String[11] = "ResistEnergy";
		String[12] = "ResistEMP";
		String[13] = "ResistChemical";
		String[14] = "ResistPsionic";
		HigherIsBetter[7] = false;
		for (int lp = 0; lp < NUM_SHIP_DISPLAY_FIELDS; lp++) {
			G_ASSERT(String[lp]);
		}
	}
	const char *Get_String(unsigned int i) const	// NAME 1 [Get_String, i]: the FAIL_IF's i
	{
		FAIL_IF(i >= SSDC::NUM_SHIP_DISPLAY_FIELDS) return("");
		return(String[i]);
	}

	bool Get_Higher_Is_Better(unsigned int i) const	// NAME 1 [Get_Higher_Is_Better, i]: what the update compares it with
	{
		FAIL_IF(i >= SSDC::NUM_SHIP_DISPLAY_FIELDS) return(true);
		return(HigherIsBetter[i]);
	}
	const char *String[NUM_SHIP_DISPLAY_FIELDS];		// NAME 5: the G_ASSERT 'String[lp]'
	bool HigherIsBetter[NUM_SHIP_DISPLAY_FIELDS];		// NAME 2: what it holds (false for 'Visibility' only)
};

static SSDC ShipDisplayFields;	// NAME 1: neutral (D10)











// 0x6A2ED0
ShipStatsDisplayClass::ShipStatsDisplayClass(SClient *client, RefCountPtr<RenderObjClass> &display) :
	Client(client),
	Display(display),
	UnknownC8(true)
{
	memset(Values, 0, sizeof(Values));
	FAIL_IF(!Display) return;
	G_ASSERT(Client);
	for (int lp = 0; lp < SSDC::NUM_SHIP_DISPLAY_FIELDS; lp++) {
		const char *name = ShipDisplayFields.Get_String(lp);
		if (*name) {
			Fonts[lp] = Unknown_BE67C4->Unknown_6D2700(name, Display);
		}
		char buf[128];
		_snprintf(buf, sizeof(buf), "ud%02d", lp);
		ChangeArrows[lp] = Create_Get(Display->Get_Sub_Object_By_Name(buf));





		FAIL_IF(!ChangeArrows[lp]) continue;
		if (!Unknown80) {
			Unknown80 = Create_Peek(Unknown_4960C0(ChangeArrows[lp].Peek(), 0));
		}
	}
	Up = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("if_check_up.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
	Down = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("if_check_dn.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
}

// 0x6A33B0
ShipStatsDisplayClass::~ShipStatsDisplayClass()
{
	Client = NULL;
}







// NOT EXACT: functional from the pseudocode (D64); the arrow's texture pick takes eax / ecx the other way round
// 0x6A34E0
void ShipStatsDisplayClass::Unknown_6A34E0(bool on)
{
	if (!Display || !Client) return;
	ClientObjectClass *ship = Client->Get_Ship();
	FAIL_IF(!ship) return;
	if (UnknownC8) on = true;
	DeltaCompoundClass *aux = (DeltaCompoundClass *)ship->AuxData;
	{
		DeltaTemplateClass<float> *hull = Unknown_Find_Valid<DeltaTemplateClass<float> >(aux, "HullPoints");
		DeltaTemplateClass<float> *max_hull = Unknown_Find_Valid<DeltaTemplateClass<float> >(aux, "MaxHullPoints");
		if (hull && max_hull && (hull->Unknown70 > 1 || max_hull->Unknown70 > 1 || on) && Fonts[0]) {
			int hp = (int)hull->Value;
			int max_hp = (int)max_hull->Value;
			Fonts[0]->Unknown_6CE420(0, "%d/%d", hp, max_hp);
			if (hp >= max_hp / 2) {
				Fonts[0]->Set_Color(Vector3(0.33f, 0.33f, 0.99f));
			} else {
				Fonts[0]->Set_Color(Vector3(0.66f, 0.0f, 0.0f));
			}
			if (UnknownC8) Values[0] = max_hp;
			if (ChangeArrows[0]) {
				TextureClass *tex;
				if (Values[0] == max_hp) tex = Unknown80.Peek();
				else if (Values[0] < max_hp) tex = Up.Peek();
				else tex = Down.Peek();
				Unknown_495FD0(ChangeArrows[0].Peek(), tex, 0);
			}
		}
	}
	std::string name;
	for (int lp = 1; lp < SSDC::NUM_SHIP_DISPLAY_FIELDS; lp++) {
		const char *field = ShipDisplayFields.Get_String(lp);
		if (!field || !*field) continue;
		name = "BaseStats.";
		name += field;
		DeltaTemplateClass<int> *base = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, name.c_str());
		name = "CurrentStats.";
		name += field;
		DeltaTemplateClass<int> *current = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, name.c_str());
		if (!base || !current || (!on && base->Unknown70 <= 1 && current->Unknown70 <= 1)) continue;
		int base_value = base->Value;
		int value = current->Value;
		RefCountPtr<FontSys> &font = Fonts[lp];























		FAIL_IF(!font) continue;
		if (value == base_value) {
			font->Set_Color(Vector3(0.33f, 0.33f, 0.99f));
		} else {
			bool increased = value > base_value;
			if (ShipDisplayFields.Get_Higher_Is_Better(lp) == increased) {
				font->Set_Color(Vector3(0.0f, 1.0f, 0.0f));
			} else {
				font->Set_Color(Vector3(0.66f, 0.0f, 0.0f));
			}
		}
		if (lp == 6 || lp == 7) {
			font->Unknown_6CE420(0, "%.1fk", value * 0.001f);
		} else {
			font->Unknown_6CE420(0, "%d", value);
		}
		if (UnknownC8) Values[lp] = value;
		if (ChangeArrows[lp]) {
			TextureClass *tex;
			if (Values[lp] == value) tex = Unknown80.Peek();
			else if (Values[lp] < value) tex = Up.Peek();
			else tex = Down.Peek();
			Unknown_495FD0(ChangeArrows[lp].Peek(), tex, 0);
		}
	}
	UnknownC8 = false;
}

// 0x6A3C10
void ShipStatsDisplayClass::Unknown_6A3C10()
{
	UnknownC8 = true;
}


/*
**	Reconstruction notes (O18, 2026-11-13)
**
**	The object: the previous object ends with its <string> / <locale> statics 0x6A2C30, 0x6A2CB0 (the guarded
**	ctype id) and 0x6A2D20 (its RefCountPtr Clear copy 0x6A2BF0 sits before them); this one opens with permstr.h's
**	PermStringInitClass static 0x6A2D50 (atexit 0x6A2D80), then SSDC's table 0x6A2DA0, the class's four functions
**	0x6A2ED0-0x6A3C10, and ends with its own <string> statics 0x6A3C30, 0x6A3CA0 (guard) and 0x6A3D30.
**	SSDC's bools are a memset(true) and one false: the client stores them as 0x01010101 dwords, a word and a byte.
**	Not exact (one round): Unknown_6A34E0 picks each arrow's texture into ecx with the arrow in eax, ours the other way
**	round (registers only; a ternary pick spends the inline budget and puts FontStyleColorStruct's copy out of line).
*/
