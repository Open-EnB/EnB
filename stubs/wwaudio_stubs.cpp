/*
**	Earth & Beyond client (reconstruction): link-time stubs for wwaudio.lib (D44, docs/milestones.md 3.2)
*/

#include "stub_log.h"
#include "listenerhandle.h"

// Declared in listenerhandle.h (overriding SoundHandleClass::Initialize), defined in no EA copy:
// ListenerHandleClass's vtable needs it. The listener has no sample to set up.
void ListenerHandleClass::Initialize(SoundBufferClass *buffer)
{
	STUB_CALLED("ListenerHandleClass::Initialize");
}
