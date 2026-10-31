// Earth & Beyond client (reconstruction): app\client\ui_cpit.h, the cockpit interface (the client's ui_cpit.cpp per its
// asserts; this header's name is provisional, NAME 1). Moved out of sclient.h and laid out from the constructor
// 0x57D830 and the destructor 0x57EB80 (CP1, 2026-10-31). sclient.h includes it where the class was declared.
#ifndef UI_CPIT_H  // NAME 1 [ui_cpit.h]: provisional header name for ui_cpit.cpp
#define UI_CPIT_H

#include <list>
#include "sclient.h"

class SClient;
class ClientObjectClass;
class RenderObjClass;
class GadgetClass;
class UICpitWarpGadgetClass;
class UICpitThrottleGadgetClass;
class UICpitTurnGadgetClass;
class IncapacitanceDetailsPacket;
class AvatarDescClass;
class Unknown6D47C0Class;
class UICpitClass;

#ifndef EFF_O_H	// eff_o.h's object watcher (eff_o.h, included first where it is used, declares it there)
// An object watcher: the base with only a virtual destructor (vtable 0xAF0DD0, its deleting destructor 0x57DBC0 first emitted
// in ui_cpit.cpp; influence.h's record has the same base)
class UnknownAF0DD0Class	// NAME 1: neutral (D10)
{
public:
	virtual ~UnknownAF0DD0Class() {}
	void Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *node, ClientObjectClass *object);	// 0x73FDB0 (the node out of its list, then into the object's (0x6F5090) with this as its value)  // NAME 1 [node, object]: what the effects pass
};
#endif

// A watched object pointer over UnknownAF0DD0Class (0x18 bytes before a subclass's own members; slots +0x04 / +0x08
// 0x57DC20 / 0x57DBF0 first emitted in ui_cpit.cpp, shared by the cockpit's two watchers and the 0x28-byte object
// 0x5DBFC0 at its +0xD0): slot +0x0C is the subclass's notice that the object goes (interim placement: D137)
class UnknownObjectWatchClass : public UnknownAF0DD0Class	// NAME 1: neutral (D10)
{
public:
	UnknownObjectWatchClass() : Object(NULL) {}
	virtual void Unknown_04(int arg) { Unknown_0C(); Unknown_08(NULL); }	// +0x04 0x57DC20 (the object's node goes)  // NAME 1 [arg]: eff_o.h's
	virtual void Unknown_08(ClientObjectClass *object) { Object = object; Unknown_73FDB0(&Node, object); }	// +0x08 0x57DBF0  // NAME 1 [object]: what it holds
	virtual void Unknown_0C() = 0;						// +0x0C
	operator ClientObjectClass * () { return(Object); }	// (the asserts' 'CurrentHarvestable == 0' / '!= 0' read +0x04)
	void operator = (ClientObjectClass *object) { Unknown_08(object); }	// (a virtual call to slot +0x08 where the member is set)

	ClientObjectClass *Object;							// +0x04  // NAME 2: what it holds
	DataNode<UnknownAF0DD0Class *> Node;				// +0x08 (vtable 0xAF0DDC; constructor 0x584C50)  // NAME 2: eff_o.h's watch nodes'
};

// The cockpit's watcher of the hulk being looted (+0xF4; vtable 0xAF0DBC, constructor 0x584A80)
class UICpitHulkWatchClass : public UnknownObjectWatchClass	// NAME 1: neutral (D10)
{
public:
	UICpitHulkWatchClass(UICpitClass *owner);			// 0x584A80  // NAME 2 [owner]: what it holds
	virtual void Unknown_0C();							// +0x0C 0x584AF0 (FAIL_IF '!co' l.2082; the cockpit's 0x582070)
	void operator = (ClientObjectClass *object) { Unknown_08(object); }	// (the base's: the implicit copy assignment hides it)

	UICpitClass *Owner;									// +0x18  // NAME 2: what it holds
};

