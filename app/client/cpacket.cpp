/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\client\cpacket.cpp (its asserts' __FILE__; client 0x7333B0-0x73F680)
**	The packets' handlers on the client side: PacketClass slot +0x08, Process_Packet(SClient *). Notes at the end.
*/
#include <windows.h>
#include "eff_o.h"
#include "sclient.h"
#include "ui_line_display.h"
#include "co_data.h"
#include "co_subtypes.h"
#include "..\packets\pk_player_object_relationship.h"
#include "..\packets\pk_sub_parts.h"
#include "..\packets\pkclickindicator.h"
#include "..\packets\pkclientavatar.h"
#include "..\packets\pkclientship.h"
#include "..\packets\pkconfirmedaction.h"
#include "..\packets\pkcreateattachment.h"
#include "..\packets\pkdialog.h"
#include "..\packets\pkgalaxymap.h"
#include "..\packets\pkgroup.h"
#include "..\packets\pkguildranknamesrequestclient.h"
#include "..\packets\pkhullupgrade.h"
#include "..\packets\pkincapacitancerequest.h"
#include "..\packets\pkinitrenderstate.h"
#include "..\packets\pkjoblist.h"
#include "..\packets\pklogoff.h"
#include "..\packets\pk_classes.h"
#include "..\packets\pkmessageline.h"
#include "..\packets\pkloothulkpermission.h"
#include "..\packets\pkloungenpc.h"
#include "..\packets\pkmanufactureitemcatagorypacket.h"
#include "..\packets\pkmessagestring.h"
#include "..\packets\pkmessagetime.h"
#include "..\packets\pkmodechange.h"
#include "..\packets\pknavdelete.h"
#include "..\packets\pknotelist.h"
#include "..\packets\pkopeninterface.h"
#include "..\packets\pknamedecal.h"
#include "..\packets\pkpointeffect.h"
#include "..\packets\pkreferralpointsavailable.h"
#include "..\packets\pkremove.h"
#include "..\packets\pkserverhandoff.h"
#include "..\packets\pkserverparameters.h"
#include "..\packets\pksetclienttype.h"
#include "..\packets\pkrequesttarget.h"
#include "..\packets\pksetzband.h"
#include "..\packets\pksoundcommand.h"
#include "..\packets\pkunknown_877640.h"
#include "..\packets\pkspline.h"
#include "..\packets\pkstarbasetow.h"
#include "..\packets\pktalktreeaction.h"
#include "..\packets\pktriggeremote.h"
#include "..\packets\pkverblist.h"
#include "..\packets\pkwarpindex.h"
#include "..\packets\pkrequesttime.h"
#include "wwmath.h"
#include "aux_roots.h"
#include "..\shared\auxdata\linearbu.h"
#include "..\shared\spln_db.h"
#include "wwprofile.h"
#include "rendobj.h"
#include "light.h"
#include "nullrobj.h"
#include "influence.h"
#include "postate.h"
#include "..\packets\pkcreate.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
// rs_track.cpp's render object (Class_ID 0x10001): the render states the packets switch, declared as far as
// the handlers call it
#define RSTRACK_CLASSID	0x10001	// NAME 2: the class id the handlers test
class RsTrackClass	// NAME 3: class-from-file (D24) for app\shared\rs_track.cpp
{
public:
	void Unknown_4604D0(int render_state_id);	// 0x4604D0 (ACTIVATE_RENDER_STATE 48)
	void Unknown_460780(int render_state_id);	// 0x460780 (DEACTIVATE_RENDER_STATE 50)
	void Unknown_460850(int render_state_id);	// 0x460850 (ACTIVATE_NEXT_RENDER_STATE 49)
	void Unknown_460880(int render_state_id, int link_id);	// 0x460880 (INIT_RENDER_STATE 47)
};

// sound_manager.cpp's sound manager (the global 0xBE82A4), declared as far as the sound packets call it
class SoundManagerClass	// NAME 3: class-from-file (D24) for app\client\sound_manager.cpp
{
public:
	void Unknown_75AE30(const char *name, int channel);	// 0x75AE30 (CLIENT_SOUND 106, Queue set)  // NAME 1 [name, channel]: the packet's fields
	void Unknown_75B0D0(const char *name, int channel);	// 0x75B0D0 (CLIENT_SOUND 106, Queue clear)  // NAME 1 [name, channel]: the packet's fields
	void Unknown_75B170(int channel);	// 0x75B170 (SOUND_COMMAND 107)  // NAME 1 [channel]: the packet's field
	void Unknown_75B2B0(int channel);	// 0x75B2B0 (SOUND_COMMAND 107)  // NAME 1 [channel]: the packet's field
};
extern SoundManagerClass *SoundManager;	// 0xBE82A4  // NAME 2: what it holds

// navigation_data.cpp's and the emote data's per-object records (co_data.h's Get_Navigation_Data /
// Get_Emote_Data), declared as far as the handlers call them
class NavigationDataClass	// NAME 3: class-from-file (D24) for app\client\navigation_data.cpp
{
public:
	void Unknown_7BA940(NavigationPacketClass *packet);	// 0x7BA940 (NAVIGATION 153)  // NAME 1 [packet]: the handler's packet
};
class GutterDataClass	// NAME 3: class-from-file (D24) for app\client\gutter_data.cpp
{
public:
	void Unknown_7B6040(bool turn_on, MainViewClass *mainview);	// 0x7B6040 (the click indicator 105)  // NAME 1 [turn_on, mainview]: the packet's TurnOn, SClient's main view
};
class EmoteDataClass	// NAME 2: co_data.h's name for what Get_Emote_Data returns
{
public:
	void Unknown_7E6650(int emote);	// 0x7E6650 (NOTIFY_EMOTE 162)  // NAME 1 [emote]: the packet's Emote
};

// shiputils.cpp's decal helper (cdecl): the packet's decals on the object's model, with the decal INI
void Unknown_80E8C0(RenderObjClass *model, std::vector<DecalStruct> &decals, SClientUnknown1334Class *decal_ini);	// 0x80E8C0  // NAME 1 [model, decals, decal_ini]: what DECAL 16 passes

// shiputils.cpp's colorization helper (cdecl): the packet's regions on the object's model
void Unknown_80EFF0(RenderObjClass *model, std::vector<ColorizationRegionStruct> &regions, SClientUnknown1338Class *unknown);	// 0x80EFF0  // NAME 1 [model, regions, unknown]: what COLORIZATION 17 passes

// co.h's +0xC8 Colorization: the object's colorization regions, which COLORIZATION 17 assigns
// (0x73F770, their vector's operator=)


// eff_o.cpp's effects, their factory (cdecl) and the ends the packets fill: eff_o.h, included first

#include "..\packets\pkstart.h"
#include "..\packets\pkgadget.h"
// NOT EXACT: register-only (the client loads PlayerID for START_ACK into eax before the temporary's lea and stores the EH state after the push; attempts.py show 0x733440)
// 0x733440 (placed here, out of address order, so that no assert line moves)
bool StartPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("StartPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown1138 = PlayerID;
	sclient->Unknown_745160();
	sclient->LoadingView->Unknown_692330();
	sclient->Send_Packet(&RequestTimePacketClass());
	sclient->Send_Packet(&StartAckPacketClass(PlayerID));
	return(true);
}

#include "..\packets\pkstarbase_avatar_change.h"
#include "..\packets\pkrecustomize.h"
// ui_starbase.cpp's starbase interface (the starbase view's +0x60), declared as far as the starbase avatar packets call it
class UIStarbaseClass : public UIUnknownBaseClass, public RefCountClass	// NAME 3: class-from-file (D24) for app\client\ui_starbase.cpp
{
public:
	void Unknown_63CF30(int avatar_id, int new_room);	// 0x63CF30 (STARBASE_AVATAR_ROOM_UPDATE 160)  // NAME 1 [avatar_id, new_room]: the packet's fields
	void Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action);	// 0x63E5C0 (STARBASE_AVATAR_UPDATE 158)  // NAME 1 [avatar_id, orient, position, action]: the packet's fields
	char Unknown40[0x7C - 0x40]; bool Unknown7C; char Unknown7D[0xAC - 0x7D];	// +0x40; +0x7C both handlers test it
	int Room;	// +0xAC (158 moves an avatar only in this room)  // NAME 2: what the handler compares with the packet's Room
	void Unknown_6437A0(int unknown, AvatarDescClass *desc);	// 0x6437A0 (RECUSTOMIZE_AVATAR_UPDATE 133)  // NAME 1 [unknown, desc]: the packet's fields
};

class SBaseViewUnknown1A0Class { public: void Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d); };	// NAME 1: neutral (D10): the starbase view's +0x1A0 (RECUSTOMIZE_SHIP_START 129: 0x61F250)  // NAME 1 [desc, cost, a, b, c, d]: the packet's fields
class SBaseViewUnknown19CClass { public: void Unknown_551C40(RecustomizationCostClass *cost); };	// NAME 1: neutral (D10): the starbase view's +0x19C (RECUSTOMIZE_AVATAR_START 131: 0x551C40)  // NAME 1 [cost]: the packet's field

void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 (unknown_debug.h's reporter)
// SClient +0x12C4's elements: the player-variable aux data roots (an UnknownSendRootClass, 0xADC bytes, made
// in 0x7518A0), declared as far as PLAYER_VAR_AUX_DATA 28 calls them
class SClientUnknown12C4Class : public UnknownSendRootClass
{
};
inline void Unknown_Set_Colorization(ClientObjectClass *object, const std::vector<ColorizationRegionStruct> &regions) { object->Colorization = regions; }

// 0x7333B0
bool LogoffConfirmationPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("LogoffConfirmationPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown_743290();
	return(true);
}

// (the debug object named: the client loads its model straight into ecx)
// 0x733590
bool ClientAvatarPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ClientAvatarPacketClass::Process_Packet()");
	ClientClass *client = (ClientClass *)instance;
	client->Set_Debug_Object(AvatarID);
	ClientObjectClass *object = client->Get_Avatar(); object->Model->Set_LOD_Bias(1000000000.0f);
	return(true);
}

// 0x733660
bool ClientShipPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ClientShipPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Set_Ship_ID(ShipID);
	return(true);
}

