/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Audio\soundlib.h (name assumed from soundlib.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\Audio\soundlib.cpp')
**
**	The sound library: sounds.ini's sound definitions (one section per sound, its files weighted in
**	a "<name>\Files" section) in an SGI hash_map keyed by the upper-case name (libs/sgistl, D87),
**	2D and 3D sounds made from them through WWAudio, the dialog channel (one dialog at a time, the
**	next one queued) and the bridge WW3D's animated sounds play through. The client names the
**	statics _NextDialogDef and _NextDialogName (an assert); the ini keys name the definition's
**	fields; the rest are guesses (D10, D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SOUNDLIB_H  // NAME 3 [soundlib.h]: header name assumed from the client's soundlib.cpp
#define __SOUNDLIB_H

#include <string>
#include <vector>
#include <hash_map.h>

class INIClass;
class Matrix3D;
class Vector3;
class AudibleSoundClass;
class Sound3DClass;
class WWAudioClass;

class SoundDefClass  // NAME 3: from the client's _NextDialogDef (0x68 bytes)
{
public:
	SoundDefClass(INIClass &ini, const char *section);					// 0x43D3E0
	SoundDefClass(const char *filename);									// 0x43DF90: one file, weight 0
	~SoundDefClass();															// 0x43E160

	const char *Pick_Filename();												// 0x43E230: by weight  // NAME 2: what it does
	AudibleSoundClass *Create_Sound(int mode);							// 0x43E360: 1 plays it, 2 plays and releases it  // NAME 2: what it does
	Sound3DClass *Create_3D_Sound(const Matrix3D &tm, int mode, float scale);	// 0x43E530  // NAME 2: what it does
	void Set_Volume(float volume) { Volume = volume; }							// NAME 2: what it does
	void Set_Type(int type) { Type = type; }										// NAME 2: what it does

	std::string Name;																// +0x00: the section name, upper case  // NAME 2: what it holds
	std::vector<std::pair<std::string, int> > Files;					// +0x10: file and weight ("NULL" is "")  // NAME 2: what it holds
	int TotalWeight;																// +0x20  // NAME 2: what it holds
	int LoopCount;																	// +0x24, SoundLoopCount  // NAME 2: the ini key
	float DropOffRadius;															// +0x28, SoundDropOffRadius  // NAME 2: the ini key
	float MaxVolRadius;															// +0x2C, SoundMaxVolRadius  // NAME 2: the ini key
	float Priority;																// +0x30, SoundPriority  // NAME 2: the ini key
	float Volume;																	// +0x34, SoundVolume  // NAME 2: the ini key
	float MinVolumeShift;														// +0x38, SoundMinVolumeShift  // NAME 2: the ini key
	float MaxVolumeShift;														// +0x3C, SoundMaxVolumeShift  // NAME 2: the ini key
	float MinPitchShift;															// +0x40, SoundMinPitchShift  // NAME 2: the ini key
	float MaxPitchShift;															// +0x44, SoundMaxPitchShift  // NAME 2: the ini key
	float MinDopplerShift;														// +0x48, SoundMinDopplerShift  // NAME 2: the ini key
	float MaxDopplerShift;														// +0x4C, SoundMaxDopplerShift  // NAME 2: the ini key
	float DopplerTweak;															// +0x50, SoundDopplerTweak  // NAME 2: the ini key
	int Type;																		// +0x54, SoundType (AudibleSoundClass::SOUND_TYPE)  // NAME 2: the ini key
	int MaxInstances;																// +0x58, SoundMaxInstances  // NAME 2: the ini key
	float Pitch;																	// +0x5C, SoundPitch  // NAME 2: the ini key
	unsigned int SecsBetweenCalls;											// +0x60, SoundSecsBetweenCalls (ticks)  // NAME 2: the ini key
	unsigned int NextCallTime;													// +0x64  // NAME 2: what it holds
};

class SoundLibraryClass  // NAME 3: from the client's soundlib.cpp ('sound library' in the start-up messages)
{
public:
	static void Init();															// 0x43E8D0  // NAME 2: what it does
	static void Init_Definitions();											// 0x43E9A0  // NAME 2: what it does
	static void Shutdown();														// 0x43E9E0  // NAME 2: what it does
	static void Free_Definitions();											// 0x43EA10  // NAME 2: what it does
	static void Reload();														// 0x43EB90  // NAME 2: what it does
	static SoundDefClass *Find(const std::string &name);				// 0x43EBD0  // NAME 2: what it does
	static AudibleSoundClass *Play_2D_Sound(const char *name, int mode);	// 0x43ED30  // NAME 2: what it does
	static Sound3DClass *Play_3D_Sound(const char *name, const Matrix3D &tm, int mode, float scale);	// 0x43EFE0  // NAME 2: what it does
	static AudibleSoundClass *Play_Dialog(const char *name, int mode);	// 0x43F2A0  // NAME 2: what it does
	static AudibleSoundClass *Play_Dialog(SoundDefClass *def, int mode, const char *name);	// 0x43F2D0  // NAME 2: what it does
	static void Update_Dialog();												// 0x43F720  // NAME 2: what it does
	static void Stop_Dialog();													// 0x43F920  // NAME 2: what it does
	static bool Is_Dialog_Playing();											// 0x43F9A0  // NAME 2: what it does
	static void Set_Listener_Position(const Vector3 &pos);				// 0x43F9C0  // NAME 4: SoundSceneClass's
	static Vector3 Get_Listener_Position();								// 0x43F9F0  // NAME 4: SoundSceneClass's
	static void Set_Listener_Transform(const Matrix3D &tm);				// 0x43FA20  // NAME 4: SoundSceneClass's
	static Matrix3D Get_Listener_Transform();								// 0x43FA50  // NAME 4: SoundSceneClass's
	static void Flush_Playlist();												// 0x43FA80  // NAME 4: WWAudioClass's
	static void Update();														// 0x43FAA0: volumes eased to the options, then the frame  // NAME 2: what it does
	static void Apply_Volumes();												// 0x43FD70  // NAME 2: what it does
	static void Load(const char *filename);								// 0x43FE30  // NAME 2: what it does

	static float _DefaultDropOffRadius;										// 0xB6F3FC (1200), SOUND_DEFAULTS' Default_DropOffRadius  // NAME 2: the ini key
	static float _DefaultMaxVolRadius;										// 0xB6F400 (400)  // NAME 2: the ini key
	static float _DefaultPriority;											// 0xB6F404 (0.9)  // NAME 2: the ini key
	static float _DefaultVolume;												// 0xB6F408 (0.8)  // NAME 2: the ini key
	static float _DefaultDialogVolume;										// 0xB6F40C (0.8)  // NAME 2: the ini key
	static int _DefaultMaxInstances;											// 0xB6F410 (4)  // NAME 2: the ini key

	static std::hash_map<std::string, SoundDefClass *> _SoundDefs;	// 0xBDAC48  // NAME 2: what it holds
	static WWAudioClass *_Audio;												// 0xBDAC68  // NAME 2: what it holds
	static AudibleSoundClass *_CurrentDialog;								// 0xBDAC6C  // NAME 2: what it holds
	static SoundDefClass *_NextDialogDef;									// 0xBDAC70 (the client's assert)
	static char *_NextDialogName;												// 0xBDAC74 (the client's assert)
	static bool _PlayUndefined;												// 0xBDAC78: names not in sounds.ini play as files (set by 0x8000C0 / 0x8008B0)  // NAME 2: what it does
};

#endif //__SOUNDLIB_H
