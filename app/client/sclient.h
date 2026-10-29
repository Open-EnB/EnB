/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\sclient.h (name assumed from SClient::Process_Packets)
**
**	SClient, the game's client object (0x14F8 bytes, constructor 0x7405E0, destructor 0x740BB0, vtable
**	0xB03240: the base's 23 slots and three): the client base ClientClass (client.h, client.cpp) at +0x00,
**	the client's options SOptionsClass (soptions.h, vtable 0xB0323C) at +0x1150. The layout is the
**	constructor's, the destructor's and Init's (docs/kernel-map-step9.md 2.2, D90 9d); the members'
**	classes are named after the file of their constructor where it is known (D24), declared as far as
**	sclient.cpp uses them. cpacket.cpp's handlers are the packets' Process_Packet(SClient *) (PacketClass
**	slot +0x08), called by Process_Packets.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SCLIENT_H  // NAME 3 [sclient.h]: header name assumed from the class
#define SCLIENT_H

#include "client.h"
#include "soptions.h"
#include "tasks\tsk_game.h"
#include "vector.h"
#include "vector2.h"
#include "vector3.h"
#include "intersec.h"
#include "intrface.h"
#include "w3derr.h"
#include <deque>
#include <vector>
#include <hash_map.h>
#include "refcount.h"
#include "slist.h"
#include "..\shared\land_lighting.h"

struct KernelEventClass;			// kernel.h: an input event (16 bytes)
class LoginUnknown0CClass;		// tsk_login.h: inpdsp.cpp's input dispatcher (0x18 bytes)
class LoginUnknown18Class;		// tsk_login.h: the software mouse (0x30 bytes, 0x514FE0)
class Font3DInstanceClass;
class FileClass;
class RefCountClass;
class LayerClass;
class PDAInterfaceUnknownA0Class;	// NAME 1: neutral (D10): what the PDA's +0xA8 object's 0x7C8DB0 returns (its +0xA0)
// what 0x49F200 returns (the referral rewards), as far as SClient 0x751460 calls it: 0x49F300 gives one entry per
// point to spend, 12 bytes each
struct BuddyRewardStruct	// NAME 2: what 0x751460 sums from it
{
	int Unknown00;										// +0x00
	int SkillPoints;									// +0x04 (0x751460's '+%d Skill Points')  // NAME 2: the text it fills
	int Credits;										// +0x08 (0x751460's '+%d Credits')  // NAME 2: the text it fills
};
class Unknown49F200Class	// NAME 1: neutral (D10)
{
public:
	std::vector<BuddyRewardStruct> Unknown_49F300(int points_spent, int points);	// 0x49F300 (SClient 0x751460)  // NAME 1 [points_spent, points]: what 0x751460 passes
};
Unknown49F200Class *Unknown_49F200();					// 0x49F200 (no this: the rewards' instance)

class MainViewClass;
class CameraClass;
class SceneClass;
class LandClass;
class SimpleSceneClass;
class DazzleLayerClass;
class LightClass;
class StarFieldClass;
class OrbitalManager;
class Vector4;
class MainViewOrbitalCallbackClass;
class MainViewUnknown84Class;
class MainViewDazzleHandlerClass;
class WWProfileIterator;
class Render2DTextClass;
class MusicManagerClass;		// NAME 3: class-from-file (D24) for audio\musicmanager.cpp (constructor 0x439E40, 0x1FC bytes)
class CameraManagerClass;		// NAME 3: class-from-file (D24) for camera_manager.cpp (constructor 0x798D20, 0x4C bytes)
class UIChatPanelClass;			// NAME 3: class-from-file (D24) for ui_chat_panel.cpp (constructor 0x561DD0, 0x268 bytes)
class UIGroupClass;			// NAME 3: class-from-file (D24) for ui_group.cpp (the main view's +0x1D8; cpacket.cpp: CLIENT_CHAT_EVENT 165)
class UILineDisplayClass;		// NAME 3: class-from-file (D24) for ui_line_display.cpp (constructor 0x5A5410, 0x100 bytes)
class PDAInterfaceClass;		// NAME 3: class-from-file (D24) for pdaint.cpp (constructor 0x693140, 0xC0 bytes)
class BridgeInterfaceClass;		// NAME 3: class-from-file (D24) for bridge_int.cpp (constructor 0x53B040, 0xC4 bytes)
class UIHotKeysClass;			// NAME 3: class-from-file (D24) for ui_hot_keys.cpp (constructor 0x59C6F0, 0x84 bytes)
class UITalkTreeSpaceClass;		// NAME 3: class-from-file (D24) for ui_talkt_space.cpp (constructor 0x64F310, 0x184 bytes)
class GalaxyMapClientClass;		// NAME 3: class-from-file (D24) for galaxy_map_client.cpp (constructor 0x7A4830, 0xCC bytes)
class UIStarmapClass;			// NAME 3: class-from-file (D24) for ui_starmap.cpp (constructor 0x7C6120, 0x14 bytes)
class DeltaPlayerClass;
class SClientUnknown12C4Class;	// NAME 1: neutral (D10): the elements of SClient +0x12C4
class SClientUnknown1334Class;	// NAME 1: neutral (D10): 0xB8 bytes, destructor 0x618A50; decal.cpp's 0x75BE00 loads the decal INIs into it
class SClientUnknown1338Class;	// NAME 1: neutral (D10): 0x24 bytes, an INIClass at +0x14; env_map_manager.cpp's 0x760360 loads 'envmaps.ini' into it
class SClientUnknown12BCClass;	// NAME 1: neutral (D10): what SClient +0x12BC points to (a virtual destructor)
class Unknown6D47C0Class;		// NAME 1: neutral (D10): the 0x4E8-byte text object CLIENT_DAMAGE 100 makes (constructor 0x6D47C0; cpacket.cpp)
class SClientUnknown1330Class;	// NAME 1: neutral (D10): what SClient +0x1330 points to (destructor 0x43C560, musicmanager.cpp's neighbourhood)
class ProjDecalManagerClass;		// NAME 3: class-from-file (D24) for proj_decal_manager.cpp (0x763050 loads 'proj_dec.ini' into it; 0x10 bytes, one vector)
class SClientUnknown1394Class;	// NAME 1: neutral (D10): the values of SClient +0x1394 (reference counted, slot +0x38)

// fontdatabase.h's FontStringFilesClass at 0xBE67BC, as far as sclient.cpp and loadingview.cpp call it (fontdatabase.h's
// own 0xBE67B8 clashes with tsk_game.h's, so they can't include it; one declaration, so the harness stand-ins agree)
class SClientFontDatabaseClass	// NAME 1: neutral (D10): fontdatabase.cpp's class
{
public:
	void Unknown_6C52B0(int arg);						// 0x6C52B0 (SClient::Init passes 0)
	void Unknown_6C55B0(const char *name);				// 0x6C55B0 (Unload: the loading view's 'fc_map_load00.ini')  // NAME 1 [name]: what it gets
};
extern SClientFontDatabaseClass *Unknown_BE67BC;		// NAME 1: neutral (D10)
class AvatarDescClass;				// avatar_desc.h
struct GuildRankNameStruct;			// pkguildranknamesrequestclient.h

// verbmanager.cpp's verb manager (0x14 bytes): SClient::Init creates it, the verb packets update it
class VerbManagerClass		// NAME 3: class-from-file (D24) for verbmanager.cpp (constructor 0x81F100, 0x14 bytes)
{
public:
	VerbManagerClass(SClient *client);					// 0x81F100
	~VerbManagerClass();								// 0x81F1E0
	void Unknown_81F810(int game_id, void *adds, void *drops);	// 0x81F810 (cpacket.cpp: VERB_UPDATE 92 passes its GameID, &Adds, &Drops: +0x20 then +0x10)  // NAME 1 [game_id, adds, drops]: the packet's fields
	void Unknown_81F780(int game_id, void *verbs);	// 0x81F780 (cpacket.cpp: VERB_LIST 91 passes its GameID, &Verbs)  // NAME 1 [game_id, verbs]: the packet's fields

	char Unknown00[0x14];
};

// the interface objects: intrface.h's InterfaceClass (P-LD4-1); view.cpp's list holds them
class UIViewClass;

// what every view holds at +0x34 (0x1F8 bytes, constructor 0x5875C0), as far as cpacket.cpp and uiview.cpp call it
class ViewUnknown34Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	ViewUnknown34Class(UIViewClass *view, const LayerClass &layer);	// 0x5875C0 (0x1F8 bytes; UIViewClass's Init 0x65A820 makes it over a layer of its +0x1C scene and +0x20 camera)  // NAME 1 [view, layer]: what 0x65A820 passes
	void Unknown_588870();								// 0x588870 (UIViewClass's +0x14 0x65B470)
	void Unknown_588A70(const char *type, int unknown1, const char *text, int flags, int unknown2, int unknown3);	// 0x588A70 (102 callers; GROUP 30, DIALOG 98: "GROUP_INVITE", "OK", "YES_NO", "OK_CANCEL", "server_message")  // NAME 1 [type, unknown1, text, flags, unknown2, unknown3]: neutral, the handlers' arguments
	int Unknown_5890B0(const char *button);				// 0x5890B0 (SClient::Do_Frame passes NULL after 'Lost connection to server.'; tsk_login.h: the button pressed)  // NAME 2 [button]: tsk_login.h's
	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58();
	virtual void Unknown_5C();							// +0x5C (SClient::Do_Frame: the main view's, once the lost-connection box is answered)

	char Unknown84[0x18C - 0x84];						// +0x84
	char Type[4];										// +0x18C the dialog's type, a string (0x74F5C0 compares it; its length not known)  // NAME 2: what 0x74F5C0 compares
	char Unknown190[0x1F8 - 0x190];						// +0x190
};

// view.cpp's view (0x14 bytes; constructor 0x6B4480, destructor 0x6B45D0): UIViewClass's base, as far as uiview.cpp
// calls it
class UIViewClass;
class ViewClass			// NAME 3: class-from-file (D24) for view.cpp
{
public:
	ViewClass();										// 0x6B4480 (table 0xAFDFA4)
	virtual ~ViewClass();								// +0x00 0x6B45D0 (G_ASSERT '!InterfaceObjects.Head()'; deleting 0x6B45A0)
	virtual void Unknown_04() = 0;						// +0x04
	virtual void Unknown_08() {}						// +0x08 0x6B4540
	virtual int Unknown_0C(bool on) = 0;				// +0x0C
	virtual void Unknown_10();							// +0x10 0x6B46F0 (every interface object's +0x7C with 0, 0; G_ASSERT '!node')
	virtual void Unknown_14();							// +0x14 0x6B4860 (every interface object's +0x64, or +0x60 under a sort object when its +0x7C is set)
	virtual void Unknown_18(int arg) {}					// +0x18 0x514CF0  // NAME 1 [arg]: neutral
	virtual UIViewClass *Unknown_1C() { return(NULL); }	// +0x1C 0x6B4560 (UIViewClass's 0x50B720 returns the view)
	virtual UIViewClass *Unknown_20() { return(NULL); }	// +0x20 0x514D10 (Unknown65B810ViewClass's 0x50B7E0 returns the view)
	virtual UIViewClass *Unknown_24() { return(NULL); }	// +0x24 0x514D30 (the main view's 0x50B800 returns the view)
	virtual int Unknown_28() { return(0); }				// +0x28 0x50B680
	virtual UIViewClass *Unknown_2C() { return(NULL); }	// +0x2C 0x50B6A0 (Unknown514CC0ViewClass's 0x514D90 returns the view)
	virtual int Unknown_30() { return(0); }				// +0x30 0x50B6C0
	virtual int Unknown_34() { return(0); }				// +0x34 0x50B6E0
	virtual int Unknown_38() { return(0); }				// +0x38 0x50B700
	virtual int Unknown_3C() { return(0); }				// +0x3C 0x6B4580 (UIViewClass's 0x50B7C0)
	void Unknown_6B4900(INIClass &ini);					// 0x6B4900 (every interface object's +0x6C; SClient 0x745080 passes settings.ini)  // NAME 1 [ini]: what 0x745080 passes
	void Unknown_6B4940(InterfaceClass *inter);	// 0x6B4940 (FAIL_IF '!inter': onto InterfaceObjects, referenced)  // NAME 2 [inter]: the FAIL_IF's
	bool Unknown_6B4990(InterfaceClass *inter);	// 0x6B4990 (FAIL_IF '!inter': off InterfaceObjects, released)  // NAME 2 [inter]: the FAIL_IF's

	LoginUnknown0CClass *Get_Unknown04() { return(Unknown04); }	// inline (Do_Frame reads it into eax first)  // NAME 1: neutral (D10): the member it returns

