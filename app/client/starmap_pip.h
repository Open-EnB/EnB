// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\starmap_pip.h (name assumed from starmap_pip.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\starmap_pip.cpp'; the client holds no string of this header)
**
**	The PDA's star map picture-in-picture over a map PIP (map_pip.h): the sector's objects, their labels and
**	the coordinates of the pointer. Constructor 0x7BF930, table 0xB06C8C, destructor 0x7BFF20 (deleting
**	0x7BFEF0), 0xD4 bytes. ui_starmap.cpp's 0x7C63D0 news it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STARMAP_PIP_H  // NAME 3 [starmap_pip.h]: header name assumed from the client's starmap_pip.cpp
#define __STARMAP_PIP_H

#include <string>
#include <vector>
#include "always.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "vector3.h"
#include "map_pip.h"
#include "nav_list_line_draw.h"

class SClient;
class SceneClass;
class FontSys;
class ClientObjectClass;
class NavigationListClass;
class NavListBuilderClass;
class MouseTargetClass;
class RadarSystemDataCacheClass;
class StarmapPipClass;

// gd_starmap.cpp's gadget (not in the tree yet), as far as this file calls it. 0x50 bytes, table 0xB05F54.
// PLACEMENT: provisional (D137): the client's gd_starmap.cpp gives it a file; no other user in the tree yet
class GadgetStarmapClass : public GadgetPipSceneClass  // NAME 3: class-from-file (D24) for app\client\gd_starmap.cpp
{
public:
	GadgetStarmapClass(StarmapPipClass *pip);			// 0x7B4700 (G_ASSERT 'StarmapPIP', +0x4C)  // NAME 1 [pip]: what the caller passes
	virtual bool Unknown_30(float x, float y);			// +0x30 (GadgetClass's pure slot)  // NAME 1 [x, y]: GadgetClass's
	StarmapPipClass *StarmapPIP;						// +0x4C  // NAME 3: the assert's
};

// A label on the star map: an object's name in a font, in the PIP's scene. 0x1C bytes, no table.
class Unknown_7C3220Class  // NAME 1: neutral (D10)
{
public:
	Unknown_7C3220Class(const RefCountPtr<SceneClass> &scene, bool arg, int type);	// 0x7C3220 (type 0..4: RADAR_GROUP, _PLAYER, _OBJECT, _JUMPSTART, _NAME)  // NAME 1 [scene, arg, type]: what it does with them
	~Unknown_7C3220Class();								// 0x7C3720
	void Unknown_7C3A90(int left, int top, int right, int bottom);	// 0x7C3A90 (Text's 0x6D88F0 rectangle; 0x7C1770)  // NAME 1 [left, top, right, bottom]: FontSys 0x6D88F0's
	void Set_Name(std::string name);					// 0x7C25B0 (0x7C2340)  // NAME 2: what it does
	void Unknown_7C3830(ClientObjectClass *co);			// 0x7C3830 (NULL: the label off)  // NAME 2 [co]: the client's usual name

	unsigned ObjectID;									// +0x00 (the object's +0x90)  // NAME 2: what it holds
	RefCountPtr<FontSys> Text;							// +0x04  // NAME 2: what it holds
	RefCountPtr<SceneClass> Scene;						// +0x08  // NAME 2: what it holds
	std::string Name;									// +0x0C (shown for an object named "Unknown")  // NAME 2: what it holds
};

