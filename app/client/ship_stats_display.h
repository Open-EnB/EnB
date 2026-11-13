// Earth & Beyond client (reconstruction): app\client\ship_stats_display.h, the ship's statistics on the PDA's equipment
// window (the client's ship_stats_display.cpp per its asserts, 'e:\build\beta\G\app\Client\ship_stats_display.cpp' at
// 0xB91100; this header's name is provisional, NAME 1, D137). Laid out from the constructor 0x6A2ED0 and the destructor
// 0x6A33B0 (O18, 2026-11-13).
#ifndef SHIP_STATS_DISPLAY_H  // NAME 1 [ship_stats_display.h]: provisional header name for ship_stats_display.cpp
#define SHIP_STATS_DISPLAY_H

#include "..\..\wwlib\ref_ptr.h"

class SClient;
class RenderObjClass;
class TextureClass;
class FontSys;

// The ship's statistics (0xCC bytes, no table; ui_pda_equip.cpp makes it over its 'tsi07a' mesh and deletes it): one
// text per field (the mesh's named font styles 'HullStrength' .. 'ResistPsionic') and one change arrow per field ('ud00'
// .. 'ud14', textured up, down or as it was against the value last kept)
class ShipStatsDisplayClass	// NAME 3: class-from-file (D24) for app\Client\ship_stats_display.cpp
{
public:
	enum { NUM_FIELDS = 15 };							// NAME 1: SSDC::NUM_SHIP_DISPLAY_FIELDS's value (the FAIL_IF text)
	ShipStatsDisplayClass(SClient *client, RefCountPtr<RenderObjClass> &display);	// 0x6A2ED0 (FAIL_IF '!Display' l.113, G_ASSERT 'Client' l.114, FAIL_IF '!ChangeArrows[lp]' l.128)  // NAME 1 [client]: what the window passes
	~ShipStatsDisplayClass();							// 0x6A33B0
	void Unknown_6A34E0(bool on);						// 0x6A34E0 (FAIL_IFs '!ship' l.155, '!font' l.217; the window's slot +0x58 passes true, +0x60 false): the texts and arrows from the ship's 'HullPoints' / 'MaxHullPoints' and 'BaseStats.' / 'CurrentStats.' fields  // NAME 1 [Unknown_6A34E0, on]: neutral (D10)
	void Unknown_6A3C10();								// 0x6A3C10 (the window's key 16): the next update keeps the values again  // NAME 1: neutral (D10)

	SClient *Client;									// +0x00  // NAME 5: the G_ASSERT 'Client'
	RefCountPtr<RenderObjClass> Display;				// +0x04  // NAME 5: the FAIL_IF '!Display'
	RefCountPtr<FontSys> Fonts[NUM_FIELDS];				// +0x08 (the mesh's named styles)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> ChangeArrows[NUM_FIELDS];	// +0x44 ('ud%02d')  // NAME 5: the FAIL_IF '!ChangeArrows[lp]'
	RefCountPtr<TextureClass> Unknown80;				// +0x80 (the first arrow's own texture: unchanged)  // NAME 1: neutral (D10)
	RefCountPtr<TextureClass> Up;						// +0x84 ('if_check_up.tga')  // NAME 2: its texture
	RefCountPtr<TextureClass> Down;						// +0x88 ('if_check_dn.tga')  // NAME 2: its texture
	int Values[NUM_FIELDS];								// +0x8C (the values kept while +0xC8 is set)  // NAME 2: what it holds
	bool UnknownC8;									// +0xC8 (set by the constructor and 0x6A3C10, cleared by 0x6A34E0)  // NAME 1: neutral (D10)
};

#endif // SHIP_STATS_DISPLAY_H