	LoginUnknown0CClass *Unknown04;						// +0x04 (SClient::Do_Frame: the current view's input dispatcher gets the event the client's +0x1270 didn't take, 0x429A20)
	SList<InterfaceClass> InterfaceObjects;		// +0x08 (the destructor's G_ASSERT '!InterfaceObjects.Head()')  // NAME 3: the assert's
};

class GadgetListManagerClass;
class UIViewUnknown30Class;
class CameraEffectsClass;

// uiview.cpp's view (0x4C bytes): the base of the six views at SClient +0x1354..+0x1368 (the slots every one of
// them gets: the deleting destructor, +0x04 after construction, +0x10 in Shutdown)
class UIViewClass : public ViewClass	// NAME 5: the client's profile label 'UIViewClass::Render' (0x65B330, uiview.cpp's __FILE__)
{
public:
	UIViewClass(ClientTaskClass *task, int arg);			// 0x65A6C0 (table 0xAF97E4; the task to +0x14, arg to +0x18; 0x514CC0's view passes 1, 0x65B810 inlines it with 0)  // NAME 1 [task, arg]: what the callers pass
	virtual ~UIViewClass();								// +0x00 0x65A760 (Shutdown deletes through it; the main view's destructor calls it)
	virtual void Unknown_04();							// +0x04 0x65A820 (Init calls it after construction: the scenes, the cameras, the interface)
	virtual void Unknown_08() {}						// +0x08 0x65A710 (Init: the main view's, +0x1358's)
	virtual int Unknown_0C(bool on);					// +0x0C 0x65B330 (the base's: UIViewClass::Render) Process_Packets passes true to the loading view (the main view's renders and returns a WW3DErrorType)
	virtual void Unknown_10();							// +0x10 0x65B4A0 Shutdown (the main view's 0x50C2F0 calls this one, past 0x65B810's 0x65B8B0)
	virtual void Unknown_14();							// +0x14 0x65B470 SClient::Do_Frame: the current view's, after Process_AI
	virtual UIViewClass *Unknown_1C() { return(this); }	// +0x1C 0x50B720
	virtual int Unknown_3C() { return((int)Unknown24); }	// +0x3C 0x50B7C0
	virtual void Unknown_40();							// +0x40 0x65AB10 (the cameras by the view's kind +0x18, then the view plane 0x65B570)
	virtual int Unknown_44() { return(0); }				// +0x44 0x514D50 (Unknown65B810ViewClass's 0x65B8D0)
	virtual int Unknown_48() { return((int)Unknown24); }	// +0x48 0x50B740 (CLIENT_DAMAGE 100 hands its result to Add_Effect)
	virtual int Unknown_4C() { return((int)Unknown28); }	// +0x4C 0x50B760
	virtual int Unknown_50() { return((int)Unknown1C); }	// +0x50 0x50B780 (the main view's 0x50BA60 holds its result in a RefCountPtr)
	virtual int Unknown_54() { return((int)Unknown20.Peek()); }	// +0x54 0x50B7A0
	virtual CameraClass *Unknown_58() { return(NULL); }	// +0x58 0x514D70

	bool Unknown_65B570();								// 0x65B570 (the view plane from the +0x54 camera, scaled by its z; inlined in 0x65AB10)
	void Unknown_65B610(Vector2 &min, Vector2 &max);	// 0x65B610 (the view plane)  // NAME 1 [min, max]: CameraClass::Get_View_Plane's
	void Unknown_65B650(const Vector2 &norm, Vector2 *coords);	// 0x65B650 (FAIL_IF '!coords': 0..1 to the view plane)  // NAME 2 [coords]: the FAIL_IF's; NAME 1 [norm]: its counterpart in 0x65B6C0
	void Unknown_65B6C0(const Vector2 &coords, Vector2 *norm);	// 0x65B6C0 (FAIL_IF '!norm', 'width == 0.0f', 'height == 0.0f': the view plane to 0..1)  // NAME 2 [norm]: the FAIL_IF's; NAME 1 [coords]: its counterpart in 0x65B650
	bool Unknown_65B7A0(int a, int b, int c, int d);	// 0x65B7A0 (+0x1C's 0x6DA6F0)  // NAME 1 [a, b, c, d]: neutral
	bool Unknown_65B7E0(int a);							// 0x65B7E0 (+0x1C's 0x6DA730)  // NAME 1 [a]: neutral
	ViewUnknown34Class *Get_Unknown34() { return(Unknown34); }	// inline (GROUP 30 and DIALOG 98 read it into eax first)  // NAME 1: neutral (D10): the member it returns

	ClientTaskClass *Unknown14;							// +0x14 (the constructor's task)
	int Unknown18;										// +0x18 (the constructor's arg: the view's kind, 0..6, that 0x65AB10 sets the cameras by)
	SimpleSceneClass *Unknown1C;						// +0x1C (slot +0x50; the main view's 0x50CC10 checks its visibility with +0x20)
	RefCountPtr<CameraClass> Unknown20;					// +0x20 (slot +0x54; 0x65A820 assigns it through RefCountPtr, 0x65A760 releases it as a member: a RefCountPtr, as 0x514E70's Peek shows)
	SimpleSceneClass *Unknown24;						// +0x24 (slots +0x3C, +0x48; checked with +0x28)
	CameraClass *Unknown28;								// +0x28 (slot +0x4C)
	GadgetListManagerClass *Unknown2C;					// +0x2C (Init 0x65A820 makes it, 0x4C bytes; the destructor deletes it)
	UIViewUnknown30Class *Unknown30;					// +0x30 (Init makes it, 0x70 bytes; Shutdown deletes it)
	ViewUnknown34Class *Unknown34;						// +0x34 (uiview.cpp's constructors 0x65A6C0 / 0x65B810 set it; cpacket.cpp's GROUP 30 and DIALOG 98 call it)
	Vector2 Unknown38;									// +0x38 the view plane's min (0x65B570: the camera's, scaled by its z)
	Vector2 Unknown40;									// +0x40 the view plane's max
	CameraEffectsClass *CameraEffects;					// +0x48 (the constructors clear it; the main view's constructor makes it)  // NAME 3: the scripted movie's G_ASSERT 'CameraEffects' (0x808220)
};

// uiview.cpp's second view (constructor 0x65B810, table 0xAF9854): the base of the main, loading and 3-D movie views
// (0x50B360, 0x691570, 0x7EEF90 and 0x69AD30 call 0x65B810); its destructor is implicit: the main view's object holds
// its out-of-line copy 0x50B930 (jmp 0x65A760, called by 0x50B360's unwind and by 0x65B880, this class's deleting
// destructor), and the main view's destructor 0x50B950 inlines it
class Unknown65B810ViewClass : public UIViewClass	// NAME 1: neutral (D10)
{
public:
	Unknown65B810ViewClass(ClientTaskClass *task, int arg);	// 0x65B810 (the base with 0; the task's Client (+0x78) to +0x4C; arg unused)  // NAME 1 [task, arg]: what the main view passes
	virtual void Unknown_10();							// +0x10 0x65B8B0 (+0x4C cleared, then the base's 0x65B4A0)
	virtual UIViewClass *Unknown_20() { return(this); }	// +0x20 0x50B7E0
	virtual int Unknown_44();							// +0x44 0x65B8D0 (+0x4C's +0x1274, 0 without one)

	SClient *Get_Client() { return(Client); }			// (inline: the main view's 0x514010 G_ASSERT 'Get_Client()->Get_Mode() == newmode')  // NAME 3: the assert's
	SClient *Client;									// +0x4C 0x65B810 copies the task's Client (+0x78); the main view's 0x5137F0 asks its slot +0x18 for the SClient  // NAME 2: what it holds
};

// the interface object of the view at SClient +0x1358 (0xAC bytes, constructor 0x5B7AB0; file unknown)
class Unknown5B7AB0Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	Unknown5B7AB0Class(UIViewClass *view, const LayerClass &layer);	// 0x5B7AB0 (0x514E70 passes its view and a layer of the base's +0x1C / +0x20)  // NAME 1 [view, layer]: what 0x514E70 passes

	char Unknown84[0xAC - 0x84];						// +0x84
};

// The 0x50-byte view at SClient +0x1358 (SClient::Init; constructor 0x514CC0, table 0xAED114): its functions follow
// mainview.cpp's, with no static initializer of their own before them, so they are kept at mainview.cpp's end
class Unknown514CC0ViewClass : public UIViewClass	// NAME 1: neutral (D10)
{
public:
	Unknown514CC0ViewClass(ClientTaskClass *task);		// 0x514CC0 (the base with 1)  // NAME 1 [task]: what SClient::Init passes
	virtual ~Unknown514CC0ViewClass();					// +0x00 0x514DE0 (deleting 0x514DB0)
	virtual void Unknown_08();							// +0x08 0x514E70 (its interface object, on through +0x54, to 0x6B4940)
	virtual UIViewClass *Unknown_2C() { return(this); }	// +0x2C 0x514D90

	Unknown5B7AB0Class *Unknown4C;						// +0x4C (0x514E70 makes it; the destructor releases it)
};

// radar_system.cpp's radar (the main view's +0x80), declared as far as cpacket.cpp calls it
class RadarSystemClass	// NAME 3: class-from-file (D24) for radar_system.cpp
{
public:
	RadarSystemClass(SClient *client);					// 0x7BC770 (0x50 bytes; the main view's 0x50BA60)  // NAME 1 [client]: what 0x50BA60 passes
	~RadarSystemClass();								// 0x7BC930 (the main view's Shutdown 0x50C2F0)
	void Unknown_7BD920(int index);						// 0x7BD920 (cpacket.cpp: WARP_INDEX 156)  // NAME 1 [index]: the packet's Index
	void Unknown_7BD6E0(ClientObjectClass *target);	// 0x7BD6E0 (the main view's 0x513630 / 0x512400: the target)  // NAME 1 [target]: what 0x512400 passes
	void Unknown_7BCA40(int arg);						// 0x7BCA40 (the main view's slot +0x18)  // NAME 1 [arg]: neutral
	void Unknown_7BCA70(MainViewUnknown84Class *object, PDAInterfaceUnknownA0Class *arg, const RefCountPtr<RefCountClass> &scene);	// 0x7BCA70 (the main view's 0x512A00, every frame; the scene's RefCountPtr: VC6 binds Create_Peek(SceneClass *) to 0x50BA60's RefCountPtr<RefCountClass>, and a named RefCountPtr<SceneClass> adds a frame slot the client lacks)  // NAME 1 [object, arg, scene]: what 0x512A00 passes

	char Unknown00[0x4D]; bool Unknown4D; char Unknown4E[0x50 - 0x4E];	// +0x4D: names of the objects under the mouse ('/art name', 0x74ABC0)
};

// what the PDA's +0xA8 points to, as far as the main view's 0x512A00 calls it
class PDAInterfaceUnknownA8Class	// NAME 1: neutral (D10)
{
public:
	PDAInterfaceUnknownA0Class *Unknown_7C8DB0(bool on);	// 0x7C8DB0 (its +0xA0, or NULL when its +0x7C is set and +0xA0's 0x693040 says no)  // NAME 1 [on]: what 0x512A00 passes
};

// pdaint.cpp's PDA (SClient +0x1284; Init news it, 0xC0 bytes), as far as SClient and the main view's 0x512A00 use it
class PDAInterfaceClass : public InterfaceClass
{
public:
	PDAInterfaceClass(SClient *client);					// 0x693140

	char Unknown84[0xA8 - 0x84];
	PDAInterfaceUnknownA8Class *UnknownA8;				// +0xA8 (0x512A00 calls its 0x7C8DB0)
	char UnknownAC[0xC0 - 0xAC];
};

// ui_scan.cpp's scanner (the cockpit's +0xD4), as far as the main view calls it
class UIScanClass		// NAME 3: class-from-file (D24) for ui_scan.cpp
{
public:
	void Unknown_60E540(ClientObjectClass *target, const std::string &assessment, const std::string &sound, int level);	// 0x60E540 (the main view's 0x513630)  // NAME 1 [target, assessment, sound, level]: what the main view's 0x512400 passes (TargetThreat, TargetThreatSound, TargetThreatLevel)

	char Unknown00[0x88];								// +0x00
	ClientObjectClass *Unknown88;						// +0x88 (the main view's slot +0x5C returns it)
};

