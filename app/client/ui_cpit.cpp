// Earth & Beyond client (reconstruction): app\Client\ui_cpit.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_cpit.h"
#include "client.h"
#include "..\packets\pkaction.h"
#include "..\packets\pkskillstringrq.h"
#include "rendobj.h"
#include "option.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h: its Type_Name(int *) for kv_pair.h, C2665 without)
#include "..\shared\unknown_debug.h"

// fontdatabase.h's FontStringFilesClass at 0xBE67BC and fontmanager.h's FontManagerClass at 0xBE67C4, as far as this
// file calls them (fontdatabase.h's 0xBE67B8 clashes with tsk_game.h's, so neither header can be included here)
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's
class FontManagerClass { public: void Add(RenderObjClass *object); void Remove(unsigned int id); };	// NAME 3: fontmanager.h's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The object at *0xBFAEE0 (kernel.cpp's), as far as the constructor calls it: its slot +0x14 makes a render object by name
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10();
	virtual RenderObjClass *Create_Render_Obj(const char *name);	// +0x14  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0
void Unknown_496730(RenderObjClass *robj, int a, int b);	// 0x496730 (ro_funcs.cpp, cdecl; the constructor passes the gutter, 0x20, 0)  // NAME 1 [robj, a, b]: what the constructor passes
void Unknown_65EED0(GadgetClass *gadget);				// 0x65EED0 (cdecl, gadget.cpp): the gadget deleted  // NAME 1 [gadget]: neutral

// local_interface_sort.cpp's sort object (8 bytes; view.cpp, mainview.cpp and ui_line_display.cpp declare it too)
class LineDisplaySortClass  // NAME 1: neutral (D10)
{
public:
	LineDisplaySortClass(InterfaceClass *inter);		// 0x541980  // NAME 1 [inter]: view.cpp's
	LineDisplaySortClass(UIViewClass *view, int arg);	// 0x6E1560  // NAME 1 [view, arg]: mainview.cpp's
	~LineDisplaySortClass();							// 0x6E18B0

private:
	char Unknown00[8];
};














































































// 0x57D6C0 (also the view's 0x6B58A0): the avatar's LockSpeed / LockOrient, where it has them
void Unknown_57D6C0(SClient *sclient, bool *lock_speed, bool *lock_orient)
{
	if (!sclient) {
		// (no client, no avatar to ask)







		G_ASSERT(sclient);
		return;
	}
	ClientObjectClass *avatar = sclient->Get_Avatar();
	if (avatar && avatar->AuxData) {
		{
			NameClass name("LockSpeed");
			DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)avatar->AuxData, name);
			if (t && t->Unknown70) *lock_speed = t->Value;
		}
		{
			NameClass name("LockOrient");
			DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)avatar->AuxData, name);
			if (t && t->Unknown70) *lock_orient = t->Value;
		}
	}
}

// 0x57D830
UICpitClass::UICpitClass(MainViewClass *view, const LayerClass &layer) :
	Unknown5BB320Class(new UIMeshClass("TSI01A", true), view, layer),
	Unknown0AC(NULL),
	Unknown0B4(NULL),
	Unknown0B8(NULL),
	Unknown0BC(NULL),
	Unknown0C0(NULL),
	Unknown0C4(NULL),
	HulkUI(NULL),
	Unknown0CC(NULL),
	Unknown0D0(NULL),
	Scan(NULL),
	Unknown0D8(0),
	UnknownDC(NULL),
	Unknown0E4(false),
	Unknown0E8(0),
	HulkWatch(this),
	CurrentHarvestable(this),
	Unknown12C(0),
	Unknown130(0xFFFFFF),
	Unknown134(0),
	Unknown138(0),
	Unknown13C(true),
	Unknown13D(false),
	Unknown13E(false),
	Unknown13F(false),
	Unknown140(false),
	Unknown144(0),
	Unknown148(0),
	Unknown14C(0),
	Unknown150(false),
	Unknown154(NULL),
	Unknown158(0)
{
	Unknown0B0 = Create_NEW(Unknown_BFAEE0->Create_Render_Obj("tsi01_gutter"));
	Unknown_496730(Unknown0B0.Peek(), 0x20, 0);
	Mesh->Unknown_5BA460(1);
	((FontStringFilesClass *)Unknown_BE67BC)->Load("fc_cpit.ini");
	Unknown_BE67C4->Add(Mesh->Peek_Object());
}

















































































































































































































// 0x57EB80
UICpitClass::~UICpitClass()
{
	REF_PTR_RELEASE(HulkUI);
	REF_PTR_RELEASE(Unknown0CC);
	REF_PTR_RELEASE(Scan);
	REF_PTR_RELEASE(UnknownDC);
	Unknown0E0 = NULL;
	for (std::list<RenderObjClass *>::iterator it = Unknown158.begin(); it != Unknown158.end(); it++) {
		(*it)->Remove();
		(*it)->Release_Ref();
	}
	Unknown158.clear();
	Unknown154 = NULL;
	Unknown_BE67C4->Remove((unsigned int)Mesh->Peek_Object());
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fc_cpit.ini");
	Unknown_5C();
	delete Unknown0D0;
	Unknown0D0 = NULL;
	Unknown_65EED0(Unknown0AC);
	Unknown0AC = NULL;
	BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Cpit_ThrottleClickLocks");
	if (option) option->Set_Value(Unknown13C);
	OptionClass::Instance->Save();
	Unknown16C = NULL;
	Unknown170 = NULL;
	Unknown174 = NULL;
}

