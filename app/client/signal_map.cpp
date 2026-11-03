// Earth & Beyond client (reconstruction): SignalMap and SignalList (the UI gadget event dispatcher).
// File PROVISIONAL (D137: no __FILE__; client 0x4F7E00-0x4F9D00).
#include "unknown_signalbase.h"
#include <hash_map.h>
#include <list>
#include <vector>

#ifndef ENABLE_WWPROFILE
#define ENABLE_WWPROFILE
#endif
#include "wwprofile.h"

class SignalList  // NAME 5: the client's profile strings 'SignalList::Add' / 'SignalList::remove' / 'SignalList::emit'
{
public:
	SignalList();
	~SignalList();
	void Add(SignalBaseClass::CallbackClass *callback);
	bool remove(SignalBaseClass::CallbackClass *callback);
	void emit(void *sender, const void *event, const type_info &sender_type, const type_info &event_type);

	char Unknown00[0x28];  // NAME 1: same layout as SignalMap
};

typedef std::list<SignalBaseClass::CallbackClass *> CallbackList;  // NAME 1: neutral (D10)
typedef std::hash_map<unsigned int, CallbackList> SignalHashMap;  // NAME 1: neutral (D10)

struct SignalMapData  // NAME 1: neutral (D10)
{
	std::vector<void *> ActiveIterators;
	SignalHashMap Map;
};


// @0x4F7EF0
// NOT EXACT: functional (shape)
SignalMap &Unknown_4F7EF0()
{
	static SignalMap s_map;
	return(s_map);
}


// @0x4F7F60
// NOT EXACT: functional (shape)
SignalList &Unknown_4F7F60()
{
	static SignalList s_list;
	return(s_list);
}


// @0x4F8000
// NOT EXACT: functional (shape)
SignalList::SignalList()
{
	SignalMapData *data = (SignalMapData *)Unknown00;
	new (&data->ActiveIterators) std::vector<void *>();
	new (&data->Map) SignalHashMap();
}


// @0x4F81E0
// NOT EXACT: functional (shape)
SignalList::~SignalList()
{
	SignalMapData *data = (SignalMapData *)Unknown00;
	data->Map.~SignalHashMap();
	data->ActiveIterators.~vector();
}


// @0x4F84C0
// NOT EXACT: functional (shape)
void SignalList::Add(SignalBaseClass::CallbackClass *callback)
{
	WWPROFILE("SignalList::Add");
	if (!callback) return;
	SignalMapData *data = (SignalMapData *)Unknown00;
	data->Map[(unsigned int)&callback->Event_Type()].push_back(callback);
}


// @0x4F8650
// NOT EXACT: functional (shape)
bool SignalList::remove(SignalBaseClass::CallbackClass *callback)
{
	WWPROFILE("SignalList::remove");
	if (!callback) return(false);
	SignalMapData *data = (SignalMapData *)Unknown00;
	SignalHashMap::iterator it = data->Map.find((unsigned int)&callback->Event_Type());
	if (it == data->Map.end()) {
		return(false);
	}
	CallbackList &list = it->second;
	for (CallbackList::iterator lit = list.begin(); lit != list.end(); ++lit) {
		if (callback->Is_Equal(**lit)) {
			delete *lit;
			list.erase(lit);
			if (list.empty()) {
				data->Map.erase(it);
			}
			return(true);
		}
	}
	return(false);
}


// @0x4F8820
// NOT EXACT: functional (shape)
void SignalList::emit(void *sender, const void *event, const type_info &sender_type, const type_info &event_type)
{
	WWPROFILE("SignalList::emit");
	SignalMapData *data = (SignalMapData *)Unknown00;
	SignalHashMap::iterator it = data->Map.find((unsigned int)&event_type);
	if (it == data->Map.end()) {
		return;
	}
	CallbackList &list = it->second;
	std::vector<SignalBaseClass::CallbackClass *> callbacks;
	for (CallbackList::iterator lit = list.begin(); lit != list.end(); ++lit) {
		callbacks.push_back(*lit);
	}
	for (size_t i = 0; i < callbacks.size(); ++i) {
		SignalBaseClass::CallbackClass *cb = callbacks[i];
		if (cb && cb->Event_Type() == event_type) {
			cb->Call(sender, (void *)event);
		}
	}
}


// @0x4F9350
// NOT EXACT: functional (shape)
SignalMap::SignalMap()
{
	SignalMapData *data = (SignalMapData *)Unknown00;
	new (&data->ActiveIterators) std::vector<void *>();
	new (&data->Map) SignalHashMap();
}


// @0x4F9530
// NOT EXACT: functional (shape)
SignalMap::~SignalMap()
{
	SignalMapData *data = (SignalMapData *)Unknown00;
	data->Map.~SignalHashMap();
	data->ActiveIterators.~vector();
}


// @0x4F9740
// NOT EXACT: functional (shape)
void SignalMap::connect(void *sender, SignalBaseClass::CallbackClass *callback)
{
	WWPROFILE("SignalMap::connect");
	if (!callback) return;
	SignalMapData *data = (SignalMapData *)Unknown00;
	data->Map[(unsigned int)sender].push_back(callback);
}


// @0x4F98D0
// NOT EXACT: functional (shape)
void SignalMap::disconnect(void *sender, const SignalBaseClass::CallbackClass &callback)
{
	WWPROFILE("SignalMap::disconnect");
	SignalMapData *data = (SignalMapData *)Unknown00;
	SignalHashMap::iterator it = data->Map.find((unsigned int)sender);
	if (it == data->Map.end()) {
		return;
	}
	CallbackList &list = it->second;
	for (CallbackList::iterator lit = list.begin(); lit != list.end(); ++lit) {
		if (callback.Is_Equal(**lit)) {
			delete *lit;
			list.erase(lit);
			break;
		}
	}
	if (list.empty()) {
		data->Map.erase(it);
	}
}


// @0x4F9AD0
// NOT EXACT: functional (shape)
void SignalMap::emit(unsigned int sender, const void *event, const type_info &sender_type, const type_info &event_type)
{
	WWPROFILE("SignalMap::emit");
	SignalMapData *data = (SignalMapData *)Unknown00;
	SignalHashMap::iterator it = data->Map.find(sender);
	if (it == data->Map.end()) {
		return;
	}
	CallbackList &list = it->second;
	std::vector<SignalBaseClass::CallbackClass *> callbacks;
	for (CallbackList::iterator lit = list.begin(); lit != list.end(); ++lit) {
		callbacks.push_back(*lit);
	}
	for (size_t i = 0; i < callbacks.size(); ++i) {
		SignalBaseClass::CallbackClass *cb = callbacks[i];
		if (cb && cb->Event_Type() == event_type) {
			cb->Call((void *)sender, (void *)event);
		}
	}
}
