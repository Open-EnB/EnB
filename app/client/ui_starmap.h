// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\ui_starmap.h (name assumed from ui_starmap.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\ui_starmap.cpp'; the client holds no string of this header)
**
**	The PDA's star map window: the star map and galaxy map PIPs, resized through a mesh animation.
**	Constructor 0x7C63D0 (pdaint.cpp's 0x693612 news it), tables 0xB071E4 / 0xB071D8 / 0xB071CC, 0x104 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UI_STARMAP_H  // NAME 3 [ui_starmap.h]: header name assumed from the client's ui_starmap.cpp
#define __UI_STARMAP_H

#include "always.h"
#include "ref_ptr.h"
#include "intrface.h"
#include "map_pip.h"
#include "fontmanager.h"
#include "starmap_pip.h"

class NavigationListClass;
class GalaxyMapPipClass;
class UIDynamicWindowClass;
class ButtonGadgetClass;
class HAnimClass;
class SClient;

class UIStarmapClass : public InterfaceClass  // NAME 3: class-from-file (D24) for ui_starmap.cpp
{
public:
	UIStarmapClass(SClient *client);					// 0x7C63D0  // NAME 1 [client]: neutral
	virtual ~UIStarmapClass();							// 0x7C6700 (slot 0x7C66D0)
	virtual void Unknown_54();							// +0x54 0x7C6BF0 (loads the mesh, the fonts, the buttons)
	virtual void Unknown_58();							// +0x58 0x7C81F0
	virtual void Unknown_5C();							// +0x5C 0x7C8440
	virtual void Unknown_60();							// +0x60 0x7C8980 (per frame)
	virtual bool Unknown_68(int key);					// +0x68 0x7C8490  // NAME 2 [key]: InterfaceClass's
	virtual void Unknown_70(ControlKeysManager *manager);	// +0x70 0x7C7B40 (FAIL_IF '!manager')  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_74(ControlKeysManager *manager);	// +0x74 0x7C7EA0 (FAIL_IF '!manager')  // NAME 3 [manager]: the FAIL_IF's

	float Get_Frame() { return(Frame); }				// (the x87 load before the inlined 0x7C8E00)  // NAME 2: what it does
	float Get_Min_Frame() { return(MinFrame); }			// (the same, 0x7C6BF0)  // NAME 2: what it does
	void Unknown_Show_How_To(const RefCountPtr<UIDynamicWindowClass> &window, const Vector3 &position);	// (inlined twice in 0x7C8490)  // NAME 2: what it does
	bool Unknown_7C8BA0(int mode);						// 0x7C8BA0 (0 the star map is current, 1 the galaxy map)  // NAME 1 [mode]: neutral
	void Unknown_7C8C20(int mode);						// 0x7C8C20 (switches the current PIP)  // NAME 1 [mode]: neutral
	bool Unknown_7C8D90();								// 0x7C8D90
	StarmapPipClass *Unknown_7C8DB0(bool on);			// 0x7C8DB0 (the main view's 0x512A00)  // NAME 1 [on]: what 0x512A00 passes
	void Unknown_7C8E00(float frame);					// 0x7C8E00  // NAME 2 [frame]: Frame
	bool Unknown_7C8E50(bool force);					// 0x7C8E50 (the labels)  // NAME 1 [force]: neutral
	bool Unknown_7C91A0();								// 0x7C91A0 (the "Dest: " label; 0x7C8980 every frame)
	void Unknown_7C94C0(bool on);						// 0x7C94C0 (the current PIP on or off)  // NAME 1 [on]: MapPipClass::Unknown_0C's

	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0x84  // NAME 3: the FAIL_IF's
	RefCountPtr<RenderObjClass> Viewport;				// +0x88  // NAME 3: the FAIL_IF's
	RefCountPtr<HAnimClass> MeshAnim;					// +0x8C  // NAME 3: the FAIL_IF's
	float Frame;										// +0x90 (0x7C8E00)  // NAME 2: what it holds
	float MaxFrame;										// +0x94 (the option 'Starmap_MaxFrame')  // NAME 2: the option's
	float MinFrame;										// +0x98 (the option 'Starmap_MinFrame')  // NAME 2: the option's
	bool Maximized;										// +0x9C (the option 'Starmap_Maximized')  // NAME 2: the option's
	StarmapPipClass *StarmapPip;						// +0xA0  // NAME 2: what it holds
	GalaxyMapPipClass *GalaxyMapPip;					// +0xA4  // NAME 2: what it holds
	MapPipClass *CurrentPip;							// +0xA8 (one of the two)  // NAME 2: what it holds
	RefCountPtr<FontSys> DestinationFont;				// +0xAC  // NAME 3: the assert's
	RefCountPtr<FontSys> SectorFont;					// +0xB0  // NAME 3: the assert's
	RefCountPtr<FontSys> SystemTitle;					// +0xB4  // NAME 3: the assert's
	RefCountPtr<FontSys> SystemName;					// +0xB8  // NAME 3: the assert's
	RefCountPtr<FontSys> SectorTitle;					// +0xBC  // NAME 3: the assert's
	RefCountPtr<FontSys> SectorName;					// +0xC0  // NAME 3: the assert's
	RefCountPtr<FontSys> StarbaseTitle;					// +0xC4  // NAME 3: the assert's
	RefCountPtr<FontSys> StarbaseName;					// +0xC8  // NAME 3: the assert's
	unsigned long Destination;							// +0xCC (the id the label shows)  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> ResizeUp;				// +0xD0 ('RESIZE_UP')  // NAME 2: the sub-object's
	RefCountPtr<RenderObjClass> ResizeDown;				// +0xD4 ('RESIZE_DOWN')  // NAME 2: the sub-object's
	RefCountPtr<UIDynamicWindowClass> HowToStarmap;		// +0xD8 ('STARMAP_HOW_TO')  // NAME 2: what it holds
	RefCountPtr<UIDynamicWindowClass> HowToGalaxyMap;	// +0xDC ('STARMAP_HOW_TO')  // NAME 2: what it holds
	ButtonGadgetClass *Buttons[7];						// +0xE0 (0xB9F8B8's)  // NAME 2: what it holds
	int ZoomDirection;									// +0xFC (0x7C8980 zooms while it is not 0)  // NAME 2: what it holds
	int Unknown100;										// +0x100 (0x7C8E50: the galaxy map's +0x7C last seen)
};

#endif // __UI_STARMAP_H
