// Earth & Beyond client (reconstruction): app\shared\observer.h, a subject's list of observers (the client's
// app\shared\observer.cpp per its assert; this header's name is provisional, NAME 1). Laid out from
// 0x49E890 / 0x49E950 / 0x49EAB0 and UIMeshClass, which derives from it at +0x04 (UM1, UP1, 2026-11-07).
#ifndef OBSERVER_H  // NAME 1 [observer.h]: provisional header name for observer.cpp
#define OBSERVER_H

#include "vector.h"

class SubjectClass;

// What a subject calls (no table of its own in the client: every slot is a subclass's)
class ObserverClass	// NAME 1: neutral, after the file (D10)
{
public:
	virtual void Unknown_00(SubjectClass *subject, void *event) = 0;	// +0x00: an event (0x49EAB0)  // NAME 1 [subject, event]: neutral
	virtual void Unknown_04(SubjectClass *subject) = 0;				// +0x04: the subject going away; it detaches itself (0x49EA60's assert)  // NAME 1 [subject]: neutral
};

// 0x18 bytes, no table of its own
class SubjectClass	// NAME 1: neutral, after the file (D10)
{
public:
	SubjectClass(int size);								// 0x49E890 (UIMeshClass passes 2)  // NAME 1 [size]: what the vector's constructor does with it
	~SubjectClass();									// 0x49E950
	void Unknown_49EA60();								// 0x49EA60: every observer told to detach, last first (inlined in the destructor)
	void Unknown_49EAB0(void *event);					// 0x49EAB0: event to each observer, last first (ui_proc.cpp's 0x5DCCE0 passes { process, event, arg })  // NAME 1 [event]: neutral

	DynamicVectorClass<ObserverClass *> Observers;		// +0x00  // NAME 3: the assert's 'Observers.Count()' (l.102)
};

#endif
