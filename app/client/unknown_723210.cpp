// Earth & Beyond client (reconstruction): app\client\unknown_723210.cpp (NAME 1: interim, D137), the client-side
// robot of unknown_723210.h. Notes at the end.
#include <stdlib.h>
#include "unknown_723210.h"
#include "sclient.h"
#include "co.h"
#include "random.h"
#include "..\packets\pkrequesttarget.h"
#include "..\packets\pkturn.h"
#include "..\packets\pktilt.h"
#include "..\packets\pkmove.h"
#include "..\packets\pkfire.h"
#include "..\packets\pkaction.h"
#include "..\packets\pkinventorymove.h"
#include <list>

static int Unknown_BE7CF0;	// 0xBE7CF0: robots made (every fifth gets the attack action)  // NAME 1: neutral (D10)

// Turn left, right or level, 4-10 s (vtable 0xB02C74, 0xC bytes)
class UnknownB02C74Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C74Class() : Turn(0) {}
	virtual bool Execute(SClient *client);				// 0x723E00
	int Turn;											// +0x08 -1, 0 or 1  // NAME 2: TurnPacketClass's sign
};

// Tilt up, down or level, 6-10 s (vtable 0xB02C70, 0xC bytes)
class UnknownB02C70Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C70Class() : Tilt(0) {}
	virtual bool Execute(SClient *client);				// 0x723F30
	int Tilt;											// +0x08 -1, 0 or 1  // NAME 2: TiltPacketClass's sign
};

// Move, 8-10 s (vtable 0xB02C6C)
class UnknownB02C6CClass : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C6CClass() {}
	virtual bool Execute(SClient *client);				// 0x724060
};

// Target a random other object when there is no target, 20 s (vtable 0xB02C60)
class UnknownB02C60Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C60Class() {}
	virtual bool Execute(SClient *client);				// 0x724180
};

// Fire everything at the target, 6-10 s (vtable 0xB02C5C)
class UnknownB02C5CClass : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C5CClass() {}
	virtual bool Execute(SClient *client);				// 0x7241F0
};

// Move an inventory item, 5-10 s (vtable 0xB02C58)
class UnknownB02C58Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C58Class() {}
	virtual bool Execute(SClient *client);				// 0x7242E0
};

// Fire one of slots 3-5 at the target, 10-15 s (vtable 0xB02C54)
class UnknownB02C54Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C54Class() {}
	virtual bool Execute(SClient *client);				// 0x7243C0
};

// Target a component and attack it once, 10-20 s (vtable 0xB02C64, 0xC bytes)
class UnknownB02C64Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C64Class() {}
	virtual bool Execute(SClient *client);				// 0x724490
	bool Attacked;										// +0x08 (set once ACTION 1 is sent)  // NAME 2: what it holds
};

// Swap two inventory slots, 15-20 s (vtable 0xB02C68)
class UnknownB02C68Class : public UnknownB02C78ActionClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C68Class() {}
	virtual bool Execute(SClient *client);				// 0x7245C0
};

ClientObjectClass *Unknown_723210(SClient *client)
{
	return(client->MainView->Unknown_5C());
}

void Unknown_723230(SClient *client, ClientObjectClass *target)
{
	client->Send_Packet(&RequestTargetPacketClass(client->Get_Avatar_ID(), target->Get_Game_ID()));
}

ClientObjectClass *Unknown_7232D0(SClient *client)
{
	std::list<ClientObjectClass *> objects;
	HashNodeClass<ClientObjectClass *> *scan = client->ObjectList.First_Valid();
	while (scan && scan->Next()) {
		ClientObjectClass *object = scan->Get_Record();
		if (object->Unknown_6F5A10() && object->Get_Game_ID() != client->Get_Avatar_ID()) {
			objects.push_back(object);
		}
		scan = scan->Next();
	}
	if (objects.size() == 0) {
		return(NULL);
	}
	int pick = rand() % (int)objects.size();
	for (std::list<ClientObjectClass *>::iterator it = objects.begin(); it != objects.end(); ++it) {
		if (pick == 0) {
			return(*it);
		}
		pick--;
	}
	return(NULL);
}

ClientObjectClass *Unknown_7234B0(SClient *client)
{
	std::list<ClientObjectClass *> objects;
	HashNodeClass<ClientObjectClass *> *scan = client->ObjectList.First_Valid();
	while (scan && scan->Next()) {
		ClientObjectClass *object = scan->Get_Record();
		if (object->Is_Component()) {
			objects.push_back(object);
		}
		scan = scan->Next();
	}
	if (objects.size() == 0) {
		return(NULL);
	}
	int pick = rand() % (int)objects.size();
	for (std::list<ClientObjectClass *>::iterator it = objects.begin(); it != objects.end(); ++it) {
		if (pick == 0) {
			return(*it);
		}
		pick--;
	}
	return(NULL);
}

UnknownB02C78Class::UnknownB02C78Class(SClient *client) :
	Client(client)
{
	Actions.push(new UnknownB02C74Class);
	Actions.push(new UnknownB02C70Class);
	Actions.push(new UnknownB02C6CClass);
	Actions.push(new UnknownB02C68Class);
	if ((Unknown_BE7CF0++ % 5) == 0) {
		Actions.push(new UnknownB02C64Class);
	} else {
		Actions.push(new UnknownB02C60Class);
		Actions.push(new UnknownB02C5CClass);
		Actions.push(new UnknownB02C58Class);
		Actions.push(new UnknownB02C54Class);
	}
}

UnknownB02C78Class::~UnknownB02C78Class()
{
	while (!Actions.empty()) {
		delete Actions.top();
		Actions.pop();
	}
}

