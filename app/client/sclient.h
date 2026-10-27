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
#include "vector3.h"
#include <deque>
#include <vector>
#include <hash_map.h>

class LoginUnknown0CClass;		// tsk_login.h: inpdsp.cpp's input dispatcher (0x18 bytes)
class LoginUnknown18Class;		// tsk_login.h: the software mouse (0x30 bytes, 0x514FE0)
class Font3DInstanceClass;
class FileClass;
class RefCountClass;
class CameraClass;
class Render2DTextClass;
class MusicManagerClass;		// NAME 3: class-from-file (D24) for audio\musicmanager.cpp (constructor 0x439E40, 0x1FC bytes)
class CameraManagerClass;		// NAME 3: class-from-file (D24) for camera_manager.cpp (constructor 0x798D20, 0x4C bytes)
class UIChatPanelClass;			// NAME 3: class-from-file (D24) for ui_chat_panel.cpp (constructor 0x561DD0, 0x268 bytes)
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
class SClientUnknown1330Class;	// NAME 1: neutral (D10): what SClient +0x1330 points to (destructor 0x43C560, musicmanager.cpp's neighbourhood)
class ProjDecalManagerClass;		// NAME 3: class-from-file (D24) for proj_decal_manager.cpp (0x763050 loads 'proj_dec.ini' into it; 0x10 bytes, one vector)
class SClientUnknown1394Class;	// NAME 1: neutral (D10): the values of SClient +0x1394 (reference counted, slot +0x38)
class AvatarDescClass;				// avatar_desc.h
struct GuildRankNameStruct;			// pkguildranknamesrequestclient.h

// verbmanager.cpp's verb manager (0x14 bytes): SClient::Init creates it, the verb packets update it
class VerbManagerClass		// NAME 3: class-from-file (D24) for verbmanager.cpp (constructor 0x81F100, 0x14 bytes)
{
public:
	VerbManagerClass(SClient *client);					// 0x81F100
	~VerbManagerClass();								// 0x81F1E0
	void Unknown_81F810(int game_id, void *drops, void *adds);	// 0x81F810 (cpacket.cpp: VERB_UPDATE 92 passes its GameID, &Drops, &Adds)  // NAME 1 [game_id, drops, adds]: the packet's fields

	char Unknown00[0x14];
};

// what every view holds at +0x34 (a 0x6B4480 result), as far as cpacket.cpp calls it
class ViewUnknown34Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_588A70(const char *type, int unknown1, const char *text, int flags, int unknown2, int unknown3);	// 0x588A70 (102 callers; GROUP 30, DIALOG 98: "GROUP_INVITE", "OK", "YES_NO", "OK_CANCEL", "server_message")  // NAME 1 [type, unknown1, text, flags, unknown2, unknown3]: neutral, the handlers' arguments
};

// view.cpp's view: the base of the six views at SClient +0x1354..+0x1368 (the slots every one of them
// gets: the deleting destructor, +0x04 after construction, +0x10 in Shutdown)
class ViewClass			// NAME 2: what the six objects share (kernel-map-step9.md 9f's view.cpp)
{
public:
	virtual ~ViewClass();								// +0x00 (Shutdown deletes through it)
	virtual void Unknown_04();							// +0x04 Init calls it after construction
	virtual void Unknown_08();							// +0x08 Init (the main view, +0x1358)
	virtual void Unknown_0C(bool on);					// +0x0C Process_Packets passes true to the loading view
	virtual void Unknown_10();							// +0x10 Shutdown

	ViewUnknown34Class *Get_Unknown34() { return(Unknown34); }	// inline (GROUP 30 and DIALOG 98 read it into eax first)  // NAME 1: neutral (D10): the member it returns

	char Unknown04[0x34 - 0x04];						// +0x04
	ViewUnknown34Class *Unknown34;						// +0x34 (view.cpp's constructors 0x65A6C0 / 0x65B810 set it; cpacket.cpp's GROUP 30 and DIALOG 98 call it)
};

// radar_system.cpp's radar (the main view's +0x80), declared as far as cpacket.cpp calls it
class RadarSystemClass	// NAME 3: class-from-file (D24) for radar_system.cpp
{
public:
	void Unknown_7BD920(int index);						// 0x7BD920 (cpacket.cpp: WARP_INDEX 156)  // NAME 1 [index]: the packet's Index
};

