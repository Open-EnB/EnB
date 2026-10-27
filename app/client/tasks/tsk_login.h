/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_login.h (name assumed from tsk_login.cpp)
**
**	LoginTaskClass (the client's name: tsk_login.cpp's asserts): login, galaxy list, character
**	select and creation, then a ClientTaskClass. Vtable 0xB040D4 (29 slots), 0x1148 bytes
**	(docs/kernel-map.md 3). The members are traced only as far as the rebuilt functions use them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TSK_LOGIN_H  // NAME 3 [tsk_login.h]: header name assumed from the client's tsk_login.cpp
#define TSK_LOGIN_H

#include "..\..\kernel\kernel.h"
#include "tsk_game.h"
#include <string>
#include <list>
#include "vector3.h"
#include "vector2.h"
#include "wwstring.h"

class ConnectionClass;
class Packet_0078Class;
class GlobalErrorPacketClass;
class Packet_0076Class;
class LinearBufferClass;
class LoginUnknown0CClass;
class LoginTaskClass;

// The character view (charview.cpp: constructor 0x678450, 0x514 bytes), declared only as far as
// LoginTaskClass uses it.
class CharacterViewUnknown34Class	// NAME 1: neutral (D10): what CharacterViewClass +0x34 points to
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
	virtual void Unknown_5C();							// +0x5C: before each message box
	void Unknown_588A70(const char *button, int a, const char *text, int width, int b, int c);	// 0x588A70: a message box  // NAME 1 [button, text, width]: 'OK', the message, 320
	int Unknown_5890B0(const char *button);				// 0x5890B0: the button pressed ('OK', 'OK_SMALL', 'CANCEL', 'HTML')

	char Unknown004[0x7C - 0x04];						// +0x04
	bool Unknown07C;									// +0x7C: LoginTaskClass::Run looks at the buttons when set
	char Unknown07D[0x18C - 0x7D];						// +0x7D
	char Unknown18C[4];									// +0x18C: a name, compared with 'HTML'
};

class CharacterViewUnknown1B4Class	// NAME 1: neutral (D10): what CharacterViewClass +0x1B4 points to
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
	virtual void Unknown_5C();
	virtual void Unknown_60();
	virtual void Unknown_64();
	virtual void Unknown_68();
	virtual void Unknown_6C();
	virtual void Unknown_70();
	virtual void Unknown_74();
	virtual bool Unknown_78(GlobalErrorPacketClass *packet);	// +0x78: true when it handled the global error (0x76B000)
};

class CharacterViewClass	// NAME 1: the member's name CharacterView (the FAIL_IFs)
{
public:
	CharacterViewClass(LoginTaskClass *task, int step);	// 0x678450
	virtual ~CharacterViewClass();						// +0x00
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C(bool on);					// +0x0C: LoginTaskClass::Render
	virtual void Unknown_10();							// +0x10: before it is deleted
	virtual void Unknown_14();
	virtual void Unknown_18(int unknown);				// +0x18: before it is deleted (0)
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();
	virtual void *Unknown_3C();							// +0x3C: handed to LoginUnknown18Class::Unknown_515340
	void Unknown_67A300();								// 0x67A300 (charview.cpp)
	void Unknown_67A370(int step);						// 0x67A370 (charview.cpp)

	LoginUnknown0CClass *Unknown004;					// +0x04: gets the event Pre_Run's +0x0C didn't take (0x429A20)
	char Unknown008[0x2C - 0x08];						// +0x08
	void *Unknown02C;									// +0x2C: when set, Pre_Run's event comes from 0x65F780 (gadget.cpp)
	char Unknown030[0x34 - 0x30];						// +0x30
	CharacterViewUnknown34Class *Unknown034;			// +0x34
	char Unknown038[0x1B4 - 0x38];						// +0x38
	CharacterViewUnknown1B4Class *Unknown1B4;			// +0x1B4
	char Unknown1B8[0x1CD - 0x1B8];						// +0x1B8
	bool Unknown1CD;									// +0x1CD
	bool Unknown1CE;									// +0x1CE
	bool Unknown1CF;									// +0x1CF
	bool Unknown1D0;									// +0x1D0
	char Unknown1D1[0x514 - 0x1D1];						// +0x1D1
};


