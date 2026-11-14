// Earth & Beyond client (reconstruction): app\client\unknown_723210.h, a client-side robot that sends random
// play packets (turn, tilt, move, fire, target, inventory moves) on a timer queue. No string, assert or RTTI
// names the file (NAME 1: interim, D137, after its first function 0x723210). The object is 0x723210-0x725050:
// its vtables 0xB02C54..0xB02C84, its static 0xBE7CF0. Nothing in the client constructs it (no caller of the
// constructor 0x723680); SClient +0x12BC holds it (Do_Frame calls slot +0x04 and deletes it when that is true).
#ifndef UNKNOWN_723210_H  // NAME 1 [unknown_723210]: interim file name (D137)
#define UNKNOWN_723210_H

#include <queue>

class SClient;
class ClientObjectClass;

// What SClient +0x12BC points to (vtable 0xB02C84: the deleting destructor 0x723AB0, slot +0x04 the shared
// 'return false' 0x9CCD10); sclient.cpp declares it locally
class SClientUnknown12BCClass  // NAME 1: neutral (D10), sclient.cpp's
{
public:
	virtual ~SClientUnknown12BCClass() {}				// 0x723A90 (out of line for the constructor's unwind)
	virtual bool Unknown_04() { return(false); }		// +0x04 (Do_Frame deletes the object when it is true)
};

// One timed action of the robot: slot +0x00 runs it and says whether it stays queued (no virtual destructor:
// the robot frees the actions with a bare operator delete)
class UnknownB02C78ActionClass  // NAME 1: neutral (D10), the robot's
{
public:
	UnknownB02C78ActionClass() : Time(0) {}
	virtual bool Execute(SClient *client) = 0;			// +0x00  // NAME 1 [Execute, client]: neutral (D10)

	unsigned Time;										// +0x04 when it runs next (against ClientClass +0x10BC)  // NAME 1: what it holds
};

// The queue's order: the earliest Time on top
struct UnknownB02C78CompareStruct  // NAME 1: neutral (D10)
{
	bool operator()(UnknownB02C78ActionClass *a, UnknownB02C78ActionClass *b) const { return(a->Time > b->Time); }
};

// The robot (vtable 0xB02C78, 0x1C bytes)
class UnknownB02C78Class : public SClientUnknown12BCClass  // NAME 1: neutral (D10), its vtable's
{
public:
	UnknownB02C78Class(SClient *client);				// 0x723680 (no caller)  // NAME 2 [client]: what the actions get
	virtual ~UnknownB02C78Class();						// 0x723B50
	virtual bool Unknown_04();							// +0x04 0x723C80: every due action run (always false)

	std::priority_queue<UnknownB02C78ActionClass *, std::vector<UnknownB02C78ActionClass *>, UnknownB02C78CompareStruct> Actions;	// +0x04  // NAME 1: what it holds
	SClient *Client;									// +0x18  // NAME 2: the constructor's argument
};

ClientObjectClass *Unknown_723210(SClient *client);		// 0x723210: the main view's target (no caller; inline in 0x724180)
void Unknown_723230(SClient *client, ClientObjectClass *target);	// 0x723230: REQUEST_TARGET for it  // NAME 2 [target]: the packet's
ClientObjectClass *Unknown_7232D0(SClient *client);		// 0x7232D0: a random other object of 0x6F5A10's kind, or NULL
ClientObjectClass *Unknown_7234B0(SClient *client);		// 0x7234B0: a random component, or NULL

#endif
