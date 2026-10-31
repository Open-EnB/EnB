// Earth & Beyond client (reconstruction): app\Client\ui_cpit.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_cpit.h"
#include "client.h"
#include "..\packets\pkaction.h"
#include "..\packets\pkskillstringrq.h"
#include "..\packets\pkturn.h"
#include "..\packets\pktilt.h"
#include "..\packets\pkmove.h"
#include "..\packets\pkwarpindex.h"
#include "..\packets\pkincapacitancerequest.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "matinfo.h"
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
	void Change_Text(const char *text, int unknown);	// 0x6CDBC0 (fontmanager.h's)  // NAME 3: the client's label
	void Unknown_6CE910(int on);						// 0x6CE910 (0x5821C0: 1 past the speed limit)  // NAME 1 [on]: neutral
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
void Unknown_65EFB0(GadgetClass *gadget, const char *section, const char *name);	// 0x65EFB0 (cdecl, gadget.cpp): the button's text from a string section  // NAME 1 [section, name]: ui_group.cpp's

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

// gadget.cpp's gadget, as far as the cockpit calls it (the warp, throttle and turn buttons)
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24();
	virtual void Unknown_28(LayerClass *layer);			// +0x28 (the set-up: the cockpit's layer)  // NAME 1 [layer]: ui_group.cpp's
	virtual void Unknown_2C(KernelEventClass *event, int a, int b, int c);	// +0x2C (slot +0x68: a zero event, 0, 0, 1 or 4 for the warp keys)  // NAME 1 [event, a, b, c]: neutral
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(int on);					// +0x40 (slot +0x60: 1 shows the warp button as RESCUE)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual bool Unknown_60();							// +0x60 (0x5845A0: the warp button is on)
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C(); virtual void Unknown_90();
	virtual void Unknown_94(); virtual void Unknown_98(); virtual void Unknown_9C(); virtual void Unknown_A0();
	virtual void Unknown_A4(float value);				// +0xA4 (slot +0x60: 2.0f when incapacitated, else 0)  // NAME 1 [value]: neutral
	virtual void Unknown_A8();
	virtual void Unknown_AC();							// +0xAC (0x583250, last)

	void Unknown_660070(int arg);						// 0x660070 (the set-up passes 1)  // NAME 1 [arg]: neutral
	void Unknown_65FFA0(int arg);						// 0x65FFA0 (the set-up passes 1000)  // NAME 1 [arg]: neutral
	void Unknown_65FFC0(int arg, GadgetClass *gadget);	// 0x65FFC0 (the set-up passes -1 and +0xAC)  // NAME 1 [arg, gadget]: neutral

	char Unknown04[0x49 - 0x04];						// +0x04 (0x50 bytes: 0x50A850's gadget)
	bool Unknown49;										// +0x49 (0x5821C0: +0xAC's keeps the throttle light on)
	char Unknown4A[0x50 - 0x4A];						// +0x4A
};

// gadget.cpp's button under the cockpit's five (0x80 bytes and more), as far as the cockpit calls it
class UICpitButtonClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_671740(bool on, float value);			// 0x671740 (0x583510: 1, 1.0f; 0, 0)  // NAME 1 [on, value]: neutral
	void Unknown_671630();								// 0x671630 (after slot +0xAC)
	void Unknown_674120(int arg);						// 0x674120 (0x5821C0 passes 1 past the speed limit)  // NAME 1 [arg]: neutral
	void Unknown_674160(int arg);						// 0x674160 (0x583250 passes 0 to the throttle and reverse buttons)  // NAME 1 [arg]: neutral
	void Unknown_676190(int a, int b);					// 0x676190 (slot +0x60: 5, 0 when incapacitated, else 0, 0)  // NAME 1 [a, b]: neutral
	void Unknown_66C360();								// 0x66C360 (slot +0x68: after slot +0xA4 on the throttle buttons)
	bool Unknown_6740E0(float value);					// 0x6740E0 (0x583790 passes 1.0f: the auto thrust engaged)  // NAME 1 [value]: neutral

	char Unknown50[0x64 - 0x50];						// +0x50
	bool Unknown64;										// +0x64: pressed (0x583250, 0x583510)
	bool Unknown65;										// +0x65
	bool Unknown66;										// +0x66 (0x583510: either turn button's set stops the turn)
	char Unknown67[0x80 - 0x67];						// +0x67
};
// The five (gadget.cpp's constructors, made by the set-up 0x57DD20)
class UICpitTurnGadgetClass : public UICpitButtonClass { public: UICpitTurnGadgetClass(RenderObjClass *object, RenderObjClass *uv, LayerClass *layer, int a, int b, int c); };	// 0x671300 (0x80 bytes; 'LEFT', 'RIGHT')  // NAME 1: neutral (D10); NAME 1 [object, uv, layer, a, b, c]: what the set-up passes (-1, 0, 3)
class UICpitThrottleGadgetClass : public UICpitButtonClass { public: UICpitThrottleGadgetClass(RenderObjClass *object, RenderObjClass *uv, LayerClass *layer, int a, int b); int Unknown80; char Unknown84[0x88 - 0x84]; };	// 0x673EE0 (0x88 bytes; 'THROTTLE', 'REVERSE'; +0x80: 2 stops the throttle, 0x583790)  // NAME 1: neutral (D10); NAME 1 [object, uv, layer, a, b]: what the set-up passes (-1, 0)
class UICpitWarpGadgetClass : public UICpitButtonClass { public: UICpitWarpGadgetClass(RenderObjClass *object, RenderObjClass *bar, LayerClass *layer); char Unknown80[0x94 - 0x80]; };	// 0x676000 (0x94 bytes; 'WARP', 'WARP_BAR')  // NAME 1: neutral (D10); NAME 1 [object, bar, layer]: what the set-up passes