// The interface vtable 0xAEB9EC (three pure slots): pk.h's Unknown_PacketDataClass with its third slot
class LoginPacketDataClass	// NAME 1: neutral (D10)
{
public:
	virtual void Write(LinearBufferClass *buffer) = 0;	// +0x00 NAME 2: pk.h's
	virtual void Read(LinearBufferClass *buffer) = 0;	// +0x04 NAME 2: pk.h's
	virtual int Get_Size() = 0;							// +0x08 NAME 1: the galaxy's returns 0x54, the bytes Write writes
};

// A galaxy of the login's list (vtable 0xAEFBD0, 0x60 bytes; the avatar list packet's, 0x862D90)
class LoginGalaxyClass : public LoginPacketDataClass	// NAME 1: what it holds
{
public:
	~LoginGalaxyClass() {}								// 0x76BBF0 (stores the vtable: explicit)
	virtual void Write(LinearBufferClass *buffer);		// +0x00 0x83C730
	virtual void Read(LinearBufferClass *buffer);		// +0x04 0x83C7C0
	virtual int Get_Size();								// +0x08 0x83C850

	char Name[0x41];									// +0x04 NAME 1: 0x40 bytes on the wire
	int ID;												// +0x48 NAME 1: compared with the characters' Galaxy_ID
	unsigned long Address;								// +0x4C NAME 1: Galaxy_Address's source
	short Unknown50;									// +0x50
	int Unknown54;										// +0x54
	int Unknown58;										// +0x58
	short Unknown5C;									// +0x5C
};

// What the global ticket packet's handler 0x76B170 copies (0x9C bytes; step 16 copies it whole)
struct LoginTicketInfoClass	// NAME 1: what it holds
{
	LoginTicketInfoClass() : Unknown20(0), Unknown24(0) {}	// (LoginTaskClass's constructors)
	std::string Unknown00;								// +0x00
	std::string Unknown10;								// +0x10
	int Unknown20;										// +0x20
	int Unknown24;										// +0x24
	std::string Unknown28;								// +0x28
	std::string Login_Key;								// +0x38 NAME 1: ClientTaskClass::Init's login key ('lkey')
	std::string Unknown48;								// +0x48
	std::string Unknown58;								// +0x58
	std::string Unknown68;								// +0x68
	std::string Unknown78;								// +0x78
	std::string Unknown88;								// +0x88
	int Unknown98;										// +0x98: handed to 0x8190D0 (step 16)
};


// The one-slot interface vtable 0xAEADC8 (delta.h's DeltaInterfaceClass has the same table)
class LoginInterfaceAEADC8Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(void *visitor) = 0;
};

// A 0x44-byte object (constructor 0x6191F0), four in each LoginUnknown88CClass
class LoginUnknown44Class	// NAME 1: neutral (D10)
{
public:
	LoginUnknown44Class();								// 0x6191F0
	char Unknown00[0x44];
};

// The five 0x160-byte objects at LoginTaskClass +0x88C (vtables 0xAF78A0 / 0xAF789C over 0xAEB9EC /
// 0xAEADC8). Its constructor is inline: in both LoginTaskClass constructors, in 0x76A370, and out of
// line at 0x7688F0 after its first caller 0x7683F0.
class LoginUnknown88CClass : public LoginPacketDataClass, public LoginInterfaceAEADC8Class	// NAME 1: neutral (D10)
{
public:
	LoginUnknown88CClass() :
		Unknown08(0), Unknown0C(0), Unknown10(0), Unknown30(1.0F, 1.0F, 1.0F),
		Unknown3C(0.5F, 0.5F), Unknown44(0x7FFFFFF)
	{
		Unknown14 = false;
		Unknown_4F6210(0);
		Unknown_4F62F0(0);
		Unknown_4F6230();
	}
	virtual void Write(LinearBufferClass *buffer);		// +0x00 0x4F6330 (thunk 0x40C7F2)
	virtual void Read(LinearBufferClass *buffer);		// +0x04 (thunk 0x405F9C)
	virtual int Get_Size();								// +0x08 (thunk 0x414AB5)
	virtual void Unknown_0C();							// +0x0C (thunk 0x403701)
	virtual void Unknown_00(void *visitor);				// second table +0x00 0x4F6750 (thunk 0x40AEE8)
	LoginUnknown88CClass &operator=(const LoginUnknown88CClass &that);	// 0x55D8A0
	void Unknown_4F6210(int unknown);					// 0x4F6210
	void Unknown_4F6230();								// 0x4F6230
	void Unknown_4F62F0(int unknown);					// 0x4F62F0

