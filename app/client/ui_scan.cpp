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
static DurationClass UIScanInterruptDelay(2000);	// 0xBE5E10 (its initialiser 0x60D7D0): how long the threat's interrupt message stays  // NAME 1: neutral (D10)
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
	InterruptInfo(NULL),
	Unknown1A4(0)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// (exact with the threat's +0x14 / +0x30 as RefCountPtr members and a plain 'delete InterruptInfo', its null test the delete's own; TL1)
// @0x60DF90
UIScanClass::~UIScanClass()
{
	if (Object != NULL) {
		Unknown_4F7EF0().Remove(Object, this, &UIScanClass::Unknown_60ED10, PlayerObjectRelationshipChangeEvent());
	}
	REF_PTR_RELEASE(Verb);
	delete InterruptInfo;
	InterruptInfo = NULL;


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
	InterruptInfo = new UIScanThreatClass(this);
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

// NOT EXACT: register (TL1; similarity 0.54, WE9's 0.51): the client keeps the key's slot for next and this in ecx, tests the key as -5 then dec, dec
// @0x60E300
bool UIScanClass::Unknown_68(int key)
{
	bool modifier = (Unknown_BE5F34 & 0x70) != 0;
	bool control = (Unknown_BE5F34 & 0x800) != 0;
	bool next;
	bool shifted;
	switch (key) {
		case 6: next = false; shifted = modifier; break;
		case 7: next = true; shifted = modifier; break;
		default:
			if (Unknown_BE5EB0.Key == 0x101) { next = true; shifted = control; }
			else if (Unknown_BE5EB0.Key == 0x102) { next = false; shifted = control; }
			else return(UIMFDClass::Unknown_68(key));
	}
	bool plain = !shifted;


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

// (exact with the gadget's Is_Unknown2C_08(): its bool return gives the client's shr / test dl, 1; TL1)
// @0x60E480
void UIScanClass::Unknown_60()
{
	Client->As_Space_Client();
	SClient *sclient = Client->As_Space_Client();
	if (sclient != NULL) {
		RadarSystemClass *radar = sclient->MainView->Radar;
		if (radar != NULL) {	// (+0x48: radar_system.h's GutterCount; sclient.h declares the class as far as cpacket.cpp calls it)
			if (Buttons[2]->Is_Unknown2C_08()) {
				GadgetClass::Set_Tooltip(Buttons[2], "UI_SCAN", UIScanButtonNames[1], *(int *)&radar->Unknown00[0x48]);
			} else if (Buttons[1]->Is_Unknown2C_08()) {
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
			InterruptInfo->Unknown_60FEE0();
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


}

#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h, as ui_faction.cpp)
#include "cfaction_database.h"
#include <stdio.h>
#include "co_player_relationship.h"
// A field under root that is set (+0x70), else NULL; and one changed since (+0x70 above 1): ui_faction.cpp's
template<class C> inline C *Unknown_Find_Set_Field(DeltaCompoundClass *root, const NameClass &name)	// NAME 1: neutral (D10)
{ C *t = Unknown_Find_Field<C>(root, name); return((t && t->Unknown70) ? t : NULL); }
template<class C> inline C *Unknown_Find_Changed_Field(DeltaCompoundClass *root, const NameClass &name)	// NAME 1: neutral (D10)
{ C *t = Unknown_Find_Field<C>(root, name); return((t && t->Unknown70 > 1) ? t : NULL); }
#include "..\..\ww3d2\assetmgr.h"
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: avatar.cpp's)
void Unknown_496730(RenderObjClass *robj, int sort_level, bool dec_if_match);	// 0x496730 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, sort_level, dec_if_match]: ui_cpit.cpp's
#pragma auto_inline(off)
template DeltaTemplateClass<DurationClass> *Unknown_Find_Field<DeltaTemplateClass<DurationClass> >(DeltaCompoundClass *root, const NameClass &name);	// (instantiated here, under auto_inline(off): the client calls it out of line, 0x610290)
template DeltaStringClass *Unknown_Find_Field<DeltaStringClass>(DeltaCompoundClass *root, const NameClass &name);	// (instantiated here, under auto_inline(off): the client calls it out of line, 0x514BD0)
// The interrupt display: the 'tsi08c' model on the scanner's 'BONE_INTER' bone, its progress display (+0x2C) and
// the ability's name on 'MFD_INTERRUPT'; the faction line hidden meanwhile
// NOT EXACT: inline-budget (TL2; similarity 0.43): the client calls the three finds (0x610290, 0x514BD0) out of line and
// assigns the cleared name through assign(const char *, size_type) out of line, and reads the time through 0x60ECD0 (the
// time field's value; a Get_Value declared in delta_template.h costs aux_roots 8 pairs); ours inlines them, its registers differ
// @0x60E6A0
void UIScanClass::Unknown_60E6A0(ClientObjectClass *object)
{










	G_ASSERT(InterruptInfo);
	Unknown_80();
	InterruptInfo->Unknown18 = false;
	if (!InterruptInfo->Unknown30) {
		InterruptInfo->Unknown30 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi08c"));
		if (InterruptInfo->Unknown30) {
			Unknown_496AE0(InterruptInfo->Unknown30.Peek(), 0);
			UIMesh->Peek_Mesh_Object()->Add_Sub_Object_To_Bone(InterruptInfo->Unknown30.Peek(), "BONE_INTER");
			InterruptInfo->Unknown_Clear();
			InterruptInfo->Unknown2C = new Unknown59E2C0Class(InterruptInfo->Unknown30.Peek(), &Layer, this, object, -1);
			Unknown_496730(InterruptInfo->Unknown30.Peek(), 8, 1);
		}
	}
	if (InterruptInfo->Unknown30) {
		ClientObjectClass *target = Object;
		if (!target || !target->AuxData) return;
		InterruptInfo->Unknown2C->Unknown_59E600();
		InterruptInfo->Unknown2C->Unknown_60EC70(DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC));
		DeltaTemplateClass<DurationClass> *time = Unknown_Find_Set_Field<DeltaTemplateClass<DurationClass> >((DeltaCompoundClass *)target->AuxData, "InterruptibleActivationTime");
		if (time) {
			InterruptInfo->Unknown2C->Unknown_60ECA0(time->Value);
		}
		RenderObjClass *robj = RObj;


















		G_ASSERT(robj);
		DeltaStringClass *name = Unknown_Find_Set_Field<DeltaStringClass>((DeltaCompoundClass *)object->AuxData, "InterruptibleAbilityName");
		if (name) {
			InterruptInfo->AbilityName = name->String;
		} else {
			InterruptInfo->AbilityName = "Unknown Skill";
		}
		if (!InterruptInfo->Unknown14) {
			InterruptInfo->Unknown14 = Create_Get(new FontSys("MFD_INTERRUPT", (int)robj, NULL));
			InterruptInfo->Unknown14->Set_Sort_Level(8);
		}
		InterruptInfo->Unknown14->Change_Text(InterruptInfo->AbilityName.c_str(), 0);
		InterruptInfo->Unknown14->Set_Hidden(0);
		FactionFont->Set_Hidden(1);
	}
	Unknown_84();
}

// @0x60EC70
void Unknown59E2C0Class::Unknown_60EC70(DurationClass time)
{
	Unknown28 = time;
}

// @0x60ECA0
void Unknown59E2C0Class::Unknown_60ECA0(DurationClass time)
{
	Unknown30 = time;
}
#pragma auto_inline(on)
















// @0x60ED10
void UIScanClass::Unknown_60ED10(PlayerObjectRelationshipChangeEvent *event, ClientObjectClass *object)
{
	FAIL_IF(object != Peek_Object()) return;
	Unknown_60EFE0(object);
}












#pragma auto_inline(off)
// @0x60ED60
void UIScanClass::Unknown_60ED60(ClientObjectClass *target, const std::string &assessment, const std::string &sound, int level)
{
	if (!target) {
		ConFont->Set_Hidden(1);
		return;
	}
	if (target->AuxData && !strcmp(((DeltaClass *)target->AuxData)->FullName.Get_String(), "Earth")) {
		ConFont->Set_Hidden(1);
		return;
	}
	if (assessment.empty()) {
		ConFont->Set_Hidden(1);
		ConFont->Unknown_6CE7D0(1);
		return;
	}
	ConFont->Unknown_6CE7D0(2);
	ConFont->Set_Hidden(0);
	std::string text = assessment;
	if (level >= 0) {
		char buf[64];
		_snprintf(buf, 64, "Combat Level %d", level);
		text = buf;
	}
	ConFont->Change_Text(text.c_str(), 0);
	if (!sound.empty()) {
		SoundLibraryClass::Play_2D_Sound(sound.c_str(), 2);
	}
}


// @0x60EFE0
void UIScanClass::Unknown_60EFE0(ClientObjectClass *object)
{
	if (!object) {
		ConFont->Set_Hidden(1);
		ConFont->Unknown_6CE7D0(1);
		return;
	}

	LineDisplayClientClass *client = Client;


	FAIL_IF(!client) return;
	SClient *sclient = client->As_Space_Client();

	FAIL_IF(!sclient) return;
	GutterColorSetStruct colors = Unknown_70B0A0(sclient, object);
	GutterColorStruct color = colors.Target;
	ConFont->Set_Color(color.Color1);
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




// NOT EXACT: inline-budget (TL1; similarity 0.65): the client calls the Reputation find out of line (0x610370; an explicit instantiation here does not move it), tail-merges only cases 2 and 5
// @0x60F1A0
void UIScanClass::Unknown_60F1A0(bool arg)
{
	if (arg) {
		FactionFont->Set_Hidden(1);
	}
	ClientObjectClass *object = Object;
	if (!object || !object->AuxData) return;
	bool expired = false;
	DeltaTemplateClass<InterruptStateType> *state = Unknown_Find_Set_Field<DeltaTemplateClass<InterruptStateType> >((DeltaCompoundClass *)object->AuxData, "InterruptState");
	if (state) {
		switch (state->Value) {
			case 0:
				break;
			case 1:
				if (InterruptInfo->Unknown28 != 1) {
					InterruptInfo->Unknown28 = 1;
					Unknown_60E6A0(object);
				}
				break;
			case 2:
				if (InterruptInfo->Unknown28 == 1) {
					InterruptInfo->Unknown28 = 2;
					InterruptInfo->Unknown14->Change_Text("Interrupted!", 0);
					InterruptInfo->Unknown18 = true;
					InterruptInfo->Unknown20 = DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC);
				}
				break;
			case 3:
				if (InterruptInfo->Unknown28 == 1) {
					InterruptInfo->Unknown28 = 3;
					InterruptInfo->Unknown14->Change_Text("Interruption Failed!", 0);
					InterruptInfo->Unknown18 = true;
					InterruptInfo->Unknown20 = DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC);
				}
				break;
			case 4:
				if (InterruptInfo->Unknown28 == 1) {
					InterruptInfo->Unknown28 = 4;
					InterruptInfo->Unknown14->Change_Text("Skill Activated!", 0);
					InterruptInfo->Unknown18 = true;
					InterruptInfo->Unknown20 = DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC);
				}
				break;
			case 5:
				if (InterruptInfo->Unknown28 == 1) {
					InterruptInfo->Unknown28 = 5;
					InterruptInfo->Unknown14->Change_Text("Skill Deactivated", 0);
					InterruptInfo->Unknown18 = true;
					InterruptInfo->Unknown20 = DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC);
				}
				break;
			default:







				G_ASSERT(false);
		}
	}

	if (InterruptInfo->Unknown2C) {
		if (InterruptInfo->Unknown18 && DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC) - InterruptInfo->Unknown20 >= UIScanInterruptDelay) {
			InterruptInfo->Unknown18 = false;
			InterruptInfo->Unknown_Refresh();
			expired = true;
		}
		if (InterruptInfo->Unknown2C) {
			DeltaTemplateClass<float> *progress = Unknown_Find_Set_Field<DeltaTemplateClass<float> >((DeltaCompoundClass *)object->AuxData, "InterruptProgress");
			InterruptInfo->Unknown2C->Unknown40 = progress ? progress->Value : 0.0f;
			InterruptInfo->Unknown_Update();



			return;
		}
	}

	DeltaStringClass *faction = Unknown_Find_Set_Field<DeltaStringClass>((DeltaCompoundClass *)object->AuxData, "FactionIdentifier");
	if (faction) {



		FAIL_IF(!Get_Client()) return;
		SClient *sclient = Get_Client()->As_Space_Client();
		FAIL_IF(!sclient) return;

		DeltaPlayerClass *aux_data = sclient->AuxData;
		FAIL_IF(!aux_data) return;

		DeltaReputationClass *reputation = (!arg && !expired) ?
			Unknown_Find_Changed_Field<DeltaReputationClass>((DeltaCompoundClass *)aux_data, "Reputation") :
			Unknown_Find_Set_Field<DeltaReputationClass>((DeltaCompoundClass *)aux_data, "Reputation");


		if (reputation) {
			FactionFont->Set_Hidden(0);
			FactionFont->Change_Text("", 0);
			int count = reputation->Faction.Children.size();
			const std::string &name = CFactionDatabaseClass::Get_Name(std::string(faction->String));
			FactionFont->Set_Color(Vector3(0.7f, 0.7f, 0.7f));
			for (int i = 0; i < count; i++) {
				DeltaFactionClass *entry = (DeltaFactionClass *)reputation->Faction.Children[i];
				if (entry->Name.String && strcmp(entry->Name.String, faction->String) == 0) {
					int reaction = (int)entry->Reaction.Value;
					if (reaction >= 2000) {
						FactionFont->Set_Color(Vector3(0.1f, 1.0f, 0.1f));
					} else if (reaction >= -2000) {
						FactionFont->Set_Color(Vector3(1.0f, 1.0f, 0.1f));
					} else {
						FactionFont->Set_Color(Vector3(1.0f, 0.1f, 0.1f));
					}
					break;
				}
			}
			FactionFont->Change_Text(name.c_str(), 0);
		}
	}

	DeltaTemplateClass<int> *tech = (!arg && !expired) ?
		Unknown_Find_Changed_Field<DeltaTemplateClass<int> >((DeltaCompoundClass *)object->AuxData, "TechLevel") :
		Unknown_Find_Set_Field<DeltaTemplateClass<int> >((DeltaCompoundClass *)object->AuxData, "TechLevel");


	if (tech && tech->Value) {
		char text[128];
		sprintf(text, "Tech Level: %d", tech->Value);
		FactionFont->Change_Text(text, 0);
		FactionFont->Set_Color(Vector3(0.7f, 0.7f, 0.7f));
		FactionFont->Set_Hidden(0);
	}
}










































































































































