// ui_cpit.cpp's cockpit interface (the main view's +0x1D0), declared as far as cpacket.cpp calls it
class UICpitClass	// NAME 3: class-from-file (D24) for ui_cpit.cpp
{
public:
	void Unknown_581F90(ClientObjectClass *hulk);		// 0x581F90 (cpacket.cpp: LOOT_HULK_PERMISSION 140)  // NAME 1 [hulk]: the packet's HulkGameID's object
	void Unknown_5820C0(bool on);						// 0x5820C0 (cpacket.cpp: OPEN_INTERFACE 102 passes UIChange == 0)  // NAME 1 [on]: what the handler passes
	void Unknown_584A60();								// 0x584A60 (SClient 0x745160)
};

// what the main view's slot +0x64 returns, as far as SClient 0x7456B0 calls it
class MainViewUnknown64Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24(int arg);					// +0x24 (0x7456B0 passes 0)  // NAME 1 [arg]: neutral
};

// what the main view's +0x13C points to, as far as SERVER_PARAMETERS 66 reads it
class MainViewUnknown13CClass	// NAME 1: neutral (D10)
{
public:
	int Get_Unknown90() { return(Unknown90); }		// (inline: SERVER_PARAMETERS 66 reads it through eax)  // NAME 1: neutral (D10): the member it returns

	char Unknown00[0x90];								// +0x00
	int Unknown90;										// +0x90
};

// mainview.cpp's view (constructor 0x50B360, 0x1EC bytes): SClient's Add_Object / Remove_Object tell it
class MainViewClass : public ViewClass	// NAME 3: class-from-file (D24) for mainview.cpp
{
public:
	MainViewClass(ClientTaskClass *task);				// 0x50B360  // NAME 1 [task]: SClient::Init passes its task
	void Set_Camera(CameraClass *camera);				// 0x5123C0: Init's camera  // NAME 2: what Init hands it
	void Unknown_5141A0();								// 0x5141A0 (Init, Unknown137C clear)
	void Unknown_514220();								// 0x514220 (Init, Unknown137C set)
	MainViewUnknown13CClass *Get_Unknown13C() { return(Unknown13C); }	// (inline: SERVER_PARAMETERS 66)  // NAME 1: neutral (D10): the member it returns
	void Unknown_513FC0();								// 0x513FC0 (cpacket.cpp: SERVER_HANDOFF 58)
	void Unknown_513700(unsigned sector);				// 0x513700 (cpacket.cpp: SERVER_PARAMETERS 66)  // NAME 1 [sector]: the packet's SectorNum
	void Unknown_5137F0(unsigned sector);				// 0x5137F0 (cpacket.cpp: SERVER_PARAMETERS 66, view mode 1)  // NAME 1 [sector]: the packet's SectorNum
	void Unknown_513B90(unsigned sector);				// 0x513B90 (cpacket.cpp: SERVER_PARAMETERS 66, view mode 5)  // NAME 1 [sector]: the packet's SectorNum
	void Unknown_513540(int base_asset);				// 0x513540 (SClient 0x745000)  // NAME 1 [base_asset]: SERVER_PARAMETERS 66's BackdropBaseAsset
	void Unknown_513600(int arg);						// 0x513600 (SClient 0x7456B0 passes 0)  // NAME 1 [arg]: neutral
	void Unknown_513CD0(int mode);						// 0x513CD0 (SClient 0x7456B0 passes the base's Unknown10F4, DebrisMode)  // NAME 1 [mode]: what it gets
	void Unknown_513D20(float value);					// 0x513D20 (SClient 0x7456B0 passes 1.0)  // NAME 1 [value]: neutral
	void Unknown_513F60(FileClass *file, int indent);	// 0x513F60 (SClient 0x745EC0's dump, two deeper)  // NAME 1 [file, indent]: Printf_Indented's
	void Unknown_514010(int old_mode, int mode);		// 0x514010 (SClient 0x745F30: the view mode before and after)  // NAME 1 [old_mode, mode]: what it gets
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();
	virtual void Unknown_3C();
	virtual void Unknown_40();
	virtual void Unknown_44();
	virtual void Unknown_48();
	virtual void Unknown_4C();
	virtual void Unknown_50();
	virtual void Unknown_54();
	virtual void Unknown_58();
	virtual ClientObjectClass *Unknown_5C();			// +0x5C (SClient 0x744E70's FAIL_IF '!target' on its result)
	virtual void Unknown_60();
	virtual MainViewUnknown64Class *Unknown_64();		// +0x64 (SClient 0x7456B0)
	virtual void Unknown_68();
	virtual void Unknown_6C();
	virtual void Unknown_70();
	virtual void Unknown_74();
	virtual void Unknown_78();
	virtual int Unknown_7C();							// +0x7C (OBJECT_EFFECT 9 hands its result to Add_Effect)
	virtual void Add_Object(ClientObjectClass *object);		// +0x80 SClient::Add_Object  // NAME 2: its caller's
	virtual void Remove_Object(ClientObjectClass *object);	// +0x84 SClient::Remove_Object  // NAME 2: its caller's

