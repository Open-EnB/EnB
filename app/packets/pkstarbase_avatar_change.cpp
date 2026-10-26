// Earth & Beyond client (reconstruction): app\packets\pkstarbase_avatar_change.cpp. Notes at the end of the file.
#include "pkstarbase_avatar_change.h"
#include <stdio.h>


Starbase_Avatar_ChangePacketClass::Starbase_Avatar_ChangePacketClass(RawPacketClass *raw)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
	raw->Get(&RoomType, sizeof(RoomType));
	raw->Get(&Orient, sizeof(Orient));
	raw->Get(&Unknown18, sizeof(Unknown18));
	raw->Get(&Unknown1C, sizeof(Unknown1C));
	raw->Get(&Unknown20, sizeof(Unknown20));
	raw->Get(&ActionFlag, sizeof(ActionFlag));
}

RawPacketClass *Starbase_Avatar_ChangePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(157, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	raw->Add(&RoomType, sizeof(RoomType));
	raw->Add(&Orient, sizeof(Orient));
	raw->Add(&Unknown18, sizeof(Unknown18));
	raw->Add(&Unknown1C, sizeof(Unknown1C));
	raw->Add(&Unknown20, sizeof(Unknown20));
	raw->Add(&ActionFlag, sizeof(ActionFlag));
	return(raw);
}

int Starbase_Avatar_ChangePacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(RoomType) + sizeof(Orient) + sizeof(Unknown18) + sizeof(Unknown1C) + sizeof(Unknown20) + sizeof(ActionFlag));
}

int Starbase_Avatar_ChangePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Starbase_Avatar_Change {AvatarID=%d RoomType=%d Orient=%f Position=(%f,%f,%f) ActionFlag=%d}", AvatarID, RoomType, Orient, Unknown18, Unknown1C, Unknown20, ActionFlag));
}

StarbaseAvatarUpdatePacketClass::StarbaseAvatarUpdatePacketClass(RawPacketClass *raw)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
	raw->Get(&Orient, sizeof(Orient));
	raw->Get(&Unknown14, sizeof(Unknown14));
	raw->Get(&Unknown18, sizeof(Unknown18));
	raw->Get(&Unknown1C, sizeof(Unknown1C));
	raw->Get(&ActionFlag, sizeof(ActionFlag));
	raw->Get(&Room, sizeof(Room));
}

RawPacketClass *StarbaseAvatarUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(158, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	raw->Add(&Orient, sizeof(Orient));
	raw->Add(&Unknown14, sizeof(Unknown14));
	raw->Add(&Unknown18, sizeof(Unknown18));
	raw->Add(&Unknown1C, sizeof(Unknown1C));
	raw->Add(&ActionFlag, sizeof(ActionFlag));
	raw->Add(&Room, sizeof(Room));
	return(raw);
}

int StarbaseAvatarUpdatePacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(Orient) + sizeof(Unknown14) + sizeof(Unknown18) + sizeof(Unknown1C) + sizeof(ActionFlag) + sizeof(Room));
}

int StarbaseAvatarUpdatePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Starbase_Avatar_Change {AvatarID=%d Orient=%f Position=(%f,%f,%f) ActionFlag=%d Room=%d}", AvatarID, Orient, Unknown14, Unknown18, Unknown1C, ActionFlag, Room));
}

Starbase_Room_ChangePacketClass::Starbase_Room_ChangePacketClass(int avatar_id, int new_room, int old_room) : AvatarID(avatar_id), NewRoom(new_room), OldRoom(old_room)
{
}

Starbase_Room_ChangePacketClass::Starbase_Room_ChangePacketClass(RawPacketClass *raw)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
	raw->Get(&NewRoom, sizeof(NewRoom));
	raw->Get(&OldRoom, sizeof(OldRoom));
}

RawPacketClass *Starbase_Room_ChangePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(159, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	raw->Add(&NewRoom, sizeof(NewRoom));
	raw->Add(&OldRoom, sizeof(OldRoom));
	return(raw);
}

int Starbase_Room_ChangePacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(NewRoom) + sizeof(OldRoom));
}

int Starbase_Room_ChangePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Starbase_Room_Change {AvatarID=%d NewRoom=%d OldRoom=%d}", AvatarID, NewRoom, OldRoom));
}

StarbaseAvatarRoomUpdatePacketClass::StarbaseAvatarRoomUpdatePacketClass(int avatar_id, int new_room, int old_room) : AvatarID(avatar_id), NewRoom(new_room), OldRoom(old_room)
{
}

StarbaseAvatarRoomUpdatePacketClass::StarbaseAvatarRoomUpdatePacketClass(RawPacketClass *raw)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
	raw->Get(&NewRoom, sizeof(NewRoom));
	raw->Get(&OldRoom, sizeof(OldRoom));
}

RawPacketClass *StarbaseAvatarRoomUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(160, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	raw->Add(&NewRoom, sizeof(NewRoom));
	raw->Add(&OldRoom, sizeof(OldRoom));
	return(raw);
}

int StarbaseAvatarRoomUpdatePacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(NewRoom) + sizeof(OldRoom));
}

int StarbaseAvatarRoomUpdatePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Starbase_Room_Update {AvatarID=%d NewRoom=%d OldRoom=%d}", AvatarID, NewRoom, OldRoom));
}


/*
**	Reconstruction notes (pkstarbase_avatar_change.cpp)
**
**	* Client 0x8A3600-0x8A4220 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
