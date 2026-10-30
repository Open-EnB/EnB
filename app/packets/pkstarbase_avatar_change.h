// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkstarbase_avatar_change.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A3600-0x8A4220 rebuilt so far (opcode 157, 158, 159, 160); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSTARBASE_AVATAR_CHANGE_H  // NAME 1 [pkstarbase_avatar_change]: provisional, pk + its first named class's name
#define PKSTARBASE_AVATAR_CHANGE_H

#include "pk.h"

// opcode 157 (0x009D, STARBASE_AVATAR_CHANGE): vtable 0xB0B8F8, constructor 0x8A36B0, 0x28 bytes
class Starbase_Avatar_ChangePacketClass : public PacketClass  // NAME 3: the client's Print text 'Starbase_Avatar_Change {' and the PacketClass convention
{
public:
	Starbase_Avatar_ChangePacketClass(RawPacketClass *raw);	// 0x8A36B0
	virtual RawPacketClass *Build();	// +0x04 0x8A37A0
	virtual bool Process_Packet(void *client);	// +0x08 0x868B50 (clientstub.cpp l.281)
	virtual int Get_Data_Size();	// +0x0C 0x8A38C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A38E0

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	int RoomType;	// +0x10  // NAME 3: the client's Print text 'RoomType='
	float Orient;	// +0x14  // NAME 3: the client's Print text 'Orient='
	float Unknown18;	// +0x18
	float Unknown1C;	// +0x1C
	float Unknown20;	// +0x20
	int ActionFlag;	// +0x24  // NAME 3: the client's Print text 'ActionFlag='
};

// opcode 158 (0x009E, STARBASE_AVATAR_CHANGE): vtable 0xB0B928, constructor 0x8A3A40, 0x28 bytes
class StarbaseAvatarUpdatePacketClass : public PacketClass
{
public:
	StarbaseAvatarUpdatePacketClass(RawPacketClass *raw);	// 0x8A3A40
	virtual RawPacketClass *Build();	// +0x04 0x8A3B30
	virtual bool Process_Packet(void *client);	// +0x08 0x73A9F0 (cpacket.cpp, register-only residue)
	virtual int Get_Data_Size();	// +0x0C 0x8A3C50
	virtual int Print(char *buffer, int len);	// +0x10 0x8A3C70
	virtual int Get_Channel() { return(AvatarID); }	// +0x18 0x8A39D0

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	float Orient;	// +0x10  // NAME 3: the client's Print text 'Orient='
	float Unknown14;	// +0x14
	float Unknown18;	// +0x18
	float Unknown1C;	// +0x1C
	int ActionFlag;	// +0x20  // NAME 3: the client's Print text 'ActionFlag='
	int Room;	// +0x24  // NAME 3: the client's Print text 'Room='
};

// opcode 159 (0x009F, STARBASE_ROOM_CHANGE): vtable 0xB0B958, constructor 0x8A3D60, 0x18 bytes
class Starbase_Room_ChangePacketClass : public PacketClass  // NAME 3: the client's Print text 'Starbase_Room_Change {' and the PacketClass convention
{
public:
	Starbase_Room_ChangePacketClass(int avatar_id, int new_room, int old_room);	// 0x8A3CE0
	Starbase_Room_ChangePacketClass(RawPacketClass *raw);	// 0x8A3D60
	virtual RawPacketClass *Build();	// +0x04 0x8A3E20
	virtual bool Process_Packet(void *client);	// +0x08 0x868B80 (clientstub.cpp l.284)
	virtual int Get_Data_Size();	// +0x0C 0x8A3F00
	virtual int Print(char *buffer, int len);	// +0x10 0x8A3F20

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	int NewRoom;	// +0x10  // NAME 3: the client's Print text 'NewRoom='
	int OldRoom;	// +0x14  // NAME 3: the client's Print text 'OldRoom='
};

// opcode 160 (0x00A0, STARBASE_ROOM_CHANGE): vtable 0xB0B988, constructor 0x8A4020, 0x18 bytes
class StarbaseAvatarRoomUpdatePacketClass : public PacketClass
{
public:
	StarbaseAvatarRoomUpdatePacketClass(int avatar_id, int new_room, int old_room);	// 0x8A3F60
	StarbaseAvatarRoomUpdatePacketClass(RawPacketClass *raw);	// 0x8A4020
	virtual RawPacketClass *Build();	// +0x04 0x8A40E0
	virtual bool Process_Packet(void *client);	// +0x08 0x73AB50 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A41C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A41E0
	virtual int Get_Channel() { return(AvatarID); }	// +0x18 0x8A3FB0

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	int NewRoom;	// +0x10  // NAME 3: the client's Print text 'NewRoom='
	int OldRoom;	// +0x14  // NAME 3: the client's Print text 'OldRoom='
};

#endif
