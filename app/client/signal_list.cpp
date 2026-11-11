// Earth & Beyond client (reconstruction): SignalList (callbacks by the event's type) and the two function-local
// static dispatchers.
// File PROVISIONAL (D137: no __FILE__; client 0x4F7EF0-0x4F9350, an object of its own: its template instances
// 0x4F8910-0x4F9320 follow SignalList::emit and precede SignalMap's constructor).
#define __SIGNAL_MEMBERS	// unknown_signalbase.h's data members and SignalList
#include "unknown_signalbase.h"

#ifndef ENABLE_WWPROFILE
#define ENABLE_WWPROFILE
#endif
#include "wwprofile.h"


// @0x4F7EF0
SignalMap &Unknown_4F7EF0()
{
	static SignalMap s_map;
	return(s_map);
}


// @0x4F7F60
SignalList &Unknown_4F7F60()
{
	static SignalList s_list;
	return(s_list);
}


// @0x4F8000
SignalList::SignalList()
{
}


// @0x4F81E0
SignalList::~SignalList()
{
	for (SignalHashMap::iterator entry = Map.begin(); entry != Map.end(); ++entry) {
		Delete_All((*entry).second);
	}
	Map.clear();
}


// @0x4F84C0
void SignalList::Add(CallbackClass *callback)
{
	WWPROFILE("SignalList::Add");
	Map[(unsigned int)&callback->Event_Type()].push_back(callback);
}


// @0x4F8650
bool SignalList::remove(const CallbackClass &callback)
{
	WWPROFILE("SignalList::remove");
	SignalHashMap::iterator entry = Map.find((unsigned int)&callback.Event_Type());
	if (entry == Map.end()) {
		return(false);
	}
	CallbackList &list = (*entry).second;
	bool removed = Remove_From(list, callback);
	if (list.empty()) {
		Invalidate(list.end());
		Map.erase(entry);
	}
	return(removed);
}


// @0x4F8820
void SignalList::emit(void *sender, const void *event, const type_info &sender_type, const type_info &event_type)
{
	WWPROFILE("SignalList::emit");
	SignalHashMap::iterator entry = Map.find((unsigned int)&event_type);
	if (entry == Map.end()) {
		return;
	}
	Emit_To((*entry).second, sender, event, sender_type, event_type);
}
