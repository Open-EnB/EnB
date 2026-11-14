// Earth & Beyond client (reconstruction): app\client\unknown_5de6e0.h, a mesh animation the petition terminal plays
// (the object 0x5DE6E0-0x5DE88A between ui_pterm.cpp's adjustor thunks and ui_ptmis.cpp; it has no assert or string of
// its own file, so the client names neither the file nor the class: interim placement, D137, NAME 1).
#ifndef UNKNOWN_5DE6E0_H  // NAME 1 [unknown_5de6e0.h]: provisional name (D137): the client's file sorts between ui_pterm.cpp and ui_ptmis.cpp
#define UNKNOWN_5DE6E0_H

class UIMeshClass;
class SceneClass;

// The animation 'name.name-1' of a mesh of its own (0x0C bytes; ui_pterm.cpp's 0x5DE230 makes one with 'icl01a')
class Unknown5DE6E0Class  // NAME 1: neutral (D10)
{
public:
	Unknown5DE6E0Class(const char *name, SceneClass *scene);	// 0x5DE6E0  // NAME 1 [name, scene]: what 0x5DE230 passes
	~Unknown5DE6E0Class();								// 0x5DE7B0
	void Unknown_5DE7F0();								// 0x5DE7F0: into the scene, the animation started on channel 0
	void Unknown_5DE830();								// 0x5DE830: stopped, out of the scene
	void Unknown_5DE860();								// 0x5DE860: one frame of the mesh's processes
	bool Unknown_5DE880();								// 0x5DE880: the mesh's channel 0 idle (the animation done)

	UIMeshClass *Mesh;									// +0x00  // NAME 1: what it holds
	SceneClass *Scene;									// +0x04  // NAME 1: what it holds
	char *AnimName;										// +0x08 ('%s.%s-1')  // NAME 1: what it holds
};

#endif // UNKNOWN_5DE6E0_H
