// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkinitrenderstate.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A1950-0x8A2500 rebuilt so far (opcode 47, 48, 49, 50); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKINITRENDERSTATE_H  // NAME 1 [pkinitrenderstate]: provisional, pk + its first named class's name
#define PKINITRENDERSTATE_H

#include "pk.h"

// opcode 47 (0x002F, INIT_RENDER_STATE): vtable 0xB0B718, constructor 0x8A1A60, 0x1C bytes
class InitRenderStatePacketClass : public UnknownPacketBaseClass
{
public:
	InitRenderStatePacketClass(int game_id, int render_state_id, int link_id);	// 0x8A1950  // NAME 1 [game_id, render_state_id, link_id]: its fields
	InitRenderStatePacketClass(PacketClass *raw);	// 0x8A1A60
	virtual PacketClass *Encode();	// +0x04 0x8A1B20  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x737610 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A1C00
	virtual int Print(char *buffer, int len);	// +0x10 0x8A1C20
	virtual int Get_Channel() { return(Unknown18); }	// +0x18 0x8A19F0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int RenderStateID;	// +0x10  // NAME 3: the client's Print text 'RenderStateID='
	int LinkID;	// +0x14  // NAME 3: the client's Print text 'LinkID='
	int Unknown18;	// +0x18
};

// opcode 48 (0x0030, ACTIVATE_RENDER_STATE): vtable 0xB0B748, constructor 0x8A1D70, 0x18 bytes
class ActivateRenderStatePacketClass : public UnknownPacketBaseClass
{
public:
	ActivateRenderStatePacketClass(int game_id, int render_state_id);	// 0x8A1C60  // NAME 1 [game_id, render_state_id]: its fields
	ActivateRenderStatePacketClass(PacketClass *raw);	// 0x8A1D70
	virtual PacketClass *Encode();	// +0x04 0x8A1E10  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x7376F0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A1EE0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A1F00
	virtual int Get_Channel() { return(Unknown14); }	// +0x18 0x8A1D00

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int RenderStateID;	// +0x10  // NAME 3: the client's Print text 'RenderStateID='
	int Unknown14;	// +0x14
};

// opcode 49 (0x0031, ACTIVATE_NEXT_RENDER_STATE): vtable 0xB0B778, constructor 0x8A2050, 0x18 bytes
class ActivateNextRenderStateOncePacketClass : public UnknownPacketBaseClass
{
public:
	ActivateNextRenderStateOncePacketClass(int game_id, int render_state_id);	// 0x8A1F40  // NAME 1 [game_id, render_state_id]: its fields
	ActivateNextRenderStateOncePacketClass(PacketClass *raw);	// 0x8A2050
	virtual PacketClass *Encode();	// +0x04 0x8A20F0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x7377C0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A21C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A21E0
	virtual int Get_Channel() { return(Unknown14); }	// +0x18 0x8A1FE0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int RenderStateID;	// +0x10  // NAME 3: the client's Print text 'RenderStateID='
	int Unknown14;	// +0x14
};

// opcode 50 (0x0032, DEACTIVATE_RENDER_STATE): vtable 0xB0B7A8, constructor 0x8A2330, 0x18 bytes
class DeactivateRenderStatePacketClass : public UnknownPacketBaseClass
{
public:
	DeactivateRenderStatePacketClass(int game_id, int render_state_id);	// 0x8A2220  // NAME 1 [game_id, render_state_id]: its fields
	DeactivateRenderStatePacketClass(PacketClass *raw);	// 0x8A2330
	virtual PacketClass *Encode();	// +0x04 0x8A23D0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x737890 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A24A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A24C0
	virtual int Get_Channel() { return(Unknown14); }	// +0x18 0x8A22C0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int RenderStateID;	// +0x10  // NAME 3: the client's Print text 'RenderStateID='
	int Unknown14;	// +0x14
};

#endif
