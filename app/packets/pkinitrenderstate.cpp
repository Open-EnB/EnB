// Earth & Beyond client (reconstruction): app\packets\pkinitrenderstate.cpp. Notes at the end of the file.
#include "pkinitrenderstate.h"
#include <stdio.h>


InitRenderStatePacketClass::InitRenderStatePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&RenderStateID, sizeof(RenderStateID));
	raw->Get(&LinkID, sizeof(LinkID));
}

RawPacketClass *InitRenderStatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(47, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&RenderStateID, sizeof(RenderStateID));
	raw->Add(&LinkID, sizeof(LinkID));
	return(raw);
}

int InitRenderStatePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(RenderStateID) + sizeof(LinkID));
}

int InitRenderStatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d RenderStateID=%u, LinkID=%u}", "Initialize_Render_State", GameID, RenderStateID, LinkID));
}

ActivateRenderStatePacketClass::ActivateRenderStatePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&RenderStateID, sizeof(RenderStateID));
}

RawPacketClass *ActivateRenderStatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(48, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&RenderStateID, sizeof(RenderStateID));
	return(raw);
}

int ActivateRenderStatePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(RenderStateID));
}

int ActivateRenderStatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d RenderStateID=%u}", "Activate_Render_State", GameID, RenderStateID));
}

ActivateNextRenderStateOncePacketClass::ActivateNextRenderStateOncePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&RenderStateID, sizeof(RenderStateID));
}

RawPacketClass *ActivateNextRenderStateOncePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(49, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&RenderStateID, sizeof(RenderStateID));
	return(raw);
}

int ActivateNextRenderStateOncePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(RenderStateID));
}

int ActivateNextRenderStateOncePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d RenderStateID=%u}", "Activate_Next_Render_State_Once", GameID, RenderStateID));
}

DeactivateRenderStatePacketClass::DeactivateRenderStatePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&RenderStateID, sizeof(RenderStateID));
}

RawPacketClass *DeactivateRenderStatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(50, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&RenderStateID, sizeof(RenderStateID));
	return(raw);
}

int DeactivateRenderStatePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(RenderStateID));
}

int DeactivateRenderStatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d RenderStateID=%u}", "Deactivate_Render_State", GameID, RenderStateID));
}


/*
**	Reconstruction notes (pkinitrenderstate.cpp)
**
**	* Client 0x8A1950-0x8A2500 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