class IncapacitanceDetailsPacket;
// ui_cpit.cpp's cockpit interface (the main view's +0x1D0), declared as far as cpacket.cpp calls it
class UICpitClass : public InterfaceClass	// NAME 3: class-from-file (D24) for ui_cpit.cpp
{
public:
	UICpitClass(MainViewClass *view, const LayerClass &layer);	// 0x57D830 (0x178 bytes; the main view's 0x50C690)  // NAME 1 [view, layer]: what 0x50C690 passes
	void Unknown_581F90(ClientObjectClass *hulk);		// 0x581F90 (cpacket.cpp: LOOT_HULK_PERMISSION 140)  // NAME 1 [hulk]: the packet's HulkGameID's object
	void Unknown_5820C0(bool on);						// 0x5820C0 (cpacket.cpp: OPEN_INTERFACE 102 passes UIChange == 0)  // NAME 1 [on]: what the handler passes
	void Unknown_584A60();								// 0x584A60 (SClient 0x745160)
	void Unknown_584690(IncapacitanceDetailsPacket *packet);	// 0x584690 (cpacket.cpp: INCAPACITANCE_DETAILS 144)  // NAME 1 [packet]: the handler's packet
	void Unknown_583DC0(Unknown6D47C0Class *text, Unknown6D47C0Class *header, int inflicted, bool emp);	// 0x583DC0 (cpacket.cpp: CLIENT_DAMAGE 100 for the player's own damage)  // NAME 1 [text, header, inflicted, emp]: what the handler passes

	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54();
	virtual void Unknown_58();							// +0x58 (the main view's 0x5141A0)
	virtual void Unknown_5C();							// +0x5C (the main view's 0x5141E0 / 0x514220)

	char Unknown84[0xD4 - 0x84];						// +0x84
	UIScanClass *Scan;									// +0xD4 (the main view's 0x5136A0 returns it, 0x513630 calls 0x60E540 on it)  // NAME 3: its file's (ui_scan.cpp)
	char UnknownD8[0xDC - 0xD8];						// +0xD8
	int UnknownDC;										// +0xDC (the main view's 0x514400 returns it)
	char UnknownE0[0x178 - 0xE0];						// +0xE0
};

// what the main view's +0x13C points to, as far as SERVER_PARAMETERS 66 reads it
class MainViewUnknown13CClass	// NAME 1: neutral (D10)
{
public:
	int Get_Unknown90() { return(Unknown90); }		// (inline: SERVER_PARAMETERS 66 reads it through eax)  // NAME 1: neutral (D10): the member it returns

	char Unknown00[0x90];								// +0x00
	int Unknown90;										// +0x90
};

class MainViewClass;
class INIClass;

// orbitalmanager.cpp's manager (the main view's +0x140, 0x1D4 bytes), as far as the main view reads it
class OrbitalManager		// NAME 2: the client's (OrbitalManager::Update)
{
public:
	~OrbitalManager();									// 0x4F4FD0 (the main view's Shutdown 0x50C2F0)
	OrbitalManager(SClient *client, MainViewOrbitalCallbackClass *callback);	// 0x4F3250 (0x1D4 bytes; the main view's 0x50BA60)  // NAME 1 [client, callback]: what 0x50BA60 passes
	void Unknown_4F3310();								// 0x4F3310 (the main view's 0x513700, before the sector's orbital%04d.ini is read)
	void Unknown_4F34C0(INIClass &ini);				// 0x4F34C0 (0x513700: orbital%04d.ini when it loaded)  // NAME 1 [ini]: what it gets
	float Get_Unknown54() const { return(Unknown54); }	// (inline: the main view's 0x50CCF0 copies it through the FPU)  // NAME 1: neutral (D10): the member it returns

	void Update(CameraClass *camera, float time, bool planet);	// 0x4F45A0 (the main view's Update unless its +0x168 is set: the hours, view mode 1)  // NAME 1 [camera, time, planet]: what Update passes
	const Vector4 &Unknown_4F50D0(int index);			// 0x4F50D0 (Update, view mode 0: light index's colour and intensity, 4 the fill)  // NAME 1 [index]: neutral
	const Vector3 &Unknown_4F5140(int index);			// 0x4F5140 (Update, view mode 0: light index's offset)  // NAME 1 [index]: neutral

	char Unknown00[0x14];								// +0x00
	Vector3 Unknown14;									// +0x14 (Update: the sun's colour, the land's LightColor)
	Vector3 Unknown20;									// +0x20 (Update: the sun's direction, the land's SunDirection)
	float Unknown2C;									// +0x2C (Update: the land's SunAngle)
	float Unknown30;									// +0x30 (Update: the land's ShadowBias)
	Vector3 Unknown34;									// +0x34 (Update: the fog colour)
	char Unknown40[0x4C - 0x40];						// +0x40
	bool Unknown4C;										// +0x4C (Update: the sun light is added; the land's Valid)
	float Unknown50;									// +0x50 (the main view's 0x50CCF0: the near plane at a unit FOV)
	float Unknown54;									// +0x54 (0x50CCF0: the far plane)
	Vector3 Unknown58;									// +0x58 (Update: the ambient light)
	char Unknown64[0x1D4 - 0x64];						// +0x64
};

// what the main view's scene (0x75ECF0) holds at +0x98, as far as Update calls it
class MainViewSceneUnknown98Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_493D00(const Vector3 &dir);			// 0x493D00 (Update: minus the sun's direction)  // NAME 1 [dir]: what Update passes
};

// what the main view's +0x48 points to (0x28 bytes, constructor 0x6E2BD0; slot +0x14 calls its +0x34)
class CameraEffectsClass	// NAME 3: class-from-file (D24) for cameraeffects.cpp (0x6E32E0's; the member's name CameraEffects)
{
public:
	CameraEffectsClass(MainViewClass *view, SClient *client);	// 0x6E2BD0  // NAME 1 [view, client]: what the main view passes
	virtual ~CameraEffectsClass();					// +0x00 (deleting; the main view's Shutdown 0x50C2F0)
	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();							// +0x34 (the main view's slot +0x14)
	void Unknown_6E32E0(int type, float amount, float time);	// 0x6E32E0 (the scripted movie's Shake_Camera passes 3)  // NAME 1 [type, amount, time]: what Shake_Camera passes

	char Unknown04[0x28 - 0x04];
};

// what the main view's +0x1D4 points to (0x50C690 makes it with a layer of the base's +0x1C scene and +0x20 camera)
class MainViewUnknown1D4Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	MainViewUnknown1D4Class(MainViewClass *view, const LayerClass &layer);	// 0x5608D0 (0xEC bytes)  // NAME 1 [view, layer]: what 0x50C690 passes
	char Unknown84[0xEC - 0x84];						// +0x84
};

// what the main view's +0x1E4 points to (0x50C690 makes it for the client, then its slot +0x7C with the view)
class MainViewUnknown1E4Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	MainViewUnknown1E4Class(SClient *client);			// 0x5E1BE0 (0xE8 bytes)  // NAME 1 [client]: what 0x50C690 passes
	virtual void Unknown_60(); virtual void Unknown_64(); virtual bool Unknown_68(int key); virtual void Unknown_6C(INIClass &ini);
	virtual void Unknown_70(ControlKeysManagerClass *manager); virtual void Unknown_74(ControlKeysManagerClass *manager); virtual bool Unknown_78(void *arg);
	virtual void Unknown_7C(UIViewClass *view, int arg);	// +0x7C (0x50C690: the view, 0)  // NAME 1 [view, arg]: what 0x50C690 passes
	char Unknown84[0xE8 - 0x84];						// +0x84
};

// the object 0x50C690 makes, registers and releases at once (0x90 bytes, with the base's +0x24 scene)
class MainViewUnknown53E970Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	MainViewUnknown53E970Class(MainViewClass *view, SimpleSceneClass *scene);	// 0x53E970  // NAME 1 [view, scene]: what 0x50C690 passes
	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50();
	virtual void Unknown_54();
	virtual void Unknown_58();
	char Unknown84[0x90 - 0x84];						// +0x84
};

// what the main view's +0x1E0 points to (Shutdown 0x50C2F0 releases it)
class MainViewUnknown1E0Class : public InterfaceClass	// NAME 1: neutral (D10)
{
};

// what the main view's +0x1DC points to, as far as 0x513540 / 0x513600 call it
class MainViewUnknown1DCClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual bool Unknown_68(int key); virtual void Unknown_6C(INIClass &ini);
	virtual void Unknown_70(ControlKeysManagerClass *manager); virtual void Unknown_74(ControlKeysManagerClass *manager); virtual bool Unknown_78(void *arg);
	virtual void Unknown_80(); virtual void Unknown_84();
	virtual void Unknown_88(RenderObjClass *model);	// +0x88 (0x513540: the backdrop model)  // NAME 2 [model]: what 0x513540 passes
	virtual void Unknown_8C(int arg);					// +0x8C (0x513600)  // NAME 1 [arg]: neutral
	MainViewUnknown1DCClass(MainViewClass *view, SimpleSceneClass *scene);	// 0x53AA30 (0x8C bytes; 0x50C690 passes the +0x110 scene)  // NAME 1 [view, scene]: what 0x50C690 passes
	char Unknown84[0x8C - 0x84];						// +0x84
};

// what the main view's +0x138 points to (0x53DFB0-0x53E3F0: the main view forwards to it)
class MainViewUnknown138Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	MainViewUnknown138Class(MainViewClass *view, SimpleSceneClass *scene, INIClass &ini);	// 0x53D250 (0xA4 bytes; 0x50C690: the +0x10C scene, settings.ini)  // NAME 1 [view, scene, ini]: what 0x50C690 passes
	void Unknown_53DFB0(int mode);						// 0x53DFB0  // NAME 1 [mode]: the main view's
	void Unknown_53E030(float value);					// 0x53E030  // NAME 1 [value]: the main view's
	void Unknown_53E470(const Vector3 &offset);			// 0x53E470 (the main view's Update, view mode 5: the hours times the wind)  // NAME 1 [offset]: neutral
	void Unknown_53E0B0();								// 0x53E0B0
	void Unknown_53E0E0();								// 0x53E0E0
	void Unknown_53E110(int arg);						// 0x53E110  // NAME 1 [arg]: neutral
	void Unknown_53E160();								// 0x53E160
	void Unknown_53E190(int arg);						// 0x53E190  // NAME 1 [arg]: neutral
	void Unknown_53E1E0(int arg);						// 0x53E1E0  // NAME 1 [arg]: neutral
	void Unknown_53E220(int arg);						// 0x53E220  // NAME 1 [arg]: neutral
	void Unknown_53E260();								// 0x53E260
	void Unknown_53E290(int arg);						// 0x53E290  // NAME 1 [arg]: neutral
	void Unknown_53E2C0();								// 0x53E2C0
	void Unknown_53E2F0(int arg);						// 0x53E2F0  // NAME 1 [arg]: neutral
	void Unknown_53E3C0();								// 0x53E3C0
	void Unknown_53E3F0(int arg);						// 0x53E3F0  // NAME 1 [arg]: neutral

	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54();
	virtual void Unknown_58();							// +0x58 (the main view's 0x514010: on)
	virtual void Unknown_5C();							// +0x5C (0x514010: off)

	char Unknown84[0x9C - 0x84];						// +0x84 (+0x7C Active: 0x514010 tests it)
	int Unknown9C;										// +0x9C (the main view's 0x513D00 returns it)
	char UnknownA0[0xA4 - 0xA0];						// +0xA0
};

// mainview.cpp's view (constructor 0x50B360, 0x1EC bytes): SClient's Add_Object / Remove_Object tell it
class MainViewClass : public Unknown65B810ViewClass	// NAME 3: class-from-file (D24) for mainview.cpp
{
public:
	MainViewClass(ClientTaskClass *ctask);				// 0x50B360 (G_ASSERT 'ctask', l.176)  // NAME 2 [ctask]: the assert's
	virtual ~MainViewClass();							// +0x00 0x50B950 (deleting 0x50B900)
	virtual void Unknown_04();							// +0x04 0x50BA60 (the scenes, the dazzle layers, the radar, the orbitals, the lights, the stars)
	virtual void Unknown_08();							// +0x08 0x50C690 (settings.ini, the interface)
	virtual int Unknown_0C(bool on);					// +0x0C 0x50CAE0 (the per-frame render: Render_Initializing / Gas_Giant / Planet / Space_Mode)
	virtual void Unknown_10();							// +0x10 0x50C2F0 (Shutdown: the counterpart of +0x04)
	virtual void Unknown_14();							// +0x14 0x5129D0 (0x512400, then the base's 0x65B470)
	virtual void Unknown_18(int arg);					// +0x18 0x514930 (hands it to the radar)  // NAME 1 [arg]: neutral
	virtual UIViewClass *Unknown_24() { return(this); }	// +0x24 0x50B800
	virtual CameraClass *Unknown_58() { return(Camera); }	// +0x58 0x50B8C0
	virtual ClientObjectClass *Unknown_5C();			// +0x5C 0x5136D0 (the scanner's +0x88; SClient 0x744E70's FAIL_IF '!target' on its result)
	virtual ClientObjectClass *Unknown_60(float x, float y, bool skip_avatar, int arg4);	// +0x60 0x512BB0 (the object under a screen point; arg4 unused) (asserts 'cam_dist_idx < InfoCount', 'cos_occluder_size > 0.0f')  // NAME 1 [x, y, skip_avatar, arg4]: what the body does with them
	virtual SimpleSceneClass *Unknown_64();			// +0x64 0x514430 (Scene; SClient 0x7456B0 calls its slot +0x24, SceneClass::Set_Fog_Enable)
	virtual SimpleSceneClass *Unknown_68() { return(Unknown110); }	// +0x68 0x50B820
	virtual SimpleSceneClass *Unknown_6C() { return(Unknown114); }	// +0x6C 0x50B840
	virtual SimpleSceneClass *Unknown_70() { return(Unknown118); }	// +0x70 0x50B860
	virtual SimpleSceneClass *Unknown_74() { return(Unknown11C); }	// +0x74 0x50B880
	virtual SimpleSceneClass *Unknown_78() { return(Unknown120); }	// +0x78 0x50B8A0
	virtual int Unknown_7C() { return((int)Camera); }	// +0x7C 0x50B8E0 (OBJECT_EFFECT 9 hands its result to Add_Effect)
	virtual void Add_Object(ClientObjectClass *object);		// +0x80 0x514270 SClient::Add_Object  // NAME 2: its caller's
	virtual void Remove_Object(ClientObjectClass *object);	// +0x84 0x514390 SClient::Remove_Object  // NAME 2: its caller's