bool UnknownB02C78Class::Unknown_04()
{
	UnknownB02C78ActionClass *action = Actions.top();
	while (action->Time < Client->Unknown10BC) {
		Actions.pop();
		if (action->Execute(Client)) {
			Actions.push(action);
		} else {
			delete action;
		}
		action = Actions.top();
	}
	return(false);
}

bool UnknownB02C74Class::Execute(SClient *client)
{
	int turn;
	if (Turn == 0) {
		turn = (rand() % 3) - 1;
	} else {
		turn = 0;
	}
	if (turn != Turn) {
		float intensity = (float)((rand() % 1001) * turn) * 0.001f;
		client->Send_Packet(&TurnPacketClass(client->Get_Avatar_ID(), intensity));
		client->Unknown_745B90((float)turn);
		Turn = turn;
	}
	Time = RandomClass()(4000, 10000) + client->Unknown10BC;
	return(true);
}

bool UnknownB02C70Class::Execute(SClient *client)
{
	int tilt;
	if (Tilt == 0) {
		tilt = (rand() % 3) - 1;
	} else {
		tilt = 0;
	}
	if (tilt != Tilt) {
		float intensity = (float)((rand() % 1001) * tilt) * 0.001f;
		client->Send_Packet(&TiltPacketClass(client->Get_Avatar_ID(), intensity));
		client->Unknown_745C70((float)tilt);
		Tilt = tilt;
	}
	Time = RandomClass()(6000, 10000) + client->Unknown10BC;
	return(true);
}

bool UnknownB02C6CClass::Execute(SClient *client)
{
	Time = RandomClass()(8000, 10000) + client->Unknown10BC;
	int move = (rand() % 12) - 1;
	if (move == -1) {
		client->Send_Packet(&MovePacketClass(client->Get_Avatar_ID(), 1));
		client->Unknown_745E40(-1, false);
	} else if (move == 0) {
		return(true);
	} else if (move == 1) {
		return(true);
	} else {
		client->Send_Packet(&MovePacketClass(client->Get_Avatar_ID(), 0));
		client->Unknown_745E40(1, false);
	}
	return(true);
}

bool UnknownB02C60Class::Execute(SClient *client)
{
	if (!Unknown_723210(client)) {
		ClientObjectClass *target = Unknown_7232D0(client);
		if (target) {
			Unknown_723230(client, target);
		}
	}
	Time = client->Unknown10BC + 20000;
	return(true);
}

bool UnknownB02C5CClass::Execute(SClient *client)
{
	if (client->MainView->Unknown_5C()) {
		client->Send_Packet(&FirePacketClass(client->Get_Avatar_ID(), -1));
	}
	Time = RandomClass()(6000, 10000) + client->Unknown10BC;
	return(true);
}

bool UnknownB02C58Class::Execute(SClient *client)
{
	Time = RandomClass()(5000, 10000) + client->Unknown10BC;
	if ((rand() % 5) == 0) {
		int slot = rand() % 3;
		client->Send_Packet(&InventoryMovePacketClass(client->Get_Avatar_ID(), (InventoryEnum)1, slot, (InventoryEnum)2, -1, -1));
	}
	return(true);
}

bool UnknownB02C54Class::Execute(SClient *client)
{
	Time = RandomClass()(10000, 15000) + client->Unknown10BC;
	if (client->MainView->Unknown_5C()) {
		int slot = (rand() % 3) + 3;
		client->Send_Packet(&FirePacketClass(client->Get_Avatar_ID(), slot));
	}
	return(true);
}

bool UnknownB02C64Class::Execute(SClient *client)
{
	Time = RandomClass()(10000, 20000) + client->Unknown10BC;
	ClientObjectClass *target = client->MainView->Unknown_5C();
	if (target) {
		if (Attacked) {
			return(true);
		}
	} else {
		target = Unknown_7234B0(client);
		if (!target) {
			return(true);
		}
		client->Send_Packet(&RequestTargetPacketClass(client->Get_Avatar_ID(), target->Get_Game_ID()));
	}
	client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 1, target->Get_Game_ID(), 0));
	Attacked = true;
	return(true);
}

bool UnknownB02C68Class::Execute(SClient *client)
{
	Time = RandomClass()(15000, 20000) + client->Unknown10BC;
	int bank = (rand() % 2) * 3;
	int from = rand() % 3;
	int to = rand() % 3;
	if (to == from) {
		to = (to + 1) % 3;
	}
	client->Send_Packet(&InventoryMovePacketClass(client->Get_Avatar_ID(), (InventoryEnum)1, from + bank, (InventoryEnum)1, to + bank, -1));
	return(true);
}

// Reconstruction notes (O22, run 2026-11-13b).
// The object is 0x7231C0-0x725050: the PermStringInit initialisers 0x7231C0 / 0x7231F0 that open it (a header's),
// 0x723210-0x7245C0 above in this order, the inline packet destructors 0x7232B0 / 0x7242C0 (out of line for the
// unwind), the std::list / std::vector / heap instances 0x7246D0-0x724E70, the <string> ctype guard 0x724EE0 and the
// two heap instances the compiler emits after it (_Push_heap 0x724F10, _Adjust_heap 0x724F90); client.cpp's
// UnknownB02C90Class follows at 0x725050. No string, assert or RTTI in the range names the file.
// Forms the bytes fixed: the action constructors zero Time and their own field through initialiser lists (the
// vtable store comes last); rand() results the packet takes are locals (the id is loaded after the call); the move
// action's rand() % 12 - 1 against -1, 0, 1; the attack action tests the target first; the swap action keeps
// (rand() % 2) * 3; the object pickers return early on an empty list and take rand() % (int)size().