	int Unknown08;										// +0x08
	int Unknown0C;										// +0x0C
	int Unknown10;										// +0x10
	bool Unknown14;										// +0x14
	char Unknown15[0x30 - 0x15];						// +0x15
	Vector3 Unknown30;									// +0x30
	Vector2 Unknown3C;									// +0x3C
	int Unknown44;										// +0x44
	LoginUnknown44Class Unknown48[4];					// +0x48
	char Unknown158[0x160 - 0x158];						// +0x158
};

// A character slot of the login's list (0x90 bytes, vtables 0xB0C560 / 0xB0C55C; LoginTaskClass's
// destructor destroys five with 0x8C0750). Its assignment is the implicit one (0x7683F0, 0x768CB0).
class LoginCharacterClass : public LoginPacketDataClass, public LoginInterfaceAEADC8Class	// NAME 1: what it holds
{
public:
	LoginCharacterClass();								// 0x8C06E0
	~LoginCharacterClass();								// 0x8C0750
	virtual void Write(LinearBufferClass *buffer);		// +0x00 (vtable 0xB0C560)
	virtual void Read(LinearBufferClass *buffer);		// +0x04
	virtual int Get_Size();								// +0x08
	virtual void Unknown_00(void *visitor);				// second table +0x00 (0xB0C55C)

	int Unknown08;										// +0x08
	int Unknown0C;										// +0x0C
	int Galaxy_ID;										// +0x10 NAME 1: compared with the galaxies' (0x769400)
	int Unknown14;										// +0x14
	__int64 ID;											// +0x18 NAME 1: 0 for a new character (Go_Next_Step)
	int Unknown20;										// +0x20
	int Unknown24;										// +0x24
	int Unknown28;										// +0x28
	int Unknown2C;										// +0x2C
	int Unknown30;										// +0x30
	int Unknown34;										// +0x34
	int Unknown38;										// +0x38
	char Unknown3C[0x51];								// +0x3C (the constructor clears its first byte)
};

// An entry of LoginTaskClass +0x350 (0x10C bytes, vtables 0xAEB9D0 / 0xAEB9CC; constructor 0x4BEB50).
// Its destructor is the implicit one (inline in 0x7683F0, out of line at 0x53BB50).
class LoginUnknown350Class : public LoginPacketDataClass, public LoginInterfaceAEADC8Class	// NAME 1: neutral (D10)
{
public:
	LoginUnknown350Class();								// 0x4BEB50
	void Unknown_4C1460(const char *name, int unknown);	// 0x4C1460 (the fast start's name 'Fast' + the user name, 1)
	virtual void Write(LinearBufferClass *buffer);		// +0x00 (vtable 0xAEB9D0)
	virtual void Read(LinearBufferClass *buffer);		// +0x04
	virtual int Get_Size();								// +0x08
	virtual void Unknown_00(void *visitor);				// second table +0x00 (0xAEB9CC)

