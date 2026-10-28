// clienttest (D94(c), P-T7-1, P-T8-1): the client base (app\client\client.cpp), SClient (sclient.cpp) and
// the packets' client-side handlers (cpacket.cpp) over a memory connection, with logging stand-ins for
// what they call outside the rebuilt files (client_stubs.cpp). Writes one line per event to OUT;
// tools/harness/clienttest.py compares the trace with tests/capharness/clienttest/expected.txt.
//
//     clienttest OUT.txt [MESSAGES.txt]
//
// (1) ClientClass's object, effect and record lists keyed by game id (wwlib's 257-bucket hash list):
//     ids 257 apart share a bucket; Add / Find / Remove through SClient's overrides, which tell a fake
//     main view; the name lookup; the ship.
// (2) The DEBUG packet's setters (Set_Debug_Mode, Set_Debug_Object, Toggle_Debug_Mode) before and
//     after the redirect (Open_Connection 0x727F00: connect.cpp's factory, MEMORY protocol, the memory
//     connection below); each DEBUG message sent is parsed back by the factory and its fields checked.
// (3) SClient::Process_Packets over the redirected connection loaded with MESSAGES (a capture's
//     messages, one per line: opcode and payload hex; clienttest.py writes them from the reference decoder's
//     alex3.trace.jsonl): opcodes whose handler cpacket.cpp defines go through the factory and the real
//     handler, the rest become one recording test packet (counted per opcode); CREATE 4's real handler
//     makes the objects (co.cpp's constructor, the base asset's model from the fake asset manager below,
//     Set_Model, Add_Object), so REMOVE and the object handlers find them. Before that, (3a): a packet whose
//     handler returns false stays on the peek connection and comes back on the next call, and (4): a
//     server's packets build and move an object (P-UP1-1): CREATE 4, SIMPLE_POSITIONAL_UPDATE 8 through the
//     real handler and postate / extrap's state, then SClient::Process_AI and co.cpp's per-frame update,
//     which hand the state's matrix at the client clock to the model's slot +0x50 (Set_Transform);
//     CHANGE_BASE_ASSET 38 swaps the model, REMOVE 7 deletes the object. (4b): synthetic CREATEs of types 24
//     and 28 make co_subtypes.cpp's subclasses (no alex3 message has them); SET_TARGET 25 aims type 28.
//     (3) also checks START 5 (+0x1138, REQUEST_TIME and START_ACK sent) and CLIENT_SET_TIME 52 (the clock
//     offset +0x10C0, the lag average cleared, the round trip time 0xBE71D4); (4c): synthetic GADGET 103
//     (the gadget list +0x1278, the gadget's and its interface's slots) and INCAPACITANCE_DETAILS 144; (4d):
//     synthetic FORMATION_POSITIONAL_UPDATE 65 (the follower's and the avatar's formation states, then an update);
//     (4e): synthetic VERB_LIST 91 to the verb manager (+0x1294); (4f): synthetic STARBASE_AVATAR_UPDATE 158 and
//     STARBASE_AVATAR_ROOM_UPDATE 160 to the starbase view's (+0x135C) interface; (4g): synthetic RECUSTOMIZE_SHIP_START
//     129, RECUSTOMIZE_AVATAR_START 131 and RECUSTOMIZE_AVATAR_UPDATE 133 to the starbase view's +0x1A0 / +0x19C / +0x60.
//
// Never: a modal dialog (asserts and FAIL_IF go to the trace), the registry (SOptionsClass's
// constructor is a stand-in), the network (the memory connection), the game's install folder.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include "clienttest.h"
#include "..\..\..\app\client\sclient.h"
#include "..\..\..\app\common\connect.h"
#include "..\..\..\app\packets\pk.h"
#include "..\..\..\app\packets\pkdebug.h"
#include "..\..\..\app\packets\pkcreate.h"
#include "..\..\..\app\packets\pkupdate.h"
#include "..\..\..\app\packets\pkgadget.h"
#include "..\..\..\app\packets\pkincapacitancerequest.h"
#include "..\..\..\app\packets\pkverblist.h"
#include "..\..\..\app\packets\pkstarbase_avatar_change.h"
#include "..\..\..\app\packets\pkrecustomize.h"
#include "..\..\..\app\client\aux_roots.h"
#include "..\..\..\app\client\co_subtypes.h"
#include "..\..\..\app\client\postate.h"
#include "..\..\..\app\client\extrap.h"
#include "..\..\..\app\shared\px_wrapper.h"
#include "rendobj.h"
#include "..\..\..\app\shared\stars.h"

static FILE *Out;
static int Lines = 0;
static int Failures = 0;
bool Stub_744B60 = false;
bool Stub_Focused = false;

void Trace(const char *format, ...)
{
	va_list va;
	va_start(va, format);
	vfprintf(Out, format, va);
	va_end(va);
	fputc('\n', Out);
	fflush(Out);		// the trace survives a crash
	Lines++;
}

static void Check(bool ok, const char *what)
{
	Trace("check %s: %s", what, ok ? "ok" : "FAIL");
	if (!ok) {
		Failures++;
	}
}

// ------------------------------------------------------------------------- what the app files call, functional

extern unsigned int Unknown_BE71D4;	// postate.cpp (CLIENT_SET_TIME 52 stores the round trip time)
static unsigned int Test_Ticks = 0;
unsigned int Unknown_Get_Ticks() { return Test_Ticks; }		// the aux-data clock (dbgconn.cpp's rate wrapper; SClient::Do_Frame's clock): 0 but in (4)

// wwonline's 64-bit byte order pair (pk.cpp's Add / Get of an __int64), as udpreplay's conn_replay.cpp
static unsigned __int64 Swap64(unsigned __int64 v)
{
	unsigned __int64 r = 0;
	for (int i = 0; i < 8; i++) {
		r = (r << 8) | ((v >> (8 * i)) & 0xFF);
	}
	return r;
}
unsigned __int64 Unknown_8B4510(unsigned __int64 value) { return Swap64(value); }
unsigned __int64 Unknown_8B4560(unsigned __int64 value) { return Swap64(value); }

// The packets whose raw constructor isn't rebuilt (pk_stubs.cpp, generated): the factory still makes
// them; the harness never hands one to a handler (Opcode_Table's Handler 0 / 2 go to the recording packet)
void Conn_Record(PacketClass *packet, RawPacketClass *)
{
	packet->Connection = 0;
	packet->Serial = 0;
}

// ------------------------------------------------------------------------- the fakes

// The main view (mainview.cpp, MainViewClass: 0x1EC bytes, slots +0x80 / +0x84 SClient's Add_Object /
// Remove_Object call) and the loading view (+0x0C, the flag at +0x68): the views' constructors are not
// rebuilt, so the harness gives SClient objects with the same slot layout (32 slots before +0x80).
// In (4), +0x64 hands SClient 0x7456B0 an object whose slot +0x24 takes an int, and +0x7C Process_AI a
// camera (a zeroed render object with the zero table: Get_Position calls its virtual Validate_Transform
// and reads its Transform).
struct FakeTableStruct
{
	void **Table;
};
static bool Path_Views = false;
static void *View64_Table[16];
struct FakeRefTableStruct
{
	void **Table;
	int NumRefs;		// 0x512A00 peeks a RefCountPtr of the +0x64 object
};
static FakeRefTableStruct Fake_View64 = { View64_Table, 1 };
static double Fake_Camera_Storage[0x400 / sizeof(double)];
static char *Fake_Camera = (char *)Fake_Camera_Storage;
static void __stdcall View64_24_Slot(int arg) { Trace("mainview 64's +0x24 %d", arg); }	// slot +0x64's scene, +0x24 SceneClass::Set_Fog_Enable: one argument, popped
static void *View1DC_Table[64];
static FakeTableStruct Fake_View1DC = { View1DC_Table };
static void __stdcall View1DC_88_Slot(RenderObjClass *model) { Trace("mainview 1DC's +0x88 %s", model ? "a model" : "NULL"); }	// 0x513540: the backdrop model
static void __stdcall View1DC_8C_Slot(int arg) { Trace("mainview 1DC's +0x8C %d", arg); }	// 0x513600
static void *View138_Table[32];
static void *Cpit_Table[64];
static void View138_58_Slot() { Trace("mainview 138's +0x58 (on)"); }	// 0x514010
static void View138_5C_Slot() { Trace("mainview 138's +0x5C (off)"); }
static void Cpit_58_Slot() { Trace("cockpit +0x58 (on)"); }	// 0x514010 (UICpitClass)
static void Cpit_5C_Slot() { Trace("cockpit +0x5C (off)"); }
static void *Scene10C_Table[32];
static void __stdcall Scene10C_Add_Slot(RenderObjClass *obj) { Trace("mainview scene Add_Render_Object"); }	// slot 3
static void Scene10C_Remove_All_Slot() { Trace("mainview scene Remove_All_Render_Objects"); }	// slot 24 (0x514010)
class FakeMainViewClass
{
public:
	FakeMainViewClass() { memset(Unknown04, 0, sizeof(Unknown04)); }	// the radar (+0x80) and the rest null: WARP_INDEX's FAIL_IF
	virtual ~FakeMainViewClass() {}
	virtual void S04() {} virtual void S08() {} virtual void S0C() {} virtual void S10() {}
	virtual void S14() {} virtual void S18() {} virtual void S1C() {} virtual void S20() {}
	virtual void S24() {} virtual void S28() {} virtual void S2C() {} virtual void S30() {}
	virtual void S34() {} virtual void S38() {} virtual void S3C() {} virtual void S40() {}
	virtual void S44() {} virtual void S48() {} virtual void S4C() {} virtual void S50() {}
	virtual void S54() {} virtual void S58() {} virtual void S5C() {} virtual void S60() {}
	virtual int S64() { Trace("mainview 64"); return(Path_Views ? (int)&Fake_View64 : 0x64); } virtual void S68() {} virtual void S6C() {} virtual void S70() {}
	virtual void S74() {} virtual void S78() {} virtual int S7C() { Trace("mainview 7C"); return(Path_Views ? (int)Fake_Camera : 0x7C); }	// +0x64 / +0x7C: OBJECT_EFFECT 9's Add_Effect arguments; in (4) Process_AI's objects (Path_Views)
	virtual void Add_Object(ClientObjectClass *object) { Trace("mainview add %u", object->Get_Game_ID()); }			// +0x80
	virtual void Remove_Object(ClientObjectClass *object) { Trace("mainview remove %u", object->Get_Game_ID()); }	// +0x84
	char Unknown04[0x1EC - 0x04];
};