// gadget.cpp's and view.cpp's gadgets the set-up makes, as far as it makes them
class UICpitGadget6B56AClass : public GadgetClass { public: UICpitGadget6B56AClass(int x, int y, int width, int height, UIViewClass *view); char Unknown50[0x5C - 0x50]; };	// 0x6B56A0 (0x5C bytes; +0xAC)  // NAME 1: neutral (D10); NAME 1 [x, y, width, height, view]: what the set-up passes (0, 0, 640, 480)
class UICpitGadget50A850Class : public GadgetClass { public: UICpitGadget50A850Class(int x, int y, int width, int height, LineDisplayClientClass *client); };	// 0x50A850 (0x50 bytes)  // NAME 1: neutral (D10); NAME 1 [x, y, width, height, client]: what the set-up passes
class UICpitGadget674260Class : public GadgetClass { public: UICpitGadget674260Class(const RefCountPtr<RenderObjClass> &model); char Unknown50[0x60 - 0x50]; };	// 0x674260 (0x60 bytes)  // NAME 1: neutral (D10); NAME 1 [model]: what the set-up passes
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral

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














// 0x57DD20: the cockpit's gutter opacity, gadgets, windows and lights
// NOT EXACT: inline budget (the client calls the name's _Tidy(true) out of line at the gutter loop's end, ours expands it)
void UICpitClass::Unknown_54()
{
	Unknown13C = OptionClass::Instance->Get_Bool("Cpit_ThrottleClickLocks");
	float opacity = OptionClass::Instance->Get_Float("Radar_Gutter_Opacity");
	char sides[4] = { 'l', 'r', 't', 'b' };
	for (int i = 0; i < 4; i++) {
		std::string name("tsi01_gutter.gutter_");
		name += sides[i];
		RefCountPtr<RenderObjClass> gutter = Create_Get(Unknown0B0->Get_Sub_Object_By_Name(name.c_str()));
		if (gutter) {
			RefCountPtr<MaterialInfoClass> info = Create_Get(gutter->Get_Material_Info());
			if (info) {
				RefCountPtr<VertexMaterialClass> material = Create_Peek(info->Peek_Vertex_Material(0));
				if (material) material->Set_Opacity(opacity);
			}
		}
	}
	UIViewClass *view = View->As_Main_View();




	G_ASSERT(View->As_Main_View());
	Unknown154 = Unknown_BE67C4->Get_Style("SpeedDisplay", (unsigned int)Mesh->Peek_Object());
	Unknown0AC = new UICpitGadget6B56AClass(0, 0, 640, 480, View->As_Main_View());
	Unknown64.Unknown_6870E0(Unknown0AC, 0x48, View, 1, 1);
	Unknown0AC->Unknown_660070(1);
	Unknown0AC->Unknown_65FFA0(1000);
	Unknown_5BB4F0(Unknown0AC);
	GadgetClass *gadget = new UICpitGadget50A850Class(0, 0, 640, 480, Client);
	gadget->Unknown_660070(1);
	gadget->Unknown_65FFC0(-1, Unknown0AC);
	Unknown64.Unknown_6870E0(gadget, 0x4D, View, 1, 0);
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();
	HulkUI = new UICpitUnknownC8Class(this, sclient, 2);
	HulkUI->Unknown_54();
	HulkUI->Unknown_7C(View, 0);
	Unknown0CC = new UICpitUnknownC8Class(this, sclient, 4);
	Unknown0CC->Unknown_54();
	Unknown0CC->Unknown_7C(View, 0);
	Scan = new UIScanClass(View->As_Main_View(), &Layer);
	Scan->Unknown_54();
	View->Unknown_6B4940(Scan);
	Unknown0D0 = new UICpitUnknownD0Class(Mesh->Peek_Object(), &Layer, this, -1);
	UnknownDC = new UIScutClass(view, &Layer);
	UnknownDC->Unknown_5417D0(this);
	UnknownDC->Unknown_54();
	View->Unknown_6B4940(UnknownDC);
	Unknown0E0 = Create_NEW(new UICpitUnknownE0Class(sclient));
	Unknown0E0->Unknown_54();
	Unknown0E0->Unknown_7C(View, 0);

	RenderObjClass *object = Mesh->Unknown_5B8B40("WARP");
	RenderObjClass *uv = Mesh->Unknown_5B8B40("WARP_BAR");
	Unknown0B4 = new UICpitWarpGadgetClass(object, uv, &Layer);
	Unknown64.Unknown_6870E0(Unknown0B4, 1, View, 1, 0);
	Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP");
	Unknown_5BB4F0(Unknown0B4);
	uv->Release_Ref();
	object->Release_Ref();

	object = Mesh->Unknown_5B8B40("LEFT");
	uv = Mesh->Unknown_5B8B40("LEFT_UV");
	Unknown0C0 = new UICpitTurnGadgetClass(object, uv, &Layer, -1, 0, 3);
	Unknown64.Unknown_6870E0(Unknown0C0, 6, View, 1, 0);
	Unknown_65EFB0(Unknown0C0, "UI_CPIT", "LEFT");
	Unknown_5BB4F0(Unknown0C0);
	uv->Release_Ref();
	object->Release_Ref();

	object = Mesh->Unknown_5B8B40("RIGHT");
	uv = Mesh->Unknown_5B8B40("RIGHT_UV");
	Unknown0C4 = new UICpitTurnGadgetClass(object, uv, &Layer, -1, 0, 3);
	Unknown64.Unknown_6870E0(Unknown0C4, 7, View, 1, 0);
	Unknown_65EFB0(Unknown0C4, "UI_CPIT", "RIGHT");
	Unknown_5BB4F0(Unknown0C4);
	uv->Release_Ref();
	object->Release_Ref();

	object = Mesh->Unknown_5B8B40("THROTTLE");
	uv = Mesh->Unknown_5B8B40("THROTTLE_UV");
	Unknown0B8 = new UICpitThrottleGadgetClass(object, uv, &Layer, -1, 0);
	Unknown64.Unknown_6870E0(Unknown0B8, 4, View, 1, 0);
	Unknown_65EFB0(Unknown0B8, "UI_CPIT", "THROTTLE");
	Unknown_5BB4F0(Unknown0B8);
	uv->Release_Ref();
	object->Release_Ref();

	object = Mesh->Unknown_5B8B40("REVERSE");
	uv = Mesh->Unknown_5B8B40("REVERSE_UV");
	Unknown0BC = new UICpitThrottleGadgetClass(object, uv, &Layer, -1, 0);
	Unknown64.Unknown_6870E0(Unknown0BC, 4, View, 1, 0);
	Unknown_65EFB0(Unknown0BC, "UI_CPIT", "REVERSE");
	Unknown_5BB4F0(Unknown0BC);
	uv->Release_Ref();
	object->Release_Ref();

	Unknown16C = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name("throttle_on"));
	Unknown170 = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name("reverse_on"));
	Unknown174 = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name("warp_on"));
	Unknown16C->Set_Hidden(true);
	Unknown170->Set_Hidden(true);
	Unknown174->Set_Hidden(true);
	gadget = new UICpitGadget674260Class(Unknown_497300(Mesh->Peek_Object()));
	gadget->Unknown_28(&Layer);
	Unknown64.Unknown_6870E0(gadget, 0x4F, View, 1, 0);
	Unknown_10();
}