	float Unknown_50CC10();								// 0x50CC10 (the visibility checks; returns PredictiveLODOptimizerClass's total cost)
	void Unknown_50CCF0(float *znear, float *zfar);	// 0x50CCF0 (FAIL_IF '!znear' l.447, '!zfar' l.448: the clip planes from the FOV and the orbitals)  // NAME 2 [znear, zfar]: the FAIL_IFs'
	void Unknown_50CE00(float znear, float zfar);		// 0x50CE00 (renders +0x11C)  // NAME 2 [znear, zfar]: Set_Clip_Planes's
	void Unknown_50CF70();								// 0x50CF70 (0x510D40 with +0x16C)
	void Unknown_50D060();								// 0x50D060 (renders +0x110)
	void Unknown_50D1E0();								// 0x50D1E0 (renders +0x114 when +0x130 is set)
	void Unknown_50D310(float znear, float zfar);		// 0x50D310 (renders the +0x10C scene)  // NAME 2 [znear, zfar]: Set_Clip_Planes's
	void Unknown_50D430(float znear, float zfar);		// 0x50D430 (renders the dazzle layer)  // NAME 2 [znear, zfar]: Set_Clip_Planes's
	void Unknown_50D520();								// 0x50D520 (the 2-D overlays, 'PB=%d SB=%d LB=%d LOD=%d')
	void Unknown_510440();								// 0x510440 (0x50D520 with the options' +0x05: the profile text)
	void Unknown_5103F0(int index);						// 0x5103F0 (the profile iterator into child index unless it is -1; no caller found)  // NAME 1 [index]: Enter_Child's
	void Unknown_510420();								// 0x510420 (the profile iterator to its parent; no caller found)
	WW3DErrorType Render_Initializing();				// 0x50DA00  // NAME 2: its profile label 'MainView::Render_Initializing'
	WW3DErrorType Render_Gas_Giant_Mode(float znear, float zfar);	// 0x50DB20  // NAME 2: its profile label
	WW3DErrorType Render_Space_Mode(float znear, float zfar);	// 0x50E090  // NAME 2: its profile label
	WW3DErrorType Render_Planet_Mode(float znear, float zfar);	// 0x50E6E0  // NAME 2: its profile label
	void Update(CameraClass *camera, bool under);		// 0x50F1B0 (the render modes: Render_Planet_Mode passes the camera under the sea level)  // NAME 2: its profile label 'MainView::Update'; NAME 1 [camera, under]: what the callers pass
	void Unknown_510D40(Vector3 &arg);					// 0x510D40 (0x50CF70 passes +0x16C; dynamic vertex and index buffers)  // NAME 1 [arg]: neutral
	void Set_Camera(CameraClass *camera);				// 0x5123C0: Init's camera  // NAME 2: what Init hands it
	void Unknown_5141A0();								// 0x5141A0 (Init, Unknown137C clear)
	void Unknown_5141E0();								// 0x5141E0
	void Unknown_514220();								// 0x514220 (Init, Unknown137C set)
	void Unknown_512400();								// 0x512400 (slot +0x14: TargetGameID / TargetThreat...)
	void Unknown_512A00();								// 0x512A00 (SClient::Do_Frame, when the main view is the current view)
	MainViewUnknown13CClass *Get_Unknown13C() { return((MainViewUnknown13CClass *)Land); }	// (inline: SERVER_PARAMETERS 66)  // NAME 1: neutral (D10): the member it returns
	void Unknown_513540(int base_asset);				// 0x513540 (SClient 0x745000)  // NAME 1 [base_asset]: SERVER_PARAMETERS 66's BackdropBaseAsset
	void Unknown_513600(int arg);						// 0x513600 (SClient 0x7456B0 passes 0)  // NAME 1 [arg]: neutral
	void Unknown_513630(ClientObjectClass *target, const std::string &assessment, const std::string &sound, int level);	// 0x513630 (the radar's 0x7BD6E0, the scanner's 0x60E540)  // NAME 1 [target, assessment, sound, level]: the scanner's
	UIScanClass *Unknown_5136A0();						// 0x5136A0 (the cockpit's scanner)
	void Unknown_513700(unsigned sector);				// 0x513700 (cpacket.cpp: SERVER_PARAMETERS 66)  // NAME 1 [sector]: the packet's SectorNum
	void Unknown_5137F0(unsigned sector);				// 0x5137F0 (cpacket.cpp: SERVER_PARAMETERS 66, view mode 1)  // NAME 1 [sector]: the packet's SectorNum
	void Unknown_513B90(unsigned sector);				// 0x513B90 (cpacket.cpp: SERVER_PARAMETERS 66, view mode 5)  // NAME 1 [sector]: the packet's SectorNum
	void Unknown_513CD0(int mode);						// 0x513CD0 (SClient 0x7456B0 passes the base's Unknown10F4, DebrisMode)  // NAME 1 [mode]: what it gets
	int Unknown_513D00();								// 0x513D00 (+0x138's +0x9C)
	void Unknown_513D20(float value);					// 0x513D20 (SClient 0x7456B0 passes 1.0)  // NAME 1 [value]: neutral
	void Unknown_513D50();								// 0x513D50
	void Unknown_513D70();								// 0x513D70
	void Unknown_513D90(int arg);						// 0x513D90  // NAME 1 [arg]: neutral
	void Unknown_513DC0();								// 0x513DC0
	void Unknown_513DE0(int arg);						// 0x513DE0  // NAME 1 [arg]: neutral
	void Unknown_513E10(int arg);						// 0x513E10  // NAME 1 [arg]: neutral
	void Unknown_513E40(int arg);						// 0x513E40  // NAME 1 [arg]: neutral
	void Unknown_513E70();								// 0x513E70
	void Unknown_513E90(int arg);						// 0x513E90  // NAME 1 [arg]: neutral
	void Unknown_513EC0();								// 0x513EC0
	void Unknown_513EE0(int arg);						// 0x513EE0  // NAME 1 [arg]: neutral
	void Unknown_513F10();								// 0x513F10
	void Unknown_513F30(int arg);						// 0x513F30  // NAME 1 [arg]: neutral
	void Unknown_513F60(FileClass *file, int indent);	// 0x513F60 (SClient 0x745EC0's dump, two deeper; empty)  // NAME 1 [file, indent]: Printf_Indented's
	void Unknown_513F80(int arg);						// 0x513F80 (empty)  // NAME 1 [arg]: neutral
	void Unknown_513FA0();								// 0x513FA0 (empty)
	void Unknown_513FC0();								// 0x513FC0 (cpacket.cpp: SERVER_HANDOFF 58: the land out of the scene)
	void Unknown_514010(int oldmode, int newmode);		// 0x514010 (SClient 0x745F30: the view mode before and after)  // NAME 3 [newmode]: its G_ASSERT's  // NAME 2 [oldmode]: newmode's counterpart
	void Unknown_512B10(int x, int y, float *fx, float *fy);	// 0x512B10 (a screen point clamped to the device and divided by its size; inlined in 0x512BB0)  // NAME 1 [x, y, fx, fy]: neutral
	void Unknown_514480();								// 0x514480 (the Graphics_* options into +0x50..+0x64, the flags +0x68 / +0x69; Doppler_Enabled)
	void Unknown_5146D0();								// 0x5146D0 (Gamma and the budgets from the options or the defaults; WW3D::Set_Gamma, Set_Texture_Reduction)
	int Unknown_514400();								// 0x514400 (the cockpit's +0xDC)
	void Unknown_514450();								// 0x514450 (sets the land's +0x136)

	bool GraphicsDefault;							// +0x50 (0x514480: the option 'Graphics_Default')  // NAME 2: its option's
	float GraphicsGamma;								// +0x54 ('Graphics_Gamma', percent)  // NAME 2: its option's
	float GraphicsLandPoly;							// +0x58 ('Graphics_Land_Poly')  // NAME 2: its option's
	float GraphicsLandDetail;						// +0x5C ('Graphics_Land_Detail')  // NAME 2: its option's
	float GraphicsSpacePoly;							// +0x60 ('Graphics_Space_Poly')  // NAME 2: its option's
	float GraphicsTextureSize;						// +0x64 ('Graphics_Texture_Size')  // NAME 2: its option's
	bool GraphicsChanged;							// +0x68 (0x514480: an option moved by more than 1; 0x50BA60 sets it)  // NAME 2: what 0x514480 records
	bool DefaultChanged;								// +0x69 (0x514480: 'Graphics_Default' changed; 0x5146D0 tests it)  // NAME 2: what 0x514480 records
	float Gamma;										// +0x6C (0x5146D0: 1.2 with the defaults, else from the option; 1..3, WW3D::Set_Gamma)  // NAME 2: what it is set to
	int LandPolyBudget;								// +0x70 (0x5146D0: the options' LandPolyBudget, 8000..40000; Render_Planet_Mode's budget)  // NAME 3: the registry value's (soptions.h)
	int LandLOD;										// +0x74 (the options' LandLOD, 0..7)  // NAME 3: the registry value's (soptions.h)
	int PolyBudget;									// +0x78 (the options' PolyBudget, 8000..40000; Render_Space_Mode's / Gas_Giant's budget)  // NAME 3: the registry value's (soptions.h)
	int TextureReduction;							// +0x7C (the options' TextureReduction, 0..3; WW3D::Set_Texture_Reduction)  // NAME 3: the registry value's (soptions.h)
	RadarSystemClass *Radar;							// +0x80 (cpacket.cpp: WARP_INDEX 156's FAIL_IF '!radar'; 0x50BA60 makes it, 0x7BC770)  // NAME 3: the FAIL_IF's name
	MainViewUnknown84Class *Unknown84;					// +0x84 (0x50BA60 makes it: 0x24 bytes, 0x7B6BD0, with the base's +0x1C scene)
	IntersectionClass Intersection;						// +0x88 (vtable 0xAEA8A0 and its statics in the constructor)  // NAME 2: what it is
	SimpleSceneClass *Scene;							// +0x10C (0x5137F0 adds the land to it through slot +0x0C, Add_Render_Object; 0x50BA60 makes it, 0xF4 bytes, 0x75ECF0)  // NAME 2: what it is
	SimpleSceneClass *Unknown110;						// +0x110 (slot +0x68)
	SimpleSceneClass *Unknown114;						// +0x114 (slot +0x6C; the stars are added to it)
	SimpleSceneClass *Unknown118;						// +0x118 (slot +0x70)
	SimpleSceneClass *Unknown11C;						// +0x11C (slot +0x74)
	SimpleSceneClass *Unknown120;						// +0x120 (slot +0x78; the lights are added to it)
	DazzleLayerClass *DazzleLayer;						// +0x124  // NAME 2: what it is
	MainViewDazzleHandlerClass *Unknown128;				// +0x128 (enbdazzle.cpp's 0x75DD00, 8 bytes: the dazzle visibility handler)
	CameraClass *Camera;								// +0x12C (Set_Camera; slots +0x58 / +0x7C)  // NAME 2: what Set_Camera stores
	bool Unknown130;									// +0x130 (SClient 0x7456B0 sets it; the constructor: true)
	RefCountPtr<StarFieldClass> StarField;				// +0x134 (0x50BA60: a StarFieldClass)  // NAME 2: what it holds
	MainViewUnknown138Class *Unknown138;				// +0x138
	LandClass *Land;									// +0x13C 0x5137F0's FAIL_IF 'Land!=NULL' / 'NULL==Land' (SERVER_PARAMETERS 66 copies its +0x90 to the client's +0x1144 in view mode 1)  // NAME 3: the FAIL_IF's name
	OrbitalManager *Orbitals;							// +0x140 (0x50BA60 makes it, 0x1D4 bytes, 0x4F3250)  // NAME 2: its class's (OrbitalManager::Update)
	RefCountPtr<LightClass> Unknown144[4];				// +0x144 (0x50BA60: four lights)
	RefCountPtr<LightClass> Unknown154[4];				// +0x154 (0x50BA60: four lights, added to +0x120)
	RefCountPtr<LightClass> Unknown164;					// +0x164 (0x50BA60: one light)
	bool Unknown168;									// +0x168
	Vector3 Unknown16C;									// +0x16C (0x50CF70 hands it to 0x510D40)
	LandLightingStruct Lighting;						// +0x178..+0x1A4 (the constructor 0x50B360 stores its defaults; Update 0x50F1B0 fills it for LandClass::Update_Lightmap_*)  // NAME 2: what it holds
	float SeaFogNear;									// +0x1A8 0x5137F0: land%04d.ini [GENERAL] SEAFOGNEAR (100)  // NAME 2: its ini key
	float SeaFogFar;									// +0x1AC SEAFOGFAR (3000)  // NAME 2: its ini key
	Vector3 SeaColor;									// +0x1B0 SEACOLORR / G / B (1, 1, 1)  // NAME 2: its ini keys
	Vector3 Wind;										// +0x1BC 0x513B90: gas%04d.ini [GENERAL] WINDX / Y / Z (0) times 3600  // NAME 2: its ini keys
	unsigned Unknown1C8;								// +0x1C8 (0x513700 keeps its sector here; 0x50C2D0 returns it)
	bool Unknown1CC;									// +0x1CC
	UICpitClass *Cpit;									// +0x1D0 (cpacket.cpp: LOOT_HULK_PERMISSION 140, OPEN_INTERFACE 102)  // NAME 3: its file's (ui_cpit.cpp)
	MainViewUnknown1D4Class *Unknown1D4;				// +0x1D4 (slots +0x58 / +0x5C with the cockpit's; 0x50C690 makes it)
	UIGroupClass *Group;								// +0x1D8 (cpacket.cpp: CLIENT_CHAT_EVENT 165's group messages)  // NAME 3: its file's (ui_group.cpp)
	MainViewUnknown1DCClass *Unknown1DC;						// +0x1DC (0x513600 calls its +0x8C)
	MainViewUnknown1E0Class *Unknown1E0;				// +0x1E0 (Shutdown 0x50C2F0 releases it first)
	MainViewUnknown1E4Class *Unknown1E4;				// +0x1E4 (0x514220 calls its +0x5C; 0x50C690 makes it)
	WWProfileIterator *ProfileIterator;					// +0x1E8 (0x50BA60: WWProfileManager::Get_Iterator)  // NAME 2: what it holds
};

