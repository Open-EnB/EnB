// Earth & Beyond client (reconstruction): app\client\mv3dview.h, the 3-D movie family: the 3-D movie view (the client's
// app\Client\mv3dview.cpp per its asserts), the sounds it plays (mv3dsnd.cpp) and the mesh sound both it and the
// scripted movie put on a model's bone (meshsnd.cpp). The header's name is provisional (NAME 1): one header for the
// three objects until meshsnd.cpp and mv3dsnd.cpp are rebuilt (L16, 2026-11-11). Include it after sclient.h.
#ifndef MV3DVIEW_H  // NAME 1 [mv3dview.h]: provisional header name for mv3dview.cpp
#define MV3DVIEW_H

#include <stdio.h>
#include "sclient.h"
#include "ini.h"
#include "vector.h"
#include "listnode.h"
#include "hashlist.h"
#include "..\shared\observer.h"

class CameraClass;
class SimpleSceneClass;
class LightClass;
class ProxyRObj;
class UIMeshClass;
class RenderObjClass;
struct ScriptedMovieViewerSettingsStruct;

class Sound3DClass;

// meshsnd.cpp's sound on a model's bone (0x50 bytes, no table of its own: +0x00 is a member node, table 0xB07968;
// constructor 0x7ED1D0, destructor 0x7ED300, called directly). The scripted movie's Play_Audio makes them (0x806BD0)
// and so does the 3-D movie's sound table (0x7EDCB0), which fills the members from movie3d.ini's entries of the same
// names. Compiled against the client in meshsnd.cpp (M1, 2026-11-12): the layout holds.
class MeshSoundClass	// NAME 3: class-from-file (D24) for meshsnd.cpp
{
public:
	MeshSoundClass(const char *name, const char *bone_name);	// 0x7ED1D0 (the sound's name and the bone's, both copied; no bone: a 2-D sound)  // NAME 1 [name, bone_name]: what Play_Audio passes
	~MeshSoundClass();									// 0x7ED300: stopped, out of the scene, everything released
	void Unknown_7ED410(RenderObjClass *mesh);			// 0x7ED410 (G_ASSERT 'mesh' l.123): the sound made on the model's bone (a 2-D one without a bone), its settings applied  // NAME 3 [mesh]: the assert's
	void Unknown_7ED630(bool release);					// 0x7ED630: stopped and out of the scene; true also releases the sound and the model  // NAME 1 [release]: what it does
	void Unknown_7ED6C0(int frame, unsigned int ms);	// 0x7ED6C0: a frame: started inside StartFrame..EndFrame, stopped after; the sound follows the bone, its pitch the bone's speed  // NAME 1 [frame, ms]: what the list passes
	bool Unknown_7EDA60(const char *name);				// 0x7EDA60: the sound's name is this one  // NAME 1 [name]: what it is compared with

	DataNode<MeshSoundClass *> Node;					// +0x00 (table 0xB07968; the node's value is this)  // NAME 1: what it is
	int StartFrame;										// +0x10: the entry 'Time%d' in frames  // NAME 1: what 0x7ED6C0 compares
	int EndFrame;										// +0x14: 'Duration%d' after it (-1: no end)  // NAME 1: what 0x7ED6C0 compares
	bool Loop;											// +0x18  // NAME 2: the INI entry's
	float PitchSpeed;									// +0x1C (-1: the pitch is left alone)  // NAME 2: the INI entry's
	float PitchChange;									// +0x20  // NAME 2: the INI entry's
	int PlaybackRate;									// +0x24 (0: the sound's own)  // NAME 2: the INI entry's
	float Volume;										// +0x28 (below 0: left alone, as the next three)  // NAME 2: the INI entry's
	float Priority;										// +0x2C  // NAME 2: the INI entry's
	float DropOffRadius;								// +0x30  // NAME 2: the INI entry's
	float MaxVolRadius;									// +0x34  // NAME 2: Set_Max_Vol_Radius's
	char *BoneName;										// +0x38 (NULL: a 2-D sound)  // NAME 1: the constructor's
	char *Name;											// +0x3C  // NAME 2: the INI entry's
	RenderObjClass *Model;								// +0x40 (referenced)  // NAME 1: what it holds
	int BoneIndex;										// +0x44  // NAME 1: Get_Bone_Index's result
	bool Playing;										// +0x48  // NAME 1: what 0x7ED6C0 keeps in it
	bool Mono;											// +0x49: the debug lines on the mono screen  // NAME 2: the INI entry's
	Sound3DClass *Sound;								// +0x4C  // NAME 1: what it holds
};

