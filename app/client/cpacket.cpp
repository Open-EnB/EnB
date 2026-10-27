/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\client\cpacket.cpp (its asserts' __FILE__; client 0x7333B0-0x73F680)
**	The packets' handlers on the client side: PacketClass slot +0x08, Process_Packet(SClient *). Notes at the end.
*/
#include <windows.h>
#include "sclient.h"
#include "ui_line_display.h"
#include "co_data.h"
#include "..\packets\pk_player_object_relationship.h"
#include "..\packets\pk_sub_parts.h"
#include "..\packets\pkclientavatar.h"
#include "..\packets\pkclientship.h"
#include "..\packets\pkconfirmedaction.h"
#include "..\packets\pkcreateattachment.h"
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
#include "..\shared\spln_db.h"
#include "wwprofile.h"
#include "rendobj.h"
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
class EmoteDataClass	// NAME 2: co_data.h's name for what Get_Emote_Data returns
{
public:
	void Unknown_7E6650(int emote);	// 0x7E6650 (NOTIFY_EMOTE 162)  // NAME 1 [emote]: the packet's Emote
};

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

	char Unknown04[0x194 - 0x04];	// +0x04
	RefCountPtr<UIJobsClass> Jobs;	// +0x194  // NAME 2: what it holds
};

// co_player_relationship.cpp's handlers of the relationship packets (cdecl)
void Unknown_70CC40(SClient *sclient, PlayerObjectRelationshipPacket *packet);	// 0x70CC40 (RELATIONSHIP 137)  // NAME 1 [sclient, packet]: what the handler passes
void Unknown_70CC90(SClient *sclient, PlayerObjectReactionUpdatePacket *packet);	// 0x70CC90 (138)  // NAME 1 [sclient, packet]: what the handler passes
void Unknown_70CD50(SClient *sclient, PlayerObjectIsAttackingUpdatePacket *packet);	// 0x70CD50 (ATTACKER_UPDATES 139)  // NAME 1 [sclient, packet]: what the handler passes

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

// shiputils.cpp's decal helper (cdecl): the packet's decals on the object's model, with the decal INI
void Unknown_80E8C0(RenderObjClass *model, std::vector<DecalStruct> &decals, SClientUnknown1334Class *decal_ini);	// 0x80E8C0  // NAME 1 [model, decals, decal_ini]: what DECAL 16 passes

// 0x7333B0
bool LogoffConfirmationPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("LogoffConfirmationPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)instance;
	sclient->Unknown_743290();
	return(true);
}

// NOT EXACT: shape (the client loads the model straight into ecx; two forms; attempts.py show 0x733590)
// 0x733590
bool ClientAvatarPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ClientAvatarPacketClass::Process_Packet()");
	ClientClass *client = (ClientClass *)instance;
	client->Set_Debug_Object(AvatarID);
	client->Get_Debug_Object()->Model->Set_LOD_Bias(1000000000.0f);
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
		object->Unknown_70E750(TargetID);
	}
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

















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// NOT EXACT: order-only (the __int64 Unknown10C0 store scheduled first; two forms; attempts.py show 0x737CB0)
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

// 0x739330
bool ManufactureSetPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("ManufactureSetPacketClass::Process_Packet(void * instance)");
	((SClient *)((ClientClass *)instance)->Unknown_18())->Unknown1328 = ManufactureID;
	return(true);
}

// 0x7396C0
bool HullUpgradePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("HullUpgradePacketClass::Process_Packet(void * instance)");
	return(true);
}

// NOT EXACT: shape (the client places the shared failure exit before the call; three forms; attempts.py show 0x7396F0)
// 0x7396F0
bool LootHulkPermissionPacketClass::Process_Packet(void *instance)
{
	WWPROFILE("LootHulkPermissionPacketClass::Process_Packet()");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	if (sclient->MainView) {
		UICpitClass *cpit = sclient->MainView->Cpit;
		if (cpit) {
			ClientObjectClass *hulk = sclient->Find_Object(HulkGameID);
			if (hulk) {
				cpit->Unknown_581F90(hulk);
				return(true);
			}
		}
	}
	return(true);
}



























































































































































































































































































































// NOT EXACT: shape (the client places the shared failure exit before the UIType test; two forms; attempts.py show 0x7397D0)
// 0x7397D0
bool OpenInterfacePacketClass::Process_Packet(void *instance)
{
	WWPROFILE("OpenInterfacePacketClass::Process_Packet(void * instance)");
	SClient *sclient = (SClient *)((ClientClass *)instance)->Unknown_18();
	UICpitClass *cpit;
	if (sclient->MainView && (cpit = sclient->MainView->Cpit) != NULL) {
		if (UIType) {
			G_ASSERT(false);
		} else cpit->Unknown_5820C0(UIChange == 0);
		return(true);
	}
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

/*
**	Reconstruction notes
**
**	cpacket.cpp holds the client side of the packets: each class's PacketClass slot +0x08 override,
**	Process_Packet(void *), which SClient::Process_Packets 0x7427C0 calls with the SClient (a true
**	return deletes the packet). The client's handlers are 0x7333B0-0x73F680 (99 classes, 109
**	functions with the helpers); the classes are declared in app\packets (their own headers or
**	pk_classes.h), the bodies are here with this file's __FILE__. The profile labels (WWPROFILE) are the
**	client's strings. Rebuilt so far: the handlers that set a field on SClient or call one of its
**	methods (D90 9d); the rest stay unbuilt (udpreplay's pk_ctor_stubs.cpp stands in for them).
**	Some handlers reach SClient through ClientClass slot +0x18 (SClient's returns this).
*/
