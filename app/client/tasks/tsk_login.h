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
#include <list>

class ConnectionClass;
class Packet_0078Class;
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

	char Unknown004[0x30];								// +0x04
	CharacterViewUnknown34Class *Unknown034;			// +0x34
	char Unknown038[0x1CD - 0x38];						// +0x38
	bool Unknown1CD;									// +0x1CD
	bool Unknown1CE;									// +0x1CE
	bool Unknown1CF;									// +0x1CF
	bool Unknown1D0;									// +0x1D0
	char Unknown1D1[0x514 - 0x1D1];						// +0x1D1
};

// A character slot of the login's list (0x90 bytes)
struct LoginCharacterClass	// NAME 1: what it holds
{
	int Galaxy_ID;										// +0x00 NAME 1: compared with the galaxies' (0x769400)
	int Unknown04;										// +0x04
	__int64 ID;											// +0x08 NAME 1: 0 for a new character (Go_Next_Step)
	char Unknown10[0x80];								// +0x10
};

// A galaxy of the login's list
struct LoginGalaxyClass	// NAME 1: what it holds
{
	char Unknown00[0x48];								// +0x00
	int ID;												// +0x48 NAME 1: compared with the characters' Galaxy_ID
	unsigned long Address;								// +0x4C NAME 1: Galaxy_Address's source
};

class LoginUnknown18Class	// NAME 1: neutral (D10): what LoginTaskClass +0x18 points to
{
public:
	void Unknown_515340(void *unknown);					// 0x515340
	void Unknown_5153B0();								// 0x5153B0
	void Unknown_515400();								// 0x515400
	void Unknown_515440();								// 0x515440: every LoginTaskClass::Run

	char Unknown00[0x24];
	bool Unknown24;										// +0x24
};

class LoginTaskClass : public TaskClass
{
public:
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
	virtual void Unknown_48(int a, int b, int galaxy, int c);	// +0x48 0x7689E0 (the FAIL_IF 'Galaxies.empty()'): sends the character creation
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
	void Unknown_769E70();								// 0x769E70 (step 16: 'LoginTask: removing self.', the ClientTaskClass)
	void Unknown_76A370();								// 0x76A370 (step 18: 'LoginTask: Request Character List.')
	void Handle_Referrer_Response(Packet_0078Class *packet);	// 0x76B2F0 (inlined in the packet's Process_Packet)  // NAME 1: what it does
	void Kill_Kernel();									// 0x769E40 ('LoginTask: killing kernel and exiting the game')  // NAME 1: what it does

	int Unknown000C;									// +0x0C
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
	char Unknown0031[0x90 - 0x31];						// +0x31
	LoginCharacterClass Characters[26];					// +0x90 NAME 1: the character list's slots (Selected indexes them)
	char Unknown0F30[0xF6C - 0xF30];					// +0xF30
	std::list<LoginGalaxyClass> Galaxies;				// +0xF6C (the FAIL_IF 'Galaxies.empty()')
	unsigned long Galaxy_Address;						// +0xF78 NAME 1: the selected character's galaxy's (0x769400)
	char Unknown0F7C[0x1080 - 0xF7C];					// +0xF7C
	int Selected;										// +0x1080 NAME 1: the selected character's slot
	char *Username;										// +0x1084 NAME 1: Set_Username's (nstrdup)
	char *Password;										// +0x1088 NAME 1: Set_Password's (nstrdup)
	char Unknown108C[0x1138 - 0x108C];					// +0x108C
	bool Unknown1138;									// +0x1138: cleared by referrer results 1 and 2
	char Unknown1139[0x1140 - 0x1139];					// +0x1139
	int Referrer_Result;								// +0x1140 NAME 1: the referrer response's 'Result='
	char Unknown1144[0x1148 - 0x1144];					// +0x1144
};

#endif
