// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\ui_hdamg.h (PROVISIONAL, D137: ui_hdamg.cpp's header, after its
**	__FILE__ 'e:\build\beta\G\app\Client\ui_hdamg.cpp' 0xB80034; the client holds no string of this header)
**
**	The HUD's hull damage display: four quadrants of a model, each scrolling the UV offset of two linear-offset texture
**	mappers by the object's 'VisibleQuadrantDamagePercent' (0x78 bytes, table 0xAF2028: one slot).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_HDAMG_H  // NAME 3 [ui_hdamg.h]: the header of ui_hdamg.cpp
#define UI_HDAMG_H

#include "..\shared\accel.h"

class RenderObjClass;
class LinearOffsetTextureMapperClass;
class ClientObjectClass;

// One quadrant (0x1C bytes: the constructor 0x59C070, the destructor 0x59C3F0)
class UIHullDamageQuadrantClass  // NAME 2 [UIHullDamageQuadrantClass]: one of 'VisibleQuadrantDamagePercent''s four
{
public:
	UIHullDamageQuadrantClass() : Mapper0(0), Mapper1(0), Last(-1.0f) {}	// 0x59C070
	~UIHullDamageQuadrantClass();												// 0x59C3F0 (inline: the mappers' references)
	void Init(RenderObjClass *model, const char *name0, const char *name1);	// 0x59C3B0  // NAME 2 [all]: what it does
	LinearOffsetTextureMapperClass *Find_Mapper(RenderObjClass *model, const char *name);	// 0x59C430 (G_ASSERT 'false' l.161)  // NAME 2 [all]: what it does
	void Set(float damage, bool immediate);									// 0x59C560  // NAME 2 [all]: what it does

	LinearOffsetTextureMapperClass *Mapper0;	// +0x00  // NAME 2: the first sub object's mapper
	LinearOffsetTextureMapperClass *Mapper1;	// +0x04  // NAME 2: the second sub object's mapper
	AccelClass Offset;							// +0x08 (the mappers' U offset)  // NAME 2: what it holds
	float Last;									// +0x18 (-1 at first: the last target)  // NAME 2: what it holds
};

class UIHullDamageClass  // NAME 2 [UIHullDamageClass]: ui_hdamg.cpp's class, its Message_Stat category 'HullDamage'
{
public:
	UIHullDamageClass(RenderObjClass *model);			// 0x59BF30
	~UIHullDamageClass();									// 0x59C0B0
	void Set_Object(ClientObjectClass *object);			// 0x59C150  // NAME 2 [all]: what it does
	virtual void Update();									// 0x59C190 (Message_Stat 'HullDamage' l.111)  // NAME 2: what it does

	UIHullDamageQuadrantClass Quadrant[4];				// +0x04 (rt/rb, br/bl, lb/lt, tl/tr)  // NAME 2: 'VisibleQuadrantDamagePercent'
	ClientObjectClass *Object;								// +0x74 (its +0x88 aux data)  // NAME 2: what it holds
};

#endif
