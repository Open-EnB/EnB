// Earth & Beyond client (reconstruction): app\shared\callback_manager.cpp, CallbackManager (client 0x49B920-0x49D2CF). Notes at the end.
#define ENABLE_WWPROFILE
#include "callback_manager.h"
#include "g_assert.h"
#include "wwprofile.h"
CallbackManager::ActiveIteratorsClass CallbackManager::ActiveIterators;	// 0xBE34E0 (its initializer 0x49B920, its guarded atexit destructor 0x49B950)
// 0x49B9F0
CallbackManager::CallbackManager()
{
}
// 0x49BA30
CallbackManager::~CallbackManager()
{
	Clear();
	ActiveIterators.Invalidate(Callbacks.end());

















}

// 0x49BBC0
bool CallbackManager::Is_Due(unsigned int time) const
{
	if (Callbacks.size() == 0) {
		return(false);
	}
	return(Callbacks.front().Time <= time);
}

// 0x49BBF0
void CallbackManager::Add(void *owner, unsigned int time, CallbackClass *f)
{
	FAIL_IF(!f) return;
	CallbackList::iterator it;
	for (it = Callbacks.begin(); it != Callbacks.end() && (*it).Time <= time; ++it) {
	}
	Callbacks.insert(it, CallbackEntry(owner, time, f));
}












// 0x49BCC0
bool CallbackManager::Remove(CallbackClass *f)
{
	FAIL_IF(!f) return(false);
	CallbackList::iterator it;
	for (it = Callbacks.begin(); it != Callbacks.end(); ++it) {
		if ((*it).Callback == f) {
			break;
		}
	}
	if (it == Callbacks.end()) {
		return(false);
	}
	Erase(it);
	return(true);
}

// 0x49BDB0
bool CallbackManager::Remove(void *owner)
{
	CallbackList::iterator it;
	for (it = Callbacks.begin(); it != Callbacks.end(); ++it) {
		if ((*it).Owner == owner) {
			break;
		}
	}
	if (it == Callbacks.end()) {
		return(false);
	}
	Erase(it);
	return(true);
}

// 0x49BE80
bool CallbackManager::Remove_Last(CallbackClass *f)
{
	FAIL_IF(!f) return(false);
	CallbackList::reverse_iterator it;
	for (it = Callbacks.rbegin(); it != Callbacks.rend(); ++it) {
		if ((*it).Callback == f) {
			break;
		}
	}
	if (it == Callbacks.rend()) {
		return(false);
	}
	Erase(--it.base());
	return(true);
}

// 0x49BF70
bool CallbackManager::Remove_Last(void *owner)
{
	CallbackList::reverse_iterator it;
	for (it = Callbacks.rbegin(); it != Callbacks.rend(); ++it) {
		if ((*it).Owner == owner) {
			break;
		}
	}
	if (it == Callbacks.rend()) {
		return(false);
	}
	Erase(--it.base());
	return(true);
}











// 0x49C040
bool CallbackManager::Remove_All(CallbackClass *f)
{
	FAIL_IF(!f) return(false);
	bool removed = false;
	CallbackList::iterator it = Callbacks.begin();
	while (it != Callbacks.end()) {
		if ((*it).Callback == f) {
			if (!Erase_Protected(it)) {
				return(true);
			}
			removed = true;
		} else {
			++it;
		}
	}
	return(removed);
}

// 0x49C1C0
bool CallbackManager::Remove_All(void *owner)
{
	bool removed = false;
	CallbackList::iterator it = Callbacks.begin();
	while (it != Callbacks.end()) {
		if ((*it).Owner == owner) {
			if (!Erase_Protected(it)) {
				return(true);
			}
			removed = true;
		} else {
			++it;
		}
	}
	return(removed);
}

// 0x49C320
void CallbackManager::Clear()
{
	CallbackList::iterator it = Callbacks.begin();
	while (it != Callbacks.end()) {
		if (!Erase_Protected(it)) {
			return;
		}
	}
}