// loadingview.cpp's view (constructor 0x691570, 0x6C bytes)
class Unknown7B3E30Class;	// NAME 1: neutral (D10): the 0xFC-byte render object of the loading view's +0x64 (constructor 0x7B3E30; loadingview.cpp)
class LoadingViewClass : public Unknown65B810ViewClass	// NAME 3: class-from-file (D24) for loadingview.cpp
{
public:
	LoadingViewClass(ClientTaskClass *ctask);			// 0x691570 (G_ASSERT 'ctask', l.52; table 0xAFCF7C)  // NAME 2 [ctask]: the assert's
	virtual ~LoadingViewClass();						// +0x00 0x691680 (deleting 0x691650)
	virtual void Unknown_04();							// +0x04 0x691770 (the base's, then both scenes' fog off)
	virtual int Unknown_0C(bool on);					// +0x0C 0x692420 (the loading screen, 'Loading_Text_map_load00')
	virtual void Unknown_10();							// +0x10 0x6917A0 (UIViewClass's 0x65B4A0, then +0x58..+0x64 released)
	virtual LoadingViewClass *Unknown_5C() { return(this); }	// +0x5C 0x691630
	void Unknown_691830(const std::string &source_sector, const std::string &source_system, const std::string &name, const std::string &system);	// 0x691830 (cpacket.cpp: SERVER_HANDOFF 58)  // NAME 1 [source_sector, source_system, name, system]: the packet's fields
	void Unknown_692330();								// 0x692330 (cpacket.cpp: START 5)
	void Unknown_692160();								// 0x692160 (SClient 0x74BA30, 0x7992F0: 0x691830 with four empty strings)

	int Unknown50;										// +0x50
	int Unknown54;										// +0x54
	RefCountPtr<RenderObjClass> Unknown58;				// +0x58 (0x691830: the asset manager's 'map_load00'; START 0x692330 removes it and its fonts, FontManagerClass::Unknown_6D2310)
	RefCountPtr<RenderObjClass> Unknown5C;				// +0x5C (0x691830: 'map_select')
	RefCountPtr<RenderObjClass> Unknown60;				// +0x60 (0x691830: 'map_from')
	RefCountPtr<Unknown7B3E30Class> Unknown64;			// +0x64 (0x691830 news it, 0xFC bytes; START removes it when set)
	bool Unknown68;										// +0x68 ClientTaskClass::Render renders when it is set
	bool Unknown69;										// +0x69
};

// The 3-D movie view (constructor 0x7EEF90, 0x2A8 bytes; SClient 0x74BC20 makes it, 0x74BD10 deletes it)
class Movie3DViewClass : public Unknown65B810ViewClass	// NAME 2: SClient's member 'Movie3DView' (G_ASSERT l.4104)
{
public:
	Movie3DViewClass(ClientTaskClass *task, int arg);	// 0x7EEF90  // NAME 1 [task, arg]: what 0x74BC20 passes
	bool Unknown_7EFCC0();								// 0x7EFCC0 (SClient::Do_Frame in view mode 3: true ends the movie, 0x74BD10)

	char Unknown50[0x2A8 - 0x50];						// +0x50
};

class SClientMovieCallbackClass;

