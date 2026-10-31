// Earth & Beyond client (reconstruction): app\Client\ui_cpit.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_cpit.h"
#include "client.h"
#include "..\packets\pkaction.h"
#include "..\packets\pkskillstringrq.h"
#include "..\packets\pkturn.h"
#include "..\packets\pkwarpindex.h"
#include "..\packets\pkincapacitancerequest.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "option.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h: its Type_Name(int *) for kv_pair.h, C2665 without)
#include "..\shared\unknown_debug.h"
#include "control_keys_manager.h"

// fontdatabase.h's FontStringFilesClass at 0xBE67BC and fontmanager.h's FontManagerClass at 0xBE67C4, as far as this
// file calls them (fontdatabase.h's 0xBE67B8 clashes with tsk_game.h's, so neither header can be included here)
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's
class FontStyleClass : public RenderObjClass	// NAME 1 [FontStyleClass]: fontmanager.h's (a DynamicMeshClass there, 0x4E8 bytes)
{
public:
	char Unknown_Pad[0x2FC - sizeof(RenderObjClass)];
	float Unknown2FC;									// +0x2FC (0x5845A0: 1, or 0.5 while the warp button is off)
	char Unknown300[0x4C5 - 0x300];
	bool Unknown4C5;									// +0x4C5 (0x5845A0 clears it)
};
class FontManagerClass { public: void Add(RenderObjClass *object); void Remove(unsigned int id); FontStyleClass *Get_Style(const char *name, unsigned int id); };	// NAME 3: fontmanager.h's
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

// gadget.cpp's gadget, as far as the cockpit calls it (the warp, throttle and turn buttons)
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual bool Unknown_60();							// +0x60 (0x5845A0: the warp button is on)
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C(); virtual void Unknown_90();
	virtual void Unknown_94(); virtual void Unknown_98(); virtual void Unknown_9C(); virtual void Unknown_A0();
	virtual void Unknown_A4(); virtual void Unknown_A8();
	virtual void Unknown_AC();							// +0xAC (0x583250, last)

	char Unknown04[0x64 - 0x04];						// +0x04
	bool Unknown64;										// +0x64: pressed (0x583250, 0x583510)
	bool Unknown65;										// +0x65
	bool Unknown66;										// +0x66 (0x583510: either turn button's set stops the turn)
	void Unknown_671740(bool on, float value);			// 0x671740 (0x583510: 1, 1.0f; 0, 0)  // NAME 1 [on, value]: neutral
	void Unknown_671630();								// 0x671630 (after slot +0xAC)
	void Unknown_674160(int arg);						// 0x674160 (0x583250 passes 0 to the throttle and reverse buttons)  // NAME 1 [arg]: neutral
};

// camera_manager.cpp's manager (SClient +0x12A8), as far as the warp button calls it
class CameraManagerClass	// NAME 3: class-from-file (D24) for camera_manager.cpp
{
public:
	void Unknown_79B2B0(int arg);						// 0x79B2B0 (0x583250 passes 17 in the 3-D movie view)  // NAME 1 [arg]: neutral
};

// (inline: the client reads these before the other arguments of the packets' constructors)
inline unsigned Unknown_Avatar_ID(LineDisplayClientClass *client) { return(((ClientClass *)client)->AvatarID); }	// NAME 1: neutral (D10)
inline int Unknown_Player_ID(LineDisplayClientClass *client) { return(((ClientClass *)client)->Unknown1138); }	// NAME 1: neutral (D10)

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
// NOT EXACT: inline budget (the client calls the LockSpeed Cast out of line, 0x584FF0, and inlines the LockOrient one; ours inlines both)
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

// 0x57F240
// NOT EXACT: inline budget (the client expands std::string's assign in the last nine keys, ours in the last eight)
void UICpitClass::Unknown_70(ControlKeysManagerClass *manager)
{



















































































































































	FAIL_IF(!manager) return;
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Stop"), (void *)13, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Backward"), (void *)9, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Backward"), (void *)13, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward"), (void *)8, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward"), (void *)13, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward Toggle"), (void *)10, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward Lock"), (void *)11, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Backward Lock"), (void *)12, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Down"), (void *)20, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Down"), (void *)21, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Up"), (void *)19, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Up"), (void *)21, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Left"), (void *)16, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Left"), (void *)18, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Right"), (void *)17, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Right"), (void *)18, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Warp"), (void *)2, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Warp"), (void *)3, 0x1000);
}
// 0x57FCC0
// NOT EXACT: inline budget (the client expands std::string's assign in other keys than ours)
void UICpitClass::Unknown_74(ControlKeysManagerClass *manager)
{
	FAIL_IF(!manager) return;
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Stop"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Backward"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Backward"), 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward"), 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward Toggle"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Forward Lock"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Move Backward Lock"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Down"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Down"), 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Up"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Tilt Up"), 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Left"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Left"), 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Right"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Turn Right"), 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Warp"), 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Warp"), 0x1000);
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
		((ClientClass *)Client)->Send_Packet(&ActionPacketClass(Unknown_Avatar_ID(Client), 17, current_harvestable->Get_Game_ID(), 0));
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






































































































































































































































































































































































