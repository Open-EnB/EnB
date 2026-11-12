#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AMBIENTSOUNDMANAGER_H  // NAME 2: ambientsoundmanager.cpp is the client's; header name assumed
#define __AMBIENTSOUNDMANAGER_H

#include <stddef.h>
#include <string>
#include <vector>

class AudibleSoundClass;
class Sound3DClass;
class RenderObjClass;
class Matrix3D;

// The ambient sounds of one place (the bridge, a sector, a starbase), kept per channel: a channel is one set of
// sounds (a race's bridge, an elevation band of a sector, a starbase room), and one channel plays at a time.
// Each channel has sounds that loop while it plays and sounds started at random intervals, both either plain
// (2-D) or attached to a bone of a model (3-D). 0x2C bytes (BridgeSoundManager's first own member is at +0x2C).
// The class and its member names LoopingSounds, RandomSounds, NumChannels, CurrentChannel, and the sounds'
// RenderObj and Sound, are the client's (ambientsoundmanager.cpp's FAIL_IF / G_ASSERT texts, l.84-577).
class AmbientSoundManager  // NAME 2: ambientsoundmanager.cpp is the client's; class name assumed
{
public:
	// a sound started at random intervals (0x28 bytes, made at 0x434C60; the destructor 0x4346C0 is the compiler's)
	struct RandomSoundStruct  // NAME 2: what it holds (the channel array's name is the client's RandomSounds)
	{
		RandomSoundStruct() : MinDelay(0), MaxDelay(0), NextTime(0), MinPan(0.5f), MaxPan(0.5f), Positional(true) {}	// inline at 0x434C60

		std::string Name;	// +0x00: the sound library's name  // NAME 2: what Play_2D_Sound gets
		int MinDelay;		// +0x10: milliseconds  // NAME 2: the ini's MinDelay%d
		int MaxDelay;		// +0x14  // NAME 2: the ini's MaxDelay%d
		int NextTime;		// +0x18: when it plays next (the time passed in + a random delay)  // NAME 2: what it holds
		float MinPan;		// +0x1C: 0.5 until set  // NAME 2: the ini's MinPan%d
		float MaxPan;		// +0x20  // NAME 2: the ini's MaxPan%d
		bool Positional;	// +0x24: played with Play_3D_Sound at the transform 0x435610 is given  // NAME 2: what it selects
	};

	// a looping sound attached to a bone of a model (0x1C bytes, made at 0x4348C0)
	struct LoopingSound3DStruct  // NAME 2: what it holds
	{
		LoopingSound3DStruct() : Sound(NULL), RenderObj(NULL), BoneIndex(0) {}	// inline at 0x4348C0
		~LoopingSound3DStruct();	// 0x436120

		std::string Name;			// +0x00  // NAME 2: what Play_3D_Sound gets
		Sound3DClass *Sound;		// +0x10: the playing sound, a reference held (G_ASSERT 'sound->Sound == NULL')
		RenderObjClass *RenderObj;	// +0x14: a reference held (G_ASSERT 'sound->RenderObj')
		int BoneIndex;				// +0x18: RenderObj's Get_Bone_Index(bone_name)  // NAME 2: what it holds
	};

	// a sound started at random intervals at a bone of a model (0x24 bytes, made at 0x434FD0)
	struct RandomSound3DStruct  // NAME 2: what it holds
	{
		RandomSound3DStruct() : RenderObj(NULL), BoneIndex(0), MinDelay(0), MaxDelay(0), NextTime(0) {}	// inline at 0x434FD0
		~RandomSound3DStruct();		// 0x436200

		RenderObjClass *RenderObj;	// +0x00: a reference held (FAIL_IF 'sound->RenderObj == NULL', l.439)
		int BoneIndex;				// +0x04  // NAME 2: what it holds
		std::string Name;			// +0x08  // NAME 2: what Play_3D_Sound gets
		int MinDelay;				// +0x18  // NAME 2: as RandomSoundStruct's
		int MaxDelay;				// +0x1C  // NAME 2: as RandomSoundStruct's
		int NextTime;				// +0x20  // NAME 2: as RandomSoundStruct's
	};

	AmbientSoundManager(int num_channels);	// 0x433D00 (0: Initialize is called later)
	~AmbientSoundManager();					// 0x4342E0
	void Initialize(int num_channels);		// 0x433D90 (FAIL_IF 'num_channels <= 0', l.86): the four channel arrays allocated
	void Reset();							// 0x434360: every sound stopped, the channel arrays freed
	void Add_Looping_Sound(int channel, const char *name);	// 0x434710 (l.203)  // NAME 2: what it does
	void Add_Looping_Sound(int channel, const char *name, RenderObjClass *rendobj, const char *bone_name);	// 0x4348C0 (l.227; rendobj, bone_name are the client's)  // NAME 2: what it does
	void Add_Random_Sound(int channel, const char *name, int min_delay, int max_delay, float min_pan, float max_pan, bool positional);	// 0x434C60 (l.266)  // NAME 2: what it does
	void Add_Random_Sound(int channel, const char *name, int min_delay, int max_delay, RenderObjClass *rendobj, const char *bone_name);	// 0x434FD0 (l.305)  // NAME 2: what it does
	void Play_Ambient_Sounds(int channel, int fade_out, int time);	// 0x435390 (l.346): the playing sounds faded out, channel's looping sounds started, its random sounds scheduled from time
	void Update(int time, const Matrix3D &tm);	// 0x435610 (l.403): the random sounds that are due started, the positional ones at tm  // NAME 2: what it does
	void Update(int time);					// 0x435AE0 (l.449): the same for a place with 2-D random sounds only  // NAME 2: what it does
	void Update_Playing_Sounds(int time);	// 0x435E50: finished sounds dropped; a pending channel started once the others are silent  // NAME 2: what it does
	void Set_Pending_Channel(int channel);	// 0x435F80: channel started by the next update, after the playing sounds end  // NAME 2: what it does
	void Stop_Sounds(int fade_out);			// 0x435FA0 (l.577): Stop, or Fade_Out when fade_out is not 0  // NAME 2: what it does

	std::vector<AudibleSoundClass *> PlayingSounds;				// +0x00: a reference held to each  // NAME 2: what it holds
	std::vector<std::string> *LoopingSounds;					// +0x10: new[NumChannels]; names of 2-D looping sounds
	std::vector<RandomSoundStruct *> *RandomSounds;				// +0x14: new[NumChannels]
	std::vector<LoopingSound3DStruct *> *LoopingSounds3D;		// +0x18: new[NumChannels]  // NAME 2: as LoopingSounds, attached to a model
	std::vector<RandomSound3DStruct *> *RandomSounds3D;			// +0x1C: new[NumChannels]  // NAME 2: as RandomSounds, attached to a model
	int NumChannels;											// +0x20
	int CurrentChannel;											// +0x24: -1 none
	bool ChannelPending;										// +0x28: CurrentChannel waits for the playing sounds to end  // NAME 2: what it does
};

#endif // __AMBIENTSOUNDMANAGER_H
