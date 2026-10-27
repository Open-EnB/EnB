// Earth & Beyond client (reconstruction): app\packets\pkrecustomize.cpp. Notes at the end of the file.
#include "pkrecustomize.h"
#include <stdio.h>

RecustomizeAvatarStartPacketClass::RecustomizeAvatarStartPacketClass(RawPacketClass *raw) :
	Unknown48(0)
{
	raw->Get(&Cost);
	raw->Get(Unknown48);
}

RecustomizeAvatarStartPacketClass::~RecustomizeAvatarStartPacketClass()
{
}

RecustomizeAvatarStartPacketClass::RecustomizeAvatarStartPacketClass(int unknown48, const RecustomizationCostClass &cost) :
	Unknown48(unknown48)
{
	Cost = cost;
}

RawPacketClass *RecustomizeAvatarStartPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(131, Get_Data_Size());
	raw->Add(Cost);
	raw->Add(Unknown48);
	return(raw);
}

int RecustomizeAvatarStartPacketClass::Get_Data_Size()
{
	return(((Unknown_PacketDataClass &)Cost).Get_Size() + 4);
}

int RecustomizeAvatarStartPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Recustomize Avatar Start"));
}


/*
**	Reconstruction notes (pkrecustomize.cpp)
**
**	* Client 0x89FB50-0x8A14D0 (0x89FB20 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x8A14D0): RECUSTOMIZE_SHIP_START 129, RECUSTOMIZE_SHIP_DONE 130, RECUSTOMIZE_AVATAR_START 131,
**	  RECUSTOMIZE_AVATAR_DONE 132 and RECUSTOMIZE_AVATAR_UPDATE 133 in that order. 131 written by hand
**	  (builder P7, 2026-10-27) over recustomization_cost.cpp's wire object; 129 / 130 (a ShipCustDescClass)
**	  and 132 / 133 (an AvatarDescClass) are still declared in pk_classes.h.
**	* The file name is provisional (no assert names it).
*/