// The scripted movie view (constructor 0x8000C0, 0x178 bytes; SClient 0x74BD90 makes it, 0x74BF00 deletes it)
// what the scripted movie view's +0x4C points to, as far as scriptedmovie.cpp calls it
class ScriptedMovieInterfaceUnknown00Class { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual int Unknown_1C(); };	// NAME 1: neutral (D10)
// the script line's parameters (0x7FFDC0 returns the next one, NULL at the end; its cursor at +0x08)
class ScriptedMovieParamsClass	// NAME 1: neutral (D10)
{
public:
	bool Unknown_7FFD30(const char *name);				// 0x7FFD30: the line starts with the command name (any case): past it and its comma  // NAME 1 [name]: what the dispatcher 0x803310 passes
	char *Unknown_7FFDC0();								// 0x7FFDC0: the next comma-separated parameter, trimmed and unquoted; NULL at the end
	char Unknown00[8];									// +0x00
	char *Unknown08;									// +0x08: the cursor
};
class ScriptedMovieInterfaceClass	// NAME 1: neutral (D10)
{
public:
	virtual ScriptedMovieInterfaceUnknown00Class *Unknown_00();	// +0x00 (Skip_Frames 0x8097D0 asks the result's +0x1C)
	virtual void Unknown_04(CameraClass *camera);		// +0x04 (0x8023A0: the view's camera or NULL)  // NAME 1 [camera]: what 0x8023A0 passes
};
// a Vector3 that fades to a target (0x24 bytes; the scripted movie view's +0x88 and +0xAC): 0x80A5A0 steps it
class ScriptedMovieFadeClass	// NAME 1: neutral (D10)
{
public:
	ScriptedMovieFadeClass();							// 0x80A570: all zero
	void Unknown_80A5A0(float time);					// 0x80A5A0: one step on (time times the rate, stopping at the target)  // NAME 1 [time]: neutral
	void Unknown_80A730(float time, Vector3 from, Vector3 to);	// 0x80A730: from to to over time  // NAME 1 [time, from, to]: neutral
	Vector3 Current;									// +0x00 (what the scene gets)  // NAME 2: what it holds
	Vector3 Target;										// +0x0C  // NAME 2: what it holds
	Vector3 Rate;										// +0x18: per unit of time  // NAME 2: what it holds
};
// ui_mesh.cpp's mesh (the scripted movie's objects), as far as scriptedmovie.cpp calls it
class UIMeshClass	// NAME 3: class-from-file (D24) for ui_mesh.cpp
{
public:
	~UIMeshClass();										// 0x5B8640 (Destroy_Object deletes it)
	void Unknown_5B92B0();								// 0x5B92B0 (Destroy_Object, before the delete)
	void Unknown_5B9100(const char *bone_name, RenderObjClass *object);	// 0x5B9100: object onto the mesh's bone  // NAME 2 [bone_name]: the FAIL_IF's 'Need bone name'; NAME 1 [object]: neutral
	void Unknown_5B9130(const char *bone_name, UIMeshClass *child);	// 0x5B9130: child onto the mesh's bone (Attach_To_Bone)  // NAME 2 [bone_name, child]: the asserts' / FAIL_IF's
	void Unknown_5B9550(int arg);						// 0x5B9550 (the Play_Animation commands pass 0)  // NAME 1 [arg]: neutral
	void Unknown_5B8DF0(int a, const char *anim_name, bool loop, int b, int c, int d, int e);	// 0x5B8DF0: an animation onto the mesh  // NAME 1 [a, loop, b, c, d, e]: neutral; NAME 2 [anim_name]: the FAIL_IF's
	RenderObjClass *Peek_Object();						// 0x5B8B00 (its +0x1C; the asserts' '(*itor).Mesh->Peek_Object()')  // NAME 3: the assert's
	char Unknown00[0x1C];								// +0x00
	RenderObjClass *Unknown1C;							// +0x1C: the model (Toggle_Engine_Trails reads it without Peek_Object)
	char Unknown20[0x70 - 0x20];						// +0x20
	int Unknown70;										// +0x70: flags (the Play_Animation commands clear bit 0, set or clear bit 1)
};
// one of the scripted movie's objects (0x0C bytes, in a vector at +0x58)
struct ScriptedMovieObjectStruct	// NAME 1: neutral (D10)
{
	UIMeshClass *Mesh;									// +0x00  // NAME 3: the asserts' '(*itor).Mesh'
	int Slot;											// +0x04: the script's slot number  // NAME 2: the user errors' 'Need slot number'
	int Unknown08;										// +0x08
};
// one of the scripted movie's cameras (0x10 bytes, in a vector at +0x110)
struct ScriptedMovieCameraStruct	// NAME 1: neutral (D10)
{
	RenderObjClass *RenderObject;						// +0x00: the model whose bone carries the camera  // NAME 3: the assert's 'CameraList[CurrentCameraIndex].RenderObject'
	int Bone;											// +0x04  // NAME 2: Add_Sub_Object_To_Bone's argument
	float HFov;											// +0x08 (0: the camera's own)  // NAME 2: Set_View_Plane's argument
	float VFov;											// +0x0C  // NAME 2: Set_View_Plane's argument
};
class ScriptedMovieSoundClass { public: void Unknown_7ED6C0(int a, int b); bool Unknown_7EDA60(const char *name); void Unknown_7ED630(int arg); };	// NAME 1: neutral (D10): mv3dsnd.cpp's object (0x7ED6C0)  // NAME 1 [a, b]: what 0x802AD0 passes
class ScriptedMovieViewClass : public UIViewClass	// NAME 2: SClient's member 'ScriptedMovieView' (G_ASSERT l.4127)
{
public:
	ScriptedMovieViewClass(ClientTaskClass *task, int arg, SClientMovieCallbackClass *callback);	// 0x8000C0  // NAME 1 [task, arg, callback]: what 0x74BD90 passes
	bool Unknown_802930();								// 0x802930 (SClient::Do_Frame in view mode 6: true ends the movie, 0x74BF00)
	void Unknown_802960(KernelEventClass event);		// 0x802960 (SClient::Do_Frame: the input events while the scripted movie view is up)  // NAME 1 [event]: what it gets
	void Unknown_8027C0();								// 0x8027C0 (G_ASSERTs 'CurrentCameraIndex < size' l.972, 973; 0x802960's key 0x22): the camera onto the next one of CameraList
	void Unknown_801860();								// 0x801860 (G_ASSERT 'ScriptedCamera', l.548): the +0x120 scene's pass
	void Unknown_8019A0();								// 0x8019A0 (G_ASSERT 'ScriptedCamera', l.589): the +0x128 scene's pass
	void Unknown_801AE0(float znear, float zfar);		// 0x801AE0 (G_ASSERTs 'OrbitalScene' / 'ScriptedCamera', l.624 / 625; the render 0x8012C0): the orbital scene's pass  // NAME 2 [znear, zfar]: Set_Clip_Planes'
	void Unknown_801C40(float znear, float zfar);		// 0x801C40 (G_ASSERTs 'ScriptedCamera' / 'BackgroundDazzleLayer', l.661 / 662): the background dazzle's pass  // NAME 2 [znear, zfar]: Set_Clip_Planes'
	void Unknown_8022C0(float time);					// 0x8022C0 (FAIL_IF 'ScriptedScene == NULL', l.832): the ambient light's fade one step on  // NAME 1 [time]: what the fade gets
	void Unknown_802330(float time);					// 0x802330 (FAIL_IF 'ScriptedScene == NULL', l.855): the fog colour's fade one step on  // NAME 1 [time]: what the fade gets
	void Unknown_8023A0(bool on);						// 0x8023A0 (G_ASSERT 'ClientInterface', l.877)  // NAME 1 [on]: neutral
	void Unknown_802900();								// 0x802900: +0x6D set once +0x6C is
	void Unknown_802AD0(int arg);						// 0x802AD0: every sound of +0xF0 gets arg twice (0x7ED6C0)  // NAME 1 [arg]: neutral
	static void Unknown_802D30();						// 0x802D30 (G_ASSERT 'AssetManager', l.1178; the destructor inlines it): the unused assets released, the mesh renderer invalidated
	bool Unknown_806AD0();								// 0x806AD0: +0x6E set
	bool Unknown_809710();								// 0x809710: +0x7C cleared
	bool Unknown_809730();								// 0x809730: +0x7C set
	bool Attach_Camera(ScriptedMovieParamsClass *params);		// 0x8052E0 (FAIL_IF lines 1780 / 1789, G_ASSERTs 1787 / 1791): the camera onto a bone of the slot's mesh  // NAME 2: the user error's 'Attach_Camera'; NAME 1 [params]: neutral
	bool Attach_To_Bone(ScriptedMovieParamsClass *params);		// 0x805810 (FAIL_IF lines 1881 / 1885 / 1913, G_ASSERTs 1905 / 1906): an object onto a bone of the host's mesh  // NAME 2: the user error's 'Attach_To_Bone'; NAME 1 [params]: neutral
	bool Attach_Light_To_Bone(ScriptedMovieParamsClass *params);	// 0x8059A0 (FAIL_IF lines 1923 / 1930 / 1954, G_ASSERTs 1926 / 1927 / 1947): a light onto a bone of the host's mesh  // NAME 2: the user error's 'Attach_Light_To_Bone'; NAME 1 [params]: neutral
	bool Set_Light_Intensity(ScriptedMovieParamsClass *params);	// 0x806100 (FAIL_IF lines 2096-2104): Lights[slot]'s intensity  // NAME 2: the user error's 'Set_Light_Intensity'; NAME 1 [params]: neutral
	bool Set_Light_Far_Attenuation(ScriptedMovieParamsClass *params);	// 0x806280 (FAIL_IF lines 2130-2138)  // NAME 2: the user error's 'Set_Light_Far_Attenuation'; NAME 1 [params]: neutral
	bool Set_Light_Ambient_Color(ScriptedMovieParamsClass *params);	// 0x806440 (FAIL_IF lines 2170-2186)  // NAME 2: the user error's 'Set_Light_Ambient_Color'; NAME 1 [params]: neutral
	bool Set_Light_Specular_Color(ScriptedMovieParamsClass *params);	// 0x806670 (FAIL_IF lines 2212-2228)  // NAME 2: the user error's 'Set_Light_Specular_Color'; NAME 1 [params]: neutral
	bool Set_Light_Diffuse_Color(ScriptedMovieParamsClass *params);	// 0x8068A0 (FAIL_IF lines 2254-2270)  // NAME 2: the user error's 'Set_Light_Diffuse_Color'; NAME 1 [params]: neutral
	bool Set_Camera_View_Plane(ScriptedMovieParamsClass *params);	// 0x806AF0 (FAIL_IF lines 2318 / 2323): the camera's view plane, in degrees  // NAME 2: the user error's 'Set_Camera_View_Plane'; NAME 1 [params]: neutral
	bool Shake_Camera(ScriptedMovieParamsClass *params);		// 0x808220 (FAIL_IF lines 2905 / 2909, G_ASSERT 'CameraEffects' 2912)  // NAME 2: the user error's 'Shake_Camera'; NAME 1 [params]: neutral
	bool Set_Ambient_Light(ScriptedMovieParamsClass *params);	// 0x808330 (FAIL_IF lines 2935-2946): the scene's ambient light, at once or faded over a time  // NAME 2: the user error's 'Set_Ambient_Light'; NAME 1 [params]: neutral
	bool Unknown_808840();								// 0x808840 (FAIL_IF 'ScriptedScene == NULL', l.3045): the scene's fog off
	bool Set_Screen_Fade_Color(ScriptedMovieParamsClass *params);	// 0x808880 (FAIL_IF lines 3069-3081): the screen fade's colour over a time (0x7FFA30)  // NAME 2: the user error's 'Set_Screen_Fade_Color'; NAME 1 [params]: neutral
	bool Set_Screen_Fade_Opacity(ScriptedMovieParamsClass *params);	// 0x808A00 (FAIL_IF lines 3107 / 3111): its opacity over a time (0x7FFBE0)  // NAME 2: the user error's 'Set_Screen_Fade_Opacity'; NAME 1 [params]: neutral
	bool Enable_Letterbox(ScriptedMovieParamsClass *params, bool on);	// 0x808BB0 (FAIL_IF line 3167): the letterbox in or out over a time (0x7FF830)  // NAME 2: the user error's 'Enable_Letterbox'; NAME 1 [params, on]: neutral
	bool Set_Clip_Plane(ScriptedMovieParamsClass *params);		// 0x808AD0 (FAIL_IF lines 3137 / 3141): the camera's clip planes  // NAME 2: the user error's 'Set_Clip_Plane'; NAME 1 [params]: neutral
	bool Stop_Audio(ScriptedMovieParamsClass *params);			// 0x807020 (FAIL_IF line 2435): the sound of that name stopped  // NAME 2: the user error's 'Stop_Audio'; NAME 1 [params]: neutral
	bool Play_Animation(ScriptedMovieParamsClass *params);		// 0x805000 (FAIL_IF lines 1673 / 1682, G_ASSERT 1679): the slot's animation (loop, two numbers)  // NAME 2: the user error's 'Play_Animation'; NAME 1 [params]: neutral
	bool Unknown_8051B0(ScriptedMovieParamsClass *params);		// 0x8051B0 (FAIL_IF lines 1731 / 1740, G_ASSERT 1737; its user errors say Play_Animation): the slot's animation, held  // NAME 1 [params]: neutral
	bool Destroy_Object(ScriptedMovieParamsClass *params);		// 0x8046E0 (FAIL_IF line 1494, G_ASSERT 'mesh' 1501): the slot's object deleted  // NAME 2: the user error's 'Destroy_Object'; NAME 1 [params]: neutral
	bool Toggle_Engine_Trails(ScriptedMovieParamsClass *params, bool on);	// 0x808F00 (FAIL_IF line 3252, G_ASSERT 3258): the slot's engine trails (shiputils.cpp's 0x810750)  // NAME 2: the user error's 'Toggle_Engine_Trails'; NAME 1 [params, on]: neutral
	bool Hide_Object(ScriptedMovieParamsClass *params, bool hide);	// 0x809890 (FAIL_IF line 3595, G_ASSERTs 3601 / 3602): the slot's model hidden or shown, its trails the other way  // NAME 2: the user error's 'Hide_Object'; NAME 1 [params, hide]: neutral
	bool Skip_Frames(ScriptedMovieParamsClass *params);			// 0x8097D0 (FAIL_IF line 3567): +0x164 on by the frames at 30 a second  // NAME 2: the user error's 'Skip_Frames'; NAME 1 [params]: neutral

	ScriptedMovieInterfaceClass *ClientInterface;		// +0x4C (0x8023A0's G_ASSERT)  // NAME 3: the assert's
	CameraClass *ScriptedCamera;						// +0x50: the camera (Set_Camera_View_Plane, Set_Clip_Plane; 0x8023A0 hands it to ClientInterface's +0x04)  // NAME 3: the asserts' 'ScriptedCamera' (0x802420, Attach_Camera)
	SimpleSceneClass *ScriptedScene;					// +0x54 (its ambient light +0x1C, its fog colour +0x2C)  // NAME 3: the FAIL_IF's
	std::vector<ScriptedMovieObjectStruct> Objects;		// +0x58: the script's objects by slot  // NAME 2: what it holds
	char Unknown68[0x6C - 0x68];						// +0x68
	bool Unknown6C;										// +0x6C
	bool Unknown6D;										// +0x6D
	bool Unknown6E;										// +0x6E (0x802930: the movie ends once +0x6C..+0x6E are set)
	char Unknown6F[0x7C - 0x6F];						// +0x6F
	bool Unknown7C;										// +0x7C
	char Unknown7D[0x84 - 0x7D];						// +0x7D
	int CurrentCameraIndex;								// +0x84: CameraList's next  // NAME 3: the asserts'
	ScriptedMovieFadeClass Unknown88;					// +0x88: the ambient light (0x8022C0)
	ScriptedMovieFadeClass UnknownAC;					// +0xAC: the fog colour (0x802330)
	enum { MAX_LIGHTS_PER_SCENE = 4 };					// NAME 3: the FAIL_IFs' 'lightslot >= MAX_LIGHTS_PER_SCENE' ('Exceeding 4 lights per scene')
	LightClass *Lights[MAX_LIGHTS_PER_SCENE];			// +0xD0: the script's lights by slot  // NAME 3: the FAIL_IFs' 'Lights[lightslot]'
	char UnknownE0[0xF0 - 0xE0];						// +0xE0
	std::vector<ScriptedMovieSoundClass *> UnknownF0;	// +0xF0: mv3dsnd.cpp's objects (0x802AD0)
	char Unknown100[0x110 - 0x100];						// +0x100
	std::vector<ScriptedMovieCameraStruct> CameraList;	// +0x110  // NAME 3: the asserts' 'CameraList[CurrentCameraIndex]'
	SimpleSceneClass *Unknown120;						// +0x120: a scene rendered at depth 1 between clip planes 20 and 100000 (0x801860)
	char Unknown124[0x128 - 0x124];						// +0x124
	SimpleSceneClass *Unknown128;						// +0x128: one between 50 and 150 (0x8019A0)
	char Unknown12C[0x134 - 0x12C];						// +0x12C
	SimpleSceneClass *OrbitalScene;						// +0x134 (0x801AE0 renders it at depth 0.95..1)  // NAME 3: the assert's
	char Unknown138[0x13C - 0x138];						// +0x138
	DazzleLayerClass *BackgroundDazzleLayer;			// +0x13C (0x801C40 renders it at depth 1)  // NAME 3: the assert's
	char Unknown140[0x164 - 0x140];						// +0x140
	int Unknown164;										// +0x164 (Skip_Frames adds to it)
	bool Unknown168;									// +0x168 (0x802960: the space key toggles it)
	char Unknown169[0x171 - 0x169];						// +0x169
	bool Unknown171;									// +0x171 (Skip_Frames only while it is set)
	char Unknown172[0x178 - 0x172];						// +0x172
};

// The 0x120 bytes at SClient +0x13CC: 24 colours set to (1, 1, 1) by the out-of-line constructor 0x764FD0
// (between proj_decal_manager.cpp and tsk_game.cpp; file unknown)
class Unknown764FD0Class	// NAME 1: neutral (D10)
{
public:
	Unknown764FD0Class();								// 0x764FD0
	void Unknown_765010();								// 0x765010 (SClient::Init)
	Vector3 Unknown_765DF0(int index);					// 0x765DF0: colour index (0..23), else (1, 1, 1) (SClient 0x750F70: the chat channel)  // NAME 1 [index]: neutral

	Vector3 Unknown00[24];								// +0x00  // NAME 2: what the constructor writes
};

// The +0xE8 member of what 0x77E610 returns (0x7817E0), as far as cpacket.cpp calls it
class Unknown77E3F0E8Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08(char *text);					// +0x08 (MESSAGE_CHATSTREAM 95: the line it shows)  // NAME 1 [text]: what the handler passes
	virtual bool Unknown_0C();							// +0x0C (CLIENT_CHAT_EVENT 165 shows a private message only when true)
};