class StarmapPipClass : public MapPipClass  // NAME 3: class-from-file (D24) for app\client\starmap_pip.cpp
{
public:
	StarmapPipClass(SClient *client);					// 0x7BF930  // NAME 1 [client]: neutral
	virtual ~StarmapPipClass();							// +0x00 0x7BFF20 (deleting 0x7BFEF0)
	virtual bool Unknown_04(const RefCountPtr<InterfaceClass> &iface_ptr, const RefCountPtr<RenderObjClass> &vp, int arg);	// +0x04 0x7C0BC0  // NAME 2 [iface_ptr, vp]: map_pip.h's  // NAME 1 [arg]: neutral
	virtual bool Unknown_08();							// +0x08 0x7C0D40
	virtual bool Unknown_0C(bool on);					// +0x0C 0x7C0AF0  // NAME 1 [on]: neutral
	virtual bool Unknown_10(const RefCountPtr<RenderObjClass> &vp);	// +0x10 0x7C0C80  // NAME 2 [vp]: map_pip.h's
	virtual void Unknown_14();							// +0x14 0x7C0D70
	virtual GadgetClass *Unknown_18();					// +0x18 0x7C01C0 (news the 0x50-byte gadget 0x7B4700)
	virtual void Unknown_1C(int arg);					// +0x1C 0x7C0240 (the next coordinates mode)  // NAME 1 [arg]: neutral
	virtual void Unknown_20(int arg);					// +0x20 0x7C0260  // NAME 1 [arg]: neutral
	virtual void Unknown_24(int arg);					// +0x24 0x7C0330 (the next label mode, and says so)  // NAME 1 [arg]: neutral

	void Unknown_7C0440(ClientObjectClass *hover, MouseTargetClass *mouse_target);	// 0x7C0440 (FAIL_IF 'MouseTarget'; radar_system.cpp)  // NAME 2 [hover, mouse_target]: its G_ASSERT
	void Unknown_7C0620(RadarSystemDataCacheClass *entry);	// 0x7C0620 (FAIL_IF '!sdata', '!co'; radar_system.cpp)  // NAME 2 [entry]: as the gutter's Update
	void Unknown_7C0910(ClientObjectClass *target, NavListBuilderClass *warp_path, NavListBuilderClass *target_path);	// 0x7C0910 (radar_system.cpp)  // NAME 2 [target, warp_path, target_path]: what it receives
	Vector2 Unknown_7C1200(const Vector2 &mouse);			// 0x7C1200 (the pointer's place in the viewport, 0..1, on the map; gd_starmap.cpp, 0x7C1C50)  // NAME 1 [mouse]: GadgetPipSceneClass's Position
	void Unknown_7C1280(const Vector3 &position, float radius);	// 0x7C1280 (0x7C0620, 0x7C1350: the bounds grow; 0x7C1350 passes ClientObjectClass slot +0x24's Vector3)  // NAME 1 [position, radius]: what it does with them
	void Unknown_7C1C50(ClientObjectClass *target, ClientObjectClass *hover);	// 0x7C1C50 (0x7C0910)  // NAME 1 [target, hover]: what 0x7C0910 passes
	void Unknown_7C1A90(float mult);					// 0x7C1A90  // NAME 1 [mult]: what it sets
	void Unknown_7C1B70(float value);					// 0x7C1B70  // NAME 1 [value]: neutral
	void Unknown_7C1C30();								// 0x7C1C30 (no caller)
	void Unknown_7C2280();								// 0x7C2280 (the labels deleted; no caller, the slots expand it)
	void Unknown_Set_54(float value);					// (inline, line 1069: 0x7C1510, 0x7C1B70)  // NAME 1: neutral (D10)
	void Unknown_Set_ScaleMult(float mult);				// (inline, line 1039: 0x7C0260, 0x7C1120, 0x7C1510, 0x7C1A90)  // NAME 1: neutral (D10)
	void Unknown_7C0EA0(bool on);						// 0x7C0EA0  // NAME 1 [on]: neutral
	NavigationListClass *Unknown_7C0EE0();				// 0x7C0EE0 (FAIL_IF '!WarpPath', '!TargetPath')
	bool Unknown_7C0FA0();								// 0x7C0FA0 (the hovered object targeted; FAIL_IF '!radar', '!targeting')
	bool Unknown_7C1060();								// 0x7C1060 (an object is hovered; FAIL_IF '!radar')
	bool Unknown_7C10D0();								// 0x7C10D0 (+0x60 the hovered object's id; FAIL_IF '!radar')
	void Unknown_7C1120(float arg);						// 0x7C1120 (0x7C8980's zoom)  // NAME 1 [arg]: neutral
	bool Unknown_7C1350();								// 0x7C1350 (FAIL_IF '!co'; the bounds of the avatar and the navigation objects, and the map on)
	void Unknown_7C1510(bool arg);						// 0x7C1510 (FAIL_IF 'MapRange_ws.X < WWMATH_EPSILON')  // NAME 1 [arg]: neutral
	void Unknown_7C1770(float x, float y);				// 0x7C1770 (FAIL_IF '!camera', 'ScaleMult < WWMATH_EPSILON')  // NAME 1 [x, y]: what 0x7C0260 passes (+0x68 over CurMult)
	void Unknown_7C2060(int mode);						// 0x7C2060 (the coordinates' fonts in or out of the interface's scene)  // NAME 1 [mode]: what it stores
	void Unknown_7C2340(unsigned id, std::string name, int type);	// 0x7C2340 (a label for the object id; 0x7C2850)  // NAME 1 [id, name, type]: what it does with them
	void Unknown_7C2850(bool force);					// 0x7C2850 (the labels)  // NAME 1 [force]: neutral
	void Unknown_7C2D30();								// 0x7C2D30 (0x7C8E50)

