// Earth & Beyond client (reconstruction)
// clienttest: wwaudio.lib's WWAudio.obj is linked for real now, and SClient's 0x7511A0 calls WWAudioClass::Flush_Cache
// on the instance (WWAudioClass::_theInstance, null here: constructing one starts Miles). A one-function mirror of the
// class (same name and member, so the same symbol ?Flush_Cache@WWAudioClass@@QAEXXZ) gives the real function's address,
// and main() jumps it to a trace line, as clienttest.cpp's Hook_Get_Ticks does for the clock.
#include <windows.h>
#include <string.h>
#include "clienttest.h"

class WWAudioClass
{
public:
	void Flush_Cache(void);
};

static void Flush_Cache_Hook()
{
	Trace("stub WWAudioClass::Flush_Cache");
}

void Hook_Audio_Functions()
{
	void (WWAudioClass::*member)(void) = &WWAudioClass::Flush_Cache;
	unsigned char *at;
	memcpy(&at, &member, 4);
	DWORD old;
	VirtualProtect(at, 5, PAGE_EXECUTE_READWRITE, &old);
	at[0] = 0xE9;
	*(int *)(at + 1) = (int)Flush_Cache_Hook - (int)(at + 5);
	VirtualProtect(at, 5, old, &old);
}
