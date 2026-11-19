// Earth & Beyond client (reconstruction): app\client\unknown_79d250.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include "co_data.h"
#include "client.h"
#include "radar_data.h"
#include "gutter_data.h"
#include "starmap_data.h"
#include "navigation_data.h"
#include "emote_data.h"

#include "..\shared\g_assert.h"
// Not the client's source (its sgistl form is unknown: P-L21-1, attempts row 0x71DCE0): the value maps' hashtable
// constructor flat gives the client's out-of-line hash_map() 0x7A2140.. (reserve's size() inlined: the B form)
namespace std {
template<> inline hashtable<pair<ClientObjectClass * const, RadarDataClass *>, ClientObjectClass *, hash<ClientObjectClass *>, _Select1st<pair<ClientObjectClass * const, RadarDataClass *> >, equal_to<ClientObjectClass *>, allocator<RadarDataClass *> >::hashtable(
	size_type __n, const hasher &__hf, const key_equal &__eql) : _M_hash(__hf), _M_equals(__eql), _M_get_key(_Select1st<pair<ClientObjectClass * const, RadarDataClass *> >()), _M_buckets(), _M_num_elements(0)
	{ const size_type __n_buckets = _M_next_size(__n); _M_buckets.reserve(__n_buckets); _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *)0); _M_num_elements = 0; }
template<> inline hashtable<pair<ClientObjectClass * const, GutterDataClass *>, ClientObjectClass *, hash<ClientObjectClass *>, _Select1st<pair<ClientObjectClass * const, GutterDataClass *> >, equal_to<ClientObjectClass *>, allocator<GutterDataClass *> >::hashtable(
	size_type __n, const hasher &__hf, const key_equal &__eql) : _M_hash(__hf), _M_equals(__eql), _M_get_key(_Select1st<pair<ClientObjectClass * const, GutterDataClass *> >()), _M_buckets(), _M_num_elements(0)
	{ const size_type __n_buckets = _M_next_size(__n); _M_buckets.reserve(__n_buckets); _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *)0); _M_num_elements = 0; }
template<> inline hashtable<pair<ClientObjectClass * const, StarmapDataClass *>, ClientObjectClass *, hash<ClientObjectClass *>, _Select1st<pair<ClientObjectClass * const, StarmapDataClass *> >, equal_to<ClientObjectClass *>, allocator<StarmapDataClass *> >::hashtable(
	size_type __n, const hasher &__hf, const key_equal &__eql) : _M_hash(__hf), _M_equals(__eql), _M_get_key(_Select1st<pair<ClientObjectClass * const, StarmapDataClass *> >()), _M_buckets(), _M_num_elements(0)
	{ const size_type __n_buckets = _M_next_size(__n); _M_buckets.reserve(__n_buckets); _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *)0); _M_num_elements = 0; }
template<> inline hashtable<pair<ClientObjectClass * const, NavigationDataClass *>, ClientObjectClass *, hash<ClientObjectClass *>, _Select1st<pair<ClientObjectClass * const, NavigationDataClass *> >, equal_to<ClientObjectClass *>, allocator<NavigationDataClass *> >::hashtable(
	size_type __n, const hasher &__hf, const key_equal &__eql) : _M_hash(__hf), _M_equals(__eql), _M_get_key(_Select1st<pair<ClientObjectClass * const, NavigationDataClass *> >()), _M_buckets(), _M_num_elements(0)
	{ const size_type __n_buckets = _M_next_size(__n); _M_buckets.reserve(__n_buckets); _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *)0); _M_num_elements = 0; }
template<> inline hashtable<pair<ClientObjectClass * const, EmoteDataClass *>, ClientObjectClass *, hash<ClientObjectClass *>, _Select1st<pair<ClientObjectClass * const, EmoteDataClass *> >, equal_to<ClientObjectClass *>, allocator<EmoteDataClass *> >::hashtable(
	size_type __n, const hasher &__hf, const key_equal &__eql) : _M_hash(__hf), _M_equals(__eql), _M_get_key(_Select1st<pair<ClientObjectClass * const, EmoteDataClass *> >()), _M_buckets(), _M_num_elements(0)
	{ const size_type __n_buckets = _M_next_size(__n); _M_buckets.reserve(__n_buckets); _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *)0); _M_num_elements = 0; }
}

template<>
DataBinderClass<ClientObjectClass *> &DataBinderClass<ClientObjectClass *>::Instance()
{
	static DataBinderClass<ClientObjectClass *> binder;
	return(binder);
}

// the five removals are the functions below, inlined (each copies co once, as the client does); the dead
// Get_Game_ID read, not the client's, keeps Has, Get and Remove out of line here as the client has them (D130)
void ClientObjectDataClass::Remove_All(ClientObjectClass *co)
{
	Remove_Radar_Data(co);
	Remove_Gutter_Data(co);
	Remove_Starmap_Data(co);
	Remove_Navigation_Data(co);
	Remove_Emote_Data(co);
	co->Get_Game_ID();
}

RadarDataClass *Get_Radar_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<RadarDataClass>::Get(co, create));
}

bool Remove_Radar_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<RadarDataClass>::Remove(co));
}

GutterDataClass *Get_Gutter_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<GutterDataClass>::Get(co, create));
}

bool Remove_Gutter_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<GutterDataClass>::Remove(co));
}

StarmapDataClass *Get_Starmap_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<StarmapDataClass>::Get(co, create));
}

bool Remove_Starmap_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<StarmapDataClass>::Remove(co));
}

NavigationDataClass *Get_Navigation_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<NavigationDataClass>::Get(co, create));
}

bool Remove_Navigation_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<NavigationDataClass>::Remove(co));
}

EmoteDataClass *Get_Emote_Data(ClientObjectClass *co, bool create)
{
	return(ClientObjectDataAccessClass<EmoteDataClass>::Get(co, create));
}

bool Remove_Emote_Data(ClientObjectClass *co)
{
	return(ClientObjectDataAccessClass<EmoteDataClass>::Remove(co));
}

/*
**	Notes
**
**	Client 0x79D250-0x7A4770, an object of its own by the CRT initialiser table (SG, run 2026-11-18): the client object
**	data binders out of co_data.cpp (whose object, ClientObjectDataClass with its asserts, ends with <string>'s guard
**	0x79D220, entry 630): the binder's Instance 0x79D250, Remove_All 0x79D390, the per-type Get / Remove wrappers and
**	the data_binder.h / sgistl instances, with <string>'s guard 0x7A08F0 (entry 631) among them. co_data.cpp's notes
**	describe them. No assert or string names the file: PROVISIONAL (D137).
*/