// camera_manager.cpp's manager (SClient +0x12A8), as far as the warp button calls it
class CameraManagerClass	// NAME 3: class-from-file (D24) for camera_manager.cpp
{
public:
	bool Unknown_79B600();								// 0x79B600 (0x5821C0: no warp, the speed shown as 100)
	void Unknown_79B2B0(int arg);						// 0x79B2B0 (0x583250 passes 17 in the 3-D movie view)  // NAME 1 [arg]: neutral
};

// (inline: the client reads these before the other arguments of the packets' constructors)
inline unsigned Unknown_Avatar_ID(LineDisplayClientClass *client) { return(((ClientClass *)client)->AvatarID); }	// NAME 1: neutral (D10)
inline int Unknown_Player_ID(LineDisplayClientClass *client) { return(((ClientClass *)client)->Unknown1138); }	// NAME 1: neutral (D10)

// sound_manager.cpp's sound manager (the global 0xBE82A4), as far as the damage texts call it (cpacket.cpp declares it too)
class SoundManagerClass	// NAME 3: class-from-file (D24) for app\client\sound_manager.cpp
{
public:
	void Unknown_75B0D0(const char *name, int channel);	// 0x75B0D0  // NAME 1 [name, channel]: cpacket.cpp's
};
extern SoundManagerClass *SoundManager;	// 0xBE82A4  // NAME 2: what it holds
extern std::string Unknown_BE5BD0;		// 0xBE5BD0: the sound of an EMP hit (0x583DC0 plays it when inflicted is 3)  // NAME 1: neutral (D10)




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
void UICpitClass::Unknown_70(ControlKeysManagerClass *manager)
{




















































































































































	FAIL_IF(!manager) return;
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Stop", (void *)13, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Backward", (void *)9, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Backward", (void *)13, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Forward", (void *)8, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Forward", (void *)13, 0x1000);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Forward Toggle", (void *)10, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Forward Lock", (void *)11, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Move Backward Lock", (void *)12, 0);
	((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, "Tilt Down", (void *)20, 0);
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
void UICpitClass::Unknown_74(ControlKeysManagerClass *manager)
{

	FAIL_IF(!manager) return;
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Stop", 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Backward", 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Backward", 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Forward", 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Forward", 0x1000);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Forward Toggle", 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Forward Lock", 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Move Backward Lock", 0);
	((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, "Tilt Down", 0);
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

































// 0x5806E0: the steering keys (the client's key ids 2 .. 21; 0x48, 0x4B .. 0x4E taken, others not)
// NOT EXACT: inline budget (the client calls Unknown_Find_Field<float> out of line, 0x546710, in the expanded 0x584000; ours expands it and calls the Cast)
bool UICpitClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)Client;
	bool lock_speed;
	bool lock_orient;
	Unknown_57D6C0(sclient, &lock_speed, &lock_orient);
	switch (key) {
		case 2:
			if (Unknown0B4->Unknown_60()) {
				Unknown0B4->Unknown_2C(&KernelEventClass(), 0, 0, 1);
				return(true);
			}
			return(true);
		case 3:
			if (Unknown0B4->Unknown_60()) {
				Unknown0B4->Unknown_2C(&KernelEventClass(), 0, 0, 4);
				return(true);
			}
			return(true);
		case 8:
			if (lock_speed) return(true);
			if (!Unknown_584000(true)) return(true);
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 0));
			sclient->Unknown_745E40(1, false);
			Unknown144 = 1;
			Unknown0B8->Unknown_674160(0);
			Unknown0BC->Unknown_674160(0);
			Unknown0B8->Unknown_671740(true, 1.0f);
			Unknown0BC->Unknown_671740(false, 0);
			return(true);
		case 9:
			if (lock_speed) return(true);
			if (!Unknown_584000(true)) return(true);
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 1));
			sclient->Unknown_745E40(-1, false);
			Unknown144 = 2;
			Unknown0B8->Unknown_674160(0);
			Unknown0BC->Unknown_674160(0);
			Unknown0B8->Unknown_671740(false, 0);
			Unknown0BC->Unknown_671740(true, 1.0f);
			return(true);
		case 13:
			if (lock_speed) return(true);
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 4));
			sclient->Unknown_745E40(0, false);
			Unknown144 = 0;
			Unknown0B8->Unknown_674160(0);
			Unknown0BC->Unknown_674160(0);
			Unknown0B8->Unknown_671740(false, 0);
			Unknown0BC->Unknown_671740(false, 0);
			return(true);
		case 10:
		case 11:
			if (lock_speed) return(true);
			if (!Unknown_584000(true)) return(true);
			switch (Unknown144) {
				default:
					((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 2));
					sclient->Unknown_745E40(1, false);
					Unknown0B8->Unknown_674160(1);
					Unknown0B8->Unknown_A4(2.0f);
					Unknown0B8->Unknown_66C360();
					Unknown0BC->Unknown_674160(0);
					Unknown0BC->Unknown_671740(false, 0);
					Unknown144 = 3;
					return(true);
				case 3:
					if (key == 11) return(true);
				case 4:
					((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 4));
					sclient->Unknown_745E40(0, false);
					Unknown0B8->Unknown_674160(0);
					Unknown0B8->Unknown_671740(false, 0);
					Unknown0BC->Unknown_674160(0);
					Unknown0BC->Unknown_671740(false, 0);
					Unknown144 = 0;
					return(true);
				case 1:
					Unknown0B8->Unknown_674160(1);
					Unknown0B8->Unknown_A4(2.0f);
					Unknown0B8->Unknown_66C360();
					Unknown144 = 3;
					return(true);
			}
		case 12:
			if (lock_speed) return(true);
			if (!Unknown_584000(true)) return(true);
			switch (Unknown144) {
				default:
					((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 3));
					sclient->Unknown_745E40(-1, false);
					Unknown0BC->Unknown_674160(1);
					Unknown0BC->Unknown_A4(2.0f);
					Unknown0BC->Unknown_66C360();
					Unknown0B8->Unknown_674160(0);
					Unknown0B8->Unknown_671740(false, 0);
					Unknown144 = 4;
					return(true);
				case 3:
					((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 4));
					sclient->Unknown_745E40(0, false);
					Unknown0B8->Unknown_674160(0);
					Unknown0B8->Unknown_671740(false, 0);
					Unknown0BC->Unknown_674160(0);
					Unknown0BC->Unknown_671740(false, 0);
					Unknown144 = 0;
					return(true);
				case 2:
					Unknown0BC->Unknown_674160(1);
					Unknown0BC->Unknown_A4(2.0f);
					Unknown0BC->Unknown_66C360();
					Unknown144 = 4;
					return(true);
				case 4:
					return(true);
			}
		case 14:
			if (!((ClientClass *)Client)->DebugMode) return(true);
			if (lock_speed) return(true);
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 5));
			sclient->Unknown_745E40(1, true);
			Unknown144 = 5;
			return(true);
		case 15:
			if (!((ClientClass *)Client)->DebugMode) return(true);
			if (lock_speed) return(true);
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 6));
			sclient->Unknown_745E40(-1, true);
			Unknown144 = 6;
			return(true);
		case 16:
			if (lock_orient) return(true);
			if (Unknown134 == -1) return(true);
			Unknown134 = -1;
			((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), -1.0f));
			sclient->Unknown_745B90(-1.0f);
			Unknown0C0->Unknown_671740(true, 1.0f);
			Unknown0C4->Unknown_671740(false, 0);
			return(true);
		case 17:
			if (lock_orient) return(true);
			if (Unknown134 == 1) return(true);
			Unknown134 = 1;
			((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), 1.0f));
			sclient->Unknown_745B90(1.0f);
			Unknown0C4->Unknown_671740(true, 1.0f);
			Unknown0C0->Unknown_671740(false, 0);
			return(true);
		case 18:
			if (lock_orient) return(true);
			if (Unknown134 == 0) return(true);
			Unknown134 = 0;
			((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), 0));
			sclient->Unknown_745B90(0);
			Unknown0C0->Unknown_671740(false, 0);
			Unknown0C4->Unknown_671740(false, 0);
			return(true);
		case 19:
			if (lock_orient) return(true);
			if (Unknown138 == 1) return(true);
			Unknown138 = 1;
			((ClientClass *)Client)->Send_Packet(&TiltPacketClass(Unknown_Avatar_ID(Client), 1.0f));
			sclient->Unknown_745C70(1.0f);
			return(true);
		case 20:
			if (lock_orient) return(true);
			if (Unknown138 == -1) return(true);
			Unknown138 = -1;
			((ClientClass *)Client)->Send_Packet(&TiltPacketClass(Unknown_Avatar_ID(Client), -1.0f));
			sclient->Unknown_745C70(-1.0f);
			return(true);
		case 21:
			if (lock_orient) return(true);
			if (Unknown138 == 0) return(true);
			Unknown138 = 0;
			((ClientClass *)Client)->Send_Packet(&TiltPacketClass(Unknown_Avatar_ID(Client), 0));
			sclient->Unknown_745C70(0);
			return(true);
		case 0x48:
		case 0x4B:
		case 0x4C:
		case 0x4D:
		case 0x4E:
			return(true);
	}
	return(false);
}

































































































































