// 0x733700
bool CreatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("CreatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *obj;
	switch (Type) {
		case 24:
			obj = new Unknown70E2E0Class(sclient, Type, GameID);
			break;
		case 28:
			obj = new Unknown70E5A0Class(sclient, GameID);
			break;
		default:
			obj = new ClientObjectClass(sclient, Type, GameID);
			break;
	}
	G_ASSERT(obj);

	// The object's model: a light for type 8, otherwise the base asset's model (co.cpp's 0x6F5140,
	// at the packet's scale and HSV shift). The packet's Type is CREATE's object type: 24 and 28
	// have their own classes, 10 also gets an influence record and an empty model (below), 2 a
	// low LOD bias.

	RenderObjClass *robj;
	if (Type != 8) {
		robj = ClientObjectClass::Unknown_6F5140(BaseAsset, sclient, Scale, HSV);
	} else {
		robj = new LightClass(LightClass::POINT);
	}
	G_ASSERT(robj);
	if (Type == 2) {
		robj->Set_LOD_Bias(0.1f);
	}
	obj->Unknown_6F33F0(robj, BaseAsset, Scale);
	obj->Unknown_04(BaseAsset);
	sclient->Add_Object(obj);
	if (Type == 10) {
		InfluenceClass *influence = new InfluenceClass(sclient, --sclient->Unknown113C, robj);
		influence->Unknown_10(GameID);
		sclient->Add_Unknown86C(influence);
		RenderObjClass *null_obj = new Null3DObjClass("NULL");
		obj->Unknown_6F33F0(null_obj, -1, 1.0f);
		null_obj->Release_Ref();
	}
	robj->Release_Ref();
	return(true);
}

// ui_jobs.cpp's interface (an InterfaceClass: its reference count at +0x3C) and sbaseview.cpp's view
// (SClient +0x135C; LOUNGE_NPC 82 calls its 0x6A0FD0) that holds it, declared as far as the job packets call them
class UIJobsClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_jobs.cpp
{
public:
	void Unknown_5A3BC0(JobListPacketClass *packet);	// 0x5A3BC0 (JOB_LIST 147)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3F10(JobDescriptionPacketClass *packet);	// 0x5A3F10 (JOB_DESCRIPTION 148)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3FB0(JobAcceptReplyPacketClass *packet);	// 0x5A3FB0 (JOB_ACCEPT_REPLY 150)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A4430(int job_id);	// 0x5A4430 (JOB_DELETE 149)  // NAME 1 [job_id]: the packet's JobID
};
class SBaseViewClass : public ViewClass	// NAME 3: class-from-file (D24) for app\client\sbaseview.cpp
{
public:
	void Unknown_6A0FD0(LoungeNPCPacketClass *packet);	// 0x6A0FD0 (LOUNGE_NPC 82)  // NAME 1 [packet]: the handler's packet
	RefCountPtr<UIJobsClass> Get_Jobs() { return(Jobs); }	// 0x73A300 (out of line after JOB_LIST 147, the first caller)  // NAME 2: what it returns
	RefCountPtr<UIStarbaseClass> Get_Starbase() { return(Starbase); }	// 0x54EDA0 (out of line in another object)  // NAME 2: what it returns
	char Unknown38[0x60 - 0x38]; RefCountPtr<UIStarbaseClass> Starbase; char Unknown64[0x194 - 0x64];	// +0x38; +0x60 ui_starbase.cpp's interface  // NAME 2 [Starbase]: what it holds
	RefCountPtr<UIJobsClass> Jobs; int Unknown198; SBaseViewUnknown19CClass *AvatarUI; SBaseViewUnknown1A0Class *ShipUI;	// +0x194  // NAME 2 [Jobs]: what it holds; +0x19C / +0x1A0  // NAME 3 [AvatarUI, ShipUI]: the recustomize handlers' '!avatar_ui' / '!ship_ui' tests
};

// ui_talkt.cpp's talk tree interface and ui_talkt_space.cpp's (SClient +0x1298), declared as far as the
// talk tree packets call them
class UITalkTreeClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_talkt.cpp
{
public:
	virtual void Unknown_88();
	virtual void Unknown_8C();
	virtual void Unknown_90(TalkTreePacketClass *packet);	// +0x90 (TALK_TREE 84)  // NAME 1 [packet]: the handler's packet
	void Unknown_64D230(int action);	// 0x64D230 (TALK_TREE_ACTION 86)  // NAME 1 [action]: the packet's Action
};
class UITalkTreeSpaceClass : public UITalkTreeClass	// NAME 3: class-from-file (D24) for app\client\ui_talkt_space.cpp
{
};

// galaxy_map_client.cpp's map (SClient +0x12A0), declared as far as GALAXY_MAP 151 calls it (each reads
// the packet's data from the buffer)
class GalaxyMapClientClass	// NAME 3: class-from-file (D24) for app\client\galaxy_map_client.cpp
{
public:
	void Unknown_7A4C50(SClient *sclient, LinearBufferClass &buffer);	// 0x7A4C50 (type 4)  // NAME 1 [sclient, buffer]: what the handler passes
	void Unknown_7A4FC0(LinearBufferClass &buffer);	// 0x7A4FC0 (type 5)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A50A0(LinearBufferClass &buffer);	// 0x7A50A0 (types 6, 7, 8, 11)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A5180(LinearBufferClass &buffer);	// 0x7A5180 (type 9)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A5260(LinearBufferClass &buffer);	// 0x7A5260 (type 2)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A52F0(LinearBufferClass &buffer);	// 0x7A52F0 (type 3)  // NAME 1 [buffer]: the packet's data
};

// co_player_relationship.cpp's handlers of the relationship packets (cdecl)
void Unknown_70CC40(SClient *sclient, PlayerObjectRelationshipPacket *packet);	// 0x70CC40 (RELATIONSHIP 137)  // NAME 1 [sclient, packet]: what the handler passes
void Unknown_70CC90(SClient *sclient, PlayerObjectReactionUpdatePacket *packet);	// 0x70CC90 (138)  // NAME 1 [sclient, packet]: what the handler passes
void Unknown_70CD50(SClient *sclient, PlayerObjectIsAttackingUpdatePacket *packet);	// 0x70CD50 (ATTACKER_UPDATES 139)  // NAME 1 [sclient, packet]: what the handler passes

// 0x733A50
bool SubPartsPacket::Process_Packet(void *instance)
{
	WWPROFILE("SubPartsPacket::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		object->Unknown_6FEF70(this);
	}
	return(true);
}

// 0x733B10
bool AvatarDescPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("AvatarDescPacketClass::Process_Packet()");
	((SClient *)((ClientClass *)instance)->Unknown_18())->Unknown_74C3B0(GameID, Desc);
	return(true);
}

// 0x733BB0
bool CreateAttachmentPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("CreateAttachmentPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *parent = sclient->Find_Object(Parent_ID);
	ClientObjectClass *child = sclient->Find_Object(Child_ID);
	if (parent && child) {
		parent->Unknown_0C(child, Unknown18, 0);
	}
	return(true);
}

// 0x733C80
bool ConstantPositionalUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ConstantPositionalUpdatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		if (!object->Unknown98) {
			object->Unknown98 = new Unknown714B50Class(this);
		} else if (object->Unknown98->Unknown_04() != 1) {
			delete object->Unknown98;
			object->Unknown98 = new Unknown714B50Class(this);
		} else {
			Unknown714B50Class *state = (Unknown714B50Class *)object->Unknown98;
			state->Unknown_714D50(this);
		}
		object->Unknown_3C();
	}
	return(true);
}
























// NOT EXACT: shape (the Is_Valid chain is the client's now; left: the clock drift's divisor converted once and the count kept in ecx, where the client converts +0x10CC again and rereads +0x10D0; attempts.py show 0x733DC0)
// 0x733DC0
bool SimplePositionalUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SimplePositionalUpdatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!(Position.Is_Valid() && Orientation.Is_Valid() && Velocity.Is_Valid())) return(true);
	TimeStamp = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	float delta = (float)(sclient->Unknown10BC - (int)TimeStamp); sclient->Unknown10D0++;
	sclient->Unknown10C8 = sclient->Unknown10C8 - sclient->Unknown10C8 / sclient->Unknown10CC + delta;
	if (fabs(sclient->Unknown10C8 / sclient->Unknown10CC) > 50.0 && sclient->Unknown10D0 > 64) {
		sclient->Unknown_728180();
	}
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		if (!object->Unknown98) {
			object->Unknown98 = new Unknown715AB0Class(this);
		} else if (object->Unknown98->Unknown_04() != 3) {
			delete object->Unknown98;
			object->Unknown98 = new Unknown715AB0Class(this);
		} else {
			Unknown715AB0Class *state = (Unknown715AB0Class *)object->Unknown98;
			state->Unknown_715C20(this);
		}
	}
	return(true);
}
// the lag average (+0x10C8 over +0x10CC): ADVANCED 62 recomputes it after the update, as the client does
static inline float Unknown_Lag_Average(SClient *sclient) { return(sclient->Unknown10C8 / sclient->Unknown10CC); }	// NAME 1: neutral (D10)

// NOT EXACT: shape (the lag average's divisor converted once, the state pointer not in ebp; the Is_Valid chain and size are the client's; attempts.py show 0x734130)
// 0x734130
bool AdvancedPositionalUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("AdvancedPositionalUpdatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!(Position.Is_Valid() && Orientation.Is_Valid() && WWMath::Is_Valid_Float(CurrentSpeed) && ImpartedVelocity.Is_Valid())) return(true);
	TimeStamp = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	float delta = (float)(sclient->Unknown10BC - (int)TimeStamp); sclient->Unknown10D0++;
	sclient->Unknown10C8 = sclient->Unknown10C8 - sclient->Unknown10C8 / sclient->Unknown10CC + delta;
	if (fabs(Unknown_Lag_Average(sclient)) > 50.0 && sclient->Unknown10D0 > 64) {
		sclient->Unknown_728180();
	}
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) { unsigned id = sclient->AvatarID;
		if (GameID == id) {
			static int requests;	// 0xBE7D6C  // NAME 1: what it counts
			if (++requests > 32) {
				sclient->Send_Packet(&RequestTimePacketClass());
				requests = 0;
			}
			if (object->Unknown98 && object->Unknown98->Unknown_04() == 4) {
				((Unknown717010Class *)object->Unknown98)->Unknown_717290(this, sclient->Unknown10BC); return(true);
			} else if (object->Unknown98 && object->Unknown98->Unknown_2C()) {
				Unknown98Class *state = object->Unknown98->Unknown_28();
				if (state && state->Unknown_04() == 4) {
					((Unknown717010Class *)state)->Unknown_717290(this, sclient->Unknown10BC); return(true);
				} else {
					delete state;
					object->Unknown98->Unknown_24(new Unknown717010Class(sclient->Unknown10BC, this)); return(true);
				}
			} else {
				delete object->Unknown98;
				object->Unknown98 = new Unknown717010Class(sclient->Unknown10BC, this); return(true);
			}
		} else {
			if (object->Unknown98 && object->Unknown98->Unknown_04() == 8) {
				((Unknown715EF0Class *)object->Unknown98)->Unknown_716140(sclient->Unknown10BC, this);
			} else if (object->Unknown98 && object->Unknown98->Unknown_2C()) {
				Unknown98Class *state = object->Unknown98->Unknown_28();
				if (state && state->Unknown_04() == 8) {
					((Unknown715EF0Class *)state)->Unknown_716140(sclient->Unknown10BC, this);
				} else {
					delete state;
					object->Unknown98->Unknown_24(new Unknown715EF0Class(sclient->Unknown10BC, this));
				}
			} else {
				delete object->Unknown98;
				object->Unknown98 = new Unknown715EF0Class(sclient->Unknown10BC, this);
			}
		}
	}
	return(true);
}
// (ClientClass +0x10C0 is a DurationClass: read as an __int64 the client's registers for its halves swap)
// 0x734A40
bool PlanetPositionalUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("PlanetPositionalUpdatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	TimeStamp = (DurationClass((unsigned)TimeStamp) - sclient->Unknown10C0).As_Int();
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		if (!object->Unknown98) {
			object->Unknown98 = new Unknown715CF0Class(this, sclient);
		} else if (object->Unknown98->Unknown_04() != 5) {
			delete object->Unknown98;
			object->Unknown98 = new Unknown715CF0Class(this, sclient);
		} else {
			Unknown715CF0Class *state = (Unknown715CF0Class *)object->Unknown98;
			state->Unknown_715E60(this);
		}
	}
	return(true);
}




































































































