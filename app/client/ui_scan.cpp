// Earth & Beyond client (reconstruction): app\client\ui_scan.cpp, the cockpit's scanner (per the client's asserts)
#include <string.h>
#include <string>
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "co.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "fontdatabase.h"
#include "ui_mesh.h"
#include "ui_verb.h"
#include "targeting.h"
#include "client_object_events.h"
#include "unknown_signalbase.h"
#include "aabox.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "ui_scan.h"

class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::Unknown_18; SClient's returns itself)  // NAME 3: the FAIL_IF's
};

extern unsigned int Unknown_BE5F34;		// (gadget.cpp's: the event mask)
extern KernelEventClass Unknown_BE5EB0;	// (gadget.cpp's: the last event)

// 0xB8892C (the two target buttons, ids 6 and 7)
static const char *UIScanButtonNames[] = {	// NAME 2: what it holds
	"PREV_TARGET", "NEXT_TARGET"
};

// @0x60D800
UIScanClass::UIScanClass(UIViewClass *view, LayerClass *layer) :
	Unknown5BE980Class(view, NULL, *layer),
	UIMesh(NULL),
	Unknown198(-1),
	Verb(NULL),
	Threat(NULL),
	Unknown1A4(0)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// NOT EXACT: the target's watch (Remove, the signal map's 0x610460 / 0x4F98D0 inline) and the threat object's inline destructor (WE9: functional)
// @0x60DF90
UIScanClass::~UIScanClass()
{
	if (Object != NULL) {
		Unknown_4F7EF0().Remove(Object, this, &UIScanClass::Unknown_60ED10, PlayerObjectRelationshipChangeEvent());
	}
	REF_PTR_RELEASE(Verb);
	if (Threat != NULL) {
		delete Threat;
	}
	Threat = NULL;
	if (UIMesh != NULL) {
		delete UIMesh;
		UIMesh = NULL;
	}
}

// NOT EXACT: functional (WE9; similarity 0.59): the verb window's and the fonts' set-up order and registers
// @0x60D8D0
void UIScanClass::Unknown_54()
{
	Create_Mesh();









































































































	G_ASSERT(UIMesh);
	for (int i = 1; i < 3; i++) {
		RenderObjClass *robj = RObj->Get_Sub_Object_By_Name(UIScanButtonNames[i - 1]);




		G_ASSERT(robj);
		GadgetClass *gadget = Buttons[i] = new ButtonGadgetClass(robj, Layer, 0x55, 3);
		GadgetClass::Set_Tooltip(gadget, "UI_SCAN", UIScanButtonNames[i - 1]);
		robj->Release_Ref();
		Unknown64.Unknown_6870E0(gadget, i + 5, View, 1, 0);
	}
	AABoxClass box;
	RObj->Get_Obj_Space_Bounding_Box(box);
	Verb = new UIVerbClass(View->Unknown_20(), Layer, UIMesh, 0);
	Verb->Unknown_54();
	View->Unknown_6B4940(Verb);
	Threat = new UIScanThreatClass(this);
	RenderObjClass *robj = RObj;


























	G_ASSERT(robj);
	ConFont = Create_Get(new FontSys("MFD_CON", (int)robj, NULL));
	FactionFont = Create_Get(new FontSys("MFD_FACTION", (int)robj, NULL));
	int level = robj->Get_Sort_Level();
	int sort = 8;
	if (level == 0) {
		RefCountPtr<RenderObjClass> frame = Create_Get(robj->Get_Sub_Object_By_Name("FRAME_SELECT"));
		if (frame) {
			level = frame->Get_Sort_Level();
		}
	}
	if (level > 1) {
		sort = level - 1;
	}
	ConFont->Set_Sort_Level(sort);
	FactionFont->Set_Sort_Level(sort);
	Unknown5BE980Class::Unknown_54();
}

// The 'TSI08A' mesh, placed at the screen's lower right, as the display's model
inline void UIScanClass::Create_Mesh()
{
























	G_ASSERT(!UIMesh);
	UIMesh = new UIMeshClass("TSI08A", 5);
	RenderObjClass *robj = UIMesh->Peek_Mesh_Object();
	if (robj != NULL) {
		robj->Set_Position(Vector3(248.0f, -162.0f, 0.0f));
		Set_RObj(robj);
	}
}

// @0x60E1E0
void UIScanClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		RObj->Get_Bone_Transform("RIGHT_BUTTON");
		RObj->Get_Bone_Transform("verb_menu");
		SceneClass *scene = Layer.Scene;
		if (RObj->Peek_Scene() == NULL && scene != NULL) {
			RObj->Add(scene);
		}
		UIMFDClass::Unknown_58();
	}
}

// @0x60E2C0
void UIScanClass::Unknown_5C()
{
	if (Active) {
		if (RObj->Peek_Scene() != NULL) {
			RObj->Remove();
		}
		UIMFDClass::Unknown_5C();
	}
}

