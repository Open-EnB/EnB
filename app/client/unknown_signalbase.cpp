// Earth & Beyond client (reconstruction): SignalBaseClass, what SignalList and SignalMap share: the callback lists'
// own code (delete, emit, remove) and the iterators of the emits in progress, so that callbacks may connect and
// disconnect while their signal is emitted.
// File PROVISIONAL (D137: no __FILE__; client 0x4F7930-0x4F7FF0, an object of its own (SG: with the two
// function-local static dispatchers 0x4F7EF0 / 0x4F7F60 and <string>'s guard 0x4F7FD0, CRT entry 135): SignalList's and SignalMap's
// constructors call its constructor 0x4F7930 out of line, and its one template instance 0x4F7C50 follows it).
#include "unknown_signalbase.h"


// @0x4F7930
SignalBaseClass::SignalBaseClass()
{
}


// @0x4F7960
SignalBaseClass::~SignalBaseClass()
{
}


// @0x4F79A0
void SignalBaseClass::Delete_All(CallbackList &list)
{
	for (CallbackList::iterator position = list.begin(); position != list.end(); ++position) {
		Invalidate(position);
		delete *position;
	}
	list.clear();
}


// @0x4F7A60
void SignalBaseClass::Emit_To(CallbackList &list, void *sender, const void *event, const type_info &sender_type, const type_info &event_type)
{
	CallbackList::iterator position = list.begin();
	while (position != list.end()) {
		CallbackClass *callback = *position;
		if (callback->Event_Type() == event_type) {
			++position;
			ActiveIterators.push_back(&position);
			callback->Call(sender, (void *)event);
			if (!Pop_Iterator()) {
				return;
			}
		} else {
			++position;
		}
	}
}


// @0x4F7B80
bool SignalBaseClass::Remove_From(CallbackList &list, const CallbackClass &callback)
{
	for (CallbackList::iterator position = list.begin(); position != list.end(); ++position) {
		if (callback.Is_Equal(**position)) {
			for (IteratorVector::iterator active = ActiveIterators.begin(); active != ActiveIterators.end(); ++active) {
				if (*active != NULL && **active == position) {
					++**active;
				}
			}
			delete *position;
			list.erase(position);
			return(true);
		}
	}
	return(false);
}


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
