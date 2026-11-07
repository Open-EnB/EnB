// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\backint.h (PROVISIONAL, D137: backint.cpp's header; its __FILE__ 0xB78C80)
*/
#ifndef BACKINT_H
#define BACKINT_H

#include "intrface.h"

class RenderObjClass;
class SimpleSceneClass;

// backint.cpp's interface (0x8C bytes, tables 0xAEE924 / 0xAEE918 / 0xAEE90C): the backdrop (the main view's +0x1DC
// over its +0x110 scene, the scripted movie view's skybox over its background scene). It keeps a model in its scene,
// the override's when it has one, else the backdrop's, and moves it onto the view's camera every frame.
class BackInterfaceClass : public InterfaceClass	// NAME 2: the member's name 'BackInterface' (the scripted movie's asserts, 0x804800) and the file's
{
public:
	BackInterfaceClass(UIViewClass *view, SimpleSceneClass *scene);	// 0x53AA30  // NAME 1 [view, scene]: what 0x50C690 / 0x804800 pass
	virtual ~BackInterfaceClass();										// 0x53AAB0 (deleting 0x53AA80)
	virtual void Unknown_54();											// 0x53AB80: the backdrop's NULL object
	virtual void Unknown_58();											// 0x53AC00: the shown model into the scene, after the base's
	virtual void Unknown_5C();											// 0x53AC40: the shown model out, then the base's
	virtual void Unknown_60();											// 0x53AE30: the shown model to the camera's position
	virtual void Unknown_88(RenderObjClass *robj);						// 0x53AC80: the backdrop model (G_ASSERT 'robj')  // NAME 3 [robj]: the assert's
	virtual void Unknown_8C(RenderObjClass *robj);						// 0x53AD40: the override model (NULL: back to the backdrop)  // NAME 1 [robj]: as +0x88's
	RenderObjClass *Backdrop;											// +0x84  // NAME 1: what 0x513540 hands it
	RenderObjClass *Override;											// +0x88  // NAME 1: it takes the backdrop's place while set
};

#endif