// (ClientClass +0x10C0 is a DurationClass: read as an __int64 the client's registers for its halves swap)
// 0x734BF0
bool ComponentPositionalUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ComponentPositionalUpdatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	TimeStamp = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	ClientObjectClass *obj = sclient->Find_Object(GameID);
	if (!obj) return(true);
	G_ASSERT(obj->Is_Component());
	if (!obj->Is_Component()) return(true);
	if (obj->Unknown98 && obj->Unknown98->Unknown_04() == 11) {
		((Unknown717EF0Class *)obj->Unknown98)->Unknown_718080(this, sclient);
	} else {
		delete obj->Unknown98;
		obj->Unknown98 = new Unknown717EF0Class(this, sclient);
	}

	return(true);
}








// (ClientClass +0x10C0 is a DurationClass: read as an __int64 the client's registers for its halves swap)
// 0x734DF0
bool ObjectEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ObjectEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	int time = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	int duration = Duration >= 0 ? Duration : -1;
	EffectObjectClass *effect = Unknown_6E9490(EffectDescID, EffectID, duration, time, sclient, Scale, HSVShift, 1.0f, -1, 0);
	FAIL_IF(!effect) return(true);
	UnknownB00060EffectClass *target = effect->Unknown_10();
	if (target) {
		target->Unknown_28(GameID);
	} else {

		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "A non-unary effect descriptor %d was given for an effect on an object", EffectDescID);
		G_ASSERT(0);
	}
	MainViewClass *view = sclient->MainView;
	sclient->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
	return(true);
}







// 0x735000
bool PointEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("PointEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	int time = (DurationClass((unsigned)TimeStamp) - sclient->Unknown10C0).As_Int();
	int duration = Duration >= 0 ? Duration : -1;
	EffectObjectClass *effect = Unknown_6E9490(EffectDescID, EffectID, duration, time, sclient, Scale, HSVShift, 1.0f, -1, 0);
	FAIL_IF(!effect) return(true);
	UnknownB00060EffectClass *target = effect->Unknown_10();
	if (target) {
		target->Unknown_2C(Matrix3D(Location));
	} else {

		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "A non-unary effect descriptor %d was given for an effect on a point", EffectDescID);
		G_ASSERT(0);
	}
	MainViewClass *view = sclient->MainView;
	sclient->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
	return(true);
}






// 0x735280
bool ObjectToObjectEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ObjectToObjectEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	int time = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	int duration = Duration >= 0 ? Duration : -1;
	EffectObjectClass *effect = Unknown_6E9490(EffectDescID, EffectID, duration, time, sclient, Scale, HSVShift, Speedup, -1, 0);
	FAIL_IF(!effect) return(true);
	UnknownB0009CEffectClass *binary = effect->Unknown_14();
	if (binary) {
		if (!SourceLego.Is_Empty()) {
			binary->Unknown_2C(SourceGameID, SourceLego);
		} else {
			binary->Unknown_28(SourceGameID);
		}
		binary->Unknown_34(TargetGameID);
		if (OutsideTargetRadius) {
			binary->Flags |= 0x2000;
		} else {
			binary->Flags &= ~0x2000;
		}
		binary->Unknown_6E82A0(Unknown36, Unknown34, TargetOffset);
	} else {

		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "A non-binary effect descriptor %d was given for an effect between two objects", EffectDescID);
		G_ASSERT(0);
	}
	MainViewClass *view = sclient->MainView;
	sclient->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
	return(true);
}






// 0x735520
bool ObjectToEffectEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ObjectToEffectEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	int time = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	int duration = Duration >= 0 ? Duration : -1;
	EffectObjectClass *effect = Unknown_6E9490(EffectDescID, EffectID, duration, time, sclient, Scale, HSVShift, Speedup, -1, 0);
	FAIL_IF(!effect) return(true);
	UnknownB0009CEffectClass *binary = effect->Unknown_14();
	if (binary) {
		if (!SourceLego.Is_Empty()) {
			binary->Unknown_2C(SourceGameID, SourceLego);
		} else {
			binary->Unknown_28(SourceGameID);
		}
		binary->Unknown_34(0);
		EffectObjectClass *target = sclient->Get_Effect(TargetGameID);
		Vector3 position = target->Unknown_08(Vector3(binary->SourcePosition));
		UnknownB0009CEffectClass *target_binary = target->Unknown_14();
		if (Unknown34 != 0 && target_binary) {
			Vector3 other = target_binary->TargetPosition; position = position * 0.5f + other * 0.5f;
		}
		binary->Unknown_38(position);
	} else {











		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "A non-binary effect descriptor %d was given for an effect between an object and an effect", EffectDescID);
		G_ASSERT(0);
	}
	MainViewClass *view = sclient->MainView;
	sclient->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
	return(true);
}








// 0x735880
bool ObjectToObjectDurationLinkedEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ObjectToObjectDurationLinkedEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	int time = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();

	EffectObjectClass *effect = Unknown_6E9490(EffectDescID, EffectID, 0, time, sclient, Scale, HSVShift, Speedup, DurationLinkedEffectDescID, 0);
	FAIL_IF(!effect) return(true);
	UnknownB0009CEffectClass *binary = effect->Unknown_14();
	if (binary) {
		if (!SourceLego.Is_Empty()) {
			binary->Unknown_2C(SourceGameID, SourceLego);
		} else {
			binary->Unknown_28(SourceGameID);
		}
		binary->Unknown_34(TargetGameID);
		if (OutsideTargetRadius) {
			binary->Flags |= 0x2000;
		} else {
			binary->Flags &= ~0x2000;
		}
		binary->Unknown_6E82A0(Unknown36, Unknown34, TargetOffset);
	} else {

		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "A non-binary effect descriptor %d was given for a duration-linked effect between two objects", EffectDescID);
		G_ASSERT(0);
	}
	MainViewClass *view = sclient->MainView;
	sclient->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
	return(true);
}







// 0x735B00
bool ObjectToPointEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ObjectToPointEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	int time = (DurationClass(TimeStamp) - sclient->Unknown10C0).As_Int();
	int duration = Duration >= 0 ? Duration : -1;
	EffectObjectClass *effect = Unknown_6E9490(EffectDescID, EffectID, duration, time, sclient, Scale, HSVShift, Speedup, -1, 0);
	FAIL_IF(!effect) return(true);
	UnknownB0009CEffectClass *binary = effect->Unknown_14();
	if (binary) {
		if (!SourceLego.Is_Empty()) {
			binary->Unknown_2C(SourceGameID, SourceLego);
		} else {
			binary->Unknown_28(SourceGameID);
		}
		binary->Unknown_38(TargetPoint);
	} else {




		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "A non-binary effect descriptor %d was given for an effect between an object and a point", EffectDescID);
		G_ASSERT(0);
	}
	MainViewClass *view = sclient->MainView;
	sclient->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
	return(true);
}

// 0x735D50
bool RemoveEffectPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("RemoveEffectPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	EffectObjectClass *effect = sclient->Get_Effect(EffectID);
	if (effect) {
		effect->Unknown_6E6E30();
	}
	return(true);
}

// 0x735DF0
bool DecalPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("DecalPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		RenderObjClass *model = object->Unknown_6F3BC0();
		if (model) {
			Unknown_80E8C0(model, Decals, sclient->Unknown1334);
			model->Release_Ref();
		}
	}
	return(true);
}

// 0x735ED0
bool ColorizationPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ColorizationPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		Unknown_Set_Colorization(object, Regions);
		RenderObjClass *model = object->Unknown_6F3BC0();
		if (model) {
			Unknown_80EFF0(model, Regions, sclient->Unknown1338);
			model->Release_Ref();
		}
	}
	return(true);
}

// 0x7362F0
bool RemovePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("RemovePacketClass::Process_Packet()");
	ClientClass *client = (ClientClass *)instance;
	if (!((SClient *)client->Unknown_18())->Unknown_744B60()) {
		((SClient *)client->Unknown_18())->Unknown_74C530(GameID);
	}
	ClientObjectClass *object = client->Find_Object(GameID);
	client->Remove_Object(GameID);
	if (object) {
		object->Unknown_08();
		delete object;
	}
	return(true);
}

// 0x7363E0
bool SetTargetPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SetTargetPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *target = sclient->Find_Object(TargetID);
	ClientObjectClass *object = sclient->Find_Object(Unknown10);
	if (Unknown10 != -1 && object && object->Unknown_6F5DC0()) {
		((Unknown70E5A0Class *)object)->Unknown_70E750(TargetID);
	}
	return(true);
}

// (exact once ClientClass +0x10C0 is a DurationClass, P-EF1-1: before, the client's Size / duration loads came before the push of 0x400)
// 0x7364C0
bool AuxDataPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("AuxDataPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	if (GameID == 0) {
		DurationClass base(sclient->Unknown10C0);
		sclient->AuxData->Decode(LinearBufferClass(Data, Size, 0x400, false), base);
		return(true);
	}
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		((DeltaCompoundClass *)object->AuxData)->Decode(LinearBufferClass(Data, Size, 0x400, false), DurationClass(sclient->Unknown10C0));
	}
	return(true);
}

// (exact once ClientClass +0x10C0 is a DurationClass, as AUX_DATA 27)
// 0x736650
bool AuxDataPlayerVarPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("AuxDataPlayerVarPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown12C4[GameID]->Decode(LinearBufferClass(Data, Size, 0x400, false), DurationClass(sclient->Unknown10C0));
	return(true);
}

// 0x736760
bool MessageStringPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("MessageStringPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown_74D990(Color, String, 0);
	return(true);
}

