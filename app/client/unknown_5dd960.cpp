// Earth & Beyond client (reconstruction): the face / target / orient buttons' display (PROVISIONAL name, D137)
#include <string>
#include "sclient.h"
#include "unknown_675120.h"
#include "ui_mesh.h"
#include "..\audio\soundlib.h"
#include "unknown_5dd960.h"

// 0xB84B94 (the three buttons, ids 6 to 8)
static const char *Unknown5DD960ButtonNames[] = {	// NAME 2: what it holds
	"face_button", "target_button", "orient_button"
};

// NOT EXACT: functional (O28; similarity 0.87): only the loop's set-up order (the names' pointer loaded after, not before, the
// buttons' address); Buttons[3] at +0x18C indexed i - 1 and a loop from 0 tried
// @0x5DD960
Unknown5DD960Class::Unknown5DD960Class(UIViewClass *view, UIMeshClass *mesh, const LayerClass &layer) :
	Unknown5BE4F0Class(view, mesh->Peek_Object(), layer)
{
	for (int i = 1; i < 4; i++) {
		GadgetClass *gadget = Buttons[i] = new ButtonGadgetClass(mesh, Unknown5DD960ButtonNames[i - 1], layer, 0x55, 3);
		Unknown64.Unknown_6870E0(gadget, i + 5, view, 1, 0);
	}
}

// @0x5DDAE0
Unknown5DD960Class::~Unknown5DD960Class()
{
}

// @0x5DDB20
void Unknown5DD960Class::Unknown_58()
{
	UIMFDClass::Unknown_58();
}

// @0x5DDB40
void Unknown5DD960Class::Unknown_5C()
{
	UIMFDClass::Unknown_5C();
}

// @0x5DDB60
bool Unknown5DD960Class::Unknown_68(int key)
{
	SClient *sclient = (SClient *)Client;
	switch (key - 5) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
			sclient->Unknown_74D990(0x13, "Facing not implemented.", 0);
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
			sclient->Unknown_74D990(0x13, "Tracking not implemented.", 0);
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
			sclient->Unknown_74D990(0x13, "Orient not implemented.", 0);
			return(true);
	}
	return(UIMFDClass::Unknown_68(key));
}

// @0x5DDC20
void Unknown5DD960Class::Unknown_60()
{
	UIMFDClass::Unknown_60();
}

/*
** Reconstruction notes (O28, 2026-11-14)
** - The object: 0x5DD960-0x5DDCA0, between ui_proc.cpp (which ends at 0x5DD910) and the TLI10A panel at 0x5DDCA0; no
**   __FILE__, assert or ini name of its own, so the file and class keep neutral names (D137). Its data: the button
**   names' table 0xB84B94 and the strings 0xB84BA4-0xB84C28.
** - Nothing in the client constructs it (no caller of 0x5DD960 or its thunk 0x410195): a display left in, its three
**   buttons answering 'not implemented' on chat colour 0x13.
** - The code after the members: <string>'s guarded static initialiser 0x5DDC40 (guard 0xC416FC, atexit 0x42B320, as
**   every object that includes it), then the deleting destructor's thunks for the other tables (0x5DDC70 / 0x5DDC80 /
**   0x5DDC90).
** - Unknown_68's switch is on key - 5 (cases 1 to 3): the client's lea eax, [edx - 5] before the decrements; on the
**   key itself (cases 6 to 8) VC6 subtracts 6 at once.
** - Unknown_58 / 5C / 60 only call UIMFDClass's (Unknown5BE4F0Class has none of its own); VC6 makes each a jmp.
*/
