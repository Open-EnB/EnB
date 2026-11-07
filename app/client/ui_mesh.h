// Earth & Beyond client (reconstruction): app\client\ui_mesh.h, the interface's animated mesh (the client's ui_mesh.cpp
// per its asserts; this header's name is provisional, NAME 1). Moved out of sclient.h and laid out from the constructors
// 0x5B8120 / 0x5B8330 and the destructor 0x5B8640 (UM1, 2026-11-07). sclient.h includes it.
#ifndef UI_MESH_H  // NAME 1 [ui_mesh.h]: provisional header name for ui_mesh.cpp
#define UI_MESH_H

#include "vector.h"
#include "listnode.h"
#include "slist.h"
#include "matrix3d.h"
#include "ui_proc.h"
#include "..\shared\observer.h"

class RenderObjClass;
class SimpleSceneClass;
class HAnimComboClass;
class ClientClass;
class GadgetClass;
class TextureClass;

typedef List<DataNode<UIProcClass *> *> UIProcListClass;	// table 0xAF3218 (deleting destructor 0x5BA5C0; ~GenericList 0x5B8260)  // NAME 1: neutral

// The mesh (0x74 bytes, table 0xAF31DC; observer.cpp's subject at +0x04, its constructor 0x49E890 and destructor 0x49E950
// called out of line, the destructor's through a null-checked this + 4): a model, its animation channels and the processes playing on them
class UIMeshClass : public SubjectClass	// NAME 3: class-from-file (D24) for ui_mesh.cpp
{
public:
	UIMeshClass(const char *name, int count);			// 0x5B8120 (ui_scut.cpp's 0x610570 passes 2)  // NAME 1 [name, count]: what Create_Object passes / what 0x5B8850 does with it
	UIMeshClass(const char *name, ClientClass *client);	// 0x5B8330 (a ship's: Create_Object; the model a render state track)  // NAME 3 [client]: the assert's
	~UIMeshClass();										// 0x5B8640 (Destroy_Object deletes it)
	virtual void Unknown_00();							// +0x00 0x5B93F0: 0x5B9410