// 0x736800
bool MessageTimePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("MessageTimePacketClass::Process_Packet()");
	time_t now;
	char buffer[64];
	sprintf(buffer, "Server Time: %s", ctime(&Time));
	SClient *sclient = (SClient *)instance;
	sclient->Unknown_74D990(Color, buffer, 0);
	time(&now);
	sprintf(buffer, "Local Time: %s", ctime(&now));
	sclient->Unknown_74D990(Color, buffer, 0);
	return(true);
}

// 0x736930
bool GroupPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("GroupPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ViewClass *view;
	if (sclient->Unknown_744B60()) {
		view = sclient->Unknown135C;
	} else {
		view = sclient->MainView;
	}
	if (!view) {
		return(true);
	}
	switch (Type) {
		case 1:
			view->Get_Unknown34()->Unknown_588A70("GROUP_INVITE", 0, String, 0x140, 0, 0);
			break;
	}
	return(true);
}

// 0x736A20
bool PriorityMessageLinePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("PriorityMessageLinePacketClass::Process_Packet()");
	UILineDisplayClass *display = ((SClient *)instance)->LineDisplay;
	if (display) {
		display->Add_Line(Get_String(), Get_Time_MS(), Get_Ephemeral_Type(), Get_Display_Priority());
	}
	return(true);
}

// 0x736AE0
bool QueueMessageLinePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("QueueMessageLinePacketClass::Process_Packet()");
	UILineDisplayClass *display = ((SClient *)instance)->LineDisplay;
	if (display) {
		display->Add_Line_7(Get_String(), Get_Time_MS(), Get_Ephemeral_Type());
	}
	return(true);
}

// 0x736BA0
bool PushMessageLinePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("PushMessageLinePacketClass::Process_Packet()");
	UILineDisplayClass *display = ((SClient *)instance)->LineDisplay;
	if (display) {
		display->Add_Line_3(Get_String(), Get_Time_MS(), Get_Ephemeral_Type());
	}
	return(true);
}

// 0x736C60
bool SuperLinePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SuperLinePacketClass::Process_Packet()");
	UILineDisplayClass *display = ((SClient *)instance)->LineDisplay;
	if (display) {
		display->Show_Ephemeral(Get_String());
	}
	return(true);
}

// 0x736D00
bool ClearMessageLinePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ClearMessageLinePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	if (sclient->LineDisplay) {
		sclient->LineDisplay->Flush_Lines();
	}
	return(true);
}

// 0x736DA0
bool DialogPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("DialogPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ViewClass *view;
	if (sclient->Unknown_744B60()) {
		view = sclient->Unknown135C;
	} else {
		view = sclient->MainView;
	}
	if (!view) {
		return(true);
	}
	switch (Type) {
		case 0:
			view->Get_Unknown34()->Unknown_588A70("OK", 0, String, 0x140, 1, 0);
			break;
		case 1:
			view->Get_Unknown34()->Unknown_588A70("YES_NO", 0, String, 0x140, 0, 0);
			break;
		case 2:
			view->Get_Unknown34()->Unknown_588A70("OK_CANCEL", 0, String, 0x140, 0, 0);
			break;
		default:
			view->Get_Unknown34()->Unknown_588A70("server_message", 0, String, 0x140, 1, 0);
			break;
	}
	return(true);
}

// shiputils.cpp's name decal helper (cdecl): the packet's name and colour on the object's model
void Unknown_80FF20(RenderObjClass *model, const char *name, float *rgb);	// 0x80FF20  // NAME 1 [model, name, rgb]: what NAME_DECAL 178 passes

// 0x735FC0 (placed here, out of address order, so that no assert line moves)
bool NameDecalPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("NameDecalPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		object->Set_Decal_Name(Name);
		RenderObjClass *model = object->Unknown_6F3BC0();
		if (model) {
			Unknown_80FF20(model, Name, RGB);
			model->Release_Ref();
		}
	}
	return(true);
}

#include "..\audio\soundlib.h"

// camera_manager.cpp's class (sclient.cpp declares its other members): CAMERA_CONTROL 146's call
class CameraManagerClass
{
public:
	void Unknown_79B2B0(int mode);	// 0x79B2B0  // NAME 1 [mode]: CAMERA_CONTROL 146 passes 0x0C, 0x0D, 0, 0x0F, 6, 0x0A, 0x17, 0x10 or 0x19
};

// audio\musicmanager.cpp's class (sclient.cpp declares its other members)
class MusicManagerClass
{
public:
	void Unknown_43ADF0();	// 0x43ADF0 (CAMERA_CONTROL 146's case 5, after the 'Docking_Init' sound)
};

// 0x739FF0 (placed here, out of address order, so that no assert line moves; the GameID locals put the id in the dead 'this' register as the client does)
bool CameraControlPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("CameraControlPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	switch (Message) {
		case 0:
			sclient->CameraManager->Unknown_79B2B0(0x0C);
			{ int id = GameID; sclient->Unknown13AC = id; }
			break;
		case 1:
			sclient->CameraManager->Unknown_79B2B0(0x0D);
			{ int id = GameID; sclient->Unknown13B0 = id; }
			break;
		case 2:
			sclient->CameraManager->Unknown_79B2B0(0);
			break;
		case 3:
			sclient->CameraManager->Unknown_79B2B0(0x0F);
			{ int id = GameID; sclient->Unknown13B4 = id; }
			break;
		case 4:
			sclient->CameraManager->Unknown_79B2B0(6);
			{ int id = GameID; sclient->Unknown13BC = id; }
			sclient->Get_Ship()->Unknown_6FC950(true);
			sclient->Get_Ship()->Unknown_6FC880();
			break;
		case 5:
			sclient->CameraManager->Unknown_79B2B0(0x0A);
			{ int id = GameID; sclient->Unknown13AC = id; }
			sclient->Get_Ship()->Unknown_6FC950(true);
			sclient->Get_Ship()->Unknown_6FC880();
			SoundLibraryClass::Play_2D_Sound("Docking_Init", 2);
			if (sclient->MusicManager) {
				sclient->MusicManager->Unknown_43ADF0();
			}
			break;
		case 6:
			sclient->CameraManager->Unknown_79B2B0(0x17);
			break;
		case 7:
			sclient->Unknown13B4 = 0;
			sclient->CameraManager->Unknown_79B2B0(0x10);
			break;
		case 8:
			sclient->CameraManager->Unknown_79B2B0(0x19);
			break;
	}
	return(true);
}

// (the avatar branch returns at its end: the type-8 branch keeps its own Unknown_24 tail, as the client's)
// 0x734740 (placed here, out of address order, so that no assert line moves): ADVANCED 62's state handling without its checks and clock drift
bool FormationPositionalUpdatePacketClass::Process_Packet(void *instance)
{
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		unsigned id = sclient->AvatarID;
		if (GameID == id) {
			static int requests;	// 0xBE7D70  // NAME 1: what it counts
			if (++requests > 32) {
				sclient->Send_Packet(&RequestTimePacketClass());
				requests = 0;
			}
			if (object->Unknown98 && object->Unknown98->Unknown_04() == 4) {
				((Unknown717010Class *)object->Unknown98)->Unknown_717400(this, sclient);
			} else if (object->Unknown98 && object->Unknown98->Unknown_2C()) {
				Unknown98Class *state = object->Unknown98->Unknown_28();
				if (state && state->Unknown_04() == 4) {
					((Unknown717010Class *)state)->Unknown_717400(this, sclient);
				} else {
					delete state;
					object->Unknown98->Unknown_24(new Unknown717010Class(sclient, this));
				}
			} else {
				delete object->Unknown98;
				object->Unknown98 = new Unknown717010Class(sclient, this);
			} return(true);
		} else {
			if (object->Unknown98 && object->Unknown98->Unknown_04() == 8) {
				((Unknown715EF0Class *)object->Unknown98)->Unknown_716220(sclient, this);
			} else if (object->Unknown98 && object->Unknown98->Unknown_2C()) {
				Unknown98Class *state = object->Unknown98->Unknown_28();
				if (state && state->Unknown_04() == 8) {
					((Unknown715EF0Class *)state)->Unknown_716220(sclient, this);
				} else {
					delete state;
					object->Unknown98->Unknown_24(new Unknown715EF0Class(sclient, this));
				}
			} else {
				delete object->Unknown98;
				object->Unknown98 = new Unknown715EF0Class(sclient, this);
			}
		}
	}
	return(true);
}






































// 0x7371B0
bool ItemBasePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ItemBasePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	HashNodeClass<Unknown831340Class *> *inode = sclient->Unknown831340List.Find(ItemTemplateID);
	if (!inode) { sclient->Unknown831340List.Add(new Unknown831340Class(ItemTemplateID, Category, Subcategory, Info, GameBasset, IconBaseAsset, ItemType, TechLevel, Cost, MaxStack, EffectUsage, Flags, Name, Unknown30, Unknown40), ItemTemplateID); } else {
		G_ASSERT(ItemType == inode->Get()->Get_Item_Type());
		G_ASSERT(Name == inode->Get()->Get_Name());
	}
	return(true);
}



// 0x737400
bool ChangeBaseAssetPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ChangeBaseAssetPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object) {
		RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(BaseAsset, sclient, Scale, Unknown18);
		object->Unknown_6F33F0(robj, BaseAsset, Scale);
		robj->Release_Ref();
		if (object->Unknown_6F5A40() && sclient->Unknown_744B60()) {
			sclient->Unknown_74D950();
		}
	}
	return(true);
}

// 0x737520
bool SetZBandPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SetZBandPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown10D4 = Min;
	sclient->Unknown10D8 = Max;
	return(true);
}

// 0x737590
bool SetBBoxPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SetBBoxPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown10DC = XMin[0];
	sclient->Unknown10E0 = XMin[1];
	sclient->Unknown10E4 = XMax[0];
	sclient->Unknown10E8 = XMax[1];
	return(true);
}

// 0x737610
bool InitRenderStatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("InitRenderStatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object && object->Model->Class_ID() == RSTRACK_CLASSID) {
		RsTrackClass *track = (RsTrackClass *)object->Model;
		track->Unknown_460880(RenderStateID, LinkID);
	}
	return(true);
}

// 0x7376F0
bool ActivateRenderStatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ActivateRenderStatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object && object->Model->Class_ID() == RSTRACK_CLASSID) {
		((RsTrackClass *)object->Model)->Unknown_4604D0(RenderStateID);
	}
	return(true);
}

// 0x7377C0
bool ActivateNextRenderStateOncePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ActivateNextRenderStateOncePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object && object->Model->Class_ID() == RSTRACK_CLASSID) {
		((RsTrackClass *)object->Model)->Unknown_460850(RenderStateID);
	}
	return(true);
}

// 0x737890
bool DeactivateRenderStatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("DeactivateRenderStatePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *object = sclient->Find_Object(GameID);
	if (object && object->Model->Class_ID() == RSTRACK_CLASSID) {
		((RsTrackClass *)object->Model)->Unknown_460780(RenderStateID);
	}
	return(true);
}

