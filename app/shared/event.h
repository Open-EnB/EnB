// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\event.h (the header's name by convention, D24, from event.cpp).
*/
#ifndef EVENT_H	// NAME 3 [event.h]: by convention from event.cpp (D24)
#define EVENT_H

#include <list>
#include <vector>

class Event;		// NAME 2: what Subject::Send_Event hands each Listener (never dereferenced in event.cpp)
class Subject;		// NAME 2: the client's asserts "Is_Subject(s)", "Data->Subjects"
class Listener;	// NAME 2: the client's asserts "Is_Listener(l)", "Data->Listeners"

struct ListenerData;	// NAME 2: the heap block behind Listener::Data (assert "Data->Subjects.size() != 0")
struct SubjectData;	// NAME 2: the heap block behind Subject::Data (assert "Data->Listeners")

// Receives a Subject's events; remembers the Subjects it is registered with (vtable 0xAEC630)
class Listener	// NAME 2: the client's assert "Is_Listener(l)"
{
public:
	Listener();												// 0x4E81B0
	virtual ~Listener();									// 0x4E8370
	virtual void On_Event(Event *event) = 0;		// NAME 2: what Send_Event calls (vtable slot 1, pure)
	virtual void Remove_Subject(Subject *s);		// 0x4E8590  // NAME 2: what it does (slot 2, called from ~Subject)

	bool Is_Subject(Subject *s) const;				// 0x4E8520
	void Remove_All_Subjects();						// 0x4E84B0  // NAME 2: what it does

	ListenerData *Data;
};

// Sends events to its Listeners; safe against Listeners leaving during Send_Event (vtable 0xAEC640)
class Subject	// NAME 2: the client's assert "Is_Subject(s)"
{
public:
	Subject();												// 0x4E8730
	virtual ~Subject();									// 0x4E8830

	void Add_Listener(Listener *l);					// 0x4E89D0  // NAME 2: what it does
	void Remove_Listener(Listener *l);				// 0x4E8BA0  // NAME 2: what it does
	bool Is_Listener(Listener *l) const;			// 0x4E8DD0
	void Send_Event(Event *event);					// 0x4E8E40  // NAME 2: what it does

	SubjectData *Data;
};

#endif // EVENT_H
