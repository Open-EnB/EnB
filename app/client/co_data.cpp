// Earth & Beyond client (reconstruction): app\client\co_data.cpp. Notes at the end of the file.
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









ClientObjectDataClass::ClientObjectDataClass(ClientObjectClass *co) :
	ClientObject(co)
{
	G_ASSERT(co);
}

ClientObjectDataClass::~ClientObjectDataClass()
{
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
		return(client->As_Space_Client());
	}
	return(0);
}

/*
**	Notes (reconstruction)
**
**	SG (run 2026-11-18): two objects by the CRT initialiser table: this one ends with <string>'s guard 0x79D220
**	(entry 630) after Unknown_18; the binders from 0x79D250 (guard 0x7A08F0, entry 631) are unknown_79d250.cpp.
**	The client's co_data.obj: 0x79D0F0-0x7A4770. ClientObjectDataClass's constructor (the assert
**	'co', line 42), its inline Peek_Client_Object and destructor (vtable 0xB056CC), Get_Client (the
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
**	The order of the client's instances gives the source's shape: Has, Get and Remove of a type sit
**	together (0x79F860, 0x79F8E0, 0x79F980), before the value maps' members, so one function body
**	names all three first: co_data.h's Remove, which Remove_All reaches through the five Remove
**	functions (inlined, one level deeper than in the functions themselves: Has and Get called
**	there, expanded here).
*/