// @0x60FEE0
void UIScanThreatClass::Unknown_60FEE0()
{
	G_ASSERT(Get_Interface()->Is_Partially_Deactivated() || !Get_Interface()->Is_Active());
	Unknown_Clear();
	if (Unknown14) {
		Unknown14->Change_Text("", 0);
		Unknown14->Set_Hidden(1);
	}
	if (Unknown30) {
		Unknown30->Remove();
		Unknown30 = NULL;
	}
}
#pragma auto_inline(on)

/*
**	Notes (WE9, 2026-11-07)
**
**	The client's object runs 0x60D750-~0x610560 (__FILE__ 0xB8899C). Rebuilt for the POC's scanner (the radar panel and
**	the target box at the lower right): the constructor, the slots and 0x60E540; the threat object's 0x60ED60 / 0x60FEE0
**	and 0x60E6A0 are not rebuilt yet (TL1 added 0x60ED10, 0x60ED60). Blank lines keep the asserts on their client lines.
**
**	TL1 (2026-11-08): 0x60EFE0 (the con line's colour: exact only with the gutter set named and its Target copied) and
**	0x60F1A0 (the threat's interrupt message, the faction and tech-level lines; functional) are rebuilt between
**	'#pragma auto_inline(off)' / '(on)' (on blank lines): without it /Ob2 inlines both into 0x60E580 (0.85 -> 0.36).
**	The threat's +0x14 / +0x30 are RefCountPtr members (the destructor's 0x51C630 / 0x4740B0 take [edi+0x14] /
**	[edi+0x30]), its +0x20 the DurationClass the interrupt message started at; 0xBE5E10 is a file static DurationClass
**	(its initialiser 0x60D7D0 matches).
**
**	TL2 (2026-11-08): 0x60E6A0 (the interrupt display; functional) and the progress display's two time setters 0x60EC70 /
**	0x60ECA0 (defined after it under auto_inline(off), as the client's addresses place them). +0x1A0 is InterruptInfo
**	(0x60E6A0's G_ASSERT l.571). The progress display (Unknown59E2C0Class) needs '#pragma pack(push, 4)': its times at
**	+0x28 / +0x30 would otherwise pad the vtable pointer (0x50 bytes, the client's new asks 0x48). Its by-value time
**	arguments are built in place (sub esp, 8; mov [esp+N], esp): a user-defined DurationClass copy constructor gives
**	that form (0.43 -> 0.46) but costs aux_roots 8 pairs and slot_data_render 0x6A7C00, so duration.h keeps the implicit one.
**	0x60ECD0 (lea eax, [ecx+0x88]) is the time field's value accessor, out of line in 0x60E6A0; declaring it in
**	delta_template.h costs aux_roots 8 pairs (its inline budget), so 0x60E6A0 reads Value directly.
**
**	TL3 (2026-11-08): 0x60FEE0 (the threat's clear-and-hide; exact) after 0x60F1A0 under the same auto_inline(off), its
**	G_ASSERT on l.1055; its body after the assert is TL2's inline Unknown_Clear, then +0x14's Change_Text('') / Set_Hidden(1)
**	and +0x30's Remove() and release. Get_Interface() (the assert's) returns Owner.
*/
