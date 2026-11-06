// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\map_pip.h (name assumed from map_pip.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\map_pip.cpp'; the client holds no string of this header)
**
**	The base of the map picture-in-picture views. Constructor 0x6927C0, table 0xAFD078, 0x50 bytes.
**	Its first 0x34 bytes come from pip_scene.cpp's constructor 0x7BA9C0 (pip_scene.h).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MAP_PIP_H  // NAME 3 [map_pip.h]: header name assumed from the client's map_pip.cpp
#define __MAP_PIP_H

#include "always.h"
#include "ref_ptr.h"
#include "point.h"
#include "vector2.h"
#include "gadget.h"
#include "pip_scene.h"

class SClient;
class InterfaceClass;
class SimpleSceneClass;
class CameraClass;
class RenderObjClass;
class UIViewClass;
class StarmapPipSupportClass;

// The gadget a map PIP makes in slot +0x18 (gd_pip_scene.cpp's constructor 0x66ACE0), as far as this file reads it
class GadgetPipSceneClass : public GadgetClass  // NAME 3: class-from-file (D24) for app\client\gd_pip_scene.cpp
{
public:
	char Unknown_Pad30[0x38 - 0x30];
	TPoint2D<float> Unknown38;							// +0x38 (0x53F9E0 hands it to 0x7A72C0 by value)
	char Unknown_Pad40[0x49 - 0x40];
	bool Unknown49;										// +0x49 (0x693020)
};

class MapPipClass : public PipSceneClass  // NAME 3: class-from-file (D24) for app\Client\map_pip.cpp
{
public:
	MapPipClass(SClient *client, bool arg);				// 0x6927C0 (arg: the two 0x118-byte objects at +0x48 / +0x4C)  // NAME 1 [client, arg]: neutral
	virtual ~MapPipClass();								// +0x00 0x692A60 (deleting 0x692A30)
	virtual bool Unknown_04(const RefCountPtr<InterfaceClass> &iface, const RefCountPtr<RenderObjClass> &vp, int arg);	// +0x04 0x692B30 (FAIL_IF 'Interface', '!iface_ptr', '!vp', '!view', '!ViewportGadget'; arg: the gadget's id in the interface)  // NAME 2 [iface, vp]: the FAIL_IFs' iface_ptr is its Peek  // NAME 1 [arg]: neutral
	virtual bool Unknown_08();							// +0x08 0x692D40 (FAIL_IF 'ViewportGadget')
	virtual bool Unknown_0C(bool on);					// +0x0C 0x692E00 (FAIL_IF '!Interface')  // NAME 1 [on]: neutral
	virtual bool Unknown_10(const RefCountPtr<RenderObjClass> &vp);	// +0x10 0x692EF0 (0x53F770 passes the vp)  // NAME 2 [vp]: what 0x692B30 passes
	virtual void Unknown_14();							// +0x14 0x693000 (empty)
	virtual GadgetClass *Unknown_18() = 0;				// +0x18 pure (0x53F6F0)
	virtual void Unknown_1C(int arg);					// +0x1C 0x6929D0 (empty; 0x53F920)  // NAME 1 [arg]: neutral
	virtual void Unknown_20(int arg);					// +0x20 0x6929F0 (empty; 0x53F4F0)  // NAME 1 [arg]: neutral
	virtual void Unknown_24(int arg);					// +0x24 0x692A10 (empty; 0x5408B0)  // NAME 1 [arg]: neutral

	bool Unknown_693020();								// 0x693020 (ViewportGadget's +0x49)

	SClient *Client;									// +0x34 (0x6927C0's client)  // NAME 1: neutral (D10): what it holds
	GadgetPipSceneClass *ViewportGadget;				// +0x38  // NAME 3: the FAIL_IF's
	RefCountPtr<InterfaceClass> Interface;				// +0x3C  // NAME 3: the FAIL_IF's
	bool Unknown40;										// +0x40
	int Unknown44;										// +0x44
	RefCountPtr<StarmapPipSupportClass> Unknown48;		// +0x48 (arg's: 0x7C3AC0)
	RefCountPtr<StarmapPipSupportClass> Unknown4C;		// +0x4C (arg's: 0x7C3AC0)
};

#endif // __MAP_PIP_H
