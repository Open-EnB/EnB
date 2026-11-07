#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AMBIENTSOUNDMANAGER_H  // NAME 2: ambientsoundmanager.cpp is the client's; header name assumed
#define __AMBIENTSOUNDMANAGER_H

class AmbientSoundManager  // NAME 2: ambientsoundmanager.cpp is the client's; class name assumed
{
public:
	AmbientSoundManager(int num_channels);
	~AmbientSoundManager();
	void Initialize(int num_channels);
	void Reset();
	void Play_Ambient_Sounds(int channel, int fade_out, int time);
	void Stop_Sound(int channel);

	char m_pad[0x2c];  // NAME 1: size from BridgeSoundManager member offset
};

#endif // __AMBIENTSOUNDMANAGER_H