// The sounds of one animation (0x24 bytes; meshsnd.cpp's 0x7EDAD0 / 0x7EDB60 / 0x7EDBE0; table 0xB079F4 is the list's)
class MeshSoundListClass	// NAME 1: after MeshSoundClass
{
public:
	MeshSoundListClass() : Frame(0), Started(false) {}	// (inline in 0x7EDCB0)
	~MeshSoundListClass() { while (Sounds.First_Valid()) delete Sounds.First_Valid()->Get(); }	// (inline in 0x7EE340)
	void Unknown_7EDAD0(int frame, RenderObjClass *mesh);	// 0x7EDAD0: every sound made on the model; started at frame  // NAME 1 [frame, mesh]: what 0x7EE690 passes
	void Unknown_7EDB60();								// 0x7EDB60: when started, every sound deleted
	void Unknown_7EDBE0(unsigned int ms);				// 0x7EDBE0: when started, a frame of every sound  // NAME 1 [ms]: what 0x7EE7E0 passes

	int Frame;											// +0x00: the animation's frame (0x7EE770 sets it)  // NAME 1: what it holds
	bool Started;										// +0x04  // NAME 1: what sets it
	List<DataNode<MeshSoundClass *> *> Sounds;			// +0x08 (table 0xB079F4)  // NAME 1: what it holds
};

// The 3-D movie's sounds (mv3dsnd.cpp, 0x40 bytes, no table of its own): each animation id's sounds, read from the
// movie's 'SFX' section of movie3d.ini (constructor 0x7EDCB0, destructor 0x7EE340). Whether the hash list is a base or
// a member is not settled by the bytes (both compile the same, mv3dsnd.cpp M1 2026-11-12); the base is kept.
class Movie3DSoundClass : public HashListClass<MeshSoundListClass *, HashDefaultUserClass, 7>	// NAME 3: class-from-file (D24) for mv3dsnd.cpp, after Movie3DViewClass
{
public:
	Movie3DSoundClass(INIClass &ini, const char *section);	// 0x7EDCB0 (G_ASSERT 'false' l.160: an entry without a 'Name')  // NAME 1 [ini, section]: what 0x7EF220 passes
	~Movie3DSoundClass();								// 0x7EE340
	void Unknown_7EE690(int id, int frame, RenderObjClass *mesh);	// 0x7EE690: the animation's sounds started on the model  // NAME 1 [id, frame, mesh]: what 0x7EF850 passes
	void Unknown_7EE700(int id);						// 0x7EE700: the animation's sounds ended  // NAME 1 [id]: as above
	void Unknown_7EE770(int id, int frame);				// 0x7EE770: the animation's frame  // NAME 1 [id, frame]: as above
	void Unknown_7EE7E0(unsigned int ms);				// 0x7EE7E0: a frame of every animation's sounds  // NAME 1 [ms]: WW3D's frame time
	MeshSoundListClass *Unknown_7EE830(int id);			// 0x7EE830: the animation's sounds or NULL (no callers)  // NAME 1 [id]: as above
};

// One movie of movie3d.ini: its section and which of the section's 'Movie%d' lists to play (mv3dview.cpp's table 0xBA1E50)
struct Movie3DStruct	// NAME 1: neutral (D10)
{
	const char *Section;								// +0x00  // NAME 1: what the INI calls take it as
	int Movie;											// +0x04  // NAME 2: the 'Movie%d' entry's number
};