// The cockpit's watcher of the object being harvested (+0x110; vtable 0xAF0DA8, constructor 0x584B70)
class UICpitHarvestWatchClass : public UnknownObjectWatchClass	// NAME 1: neutral (D10)
{
public:
	UICpitHarvestWatchClass(UICpitClass *owner);		// 0x584B70  // NAME 2 [owner]: what it holds
	virtual void Unknown_0C();							// +0x0C 0x584BE0 (FAIL_IF '!co' l.2097; the cockpit's 0x5820C0(false))
	void operator = (ClientObjectClass *object) { Unknown_08(object); }	// (the base's: the implicit copy assignment hides it)

	UICpitClass *Owner;									// +0x18  // NAME 2: what it holds
};

// What +0xD0 points to (0x28 bytes, constructor 0x5DBFC0; its file is unknown): a watcher of the avatar with a slot +0x10
class UICpitUnknownD0Class : public UnknownObjectWatchClass	// NAME 1: neutral (D10)
{
public:
	UICpitUnknownD0Class(RenderObjClass *model, LayerClass *layer, UICpitClass *cpit, int arg);	// 0x5DBFC0 (the set-up passes the mesh's model, the layer, the cockpit, -1)  // NAME 1 [model, layer, cpit, arg]: what the set-up passes
	virtual void Unknown_0C();							// +0x0C (its own)
	virtual void Unknown_10();							// +0x10 (slot +0x60, every frame)
	char Unknown18[0x28 - 0x18];						// +0x18
	void operator = (ClientObjectClass *object) { Unknown_08(object); }	// (the base's: the implicit copy assignment hides it)
};

// The hulk (+0xC8) and harvest (+0xCC) windows (0xE4 bytes, constructor 0x5850E0 with 2 / 4; their file is unknown)
class UICpitUnknownC8Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	UICpitUnknownC8Class(UICpitClass *cpit, SClient *sclient, int type);	// 0x5850E0  // NAME 1 [cpit, sclient, type]: what the set-up passes
	char Unknown84[0xE4 - 0x84];						// +0x84
};

// What +0xE0 holds (0x1B4 bytes, constructor 0x551FB0; its file is unknown): the incapacitance question
class UICpitUnknownE0Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	UICpitUnknownE0Class(SClient *sclient);			// 0x551FB0  // NAME 1 [sclient]: what the set-up passes
	char Unknown84[0x1B4 - 0x84];						// +0x84
	void Unknown_552FC0(AvatarDescClass *desc);			// 0x552FC0 (0x584690: the packet's Desc)  // NAME 1 [desc]: what 0x584690 passes
	void Unknown_553370(std::string text);				// 0x553370 (0x584690: the packet's +0x128, by value)  // NAME 1 [text]: neutral
	void Unknown_553500(std::vector<std::string> lines);	// 0x553500 (0x584690: the packet's +0x138 .. +0x158, by value)  // NAME 1 [lines]: neutral
	void Unknown_5537A0(std::vector<std::string> lines);	// 0x5537A0 (0x584690: the packet's +0x168 .. +0x188, by value)  // NAME 1 [lines]: neutral
	int Unknown_553A40();								// 0x553A40: the answer once it closes (-1 none, 0 / 1: slot +0x60's Packet_008FClass / distress beacon)
};

// ui_scut.cpp's interface (0xB8 bytes, constructor 0x610570), as far as the cockpit holds it
class UIScutClass : public InterfaceClass	// NAME 3: class-from-file (D24) for ui_scut.cpp
{
public:
	UIScutClass(UIViewClass *view, LayerClass *layer);	// 0x610570 (0x57DD20 passes the view and the cockpit's layer)  // NAME 1 [view, layer]: what 0x57DD20 passes
	void Unknown_5417D0(InterfaceClass *owner);		// 0x5417D0 (intrface.cpp's; the set-up passes the cockpit)  // NAME 1 [owner]: neutral
	char Unknown84[0xB8 - 0x84];						// +0x84
};

void Unknown_57D6C0(SClient *sclient, bool *lock_speed, bool *lock_orient);	// 0x57D6C0 (cdecl; the cockpit's 0x5806E0, the view's 0x6B58A0): the avatar's LockSpeed / LockOrient (G_ASSERT 'sclient' l.132)  // NAME 1 [sclient, lock_speed, lock_orient]: what they hold

