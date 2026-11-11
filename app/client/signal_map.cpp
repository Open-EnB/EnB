// Earth & Beyond client (reconstruction): SignalMap (callbacks by sender).
// File PROVISIONAL (D137: no __FILE__; client 0x4F9350-0x4F9BF0, before split_string.cpp; its template instances are
// SignalList's, emitted there).
#define __SIGNAL_MEMBERS	// unknown_signalbase.h's data members and SignalList
#include "unknown_signalbase.h"

#ifndef ENABLE_WWPROFILE
#define ENABLE_WWPROFILE
#endif
#include "wwprofile.h"


// @0x4F9350
SignalMap::SignalMap()
{
}


// @0x4F9530
SignalMap::~SignalMap()
{
	for (SignalHashMap::iterator entry = Map.begin(); entry != Map.end(); ++entry) {
		Delete_All((*entry).second);
	}
	Map.clear();
}


// @0x4F9740
void SignalMap::connect(void *sender, SignalBaseClass::CallbackClass *callback)
{
	WWPROFILE("SignalMap::connect");
	Map[(unsigned int)sender].push_back(callback);
}


// @0x4F98D0
bool SignalMap::disconnect(void *sender, const SignalBaseClass::CallbackClass &callback)
{
	WWPROFILE("SignalMap::disconnect");
	SignalHashMap::iterator entry = Map.find((unsigned int)sender);
	if (entry == Map.end()) {
		return(false);
	}
	CallbackList &list = (*entry).second;
	bool removed = Remove_From(list, callback);
	if (list.empty()) {
		Invalidate(list.end());
		Map.erase((unsigned int)sender);
	}
	return(removed);
}


// @0x4F9AD0
void SignalMap::emit(unsigned int sender, const void *event, const type_info &sender_type, const type_info &event_type)
{
	WWPROFILE("SignalMap::emit");
	SignalHashMap::iterator entry = Map.find(sender);
	if (entry == Map.end()) {
		return;
	}
	Emit_To((*entry).second, (void *)sender, event, sender_type, event_type);
}
