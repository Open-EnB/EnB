// Earth & Beyond client (reconstruction): app\client\unknown_5de6e0.cpp (PROVISIONAL file name, D137). Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_mesh.h"
#include "unknown_5de6e0.h"

// 0x5DE6E0
Unknown5DE6E0Class::Unknown5DE6E0Class(const char *name, SceneClass *scene) :
	Mesh(new UIMeshClass(name, 1)),
	Scene(scene)
{
	AnimName = new char[strlen(name) * 2 + 4];
	sprintf(AnimName, "%s.%s-1", name, name);
}

// 0x5DE7B0
Unknown5DE6E0Class::~Unknown5DE6E0Class()
{
	delete Mesh;
	delete [] AnimName;
}

// 0x5DE7F0
void Unknown5DE6E0Class::Unknown_5DE7F0()
{
	Mesh->Unknown_5B9290((SimpleSceneClass *)Scene);
	Mesh->Unknown_5B8EE0(0, 0, AnimName, false, false, false, 0, -1);
}

// 0x5DE830
void Unknown5DE6E0Class::Unknown_5DE830()
{
	Mesh->Unknown_5B95E0(0, 0);
	Mesh->Unknown_5B92B0();
}

// 0x5DE860
void Unknown5DE6E0Class::Unknown_5DE860()
{
	Mesh->Unknown_5B9410();
}

// 0x5DE880
bool Unknown5DE6E0Class::Unknown_5DE880()
{
	return(Mesh->Unknown_5BA2A0(0));
}

/*
**	Reconstruction notes (app\client\unknown_5de6e0.cpp, PROVISIONAL name, D137).
**
**	The object 0x5DE6E0-0x5DE88A: ui_pterm.cpp ends with its guarded <string> initialiser 0x5DE690 and its adjustor
**	thunks 0x5DE6C0 / 0x5DE6D0 (as ui_ptmis.cpp's object ends), and none of these six functions is inlined into
**	ui_pterm.cpp's callers (0x5DE860 is two instructions), so they are compiled apart; ui_ptmis.cpp's constructor
**	follows at 0x5DE8A0. No string names the file or the class (neutral names, D10). The name buffer is twice the
**	name and 4 bytes ('%s.%s-1').
*/
