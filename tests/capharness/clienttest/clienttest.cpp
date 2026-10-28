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
//     handler, the rest become one recording test packet (counted per opcode); CREATE also adds a
//     client object with its game id (the trace shows the main view's 'add'), so REMOVE and the object
//     handlers find it; an odd game id gets a fake rs_track model. Before that, (3a): a packet whose
//     handler returns false stays on the peek connection and comes back on the next call, and (4): a
//     positional packet to the model's transform (P-UP1-1): SIMPLE_POSITIONAL_UPDATE 8 through the real
//     handler and postate / extrap's state, then SClient::Process_AI and co.cpp's per-frame update, which
//     hand the state's matrix at the client clock to the model's slot +0x50 (Set_Transform).
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
#include "clienttest.h"
#include "..\..\..\app\client\sclient.h"
#include "..\..\..\app\common\connect.h"
#include "..\..\..\app\packets\pk.h"
#include "..\..\..\app\packets\pkdebug.h"
#include "..\..\..\app\packets\pkcreate.h"
#include "..\..\..\app\packets\pkupdate.h"

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

unsigned int Unknown_Get_Ticks() { return 0; }		// the aux-data clock (dbgconn.cpp's rate wrapper)

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
static FakeTableStruct Fake_View64 = { View64_Table };
static double Fake_Camera_Storage[0x400 / sizeof(double)];
static char *Fake_Camera = (char *)Fake_Camera_Storage;
static void __stdcall View64_24_Slot(int arg) { Trace("mainview 64's +0x24 %d", arg); }	// MainViewUnknown64Class +0x24: one argument, popped
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

// A client object's model (co.cpp +0xAC): the render-state handlers ask its Class_ID; the slots that take
// an argument get __stdcall stand-ins (Set_Transform +0x50, Set_LOD_Bias +0x12C). Every other slot of a fake's table returns the same value: an object with
// an odd game id gets an rs_track model (0x10001, the handlers call RsTrackClass), an even one a model
// that isn't (0).
static int Zero_Slot() { return 0; }
static int Track_Slot() { return 0x10001; }
static void __stdcall Lod_Bias_Slot(float bias) { Trace("model Set_LOD_Bias %g", bias); }	// slot +0x12C (75), CLIENT_AVATAR 55: one argument, popped
struct FakeModelStruct
{
	void **Table;
	char Unknown04[0x100];
};
// slot +0x50 (20), co.cpp's per-frame update 0x6F3C60: Set_Transform (one argument, popped); the path
// model (4) keeps the matrix it was given
static void __stdcall Set_Transform_Slot(const Matrix3D &tm) { Trace("model Set_Transform (%g, %g, %g)", tm[0][3], tm[1][3], tm[2][3]); }
static Matrix3D Path_Transform(true);
static int Path_Transforms = 0;
static void __stdcall Path_Set_Transform_Slot(const Matrix3D &tm)
{
	Path_Transform = tm;
	Path_Transforms++;
	Trace("path model Set_Transform (%g, %g, %g)", tm[0][3], tm[1][3], tm[2][3]);
}
static void *Zero_Table[256];
static void *Track_Table[256];
static void *Path_Table[256];
static FakeModelStruct Fake_Model;
static FakeModelStruct Track_Model;
static FakeModelStruct Path_Model;

static char Fake_Aux[0x4000];		// the player aux data co.cpp's accessors read (zeros)
static char Fake_Player_Root[0x10];	// SClient +0x12C0 (Init's DeltaPlayerClass: AUX_DATA 27 with game id 0)
static char Fake_Player_Var_Root[0x10];	// SClient +0x12C4's entry 0 (Init's player-variable root: PLAYER_VAR_AUX_DATA 28)
const char *Aux_Root_Name(const void *root)
{
	return(root == Fake_Player_Root ? "player" : root == Fake_Player_Var_Root ? "player var 0" : root == Fake_Aux ? "object" : "unknown");
}

// co.cpp's object through its own constructor and destructor (app.lib); the harness's fakes (aux data, model)
// go back before the destructor would release them
class TestObjectClass : public ClientObjectClass
{
public:
	TestObjectClass(ClientClass *client, unsigned id) : ClientObjectClass(client, 0, id) {}	// type 0: a player ship for co.cpp's type tests (0x6F5A10); the client: the per-frame update reads its clock
	virtual ~TestObjectClass() { Trace("stub ClientObjectClass::~ClientObjectClass %u", GameID); AuxData = 0; Model = 0; }
};

static SClient *The_Client = 0;