// NOT EXACT: functional (WE9; similarity 0.51): the key tests' block layout
// @0x60E300
bool UIScanClass::Unknown_68(int key)
{
	bool next;
	bool plain = (Unknown_BE5F34 & 0x70) == 0;
	if (key == 6) {
		next = false;
	} else if (key == 7) {
		next = true;
	} else if (Unknown_BE5EB0.Key == 0x101) {
		next = true;
		plain = !(Unknown_BE5F34 & 0x800);
	} else if (Unknown_BE5EB0.Key == 0x102) {
		next = false;
		plain = !(Unknown_BE5F34 & 0x800);
	} else {
		return(UIMFDClass::Unknown_68(key));
	}
	TargetingClass *ts = Get_Targeting();
	if (ts != NULL) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
		if (!next) {
			if (!plain) {
				ts->Select_Prev_Target();
			} else {
				ts->Select_Prev_Target_Filtered();
			}
		} else {
			if (!plain) {
				ts->Select_Next_Target();
			} else {
				ts->Select_Next_Target_Filtered();
			}
		}
		return(true);
	}






























































































































	G_ASSERT(false);
	return(false);
}

// NOT EXACT: functional (WE9; similarity 0.68)
// @0x60E480
void UIScanClass::Unknown_60()
{
	Client->As_Space_Client();
	SClient *sclient = Client->As_Space_Client();
	if (sclient != NULL) {
		RadarSystemClass *radar = sclient->MainView->Radar;
		if (radar != NULL) {	// (+0x48: radar_system.h's GutterCount; sclient.h declares the class as far as cpacket.cpp calls it)
			if (Buttons[2]->Unknown2C_08) {
				GadgetClass::Set_Tooltip(Buttons[2], "UI_SCAN", UIScanButtonNames[1], *(int *)&radar->Unknown00[0x48]);
			} else if (Buttons[1]->Unknown2C_08) {
				GadgetClass::Set_Tooltip(Buttons[1], "UI_SCAN", UIScanButtonNames[0], *(int *)&radar->Unknown00[0x48]);
			}
		}
	}
	Unknown_60F1A0(false);
	UIMesh->Unknown_5B9410();
	UIMFDClass::Unknown_60();
}

// @0x60E540
void UIScanClass::Unknown_60E540(ClientObjectClass *target, const std::string &assessment, const std::string &sound, int level)
{
	Unknown_88(target);
	Unknown_60ED60(target, assessment, sound, level);
}

// NOT EXACT: order (WE9; similarity 0.85): the client builds the old target's Remove arguments before calling 0x4F7EF0, ours after
// @0x60E580
void UIScanClass::Unknown_88(ClientObjectClass *object)
{
	ClientObjectClass *old = Object;
	if (object != old) {
		Unknown_80();
		if (old != NULL) {
			Unknown_4F7EF0().Remove(old, this, &UIScanClass::Unknown_60ED10, PlayerObjectRelationshipChangeEvent());
			Threat->Unknown_60FEE0();
		}
		UIMFDClass::Unknown_88(object);
		if (object != NULL) {
			Unknown_4F7EF0().Add(object, this, &UIScanClass::Unknown_60ED10, PlayerObjectRelationshipChangeEvent());
		}
		Unknown_60EFE0(object);
		Verb->Unknown_88(Object);
		Unknown_60F1A0(true);
		Unknown_84();
	}
}

// (inline in UIScanClass's destructor 0x60DF90)
UIScanThreatClass::~UIScanThreatClass()
{
	bool active = Owner->Is_Active();
	if (active) {
		Owner->Unknown_5C();
	}
	Unknown_60FEE0();
	if (active) {
		Owner->Unknown_58();
	}
	REF_PTR_RELEASE(Unknown24);
	REF_PTR_RELEASE(Unknown20);
}

// The cockpit's targeting, through the client
inline TargetingClass *UIScanClass::Get_Targeting()
{

































































































































































































	FAIL_IF(!Client) return(NULL);
	SClient *sclient = Client->As_Space_Client();

	FAIL_IF(!sclient) return(NULL);
	TargetingClass *ts = sclient->Targeting;

	FAIL_IF(!ts) return(NULL);
	return(ts);
}

/*
**	Notes (WE9, 2026-11-07)
**
**	The client's object runs 0x60D750-~0x610560 (__FILE__ 0xB8899C). Rebuilt for the POC's scanner (the radar panel and
**	the target box at the lower right): the constructor, the slots and 0x60E540; the threat object's 0x60ED60 / 0x60FEE0
**	and 0x60ED10 / 0x60EFE0 / 0x60F1A0 are not rebuilt yet. Blank lines keep the asserts on their client lines.
*/