// 0x581670: every frame (the base's slot +0x60 last)
void UICpitClass::Unknown_60()
{
	if (HulkWatch && HulkWatch != Scan->Unknown88) Unknown_582070();
	ClientObjectClass *current_harvestable = CurrentHarvestable;
	if (current_harvestable && current_harvestable != Scan->Unknown88) {
		((ClientClass *)Client)->Send_Packet(&ActionPacketClass(Unknown_Avatar_ID(Client), 17, current_harvestable->Get_Game_ID(), 0));
		CurrentHarvestable = NULL;
	}
	ClientObjectClass *avatar = ((ClientClass *)Client)->Get_Avatar();
	if (avatar) {
		Unknown_582190(avatar, 0);
		Unknown_5821C0(avatar);
	}
	Unknown_583250();
	Unknown_583790();
	Unknown_583510();
	Unknown_583F70();
	Unknown0D0->Unknown_10();
	if (((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_750B60()) {
		if (!Unknown13D) {
			LoadingViewClass *loading = ((SClient *)((ClientClass *)Client)->Unknown_18())->LoadingView;
			if (loading && loading->Unknown68) loading->Unknown_692330();
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 4));
			((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_745E40(0, false);
			Unknown144 = 0;
			Unknown0B8->Unknown_671740(false, 0);
			Unknown0BC->Unknown_671740(false, 0);
			((ClientClass *)Client)->Send_Packet(&TurnPacketClass(Unknown_Avatar_ID(Client), 0));
			((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_745B90(0);
			((ClientClass *)Client)->Send_Packet(&TiltPacketClass(Unknown_Avatar_ID(Client), 0));
			((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_745C70(0);
			avatar->Unknown_6FC880();
			Unknown0B4->Unknown_676190(5, 0);
			Unknown0B4->Unknown_A4(2.0f);
			Unknown13D = true;
			Unknown140 = false;
			Unknown13F = false;
		}
		if (!Unknown13F) {
			Unknown0B4->Unknown_40(1);
			Unknown_65EFB0(Unknown0B4, "UI_CPIT", "RESCUE");
			Unknown_5845A0();
		}
	} else if (Unknown13D) {
		Unknown0B4->Unknown_676190(0, 0);
		Unknown0B4->Unknown_A4(0);
		Unknown13D = false;
		Unknown140 = false;
		Unknown13F = false;
	}
	if (Unknown0E4 && !Unknown0E0->Is_Active()) {
		int choice = Unknown0E0->Unknown_553A40();
		Unknown0E4 = false;
		if (choice != -1) {
			switch (choice) {
				case 0:
					Unknown13F = true;
					((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown13C1 = true;
					((ClientClass *)Client)->Send_Packet(&Packet_008FClass(Unknown_Avatar_ID(Client)));
					break;
				case 1:
					Unknown140 = !Unknown140;
					((ClientClass *)Client)->Send_Packet(&IncapacitanceDistressBeaconPacketClass(Unknown_Avatar_ID(Client), Unknown140));
					if (Unknown140) {
						((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D990(0, "Distress beacon activated.", 0);
					} else {
						((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D990(0, "Distress beacon deactivated.", 0);
					}
					break;
			}
		}
	}
	Unknown_584110();
	Unknown5BB320Class::Unknown_60();
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

// bridge_int.cpp's interface (SClient +0x128C), as far as the incapacitance details call it (mainview.cpp, sclient.cpp declare it too)
class BridgeInterfaceClass	// NAME 3: class-from-file (D24) for bridge_int.cpp
{
public:
	void Unknown_53B770();								// 0x53B770 (0x584690, first)
	void Unknown_53B710();								// 0x53B710 (0x5821C0: the warp done)
	void Unknown_53B740();								// 0x53B740 (0x5821C0: waiting for the warp)
};

// 0x5821C0: the warp button's state from the avatar's aux data, the throttle lights and the speed display (slot +0x60)
// NOT EXACT: written from the client (the finds' out-of-line casts and the expanded 0x584000 follow the client's inline budget)
void UICpitClass::Unknown_5821C0(ClientObjectClass *avatar)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)avatar->AuxData;
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();
	{
		NameClass name("MaxSpeed");
		DeltaTemplateClass<float> *max_speed = Unknown_Find_Field<DeltaTemplateClass<float> >(aux, name);
		if (max_speed && max_speed->Unknown70 <= 1) max_speed = NULL;
		name = NameClass("LockSpeed");
		DeltaTemplateClass<bool> *lock_speed = Unknown_Find_Field<DeltaTemplateClass<bool> >(aux, name);
		if (lock_speed && lock_speed->Unknown70 <= 1) lock_speed = NULL;
		if (max_speed || lock_speed) {
			if (max_speed && !lock_speed) {
				NameClass lock_name("LockSpeed");
				lock_speed = Unknown_Find_Field<DeltaTemplateClass<bool> >(aux, lock_name);
				if (lock_speed && !lock_speed->Unknown70) lock_speed = NULL;
			}
			bool engine = Unknown_584000(true);
			bool locked = lock_speed ? lock_speed->Value : false;
			bool on = engine && !locked;
			Unknown0B8->Unknown_40(on);
			Unknown0BC->Unknown_40(on);
			Unknown0B4->Unknown_40(on);
			if (engine) Unknown13E = true;
			Unknown_5845A0();
		}
	}
	{
		NameClass name("LockOrient");
		DeltaTemplateClass<bool> *lock_orient = Unknown_Find_Field<DeltaTemplateClass<bool> >(aux, name);
		if (lock_orient && lock_orient->Unknown70 > 1) {
			if (lock_orient->Value) {
				Unknown0C0->Unknown_40(0);
				Unknown0C4->Unknown_40(0);
			} else {
				Unknown0C0->Unknown_40(1);
				Unknown0C4->Unknown_40(1);
			}
		}
	}
	CameraManagerClass *camera = ((SClient *)((ClientClass *)Client)->Unknown_18())->CameraManager;
	if (camera && camera->Unknown_79B600()) {
		if (Unknown0B4->Unknown_60()) {
			Unknown0B4->Unknown_40(0);
			Unknown_65EFB0(Unknown0B4, "UI_CPIT", "CANNOT_WARP");
			Unknown13E = true;
			Unknown_5845A0();
		}
	} else if (Unknown13D) {
		if (!Unknown13E) {
			Unknown0B4->Unknown_40(1);
			Unknown_65EFB0(Unknown0B4, "UI_CPIT", "RESCUE");
			Unknown13E = true;
		} else if (Unknown13F) {
			Unknown0B4->Unknown_40(0);
		}
	} else if (((SClient *)((ClientClass *)Client)->Unknown_18())->ViewMode == 1) {
		Unknown_65EFB0(Unknown0B4, "UI_CPIT", "ON_PLANET");
		Unknown0B4->Unknown_40(0);
		Unknown_5845A0();
	} else if (((SClient *)((ClientClass *)Client)->Unknown_18())->ViewMode == 5) {
		Unknown_65EFB0(Unknown0B4, "UI_CPIT", "GAS_GIANT");
		Unknown0B4->Unknown_40(0);
		Unknown_5845A0();
	} else {
		{
			NameClass name("WarpAvailable");
			DeltaTemplateClass<int> *warp = Unknown_Find_Field<DeltaTemplateClass<int> >(aux, name);
			if (Unknown13E) {
				if (warp && !warp->Unknown70) warp = NULL;
			} else {
				if (warp && warp->Unknown70 <= 1) warp = NULL;
			}
			if (warp) {
				const char *text;
				if (warp->Value == 2) {
					Unknown150 = true;
					Unknown0B4->Unknown_40(1);
					text = "WARP";
				} else {
					Unknown150 = false;
					Unknown0B4->Unknown_40(0);
					text = (warp->Value == 1) ? "NO_WARP" : "CANNOT_WARP";
				}
				Unknown_65EFB0(Unknown0B4, "UI_CPIT", text);
				Unknown_5845A0();
			}
		}
		{
			NameClass name("PrivateWarpState");
			DeltaTemplateClass<int> *state = Unknown_Find_Field<DeltaTemplateClass<int> >(aux, name);
			if (Unknown13E) {
				if (state && !state->Unknown70) state = NULL;
			} else {
				if (state && state->Unknown70 <= 1) state = NULL;
			}
			if (state) {
				switch (state->Value) {
					case 0:
						if (Unknown150) {
							Unknown0B4->Unknown_40(1);
							Unknown_5845A0();
							Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP");
						} else {
							Unknown0B4->Unknown_40(0);
							Unknown_5845A0();
							NameClass warp_name("WarpAvailable");
							DeltaTemplateClass<int> *warp = Unknown_Find_Field<DeltaTemplateClass<int> >(aux, warp_name);
							if (warp && warp->Unknown70 && warp->Value == 1) {
								Unknown_65EFB0(Unknown0B4, "UI_CPIT", "NO_WARP");
							} else {
								Unknown_65EFB0(Unknown0B4, "UI_CPIT", "CANNOT_WARP");
							}
						}
						break;
					case 10:
						Unknown0B4->Unknown_40(0);
						Unknown_5845A0();
						Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP_LOW");
						break;
					case 4:
						Unknown0B4->Unknown_40(0);
						Unknown_5845A0();
						Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP_LOCKED");
						break;
					case 5:
						Unknown0B4->Unknown_40(0);
						Unknown_5845A0();
						Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP_ENGINE_NOT_INSTALLED");
						break;
					case 3: case 6: case 7: case 8: case 9: case 11:
					{
						Unknown0B4->Unknown_40(0);
						Unknown_5845A0();
						Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP_WAIT");
						NameClass time_name("WarpTriggerTime");
						DeltaTemplateClass<DurationClass> *time = Unknown_Find_Field<DeltaTemplateClass<DurationClass> >(aux, time_name);
						if (time && time->Unknown70) Unknown0B4->Unknown_676190(4, time->Value.As_Int());
						Unknown0B4->Unknown_A4(0);
						SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
						if (client && client->BridgeInterface) client->BridgeInterface->Unknown_53B740();
						Unknown174->Set_Hidden(true);
						break;
					}
					case 1:
					{
						Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP_DONE");
						NameClass time_name("WarpTriggerTime");
						DeltaTemplateClass<DurationClass> *time = Unknown_Find_Field<DeltaTemplateClass<DurationClass> >(aux, time_name);
						if (time && time->Unknown70) Unknown0B4->Unknown_676190(1, time->Value.As_Int());
						Unknown0B4->Unknown_A4(0);
						break;
					}
					case 2:
					{
						Unknown_65EFB0(Unknown0B4, "UI_CPIT", "WARP_DONE");
						Unknown174->Set_Hidden(false);
						Unknown0B4->Unknown_676190(2, 0);
						Unknown0B4->Unknown_A4(1.0f);
						((MainViewClass *)View->As_Main_View())->CameraEffects->Unknown_6E32E0(2, 1.0f, 0);
						SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
						if (client && client->BridgeInterface) client->BridgeInterface->Unknown_53B710();
						break;
					}
				}
			}
		}
		if (Unknown13E) Unknown13E = false;
	}
	Vector3 velocity = avatar->Unknown_2C();
	float speed = velocity.Length() * 1000.0f;
	if (Vector3::Dot_Product(velocity, avatar->Unknown_28(true).Get_X_Vector()) < 0.0f) speed = -speed;
	if (Unknown144 == 1 || Unknown144 == 3 || ((UICpitGadget6B56AClass *)Unknown0AC)->Unknown49) {
		Unknown16C->Set_Hidden(false);
	} else {
		Unknown16C->Set_Hidden(true);
	}
	if (Unknown144 == 2 || Unknown144 == 4) {
		Unknown170->Set_Hidden(false);
	} else {
		Unknown170->Set_Hidden(true);
	}
	if (camera && camera->Unknown_79B600()) {
		((FontStyleClass *)Unknown154)->Change_Text("100", 0);
		Unknown130 = 100;
	} else {
		int shown = (int)speed;
		if (Unknown130 != shown) {
			Unknown130 = shown;
			char text[8];
			sprintf(text, "%03d", shown);
			((FontStyleClass *)Unknown154)->Change_Text(text, 0);
		}
	}
	UICpitThrottleGadgetClass *gadget;
	if (Unknown144 == 1 || Unknown144 == 3) {
		NameClass name("MaxSpeed");
		DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux, name);
		if (!t || !t->Unknown70) return;
		if (!(t->Value * 0.99f < speed)) {
			((FontStyleClass *)Unknown154)->Unknown_6CE910(0);
			return;
		}
		gadget = Unknown0B8;
	} else if (Unknown144 == 2 || Unknown144 == 4) {
		NameClass name("MinSpeed");
		DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux, name);
		if (!t || !t->Unknown70) return;
		if (!(t->Value * 0.99f > speed)) {
			((FontStyleClass *)Unknown154)->Unknown_6CE910(0);
			return;
		}
		gadget = Unknown0BC;
	} else {
		((FontStyleClass *)Unknown154)->Unknown_6CE910(0);
		return;
	}
	if (gadget->Unknown80 == 0) gadget->Unknown_674120(1);
	((FontStyleClass *)Unknown154)->Unknown_6CE910(1);
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

// 0x583790: the throttle (+0xB8) and reverse (+0xBC) buttons
// NOT EXACT: inline budget (the client calls the IsAutoFollowing Cast out of line, 0x584FF0; then the packets' client loads in other registers)
void UICpitClass::Unknown_583790()
{
	if (!Unknown0B8->Unknown_60() && !Unknown0BC->Unknown_60()) return;
	SClient *sclient = (SClient *)Client;
	if (!sclient) return;
	ClientObjectClass *avatar = sclient->Get_Avatar();
	if (!avatar) return;
	if (!avatar->AuxData) return;
	NameClass name("IsAutoFollowing");
	DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)avatar->AuxData, name);
	if (t && t->Unknown70 > 1) {
		Unknown0B8->Unknown_671740(false, 0);
		Unknown0BC->Unknown_671740(false, 0);
		Unknown144 = 0;
		Unknown0B8->Unknown_674160(0);
		Unknown0BC->Unknown_674160(0);
		return;
	}
	if (Unknown0B8->Unknown66 || Unknown0BC->Unknown66
		|| (Unknown0B8->Unknown64 && (Unknown0B8->Unknown80 == 2 || Unknown0BC->Unknown80 == 2))
		|| (Unknown0BC->Unknown64 && (Unknown0B8->Unknown80 == 2 || Unknown0BC->Unknown80 == 2))) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
		((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 4));
		sclient->Unknown_745E40(0, false);
		Unknown144 = 0;
		Unknown0B8->Unknown_671740(false, 0);
		Unknown0BC->Unknown_671740(false, 0);
		Unknown0B8->Unknown_674160(0);
		Unknown0BC->Unknown_674160(0);
	} else if (Unknown0B8->Unknown64) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
		if (Unknown13C) {
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 2));
			sclient->Unknown_745E40(1, false);
			Unknown144 = 3;
			Unknown0B8->Unknown_671740(true, 2.0f);
			Unknown0B8->Unknown_674160(1);
			Unknown0BC->Unknown_674160(0);
		} else {
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 0));
			sclient->Unknown_745E40(1, false);
			Unknown144 = 1;
			Unknown0B8->Unknown_671740(true, 1.0f);
		}
	} else if (Unknown0BC->Unknown64) {
		SoundLibraryClass::Play_2D_Sound("ButtonClick01.wav", 2);
		if (Unknown13C) {
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 3));
			sclient->Unknown_745E40(-1, false);
			Unknown144 = 4;
			Unknown0BC->Unknown_671740(true, 2.0f);
			Unknown0BC->Unknown_674160(1);
			Unknown0B8->Unknown_674160(0);
		} else {
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 1));
			sclient->Unknown_745E40(-1, false);
			Unknown144 = 2;
			Unknown0BC->Unknown_671740(true, 1.0f);
		}
	} else if (Unknown0B8->Unknown_6740E0(1.0f)) {
		if (!Unknown13C) SoundLibraryClass::Play_2D_Sound("talk_auto_thrust_engaged.wav", 2);
		Unknown0B8->Unknown_674160(1);
		Unknown0BC->Unknown_674160(0);
		Unknown0B8->Unknown_671740(true, 2.0f);
		Unknown144 = 3;
	} else if (Unknown0BC->Unknown_6740E0(1.0f)) {
		if (Unknown13C) SoundLibraryClass::Play_2D_Sound("talk_auto_thrust_engaged.wav", 2);
		Unknown0B8->Unknown_674160(0);
		Unknown0BC->Unknown_674160(1);
		Unknown0BC->Unknown_671740(true, 2.0f);
	}
	Unknown0B8->Unknown_AC();
	Unknown0BC->Unknown_AC();
	Unknown0B8->Unknown_671630();
	Unknown0BC->Unknown_671630();
}


















































