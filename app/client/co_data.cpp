// Earth & Beyond client (reconstruction): app\client\co_data.cpp. Notes at the end of the file.
#include "co_data.h"
#include "client.h"
#include "radar_data.h"
#include "gutter_data.h"
#include "starmap_data.h"
#include "navigation_data.h"
#include "emote_data.h"

#include "..\shared\unknown_debug.h"




























ClientObjectDataClass::ClientObjectDataClass(ClientObjectClass *co) :
	ClientObject(co)
{
	G_ASSERT(co);
}








ClientClass *ClientObjectDataClass::Get_Client()
{
	FAIL_IF(!ClientObject) {
		return(0);
	}
	return(ClientObject->Client);
}

int ClientObjectDataClass::Unknown_18()
{
	ClientClass *client = Get_Client();
	if (client != 0) {
		return(client->Unknown_18());
	}
	return(0);
}

template<>
DataBinderClass<ClientObjectClass *> &DataBinderClass<ClientObjectClass *>::Instance()
{
	static DataBinderClass<ClientObjectClass *> binder;
	return(binder);
}

// NOT EXACT: shape (the client calls Has, Get and Remove out of line for all five types; ours inlines Has four times)
void ClientObjectDataClass::Remove_All(ClientObjectClass *co)
{
	ClientObjectDataAccessClass<RadarDataClass>::Remove(co);
	ClientObjectDataAccessClass<GutterDataClass>::Remove(co);
	ClientObjectDataAccessClass<StarmapDataClass>::Remove(co);
	ClientObjectDataAccessClass<NavigationDataClass>::Remove(co);
	ClientObjectDataAccessClass<EmoteDataClass>::Remove(co);
}

RadarDataClass *Get_Radar_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<RadarDataClass>::Get(co, create));
}

// NOT EXACT: inline budget (the client calls the value map's find out of line; ours inlines it)
bool Remove_Radar_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<RadarDataClass>::Remove(co));
}

GutterDataClass *Get_Gutter_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<GutterDataClass>::Get(co, create));
}

// NOT EXACT: inline budget (the client calls the value map's find out of line; ours inlines it)
bool Remove_Gutter_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<GutterDataClass>::Remove(co));
}

StarmapDataClass *Get_Starmap_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<StarmapDataClass>::Get(co, create));
}

// NOT EXACT: inline budget (the client calls the value map's find out of line; ours inlines it)
bool Remove_Starmap_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<StarmapDataClass>::Remove(co));
}

NavigationDataClass *Get_Navigation_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<NavigationDataClass>::Get(co, create));
}

// NOT EXACT: inline budget (the client calls the value map's find out of line; ours inlines it)
bool Remove_Navigation_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<NavigationDataClass>::Remove(co));
}

EmoteDataClass *Get_Emote_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<EmoteDataClass>::Get(co, create));
}

// NOT EXACT: inline budget (the client calls the value map's find out of line; ours inlines it)
bool Remove_Emote_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<EmoteDataClass>::Remove(co));
}

/*
**	Notes (reconstruction)
**
**	The client's co_data.obj: 0x79D0F0-0x7A4770. ClientObjectDataClass's constructor (the assert
**	'co', line 42), its inline Get_Client_Object and destructor (vtable 0xB056CC), Get_Client (the
**	FAIL_IF '!ClientObject', line 54) and Unknown_18 (Get_Client inlined), the binder's Instance
**	0x79D250 (a function-local static at 0xBE86E0, guard 0xBE86D8, destructor 0x79D2C0),
**	Remove_All 0x79D390 (co.cpp's object destructor 0x6F2FE0 calls it), then per type (radar,
**	gutter, starmap, navigation, emote) a 0x13-byte Get wrapper and a Remove wrapper, then the
**	instances of data_binder.h and libs/sgistl: the type map hash_map<std::string, void *> keyed by
**	typeid(T *).name() ('.PAVRadarDataClass@@' 0xB9E778, Gutter 0xB9E7A0, Starmap 0xB9E7C8,
**	Navigation 0xB9E7F0, Emote 0xB9E818) and one hash_map<ClientObjectClass *, T *> per type; this
**	file's __stl_prime_list copy is 0xB056D8. The per-type instances have the same bytes for all
**	five types (D54's ambiguous column). Get's new sizes: radar 0x7C (0x7BB1D0), gutter 0x2C
**	(0x7B4F30), starmap 0x3C (0x7BE640), navigation 0x18 (0x7BA8B0), emote 0x48 (0x7E62F0).
**	The blank lines above the constructor keep the asserts on the client's lines (D25).
*/