// What the +0x1394 values' 0x77E3F0 / 0x77E610 return, as far as SClient calls it
class Unknown77E3F0Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_784B80(char *data, int size, bool flag);	// 0x784B80 (avatar.cpp; cpacket.cpp: CLIENT_CHAT_EVENT 165 passes Unknown74, Unknown78, 1)  // NAME 1 [data, size, flag]: what the handler passes
	Unknown77E3F0E8Class *Unknown_7817E0();			// 0x7817E0 (avatar.cpp: returns this + 0xE8)
	void Unknown_780810(SClientUnknown1338Class *unknown);	// 0x780810 (SClient 0x74C7B0 / 0x74C840 / 0x74C8D0 pass Unknown1338)  // NAME 1 [unknown]: neutral
};

// The values of SClient +0x1394 (reference counted: Delete_This in slot 0, the count at +0x04)
class SClientUnknown1394Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();							// +0x38 the destructor calls it before the release
	SClientUnknown1394Class(AvatarDescClass &desc, int unknown1, int unknown2, int unknown3);	// 0x77B870 (0x384 bytes; SClient 0x74C3B0 passes the AVATAR_DESCRIPTION and three 0)  // NAME 1 [desc, unknown1, unknown2, unknown3]: what 0x74C3B0 passes
	Unknown77E3F0Class *Unknown_77E3F0();				// 0x77E3F0 (SClient 0x74C7B0)
	Unknown77E3F0Class *Unknown_77E610();				// 0x77E610 (SClient 0x74C840)

	char Unknown08[0xF8 - 0x08];						// +0x08
	int UnknownF8;										// +0xF8 (SClient 0x74C960 returns its address; 0x74C3B0 hands it to the bridge interface)
	char UnknownFC[0x384 - 0xFC];						// +0xFC
};

// What the chat panel's +0x138 points to, as far as SClient 0x744E70 calls it
class UIChatPanelUnknown138Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C();
	virtual void Unknown_90(); virtual void Unknown_94(); virtual void Unknown_98(); virtual void Unknown_9C();
	virtual void Unknown_A0(); virtual void Unknown_A4(); virtual void Unknown_A8();
	virtual void Unknown_AC(bool on);					// +0xAC (0x744E70: false when the current view is +0x135C or asked, true for a type-1 target)  // NAME 1 [on]: neutral
};

#pragma pack(push, 4)		// as ClientClass (client.h)
class SClient : public ClientClass, public SOptionsClass	// NAME 2: the client's 'SClient::Process_Packets' (0x7427C0)
{
public:
	SClient(ClientTaskClass *task);						// 0x7405E0  // NAME 1 [task]: ClientTaskClass::Init passes this
	~SClient();											// 0x740BB0
	virtual void Init();								// +0x00 0x741180: ClientTaskClass::Init calls it once Kernel is set
	virtual void Shutdown();							// +0x04 0x741BB0: ClientTaskClass::Shutdown
	virtual void Unknown_08();							// +0x08 0x7427A0 (empty)
	virtual void Process_Packets();						// +0x0C 0x7427C0 (S: 'SClient::Process_Packets')
	virtual void Unknown_10();							// +0x10 0x7428F0: pumps the window's message queue (the base's name kept)
	virtual int Do_Frame(bool focused);					// +0x14 0x742B60: ClientTaskClass::Sector, 0 .. 5
	virtual int Unknown_18() { return((int)this); }		// +0x18 0x740AC0 (the base's returns 0)
	virtual unsigned Set_Avatar_ID(unsigned id);		// +0x24 0x745730: the base's
	virtual bool Unknown_34(const char *text);			// +0x34 0x74ABC0: the chat line's '/' commands ('/group', 'Unable to camp - insufficient marshmallows.'); true when it was one  // NAME 1 [text]: the chat line
	bool Unknown_747510(const char *text);				// 0x747510 (0x74ABC0: '/group ...')  // NAME 1 [text]: the chat line
	bool Unknown_745FF0(const char *text);				// 0x745FF0 (0x74ABC0: '/chat ...')  // NAME 1 [text]: the chat line
	bool Unknown_747C70(const char *text);				// 0x747C70 (0x74ABC0: before the logoff commands)  // NAME 1 [text]: the chat line
	bool Unknown_747FA0(const char *text);				// 0x747FA0 (0x74ABC0: the first of five command groups after '/self tag')  // NAME 1 [text]: the chat line
	bool Unknown_749020(const char *text);				// 0x749020 (0x74ABC0)  // NAME 1 [text]: the chat line
	bool Unknown_749590(const char *text);				// 0x749590 (0x74ABC0)  // NAME 1 [text]: the chat line
	bool Unknown_749A70(const char *text);				// 0x749A70 (0x74ABC0)  // NAME 1 [text]: the chat line
	bool Unknown_749ED0(const char *text);				// 0x749ED0 (0x74ABC0)  // NAME 1 [text]: the chat line
	virtual void Set_Debug_Mode(bool on);				// +0x3C 0x743330: the base's, then 'Debug mode turned %s.'
	virtual void Add_Object(ClientObjectClass *object);	// +0x40 0x744B90: the base's, then the main view's
	virtual ClientObjectClass *Remove_Object(unsigned id);	// +0x44 0x744BD0: the base's, then the main view's
	virtual void Load_INIs();							// +0x5C 0x740F60: action.ini, cskill_*.ini, testeffects.ini, gutter_color.ini, cbasset.ini  // NAME 2: what it does
	virtual void Unload_INIs();							// +0x60 0x741B70: Load_INIs undone by its last user  // NAME 2: what it does
	virtual void Unknown_64();							// +0x64 0x744690: ClientTaskClass::Render, Wait_For_Redirect

	void Print(int channel, const char *format, ...);	// 0x74DF70 (cdecl): a line in the chat panel  // NAME 2: what it does
	void Unknown_742530(UIViewClass *view);				// 0x742530 (Shutdown passes 0; 0x745F30 the view of the new view mode)  // NAME 1 [view]: what it gets
	void Unknown_7429B0();								// 0x7429B0 (Do_Frame, after the lost-dialog check; FAIL_IF '!CurrentView', l.749)
	void Unknown_743380(KernelEventClass *event);		// 0x743380 (Do_Frame: the input queue's event without the +0x1278 source)  // NAME 1 [event]: what it gets
	bool Unknown_7436A0(KernelEventClass event);		// 0x7436A0 (Do_Frame: an event no dispatcher took; false: 0x744240)  // NAME 1 [event]: what it gets
	void Unknown_744240(KernelEventClass event);		// 0x744240 (Do_Frame: an event 0x7436A0 refused)  // NAME 1 [event]: what it gets
	void Unknown_74EDB0();								// 0x74EDB0 (Do_Frame, after 0x7429B0)
	void Unknown_751660();								// 0x751660 (Do_Frame, after 0x74EDB0)
	void Unknown_74F5C0();								// 0x74F5C0 (Do_Frame, after 0x751660)
	void Unknown_7451D0();								// 0x7451D0 (Init)
	void Unknown_745350();								// 0x745350 (Init)
	void Unknown_744C10();								// 0x744C10 (Init, after the camera mode)
	void Unknown_744D80();								// 0x744D80 (Init's camera block as a function; 0x6B58A0 calls it)
	void Unknown_744E50();								// 0x744E50: the camera manager's callback (Init registers it, Shutdown removes it)
	void Unknown_7518A0();								// 0x7518A0 (Init)
	void Unknown_7511A0(); void Unknown_750C20(int index);	// 0x7511A0 (the main view's 0x514010, on every view-mode change); 0x750C20 (no caller: plays testeffects.ini's record)  // NAME 1 [index]: the record's
	void Unknown_7555B0();								// 0x7555B0 (Shutdown)
	void Unknown_755600();								// 0x755600 (0x745750, before the +0x1394 values go)
	void Unknown_743290();								// 0x743290 (cpacket.cpp: LOGOFF_CONFIRMATION 186)
	void Unknown_74CA70();								// 0x74CA70 (0x69F8F0): +0x1374 = 3
	bool Unknown_74D730();								// 0x74D730 (camera_manager.cpp 0x799E40): an object's Relationship.IsAttacking is set
	void Unknown_74D950();								// 0x74D950 (CHANGE_BASE_ASSET 38, after 0x744B60, for the objects 0x6F5A40 picks)
	bool Unknown_744B60();								// 0x744B60 (cpacket.cpp: REMOVE 7, GROUP 30)
	void Unknown_74C530(unsigned id);					// 0x74C530 (cpacket.cpp: REMOVE 7 passes its GameID)  // NAME 1 [id]: the packet's GameID
	bool Unknown_74B5E0();								// 0x74B5E0 (0x639630, 0x63A100): the task's Is_Focused
	void Unknown_74B600();								// 0x74B600 (cpacket.cpp: mode change 76, mode 0)
	void Unknown_74B640();								// 0x74B640 (cpacket.cpp: mode change 76, mode 1)
	void Unknown_74C3B0(int game_id, AvatarDescClass &desc);	// 0x74C3B0 (cpacket.cpp: AVATAR_DESCRIPTION 97 passes its GameID and Desc)  // NAME 1 [game_id, desc]: the packet's fields
	void Unknown_745750();								// 0x745750 (cpacket.cpp: SERVER_HANDOFF 58)
	void Unknown_745AA0(float *turn, float *tilt, int *move_direction, bool fixed_speed);	// 0x745AA0 (no caller): postate's 0x717430 on the avatar's type-4 state  // NAME 2 [turn, tilt, move_direction, fixed_speed]: 0x717430's
	void Unknown_745B20(bool level, bool auto_level);	// 0x745B20: postate's 0x7179A0  // NAME 2 [level, auto_level]: 0x7179A0's
	void Unknown_745B90(float turn);					// 0x745B90 (ui_cpit.cpp 0x5806E0): turn, or level out below 0.0001  // NAME 2 [turn]: 0x717430's
	void Unknown_745C70(float tilt);					// 0x745C70 (0x5806E0): tilt, or auto-level below 0.0001  // NAME 2 [tilt]: 0x717430's
	void Unknown_745D50(float turn, float tilt);		// 0x745D50 (0x6B58A0)  // NAME 2 [turn, tilt]: 0x717430's
	void Unknown_745DD0();								// 0x745DD0 (no caller): level and auto-level
	void Unknown_745E40(int move_direction, bool fixed_speed);	// 0x745E40 (0x5806E0, 19 calls)  // NAME 2 [move_direction, fixed_speed]: 0x717430's
	void Unknown_745000(int base_asset);				// 0x745000 (cpacket.cpp: SERVER_PARAMETERS 66 passes its BackdropBaseAsset)  // NAME 1 [base_asset]: the packet's field
	void Unknown_750A70(unsigned sector);				// 0x750A70 (cpacket.cpp: SERVER_PARAMETERS 66 passes its SectorNum)  // NAME 1 [sector]: the packet's field
	void Unknown_74BB50(int type);						// 0x74BB50: 'Logging off to desktop.' (1), '... login screen.' (2), '... character selection.' (3) when Unknown138C is set, then LOGOFF_REQUEST on the connection  // NAME 1 [type]: what it selects
	void Unknown_74C690();								// 0x74C690: every Unknown1394 value's slot +0x38 and release, then the table cleared (as the destructor)
	bool Unknown_745490();								// 0x745490: the current camera mode saved into 'cam<character id>.ini' (angles in degrees)
	bool Unknown_74D570();								// 0x74D570 (0x65CCD0, 0x7C44B0, 0x7C4940): the player is in a group (a GroupInfo member with a name)
	bool Unknown_74D690();								// 0x74D690 (CTA_RESPONSE's handler 0x73DAA0, 0x645510): the player leads its group (GroupInfo.IsGroupLeader)
	bool Unknown_74D780(ClientObjectClass *co);			// 0x74D780: co is another avatar, not skipped by 0x70BAC0, and both are in PvP (FAIL_IFs '!co', '!avatar', ll.4521 / 4524)  // NAME 1 [co]: the FAIL_IF's name
	bool Unknown_74D8B0(ClientObjectClass *co);			// 0x74D8B0: the debug object's or co's 0x6F5F60 (FAIL_IFs '!player_co', '!co', ll.4552 / 4553)  // NAME 1 [co]: the FAIL_IF's name
	bool Unknown_74D870();								// 0x74D870: the debug object's 0x6F5ED0 ('IsInPVP'; FAIL_IF '!co', l.4545)
	void Unknown_74BC20(int arg);						// 0x74BC20: the 3-D movie view made and shown, view mode 3 (G_ASSERT '!Movie3DView', l.4090)  // NAME 1 [arg]: neutral
	void Unknown_74BD10();								// 0x74BD10: the 3-D movie view ended, the view mode before it back (l.4104)
	void Unknown_74BD90(int arg);						// 0x74BD90: the scripted movie view made, view mode 6 (G_ASSERT '!ScriptedMovieView', l.4117)  // NAME 1 [arg]: neutral
	void Unknown_74BF00(int mode);						// 0x74BF00: the scripted movie view ended into view mode 'mode' (l.4127)  // NAME 1 [mode]: what it is
	void Unknown_744E70(bool arg);						// 0x744E70: the main view's target (FAIL_IF '!target', l.2110) as an ACTION 44 packet, else the chat panel's +0x138  // NAME 1 [arg]: neutral
	Unknown77E3F0Class *Unknown_74C7B0(unsigned id);	// 0x74C7B0: Unknown1394's value of id, its 0x77E3F0 given Unknown1338  // NAME 1 [id]: the table's key
	Unknown77E3F0Class *Unknown_74C840(unsigned id);	// 0x74C840: as 0x74C7B0 through 0x77E610  // NAME 1 [id]: the table's key
	Unknown77E3F0Class *Unknown_74C8D0(const char *name);	// 0x74C8D0: as 0x74C840 for the named object's GameID  // NAME 1 [name]: Find_Object's
	int *Unknown_74C960(unsigned id);					// 0x74C960: Unknown1394's value's +0xF8  // NAME 1 [id]: the table's key
	void Unknown_74C9D0(const RefCountPtr<UITalkTreeSpaceClass> &tree);	// 0x74C9D0: Unknown1298 set, or reset to Unknown129C  // NAME 1 [tree]: what it holds
	void Unknown_745F30(int type);						// 0x745F30 (cpacket.cpp: CLIENT_TYPE 60 passes 0, 1 or 5 for the packet's Type 0, 1 or 2)  // NAME 1 [type]: what the handler passes
	void Unknown_74B680(int a, int starbase_id);		// 0x74B680 (cpacket.cpp: starbase tow 80 passes 0 and StarbaseID1)  // NAME 1 [a, starbase_id]: what the handler passes
	void Unknown_74BA30(int a, int starbase_id, int mode);	// 0x74BA30 (cpacket.cpp: STARBASE_SET 79 passes 0, StarbaseID and 0 / 1 / 5 by its exit mode)  // NAME 1 [a, starbase_id, mode]: what the handler passes
	void Unknown_750F70(const char *name, int channel);	// 0x750F70 (cpacket.cpp: CLIENT_CHAT_EVENT 165, the sender and 2..15 by the channel)  // NAME 1 [name, channel]: what the handler passes
	void Unknown_74D990(int color, char *text, int a);	// 0x74D990 (cpacket.cpp: MESSAGE_STRING 29 passes 0 last)  // NAME 1 [color, text, a]: what the handler passes
	void Unknown_74E710(const std::string &message, int id, int type);	// 0x74E710 (cpacket.cpp: CONFIRMED_ACTION_OFFER 190)  // NAME 1 [message, id, type]: the packet's fields
	void Unknown_74EB00(int type, int id);				// 0x74EB00 (cpacket.cpp: the confirmed action's timeout 191)  // NAME 1 [type, id]: the packet's fields
	void Unknown_751460(int points_total, int points_spent);	// 0x751460 (cpacket.cpp: the referral points 233)  // NAME 1 [points_total, points_spent]: the packet's fields
	void Unknown_750800();								// 0x750800 (the chat panel's 0x563280, its message button): yes to the oldest type 1 offer
	void Unknown_7517E0(int a, int b, int c, int d);		// 0x7517E0 (cpacket.cpp: GUILD_PLAYER_PERMISSIONS 210 passes its four fields)  // NAME 1 [a, b, c, d]: neutral
	void Unknown_751850(std::vector<GuildRankNameStruct> &ranks);	// 0x751850 (cpacket.cpp: GUILD_RANK_NAMES_SECTOR 211)  // NAME 1 [ranks]: the packet's field
	void Unknown_745160();								// 0x745160: the cockpit's 0x584A60 after clearing Unknown1198
	void Unknown_745050();								// 0x745050: 0x745000 again with the kept base asset (scriptedmovie.cpp's neighbourhood 0x8010A0)
	void Unknown_7456B0();								// 0x7456B0: the main view's debris mode and four resets
	void Process_AI();									// 0x744260 (Do_Frame, after Process_Packets): every object's slot +0x20, the camera, the finished effects, the third list's records (S: 'SClient::Process_AI')
	void Unknown_745EC0(FileClass *file, int indent);	// 0x745EC0: 'Client %x' and the main view's dump  // NAME 1 [file, indent]: Printf_Indented's
	ClientObjectClass *Unknown_74BF80();				// 0x74BF80: the object of Unknown1328, else NULL
	void Unknown_74C370(RefCountClass *object);			// 0x74C370: Unknown12B8's reference-counted setter (0x7EFA40 calls it)  // NAME 1 [object]: neutral
	void Unknown_74FA30();								// 0x74FA30 (0x750210)
	void Unknown_74FEC0();								// 0x74FEC0 (0x750210)
	void Unknown_750210();								// 0x750210: 0x74FA30, 0x74FEC0, then the chat panel's +0xE0
	void Unknown_750260(bool reset);					// 0x750260 (the main view's 0x50D520 passes 0 with the options' +0x05): the audio debug page  // NAME 2 [reset]: true sets every sample name to NONE
	void Unknown_750B30(int arg);						// 0x750B30: Unknown1330's 0x43C660  // NAME 1 [arg]: neutral
	void Unknown_74E580(const char *text, const char *type, const char *value);	// 0x74E580 (the guild confirmations 0x73E720 / 0x73EEF0)  // NAME 1 [text, type, value]: what it does with them
	void Unknown_74DFF0(int color, const char *text, int message_type, int arg);	// 0x74DFF0 (cpacket.cpp: GUILD_MESSAGE 208 passes 8, the substituted text, MessageType, 0)  // NAME 1 [color, text, message_type, arg]: what the handler passes
	bool Unknown_750B60();								// 0x750B60 (camera_manager.cpp 0x7992F0 / 0x7993E0): the avatar is incapacitated (its aux data IsIncapacitated)

