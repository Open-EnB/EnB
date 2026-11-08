#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_45F390_H
#define UNKNOWN_45F390_H

#include "rs_track.h"

class Sound3DClass;

// The 'sound' render state (vtable 0xAE821C, 0x3C bytes; rs.ini TYPE 'sound'): a message state that plays
// a 3D sound by name while it is active, moved along with the owner's transform; flag 3 keeps it looping.
class Unknown45F390Class : public Unknown45D840Class	// NAME 1: neutral (D10)
{
public:
	Unknown45F390Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *sound, int message);	// 0x45F390  // NAME 1 [sound, message]: rs_list.cpp's keys
	Unknown45F390Class(RenderStateListClass *list, const Unknown45F390Class &source);	// 0x45F4D0  // NAME 1 [list, source]: neutral
	virtual ~Unknown45F390Class();									// 0x45F6C0 (deleting 0x45F4A0)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list);	// +0x04 0x45F5B0
	virtual void Unknown_0C();										// +0x0C 0x45FAA0 (stops the sound)
	virtual bool Unknown_10();										// +0x10 0x45FB10 (starts it: true when it plays)
	virtual void Unknown_14(RsTrackClass *owner);					// +0x14 0x45F7D0
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 0x45F810
	virtual void Unknown_30(RsTrackClass *owner, float seconds);	// +0x30 0x45FC50
	void Unknown_45F890(const char *sound);							// 0x45F890 (a new sound name, started)  // NAME 1 [sound]: the key
	void Unknown_45F960(const char *sound);							// 0x45F960 (a new sound name)  // NAME 1 [sound]: the key
	void Unknown_45FA30();											// 0x45FA30 (stops and releases the sound)  // NAME 1: neutral (D10)
	void Unknown_45FB40();											// 0x45FB40 (plays it from the start)  // NAME 1: neutral (D10)

	char *SoundName;												// +0x30  // NAME 1: rs.ini's SOUND
	Sound3DClass *Sound;											// +0x34  // NAME 2: Play_3D_Sound's
	bool Unknown38;													// +0x38 (in the scene)  // NAME 1: neutral (D10)
};

#endif
