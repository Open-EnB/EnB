// Earth & Beyond client (reconstruction): app\shared\callback_manager.h, CallbackManager: callbacks kept in time
// order and run when due (callback_manager.cpp, client 0x49B920-0x49D2CF). The header's name follows the .cpp's
// __FILE__ (no string names it; interim placement, D137).
#ifndef CALLBACK_MANAGER_H	// NAME 1 [callback_manager.h]: the .cpp's name (D137)
#define CALLBACK_MANAGER_H

#include <list>
#include <vector>

// A timed callback (0x10 bytes in timer_manager.cpp, 0x4A0E60: the base's vtable 0xAEAA08, the destructor 0x4A0ED0,
// slot +0x04 0x4A0EB0 'return true', slot +0x08 a shared 'return 0')
class CallbackClass	// NAME 1: neutral (D10)
{
public:
	virtual ~CallbackClass() {}
	virtual bool Delete_When_Removed() { return(true); }	// +0x04: true deletes the callback once it is off the list  // NAME 1: what it does
	virtual bool Execute() { return(false); }			// +0x08: CallbackManager::Process runs it when it is due  // NAME 1: what it does
};

class CallbackManager	// NAME 5: the client's profile string 'CallbackManager::Process'
{
public:
	// One callback and when it is due (the list's element, 0x0C bytes)
	struct CallbackEntry	// NAME 1: neutral (D10)
	{
		CallbackEntry(void *owner, unsigned int time, CallbackClass *callback) : Owner(owner), Time(time), Callback(callback) {}

		void *Owner;				// +0x00: what timer_manager.cpp removes them by  // NAME 1: what it holds
		unsigned int Time;			// +0x04: timeGetTime's time it is due  // NAME 1: what it holds
		CallbackClass *Callback;	// +0x08  // NAME 1: what it holds
	};
	typedef std::list<CallbackEntry> CallbackList;	// NAME 1: neutral (D10)

	// The iterators of the walks in progress, shared by every manager (the static 0xBE34E0, 0x10 bytes: its
	// initializer 0x49B920, its atexit destructor 0x49B950 guarded by 0xBE34F0); erasing an element moves every
	// iterator on it to the next one, and a walk that finds its own iterator NULL (its manager was destroyed
	// meanwhile) stops
	class ActiveIteratorsClass	// NAME 1: neutral (D10)
	{
	public:
		// An element is erased: every iterator on it steps to the next (0x49CC60)
		void Erasing(const CallbackList::iterator &position)	// NAME 1: what it does
		{
			for (std::vector<CallbackList::iterator *>::iterator active = Iterators.begin(); active != Iterators.end(); ++active) {
				if (*active != NULL && **active == position) {
					++(**active);
				}
			}
		}

		// A list is destroyed: every iterator at its end is set NULL, so its walk stops (inline in 0x49BA30)
		void Invalidate(const CallbackList::iterator &end)	// NAME 1: what it does
		{
			for (std::vector<CallbackList::iterator *>::iterator active = Iterators.begin(); active != Iterators.end(); ++active) {
				if (*active != NULL && **active == end) {
					*active = NULL;
				}
			}
		}

		// Takes the innermost walk's iterator off; false when it was invalidated meanwhile (0x49CBE0)
		bool Pop()	// NAME 1: what it does
		{
			if (Iterators.empty()) {
				return(false);
			}
			CallbackList::iterator *active = Iterators.back();
			Iterators.pop_back();
			return(active != NULL);
		}

		std::vector<CallbackList::iterator *> Iterators;	// +0x00 (0x10 bytes)  // NAME 1: what it holds
	};
	static ActiveIteratorsClass ActiveIterators;	// 0xBE34E0  // NAME 1: what it holds

	// A position in the list (the assert's 'iter.Iter', 0x49C490)
	struct Iterator	// NAME 2: the assert's variable 'iter' (its member is 'Iter')
	{
		Iterator() {}
		Iterator(CallbackList::const_iterator iter) : Iter(iter) {}	// 0x49C660

		CallbackList::const_iterator Iter;	// NAME 5: the client's assert text 'iter.Iter == Callbacks.end()'
	};

	CallbackManager();						// 0x49B9F0
	~CallbackManager();						// 0x49BA30

	bool Is_Due(unsigned int time) const;									// 0x49BBC0  // NAME 1: what it does
	void Add(void *owner, unsigned int time, CallbackClass *f);				// 0x49BBF0  // NAME 1: what it does
	bool Remove(CallbackClass *f);											// 0x49BCC0  // NAME 1: what it does
	bool Remove(void *owner);												// 0x49BDB0  // NAME 1: what it does
	bool Remove_Last(CallbackClass *f);										// 0x49BE80  // NAME 1: what it does
	bool Remove_Last(void *owner);											// 0x49BF70  // NAME 1: what it does
	bool Remove_All(CallbackClass *f);										// 0x49C040  // NAME 1: what it does
	bool Remove_All(void *owner);											// 0x49C1C0  // NAME 1: what it does
	void Clear();															// 0x49C320  // NAME 1: what it does
	Iterator Remove(const Iterator &iter);									// 0x49C490  // NAME 1: what it does
	bool Process(unsigned int time);										// 0x49C860: the client's 'CallbackManager::Process'

protected:
	void Erase(const CallbackList::iterator &position);					// 0x49C680  // NAME 1: what it does
	bool Erase_Protected(CallbackList::iterator &position);				// 0x49C720  // NAME 1: what it does

	CallbackList Callbacks;	// +0x00 (0x0C bytes)  // NAME 5: the client's assert text 'Callbacks.end()'
};

#endif