// 0x737C10
bool SplinePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SplinePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	SplineDatabaseClass *splines = sclient->Splines;
	splines->Unpack(Block);
	return(true);
}


// NOT EXACT: shape (the __int64 sums and differences in other registers and order; frame and size as the client's; attempts.py show 0x737960)
// 0x737960 (placed after SPLINE 0x737C10 so that no assert line moves)
extern unsigned int Unknown_BE71D4;	// 0xBE71D4 (postate.cpp: the state blend time; CLIENT_SET_TIME 52 stores the round trip time in it)
bool SetClientTimePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SetClientTimePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	__int64 client_sent = ClientSent;
	__int64 server_received = ServerReceived;
	__int64 server_sent = ServerSent;
	__int64 client_received = ClientReceived;
	__int64 client_total = client_received + client_sent; __int64 server_total = server_sent + server_received;
	__int64 client_time = client_received - client_sent; __int64 server_time = server_sent - server_received; DurationClass rtt(client_time - server_time); __int64 correction = (server_total - client_total) / 2;
	Unknown_BE71D4 = rtt.As_Int();
	sclient->Unknown10C0 = correction;
	sclient->Unknown10D0 = 0;
	sclient->Unknown10C8 = 0;
	if (sclient->DebugMode) {
		char buffer[256];
		sprintf(buffer, "rtt = %u  correction = %d", rtt.As_Int(), sclient->Unknown10C0.As_Int());
		sclient->Unknown_74D990(0x17, buffer, 0);
	}
	return(true);
}



























// (exact once ClientClass +0x10C0 is a DurationClass: its operator= stores the halves where the client does)
// 0x737CB0
bool ServerHandoffPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ServerHandoffPacketClass::Process_Packet()");
	Unknown_Log("HANDOFF:INF", "Received ServerHandoff packet to: %d", Ticket.Unknown18);
	SClient *client = (SClient *)instance;
	G_ASSERT(client);
	client->Unknown_745750();
	client->MainView->Unknown_513FC0();
	client->Unknown1144 = 0;
	client->Unknown10C8 = 0;
	client->Unknown1198 = true;
	client->Unknown10C0 = 0;
	client->Unknown10D0 = 0;
	client->Unknown12D4 = 4;
	client->Ticket = Ticket;
	client->LoadingView->Unknown_691830(Source_Sector, Source_System, Name, System);
	return(true);
}













// 0x737E10
bool SetClientTypePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SetClientTypePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	Unknown_Log("HANDOFF:INF", "Client type set to %d", Type);
	switch (Type) {
		case 0:
			sclient->Unknown_745F30(0);
			break;
		case 1:
			sclient->Unknown_745F30(1);
			break;
		case 2:
			sclient->Unknown_745F30(5);
			break;
		default:
			G_ASSERT(0);
			break;
	}
	return(true);
}

// 0x737F00
bool ServerParametersPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ServerParametersPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown10D4 = ZBandMin;
	sclient->Unknown10D8 = ZBandMax;
	sclient->Unknown10DC = Min.X;
	sclient->Unknown10E0 = Min.Y;
	sclient->Unknown10E4 = Max.X;
	sclient->Unknown10E8 = Max.Y;
	sclient->Unknown10EC = FogNear;
	sclient->Unknown10F0 = FogFar;
	sclient->Unknown10F4 = DebrisMode;
	sclient->Unknown10F8 = LightBackdrop;
	sclient->Unknown10F9 = FogBackdrop;
	sclient->Unknown10FA = SwapBackdrop;
	sclient->Unknown10FC = BackdropFogNear;
	sclient->Unknown1100 = BackdropFogFar;
	sclient->Unknown1104 = MaxTilt;
	sclient->Unknown1108 = AutoLevel;
	sclient->Unknown110C = ImpulseRate;
	sclient->Unknown1110 = DecayVelocity;
	sclient->Unknown1114 = DecaySpin;
	sclient->Unknown_745000(BackdropBaseAsset);
	sclient->MainView->Unknown_513700(SectorNum);
	sclient->Unknown_750A70(SectorNum);
	switch (sclient->ViewMode) {
		case 1:
			sclient->MainView->Unknown_5137F0(SectorNum);
			sclient->Unknown1144 = sclient->MainView->Get_Unknown13C()->Get_Unknown90();
			break;
		case 5:
			sclient->MainView->Unknown_513B90(SectorNum);
			break;
	}
	return(true);
}

// 0x738100
bool ModeChangePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ModeChangePacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	switch (Mode) {
		case 0:
			sclient->Unknown_74B600();
			break;
		case 1:
			sclient->Unknown_74B640();
			break;
	}
	return(true);
}











// NOT EXACT: shape (the client keeps four calls of 0x74BA30, ours merges them; attempts.py show 0x7381C0)
// 0x7381C0
bool StarbaseSetPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("StarbaseSetPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	switch (Action) {
		case 0:
			sclient->Unknown_74B680(0, StarbaseID);
			break;
		case 1:
		{
			int mode = 0;
			switch (ExitMode) {
				case 0:
					mode = 0;
					sclient->Unknown_74BA30(0, StarbaseID, mode);
					break;
				case 1:
					mode = 1;
					sclient->Unknown_74BA30(0, StarbaseID, mode);
					break;
				case 2:
					mode = 5;
					sclient->Unknown_74BA30(0, StarbaseID, mode);
					break;
				default:
					G_ASSERT(!"Bad exit mode in StarbaseSetPacketClass::Process_Packet()");
					sclient->Unknown_74BA30(0, StarbaseID, mode);
					break;
			}
			break;
		}
	}
	return(true);
}

// 0x738330
bool StarbaseTowPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("StarbaseTowPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	sclient->Unknown_74B680(0, StarbaseID1);
	return(true);
}

// 0x7383D0
bool ClientSoundPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SoundPacketClass::Process_Packet(void * instance)");
	if (Queue) {
		SoundManager->Unknown_75AE30(SoundName, Channel);
	} else {
		SoundManager->Unknown_75B0D0(SoundName, Channel);
	}
	return(true);
}

// 0x7384A0
bool SoundCommandPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("SoundCommandPacketClass::Process_Packet(void * instance)");
	switch (Command) {
		case 0:
			SoundManager->Unknown_75B2B0(Channel);
			break;
		case 1:
			SoundManager->Unknown_75B2B0(Channel);
			SoundManager->Unknown_75B170(Channel);
			break;
		case 2:
			for (int channel = 0; channel < 16; channel++) {
				SoundManager->Unknown_75B2B0(channel);
				SoundManager->Unknown_75B170(channel);
			}
			break;
	}
	return(true);
}

// 0x7385B0
bool LoungeNPCPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("LoungeNPCPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown135C) {
		((SBaseViewClass *)sclient->Unknown135C)->Unknown_6A0FD0(this);
	}
	return(true);
}

// 0x738670
bool TalkTreePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("TalkTreePacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown1298) {
		sclient->Unknown1298->Unknown_90(this);
	}
	return(true);
}

// 0x738730
bool TalkTreeActionPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("TalkTreeActionPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown1298) {
		sclient->Unknown1298->Unknown_64D230(Action);
	}
	return(true);
}

// 0x7388B0
bool VerbUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("VerbUpdatePacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	if (sclient->VerbManager) {
		sclient->VerbManager->Unknown_81F810(GameID, &Drops, &Adds);
	}
	return(true);
}

// 0x7387F0
bool VerbListPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("VerbListPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	if (sclient->VerbManager) {
		sclient->VerbManager->Unknown_81F780(GameID, &Verbs);
	}
	return(true);
}



























#include "vector2.h"

// The 0x4E8-byte text object (constructor 0x6D47C0, as ui_line_display.h's and fontmanager.h's), as CLIENT_DAMAGE
// 100 makes it: a section name, 0 and the text
class Unknown6D47C0Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	Unknown6D47C0Class(const char *name, int unknown, const char *text);	// 0x6D47C0  // NAME 1 [name, unknown, text]: what the handler passes
	void Unknown_6D8780(float x, float y);	// 0x6D8780 (CLIENT_DAMAGE 100 passes 320, 240)  // NAME 1 [x, y]: neutral

	char Unknown08[0x4E8 - 0x08];
};

// eff_robj.cpp's 0xF0-byte effect around a text object, as far as CLIENT_DAMAGE 100 makes it
class Unknown6EF4C0Class	// NAME 1: neutral (D10)
{
public:
	Unknown6EF4C0Class(SClient *client, int id, int time, int duration, int unknown, Unknown6D47C0Class *text, const Vector2 &offset, const Vector3 &velocity);	// 0x6EF4C0  // NAME 1 [client, id, time, duration, unknown, text, offset, velocity]: what the handler passes

	virtual ~Unknown6EF4C0Class();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28(int id);	// +0x28 the object's id (as UnknownB00060EffectClass's)  // NAME 1 [id]: the packet's Victim

	char Unknown04[0xF0 - 0x04];
};
// NOT EXACT: frame (the same 487 instructions; the client gives the locals other stack slots: time, inflicted, mainview, avatar_id, type ascending; 24 forms; attempts.py show 0x738970)
// 0x738970
bool DamagePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("DamagePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *avatar = sclient->Get_Avatar();


	FAIL_IF(!avatar) return(true);
	unsigned avatar_id = avatar->GameID;
	int time = sclient->Unknown10BC;
	MainViewClass *mainview = sclient->MainView;







	FAIL_IF(!mainview) return(true);
	int type = Type;
	int inflicted = Inflicted;
	int damage = (int)(Damage + 0.5f);
	int modifier = (int)Modifer;
	char text[64];
	if (modifier > 0) {
		_snprintf(text, sizeof(text), "%d (+%d)", damage, modifier);
	} else if (modifier < 0) {
		_snprintf(text, sizeof(text), "%d (%d)", damage, modifier);
	} else {
		_snprintf(text, sizeof(text), "%d", damage);
	}
	std::string header("VictimDamageNormal");
	switch (type) {
		case 0:
		case 1:
		case 3:
			if (inflicted == 3) {
				header = "VictimDamageCritical";
			}
			break;
		case 2:
			header = "VictimDamagePlasma";
			break;
		case 4:
			header = "VictimDamageEMP";
			break;
		case 5:
			header = "VictimDamageChemical";
			break;
	}
	Unknown6D47C0Class *damage_text = new Unknown6D47C0Class(header.c_str(), 0, text);
	Unknown6D47C0Class *header_text = 0;
	if (inflicted == 3) {
		if (Victim == avatar_id) {
			header_text = new Unknown6D47C0Class("PlayerCriticalDamageHeader", 0, "Critical!");
		} else {
			header_text = new Unknown6D47C0Class("VictimCriticalDamageHeader", 0, "Critical!");
		}
	} else if (type == 4) {
		if (Victim == avatar_id) {
			header_text = new Unknown6D47C0Class("PlayerEMPHeader", 0, "EMP");
		} else {
			header_text = new Unknown6D47C0Class("VictimEMPHeader", 0, "EMP");
		}
	}
	if (Victim == avatar_id) {
		if (sclient->MainView->Cpit) {
			sclient->MainView->Cpit->Unknown_583DC0(damage_text, header_text, inflicted, type == 4);
		}
	} else if (Source == avatar_id) {
		if (damage_text) {
			damage_text->Unknown_6D8780(320.0f, 240.0f);
			int id = -(--sclient->Unknown113C);
			Unknown6EF4C0Class *effect = new Unknown6EF4C0Class(sclient, id, time, 2000, 0, damage_text, Vector2(0.0f, 0.1f), Vector3(0.0f, 0.0f, 0.0f));
			effect->Unknown_28(Victim);
			damage_text->Release_Ref();
			sclient->Add_Effect((EffectObjectClass *)effect, mainview->Unknown_48(), mainview->Unknown_7C());
		}
		if (header_text) {
			header_text->Unknown_6D8780(320.0f, 240.0f);
			int id = -(--sclient->Unknown113C);
			Unknown6EF4C0Class *effect = new Unknown6EF4C0Class(sclient, id, time, 2000, 0, header_text, Vector2(0.0f, 0.2f), Vector3(0.0f, 0.0f, 0.0f));
			effect->Unknown_28(Victim);
			header_text->Release_Ref();
			sclient->Add_Effect((EffectObjectClass *)effect, mainview->Unknown_48(), mainview->Unknown_7C());
		}
	}
	return(true);
}