// 0x49C490
CallbackManager::Iterator CallbackManager::Remove(const Iterator &iter)
{
	FAIL_IF(iter.Iter == Callbacks.end()) return(Iterator(Callbacks.end()));
	CallbackList::const_iterator first = Callbacks.begin(); CallbackList::iterator it = Callbacks.begin();
	std::advance(it, std::distance(first, iter.Iter));
	if (Erase_Protected(it)) {
		return(Iterator(it));
	}
	return(Iterator());
}

// 0x49C680
void CallbackManager::Erase(const CallbackList::iterator &position)
{
	CallbackClass *callback = (*position).Callback;
	ActiveIterators.Erasing(position);
	Callbacks.erase(position);
	if (callback->Delete_When_Removed()) {
		delete callback;
	}
}

// 0x49C720
bool CallbackManager::Erase_Protected(CallbackList::iterator &position)
{
	CallbackClass *callback = (*position).Callback;
	ActiveIterators.Erasing(position);
	position = Callbacks.erase(position);
	ActiveIterators.Iterators.push_back(&position);
	if (callback->Delete_When_Removed()) {
		delete callback;
	}
	return(ActiveIterators.Pop());
}

// 0x49C860
bool CallbackManager::Process(unsigned int time)
{
	WWPROFILE("CallbackManager::Process");
	if (Callbacks.size() == 0) {
		return(false);
	}
	if (Callbacks.front().Time > time) {
		return(false);
	}
	CallbackList::iterator it = Callbacks.begin();
	while (it != Callbacks.end()) {
		if ((*it).Time > time) {
			break;
		}
		CallbackClass *callback = (*it).Callback;
		ActiveIterators.Erasing(it);
		it = Callbacks.erase(it);
		ActiveIterators.Iterators.push_back(&it);
		callback->Execute();
		if (callback->Delete_When_Removed()) {
			delete callback;
		}
		if (!ActiveIterators.Pop()) {
			return(false);
		}
	}
	return(true);
}

/*
**	Reconstruction notes
**
**	client.exe 0x49B920-0x49D2CF, after boolpars.cpp's object (its guarded std::ctype<unsigned short>::id
**	initializer 0x49B8F0 ends it) and before damtype.cpp's (0x49D2D0): the static's initializer 0x49B920 and
**	its destructor 0x49B950 (guard 0xBE34F0), ActiveIteratorsClass's constructor 0x49B9C0 / destructor
**	0x49B980, then the file's functions in source order (0x49B9F0 to Process 0x49C860) and the template and inline
**	copies 0x49CAE0-0x49D2CF. The asserts sit on the client's lines 47, 68, 101, 144 ('!f') and 222
**	('iter.Iter == Callbacks.end()'). Every function is byte-exact (appfile.py match, 2026-11-13).
**
**	Names: CallbackManager, Process and Callbacks are the client's ('CallbackManager::Process' is Process's
**	WWPROFILE label, 'Callbacks' and 'iter.Iter' are assert texts); the rest are neutral (NAME tags in the
**	header). The only user is timer_manager.cpp (0x4A0C40-0x4A0F90, not yet in the tree): its constructor
**	news the manager (0x0C bytes) and calls 0x49B9F0, its destructor 0x49BA30; it adds through Add 0x49BBF0
**	(owner, timeGetTime() + delay, a new 0x10-byte callback), removes through Remove_All(owner) 0x49C1C0 and
**	loops on Process 0x49C860 while it returns true.
**
**	The forms the bytes asked for: the destructor is Clear() then an inline Invalidate (the loop written out,
**	or Erase_Protected called directly, inlines Erasing / Pop where the client calls them); the remove
**	functions test the not-found case first; Process tests size() and the first element's time itself (the
**	inline Is_Due left a setbe); Remove(Iterator) takes a const_iterator copy of begin() before the iterator
**	it advances (one load of begin(), as the client). The static is a plain (non-template) class static:
**	VC6 still guards its atexit destructor.
*/