	bool Unknown1198;									// +0x1198 (true)
	std::string Unknown119C;							// +0x119C
	std::deque<int> Unknown11AC[2];						// +0x11AC (0x30 bytes each: the elements' constructor 0x740AE0, destructor 0x755A10)
	std::deque<int> Unknown120C[2];						// +0x120C
	int Unknown126C;									// +0x126C (-1)
	LoginUnknown0CClass *Unknown1270;					// +0x1270 Init: the input dispatcher (inpdsp.cpp)
	LoginUnknown18Class *Unknown1274;					// +0x1274 Init: the software mouse (0x514FE0)
	int Unknown1278;									// +0x1278
	RefCountPtr<UIChatPanelClass> ChatPanel;			// +0x127C Init (the reference count at +0x3C)  // NAME 3: its file's
	UILineDisplayClass *LineDisplay;					// +0x1280 Init  // NAME 3: its file's
	PDAInterfaceClass *PDAInterface;					// +0x1284 Init  // NAME 3: its file's
	int Unknown1288;									// +0x1288
	BridgeInterfaceClass *BridgeInterface;				// +0x128C Init  // NAME 3: its file's
	UIHotKeysClass *HotKeys;							// +0x1290 Init  // NAME 3: its file's
	VerbManagerClass *VerbManager;						// +0x1294 Init  // NAME 3: its file's
	RefCountPtr<UITalkTreeSpaceClass> Unknown1298;		// +0x1298 Init: Unknown129C's object
	RefCountPtr<UITalkTreeSpaceClass> Unknown129C;		// +0x129C Init
	GalaxyMapClientClass *GalaxyMap;					// +0x12A0 Init  // NAME 3: its file's
	UIStarmapClass *Starmap;							// +0x12A4 Init  // NAME 3: its file's
	CameraManagerClass *CameraManager;					// +0x12A8 Init  // NAME 3: its file's
	int Unknown12AC;									// +0x12AC (the constructor leaves it)
	int Unknown12B0;									// +0x12B0 Init: the base's Unknown10BC
	int Unknown12B4;									// +0x12B4
	RefCountClass *Unknown12B8;							// +0x12B8 (0x74C370: released, set, referenced)
	SClientUnknown12BCClass *Unknown12BC;				// +0x12BC (Shutdown deletes it)
	DeltaPlayerClass *AuxData;							// +0x12C0 (0x7518A0's G_ASSERT '!AuxData' before its new; AUX_DATA 27 decodes into it)  // NAME 3: the assert's name
	VectorClass<SClientUnknown12C4Class *> Unknown12C4;	// +0x12C4 vtable 0xB032BC, one entry (NULL)
	int Unknown12D4;									// +0x12D4
	TicketClass Ticket;									// +0x12D8 NAME 1: ClientTaskClass::Init copies the ticket here
	bool Unknown1320;									// +0x1320 (true)
	int Unknown1324;									// +0x1324
	int Unknown1328;									// +0x1328 (the constructor leaves it)
	MusicManagerClass *MusicManager;					// +0x132C Init  // NAME 3: its file's
	SClientUnknown1330Class *Unknown1330;				// +0x1330 (Shutdown deletes it: destructor 0x43C560)
	SClientUnknown1334Class *Unknown1334;				// +0x1334
	SClientUnknown1338Class *Unknown1338;				// +0x1338
	ProjDecalManagerClass *ProjDecalManager;			// +0x133C  // NAME 3: its file's
	VectorClass<Vector3> Unknown1340;					// +0x1340 vtable 0xAE9B0C
	UIViewClass *CurrentView;								// +0x1350 (0x7429B0's FAIL_IF '!CurrentView')
	MainViewClass *MainView;							// +0x1354 Init  // NAME 3: its file's
	UIViewClass *Unknown1358;								// +0x1358 Init (0x514CC0, 0x50 bytes)
	UIViewClass *Unknown135C;								// +0x135C
	Movie3DViewClass *Movie3DView;						// +0x1360 view mode 3 (the client's G_ASSERT names, ll.4090 / 4104)
	ScriptedMovieViewClass *ScriptedMovieView;			// +0x1364 view mode 6 (the client's G_ASSERT names, ll.4117 / 4127)
	LoadingViewClass *LoadingView;						// +0x1368 Init  // NAME 3: its file's
	int Get_Mode() { return(ViewMode); }				// (inline: the main view's 0x514010 G_ASSERT 'Get_Client()->Get_Mode() == newmode')  // NAME 3: the assert's
	int ViewMode;										// +0x136C (2; 3: the 3-D movie view, 6: the scripted movie view)  // NAME 2: what Do_Frame's asserts show
	int Unknown1370;									// +0x1370 (3)
	int Unknown1374;									// +0x1374 (3)
	int Unknown1378;									// +0x1378
	bool Unknown137C;									// +0x137C
	int Unknown1380;									// +0x1380
	ClientTaskClass *Task;								// +0x1384 the constructor's argument  // NAME 2: what it holds
	bool Unknown1388;									// +0x1388
	int Unknown138C;									// +0x138C (1 after '/camp')
	float Unknown1390;									// +0x1390 (the main view's Update adds it to the hours in debug mode)
	std::hash_map<unsigned, SClientUnknown1394Class *> Unknown1394;	// +0x1394 keyed by game id (0x756820: the debug object's)
	int Unknown13AC;									// +0x13AC
	int Unknown13B0;									// +0x13B0
	int Unknown13B4;									// +0x13B4
	int Unknown13B8;									// +0x13B8
	int Unknown13BC;									// +0x13BC
	bool Unknown13C0;									// +0x13C0
	bool Unknown13C1;									// +0x13C1
	Render2DTextClass *Text;							// +0x13C4 Init: font_med10.tga's text  // NAME 2: what it holds
	RefCountPtr<Font3DInstanceClass> Font;				// +0x13C8 Init: font_med10.tga  // NAME 2: what it holds
	Unknown764FD0Class Unknown13CC;						// +0x13CC
	int Unknown14EC;									// +0x14EC
	bool Unknown14F0;									// +0x14F0
	int Unknown14F4;									// +0x14F4
};
#pragma pack(pop)

// The scripted movie view's link back to SClient (vtable 0xB032F8, 8 bytes; 0x74BD90 makes it)
class SClientMovieCallbackClass	// NAME 1: neutral (D10)
{
public:
	SClientMovieCallbackClass(SClient *client) { Client = client; }
	virtual SClient *Get_Client() { return(Client); }	// +0x00 0x74BEA0  // NAME 2: what it returns
	virtual void Unknown_04(RefCountClass *object) { Client->Unknown_74C370(object); }	// +0x04 0x74BEC0: SClient's 0x74C370  // NAME 1 [object]: neutral

	SClient *Client;									// +0x04  // NAME 2: what it holds
};

#endif