	char Unknown38[0x80 - 0x38];						// +0x38
	RadarSystemClass *Radar;							// +0x80 (cpacket.cpp: WARP_INDEX 156's FAIL_IF '!radar')  // NAME 3: the FAIL_IF's name
	char Unknown84[0x130 - 0x84];						// +0x84
	bool Unknown130;									// +0x130 (SClient 0x7456B0 sets it)
	char Unknown131[0x13C - 0x131];						// +0x131
	MainViewUnknown13CClass *Unknown13C;				// +0x13C (SERVER_PARAMETERS 66 copies its +0x90 to the client's +0x1144 in view mode 1)
	char Unknown140[0x1D0 - 0x140];						// +0x140
	UICpitClass *Cpit;									// +0x1D0 (cpacket.cpp: LOOT_HULK_PERMISSION 140, OPEN_INTERFACE 102)  // NAME 3: its file's (ui_cpit.cpp)
	char Unknown1D4[0x1EC - 0x1D4];						// +0x1D4
};

// loadingview.cpp's view (constructor 0x691570, 0x6C bytes)
class LoadingViewClass : public ViewClass	// NAME 3: class-from-file (D24) for loadingview.cpp
{
public:
	LoadingViewClass(ClientTaskClass *task);			// 0x691570  // NAME 1 [task]: SClient::Init passes its task
	void Unknown_691830(const std::string &source_sector, const std::string &source_system, const std::string &name, const std::string &system);	// 0x691830 (cpacket.cpp: SERVER_HANDOFF 58)  // NAME 1 [source_sector, source_system, name, system]: the packet's fields

	char Unknown38[0x68 - 0x38];						// +0x38
	bool Unknown68;										// +0x68 ClientTaskClass::Render renders when it is set
};

// The 3-D movie view (constructor 0x7EEF90, 0x2A8 bytes; SClient 0x74BC20 makes it, 0x74BD10 deletes it)
class Movie3DViewClass : public ViewClass	// NAME 2: SClient's member 'Movie3DView' (G_ASSERT l.4104)
{
public:
	Movie3DViewClass(ClientTaskClass *task, int arg);	// 0x7EEF90  // NAME 1 [task, arg]: what 0x74BC20 passes

	char Unknown38[0x2A8 - 0x38];						// +0x38
};

class SClientMovieCallbackClass;

// The scripted movie view (constructor 0x8000C0, 0x178 bytes; SClient 0x74BD90 makes it, 0x74BF00 deletes it)
class ScriptedMovieViewClass : public ViewClass	// NAME 2: SClient's member 'ScriptedMovieView' (G_ASSERT l.4127)
{
public:
	ScriptedMovieViewClass(ClientTaskClass *task, int arg, SClientMovieCallbackClass *callback);	// 0x8000C0  // NAME 1 [task, arg, callback]: what 0x74BD90 passes

	char Unknown38[0x178 - 0x38];						// +0x38
};

// The 0x120 bytes at SClient +0x13CC: 24 colours set to (1, 1, 1) by the out-of-line constructor 0x764FD0
// (between proj_decal_manager.cpp and tsk_game.cpp; file unknown)
class Unknown764FD0Class	// NAME 1: neutral (D10)
{
public:
	Unknown764FD0Class();								// 0x764FD0
	void Unknown_765010();								// 0x765010 (SClient::Init)

	Vector3 Unknown00[24];								// +0x00  // NAME 2: what the constructor writes
};

// What the +0x1394 values' 0x77E3F0 / 0x77E610 return, as far as SClient calls it
class Unknown77E3F0Class	// NAME 1: neutral (D10)
{
public:
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
	Unknown77E3F0Class *Unknown_77E3F0();				// 0x77E3F0 (SClient 0x74C7B0)
	Unknown77E3F0Class *Unknown_77E610();				// 0x77E610 (SClient 0x74C840)