class FakeLoadingViewClass
{
public:
	virtual ~FakeLoadingViewClass() {}
	virtual void S04() {} virtual void S08() {}
	virtual void Unknown_0C(bool on) { Trace("loadingview 0C %d", on); }	// +0x0C
	virtual void S10() {}
	char Unknown04[0x68 - 0x04];
	bool Unknown68;		// +0x68
};

// The models of (1)'s objects (co.cpp +0xAC; New_Object): a table whose slots that take an argument get __stdcall
// stand-ins (Set_Transform +0x50, Set_LOD_Bias +0x12C); every other slot returns the same value: an odd game id gets
// an rs_track model (0x10001), an even one a model that isn't (0). The objects CREATE makes get FakeModelClass's
// models from the fake asset manager (below).
static int Zero_Slot() { return 0; }
static int Track_Slot() { return 0x10001; }
static void __stdcall Lod_Bias_Slot(float bias) { Trace("model Set_LOD_Bias %g", bias); }	// slot +0x12C (75), CLIENT_AVATAR 55: one argument, popped
struct FakeModelStruct
{
	void **Table;
	char Unknown04[0x100];
};
// slot +0x50 (20), co.cpp's per-frame update 0x6F3C60: Set_Transform (one argument, popped)
static void __stdcall Set_Transform_Slot(const Matrix3D &tm) { Trace("model Set_Transform (%g, %g, %g)", tm[0][3], tm[1][3], tm[2][3]); }
static void *Zero_Table[256];
static void *Track_Table[256];
static FakeModelStruct Fake_Model;
static FakeModelStruct Track_Model;

static char Fake_Aux[0x4000];		// the aux data co.cpp's accessors read (zeros but the table, below): every object's root
static char Fake_Player_Root[0x10];	// SClient +0x12C0 (Init's DeltaPlayerClass: AUX_DATA 27 with game id 0)
static char Fake_Player_Var_Root[0x10];	// SClient +0x12C4's entry 0 (Init's player-variable root: PLAYER_VAR_AUX_DATA 28)
// the fake aux data's table: slot +0x20 (8, no argument) is the age step, +0x04 (1) the deleting destructor
// (~ClientObjectClass deletes its root: one argument, popped; the root is shared, so nothing is freed)
static void *Aux_Table[256];
static void Aux_Age_Slot() { Trace("aux data +0x20 (the age step)"); }
static void __stdcall Aux_Delete_Slot(unsigned) { Trace("aux data deleted (slot +0x04)"); }
const char *Aux_Root_Name(const void *root)
{
	return(root == Fake_Player_Root ? "player" : root == Fake_Player_Var_Root ? "player var 0" : root == Fake_Aux ? "object" : "unknown");
}
// co.cpp's constructor asks aux_roots.cpp's factory 0x6F6000 for the object's root (aux_roots.obj and the roots'
// code stay out): the types the factory gives a root (packet-flow.md 2.4) get the harness's, the others none
UnknownRootClass *UnknownCOClass::Unknown_Create_Aux_Root(int type, int)
{
	switch (type) {
		case 0: case 1: case 3: case 4: case 8: case 11: case 12: case 25: case 34: case 37: case 38: case 39: case 41:
			return((UnknownRootClass *)Fake_Aux);
	}
	return(0);
}

// ------------------------------------------------------------------------- the fake asset manager and its models

// The models CREATE 4, CHANGE_BASE_ASSET 38 and SUBPARTS 180 load: co.cpp's loader 0x6F5140 / 0x6F51D0 reads
// basset.ini's BASE-n section (clienttest's own, tests/capharness/clienttest/data: clienttest.py copies it into
// the run's folder, never the game's, D69) and asks E&B's asset manager (*0xBDADC0) for the NAME at the scale and
// HSV shift (its slot +0x78), loading FILE (Load_3D_Assets) when it has no such model yet; a section that
// isn't there gives Create_Render_Obj("missing"). WW3DAssetManager::Get_Instance() (0xBFAEE0) is the same object.
// Each model is a real RenderObjClass (ww3d2's slots; the ones co.cpp and the handlers call log): numbered in
// the order made, deleted when its last reference goes. Ship models (NAME ship...) have the bones ~FL ~FR ~BL ~BR
// that 0x6F3810 hangs the six part models on; a NAME rs_... is an rs_track model (Class_ID 0x10001: the render-
// state handlers call RsTrackClass; the client's rs_track models come from an RSLID section, whose RsTrackClass
// constructor 0x45FFB0 isn't rebuilt).
#include "assetmgr.h"
class UnknownBDADC0Class : public WW3DAssetManager	// as co.cpp declares it
{
public:
	virtual RenderObjClass *Unknown_78(const char *name, float scale, float *hsv);	// +0x78
};
RenderObjClass *UnknownBDADC0Class::Unknown_78(const char *, float, float *) { return(0); }	// (the fake's overrides it)
UnknownBDADC0Class *Unknown_BDADC0 = 0;	// 0xBDADC0 (main sets it)

static int Models_Made = 0;
static int Models_Alive = 0;
static std::vector<int> Deleted_Models;	// the numbers of the models deleted
class FakeModelClass : public RenderObjClass
{
public:
	FakeModelClass(const char *name) : Number(++Models_Made), Transforms(0), Bones(0), LastTransform(true)
	{
		strncpy(Name, name, sizeof(Name) - 1);
		Name[sizeof(Name) - 1] = 0;
		Models_Alive++;
	}
	virtual ~FakeModelClass() { Trace("model %d '%s' deleted", Number, Name); Models_Alive--; Deleted_Models.push_back(Number); }
	virtual RenderObjClass *Clone() const { return(0); }
	virtual int Class_ID() const { return(strncmp(Name, "rs_", 3) == 0 ? 0x10001 : CLASSID_UNKNOWN); }
	virtual const char *Get_Name() const { return(Name); }
	virtual void Render(RenderInfoClass &) {}
	virtual void Set_Transform(const Matrix3D &tm)	// +0x50
	{
		RenderObjClass::Set_Transform(tm);
		LastTransform = tm;
		Transforms++;
		Trace("model %d Set_Transform (%g, %g, %g)", Number, tm[0][3], tm[1][3], tm[2][3]);
	}
	virtual void Set_LOD_Bias(float bias) { Trace("model %d Set_LOD_Bias %g", Number, bias); }	// +0x12C
	virtual void Set_Hidden(int onoff)	// (traced when it changes: the per-frame update hides the part models again each frame)
	{
		if ((Is_Hidden() != 0) != (onoff != 0)) {
			Trace("model %d Set_Hidden %d", Number, onoff);
		}
		RenderObjClass::Set_Hidden(onoff);
	}
	virtual int Get_Bone_Index(const char *bone)
	{
		static const char *bones[] = { "~FL", "~FR", "~BL", "~BR" };
		for (int i = 0; i < 4 && strncmp(Name, "ship", 4) == 0; i++) {
			if (strcmp(bone, bones[i]) == 0) {
				return(i + 1);
			}
		}
		return(0);
	}
	virtual int Add_Sub_Object_To_Bone(RenderObjClass *sub, int bone)
	{
		Bones++;
		Trace("model %d Add_Sub_Object_To_Bone '%s' bone %d", Number, sub ? sub->Get_Name() : "(null)", bone);
		return(0);
	}
	int Number;
	int Transforms;		// Set_Transform calls
	int Bones;			// Add_Sub_Object_To_Bone calls
	Matrix3D LastTransform;
	char Name[64];
};
static FakeModelClass *Fake_Model_Of(ClientObjectClass *object)	// (only CREATE's objects hold one)
{
	return(object ? (FakeModelClass *)object->Model : 0);
}

class FakeAssetManagerClass : public UnknownBDADC0Class
{
public:
	FakeAssetManagerClass() : Loads(0) {}
	virtual bool Load_3D_Assets(const char *filename)
	{
		Trace("asset manager Load_3D_Assets '%s'", filename);
		Files.push_back(filename);
		Loads++;
		return(true);
	}
	virtual RenderObjClass *Create_Render_Obj(const char *name)
	{
		FakeModelClass *model = new FakeModelClass(name);
		Trace("asset manager Create_Render_Obj '%s' -> model %d", name, model->Number);
		return(model);
	}
	// +0x78: a model of a loaded file's (FILE is NAME.w3d in clienttest's basset.ini)
	virtual RenderObjClass *Unknown_78(const char *name, float scale, float *hsv)
	{
		std::string file = std::string(name) + ".w3d";
		for (size_t i = 0; i < Files.size(); i++) {
			if (stricmp(Files[i].c_str(), file.c_str()) == 0) {
				FakeModelClass *model = new FakeModelClass(name);
				Trace("asset manager +0x78 '%s' scale %g hsv (%g %g %g) -> model %d", name, scale, hsv[0], hsv[1], hsv[2], model->Number);
				return(model);
			}
		}
		Trace("asset manager +0x78 '%s': not loaded", name);
		return(0);
	}
	std::vector<std::string> Files;
	int Loads;
};
static FakeAssetManagerClass *Fake_Assets = 0;

// co.cpp's object through its own constructor and destructor (app.lib); the harness's fakes (aux data, model)
// go back before the destructor would release them
class TestObjectClass : public ClientObjectClass
{
public:
	TestObjectClass(ClientClass *client, unsigned id) : ClientObjectClass(client, 0, id) {}	// type 0: a player ship for co.cpp's type tests (0x6F5A10); the client: the per-frame update reads its clock
	virtual ~TestObjectClass() { Trace("stub ClientObjectClass::~ClientObjectClass %u", GameID); AuxData = 0; Model = 0; }
};

static SClient *The_Client = 0;