	int Unknown08;										// +0x08
	char Unknown0C;										// +0x0C
	char Unknown0D;										// +0x0D
	int Unknown10;										// +0x10
	int Unknown14;										// +0x14
	int Unknown18;										// +0x18
	int Unknown1C;										// +0x1C
	char Unknown20;									// +0x20
	char Unknown21;									// +0x21
	char Unknown22;									// +0x22
	char Unknown23;									// +0x23
	char Unknown24;									// +0x24
	char Unknown25;									// +0x25
	char Unknown26;									// +0x26
	char Unknown27;									// +0x27
	char Unknown28;									// +0x28
	char Unknown29;									// +0x29
	char Unknown2A;									// +0x2A
	char Unknown2B;									// +0x2B
	char Unknown2C;									// +0x2C
	char Unknown2D;									// +0x2D
	Vector3 Unknown30;									// +0x30 (Write: 12 bytes)
	Vector3 Unknown3C;									// +0x3C (Write: 12 bytes)
	Vector3 Unknown48;									// +0x48 (Write: 12 bytes)
	Vector3 Unknown54;									// +0x54 (Write: 12 bytes)
	Vector3 Unknown60;									// +0x60 (Write: 12 bytes)
	Vector3 Unknown6C;									// +0x6C (Write: 12 bytes)
	Vector3 Unknown78;									// +0x78 (Write: 12 bytes)
	Vector3 Unknown84;									// +0x84 (Write: 12 bytes)
	Vector3 Unknown90;									// +0x90 (Write: 12 bytes)
	int Unknown9C;										// +0x9C
	int UnknownA0;										// +0xA0
	int UnknownA4;										// +0xA4
	int UnknownA8;										// +0xA8
	int UnknownAC[5];									// +0xAC
	int UnknownC0[5];									// +0xC0
	char UnknownD4[0x14];								// +0xD4
	char UnknownE8[0x14];								// +0xE8
	std::string UnknownFC;								// +0xFC
};

// The serialisable name (vtable 0xB04160: Write 0x78E0E0 writes its 0x80 bytes; 0x84 bytes)
class LoginNameDataClass : public LoginPacketDataClass	// NAME 1: neutral (D10)
{
public:
	LoginNameDataClass() { Name[0] = 0; }
	virtual void Write(LinearBufferClass *buffer);		// +0x00 0x78E0E0
	virtual void Read(LinearBufferClass *buffer);		// +0x04 0x78E110
	virtual int Get_Size();								// +0x08 0x78E140

	char Name[0x80];									// +0x04 NAME 1: 0x80 bytes on the wire
};

// What 0x7683F0 and 0x768CB0 make on the stack (constructor 0x78CEE0: the registry key 'SOFTWARE\Westwood
// Studios\Earth and Beyond\Character and Starship Creator'); its destructor is StringClass's (0x7689C0)
class LoginUnknown78CEE0Class	// NAME 1: neutral (D10)
{
public:
	LoginUnknown78CEE0Class();							// 0x78CEE0
	void Unknown_78D480(int slot, LoginCharacterClass *character, LoginUnknown350Class *unknown, LoginUnknown88CClass *unknown2, LoginNameDataClass *name);	// 0x78D480
	void Unknown_78DAF0(int slot);						// 0x78DAF0 (Unknown_50 passes the selected slot)

	StringClass Unknown00;								// +0x00
};

class LoginUnknown0CClass	// NAME 1: neutral (D10): what LoginTaskClass +0x0C points to (inpdsp.cpp)
{
public:
	LoginUnknown0CClass();								// 0x429630 (0x18 bytes)
	bool Unknown_429A20(KernelEventClass *event);		// 0x429A20 (inpdsp.cpp): true when it took the event
	char Unknown00[0x18];
	~LoginUnknown0CClass();								// 0x429800 (inpdsp.cpp)
};

class LoginUnknown18Class	// NAME 1: neutral (D10): what LoginTaskClass +0x18 points to
{
public:
	LoginUnknown18Class(int unknown);					// 0x514FE0 (0x30 bytes)
	~LoginUnknown18Class();								// 0x515030
	void Unknown_515120(int unknown);					// 0x515120 (the constructor passes 1)
	void Unknown_515340(void *unknown);					// 0x515340
	void Unknown_5153B0();								// 0x5153B0
	void Unknown_515400();								// 0x515400
	void Unknown_515440();								// 0x515440: every LoginTaskClass::Run