// 0x57F010
void UICpitClass::Unknown_7C(UIViewClass *view, int arg)
{
	Unknown5BB320Class::Unknown_7C(view, arg);
	HulkUI->Unknown_7C(view, 0);
	Unknown0CC->Unknown_7C(view, 0);
	UnknownDC->Unknown_7C(view, arg);
	Scan->Unknown_7C(view, arg);
}

// 0x57F080
void UICpitClass::Unknown_58()
{
	if (Unknown148 == 0) {
		LineDisplaySortClass sort(this);
		Unknown148 = 1;
		ClientObjectClass *avatar = ((ClientClass *)Client)->Get_Avatar();
		if (avatar) Unknown_582190(avatar, 0);
		UnknownDC->Unknown_58();
		Scan->Unknown_58();
		Unknown_584110();
		Unknown5BB320Class::Unknown_58();
		LineDisplaySortClass view_sort(View, 0);
		Unknown0B0->Add(Layer.Peek_Scene());
	}
}

// 0x57F1B0
void UICpitClass::Unknown_5C()
{
	if (Unknown148 == 1) {
		Unknown0B0->Remove();
		Mesh->Unknown_5B9830();
		Mesh->Unknown_5B9930(-1);
		HulkUI->Unknown_5C();
		Unknown0CC->Unknown_5C();
		UnknownDC->Unknown_5C();
		Scan->Unknown_5C();
		Unknown5BB320Class::Unknown_5C();
		Unknown148 = 0;
	}
}












































































































































































































































































































































































































































































































































































































































































































// 0x581CE0: a window of the cockpit's closed
void UICpitClass::Unknown_581CE0(InterfaceClass *ui)
{
	if (ui == HulkUI) {
		Unknown_582070();
	} else if (ui == Unknown0CC) {
		ClientObjectClass *current_harvestable = CurrentHarvestable;
		// (the harvest stopped: the server is told)






		FAIL_IF(!current_harvestable) return;
		((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 17, current_harvestable->Get_Game_ID(), 0));
		CurrentHarvestable = NULL;
	} else {
		//



		G_ASSERT(false);
	}
}

// 0x581E60
void UICpitClass::Unknown_581E60()
{
	if (HulkWatch) { Unknown_582070(); return; }
	ClientObjectClass *current_target = Scan->Unknown88;


	FAIL_IF(!current_target) return;
	((ClientClass *)Client)->Send_Packet(&SkillStringRqPacketClass(((ClientClass *)Client)->Unknown1138, 0));
}
// 0x581F90
void UICpitClass::Unknown_581F90(ClientObjectClass *hulk)
{
	FAIL_IF(!hulk) return;
	ClientObjectClass *current_target = Scan->Unknown88;

	FAIL_IF(!current_target) return;
	if (hulk == current_target) {
		HulkWatch = current_target;
		//







		if (HulkUI) HulkUI->Unknown_58(); else G_ASSERT(HulkUI);
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_744560(11);
	}
}

// 0x582070
void UICpitClass::Unknown_582070()
{
	if (HulkWatch) {
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_744560(12);
		HulkUI->Unknown_5C();
		HulkWatch = NULL;
	}
}

// 0x5820C0
void UICpitClass::Unknown_5820C0(bool on)
{
	if (on) {
		G_ASSERT(CurrentHarvestable == 0);
		CurrentHarvestable = Scan->Unknown88;
		G_ASSERT(CurrentHarvestable != 0);
		Unknown0CC->Unknown_58();
	} else {
		CurrentHarvestable = NULL;
		Unknown0CC->Unknown_5C();
	}
}

// 0x582190
void UICpitClass::Unknown_582190(ClientObjectClass *object, int arg)
{
	if (Unknown0D0->Object != object) *Unknown0D0 = object;
}
































































































































































































































































































































































































































































































































































































































// 0x583DA0
void UICpitClass::Unknown_583DA0(int arg)
{
}














































// 0x583F70: the effects that have finished, removed
void UICpitClass::Unknown_583F70()
{
	std::list<RenderObjClass *>::iterator it = Unknown158.begin();
	while (it != Unknown158.end()) {
		RenderObjClass *effect = *it;
		if (effect->Is_Complete()) {
			effect->Remove();
			effect->Release_Ref();
			it = Unknown158.erase(it);
		} else {
			it++;
		}
	}
}







































































































































// 0x584A20
void UICpitClass::Unknown_584A20()
{
	if (Unknown0E0 && Unknown0E0->Is_Active()) {
		Unknown0E0->Unknown_5C();
		Unknown0E4 = false;
	}
}

// 0x584A60
void UICpitClass::Unknown_584A60()
{
	Unknown144 = 0;
}

// 0x584A80
UICpitHulkWatchClass::UICpitHulkWatchClass(UICpitClass *owner) : Owner(owner)
{
}

// 0x584AF0
void UICpitHulkWatchClass::Unknown_0C()
{
	ClientObjectClass *co = Object;








	FAIL_IF(!co) return;
	Owner->Unknown_582070();
}

// 0x584B70
UICpitHarvestWatchClass::UICpitHarvestWatchClass(UICpitClass *owner) : Owner(owner)
{
}

// 0x584BE0
void UICpitHarvestWatchClass::Unknown_0C()
{
	ClientObjectClass *co = Object;


	FAIL_IF(!co) return;
	Owner->Unknown_5820C0(false);
}











/*
** Reconstruction notes (CP1, 2026-10-31)
**
** The client's e:\build\beta\G\app\Client\ui_cpit.cpp: the cockpit interface UICpitClass (0x57D830-0x584BE1) and
** its two object watchers. Each function is placed by address; the asserts sit on the client's lines.
*/
