#include "g_assert.h"
#include "event.h"
#include <list>
#include <vector>
#include <algorithm>

struct ListenerData	// NAME 2: the heap block behind Listener::Data (assert "Data->Subjects.size() != 0")
{
	std::list<Subject *> Subjects;
};

struct SubjectData	// NAME 2: the heap block behind Subject::Data (assert "find_result == Data->Listeners.end()")
{
	std::list<Listener *> Listeners;
};






Listener::Listener() :
	Data(0)
{
	Data = new ListenerData;

	G_ASSERT(Data);
}


Listener::~Listener()
{

	FAIL_IF(!Data) {
		return;
	}
	FAIL_IF(Data->Subjects.size() != 0) {
		Remove_All_Subjects();
	}
	if (Data) {
		delete Data;
	}
	Data = NULL;
}



void Listener::Remove_All_Subjects()
{

	FAIL_IF(!Data) {
		return;
	}
	std::list<Subject *>::iterator it = Data->Subjects.begin();
	while (it != Data->Subjects.end()) {
		(*it++)->Remove_Listener(this);
	}
}




bool Listener::Is_Subject(Subject *s) const
{

	FAIL_IF(!Data) {
		return false;
	}
	return std::find(Data->Subjects.begin(), Data->Subjects.end(), s) != Data->Subjects.end();
}




void Listener::Remove_Subject(Subject *s)
{

	FAIL_IF(!Data || !s) {
		return;
	}
	FAIL_IF(!Is_Subject(s)) {
		return;
	}
	Data->Subjects.remove(s);
}

static std::vector<std::list<Listener *>::iterator *> ActiveIterators;	// NAME 1: the iterators of the Send_Event calls in progress (0xBE5610), fixed up by Remove_Listener and ~Subject














Subject::Subject() :
	Data(0)
{
	Data = new SubjectData;

	G_ASSERT(Data);
}


Subject::~Subject()
{

	FAIL_IF(!Data) {
		return;
	}
	std::list<Listener *> &listeners = Data->Listeners;
	for (std::list<Listener *>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
		for (std::vector<std::list<Listener *>::iterator *>::iterator ait = ActiveIterators.begin(); ait != ActiveIterators.end(); ++ait) {
			if (*ait && **ait == it) {
				++(**ait);
			}
		}
		(*it)->Remove_Subject(this);
	}
	listeners.erase(listeners.begin(), listeners.end());
	std::list<Listener *>::iterator end = Data->Listeners.end();
	for (std::vector<std::list<Listener *>::iterator *>::iterator ait = ActiveIterators.begin(); ait != ActiveIterators.end(); ++ait) {
		if (*ait && **ait == end) {
			*ait = NULL;
		}
	}
	if (Data) {
		delete Data;
	}
	Data = NULL;
}

void Subject::Add_Listener(Listener *l)
{
	FAIL_IF(!Data || !l) {
		return;
	}
	FAIL_IF(Is_Listener(l)) {
		return;
	}
	FAIL_IF(l->Is_Subject(this)) {
		return;
	}
	Data->Listeners.push_back(l);
	l->Data->Subjects.push_back(this);
}






void Subject::Remove_Listener(Listener *l)
{
	FAIL_IF(!Data || !l) {
		return;
	}
	FAIL_IF(!Is_Listener(l)) {
		return;
	}
	FAIL_IF(!l->Is_Subject(this)) {
		return;
	}
	l->Data->Subjects.remove(this);




	std::list<Listener *>::iterator find_result = std::find(Data->Listeners.begin(), Data->Listeners.end(), l);
	FAIL_IF(find_result == Data->Listeners.end()) {
		return;
	}
	for (std::vector<std::list<Listener *>::iterator *>::iterator ait = ActiveIterators.begin(); ait != ActiveIterators.end(); ++ait) {
		if (*ait && **ait == find_result) {
			++(**ait);
		}
	}
	Data->Listeners.erase(find_result);
}

bool Subject::Is_Listener(Listener *l) const
{
	FAIL_IF(!Data || !l) {
		return false;
	}
	return std::find(Data->Listeners.begin(), Data->Listeners.end(), l) != Data->Listeners.end();
}


static inline bool Pop_Iterator() { if (ActiveIterators.empty()) return false; std::list<Listener *>::iterator *p = ActiveIterators.back(); ActiveIterators.pop_back(); return p != NULL; }	// NAME 1: pops Send_Event's iterator; false once ~Subject cleared it (inlined into 0x4E8E40)


void Subject::Send_Event(Event *event)
{
	FAIL_IF(!Data) {
		return;
	}
	for (std::list<Listener *>::iterator it = Data->Listeners.begin(); it != Data->Listeners.end(); ) {
		Listener *l = *it;
		++it;
		ActiveIterators.push_back(&it);
		l->On_Event(event);
		if (!Pop_Iterator()) {
			break;
		}
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4E81B0-0x4E8E40, with its template helpers following to 0x4E9110 (g_assert.cpp follows).
**	Listener (vtable 0xAEC630: scalar deleting destructor, On_Event (pure), Remove_Subject) 0x4E81B0-0x4E8590,
**	the static ActiveIterators (0xBE5610, constructed by $E3 / destroyed by 0x4E8700 after Remove_Subject) and
**	Subject (vtable 0xAEC640: scalar deleting destructor) 0x4E8730-0x4E8E40. Asserts on the client's lines
**	27, 34, 37, 51, 66, 78, 81, 107, 114, 141, 144, 147, 161, 164, 167, 176, 189, 201 (D25).
**
**	Forms the client's code needs: ~Subject takes a reference to Data->Listeners for the notify loop and the
**	range erase (the single erase 0x4E8F90 stays out of line) and reads end() once before the clean-up loop;
**	Remove_Listener uses list::remove(this) (erase 0x4E8FE0 and iterator++ 0x4E8F70 out of line) and an
**	inlined std::find; Send_Event's pop is an inline helper returning bool (its setne / test al, al).
**	The classes' names follow the client's asserts (Is_Subject, Is_Listener, Data->Subjects, Data->Listeners);
**	Event, On_Event, Send_Event, Add_Listener, Remove_Listener, Remove_Subject and Remove_All_Subjects are ours.
*/