	char Unknown00[0x24];
	bool Unknown24;										// +0x24
	char Unknown25[0x30 - 0x25];						// +0x25
};

class LoginTaskClass : public TaskClass
{
public:
	LoginTaskClass();									// 0x767240 (no caller: Step 14)
	LoginTaskClass(int mode);							// 0x7675D0  // NAME 1 [mode]: MenuTask passes 1, or 9 (0x5FAE70 and not 0x5FAD50)
	virtual ~LoginTaskClass();							// +0x04 0x7675A0 / 0x767A20
	virtual int Unknown_08() { return (int)this; }		// +0x08 0x767560 (the base's returns 0)
	virtual bool Accept_Focus();						// +0x28 0x767CE0 ('LoginTask: accepting focus.')
	virtual bool Yield_Focus();							// +0x2C 0x767D30 ('LoginTask: yielding focus.')
	virtual void Run();									// +0x30 0x767F50 ('LoginTask::Run')
	virtual int Get_Interval();							// +0x34 0x767D70
	virtual void Pre_Run();								// +0x38 0x767DA0
	virtual void Render();								// +0x3C 0x767EE0
	virtual bool Delete_On_Remove() { return true; }	// +0x44 0x767580
	virtual void Unknown_48(LoginUnknown350Class *a, LoginUnknown88CClass *b, int galaxy, int c);	// +0x48 0x7689E0 (the FAIL_IF 'Galaxies.empty()'): sends the character creation
	virtual bool Unknown_4C();							// +0x4C 0x768B00: the ticket step
	virtual void Unknown_50();							// +0x50 0x7683F0
	virtual void Go_Next_Step();						// +0x54 0x769500 (S)
	virtual void Go_Back_Step();						// +0x58 0x769680 (S)
	virtual void Unknown_5C();							// +0x5C 0x769400
	virtual void Set_Step(int step, bool force);		// +0x60 0x7694D0: not past step 16 unless forced  // NAME 1: what it does
	virtual bool Unknown_64();							// +0x64 0x7698E0
	virtual void Process_Packets();						// +0x68 0x7683A0: the global connection's  // NAME 1: what it does
	virtual void Unknown_6C();							// +0x6C 0x769360
	virtual void Create_Character_View();				// +0x70 0x769D60  // NAME 1: what it does

	bool Is_Registered_Character() { return Characters[Selected].ID != 0 && Characters[Selected].Galaxy_ID >= 0; }	// (the assert '!Is_Registered_Character()')  // NAME 1 [Galaxy_ID]: 0x769400 looks it up in the galaxies
	void Unknown_7692A0();								// 0x7692A0 ('Avatar %d selected.  Ticket request sent.')
	void Unknown_7693E0();								// 0x7693E0: Step 17
	void Set_Username(const char *name);				// 0x7697A0 (ClientTaskClass hands it back)  // NAME 1: what it does
	void Set_Password(const char *password);			// 0x7697E0  // NAME 1: what it does
	void Show_Message(const char *text);				// 0x769820: an 'OK' box, then step 15  // NAME 1: what it does
	void Show_Message(const char *text, const char *button);	// 0x769870  // NAME 1: what it does
	void Close_Global_Connection();						// 0x769CE0  // NAME 1: what it does
	void Delete_Character_View();						// 0x769D20  // NAME 1: what it does
	bool Fetch_EA_Ticket();								// 0x76A7B0 ('AuthLogin': kcom's authFactory / iauth)  // NAME 1: what it does
	void Start_Game();									// 0x769E70 (step 16: 'LoginTask: removing self.', the ClientTaskClass)  // NAME 1: what it does
	void Unknown_76A370();								// 0x76A370 (step 18: the fast start, Unknown108C's states 1-3: 'LoginTask: Request Character List.')
	void Save_Login_Name();								// 0x76AD30: login.ini's LOGIN_DATA Name  // NAME 1: what it does
	void Load_Login();									// 0x76ADD0: login.ini, -LOGIN, -PASSWORD  // NAME 1: what it does
	void Unknown_768CB0(LoginCharacterClass *characters, LoginUnknown350Class *unknown);	// 0x768CB0: the avatar list's five characters (0x90 bytes each) and five 0x10C entries
	void Set_Galaxies(const std::list<LoginGalaxyClass> &galaxies);	// 0x769110 (inlined in the avatar list's handler)  // NAME 1: what it does
	void Handle_Global_Error(GlobalErrorPacketClass *packet);	// 0x76B000 (GlobalErrorPacketClass's handler)  // NAME 1: what it does
	void Handle_Global_Login(Packet_0076Class *packet);	// 0x76B170 (inlined in the packet's Process_Packet: 'GlobalLoginPacketClass recv')  // NAME 1: what it does
	void Send_Referrer(const char *name);				// 0x76B330: opcode 119 (called by 0x55D060: 'Sorry, you cannot refer yourself.')  // NAME 1: what it does
	void Handle_Referrer_Response(Packet_0078Class *packet);	// 0x76B2F0 (inlined in the packet's Process_Packet)  // NAME 1: what it does
	void Kill_Kernel();									// 0x769E40 ('LoginTask: killing kernel and exiting the game')  // NAME 1: what it does