// ui_cpit.cpp's cockpit interface (the main view's +0x1D0; 0x178 bytes, tables 0xAF0CFC / 0xAF0CF0 / 0xAF0CE4)
class UICpitClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for ui_cpit.cpp
{
public:
	UICpitClass(MainViewClass *view, const LayerClass &layer);	// 0x57D830 (0x178 bytes; the main view's 0x50C690)  // NAME 1 [view, layer]: what 0x50C690 passes
	virtual ~UICpitClass();								// 0x57EB80 (slot 0x57DC50)
	virtual void Unknown_54();							// 0x57DD20 (G_ASSERT 'View->As_Main_View()' l.289; the main view calls it after construction)
	virtual void Unknown_58();							// 0x57F080 (the main view's 0x5141A0)
	virtual void Unknown_5C();							// 0x57F1B0 (the main view's 0x5141E0 / 0x514220)
	virtual void Unknown_60();							// 0x581670
	virtual bool Unknown_68(int key);					// 0x5806E0  // NAME 1 [key]: the base's
	virtual void Unknown_70(ControlKeysManagerClass *manager);	// 0x57F240 (FAIL_IF '!manager' l.623)  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_74(ControlKeysManagerClass *manager);	// 0x57FCC0 (FAIL_IF '!manager' l.647)  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_7C(UIViewClass *view, int arg);	// 0x57F010  // NAME 1 [view, arg]: the base's
	void Unknown_581CE0(InterfaceClass *ui);			// 0x581CE0 (FAIL_IF '!current_harvestable' l.1169, G_ASSERT 'false' l.1177)  // NAME 1 [ui]: what it compares
	void Unknown_581E60();								// 0x581E60 (FAIL_IF '!current_target' l.1188)
	void Unknown_581F90(ClientObjectClass *hulk);		// 0x581F90 (cpacket.cpp: LOOT_HULK_PERMISSION 140; FAIL_IFs l.1194 / 1197, G_ASSERT 'HulkUI' l.1208)  // NAME 3 [hulk]: the FAIL_IF's
	void Unknown_582070();								// 0x582070: the hulk let go
	void Unknown_5820C0(bool on);						// 0x5820C0 (cpacket.cpp: OPEN_INTERFACE 102 passes UIChange == 0; G_ASSERTs l.1227 / 1229)  // NAME 1 [on]: what the handler passes
	void Unknown_5821C0(ClientObjectClass *avatar);		// 0x5821C0 (slot +0x60 passes the avatar)  // NAME 1 [avatar]: what it gets
	void Unknown_582190(ClientObjectClass *object, int arg);	// 0x582190: +0xD0's object (no caller; slot +0x58 expands it)  // NAME 1 [object, arg]: neutral
	void Unknown_583250();								// 0x583250: the warp button (+0xB4): a WarpPacketClass from the radar's path (FAIL_IF '!radar' l.1615), the distress request, or the 3-D movie view's camera
	void Unknown_583510();								// 0x583510: the turn buttons (+0xC0 / +0xC4): a TurnPacketClass and SClient 0x745B90
	void Unknown_583790();								// 0x583790: the throttle (+0xB8) and reverse (+0xBC) buttons: MovePacketClass and SClient 0x745E40 (nothing while IsAutoFollowing)
	void Unknown_583DA0(int arg);						// 0x583DA0 (empty)  // NAME 1 [arg]: neutral
	void Unknown_583F70();								// 0x583F70: the finished effects of Unknown158 removed
	bool Unknown_584000(bool report);					// 0x584000: the avatar's MaxSpeed is set, else (report) 'Engine unavailable. Please wait.'  // NAME 1 [report]: neutral
	void Unknown_584110();								// 0x584110 (FAIL_IFs '!warpfont' / '!distressfont' / '!spacefont' l.1986-1988)
	void Unknown_5845A0();								// 0x5845A0: the WARP_CPIT text dimmed while the warp button (+0xB4) is off
	void Unknown_584A20();								// 0x584A20
	void Unknown_584A60();								// 0x584A60 (SClient 0x745160)
	void Unknown_584690(IncapacitanceDetailsPacket *packet);	// 0x584690 (cpacket.cpp: INCAPACITANCE_DETAILS 144)  // NAME 1 [packet]: the handler's packet
	void Unknown_583DC0(Unknown6D47C0Class *text, Unknown6D47C0Class *header, int inflicted, bool emp);	// 0x583DC0 (cpacket.cpp: CLIENT_DAMAGE 100 for the player's own damage)  // NAME 1 [text, header, inflicted, emp]: what the handler passes