	char Unknown08[0xF8 - 0x08];						// +0x08
	int UnknownF8;										// +0xF8 (SClient 0x74C960 returns its address)
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
	virtual unsigned Set_Debug_Object(unsigned id);		// +0x24 0x745730: the base's
	virtual bool Unknown_34(int arg);					// +0x34 0x74ABC0: the chat line's '/' commands ('/group', 'Unable to camp - insufficient marshmallows.')
	virtual void Set_Debug_Mode(bool on);				// +0x3C 0x743330: the base's, then 'Debug mode turned %s.'
	virtual void Add_Object(ClientObjectClass *object);	// +0x40 0x744B90: the base's, then the main view's
	virtual ClientObjectClass *Remove_Object(unsigned id);	// +0x44 0x744BD0: the base's, then the main view's
	virtual void Load_INIs();							// +0x5C 0x740F60: action.ini, cskill_*.ini, testeffects.ini, gutter_color.ini, cbasset.ini  // NAME 2: what it does
	virtual void Unload_INIs();							// +0x60 0x741B70: Load_INIs undone by its last user  // NAME 2: what it does
	virtual void Unknown_64();							// +0x64 0x744690: ClientTaskClass::Render, Wait_For_Redirect

	void Print(int channel, const char *format, ...);	// 0x74DF70 (cdecl): a line in the chat panel  // NAME 2: what it does
	void Unknown_742530(ViewClass *view);				// 0x742530 (Shutdown passes 0; 0x745F30 the view of the new view mode)  // NAME 1 [view]: what it gets
	void Unknown_7451D0();								// 0x7451D0 (Init)
	void Unknown_745350();								// 0x745350 (Init)
	void Unknown_744C10();								// 0x744C10 (Init, after the camera mode)
	void Unknown_744E50();								// 0x744E50: the camera manager's callback (Init registers it, Shutdown removes it)
	void Unknown_7518A0();								// 0x7518A0 (Init)
	void Unknown_7555B0();								// 0x7555B0 (Shutdown)
	void Unknown_743290();								// 0x743290 (cpacket.cpp: LOGOFF_CONFIRMATION 186)
	bool Unknown_744B60();								// 0x744B60 (cpacket.cpp: REMOVE 7, GROUP 30)
	void Unknown_74C530(unsigned id);					// 0x74C530 (cpacket.cpp: REMOVE 7 passes its GameID)  // NAME 1 [id]: the packet's GameID
	void Unknown_74B600();								// 0x74B600 (cpacket.cpp: mode change 76, mode 0)
	void Unknown_74B640();								// 0x74B640 (cpacket.cpp: mode change 76, mode 1)
	void Unknown_74C3B0(int game_id, AvatarDescClass &desc);	// 0x74C3B0 (cpacket.cpp: AVATAR_DESCRIPTION 97 passes its GameID and Desc)  // NAME 1 [game_id, desc]: the packet's fields
	void Unknown_745750();								// 0x745750 (cpacket.cpp: SERVER_HANDOFF 58)
	void Unknown_745000(int base_asset);				// 0x745000 (cpacket.cpp: SERVER_PARAMETERS 66 passes its BackdropBaseAsset)  // NAME 1 [base_asset]: the packet's field
	void Unknown_750A70(unsigned sector);				// 0x750A70 (cpacket.cpp: SERVER_PARAMETERS 66 passes its SectorNum)  // NAME 1 [sector]: the packet's field
	void Unknown_74BB50(int type);						// 0x74BB50: 'Logging off to desktop.' (1), '... login screen.' (2), '... character selection.' (3) when Unknown138C is set, then LOGOFF_REQUEST on the connection  // NAME 1 [type]: what it selects
	bool Unknown_745490();								// 0x745490: the current camera mode saved into 'cam<character id>.ini' (angles in degrees)
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
	int *Unknown_74C960(unsigned id);					// 0x74C960: Unknown1394's value's +0xF8  // NAME 1 [id]: the table's key
	void Unknown_74C9D0(const RefCountPtr<UITalkTreeSpaceClass> &tree);	// 0x74C9D0: Unknown1298 set, or reset to Unknown129C  // NAME 1 [tree]: what it holds
	void Unknown_745F30(int type);						// 0x745F30 (cpacket.cpp: CLIENT_TYPE 60 passes 0, 1 or 5 for the packet's Type 0, 1 or 2)  // NAME 1 [type]: what the handler passes
	void Unknown_74B680(int a, int starbase_id);		// 0x74B680 (cpacket.cpp: starbase tow 80 passes 0 and StarbaseID1)  // NAME 1 [a, starbase_id]: what the handler passes
	void Unknown_74BA30(int a, int starbase_id, int mode);	// 0x74BA30 (cpacket.cpp: STARBASE_SET 79 passes 0, StarbaseID and 0 / 1 / 5 by its exit mode)  // NAME 1 [a, starbase_id, mode]: what the handler passes
	void Unknown_74D990(int color, char *text, int a);	// 0x74D990 (cpacket.cpp: MESSAGE_STRING 29 passes 0 last)  // NAME 1 [color, text, a]: what the handler passes
	void Unknown_74E710(const std::string &message, int id, int type);	// 0x74E710 (cpacket.cpp: CONFIRMED_ACTION_OFFER 190)  // NAME 1 [message, id, type]: the packet's fields
	void Unknown_74EB00(int type, int id);				// 0x74EB00 (cpacket.cpp: the confirmed action's timeout 191)  // NAME 1 [type, id]: the packet's fields
	void Unknown_751460(int points_total, int points_spent);	// 0x751460 (cpacket.cpp: the referral points 233)  // NAME 1 [points_total, points_spent]: the packet's fields
	void Unknown_7517E0(int a, int b, int c, int d);		// 0x7517E0 (cpacket.cpp: GUILD_PLAYER_PERMISSIONS 210 passes its four fields)  // NAME 1 [a, b, c, d]: neutral
	void Unknown_751850(std::vector<GuildRankNameStruct> &ranks);	// 0x751850 (cpacket.cpp: GUILD_RANK_NAMES_SECTOR 211)  // NAME 1 [ranks]: the packet's field
	void Unknown_745160();								// 0x745160: the cockpit's 0x584A60 after clearing Unknown1198
	void Unknown_745050();								// 0x745050: 0x745000 again with the kept base asset (scriptedmovie.cpp's neighbourhood 0x8010A0)
	void Unknown_7456B0();								// 0x7456B0: the main view's debris mode and four resets
	void Unknown_745EC0(FileClass *file, int indent);	// 0x745EC0: 'Client %x' and the main view's dump  // NAME 1 [file, indent]: Printf_Indented's
	ClientObjectClass *Unknown_74BF80();				// 0x74BF80: the object of Unknown1328, else NULL
	void Unknown_74C370(RefCountClass *object);			// 0x74C370: Unknown12B8's reference-counted setter (0x7EFA40 calls it)  // NAME 1 [object]: neutral
	void Unknown_74FA30();								// 0x74FA30 (0x750210)
	void Unknown_74FEC0();								// 0x74FEC0 (0x750210)
	void Unknown_750210();								// 0x750210: 0x74FA30, 0x74FEC0, then the chat panel's +0xE0
	void Unknown_750B30(int arg);						// 0x750B30: Unknown1330's 0x43C660  // NAME 1 [arg]: neutral

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
	ViewClass *CurrentView;								// +0x1350 (0x7429B0's FAIL_IF '!CurrentView')
	MainViewClass *MainView;							// +0x1354 Init  // NAME 3: its file's
	ViewClass *Unknown1358;								// +0x1358 Init (0x514CC0, 0x50 bytes)
	ViewClass *Unknown135C;								// +0x135C
	Movie3DViewClass *Movie3DView;						// +0x1360 view mode 3 (the client's G_ASSERT names, ll.4090 / 4104)
	ScriptedMovieViewClass *ScriptedMovieView;			// +0x1364 view mode 6 (the client's G_ASSERT names, ll.4117 / 4127)
	LoadingViewClass *LoadingView;						// +0x1368 Init  // NAME 3: its file's
	int ViewMode;										// +0x136C (2; 3: the 3-D movie view, 6: the scripted movie view)  // NAME 2: what Do_Frame's asserts show
	int Unknown1370;									// +0x1370 (3)
	int Unknown1374;									// +0x1374 (3)
	int Unknown1378;									// +0x1378
	bool Unknown137C;									// +0x137C
	int Unknown1380;									// +0x1380
	ClientTaskClass *Task;								// +0x1384 the constructor's argument  // NAME 2: what it holds
	bool Unknown1388;									// +0x1388
	int Unknown138C;									// +0x138C (1 after '/camp')
	int Unknown1390;									// +0x1390
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