	LoginUnknown0CClass *Unknown000C;					// +0x0C
	CharacterViewClass *CharacterView;					// +0x10 (the FAIL_IFs '!CharacterView')
	bool Unknown0014;									// +0x14: Set_Step sets it
	LoginUnknown18Class *Unknown0018;					// +0x18
	bool Done;											// +0x1C NAME 2: Yield_Focus yields only when it is set
	bool Unknown001D;									// +0x1D: Run's step 8 goes on to step 2 when it is set
	int Unknown0020;									// +0x20
	int Step;											// +0x24 NAME 1: Set_Step's (Go_Next_Step, Go_Back_Step)
	int Previous_Step;									// +0x28 NAME 1: Go_Next_Step / Go_Back_Step keep the step they leave
	ConnectionClass *GlobalConnection;					// +0x2C (the asserts 'GlobalConnection')
	bool Unknown0030;									// +0x30
	int Unknown0034;									// +0x34
	TicketClass Ticket;									// +0x38 NAME 1: handed to ClientTaskClass::Init (step 16)
	LoginCharacterClass Characters[5];					// +0x80 NAME 1: the character list's slots (Selected indexes them)
	LoginUnknown350Class Unknown0350[5];				// +0x350
	LoginUnknown88CClass Unknown088C[5];				// +0x88C
	std::list<LoginGalaxyClass> Galaxies;				// +0xF6C (the FAIL_IF 'Galaxies.empty()')
	unsigned long Galaxy_Address;						// +0xF78 NAME 1: the selected character's galaxy's (0x769400)
	char EATicket[0x1080 - 0xF7C];						// +0xF7C (the assert 'sizeof(EATicket) > strlen(response.Get_Ticket()) + 1')
	int Selected;										// +0x1080 NAME 1: the selected character's slot
	char *Username;										// +0x1084 NAME 1: Set_Username's (nstrdup)
	char *Password;										// +0x1088 NAME 1: Set_Password's (nstrdup)
	int Unknown108C;									// +0x108C: the fast start's state (0x76A370)
	int Error;											// +0x1090 NAME 1: the global error packet's code (0x76B000)
	char Unknown1094[0x1098 - 0x1094];					// +0x1094
	LoginTicketInfoClass Ticket_Info;					// +0x1098 NAME 1: what the ticket packet's handler 0x76B170 fills
	int Unknown1134;									// +0x1134
	bool Unknown1138;									// +0x1138: cleared by referrer results 1 and 2
	char Unknown1139[0x113C - 0x1139];					// +0x1139
	int Unknown113C;									// +0x113C: the login packet's +0xB4 less its +0xB8 (0x76B170)
	int Referrer_Result;								// +0x1140 NAME 1: the referrer response's 'Result='
	char Unknown1144[0x1148 - 0x1144];					// +0x1144
};

#endif