// gadget.cpp's gadget, its slot +0x08's interface and gadget_list_manager.cpp's list (SClient +0x1278),
// declared as far as GADGET 103 calls them
class UIGadgetInterfaceClass : public InterfaceClass	// NAME 1: neutral (D10): what a gadget's slot +0x08 returns
{
public:
	virtual void Unknown_88();
	virtual void Unknown_8C(bool on);	// +0x8C (GADGET 103: Function bits 1 / 2)  // NAME 1 [on]: what the handler passes
};
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04();
	virtual UIGadgetInterfaceClass *Unknown_08();	// +0x08 (GADGET 103)
	virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18();
	virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28();
	virtual void Unknown_2C(); virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38();
	virtual void Unknown_3C();
	virtual void Unknown_40(bool on);	// +0x40 (GADGET 103: Function bits 4 / 8)  // NAME 1 [on]: what the handler passes
	void Unknown_6603A0(int arg, ViewClass *view);	// 0x6603A0 (GADGET 103: bit 0x10, the current view's slot +0x1C)  // NAME 1 [arg, view]: what the handler passes
	void Unknown_6607D0();	// 0x6607D0 (GADGET 103: bit 0x20)
};
class GadgetListManagerClass	// NAME 3: class-from-file (D24) for app\client\gadget_list_manager.cpp (its FAIL_IF '!Manager' on this)
{
public:
	GadgetClass *Unknown_661940(const char *name);	// 0x661940 (FAIL_IF '!name')  // NAME 3 [name]: its FAIL_IF's
};









// 0x739180
bool GadgetPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("GadgetPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	GadgetClass *gadget = ((GadgetListManagerClass *)sclient->Unknown1278)->Unknown_661940(Name);
	FAIL_IF(!gadget) return(true);
	UIGadgetInterfaceClass *iface = gadget->Unknown_08();
	if ((Function & 1) && iface) iface->Unknown_8C(true);
	if ((Function & 2) && iface) iface->Unknown_8C(false);
	if (Function & 4) gadget->Unknown_40(true);
	if (Function & 8) gadget->Unknown_40(false);
	if ((Function & 0x10) && sclient->CurrentView) gadget->Unknown_6603A0(1, sclient->CurrentView->Unknown_1C());
	if ((Function & 0x20) && sclient->CurrentView) gadget->Unknown_6607D0();
	return(true);
}


























// 0x739330
bool ManufactureSetPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ManufactureSetPacketClass::Process_Packet(void * instance)");
	((SClient *)((ClientClass *)instance)->Unknown_18())->Unknown1328 = ManufactureID;
	return(true);
}

// 0x7393D0
bool ClickIndicatorPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ClickIndicatorPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	FAIL_IF(!sclient) return(true);
	ClientObjectClass *co = sclient->Find_Object(ObjID);

	FAIL_IF(!co) return(true);
	GutterDataClass *gdata = Get_Gutter_Data(co, true);

	FAIL_IF(!gdata) return(true);
	gdata->Unknown_7B6040(TurnOn, sclient->MainView);
	return(true);
}

// 0x7396C0
bool HullUpgradePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("HullUpgradePacketClass::Process_Packet(void * instance)");
	return(true);
}

// (each failed test returns at once: the client's shared failure exit sits before the call)
// 0x7396F0
bool LootHulkPermissionPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("LootHulkPermissionPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (!sclient->MainView) return(true);
	UICpitClass *cpit = sclient->MainView->Cpit;
	if (!cpit) return(true);
	ClientObjectClass *hulk = sclient->Find_Object(HulkGameID);
	if (!hulk) return(true);
	cpit->Unknown_581F90(hulk);
	return(true);




}

















































// (each failed test returns at once: the client's shared failure exit sits before the UIType test)
// 0x7397D0
bool OpenInterfacePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("OpenInterfacePacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (!sclient->MainView) return(true);
	UICpitClass *cpit = sclient->MainView->Cpit; if (!cpit) return(true);
	if (UIType) {
		G_ASSERT(false);
	} else cpit->Unknown_5820C0(UIChange == 0);
	return(true);


}

// 0x7398E0
bool PlayerObjectRelationshipPacket::Process_Packet(void *instance)
{
	WWPROFILE("PlayerObjectRelationshipPacket::Process_Packet(void * instance)");
	ClientClass *client = (ClientClass *)instance;
	FAIL_IF(!client) return(true);
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return(true);
	Unknown_70CC40(sclient, this);
	return(true);
}


// 0x7399D0
bool PlayerObjectReactionUpdatePacket::Process_Packet(void *instance)
{
	WWPROFILE("PlayerObjectReactionUpdatePacket::Process_Packet(void * instance)");
	ClientClass *client = (ClientClass *)instance;
	FAIL_IF(!client) return(true);
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return(true);
	Unknown_70CC90(sclient, this);
	return(true);
}


// 0x739AC0
bool PlayerObjectIsAttackingUpdatePacket::Process_Packet(void *instance)
{
	WWPROFILE("PlayerObjectIsAttackingUpdatePacket::Process_Packet(void * instance)");
	ClientClass *client = (ClientClass *)instance;
	FAIL_IF(!client) return(true);
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return(true);
	Unknown_70CD50(sclient, this);
	return(true);
}


// 0x739BB0
bool IncapacitanceDetailsPacket::Process_Packet(void *instance)
{
	WWPROFILE("IncapacitanceDetailsPacket::Process_Packet(void * instance)");
	ClientClass *client = (ClientClass *)instance;
	FAIL_IF(!client) return(true);
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return(true);
	sclient->CameraManager->Unknown_79B2B0(0x19);
	sclient->MainView->Cpit->Unknown_584690(this);
	return(true);
}




// 0x739CD0
bool IncapacitanceRescueConfirmationPacket::Process_Packet(void *instance)
{
	WWPROFILE("IncapacitanceRescueConfirmationPacket::Process_Packet(void * instance)");
	ClientClass *client = (ClientClass *)instance;
	FAIL_IF(!client) return(true);
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return(true);
	sclient->Print(5, "%s", Unknown0C.c_str());
	return(true);
}

// 0x73A210
bool JobListPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("JobListPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown135C) {
		RefCountPtr<UIJobsClass> jobs = ((SBaseViewClass *)sclient->Unknown135C)->Get_Jobs();
		if (jobs) {
			jobs->Unknown_5A3BC0(this);
		}
	}
	return(true);
}

// 0x73A330
bool JobDescriptionPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("JobDescriptionPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown135C) {
		RefCountPtr<UIJobsClass> jobs = ((SBaseViewClass *)sclient->Unknown135C)->Get_Jobs();
		if (jobs) {
			jobs->Unknown_5A3F10(this);
		}
	}
	return(true);
}

// 0x73A420
bool JobDeletePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("JobDeletePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown135C) {
		RefCountPtr<UIJobsClass> jobs = ((SBaseViewClass *)sclient->Unknown135C)->Get_Jobs();
		if (jobs) {
			jobs->Unknown_5A4430(JobID);
		}
	}
	return(true);
}

// 0x73A510
bool JobAcceptReplyPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("JobAcceptReplyPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->Unknown135C) {
		RefCountPtr<UIJobsClass> jobs = ((SBaseViewClass *)sclient->Unknown135C)->Get_Jobs();
		if (jobs) {
			jobs->Unknown_5A3FB0(this);
		}
	}
	return(true);
}
























































































































// 0x73A600
bool NavigationPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("NavigationPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *co = sclient->Find_Object(GameID);
	FAIL_IF(!co) return(true);
	NavigationDataClass *nav = Get_Navigation_Data(co, true);


	FAIL_IF(!nav) return(true);
	nav->Unknown_7BA940(this);
	return(true);
}





// 0x73A710
bool NavDeletePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("NavDeletePacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *co = sclient->Find_Object(GameID);
	FAIL_IF(!co) return(true);
	Remove_Navigation_Data(co);
	return(true);
}






// 0x73A7F0
bool WarpIndexPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("WarpIndexPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	MainViewClass *mainview = sclient->MainView;
	FAIL_IF(!mainview) return(true);
	RadarSystemClass *radar = mainview->Radar;

	FAIL_IF(!radar) return(true);
	radar->Unknown_7BD920(Index);
	return(true);
}




// 0x73A8E0
bool NotifyEmotePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("NotifyEmotePacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	ClientObjectClass *co = sclient->Find_Object(GameID);
	FAIL_IF(!co) return(true);
	EmoteDataClass *emote = Get_Emote_Data(co, true);


	FAIL_IF(!emote) return(true);
	emote->Unknown_7E6650(Emote);
	return(true);
}
// NOT EXACT: register-only (this in edi, the interface in esi: the client has them the other way round; attempts.py show 0x73A9F0)
// 0x73A9F0
bool StarbaseAvatarUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("StarbaseAvatarUpdatePacketClass::Process_Packet ()");
	SClient *sclient = (SClient *)instance;
	if (!sclient->Unknown135C) return(true);
	UIStarbaseClass *starbase = ((SBaseViewClass *)sclient->Unknown135C)->Get_Starbase().Peek();
	if (!starbase || starbase->Room != Room || !starbase->Unknown7C) return(true);
	starbase->Unknown_63E5C0(AvatarID, Orient, &Vector3(Unknown14, Unknown18, Unknown1C), ActionFlag);
	return(true);
}