static ClientObjectClass *New_Object(unsigned id)
{
	ClientObjectClass *object = new TestObjectClass(The_Client, id);
	object->AuxData = Fake_Aux;		// (the constructor's own aux root, if any, is left alone)
	// the constructor's default state (type 0, identity) stays: the positional handlers replace it, and
	// Process_AI's per-frame update (4) asks every object's state for its matrix
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
			if (opcode == 4) {
				// CREATE: the object the client would make (co.cpp isn't rebuilt), with the message's game id
				CreatePacketClass *create = (CreatePacketClass *)Unknown_Create_Packet(&m[0]);
				if (create) {
					Client->Add_Object(New_Object((unsigned)create->GameID));
					delete create;
				}
			}
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
	Check(client->Get_Debug_Object() == client->Find_Object(1000), "Get_Debug_Object is object 1000");
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
	while (!Memory->Incoming.empty()) {
		// one message per call, so that the state after each is traced
		std::vector<unsigned char> next = Memory->Incoming.front();
		Memory->Incoming.pop_front();
		std::deque<std::vector<unsigned char> > rest;
		rest.swap(Memory->Incoming);
		Memory->Incoming.push_back(next);
		client->Process_Packets();
		Memory->Incoming.swap(rest);
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

// One object (the harness's, as for CREATE) gets SIMPLE_POSITIONAL_UPDATE 8 through Process_Packets and the
// real handler (cpacket.cpp 0x733DC0: the simple state, postate.cpp 0x715AB0, over extrap.cpp's linear node
// 0x70F250); SClient::Process_AI 0x744260 then runs every object's per-frame update (co.cpp 0x6F3C60:
// slot +0x28 0x6F4610 asks the state for its matrix at the client clock +0x10BC, re-orthogonalises it and
// hands it to the model's slot +0x50). A second packet updates the state in place (0x715C20: a 2000-tick
// blend 0x7125F0 to the new linear node); past the blend's end the model follows the second packet.
static void Test_Path(SClient *client)
{
	Trace("== (4) a positional packet to the model's transform");
	if (Memory == 0) {
		Check(false, "the memory connection");
		return;
	}
	int clock = client->Unknown10BC;		// what the lag average reads: put back for (3)
	float lag = client->Unknown10C8;
	int lags = client->Unknown10D0;
	const unsigned id = 4000;
	ClientObjectClass *object = New_Object(id);
	object->Model = (RenderObjClass *)&Path_Model;
	client->Add_Object(object);
	Check(object->Unknown98 != 0 && object->Unknown98->Unknown_04() == 0, "the new object holds the constructor's default state (type 0)");

	const Vector3 pos(100.0f, 200.0f, 30.0f);
	const Quaternion orient(0.0f, 0.0f, 0.70710677f, 0.70710677f);	// 90 degrees about Z
	const Vector3 vel(0.01f, 0.02f, -0.004f);						// per tick
	SimplePositionalUpdatePacketClass first(0, id, 1000, pos, orient, vel);
	client->Unknown10BC = 1000;
	Memory->Incoming.push_back(Built_Message(&first));
	client->Process_Packets();
	Check(object->Unknown98 != 0 && object->Unknown98->Unknown_04() == 3, "SIMPLE_POSITIONAL_UPDATE 8 replaced it with the simple state (type 3)");

	Path_Views = true;
	client->Unknown10BC = 1500;
	Path_Transforms = 0;
	client->Process_AI();
	Matrix3D want(orient, pos + vel * 500.0f);
	Trace_Matrix("model", Path_Transform);
	Check(Path_Transforms == 1, "Process_AI: the object's model got one Set_Transform");
	Check(Near(Path_Transform, want), "at clock 1500 the model is at the packet's position + velocity * 500, turned 90 degrees about Z");

	const Vector3 pos2(-50.0f, 10.0f, 5.0f);
	const Quaternion orient2(0.0f, 0.0f, 0.0f, 1.0f);
	const Vector3 vel2(0.0f, -0.01f, 0.0f);
	SimplePositionalUpdatePacketClass second(0, id, 2000, pos2, orient2, vel2);
	client->Unknown10BC = 2000;
	Memory->Incoming.push_back(Built_Message(&second));
	Unknown98Class *state = object->Unknown98;
	client->Process_Packets();
	Check(object->Unknown98 == state && state->Unknown_04() == 3, "a second SIMPLE packet updates the simple state in place");
	client->Unknown10BC = 4500;
	Path_Transforms = 0;
	client->Process_AI();
	Matrix3D want2(orient2, pos2 + vel2 * 2500.0f);
	Trace_Matrix("model", Path_Transform);
	Check(Path_Transforms == 1 && Near(Path_Transform, want2), "at clock 4500 (past the 2000-tick blend) the model follows the second packet");
	Path_Views = false;

	client->Unknown10BC = clock;
	client->Unknown10C8 = lag;
	client->Unknown10D0 = lags;
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
	memcpy(Path_Table, Zero_Table, sizeof(Path_Table));
	Path_Table[20] = (void *)Path_Set_Transform_Slot;
	for (int k = 0; k < 16; k++) {
		View64_Table[k] = (void *)Zero_Slot;
	}
	View64_Table[9] = (void *)View64_24_Slot;
	Fake_Model.Table = Zero_Table;
	Track_Model.Table = Track_Table;
	Path_Model.Table = Path_Table;
	*(void ***)Fake_Camera = Zero_Table;	// its Validate_Transform (virtual) does nothing; the Transform stays zero

	Trace("== SClient (its constructor, the base's)");
	SClient *client = new SClient(0);
	The_Client = client;
	FakeMainViewClass main_view;
	client->MainView = (MainViewClass *)&main_view;
	static char fake_line_display[0x100];	// ui_line_display.cpp's object (0x100 bytes): client_stubs.cpp's members trace
	client->LineDisplay = (UILineDisplayClass *)fake_line_display;
	client->AuxData = (DeltaPlayerClass *)Fake_Player_Root;	// Init's roots (sclient.cpp 0x7518A0, not run)
	client->Unknown12C4[0] = (SClientUnknown12C4Class *)Fake_Player_Var_Root;
	Check(client->Unknown_18() == (int)client, "SClient's slot +0x18 returns the client");

	Test_Lists(client);
	Test_Debug(client);
	Test_Refuse(client);
	Test_Path(client);
	if (argc > 2) {
		Test_Packets(client, argv[2]);
	}
	// the client isn't deleted: its destructor reaches control_keys_manager.cpp and WWAudio
	Trace("== done: %d checks failed", Failures);
	fclose(Out);
	printf("clienttest: %d lines, %d checks failed\n", Lines + 1, Failures);
	return Failures ? 1 : 0;
}