// (1)'s objects (the packet sections' come from CREATE 4's real handler)
static ClientObjectClass *New_Object(unsigned id)
{
	ClientObjectClass *object = new TestObjectClass(The_Client, id);
	object->AuxData = Fake_Aux;		// (the constructor's own aux root is the same)
	// the constructor's default state (type 0, identity) stays: Process_AI's per-frame update (4) asks every
	// object's state for its matrix
	object->Model = (RenderObjClass *)((id & 1) ? &Track_Model : &Fake_Model);
	return object;
}

static EffectObjectClass *New_Effect(unsigned id)
{
	EffectObjectClass *effect = new EffectObjectClass;
	effect->ID = id;
	effect->Node.Set(effect);		// eff_o.cpp's, likewise
	return effect;
}

// ------------------------------------------------------------------------- the memory connection

static const char *Opcode_Name(int opcode, int *handler)
{
	for (int i = 0; i < Opcode_Count; i++) {
		if (Opcode_Table[i].Opcode == opcode) {
			if (handler) *handler = Opcode_Table[i].Handler;
			return Opcode_Table[i].Name;
		}
	}
	if (handler) *handler = 0;
	return "?";
}

// What the harness hands Process_Packets for an opcode it doesn't drive: counts and accepts it
static std::map<int, int> Recorded;
class RecordingPacketClass : public PacketClass
{
public:
	RecordingPacketClass(int opcode, int size) : Opcode(opcode), Size(size), Refuse(opcode == REFUSE_OPCODE ? 2 : 0) { Serial = 0; }
	virtual bool Process_Packet(void *)
	{
		if (Refuse > 0) {
			// the harness's own message (Test_Refuse): false leaves the packet on the connection
			Trace("recording packet %d: refused (%d more)", Serial, --Refuse);
			return(false);
		}
		if (Opcode == REFUSE_OPCODE) {
			Trace("recording packet %d: taken", Serial);
			return(true);
		}
		Recorded[Opcode]++;
		return(true);
	}
	virtual int Get_Data_Size() { return(Size); }
	enum { REFUSE_OPCODE = 0xFFFE };
	int Opcode;
	int Size;
	int Refuse;
};

static std::vector<unsigned char> Message_Bytes(int opcode, const std::vector<unsigned char> &payload)
{
	std::vector<unsigned char> m(4 + payload.size());
	*(short *)&m[0] = (short)m.size();
	*(unsigned short *)&m[2] = (unsigned short)opcode;
	if (!payload.empty()) {
		memcpy(&m[4], &payload[0], payload.size());
	}
	return m;
}

// The redirect's connection (memconn.cpp's factory below hands it out): what is sent is kept as
// messages; what is received comes from Incoming, one message per Receive.
class MemoryConnectionClass : public ConnectionClass
{
public:
	MemoryConnectionClass() : Received(0) {}
	virtual ~MemoryConnectionClass() { Trace("memory connection deleted"); }
	virtual bool Send_Message(RawPacketClass *raw)
	{
		std::vector<unsigned char> m(raw->Header.Size < 4 ? 4 : raw->Header.Size);
		memcpy(&m[0], &raw->Header, 4);
		if (m.size() > 4) {
			memcpy(&m[4], raw->Data.Buffer, m.size() - 4);
		}
		std::string hex;
		char b[4];
		for (size_t i = 4; i < m.size(); i++) {
			sprintf(b, "%02x", m[i]);
			hex += b;
		}
		Trace("sent %s (%d) size %d: %s", Opcode_Name(*(unsigned short *)&m[2], 0), *(unsigned short *)&m[2], (int)*(short *)&m[0], hex.c_str());
		Sent.push_back(m);
		return(true);
	}
	virtual bool Send_Packet(PacketClass *packet)
	{
		RawPacketClass *raw = packet->Build();
		if (raw == 0) {
			Trace("sent: Build gave nothing");
			return(false);
		}
		bool ok = Send_Message(raw);
		delete raw;
		return(ok);
	}
	virtual PacketClass *Receive()
	{
		if (Incoming.empty()) {
			return(0);
		}
		std::vector<unsigned char> m = Incoming.front();
		Incoming.pop_front();
		Received++;
		int opcode = *(unsigned short *)&m[2];
		int handler;
		const char *name = Opcode_Name(opcode, &handler);
		PacketClass *packet;
		if (handler == 1) {
			Trace("msg %d %s (%d) size %d: cpacket.cpp's handler", Received, name, opcode, (int)m.size());
			packet = Unknown_Create_Packet(&m[0]);
			if (packet == 0) {
				Trace("msg %d: the factory made nothing", Received);
				Failures++;
				packet = new RecordingPacketClass(opcode, (int)m.size() - 4);
			}
		} else {
			packet = new RecordingPacketClass(opcode, (int)m.size() - 4);
		}
		packet->Connection = this;
		return(packet);
	}
	std::vector<std::vector<unsigned char> > Sent;
	std::deque<std::vector<unsigned char> > Incoming;
	int Received;
	SClient *Client;
};

static MemoryConnectionClass *Memory = 0;

// memconn.cpp's factory (not rebuilt): one server listening, on the redirect's port
class MemoryConnectionFactoryClass
{
public:
	bool Is_Listening(unsigned short port);
	ConnectionClass *Connect(unsigned short port);
};
bool MemoryConnectionFactoryClass::Is_Listening(unsigned short port)
{
	Trace("memconn is_listening %u", port);
	return(port == 7000);
}
ConnectionClass *MemoryConnectionFactoryClass::Connect(unsigned short port)
{
	Trace("memconn connect %u", port);
	Memory = new MemoryConnectionClass;
	Memory->Client = The_Client;
	return(Memory);
}

// ------------------------------------------------------------------------- (1) the lists

static void Test_Lists(SClient *client)
{
	Trace("== (1) the lists");
	const unsigned ids[] = { 5, 5 + 257, 5 + 514, 6, 1000 };
	ClientObjectClass *objects[5];
	for (int i = 0; i < 5; i++) {
		objects[i] = New_Object(ids[i]);
		client->Add_Object(objects[i]);
	}
	Check(client->Get_Objects_Changed(), "objects changed after Add_Object");
	Check(!client->Get_Objects_Changed(), "Get_Objects_Changed clears the flag");
	bool all = true;
	for (int j = 0; j < 5; j++) {
		all = all && client->Find_Object(ids[j]) == objects[j];
	}
	Check(all, "Find_Object finds each of 5, 262, 519, 6, 1000 (5 / 262 / 519 share a bucket)");
	Check(client->Find_Object(5 + 3 * 257) == 0, "Find_Object 776 (same bucket, not added): NULL");
	Check(client->Find_Object("Unknown") != 0, "Find_Object by name 'Unknown' (co.cpp's Get_Name over the stand-in aux data): found");
	Check(client->Find_Object("obj7") == 0, "Find_Object by name 'obj7': NULL");

	Check(client->Remove_Object(262) == objects[1], "Remove_Object 262 (the bucket's middle)");
	Check(client->Get_Objects_Changed(), "objects changed after Remove_Object");
	Check(client->Find_Object(262) == 0 && client->Find_Object(5) == objects[0] && client->Find_Object(519) == objects[2],
		"262 gone, 5 and 519 still found");
	Check(client->Remove_Object(262) == 0, "Remove_Object 262 again: NULL, no notification");
	Check(!client->Get_Objects_Changed(), "a failed Remove_Object leaves the flag");
	Check(client->Remove_Object(519) == objects[2] && client->Remove_Object(5) == objects[0], "Remove_Object 519, then 5");
	Check(client->Find_Object(6) == objects[3] && client->Find_Object(1000) == objects[4], "6 and 1000 untouched");
	client->Add_Object(objects[1]);
	Check(client->Find_Object(262) == objects[1], "262 added back, found");

	Check(client->Set_Ship_ID(1000) == 0 && client->Get_Ship() == objects[4], "Set_Ship_ID 1000: Get_Ship is object 1000");
	Check(client->Set_Ship_ID(4) == 1000 && client->Get_Ship() == 0, "Set_Ship_ID 4 returns 1000; no object 4");
	client->Set_Ship_ID(1000);
	Check(client->Unknown_728370() == 0, "Unknown_728370 asks the ship's 0x6FFF50");

	EffectObjectClass *e1 = New_Effect(40);
	EffectObjectClass *e2 = New_Effect(40);
	EffectObjectClass *e3 = New_Effect(40 + 257);
	client->Add_Effect(e1, 1, 2);
	client->Add_Effect(e2, 3, 4);
	client->Add_Effect(e3, 5, 6);
	Check(client->Get_Effect(40 + 257) == e3, "Get_Effect 297");
	Check(client->Find_Effect(40) == e1 || client->Find_Effect(40) == e2, "Find_Effect 40 finds one of the two");
	EffectObjectClass *last = client->Remove_Effects(40);
	Check((last == e1 || last == e2) && client->Get_Effect(40) == 0, "Remove_Effects 40 takes both, returns the last");
	Check(client->Get_Effect(40 + 257) == e3, "297 left");
	Check(client->Remove_Effects(41) == 0, "Remove_Effects 41: NULL");
	delete e1;
	delete e2;
}

// ------------------------------------------------------------------------- (2) DEBUG and the redirect

static void Check_Debug(const char *what, int id, int idx, int user)
{
	if (Memory == 0 || Memory->Sent.empty()) {
		Check(false, what);
		return;
	}
	std::vector<unsigned char> m = Memory->Sent.back();
	DebugPacketClass *packet = (DebugPacketClass *)Unknown_Create_Packet(&m[0]);
	bool ok = packet != 0 && *(unsigned short *)&m[2] == 26 && packet->GameID == id && packet->DebugIdx == idx && packet->UserData == user;
	if (packet) {
		char text[256];
		memset(text, 0, sizeof(text));
		packet->Print(text, sizeof(text) - 1);
		Trace("  parsed back: %s", text);
	}
	delete packet;
	Check(ok, what);
}