// 0x583250: the warp button (+0xB4)
void UICpitClass::Unknown_583250()
{
	if (Unknown0B4->Unknown_60()) {
		if (!Client) return;
		if (Unknown0B4->Unknown64) {
			SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
			SClient *sclient = (SClient *)Client;
			if (Unknown13D) {
				if (!Unknown13F && !Unknown0E4) {
					((ClientClass *)Client)->Send_Packet(&IncapacitanceRequestPacketClass(Unknown_Avatar_ID(Client)));
				}
			} else if (sclient->ViewMode == 0) {
				RadarSystemClass *radar = sclient->MainView->Radar;

				FAIL_IF(!radar) return;
				if (radar->Unknown_7BD980()) {
					((ClientClass *)Client)->Send_Packet(&WarpPacketClass(Unknown_Player_ID(Client), radar->Unknown_7BD960()));
					sclient->Unknown_745C70(0);
					sclient->Unknown_745B90(0);
					sclient->Unknown_745E40(0, false);
					Unknown144 = 0;
					Unknown0B8->Unknown_674160(0);
					Unknown0B8->Unknown_671740(false, 0);
					Unknown0BC->Unknown_674160(0);
				} else {
					sclient->Unknown_74D990(17, "Warp path has not finished building!  Press warp again.", 0);
				}
				Unknown0BC->Unknown_671740(false, 0);
			} else if (sclient->ViewMode == 1) {
				((SClient *)sclient->Unknown_18())->CameraManager->Unknown_79B2B0(17);
			}
		}
	}
	Unknown0B4->Unknown_AC();
	Unknown0B4->Unknown_671630();
}































































// 0x583510: the turn buttons (LEFT +0xC0, RIGHT +0xC4)
void UICpitClass::Unknown_583510()
{
	if (!Unknown0C0->Unknown_60() && !Unknown0C4->Unknown_60()) return;
	SClient *sclient = (SClient *)Client;
	if (!sclient) return;
	if (Unknown0C0->Unknown66 || Unknown0C4->Unknown66) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
		Unknown134 = 0;
		((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), 0.0f));
		sclient->Unknown_745B90(0.0f);
		Unknown0C0->Unknown_671740(false, 0.0f);
		Unknown0C4->Unknown_671740(false, 0.0f);
	} else if (Unknown0C0->Unknown64) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
		Unknown134 = -1;
		((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), -1.0f));
		sclient->Unknown_745B90(-1.0f);
		Unknown0C0->Unknown_671740(true, 1.0f);
	} else if (Unknown0C4->Unknown64) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
		Unknown134 = 1;
		((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), 1.0f));
		sclient->Unknown_745B90(1.0f);
		Unknown0C4->Unknown_671740(true, 1.0f);
	}
	Unknown0C0->Unknown_AC();
	Unknown0C4->Unknown_AC();
	Unknown0C0->Unknown_671630();
	Unknown0C4->Unknown_671630();
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

// 0x584000: whether the avatar's engine runs (its MaxSpeed is set), else the message when report is set
// NOT EXACT: order (the client keeps an epilogue at each early return and the found path inline; ours shares one exit)
bool UICpitClass::Unknown_584000(bool report)
{
	SClient *sclient = (SClient *)Client;
	ClientObjectClass *avatar = sclient->Get_Avatar();
	if (!avatar) return(false);
	if (!avatar->AuxData) return(false);
	NameClass name("MaxSpeed");
	DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >((DeltaCompoundClass *)avatar->AuxData, name);
	if (t && t->Unknown70 && t->Value != 0.0f) return(true);
	if (report) sclient->Unknown_74D990(17, "Engine unavailable. Please wait.", 0);
	return(false);
}

// 0x584110: the WARP_CPIT / DISTRESS_CPIT / SPACE_CPIT texts shown by the state
void UICpitClass::Unknown_584110()
{
	RefCountPtr<FontStyleClass> warpfont = Create_Get(Unknown_BE67C4->Get_Style("WARP_CPIT", (unsigned int)Mesh->Peek_Object()));
	RefCountPtr<FontStyleClass> distressfont = Create_Get(Unknown_BE67C4->Get_Style("DISTRESS_CPIT", (unsigned int)Mesh->Peek_Object()));
	RefCountPtr<FontStyleClass> spacefont = Create_Get(Unknown_BE67C4->Get_Style("SPACE_CPIT", (unsigned int)Mesh->Peek_Object()));

















































	FAIL_IF(!warpfont) return;
	FAIL_IF(!distressfont) return;
	FAIL_IF(!spacefont) return;
	if (Unknown13D) {
		warpfont->Set_Hidden(true);
		distressfont->Set_Hidden(false);
		spacefont->Set_Hidden(true);
	} else if (((SClient *)((ClientClass *)Client)->Unknown_18())->ViewMode == 1) {
		warpfont->Set_Hidden(false);
		distressfont->Set_Hidden(true);
		spacefont->Set_Hidden(true);
	} else {
		warpfont->Set_Hidden(false);
		distressfont->Set_Hidden(true);
		spacefont->Set_Hidden(true);
	}
}

// 0x5845A0: the WARP_CPIT text dimmed while the warp button is off
void UICpitClass::Unknown_5845A0()
{
	RefCountPtr<FontStyleClass> style = Create_Get(Unknown_BE67C4->Get_Style("WARP_CPIT", (unsigned int)Mesh->Peek_Object()));
	if (style) {
		FontStyleClass *font;
		if (Unknown0B4->Unknown_60()) {
			(font = style.Peek())->Unknown2FC = 1.0f;
		} else {
			(font = style.Peek())->Unknown2FC = 0.5f;
		}
		font->Unknown4C5 = false;
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