	void Unknown_5B8850(const char *name, int count);	// 0x5B8850: the model of that name with count channels (ui_ship_cust.cpp's 0x61BD80: the page's style name, 1)  // NAME 1 [name, count]: neutral
	RenderObjClass *Peek_Object();						// 0x5B8B00 (the asserts' '(*itor).Mesh->Peek_Object()')  // NAME 3: the assert's
	RenderObjClass *Unknown_5B8B20();					// 0x5B8B20: the model with a reference added (Add_Camera)
	RenderObjClass *Unknown_5B8B40(const char *name);	// 0x5B8B40: a sub-object by name ('a.b': b inside a), a reference added  // NAME 1 [name]: neutral
	void Unknown_5B8C10(RenderObjClass *robj, float r, float g, float b);	// 0x5B8C10: every vertex material's emissive  // NAME 1 [robj, r, g, b]: neutral
	void Unknown_5B8CB0(RenderObjClass *robj);			// 0x5B8CB0: the same, 0.35 grey
	void Unknown_5B8D50(RenderObjClass *robj);			// 0x5B8D50: the same, white
	void Unknown_5B8DF0(int id, const char *anim_name, bool loop, bool a, bool b, int c, int d);	// 0x5B8DF0: an animation onto the mesh  // NAME 1 [id, loop, a..d]: neutral; NAME 2 [anim_name]: 0x5DCD20's FAIL_IF
	void Unknown_5B8EE0(int slot, int id, const char *anim_name, bool loop, bool a, bool b, int c, int d);	// 0x5B8EE0: the same onto a channel (ui_verb.cpp: the button's index twice, its animation, 0s, -1)  // NAME 1 [slot, id, loop, a..d]: neutral
	void Unknown_5B8FF0(int slot, int id, HAnimClass *morph_anim, bool loop, bool a, bool b, int c, int d);	// 0x5B8FF0: a morph onto a channel  // NAME 2 [morph_anim]: 0x5DCF00's FAIL_IF; NAME 1 [slot, id, loop, a..d]: neutral
	void Unknown_5B9100(const char *bone_name, RenderObjClass *object);	// 0x5B9100: object onto the mesh's bone  // NAME 2 [bone_name]: the FAIL_IF's 'Need bone name'; NAME 1 [object]: neutral
	void Unknown_5B9130(const char *bone_name, UIMeshClass *child);	// 0x5B9130: child onto the mesh's bone (Attach_To_Bone)  // NAME 2 [bone_name, child]: the asserts' / FAIL_IF's
	int Unknown_5B9160(const char *bone_name);			// 0x5B9160: the bone's index  // NAME 1 [bone_name]: neutral
	Matrix3D Unknown_5B9190(int bone);					// 0x5B9190: the bone's transform  // NAME 1 [bone]: neutral
	Matrix3D Unknown_5B9210(const char *bone_name);		// 0x5B9210: the bone's transform  // NAME 1 [bone_name]: neutral
	void Unknown_5B9290(SimpleSceneClass *scene);		// 0x5B9290: the model into the scene  // NAME 1 [scene]: neutral
	void Unknown_5B92B0();								// 0x5B92B0: out of its scene (Destroy_Object, before the delete)
	UIProcClass *Unknown_5B92D0(int slot);				// 0x5B92D0: the channel's next process that starts, moved to the main list  // NAME 1 [slot]: neutral
	void Unknown_5B9410();								// 0x5B9410: one frame of the processes
	void Unknown_5B9550(int id);						// 0x5B9550: ends the main list's processes of that id (the Play_Animation commands pass 0)  // NAME 1 [id]: neutral
	void Unknown_5B95E0(int slot, int arg);				// 0x5B95E0  // NAME 1 [slot, arg]: neutral
	void Unknown_5B9830();								// 0x5B9830: ends every process of the main list (the cockpit's)
	void Unknown_5B98B0(int slot, int id);				// 0x5B98B0: deletes a channel's waiting processes of that id (-1: all)  // NAME 1 [slot, id]: neutral
	void Unknown_5B9930(int id);						// 0x5B9930: the same on every channel (ui_cpit.cpp passes -1)  // NAME 1 [id]: neutral
	void Unknown_5B99C0(GadgetClass *gadget);			// 0x5B99C0  // NAME 1 [gadget]: what 0x5BB4F0 passes
	void Unknown_5B99F0(GadgetClass *gadget);			// 0x5B99F0  // NAME 1 [gadget]: neutral
	void Unknown_5B9A20(int id, bool on);				// 0x5B9A20: the gadgets of that id (slot +0x40 the reverse)  // NAME 1 [id, on]: neutral
	void Unknown_5B9A70(int slot, int arg, bool on);	// 0x5B9A70  // NAME 1 [slot, arg, on]: neutral
	void Unknown_5B9BC0(int slot, const char *name, int arg);	// 0x5B9BC0  // NAME 1 [slot, name, arg]: neutral
	void Unknown_5B9D80(const char *name, TextureClass *texture);	// 0x5B9D80: the texture onto a sub-object (ro_funcs.cpp's 0x495FD0)  // NAME 1 [name, texture]: neutral
	void Unknown_5B9DF0(int slot, const char *name, bool on);	// 0x5B9DF0  // NAME 1 [slot, name, on]: neutral
	void Unknown_5B9FB0(const char *name, bool hide);	// 0x5B9FB0: a sub-object hidden or shown  // NAME 1 [name, hide]: neutral
	bool Unknown_5BA030(const char *name);				// 0x5BA030: a sub-object hidden (true when missing)  // NAME 1 [name]: neutral
	void Unknown_5BA0B0(int slot, const char *name);	// 0x5BA0B0  // NAME 1 [slot, name]: neutral
	void Unknown_5BA270(const char *name);				// 0x5BA270: a 2-D sound  // NAME 1 [name]: neutral
	bool Unknown_5BA2A0(int slot);						// 0x5BA2A0: the channel idle  // NAME 1 [slot]: neutral
	bool Unknown_5BA320(int index, bool reverse);		// 0x5BA320: the combo's animation at an end  // NAME 1 [index, reverse]: neutral
	int Unknown_5BA3B0(int slot);						// 0x5BA3B0: the id playing on a channel (-1: none)  // NAME 1 [slot]: neutral
	void Unknown_5BA430(float scale);					// 0x5BA430  // NAME 1 [scale]: neutral
	void Unknown_5BA460(int arg);						// 0x5BA460: empty (the cockpit's)  // NAME 1 [arg]: neutral
	RenderObjClass *Unknown_5E26D0();					// 0x5E26D0: Peek_Object's read, out of line in ui_ptrade.cpp's window (0x809600, 0x808C30)
	RenderObjClass *Peek_Mesh_Object() { return(InterfaceMesh); }

	RenderObjClass *InterfaceMesh;						// +0x1C: the model  // NAME 3: the asserts' '(InterfaceMesh != 0)' (l.47) / FAIL_IF '!InterfaceMesh' (l.101)
	SList<GadgetClass> Gadgets;							// +0x20 (table 0xAF321C)  // NAME 1: what it holds
	HAnimComboClass *Unknown2C;							// +0x2C: the channels' blend
	UIProcListClass Unknown30;							// +0x30: the running processes
	VectorClass<UIProcListClass *> Unknown4C;			// +0x4C (table 0xAF31FC): each channel's waiting processes
	VectorClass<bool> Unknown5C;						// +0x5C (table 0xAF31E0): each channel busy
	int Unknown6C;										// +0x6C: the channels
	int Unknown70;										// +0x70: flags (the Play_Animation commands clear bit 0, set or clear bit 1)
};

#endif