static void Test_Debug(SClient *client)
{
	Trace("== (2) DEBUG and the redirect");
	client->Set_Debug_Mode(true);
	Check(client->DebugMode, "Set_Debug_Mode true before the redirect (the do-nothing connection)");
	client->Open_Connection(0x7F000001, 7000);
	Check(Memory != 0, "the redirect opened the memory connection");
	if (Memory == 0) {
		return;
	}
	size_t before = Memory->Sent.size();
	Check(client->Set_Debug_Object(1000) == 0, "Set_Debug_Object 1000 returns the old id 0");
	Check_Debug("Set_Debug_Object sends DEBUG {1000, 0x21, 0}", 1000, 0x21, 0);
	Check(client->Get_Avatar() == client->Find_Object(1000), "Get_Avatar is object 1000");
	client->Set_Debug_Mode(false);
	Check_Debug("Set_Debug_Mode false sends DEBUG {1000, 0x1E, 0}", 1000, 0x1E, 0);
	size_t n = Memory->Sent.size();
	client->Set_Debug_Mode(false);
	Check(Memory->Sent.size() == n, "Set_Debug_Mode false again sends nothing");
	Check(client->Toggle_Debug_Mode(0), "Toggle_Debug_Mode returns true");
	Check_Debug("Toggle_Debug_Mode sends DEBUG {1000, 0x1E, 1}", 1000, 0x1E, 1);
	client->Unknown11 = true;
	Check(client->Set_Debug_Object(6) == 1000, "Set_Debug_Object 6 returns 1000");
	Check_Debug("with +0x11 set, DEBUG {6, 0x21, 1}", 6, 0x21, 1);
	client->Unknown11 = false;
	Check(Memory->Sent.size() - before == 4, "four DEBUG messages over the memory connection");
}

// ------------------------------------------------------------------------- (3) Process_Packets

static bool Load_Messages(const char *path)
{
	FILE *f = fopen(path, "r");
	if (f == 0) {
		return false;
	}
	static char line[0x30000];
	while (fgets(line, sizeof(line), f)) {
		int opcode;
		char *p = line;
		if (sscanf(p, "%d", &opcode) != 1) {
			continue;
		}
		while (*p && *p != ' ') p++;
		while (*p == ' ') p++;
		std::vector<unsigned char> payload;
		while (p[0] && p[1] && p[0] != '\n' && p[0] != '\r') {
			unsigned v;
			char b[3] = { p[0], p[1], 0 };
			sscanf(b, "%x", &v);
			payload.push_back((unsigned char)v);
			p += 2;
		}
		Memory->Incoming.push_back(Message_Bytes(opcode, payload));
	}
	fclose(f);
	return true;
}

// SClient::Process_Packets stops at a handler that returns false and leaves the packet on the peek
// connection (PeekConnectionClass +0x34 keeps it): the next call hands the same packet over again
static void Test_Refuse(SClient *client)
{
	Trace("== (3a) a handler that refuses twice");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	std::vector<unsigned char> none;
	Memory->Incoming.push_back(Message_Bytes(RecordingPacketClass::REFUSE_OPCODE, none));
	int before = Memory->Received;
	client->Process_Packets();
	client->Process_Packets();
	Check(Memory->Received == before + 1, "refused twice: read from the connection once");
	client->Process_Packets();
	client->Process_Packets();
	Check(Memory->Received == before + 1 && Memory->Incoming.empty(), "taken on the third call; nothing more to read");
	Memory->Received = 0;
}

static void Test_Packets(SClient *client, const char *path)
{
	Trace("== (3) Process_Packets");
	if (Memory == 0 || !Load_Messages(path)) {
		Check(false, "messages loaded");
		return;
	}
	int total = (int)Memory->Incoming.size();
	Trace("messages %d", total);
	FakeLoadingViewClass loading;
	loading.Unknown68 = true;
	client->LoadingView = (LoadingViewClass *)&loading;
	// the first message with the task focused and the loading view up (Process_Packets tells it), then not
	Stub_Focused = true;
	if (!Memory->Incoming.empty()) {
		std::vector<unsigned char> first = Memory->Incoming.front();
		Memory->Incoming.pop_front();
		std::deque<std::vector<unsigned char> > rest;
		rest.swap(Memory->Incoming);
		Memory->Incoming.push_back(first);
		client->Process_Packets();
		Memory->Incoming.swap(rest);
	}
	Stub_Focused = false;
	unsigned ship = client->ShipID;
	float zmin = client->Unknown10D4, zmax = client->Unknown10D8;
	int creates = 0, created = 0, removes = 0, removed = 0;
	int starts = 0, started = 0, set_times = 0, set_timed = 0;
	while (!Memory->Incoming.empty()) {
		// one message per call, so that the state after each is traced
		std::vector<unsigned char> next = Memory->Incoming.front();
		Memory->Incoming.pop_front();
		std::deque<std::vector<unsigned char> > rest;
		rest.swap(Memory->Incoming);
		Memory->Incoming.push_back(next);
		size_t sent = Memory->Sent.size();
		client->Process_Packets();
		Memory->Incoming.swap(rest);
		int opcode = *(unsigned short *)&next[2];
		if (opcode == 5 && next.size() >= 4 + 4) {
			// START (pkstart.cpp: PlayerID): +0x1138 = PlayerID, then REQUEST_TIME 68 and START_ACK 6 (PlayerID) sent
			int player_id = *(int *)&next[4];
			starts++;
			if (client->Unknown1138 == player_id && Memory->Sent.size() == sent + 2 && *(unsigned short *)&Memory->Sent[sent][2] == 68
				&& *(unsigned short *)&Memory->Sent[sent + 1][2] == 6 && Memory->Sent[sent + 1].size() >= 8 && *(int *)&Memory->Sent[sent + 1][4] == player_id) {
				started++;
			}
		} else if (opcode == 52 && next.size() >= 4 + 12) {
			// CLIENT_SET_TIME (pkrequesttime.cpp: ClientSent, ServerReceived, ServerSent; ClientReceived the clock):
			// +0x10C0 = ((SR + SS) - (CS + CR)) / 2, +0x10C8 / +0x10D0 cleared, 0xBE71D4 = (CR - CS) - (SS - SR)
			__int64 cs = *(unsigned *)&next[4], sr = *(unsigned *)&next[8], ss = *(unsigned *)&next[12], cr = Unknown_Get_Ticks();
			__int64 correction = ((sr + ss) - (cs + cr)) / 2, rtt = (cr - cs) - (ss - sr);
			set_times++;
			Trace("state CLIENT_SET_TIME correction %d rtt %d", (int)correction, (int)rtt);
			if (client->Unknown10C0.Time == correction && client->Unknown10C8 == 0 && client->Unknown10D0 == 0 && Unknown_BE71D4 == DurationClass(rtt).As_Int()) {
				set_timed++;
			}
		}
		if (opcode == 4 && next.size() >= 4 + 11) {
			// CREATE's GameID, Scale, BaseAsset, Type (pkcreate.cpp's order): the object is in the table with them
			unsigned id = *(unsigned *)&next[4];
			ClientObjectClass *object = client->Find_Object(id);
			creates++;
			if (object && object->Get_Game_ID() == id && object->Type == (char)next[14] && object->BaseAsset == *(short *)&next[12] && object->Model) {
				created++;
			}
		} else if (opcode == 7 && next.size() >= 4 + 4) {
			removes++;
			if (client->Find_Object(*(unsigned *)&next[4]) == 0) {
				removed++;
			}
		}
		if (client->ShipID != ship) {
			ship = client->ShipID;
			Trace("state ship id %u", ship);
		}
		if (client->Unknown10D4 != zmin || client->Unknown10D8 != zmax) {
			zmin = client->Unknown10D4;
			zmax = client->Unknown10D8;
			Trace("state z band %g %g", zmin, zmax);
		}
	}
	Check(Memory->Received == total, "every message taken off the connection");
	Trace("CREATE x%d: %d objects in the table with the packet's game id, type, base asset and a model; REMOVE x%d: %d gone",
		creates, created, removes, removed);
	Check(creates > 0 && created == creates, "every CREATE 4 put a new object with its game id, type, base asset and model in the table");
	Check(removes > 0 && removed == removes, "every REMOVE 7 took its object out of the table");
	Trace("START x%d, CLIENT_SET_TIME x%d", starts, set_times);
	Check(starts > 0 && started == starts, "every START 5 set +0x1138 to its PlayerID and sent REQUEST_TIME 68 and START_ACK 6 with it");
	Check(set_times > 0 && set_timed == set_times, "every CLIENT_SET_TIME 52 set the clock offset +0x10C0, cleared +0x10C8 / +0x10D0 and stored the round trip time in 0xBE71D4");
	int recorded = 0;
	for (std::map<int, int>::iterator it = Recorded.begin(); it != Recorded.end(); it++) {
		Trace("recorded %s (%d) x%d", Opcode_Name(it->first, 0), it->first, it->second);
		recorded += it->second;
	}
	Trace("recorded %d, through cpacket.cpp's handlers %d", recorded, total - recorded);
	client->LoadingView = 0;
}

// ------------------------------------------------------------------------- (4) a positional packet to the model

// The message a packet's Build gives (what the server would send)
static std::vector<unsigned char> Built_Message(PacketClass *packet)
{
	RawPacketClass *raw = packet->Build();
	std::vector<unsigned char> m;
	if (raw == 0) {
		return m;
	}
	m.resize(raw->Header.Size < 4 ? 4 : raw->Header.Size);
	memcpy(&m[0], &raw->Header, 4);
	if (m.size() > 4) {
		memcpy(&m[4], raw->Data.Buffer, m.size() - 4);
	}
	delete raw;
	return m;
}

static bool Near(const Matrix3D &a, const Matrix3D &b)
{
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 4; c++) {
			float d = a[r][c] - b[r][c];
			if (d > 0.001f || d < -0.001f) {
				return false;
			}
		}
	}
	return true;
}

static void Trace_Matrix(const char *what, const Matrix3D &tm)
{
	for (int r = 0; r < 3; r++) {
		Trace("  %s row %d: %.4f %.4f %.4f %.4f", what, r, tm[r][0], tm[r][1], tm[r][2], tm[r][3]);
	}
}