// 0x583DA0
void UICpitClass::Unknown_583DA0(int arg)
{
}

// 0x583DC0: the player's damage texts onto the cockpit (CLIENT_DAMAGE 100), kept in Unknown158 until they finish
// NOT EXACT: inline budget (the client calls the list's _Construct out of line, 0x584F90, and keeps emp in bl; ours expands it)
void UICpitClass::Unknown_583DC0(Unknown6D47C0Class *text, Unknown6D47C0Class *header, int inflicted, bool emp)
{
	if (text) {
		if (emp) {
			Mesh->Peek_Object()->Add_Sub_Object_To_Bone((RenderObjClass *)text, "reactor_bar0");
		} else {
			Mesh->Peek_Object()->Add_Sub_Object_To_Bone((RenderObjClass *)text, "shield_bar0");
		}
		Unknown158.push_back(*(RenderObjClass **)&text);	// (the text is fontmanager.h's FontStyleClass, a render object: pushed from its argument slot)
	}
	if (header) {
		if (emp) {
			Mesh->Peek_Object()->Add_Sub_Object_To_Bone((RenderObjClass *)header, "reactor_bar0");
		} else {
			Mesh->Peek_Object()->Add_Sub_Object_To_Bone((RenderObjClass *)header, "shield_bar0");
		}
		Unknown158.push_back(*(RenderObjClass **)&header);
		if (inflicted == 3) {
			SoundManager->Unknown_75B0D0(Unknown_BE5BD0.c_str(), 1);
		}
	}
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

// 0x584690: the incapacitance details (INCAPACITANCE_DETAILS 144) onto the +0xE0 question, shown
// NOT EXACT: inline budget (the client calls the vectors' erase out of line, 0x4FC7C0; ours expands it)
void UICpitClass::Unknown_584690(IncapacitanceDetailsPacket *packet)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();
	if (sclient && sclient->BridgeInterface) sclient->BridgeInterface->Unknown_53B770();
	std::vector<std::string> first;
	std::vector<std::string> second;
	first.erase(first.begin(), first.end());
	second.erase(second.begin(), second.end());
	first.insert(first.end(), packet->Unknown138);
	first.insert(first.end(), packet->Unknown148);
	first.insert(first.end(), packet->Unknown158);
	second.insert(second.end(), packet->Unknown168);
	second.insert(second.end(), 1, packet->Unknown178);
	second.insert(second.end(), 1, packet->Unknown188);
	Unknown0E0->Unknown_552FC0(&packet->Desc);
	Unknown0E0->Unknown_553370(packet->Unknown128);
	Unknown0E0->Unknown_553500(first);
	Unknown0E0->Unknown_5537A0(second);
	Unknown0E0->Unknown_58();
	Unknown0E4 = true;
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
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_cpit.cpp (its __FILE__)
**
**	The cockpit interface (ui_cpit.h's UICpitClass, the main view's +0x1D0; 0x178 bytes over Unknown5BB320Class,
**	the TSI01A mesh and fc_cpit.ini's texts): the warp, throttle, reverse and turn buttons, the hulk and harvest
**	windows (+0xC8 / +0xCC), the scanner (+0xD4), ui_scut.cpp's window (+0xDC), the incapacitance question
**	(+0xE0), two watchers of the looted hulk and the harvested object (+0xF4 / +0x110), the damage texts
**	(Unknown158) and the speed display. The client's window runs 0x57D6C0-0x584BE1 (claims #29 / #30), followed by
**	its template instances to 0x585100.
**
**	Rebuilt (CP1, 2026-10-31), the asserts on the client's lines (D25), the functions without asserts between their
**	neighbours' lines in the client's order:
**	  exact: the constructor 0x57D830, destructor 0x57EB80, slots +0x58 / +0x5C / +0x60 / +0x7C, the control keys
**	  0x57F240 / 0x57FCC0 (the first nine keys' names converted implicitly, the rest as std::string temporaries: the client's
**	  inline budget), the window close
**	  0x581CE0, 0x581E60 / 0x581F90 / 0x582070 / 0x5820C0 / 0x582190, the warp and turn buttons 0x583250 / 0x583510,
**	  0x583DA0, 0x583F70, the font state 0x584110 / 0x5845A0, 0x584A20 / 0x584A60 and both watchers 0x584A80-0x584BE0;
**	  still differing (the inline budget unless noted): 0x57D6C0, the set-up 0x57DD20, the
**	  steering keys 0x5806E0, the warp state 0x5821C0, the throttle buttons 0x583790, the damage texts 0x583DC0,
**	  0x584000 (order) and the incapacitance details 0x584690.
**	The packets' player ids go through two inline accessors (Unknown_Avatar_ID / Unknown_Player_ID): the client
**	reads them before the constructors' other arguments, which a member read does not reproduce. The PacketClass
**	destructor copies 0x581610-0x581CC0, 0x581F70 and 0x5834F0 are too short to count.
*/