	GadgetClass *Unknown0AC;							// +0xAC (0x57DD20 news it, 0x5C bytes, 0x6B56A0; the destructor deletes it through 0x65EED0)
	RefCountPtr<RenderObjClass> Unknown0B0;				// +0xB0 'tsi01_gutter' (the constructor)
	UICpitWarpGadgetClass *Unknown0B4;							// +0xB4 'WARP' (0x57DD20)
	UICpitThrottleGadgetClass *Unknown0B8;							// +0xB8 'THROTTLE'
	UICpitThrottleGadgetClass *Unknown0BC;							// +0xBC 'REVERSE'
	UICpitTurnGadgetClass *Unknown0C0;							// +0xC0 'LEFT'
	UICpitTurnGadgetClass *Unknown0C4;							// +0xC4 'RIGHT'
	InterfaceClass *HulkUI;								// +0xC8 (0xE4 bytes, 0x5850E0; G_ASSERT 'HulkUI' l.1208)  // NAME 3: the assert's
	InterfaceClass *Unknown0CC;							// +0xCC (0xE4 bytes, 0x5850E0: the harvesting window)
	UICpitUnknownD0Class *Unknown0D0;				// +0xD0 (0x28 bytes, 0x5DBFC0)
	UIScanClass *Scan;									// +0xD4 (0x1B0 bytes, 0x60D800; the main view's 0x5136A0 returns it, 0x513630 calls 0x60E540 on it)  // NAME 3: its file's (ui_scan.cpp)
	int Unknown0D8;										// +0xD8
	UIScutClass *UnknownDC;								// +0xDC (0xB8 bytes, 0x610570; the main view's 0x514400 returns it)
	RefCountPtr<UICpitUnknownE0Class> Unknown0E0;				// +0xE0 (0x1B4 bytes, 0x551FB0)
	bool Unknown0E4;									// +0xE4 (0x584A20 clears it)
	int Unknown0E8;										// +0xE8
	char Unknown0EC[0xF4 - 0xEC];						// +0xEC
	UICpitHulkWatchClass HulkWatch;						// +0xF4 (0x581F90 sets it to the hulk)  // NAME 2: what it holds
	UICpitHarvestWatchClass CurrentHarvestable;			// +0x110 (G_ASSERTs 'CurrentHarvestable == 0' / '!= 0', l.1227 / 1229)  // NAME 3: the asserts'
	int Unknown12C;										// +0x12C
	int Unknown130;										// +0x130 (0xFFFFFF)
	int Unknown134;										// +0x134
	int Unknown138;										// +0x138
	bool Unknown13C;									// +0x13C option 'Cpit_ThrottleClickLocks' (0x57DD20 reads it, the destructor saves it)
	bool Unknown13D;									// +0x13D
	bool Unknown13E;									// +0x13E
	bool Unknown13F;									// +0x13F
	bool Unknown140;									// +0x140
	int Unknown144;										// +0x144 (0x584A60 clears it)
	int Unknown148;										// +0x148: 1 while active (slots +0x58 / +0x5C)
	int Unknown14C;										// +0x14C
	bool Unknown150;									// +0x150
	void *Unknown154;									// +0x154 ('SpeedDisplay': the font manager's 0x6D25D0)
	std::list<RenderObjClass *> Unknown158;				// +0x158 (constructed with a count of 0)
	char Unknown164[0x16C - 0x164];						// +0x164
	RefCountPtr<RenderObjClass> Unknown16C;				// +0x16C 'throttle_on'
	RefCountPtr<RenderObjClass> Unknown170;				// +0x170 'reverse_on'
	RefCountPtr<RenderObjClass> Unknown174;				// +0x174 'warp_on'
};

#endif // UI_CPIT_H