// A server's packets build the object and move it (packet-flow.md 4): CREATE 4 through its real handler
// (cpacket.cpp 0x733700: co.cpp's constructor 0x6F2820, the model from base asset 900 through the loader
// 0x6F5140 and the fake asset manager, Set_Model 0x6F33F0 with cbasset.ini's radius and, for a type 0 ship,
// the six part models on its bones (0x6FCB70), the mount points 0x7251C0, Add_Object), then
// SIMPLE_POSITIONAL_UPDATE 8; each frame is SClient::Do_Frame 0x742B60: the age step (the player's and every
// object's aux data, slot +0x20), the clock +0x10BC from Unknown_Get_Ticks (Test_Ticks), input when focused,
// Process_Packets (the real handler, cpacket.cpp 0x733DC0: the simple state, postate.cpp 0x715AB0, over
// extrap.cpp's linear node 0x70F250), then Process_AI 0x744260: every object's per-frame update (co.cpp
// 0x6F3C60: slot +0x28 0x6F4610 asks the state for its matrix at the clock, re-orthogonalises it and hands it
// to the model's slot +0x50). A second packet updates the state in place (0x715C20: a 2000-tick blend 0x7125F0
// to the new linear node); past the blend's end the model follows the second packet. CHANGE_BASE_ASSET 38
// (0x737400) then gives the object base asset 901's model, which the next frame moves; REMOVE 7 (0x7362F0)
// deletes the object and its models. Do_Frame runs with what Init would have set (+0x1198 clear, the main view
// current, a bridge interface whose +0x14 is 0) and puts it back afterwards.
static int Frame(SClient *client, unsigned ticks, bool focused)
{
	Test_Ticks = ticks;
	Trace("frame at %u%s", ticks, focused ? ", focused" : "");
	return(client->Do_Frame(focused));
}

// The server's messages (pkcreate.cpp, pkchangebaseasset.cpp, pkremove.cpp: the raw constructors' order)
static std::vector<unsigned char> Create_Message(unsigned id, float scale, short base_asset, char type)
{
	std::vector<unsigned char> p(4 + 4 + 2 + 1 + 12, 0);	// GameID, Scale, BaseAsset, Type, HSV (0, 0, 0)
	memcpy(&p[0], &id, 4);
	memcpy(&p[4], &scale, 4);
	memcpy(&p[8], &base_asset, 2);
	p[10] = (unsigned char)type;
	return(Message_Bytes(4, p));
}
static std::vector<unsigned char> Change_Base_Asset_Message(unsigned id, int base_asset, float scale)
{
	std::vector<unsigned char> p(4 + 4 + 4 + 12, 0);	// GameID, BaseAsset, Scale, HSV (0, 0, 0)
	memcpy(&p[0], &id, 4);
	memcpy(&p[4], &base_asset, 4);
	memcpy(&p[8], &scale, 4);
	return(Message_Bytes(38, p));
}
static std::vector<unsigned char> Remove_Message(unsigned id)
{
	std::vector<unsigned char> p(4, 0);
	memcpy(&p[0], &id, 4);
	return(Message_Bytes(7, p));
}

static bool Model_Deleted(int number)
{
	for (size_t i = 0; i < Deleted_Models.size(); i++) {
		if (Deleted_Models[i] == number) {
			return(true);
		}
	}
	return(false);
}

static void Test_Path(SClient *client, FakeMainViewClass *main_view)
{
	Trace("== (4) a server's packets create the object, move its model, change and remove it");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	int clock = client->Unknown10BC;		// what the lag average reads: put back for (3)
	float lag = client->Unknown10C8;
	int lags = client->Unknown10D0;
	int toggle = client->Unknown1128;
	static char fake_bridge[0xC4];			// bridge_int.cpp's object: +0x14 0, so Do_Frame doesn't call it
	client->Unknown1198 = false;
	client->CurrentView = (ViewClass *)main_view;
	client->BridgeInterface = (BridgeInterfaceClass *)fake_bridge;
	Path_Views = true;
	// 0x512A00 (Do_Frame, the main view current): a radar (its 0x7BCA70 is client_stubs.cpp's) and a PDA whose +0xA8
	// object's 0x7C8DB0 is a stand-in
	static char fake_radar[0x50];
	static char fake_pda_a8[0x100];
	static char fake_pda[0xC0];
	*(void **)&fake_pda[0xA8] = fake_pda_a8;
	*(void **)&main_view->Unknown04[0x80 - 0x04] = fake_radar;
	client->PDAInterface = (PDAInterfaceClass *)fake_pda;
	const unsigned id = 4000;

	// CREATE 4: a ship (type 0) of base asset 900 at scale 0.5
	int alive = Models_Alive;
	Memory->Incoming.push_back(Create_Message(id, 0.5f, 900, 0));
	client->Process_Packets();
	ClientObjectClass *object = client->Find_Object(id);
	Check(object != 0 && object->Get_Game_ID() == id && object->Type == 0 && object->BaseAsset == 900 && object->Scale == 0.5f,
		"CREATE 4 (game id 4000, type 0, base asset 900, scale 0.5): Find_Object finds the new object with the packet's game id, type, base asset and scale");
	if (object == 0) {
		return;
	}
	FakeModelClass *model = Fake_Model_Of(object);
	Check(model != 0 && strcmp(model->Name, "ship_test") == 0 && Fake_Assets->Loads >= 1 && Fake_Assets->Files[0] == "ship_test.w3d",
		"the model was loaded for base asset 900 (basset.ini's BASE-900: FILE ship_test.w3d loaded, then NAME ship_test)");
	Check(object->Radius == 12.5f, "Set_Model's radius from cbasset.ini (BASE-900: (Radius 20 + RadiusAdjust 5) x scale 0.5 = 12.5)");
	bool parts = model != 0 && model->Bones == 6;
	for (int i = 0; i < 6; i++) {
		parts = parts && object->Unknown158[i].Peek() != 0 && object->Unknown158[i].Peek()->Is_Hidden();
	}
	Check(parts, "the ship's six part models (base assets 0x618, 0x617, 0x73F), hidden, hung on its ~FL ~FR ~BL ~BR bones");
	Check(object->Unknown98 != 0 && object->Unknown98->Unknown_04() == 0, "the new object holds the constructor's default state (type 0)");

	const Vector3 pos(100.0f, 200.0f, 30.0f);
	const Quaternion orient(0.0f, 0.0f, 0.70710677f, 0.70710677f);	// 90 degrees about Z
	const Vector3 vel(0.01f, 0.02f, -0.004f);						// per tick
	SimplePositionalUpdatePacketClass first(0, id, 1000, pos, orient, vel);
	Memory->Incoming.push_back(Built_Message(&first));
	int transforms = model->Transforms;
	int result = Frame(client, 1500, true);
	Check(result == 0 && client->Unknown10BC == 1500, "Do_Frame returns 0 and sets the clock from Unknown_Get_Ticks");
	Check(object->Unknown98 != 0 && object->Unknown98->Unknown_04() == 3, "SIMPLE_POSITIONAL_UPDATE 8 (time 1000) replaced it with the simple state (type 3)");
	Matrix3D want(orient, pos + vel * 500.0f);
	Trace_Matrix("model", model->LastTransform);
	Check(model->Transforms == transforms + 1, "Process_AI: the object's model got one Set_Transform");
	Check(Near(model->LastTransform, want), "at clock 1500 the model is at the packet's position + velocity * 500, turned 90 degrees about Z");

	const Vector3 pos2(-50.0f, 10.0f, 5.0f);
	const Quaternion orient2(0.0f, 0.0f, 0.0f, 1.0f);
	const Vector3 vel2(0.0f, -0.01f, 0.0f);
	SimplePositionalUpdatePacketClass second(0, id, 2000, pos2, orient2, vel2);
	Memory->Incoming.push_back(Built_Message(&second));
	Unknown98Class *state = object->Unknown98;
	Frame(client, 2000, false);
	Check(object->Unknown98 == state && state->Unknown_04() == 3, "a second SIMPLE packet (time 2000) updates the simple state in place");
	transforms = model->Transforms;
	Frame(client, 4500, false);
	Matrix3D want2(orient2, pos2 + vel2 * 2500.0f);
	Trace_Matrix("model", model->LastTransform);
	Check(model->Transforms == transforms + 1 && Near(model->LastTransform, want2), "at clock 4500 (past the 2000-tick blend) the model follows the second packet");

	// CHANGE_BASE_ASSET 38: base asset 901 at scale 1; the next frame moves the new model
	int old_number = model->Number;
	Memory->Incoming.push_back(Change_Base_Asset_Message(id, 901, 1.0f));
	Frame(client, 5000, false);
	FakeModelClass *model2 = Fake_Model_Of(object);
	Check(client->Find_Object(id) == object && model2 != 0 && model2->Number != old_number && strcmp(model2->Name, "ship_test2") == 0
		&& object->BaseAsset == 901 && object->Scale == 1.0f,
		"CHANGE_BASE_ASSET 38 (base asset 901, scale 1): the same object holds base asset 901's model (NAME ship_test2)");
	Check(Model_Deleted(old_number), "the old model was released and deleted");
	Matrix3D want3(orient2, pos2 + vel2 * 3000.0f);
	Check(model2 != 0 && model2->Transforms == 1 && Near(model2->LastTransform, want3), "at clock 5000 the new model follows the second packet");

	// REMOVE 7
	int number2 = model2 ? model2->Number : -1;
	Memory->Incoming.push_back(Remove_Message(id));
	client->Process_Packets();
	Check(client->Find_Object(id) == 0, "REMOVE 7: the object is out of the client's table");
	Check(Model_Deleted(number2) && Models_Alive == alive, "REMOVE 7 deleted the object, its model and its part models");

	Path_Views = false;
	*(void **)&main_view->Unknown04[0x80 - 0x04] = 0;	// the radar back to NULL (WARP_INDEX 156's FAIL_IF in (3))
	client->PDAInterface = 0;
	Test_Ticks = 0;
	client->Unknown1198 = true;
	client->CurrentView = 0;
	client->BridgeInterface = 0;
	client->Unknown1128 = toggle;
	client->Unknown10BC = clock;
	client->Unknown10C8 = lag;
	client->Unknown10D0 = lags;
	Memory->Received = 0;
}

