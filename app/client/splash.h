// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\splash.h after the class's name, D137)
**
**	Splash: the start-up and login splash scene (0x10 bytes; constructor 0x6A9B60, destructor 0x6A9DB0,
**	Render 0x6A9E40 by its profile string 'Splash::Render'). WinMain 0x526DE0 shows type 2 or 0
**	before the options load; LoginTaskClass shows type 1 once the login is accepted.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SPLASH_H  // NAME 3 [splash.h]: after the class name 'Splash' (0x6A9E40's profile string)
#define SPLASH_H

class CameraClass;
class SimpleSceneClass;
class RenderObjClass;

class Splash
{
public:
	Splash(int type);			// NAME 1 [type]: 0 / 2 an 'install%02da' model (2 adds 'demo-esrb01'), 1 'starmap%02da'
	~Splash();
	void Render();

	CameraClass *Camera;		// +0x00  // NAME 1 [Camera]: a CameraClass (0x384 bytes)
	SimpleSceneClass *Scene;	// +0x04  // NAME 1 [Scene]: a SimpleSceneClass (0x98 bytes)
	RenderObjClass *Model;		// +0x08  // NAME 1 [Model]: the asset manager's 'install%02da' / 'starmap%02da'
	RenderObjClass *Rating;		// +0x0C  // NAME 1 [Rating]: 'demo-esrb01', a sub-object of Model (type 2)
};

#endif
