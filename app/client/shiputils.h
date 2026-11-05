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

// The engines' glow: 'exhaust*' bones' objects shown or hidden, 'eng#' bones given 'p-eng1'..'p-eng4' first;
// recursive over the sub-objects
void Unknown_810750(RenderObjClass *robj, bool on);	// 0x810750 (cdecl; FAIL_IF '!robj' l.537)  // NAME 1 [robj, on]: what it does with them

#endif // SHIPUTILS_H