// (4b) CREATE 4's subclasses (cpacket.cpp's switch, co_subtypes.cpp): type 24 (0x70E2E0 over 0x70DBC0, which is also
// dynamic_obj.cpp's DynObj at +0x188) and type 28 (0x70E5A0), from synthetic packets: no alex3 message
// creates either. Set_Model's slot +0x34 reaches type 24's override (0x70DCF0: the radius also into the proximity
// object's +0x04); SET_TARGET 25 (0x7363E0) calls type 28's 0x70E750; REMOVE 7 deletes both through their destructors.
static std::vector<unsigned char> Set_Target_Message(int target_id, int object_id)
{
	std::vector<unsigned char> p(8, 0);	// TargetID, the object's id (pkrequesttarget.cpp's raw constructor)
	memcpy(&p[0], &target_id, 4);
	memcpy(&p[4], &object_id, 4);
	return(Message_Bytes(25, p));
}

static void Test_Subtypes(SClient *client)
{
	Trace("== (4b) CREATE 4's types 24 and 28 make co_subtypes.cpp's subclasses");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	int alive = Models_Alive;
	const unsigned id24 = 4100, id28 = 4200;
	Memory->Incoming.push_back(Create_Message(id24, 0.5f, 900, 24));
	Memory->Incoming.push_back(Create_Message(id28, 1.0f, 901, 28));
	client->Process_Packets();

	ClientObjectClass *object24 = client->Find_Object(id24);
	Check(object24 != 0 && object24->Type == 24 && object24->BaseAsset == 900 && object24->Radius == 12.5f,
		"CREATE 4 type 24 (game id 4100): in the table with its type, base asset and Set_Model's radius 12.5");
	if (object24 != 0) {
		Unknown70E2E0Class *type24 = (Unknown70E2E0Class *)object24;
		DynObj *proximity = type24;
		Check((char *)proximity - (char *)type24 == 0x188 && proximity->Unknown_Dyn04() == proximity,
			"type 24's proximity object sits at +0x188 and its slot +0x04 (0x70DC90) returns itself");
		Check(proximity->ProximityObject != 0 && proximity->ProximityObject->Owner == proximity,
			"DynObj's constructor (0x44AC70) gave it a PxWrapper (a stand-in) that records its owner");
		Check(proximity->Unknown04 == 12.5f && type24->Unknown220 == false,
			"type 24's slot +0x34 (0x70DCF0) put the radius into the proximity object's +0x04 as well; +0x220 is false");
		Check(proximity->Unknown_Dyn20() == 24 && strcmp(proximity->Unknown_Dyn30(), "Unnamed ClientProximityObject") == 0,
			"through the proximity object: slot +0x20 (0x70E220) gives the type 24, slot +0x30 (0x70DD20) the name 'Unnamed ClientProximityObject'");
		Check(object24->Unknown_40() == (int)object24 && object24->Unknown_44() == (int)object24,
			"type 24's slots +0x40 (0x70DCB0) and +0x44 (0x70E330) return the object");
	}

	ClientObjectClass *object28 = client->Find_Object(id28);
	Check(object28 != 0 && object28->Type == 28 && object28->Unknown_6F5DC0(),
		"CREATE 4 type 28 (game id 4200): in the table, and 0x6F5DC0 (SET_TARGET's test) says type 28");
	if (object28 != 0) {
		Unknown70E5A0Class *type28 = (Unknown70E5A0Class *)object28;
		Check(type28->TargetID == -1 && type28->ParentID == -1 && type28->Bone == -1 && type28->Unknown1D0 == 1.0f,
			"type 28's constructor (0x70E5A0): no target, no parent, no bone, +0x1D0 1.0");
		Memory->Incoming.push_back(Set_Target_Message(id24, id28));
		client->Process_Packets();
		Check(type28->TargetID == (int)id24 && type28->Unknown1D0 == 0.0f,
			"SET_TARGET 25 (target 4100 for object 4200): type 28's 0x70E750 stores the target and clears +0x1D0");
	}

	Memory->Incoming.push_back(Remove_Message(id24));
	Memory->Incoming.push_back(Remove_Message(id28));
	client->Process_Packets();
	Check(client->Find_Object(id24) == 0 && client->Find_Object(id28) == 0 && Models_Alive == alive,
		"REMOVE 7 deleted both through their destructors (0x70E380 / 0x70E0C0, 0x70E6B0) with their models");
	Memory->Received = 0;
}

// (4c) GADGET 103 (0x739180) and INCAPACITANCE_DETAILS 144 (0x739BB0) from synthetic packets: no alex3 message has
// them. GADGET finds the gadget by the packet's Name in SClient +0x1278's list (gadget_list_manager.cpp 0x661940,
// a stand-in here that knows one gadget) and applies Function's bits: 1 / 2 the gadget's interface (slot +0x08)
// slot +0x8C on / off, 4 / 8 the gadget's slot +0x40 on / off, 0x10 0x6603A0(1, the current view's slot +0x1C),
// 0x20 0x6607D0 (both only with a current view +0x1350); a Name the list doesn't know takes the FAIL_IF '!gadget'.
// INCAPACITANCE_DETAILS puts the camera in mode 0x19 and hands the packet to the cockpit (main view +0x1D0, 0x584690).
class UIGadgetInterfaceClass;
class GadgetClass
{
public:
	void Unknown_6603A0(int arg, ViewClass *view);
	void Unknown_6607D0();
};
class GadgetListManagerClass
{
public:
	GadgetClass *Unknown_661940(const char *name);
};
static std::string Gadget_Log;
static void *Gadget_Table[32];
static void *Gadget_Iface_Table[64];
static FakeTableStruct Fake_Gadget_Iface = { Gadget_Iface_Table };
static FakeTableStruct Fake_Gadget = { Gadget_Table };
static FakeTableStruct *__stdcall Gadget_08_Slot() { Gadget_Log += "+08 "; return(&Fake_Gadget_Iface); }
static void __stdcall Gadget_40_Slot(bool on) { Gadget_Log += on ? "+40(1) " : "+40(0) "; }
static void __stdcall Gadget_Iface_8C_Slot(bool on) { Gadget_Log += on ? "iface+8C(1) " : "iface+8C(0) "; }
GadgetClass *GadgetListManagerClass::Unknown_661940(const char *name)
{
	Trace("stub GadgetListManagerClass::Unknown_661940 '%s'", name ? name : "(null)");
	return((name && strcmp(name, "gadget_test") == 0) ? (GadgetClass *)&Fake_Gadget : 0);
}
void GadgetClass::Unknown_6603A0(int arg, ViewClass *view) { Gadget_Log += arg == 1 && view ? "6603A0(1, view) " : "6603A0(?) "; }
void GadgetClass::Unknown_6607D0() { Gadget_Log += "6607D0 "; }
static void *Incap_Packet = 0;
static std::string Incap_Name;
void UICpitClass::Unknown_584690(IncapacitanceDetailsPacket *packet)
{
	Trace("stub UICpitClass::Unknown_584690 '%s'", packet ? packet->Unknown0C.c_str() : "(null)");
	Incap_Packet = packet;
	Incap_Name = packet ? packet->Unknown0C : "";
}

static void Test_Gadget(SClient *client, FakeMainViewClass *main_view)
{
	Trace("== (4c) GADGET 103 and INCAPACITANCE_DETAILS 144");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	for (int g = 0; g < 32; g++) {
		Gadget_Table[g] = (void *)Zero_Slot;
	}
	for (int i = 0; i < 64; i++) {
		Gadget_Iface_Table[i] = (void *)Zero_Slot;
	}
	Gadget_Table[0x08 / 4] = (void *)Gadget_08_Slot;
	Gadget_Table[0x40 / 4] = (void *)Gadget_40_Slot;
	Gadget_Iface_Table[0x8C / 4] = (void *)Gadget_Iface_8C_Slot;
	int list = client->Unknown1278;
	ViewClass *view = client->CurrentView;
	static char fake_list[0x40];
	client->Unknown1278 = (int)fake_list;
	client->CurrentView = (ViewClass *)main_view;

	Gadget_Log = "";
	Memory->Incoming.push_back(Built_Message(&GadgetPacketClass(0x3F, "gadget_test")));
	client->Process_Packets();
	Trace("gadget calls: %s", Gadget_Log.c_str());
	Check(Gadget_Log == "+08 iface+8C(1) iface+8C(0) +40(1) +40(0) 6603A0(1, view) 6607D0 ",
		"GADGET 103 Function 0x3F on 'gadget_test': the interface's +0x8C on and off, the gadget's +0x40 on and off, 0x6603A0(1, view), 0x6607D0");
	Gadget_Log = "";
	client->CurrentView = 0;
	Memory->Incoming.push_back(Built_Message(&GadgetPacketClass(0x14, "gadget_test")));
	client->Process_Packets();
	Check(Gadget_Log == "+08 +40(1) ", "GADGET 103 Function 0x14 without a current view: only the gadget's +0x40 on");
	Gadget_Log = "";
	Memory->Incoming.push_back(Built_Message(&GadgetPacketClass(0x3F, "gadget_none")));
	client->Process_Packets();
	Check(Gadget_Log == "", "GADGET 103 for a gadget the list doesn't know: the FAIL_IF '!gadget', nothing called");
	client->Unknown1278 = list;
	client->CurrentView = view;

	Incap_Packet = 0;
	Incap_Name = "";
	Memory->Incoming.push_back(Built_Message(&IncapacitanceDetailsPacket("rescuer", AvatarDescClass(), "a", "b", "c", "d", "e", "f", "g")));
	client->Process_Packets();
	Check(Incap_Packet != 0 && Incap_Name == "rescuer",
		"INCAPACITANCE_DETAILS 144: the camera's mode 0x19 (0x79B2B0), then the cockpit's 0x584690 gets the packet (Unknown0C 'rescuer')");
	Memory->Received = 0;
}

// (4d) FORMATION_POSITIONAL_UPDATE 65 (0x734740) from synthetic packets: no alex3 message has it. For an object other
// than the avatar (+0x112C) the handler puts in the type-8 state (postate.cpp 0x716080) whose extrapolator is
// extrap.cpp's formation node 0x714340 (the packet's LeaderID and Offset); for the avatar the type-4 state (0x7171C0);
// a second packet updates the state in place (0x716220: a blend from the old node to a new formation node).
static std::vector<unsigned char> Formation_Message(unsigned id, unsigned leader, float x, float y, float z)
{
	std::vector<unsigned char> p(4 + 4 + 12, 0);	// GameID, LeaderID, Offset (pkupdate.cpp's raw constructor)
	memcpy(&p[0], &id, 4);
	memcpy(&p[4], &leader, 4);
	memcpy(&p[8], &x, 4);
	memcpy(&p[12], &y, 4);
	memcpy(&p[16], &z, 4);
	return(Message_Bytes(65, p));
}