// The 3-D movie view (0x2A8 bytes, tables 0xB07A94 / 0xB07A88; SClient 0x74BC20 makes it, 0x74BD10 deletes it): the
// meshes of one of movie3d.ini's movies, each playing its channels' animations, in a scene of its own under the
// interface; it observes the meshes to start and stop the animations' sounds
class Movie3DViewClass : public Unknown65B810ViewClass, public ObserverClass	// NAME 2: SClient's member 'Movie3DView' (G_ASSERT l.4104)
{
public:
	Movie3DViewClass(ClientTaskClass *task, int movie);	// 0x7EEF90 (movie: 1..5 of the table, 0: movie3d.ini's [TEST] or a random one)  // NAME 1 [task, movie]: what 0x74BC20 passes / what it selects
	virtual ~Movie3DViewClass();						// +0x00 0x7EF5F0 (deleting 0x7EF1B0)
	virtual void Unknown_04();							// +0x04 0x7EF220: the scene, the camera, the movie's meshes, lights, ship and sounds
	virtual void Unknown_08();							// +0x08 0x7EF5D0: empty
	virtual int Unknown_0C(bool on);					// +0x0C 0x7EF8C0: the movie's scene, then the interface's two; the sounds' frame
	virtual void Unknown_10();							// +0x10 0x7EF7D0: the lights out of the scene, the scene and camera released
	virtual void Unknown_14();							// +0x14 0x7EFAE0: a frame of every mesh, then the camera
	virtual void Unknown_18(bool arg);					// +0x18 0x7EFA40: shown (the client's camera is the movie's; the movie starts) or hidden  // NAME 1 [arg]: the base's
	virtual int Unknown_34() { return((int)this); }		// +0x34 0x7EF170
	virtual SimpleSceneClass *Unknown_5C() { return(Scene); }	// +0x5C 0x7EF190
	virtual void Unknown_00(SubjectClass *subject, void *event);	// ObserverClass +0x00 0x7EF850: a mesh's process moved, started or ended: its sound  // NAME 1 [subject, event]: the base's

	bool Unknown_7EFCC0();								// 0x7EFCC0 (SClient::Do_Frame in view mode 3: true ends the movie, 0x74BD10)
	bool Unknown_7EFD00();								// 0x7EFD00 (G_ASSERT '!Started' l.472): the movie's meshes and their channels' animations; false without a 'Movie%d' entry
	void Unknown_7F0410();								// 0x7F0410: the meshes' animations let go (flag 2 cleared), once
	void Unknown_7F0460();								// 0x7F0460: the lights ('Lights', 'Light%d': the viewer settings' INI) into the scene
	void Unknown_7F06C0();								// 0x7F06C0: the camera and the light onto the camera mesh's 'camera' bone
	char *Unknown_7F0880(int count, ...);				// 0x7F0880 (G_ASSERT 'str && *str' l.697): SectionName = the movie's section and count more names, '\' between  // NAME 1 [count]: what it counts
	char *Unknown_7F09B0(const char *format, int number);	// 0x7F09B0: EntryName printed (defined after its callers, which inline it)  // NAME 1 [format, number]: sprintf's

	void Add_Mesh(UIMeshClass *mesh) { mesh->Unknown_5B9290(Scene); mesh->Attach(this); mesh->Unknown70 |= 3; Meshes.Add(mesh); }	// (inline in 0x7EFD00, with its own copy of this: into the scene, observed, held (flags 1 and 2) and listed)  // NAME 1: what it does

	CameraClass *Camera;								// +0x54  // NAME 1: what it holds
	SimpleSceneClass *Scene;							// +0x58  // NAME 1: what it holds
	DynamicVectorClass<UIMeshClass *> Meshes;			// +0x5C (tables 0xB07B24 / 0xB07B08)  // NAME 1: what it holds
	Movie3DSoundClass *Sounds;							// +0x74 (Unknown_04 makes it from the section's 'SFX')  // NAME 1: what it holds
	ScriptedMovieViewerSettingsStruct *Settings;		// +0x78: the scene light and the background colour  // NAME 1: after the struct
	LightClass *Light;									// +0x7C: the light that follows the camera  // NAME 1: what it holds
	ProxyRObj *Ship;									// +0x80: the avatar's model, on the 'Ship' mesh's bone  // NAME 2: the INI entry's
	int CameraMesh;										// +0x84: the mesh whose 'camera' bone carries the camera (-1: none)  // NAME 2: the INI entry 'Camera'
	INIClass INI;										// +0x88: movie3d.ini  // NAME 1: what it holds
	int Frames;											// +0x98: frames rendered (the fifth clears the meshes' flag 1)  // NAME 1: what it counts
	Movie3DStruct *Movie;								// +0x9C: the movie playing  // NAME 1: what it points to
	int MovieIndex;										// +0xA0: the constructor's  // NAME 1: what it indexes
	bool Loaded;										// +0xA4: Unknown_04 found the movie  // NAME 1: what sets it
	bool Started;										// +0xA5  // NAME 3: the G_ASSERT's '!Started' (l.472)
	char SectionName[256];								// +0xA6 (0x7F0880 builds it)  // NAME 1: what the INI calls take it as
	char EntryName[256];								// +0x1A6 (0x7F09B0 prints it)  // NAME 1: what the INI calls take it as
};

#endif
