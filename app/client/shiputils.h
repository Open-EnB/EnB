// Earth & Beyond client (reconstruction): app\client\shiputils.h, the free functions of shiputils.cpp (the
// client's __FILE__ 'e:\build\beta\G\app\Client\shiputils.cpp'; this header's name is provisional, NAME 1).
// Notes at the end of shiputils.cpp.
#ifndef SHIPUTILS_H  // NAME 1 [shiputils.h]: provisional header name for shiputils.cpp
#define SHIPUTILS_H

#include <vector>
#include "vector3.h"
#include "..\packets\pkcolorization.h"
#include "..\packets\pknamedecal.h"

class RenderObjClass;
class EnvironmentMapManagerClass;
class SClientUnknown1334Class;

// The packet's decals on the model, with the decal library (cpacket.cpp's DECAL 16, ui_shipcust.cpp's 0x621390)
void Unknown_80E8C0(RenderObjClass *model, std::vector<DecalStruct> &decals, SClientUnknown1334Class *decal_ini);	// 0x80E8C0 (cdecl)  // NAME 1 [model, decals, decal_ini]: as cpacket.cpp's
// The colourization regions on the model (cpacket.cpp's COLORIZATION 17, ui_shipcust.cpp's 0x6211D0)
void Unknown_80EFF0(RenderObjClass *model, std::vector<ColorizationRegionStruct> &regions, EnvironmentMapManagerClass *unknown);	// 0x80EFF0 (cdecl)  // NAME 1 [model, regions, unknown]: as cpacket.cpp's

// The ship's name on its 'namedecal' meshes: 'ship_font.tga' printed into a texture, tinted with the colour
// (ui_shipcust.cpp's 0x621500 / 0x622FC0, with the ship description's Ship_Name and Ship_Name_Color)
void Unknown_80FF20(RenderObjClass *robj, const char *name, float *color);	// 0x80FF20 (cdecl; FAIL_IFs l.477 / l.483)  // NAME 1 [robj, name, color]: what ui_shipcust.cpp passes

// The model's '<name>.<name>-<state>' animation (an HLOD's) at the fraction of its frames, recursive (co.h declares it too)
void Play_State_Animation(RenderObjClass *robj, int state, float fraction);	// 0x810540 (cdecl; FAIL_IF '!robj' l.523)  // NAME 2: what it does

// The engines' glow: 'exhaust*' bones' objects shown or hidden, 'eng#' bones given 'p-eng1'..'p-eng4' first;
// recursive over the sub-objects
void Set_Engine_Glow(RenderObjClass *robj, bool on);	// 0x810750 (cdecl; FAIL_IF '!robj' l.537)  // NAME 1 [robj, on]: what it does with them

// The damage particles on the bones: 'part0q<0..3>' by the four quadrants' levels, 'tractor' / '~weap_' by the hull's
// (0 no change, 1 cleared, 2-4 smoke / sparks / fire); recursive; ends the client's effect effect_id and returns 0, else effect_id
class ClientClass;
int Update_Damage_Particles(RenderObjClass *robj, int *levels, int hull_level, ClientClass *client, unsigned id, int effect_id);	// 0x810CF0 (cdecl; FAIL_IF '!robj' l.608)  // NAME 2: what it does
// The blood of a damaged organic object (blood.ini by its number, BloodType, Blood1 / Blood0): its sound and looping sound,
// two 'Level<n>' models per level on random bones; returns the looping sound's effect id, else effect_id (0 once ended)
int Update_Blood_Particles(RenderObjClass *robj, int hull_level, int number, unsigned id, bool organic, bool view, ClientClass *client, int effect_id);	// 0x8117D0 (cdecl; FAIL_IFs l.790 / l.859)  // NAME 2: what it does

#endif // SHIPUTILS_H