static Unknown714340Class *Formation_Node(ClientObjectClass *object)
{
	if (object == 0 || object->Unknown98 == 0 || (object->Unknown98->Unknown_04() != 8 && object->Unknown98->Unknown_04() != 4)) {
		return(0);
	}
	return((Unknown714340Class *)((UnknownB027A0Class *)object->Unknown98)->Extrap);
}

static void Test_Formation(SClient *client)
{
	Trace("== (4d) FORMATION_POSITIONAL_UPDATE 65");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	const unsigned leader = 4300, follower = 4301, avatar = 4302;
	Memory->Incoming.push_back(Create_Message(leader, 1.0f, 900, 0));
	Memory->Incoming.push_back(Create_Message(follower, 1.0f, 900, 0));
	Memory->Incoming.push_back(Create_Message(avatar, 1.0f, 900, 0));
	client->Process_Packets();
	unsigned saved = client->AvatarID;
	client->AvatarID = avatar;
	Memory->Incoming.push_back(Formation_Message(follower, leader, 10.0f, 0.0f, 0.0f));
	Memory->Incoming.push_back(Formation_Message(avatar, leader, 0.0f, 5.0f, 0.0f));
	client->Process_Packets();
	ClientObjectClass *f = client->Find_Object(follower);
	ClientObjectClass *a = client->Find_Object(avatar);
	Unknown714340Class *fn = Formation_Node(f);
	Unknown714340Class *an = Formation_Node(a);
	Check(f != 0 && f->Unknown98 != 0 && f->Unknown98->Unknown_04() == 8 && fn != 0 && fn->LeaderID == (int)leader
		&& fn->Offset.X == 10.0f && fn->Offset.Y == 0.0f && fn->Client == client,
		"FORMATION 65 for object 4301: the type-8 state (0x716080) with a formation node (0x714340) holding leader 4300 and offset (10, 0, 0)");
	Check(a != 0 && a->Unknown98 != 0 && a->Unknown98->Unknown_04() == 4 && an != 0 && an->LeaderID == (int)leader && an->Offset.Y == 5.0f,
		"FORMATION 65 for the avatar 4302: the type-4 state (0x7171C0) with leader 4300 and offset (0, 5, 0)");
	if (f != 0 && fn != 0) {
		Unknown98Class *state = f->Unknown98;
		Memory->Incoming.push_back(Formation_Message(follower, leader, 20.0f, 0.0f, 0.0f));
		client->Process_Packets();
		Check(f->Unknown98 == state && ((UnknownB027A0Class *)state)->Extrap != (StateNode *)fn,
			"a second FORMATION 65 for 4301 updates its state in place (0x716220: a new extrapolator, the blend to the new node)");
	}
	client->AvatarID = saved;
	Memory->Incoming.push_back(Remove_Message(leader));
	Memory->Incoming.push_back(Remove_Message(follower));
	Memory->Incoming.push_back(Remove_Message(avatar));
	client->Process_Packets();
	Check(client->Find_Object(leader) == 0 && client->Find_Object(follower) == 0 && client->Find_Object(avatar) == 0, "REMOVE 7 took the three out");
	Memory->Received = 0;
}

// (4e) VERB_LIST 91 (0x7387F0) from a synthetic packet: an object's verbs (pkverblist.cpp: GameID, then the set) go to
// SClient +0x1294's verb manager (verbmanager.cpp 0x81F780, a stand-in here that records them); without a manager
// the handler does nothing.
static int Verb_Calls = 0;
static int Verb_Game_ID = 0;
static std::set<VerbEnum> Verb_Set;
void VerbManagerClass::Unknown_81F780(int game_id, void *verbs)
{
	Verb_Calls++;
	Verb_Game_ID = game_id;
	Verb_Set = *(std::set<VerbEnum> *)verbs;
	Trace("stub VerbManagerClass::Unknown_81F780 %d, %d verbs", game_id, (int)Verb_Set.size());
}

static void Test_Verbs(SClient *client)
{
	Trace("== (4e) VERB_LIST 91");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	std::set<VerbEnum> verbs;
	verbs.insert((VerbEnum)1);
	verbs.insert((VerbEnum)3);
	verbs.insert((VerbEnum)7);
	VerbManagerClass *manager = client->VerbManager;
	client->VerbManager = 0;
	Verb_Calls = 0;
	Memory->Incoming.push_back(Built_Message(&VerbListPacketClass(4400, verbs)));
	client->Process_Packets();
	Check(Verb_Calls == 0, "VERB_LIST 91 without a verb manager: nothing called");
	static char fake_manager[0x14];
	client->VerbManager = (VerbManagerClass *)fake_manager;
	Memory->Incoming.push_back(Built_Message(&VerbListPacketClass(4400, verbs)));
	client->Process_Packets();
	Check(Verb_Calls == 1 && Verb_Set == verbs,
		"VERB_LIST 91 for object 4400: the verb manager's 0x81F780 gets the packet's GameID and its three verbs (1, 3, 7)");
	Trace("verb manager got game id %d", Verb_Game_ID);
	client->VerbManager = manager;
	Memory->Received = 0;
}

// (4f) STARBASE_AVATAR_UPDATE 158 (0x73A9F0) and STARBASE_AVATAR_ROOM_UPDATE 160 (0x73AB50) from synthetic packets: the
// starbase view (+0x135C) hands out its +0x60 interface (0x54EDA0, a RefCountPtr: the count at +0x3C goes up and back
// down); 158 moves an avatar (0x63E5C0: AvatarID, Orient, the position, ActionFlag) only when the interface's room
// (+0xAC) is the packet's Room and its +0x7C is set; 160 tells it an avatar's new room (0x63CF30) when +0x7C is set.
class UIStarbaseClass
{
public:
	void Unknown_63CF30(int avatar_id, int new_room);
	void Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action);
	void Unknown_6437A0(int unknown, AvatarDescClass *desc);
};
static std::string Starbase_Log;
void UIStarbaseClass::Unknown_63CF30(int avatar_id, int new_room)
{
	char b[64];
	sprintf(b, "63CF30(%d, %d) ", avatar_id, new_room);
	Starbase_Log += b;
}
void UIStarbaseClass::Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action)
{
	char b[96];
	sprintf(b, "63E5C0(%d, %g, (%g %g %g), %d) ", avatar_id, orient, position->X, position->Y, position->Z, action);
	Starbase_Log += b;
}
static std::vector<unsigned char> Starbase_Avatar_Message(int avatar, float orient, float x, float y, float z, int action, int room)
{
	std::vector<unsigned char> p(4 * 7, 0);	// AvatarID, Orient, the three floats, ActionFlag, Room (pkstarbase_avatar_change.cpp)
	memcpy(&p[0], &avatar, 4);
	memcpy(&p[4], &orient, 4);
	memcpy(&p[8], &x, 4);
	memcpy(&p[12], &y, 4);
	memcpy(&p[16], &z, 4);
	memcpy(&p[20], &action, 4);
	memcpy(&p[24], &room, 4);
	return(Message_Bytes(158, p));
}
static std::vector<unsigned char> Starbase_Room_Message(int avatar, int new_room, int old_room)
{
	std::vector<unsigned char> p(12, 0);	// AvatarID, NewRoom, OldRoom
	memcpy(&p[0], &avatar, 4);
	memcpy(&p[4], &new_room, 4);
	memcpy(&p[8], &old_room, 4);
	return(Message_Bytes(160, p));
}

static void Test_Starbase(SClient *client)
{
	Trace("== (4f) STARBASE_AVATAR_UPDATE 158 and STARBASE_AVATAR_ROOM_UPDATE 160");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	ViewClass *saved = client->Unknown135C;
	static double fake_view[0x200 / 8];
	static double fake_starbase[0x100 / 8];
	char *view = (char *)fake_view;
	char *starbase = (char *)fake_starbase;
	*(int *)(starbase + 0x3C) = 5;		// the reference count (+0x38's RefCountClass): never reaches 0 here
	*(int *)(starbase + 0xAC) = 3;		// its room
	*(bool *)(starbase + 0x7C) = true;
	*(char **)(view + 0x60) = starbase;
	client->Unknown135C = 0;
	Starbase_Log = "";
	Memory->Incoming.push_back(Starbase_Avatar_Message(77, 1.5f, 10.0f, 20.0f, 30.0f, 2, 3));
	client->Process_Packets();
	Check(Starbase_Log == "", "STARBASE_AVATAR_UPDATE 158 without a starbase view: nothing called");
	client->Unknown135C = (ViewClass *)view;
	Memory->Incoming.push_back(Starbase_Avatar_Message(77, 1.5f, 10.0f, 20.0f, 30.0f, 2, 3));
	Memory->Incoming.push_back(Starbase_Avatar_Message(78, 0.5f, 1.0f, 2.0f, 3.0f, 1, 4));
	client->Process_Packets();
	client->Process_Packets();
	Trace("starbase calls: %s", Starbase_Log.c_str());
	Check(Starbase_Log == "63E5C0(77, 1.5, (10 20 30), 2) " && *(int *)(starbase + 0x3C) == 5,
		"STARBASE_AVATAR_UPDATE 158: avatar 77 in room 3 moved (0x63E5C0 with Orient, the position and ActionFlag); room 4's ignored; the interface's count back to 5");
	Starbase_Log = "";
	Memory->Incoming.push_back(Starbase_Room_Message(77, 4, 3));
	client->Process_Packets();
	*(bool *)(starbase + 0x7C) = false;
	Memory->Incoming.push_back(Starbase_Room_Message(78, 5, 4));
	Memory->Incoming.push_back(Starbase_Avatar_Message(79, 0.0f, 0.0f, 0.0f, 0.0f, 0, 3));
	client->Process_Packets();
	client->Process_Packets();
	Trace("starbase calls: %s", Starbase_Log.c_str());
	Check(Starbase_Log == "63CF30(77, 4) ", "STARBASE_AVATAR_ROOM_UPDATE 160: avatar 77's new room 4 (0x63CF30); with +0x7C clear neither handler calls");
	client->Unknown135C = saved;
	Memory->Received = 0;
}

