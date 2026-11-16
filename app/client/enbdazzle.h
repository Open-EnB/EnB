// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\enbdazzle.h (name assumed from enbdazzle.cpp's __FILE__)
**
**	The main view's dazzle visibility handler (mainview.cpp's +0x128, made by 0x50BA60 and handed to the dazzle
**	layer): a dazzle is as visible as its orbital (OrbitalManager 0x4F5090) times the share of 16 sample points
**	around it that no land or visible object of the main scene hides from the camera.
**
**	THE CLIENT'S LAYOUT (8 bytes), from the constructor 0x75DD00; vtable 0xB03D38 (one slot, 0x75DEE0).
*/

#ifndef ENBDAZZLE_H
#define ENBDAZZLE_H

#include "..\..\ww3d2\dazzle.h"

class SClient;

class MainViewDazzleHandlerClass : public DazzleVisibilityClass	// NAME 1: neutral (D10)
{
public:
	MainViewDazzleHandlerClass(SClient *client); ~MainViewDazzleHandlerClass();	// 0x75DD00 / 0x75DE70 (Shutdown 0x50C2F0)  // NAME 1 [client]: what 0x50BA60 passes
	virtual float Compute_Dazzle_Visibility(RenderInfoClass &rinfo, DazzleRenderObjClass *dazzle, const Vector3 &point) const;	// +0x00 0x75DEE0  // NAME 3: the base's
	SClient *Client;								// +0x04  // NAME 2: what it holds
};

#endif