// 0x73AB50
bool StarbaseAvatarRoomUpdatePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("StarbaseAvatarRoomUpdatePacketClass::Process_Packet ()");
	SClient *sclient = (SClient *)instance;
	if (!sclient->Unknown135C) return(true);
	UIStarbaseClass *starbase = ((SBaseViewClass *)sclient->Unknown135C)->Get_Starbase().Peek();
	if (!starbase || !starbase->Unknown7C) return(true);
	starbase->Unknown_63CF30(AvatarID, NewRoom);
	return(true);
}











// 0x73AC60
bool GalaxyMapPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("GalaxyMapPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	GalaxyMapClientClass *map = sclient->GalaxyMap;
	FAIL_IF(!map) return(true);
	LinearBufferClass buffer(Data, Size, 0x400, false);
	switch (Type) {
		case 2:
			map->Unknown_7A5260(buffer);
			break;
		case 4:
			map->Unknown_7A4C50(sclient, buffer);
			break;
		case 5:
			map->Unknown_7A4FC0(buffer);
			break;
		case 6:
		case 7:
		case 8:
		case 11:
			map->Unknown_7A50A0(buffer);
			break;
		case 9:
			map->Unknown_7A5180(buffer);
			break;
		case 3:
			map->Unknown_7A52F0(buffer);
			break;








		default:
			G_ASSERT(false);
			break;
	}
	return(true);
}

// 0x73AE40
bool NoteListPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("NoteListPacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)instance;
	for (std::list<NoteListEntryStruct>::iterator it = Notes.begin(); it != Notes.end(); it++) {
		char buffer[128];
		_snprintf(buffer, sizeof(buffer), "%3d : %s : %s", it->Unknown00, it->Unknown08, ctime((time_t *)&it->Unknown04));
		buffer[strlen(buffer) - 1] = 0;
		sclient->Unknown_74D990(6, buffer, 0);
	}
	sclient->Unknown_74D990(6, "Usage: /mail read X or /mail delete X", 0);
	return(true);
}

// What the chat panel's +0xD8 points to, as far as CLIENT_CHAT_EVENT 165 calls it
class UIChatPanelUnknownD8Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_5DA780(std::string name);				// 0x5DA780 (CLIENT_CHAT_EVENT 165, event 19 'is now ignoring')  // NAME 1 [name]: the event's Unknown54
	void Unknown_5DA8C0(std::string name);				// 0x5DA8C0 (CLIENT_CHAT_EVENT 165, event 20 'is no longer ignoring')  // NAME 1 [name]: the event's Unknown54
};

class UIFriendsClass	// NAME 3: class-from-file (D24) for app\client\ui_friends.cpp
{
public:
	void Unknown_5916B0(std::string name, std::string other);	// 0x5916B0 (CLIENT_CHAT_EVENT 165, event 21 'is now friends with')  // NAME 1 [name, other]: the event's Unknown54, Unknown64
	void Unknown_591880(std::string name);				// 0x591880 (CLIENT_CHAT_EVENT 165, event 22 'is no longer friends with')  // NAME 1 [name]: the event's Unknown54
};

// What the chat panel's +0xE4 points to, as far as CLIENT_CHAT_EVENT 165 calls it
class UIChatPanelUnknownE4Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_5E8FF0(std::string name, bool flag);	// 0x5E8FF0 (CLIENT_CHAT_EVENT 165, event 7, 1 last)  // NAME 1 [name, flag]: neutral
	void Unknown_5E9160(std::string name, bool flag);	// 0x5E9160 (CLIENT_CHAT_EVENT 165, events 8, 16, 18, 1 last)  // NAME 1 [name, flag]: neutral

	char Unknown00[0xA0];
	int UnknownA0;										// +0xA0 (events 16 and 18 test it for 1)
};

// What the chat panel's +0xE8 points to, as far as CLIENT_CHAT_EVENT 165 calls it
class UIChatPanelUnknownE8Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_5E5EF0(std::string name);				// 0x5E5EF0 (CLIENT_CHAT_EVENT 165, event 7)  // NAME 1 [name]: neutral
	void Unknown_5E6030(std::string name);				// 0x5E6030 (CLIENT_CHAT_EVENT 165, events 8, 16, 18)  // NAME 1 [name]: neutral
};

// What the chat panel's +0xE0 points to (as sclient.cpp's view)
class UIChatPanelUnknownE0Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_81BF40();								// 0x81BF40 (0x750210; CLIENT_CHAT_EVENT 165 on 'guild')

	char Unknown00[0x7C];
	bool Unknown7C;										// +0x7C (tested before every call)
};

// The chat panel (ui_chat_panel.cpp; constructor 0x561DD0, 0x268 bytes), as far as CLIENT_CHAT_EVENT 165 uses it
class UIChatPanelClass	// NAME 3: class-from-file (D24) for app\client\ui_chat_panel.cpp
{
public:
	void Unknown_563910(unsigned id, bool flag);		// 0x563910 (CLIENT_CHAT_EVENT 165: the sender's game id, 0)  // NAME 1 [id, flag]: what it gets
	void Unknown_565D80(const char *name);				// 0x565D80 (CLIENT_CHAT_EVENT 165: the private sender)  // NAME 1 [name]: the event's Unknown14
	void Unknown_5679A0(int event, const std::string &channel, const std::string &name, bool flag);	// 0x5679A0 (CLIENT_CHAT_EVENT 165, events 7, 8, 9, 23, 24)  // NAME 1 [event, channel, name, flag]: what the handler passes

	char Unknown00[0xD8];
	UIChatPanelUnknownD8Class *UnknownD8;				// +0xD8
	UIFriendsClass *Friends;							// +0xDC (ui_friends.cpp's 0x5916B0 / 0x591880)  // NAME 3: its methods' file
	UIChatPanelUnknownE0Class *UnknownE0;				// +0xE0
	UIChatPanelUnknownE4Class *UnknownE4;				// +0xE4
	UIChatPanelUnknownE8Class *UnknownE8;				// +0xE8
};

#include "ui_group.h"		// UIGroupClass (CLIENT_CHAT_EVENT 165)