// (4g) the recustomize handlers from synthetic packets: RECUSTOMIZE_SHIP_START 129 (0x73F4B0) hands the ship's
// description, the costs and four ints to the starbase view's +0x1A0 (0x61F250; FAIL_IF '!ship_ui' without one),
// RECUSTOMIZE_AVATAR_START 131 (0x73F570) the costs to +0x19C (0x551C40; '!avatar_ui'), RECUSTOMIZE_AVATAR_UPDATE 133
// (0x73F5F0) its int and the avatar description to the +0x60 interface (0x6437A0). Stand-ins record the calls.
class SBaseViewUnknown1A0Class { public: void Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d); };
class SBaseViewUnknown19CClass { public: void Unknown_551C40(RecustomizationCostClass *cost); };
static std::string Recustomize_Log;
void SBaseViewUnknown1A0Class::Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d)
{
	char t[96];
	sprintf(t, "61F250(%s, %s, %d, %d, %d, %d) ", desc ? "desc" : "NULL", cost ? "cost" : "NULL", a, b, c, d);
	Recustomize_Log += t;
}
void SBaseViewUnknown19CClass::Unknown_551C40(RecustomizationCostClass *cost) { Recustomize_Log += cost ? "551C40(cost) " : "551C40(NULL) "; }
void UIStarbaseClass::Unknown_6437A0(int unknown, AvatarDescClass *desc)
{
	char t[64];
	sprintf(t, "6437A0(%d, %s) ", unknown, desc ? "desc" : "NULL");
	Recustomize_Log += t;
}

static void Test_Recustomize(SClient *client)
{
	Trace("== (4g) RECUSTOMIZE_SHIP_START 129, RECUSTOMIZE_AVATAR_START 131, RECUSTOMIZE_AVATAR_UPDATE 133");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	ViewClass *saved = client->Unknown135C;
	static double fake_view[0x200 / 8];
	static double fake_starbase[0x100 / 8];
	static char fake_ship_ui[4], fake_avatar_ui[4];
	char *view = (char *)fake_view;
	*(int *)((char *)fake_starbase + 0x3C) = 5;
	*(double **)(view + 0x60) = fake_starbase;
	client->Unknown135C = (ViewClass *)view;
	// 129 and 131 are called directly: their cost classes (shipcust / recustomization costs) are stand-ins here, so
	// the packets can't go through Build and the factory; 133 goes through the connection
	ShipCustDescClass desc;
	ShipRecustomizationCostClass ship_cost;
	RecustomizationCostClass avatar_cost;
	RecustomizeShipStartPacketClass ship_start(1, desc, ship_cost, 2, 3, 4, 5);
	RecustomizeAvatarStartPacketClass avatar_start(6, avatar_cost);
	Trace("packets made");
	Recustomize_Log = "";
	ship_start.Process_Packet(client);
	avatar_start.Process_Packet(client);
	Check(Recustomize_Log == "", "129 and 131 without the view's +0x1A0 / +0x19C: the FAIL_IFs '!ship_ui', '!avatar_ui', nothing called");
	*(char **)(view + 0x1A0) = fake_ship_ui;
	*(char **)(view + 0x19C) = fake_avatar_ui;
	ship_start.Process_Packet(client);
	avatar_start.Process_Packet(client);
	Memory->Incoming.push_back(Built_Message(&RecustomizeAvatarUpdatePacketClass(7, AvatarDescClass())));
	client->Process_Packets();
	Trace("recustomize calls: %s", Recustomize_Log.c_str());
	Check(Recustomize_Log == "61F250(desc, cost, 2, 3, 4, 5) 551C40(cost) 6437A0(7, desc) ",
		"129: +0x1A0's 0x61F250 gets the description, the costs and +0x1A4..+0x1B0; 131: +0x19C's 0x551C40 the costs; 133: the interface's 0x6437A0 its int 7 and the description");
	client->Unknown135C = saved;
	Memory->Received = 0;
}

// ------------------------------------------------------------------------- main

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("usage: clienttest OUT.txt [MESSAGES.txt]\n");
		return 2;
	}
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	Out = fopen(argv[1], "w");
	if (Out == 0) {
		return 2;
	}
	for (int i = 0; i < 256; i++) {
		Zero_Table[i] = (void *)Zero_Slot;
		Track_Table[i] = (void *)Track_Slot;
	}
	Zero_Table[75] = (void *)Lod_Bias_Slot;
	Track_Table[75] = (void *)Lod_Bias_Slot;
	Zero_Table[20] = (void *)Set_Transform_Slot;
	Track_Table[20] = (void *)Set_Transform_Slot;
	for (int k = 0; k < 16; k++) {
		View64_Table[k] = (void *)Zero_Slot;
	}
	View64_Table[9] = (void *)View64_24_Slot;
	for (int s = 0; s < 64; s++) {
		View1DC_Table[s] = (void *)Zero_Slot;
	}
	View1DC_Table[0x88 / 4] = (void *)View1DC_88_Slot;
	View1DC_Table[0x8C / 4] = (void *)View1DC_8C_Slot;
	for (int c = 0; c < 64; c++) {
		Cpit_Table[c] = (void *)Zero_Slot;
		if (c < 32) View138_Table[c] = (void *)Zero_Slot;
	}
	for (int t = 0; t < 32; t++) {
		Scene10C_Table[t] = (void *)Zero_Slot;
	}
	Scene10C_Table[3] = (void *)Scene10C_Add_Slot;
	Scene10C_Table[24] = (void *)Scene10C_Remove_All_Slot;
	View138_Table[0x58 / 4] = (void *)View138_58_Slot;
	View138_Table[0x5C / 4] = (void *)View138_5C_Slot;
	Cpit_Table[0x58 / 4] = (void *)Cpit_58_Slot;
	Cpit_Table[0x5C / 4] = (void *)Cpit_5C_Slot;
	Fake_Model.Table = Zero_Table;
	Track_Model.Table = Track_Table;
	for (int a = 0; a < 256; a++) {
		Aux_Table[a] = (void *)Zero_Slot;
	}
	Aux_Table[1] = (void *)Aux_Delete_Slot;
	Aux_Table[8] = (void *)Aux_Age_Slot;
	*(void ***)Fake_Aux = Aux_Table;			// the aux data's slot +0x20, Do_Frame's age step (0x6F5120 for the objects)
	*(void ***)Fake_Player_Root = Aux_Table;
	*(void ***)Fake_Camera = Zero_Table;	// its Validate_Transform (virtual) does nothing; the Transform stays zero

	Trace("== SClient (its constructor, the base's)");
	SClient *client = new SClient(0);
	The_Client = client;
	FakeMainViewClass main_view;
	client->MainView = (MainViewClass *)&main_view;
	// 0x513700 (SERVER_PARAMETERS 66) calls the main view's stars (+0x134: a real StarFieldClass; no stars.ini in the
	// working directory, so its Init loads nothing) and orbitals (+0x140: zeroed, OrbitalManager's members are stand-ins)
	static double fake_orbitals[(sizeof(OrbitalManager) + 7) / 8];
	*(StarFieldClass **)&main_view.Unknown04[0x134 - 0x04] = new StarFieldClass(0);
	*(OrbitalManager **)&main_view.Unknown04[0x140 - 0x04] = (OrbitalManager *)fake_orbitals;
	// 0x513540 / 0x513600 (SClient 0x745000, 0x7456B0) call the +0x1DC object's slots +0x88 / +0x8C
	*(FakeTableStruct **)&main_view.Unknown04[0x1DC - 0x04] = &Fake_View1DC;
	// 0x514010 (SClient 0x745F30, CLIENT_TYPE 60) turns the cockpit (+0x1D0, zeroed: no scanner) and the +0x138
	// object (+0x7C false) off and on
	static void **fake_cpit[0x100 / 4] = { Cpit_Table };
	static void **fake_138[0xA0 / 4] = { View138_Table };
	*(void ***)&main_view.Unknown04[0x1D0 - 0x04] = (void **)fake_cpit;
	*(void ***)&main_view.Unknown04[0x138 - 0x04] = (void **)fake_138;
	static void **fake_scene[0x100 / 4] = { Scene10C_Table };	// the +0x10C scene (0x514010)
	*(void ***)&main_view.Unknown04[0x10C - 0x04] = (void **)fake_scene;
	*(SClient **)&main_view.Unknown04[0x4C - 0x04] = client;	// +0x4C, the view's client (0x514010's G_ASSERT reads its mode)
	static char fake_line_display[0x100];	// ui_line_display.cpp's object (0x100 bytes): client_stubs.cpp's members trace
	client->LineDisplay = (UILineDisplayClass *)fake_line_display;
	client->AuxData = (DeltaPlayerClass *)Fake_Player_Root;	// Init's roots (sclient.cpp 0x7518A0, not run)
	client->Unknown12C4[0] = (SClientUnknown12C4Class *)Fake_Player_Var_Root;
	Check(client->Unknown_18() == (int)client, "SClient's slot +0x18 returns the client");
	// the asset manager (after the SClient, whose constructor runs as before without one)
	Fake_Assets = new FakeAssetManagerClass;
	Unknown_BDADC0 = Fake_Assets;
	Check(WW3DAssetManager::Get_Instance() == Fake_Assets, "the fake asset manager is *0xBDADC0 and WW3DAssetManager::Get_Instance()");

	Test_Lists(client);
	Test_Debug(client);
	Test_Refuse(client);
	Test_Path(client, &main_view);
	Test_Subtypes(client);
	if (argc > 2) {
		Test_Packets(client, argv[2]);
	}
	Test_Gadget(client, &main_view);	// (4c) and (4d) after (3): their models would renumber (3)'s
	Test_Formation(client);
	Test_Verbs(client);
	Test_Starbase(client);
	Test_Recustomize(client);
	// the client isn't deleted: its destructor reaches control_keys_manager.cpp and WWAudio
	Trace("== done: %d checks failed", Failures);
	fclose(Out);
	printf("clienttest: %d lines, %d checks failed\n", Lines + 1, Failures);
	return Failures ? 1 : 0;
}