	bool Unknown50;										// +0x50 (0x7C0D70 clears it)
	bool Unknown51;										// +0x51
	float Unknown54;									// +0x54 (1.0; times the mult: CurMult)
	float ScaleMult;									// +0x58 (1.0; 0x7C0260 sets 0.5)  // NAME 3: 0x7C1770's FAIL_IF 'ScaleMult < WWMATH_EPSILON'
	float CurMult;										// +0x5C (1.0)  // NAME 3: the FAIL_IF's
	unsigned Unknown60;									// +0x60 (the client's AvatarID, 0x7C0260 / 0x7C0AF0)
	RefCountPtr<RenderObjClass> Unknown64;				// +0x64
	Vector3 Unknown68;									// +0x68 (X, Y over CurMult: 0x7C1770's scale; Z: 0x7C0C80, the viewport's larger side; 0x7C1770 copies it whole)
	Vector2 Unknown74;									// +0x74 (0x7C0D70: FLT_MAX, FLT_MAX)
	Vector2 Unknown7C;									// +0x7C (0x7C0D70: -FLT_MAX, -FLT_MAX)
	Vector2 MapRange_ws;									// +0x84 (0x7C1280: +0x7C less +0x74)  // NAME 3: 0x7C1510's FAIL_IF 'MapRange_ws.X < WWMATH_EPSILON'
	NavListLineDrawClass *TargetPath;					// +0x8C (0.75, 1, green)  // NAME 3: 0x7C0EE0's FAIL_IF
	NavListLineDrawClass *WarpPath;						// +0x90 (0.75, 2, red)  // NAME 3: 0x7C0EE0's FAIL_IF
	int Unknown94;										// +0x94 (-1)
	int Unknown98;										// +0x98 (-1)
	MouseTargetClass *MouseTarget;						// +0x9C (0x7C0440 sets it, 0x7C0620 tests against it, 0x7C0910 clears it)  // NAME 3: 0x7C0440's FAIL_IF 'MouseTarget'
	Unknown_7C3220Class *UnknownA0;						// +0xA0 (type 4, the scene's)
	Unknown_7C3220Class *UnknownA4;						// +0xA4 (type 4, on)
	RefCountPtr<FontSys> XPos;							// +0xA8 ('STARMAP_X_POS')  // NAME 2: the font's
	RefCountPtr<FontSys> YPos;							// +0xAC ('STARMAP_Y_POS')  // NAME 2: the font's
	RefCountPtr<FontSys> ZPos;							// +0xB0 ('STARMAP_Z_POS')  // NAME 2: the font's
	int ShowCoords;										// +0xB4 (the option 'Starmap_ShowCoords')  // NAME 2: the option's
	float UnknownB8;									// +0xB8 (1.0)
	float UnknownBC;									// +0xBC (1.0)
	int Label;											// +0xC0 (the option 'Starmap_Label': 0 none .. 3 all)  // NAME 2: the option's
	std::vector<Unknown_7C3220Class *> Labels;			// +0xC4  // NAME 2: what it holds
};

#endif // __STARMAP_PIP_H