// NOT EXACT: inline-budget (1,638 instructions vs the client's 1,615: the client calls std::string's range constructor 0x73F680 out of line and inlines other std::string members elsewhere; frame 0x568 vs 0x558; attempts.py show 0x73B150)
// 0x73B150
bool ClientChatEventPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ClientChatEventPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	if (Unknown0C == 3 && !strnicmp(Unknown44.c_str(), "group", 5)) {
		const char *message = Unknown54.c_str();
		int index;
		if (message[0] == '<' && message[1] == 'S' && message[2] == '>' && message[3] && (index = message[3] - '0') >= 0 && index <= 1) {
			const char *end = strchr(message + 4, '`');
			if (end) {
				std::string name(message + 4, end);
				if (sclient->MainView->Group) {
					sclient->MainView->Group->Unknown_597080(end + 1, name.c_str(), index);
				}
				return(true);
			}
		}
	}
	UIChatPanelClass *panel = sclient->ChatPanel.Peek();
	if (!panel) return(true);
	UIChatPanelUnknownD8Class *d8 = panel->UnknownD8;
	UIFriendsClass *friends = panel->Friends;
	UIChatPanelUnknownE4Class *e4 = panel->UnknownE4;
	UIChatPanelUnknownE8Class *e8 = panel->UnknownE8;
	std::string channel(Unknown44);
	unsigned pos = channel.find('`');
	if (pos != std::string::npos) channel.resize(pos);
	const char *from = Unknown24.c_str();
	const char *name = Unknown14.c_str();
	const char *target = Unknown34.c_str();
	const char *chan = channel.c_str();
	const char *text = Unknown54.c_str();
	char buffer[1024];
	buffer[0] = 0;
	switch (Unknown0C) {
		case 1:
			_snprintf(buffer, sizeof(buffer), "%s logged in.", from);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 2:
			_snprintf(buffer, sizeof(buffer), "%s logged out.", from);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 3:
			if (!strnicmp(chan, "Sector", 6)) {
				sclient->Unknown_74D990(2, (char *)text, 0);
				sclient->Unknown_750F70(name, 2);
			} else if (!strnicmp(chan, "Group", 5)) {
				_snprintf(buffer, sizeof(buffer), "[%s] %s: %s", chan, from, text);
				sclient->Unknown_74D990(4, buffer, 0);
				sclient->Unknown_750F70(name, 4);
			} else if (!strnicmp(chan, "Guild", 5)) {
				_snprintf(buffer, sizeof(buffer), "[%s] %s: %s", chan, from, text);
				sclient->Unknown_74D990(8, buffer, 0);
				sclient->Unknown_750F70(name, 8);
			} else {
				_snprintf(buffer, sizeof(buffer), "[%s] %s: %s", chan, from, text);
				if (!stricmp(chan, "General")) {
					sclient->Unknown_750F70(name, 9);
				} else if (!stricmp(chan, "Out of Context")) {
					sclient->Unknown_750F70(name, 10);
				} else if (!stricmp(chan, "Market")) {
					sclient->Unknown_750F70(name, 12);
				} else if (!stricmp(chan, "New Players")) {
					sclient->Unknown_750F70(name, 11);
				} else if (atoi(chan) > 0) {
					sclient->Unknown_750F70(name, 13);
				} else if (!stricmp(chan, "Terran") || !stricmp(chan, "Jenquai") || !stricmp(chan, "Progen")) {
					sclient->Unknown_750F70(name, 14);
				} else {
					sclient->Unknown_750F70(name, 15);
				}
				break;
			}
			return(true);
		case 4:
			if (name) {
				Unknown77E3F0Class *avatar;
				ClientObjectClass *object;
				unsigned id;
				if (Unknown78 && (object = sclient->Find_Object(name)) != NULL && (avatar = sclient->Unknown_74C840(id = object->GameID)) != NULL) {
					avatar->Unknown_784B80(Unknown74, Unknown78, true);
					if (sclient->AvatarID == id) return(true);
					if (sclient->ChatPanel) {
						sclient->ChatPanel->Unknown_563910(id, false);
					}
					if (sclient->AvatarID == id) return(true);
					if (!avatar->Unknown_7817E0()->Unknown_0C()) return(true);
					_snprintf(buffer, sizeof(buffer), "%s (private): %s", from, text);
				} else {
					panel->Unknown_565D80(Unknown14.c_str());
					_snprintf(buffer, sizeof(buffer), "%s (private): %s", from, text);
				}
				sclient->Unknown_74D990(3, buffer, 0);
				sclient->Unknown_750F70(name, 3);
				SoundLibraryClass::Play_2D_Sound("private_message_beep", 2);
				if (sclient->ChatPanel) {
					sclient->ChatPanel->Unknown_565D80(name);
				}
			}
			return(true);
		case 5:
			sclient->Unknown_74D990(7, (char *)text, 0);
			sclient->Unknown_750F70(name, 7);
			return(true);
		case 6:
			_snprintf(buffer, sizeof(buffer), "[system] %s: %s", from, text);
			break;
		case 7:
			if (strnicmp(chan, "Sector", 6)) {
				panel->Unknown_5679A0(7, std::string(chan), std::string(name), Unknown10 != 0);
				if (e8) {
					e8->Unknown_5E5EF0(Get_Unknown14());
				} else if (e4) {
					e4->Unknown_5E8FF0(Get_Unknown14(), true);
				}
				if (!stricmp(chan, "guild")) {
					if (sclient->Unknown_728300() && sclient->ChatPanel && sclient->ChatPanel->UnknownE0 && sclient->ChatPanel->UnknownE0->Unknown7C) {
						sclient->ChatPanel->UnknownE0->Unknown_81BF40();
					}
				}
			}
			return(true);
		case 8:
			if (strnicmp(chan, "Sector", 6)) {
				panel->Unknown_5679A0(8, std::string(chan), std::string(name), Unknown10 != 0);
				if (e8) {
					e8->Unknown_5E6030(Get_Unknown14());
				} else if (e4) {
					e4->Unknown_5E9160(Get_Unknown14(), true);
				}
				if (!stricmp(chan, "guild")) {
					if (sclient->Unknown_728300() && sclient->ChatPanel && sclient->ChatPanel->UnknownE0 && sclient->ChatPanel->UnknownE0->Unknown7C) {
						sclient->ChatPanel->UnknownE0->Unknown_81BF40();
					}
				}
			}
			return(true);
		case 9:
			panel->Unknown_5679A0(9, std::string(chan), std::string(""), Unknown10 != 0);
			_snprintf(buffer, sizeof(buffer), "[%s] New Channel Created.", chan);
			if (sclient->LineDisplay) {
				char line[128];
				_snprintf(line, sizeof(line), "You have joined channel %s", chan);
				sclient->LineDisplay->Add_Line_7(line, 3000, "MessageLine");
			}
			break;
		case 11:
			_snprintf(buffer, sizeof(buffer), "%s removed channel %s.", from, chan);
			break;
		case 12:
			_snprintf(buffer, sizeof(buffer), "[%s] %s invited %s.", chan, from, text);
			break;
		case 13:
			_snprintf(buffer, sizeof(buffer), "[%s] %s uninvited %s.", chan, from, text);
			break;
		case 14:
			_snprintf(buffer, sizeof(buffer), "[%s] %s gagged %s.", chan, from, text);
			break;
		case 15:
			_snprintf(buffer, sizeof(buffer), "[%s] %s ungagged %s.", chan, from, text);
			break;
		case 16:
			if (e4 && e4->UnknownA0 == 1) {
				e4->Unknown_5E9160(Get_Unknown54(), true);
			}
			if (e8) {
				e8->Unknown_5E6030(Get_Unknown54());
			}
			_snprintf(buffer, sizeof(buffer), "[%s] %s banned %s.", chan, from, text);
			break;
		case 17:
			_snprintf(buffer, sizeof(buffer), "[%s] %s unbanned %s.", chan, from, text);
			break;
		case 18:
			if (e4 && e4->UnknownA0 == 1) {
				e4->Unknown_5E9160(Get_Unknown54(), true);
			}
			if (e8) {
				e8->Unknown_5E6030(Get_Unknown54());
			}
			_snprintf(buffer, sizeof(buffer), "[%s] %s kicked %s.", chan, from, target);
			break;
		case 19:
			if (d8) {
				d8->Unknown_5DA780(Get_Unknown54());
			}
			_snprintf(buffer, sizeof(buffer), "%s is now ignoring %s.", from, text);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 20:
			if (d8) {
				d8->Unknown_5DA8C0(Get_Unknown54());
			}
			_snprintf(buffer, sizeof(buffer), "%s is no longer ignoring %s.", from, text);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 21:
			if (friends) {
				friends->Unknown_5916B0(Get_Unknown54(), Get_Unknown64());
			}
			_snprintf(buffer, sizeof(buffer), "%s is now friends with %s.", from, text);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 22:
			if (friends) {
				friends->Unknown_591880(Get_Unknown54());
			}
			_snprintf(buffer, sizeof(buffer), "%s is no longer friends with %s.", from, text);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 23:
			panel->Unknown_5679A0(23, std::string(chan), std::string(target), Unknown10 != 0);
			if (*from) {
				_snprintf(buffer, sizeof(buffer), "[%s] %s added %s to the channel owner list.", chan, from, target);
			} else {
				_snprintf(buffer, sizeof(buffer), "[%s] The system has added %s to the channel owner list.", chan, target);
			}
			break;
		case 24:
			panel->Unknown_5679A0(24, std::string(chan), std::string(target), Unknown10 != 0);
			if (*from) {
				_snprintf(buffer, sizeof(buffer), "[%s] %s removed %s from the channel owner list.", chan, from, target);
			} else {
				_snprintf(buffer, sizeof(buffer), "[%s] The system has removed %s from the channel owner list.", chan, target);
			}
			break;
		case 25:
			_snprintf(buffer, sizeof(buffer), "Only members of your Friends List may see if you are online.");
			sclient->Unknown_74D990(19, buffer, 0);
			return(true);
		case 26:
			_snprintf(buffer, sizeof(buffer), "All players may see you on their Friends List.");
			sclient->Unknown_74D990(19, buffer, 0);
			return(true);
		case 27:
			_snprintf(buffer, sizeof(buffer), "%s has been gagged by GM %s.", text, name);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		case 28:
			_snprintf(buffer, sizeof(buffer), "GM %s has removed the gag on %s.", name, text);
			sclient->Unknown_74D990(0, buffer, 0);
			return(true);
		default:
			return(true);
	}
	if (buffer[0]) {
		if (!stricmp(chan, "General")) {
			sclient->Unknown_74D990(9, buffer, 0);
		} else if (!stricmp(chan, "Out of Context")) {
			sclient->Unknown_74D990(10, buffer, 0);
		} else if (!stricmp(chan, "Market")) {
			sclient->Unknown_74D990(12, buffer, 0);
		} else if (!stricmp(chan, "New Players")) {
			sclient->Unknown_74D990(11, buffer, 0);
		} else if (atoi(chan) > 0) {
			sclient->Unknown_74D990(13, buffer, 0);
		} else if (!stricmp(chan, "Terran") || !stricmp(chan, "Jenquai") || !stricmp(chan, "Progen")) {
			sclient->Unknown_74D990(14, buffer, 0);
		} else {
			sclient->Unknown_74D990(15, buffer, 0);
		}
	}
	return(true);
}

// 0x73CC30 (out of line: defined after their caller)
const std::string &ClientChatEventPacketClass::Get_Unknown14()
{
	return(Unknown14);
}

// 0x73CC50
const std::string &ClientChatEventPacketClass::Get_Unknown54()
{
	return(Unknown54);
}

// 0x73CC70
const std::string &ClientChatEventPacketClass::Get_Unknown64()
{
	return(Unknown64);
}
















































































































































































































































































































































































































// 0x73DC60
bool ConfirmedActionOfferPacketClass::Process_Packet(void *instance)
{
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	sclient->Unknown_74E710(Message, ID, Type);
	return(true);
}

// 0x73DCB0
bool ConfirmedActionTimeoutPacketClass::Process_Packet(void *instance)
{
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	sclient->Unknown_74EB00(Type, ID);
	return(true);
}








































































































































// 0x73ED40
bool GuildPlayerPermissionsPacket::Process_Packet(void *instance)
{
	WWPROFILE("GuildPlayerPermissionsPacket::Process_Packet");
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	sclient->Unknown_7517E0(Unknown0C, Unknown10, Unknown14, Unknown18);
	return(true);
}



// 0x73EE20
bool GuildRankNamesSectorPacket::Process_Packet(void *instance)
{
	WWPROFILE("GuildRankNamesSectorPacket::Process_Packet");
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	sclient->Unknown_751850(Ranks);
	return(true);
}



























































// 0x73F3D0
bool ReferralPointsAvailablePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ReferralPointsAvailablePacketClass::Process_Packet");
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	sclient->Unknown_751460(PointsTotal, PointsSpent);
	return(true);
}

// 0x73F4B0
bool RecustomizeShipStartPacketClass::Process_Packet(void *instance)
{
	SClient *sclient = (SClient *)instance;

	FAIL_IF(!sclient) return(true);
	SBaseViewUnknown1A0Class *ship_ui = ((SBaseViewClass *)sclient->Unknown135C)->ShipUI;

	FAIL_IF(!ship_ui) return(true);
	ship_ui->Unknown_61F250(&Desc, &Cost, Unknown1A4, Unknown1A8, Unknown1AC, Unknown1B0);
	return(true);
}

// 0x73F570
bool RecustomizeAvatarStartPacketClass::Process_Packet(void *instance)
{
	SClient *sclient = (SClient *)instance;

	FAIL_IF(!sclient) return(true);
	SBaseViewUnknown19CClass *avatar_ui = ((SBaseViewClass *)sclient->Unknown135C)->AvatarUI;

	FAIL_IF(!avatar_ui) return(true);
	avatar_ui->Unknown_551C40(&Cost);
	return(true);
}

// 0x73F5F0
bool RecustomizeAvatarUpdatePacketClass::Process_Packet(void *instance)
{
	SClient *sclient = (SClient *)instance;
	FAIL_IF(!sclient) return(true);
	UIStarbaseClass *starbase = ((SBaseViewClass *)sclient->Unknown135C)->Get_Starbase().Peek();
	starbase->Unknown_6437A0(Unknown118, &Desc);
	return(true);
}

/*
**	Reconstruction notes
**
**	cpacket.cpp holds the client side of the packets: each class's PacketClass slot +0x08 override,
**	Process_Packet(void *), which SClient::Process_Packets 0x7427C0 calls with the SClient (a true
**	return deletes the packet). The client's handlers are 0x7333B0-0x73F680 (99 classes, 109
**	functions with the helpers); the classes are declared in app\packets (their own headers or
**	pk_classes.h), the bodies are here with this file's __FILE__. The profile labels (WWPROFILE) are the
**	client's strings. Rebuilt so far (D90 9d): the handlers that set a field on SClient or call one of its
**	methods, or reach another file's object through it (the views, the line display, the jobs, talk tree
**	and cockpit interfaces, co_data.cpp's per-object records); the rest stay unbuilt (udpreplay's
**	pk_ctor_stubs.cpp stands in for them). Other files' classes are declared here only as far as the
**	handlers call them. Where the client loads a packet field or a member chain into registers before
**	the pushes, the source reads it through an inline accessor (the line display's fields, the views'
**	+0x34, the main view's +0x13C).
**	Some handlers reach SClient through ClientClass slot +0x18 (SClient's returns this).
*/
