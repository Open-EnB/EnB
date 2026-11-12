// Earth & Beyond client (reconstruction): app\Audio\ambientsoundmanager.cpp. Notes at the end of the file.
#include "ambientsoundmanager.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwaudio\Sound3D.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\wwlib\random.h"
#include "..\..\wwmath\wwmath.h"
#include "soundlib.h"

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's

// @0x433D00
AmbientSoundManagerClass::AmbientSoundManagerClass(int num_channels) :
	LoopingSounds(NULL),
	RandomSounds(NULL),
	LoopingSounds3D(NULL),
	RandomSounds3D(NULL),
	NumChannels(num_channels),
	CurrentChannel(-1),
	ChannelPending(false)
{
	if (num_channels != 0) {
		Initialize(num_channels);
	}
}





















































// @0x433D90
void AmbientSoundManagerClass::Initialize(int num_channels)
{
	FAIL_IF(LoopingSounds != NULL) return;
	FAIL_IF(RandomSounds != NULL) return;
	FAIL_IF(num_channels <= 0) return;
	NumChannels = num_channels;
	LoopingSounds = new std::vector<std::string>[num_channels];
	RandomSounds = new std::vector<RandomSoundStruct *>[num_channels];
	LoopingSounds3D = new std::vector<LoopingSound3DStruct *>[num_channels];
	RandomSounds3D = new std::vector<RandomSound3DStruct *>[num_channels];
}

// @0x4342E0
AmbientSoundManagerClass::~AmbientSoundManagerClass()
{
	Reset();
}

// @0x434360
void AmbientSoundManagerClass::Reset()
{
	Stop_Sounds(0);
	int channel;
	if (LoopingSounds != NULL) {
		for (channel = 0; channel < NumChannels; channel++) {
			LoopingSounds[channel].clear();
		}
		delete [] LoopingSounds;
		LoopingSounds = NULL;
	}
	if (RandomSounds != NULL) {
		for (channel = 0; channel < NumChannels; channel++) {
			for (std::vector<RandomSoundStruct *>::iterator itor = RandomSounds[channel].begin(); itor != RandomSounds[channel].end(); itor++) {
				delete *itor;
			}
			RandomSounds[channel].clear();
		}
		delete [] RandomSounds;
		RandomSounds = NULL;
	}
	if (LoopingSounds3D != NULL) {
		for (channel = 0; channel < NumChannels; channel++) {
			for (std::vector<LoopingSound3DStruct *>::iterator itor = LoopingSounds3D[channel].begin(); itor != LoopingSounds3D[channel].end(); itor++) {
				delete *itor;
			}
			LoopingSounds3D[channel].clear();
		}
		delete [] LoopingSounds3D;
		LoopingSounds3D = NULL;
	}
	if (RandomSounds3D != NULL) {
		for (channel = 0; channel < NumChannels; channel++) {
			for (std::vector<RandomSound3DStruct *>::iterator itor = RandomSounds3D[channel].begin(); itor != RandomSounds3D[channel].end(); itor++) {
				delete *itor;
			}
			RandomSounds3D[channel].clear();
		}
		delete [] RandomSounds3D;
		RandomSounds3D = NULL;
	}
	NumChannels = 0;
	CurrentChannel = -1;
}























































// @0x434710
void AmbientSoundManagerClass::Add_Looping_Sound(int channel, const char *name)
{
	FAIL_IF(channel >= NumChannels) return;
	FAIL_IF(channel < 0) return;
	LoopingSounds[channel].push_back(std::string(name));
}

















// @0x4348C0
void AmbientSoundManagerClass::Add_Looping_Sound(int channel, const char *name, RenderObjClass *rendobj, const char *bone_name)
{
	FAIL_IF(channel >= NumChannels) return;
	FAIL_IF(channel < 0) return;
	FAIL_IF(rendobj == NULL) return;
	FAIL_IF(bone_name == NULL) return;
	int bone_index = rendobj->Get_Bone_Index(bone_name);
	if (bone_index != 0) {
		LoopingSound3DStruct *sound = new LoopingSound3DStruct;
		sound->Name = name;
		sound->RenderObj = rendobj;
		sound->RenderObj->Add_Ref();
		sound->BoneIndex = bone_index;
		LoopingSounds3D[channel].push_back(sound);
	}
}






















// @0x434C60
void AmbientSoundManagerClass::Add_Random_Sound(int channel, const char *name, int min_delay, int max_delay, float min_pan, float max_pan, bool positional)
{
	FAIL_IF(channel >= NumChannels) return;
	FAIL_IF(channel < 0) return;
	if (min_pan > max_pan) {
		float pan = min_pan;
		min_pan = max_pan;
		max_pan = pan;
	}
	RandomSoundStruct *sound = new RandomSoundStruct;
	sound->MinDelay = min_delay;
	sound->MaxDelay = max_delay;
	sound->Name = name;
	sound->NextTime = 0;
	sound->MinPan = min_pan;
	sound->MaxPan = max_pan;
	sound->Positional = positional;
	RandomSounds[channel].push_back(sound);
}



















// @0x434FD0
void AmbientSoundManagerClass::Add_Random_Sound(int channel, const char *name, int min_delay, int max_delay, RenderObjClass *rendobj, const char *bone_name)
{
	FAIL_IF(channel >= NumChannels) return;
	FAIL_IF(channel < 0) return;
	FAIL_IF(rendobj == NULL) return;
	FAIL_IF(bone_name == NULL) return;
	int bone_index = rendobj->Get_Bone_Index(bone_name);
	if (bone_index != 0) {
		RandomSound3DStruct *sound = new RandomSound3DStruct;
		sound->Name = name;
		sound->MinDelay = min_delay;
		sound->MaxDelay = max_delay;
		sound->NextTime = 0;
		sound->RenderObj = rendobj;
		sound->RenderObj->Add_Ref();
		sound->BoneIndex = bone_index;
		RandomSounds3D[channel].push_back(sound);
	}
}





















// @0x435390
void AmbientSoundManagerClass::Play_Ambient_Sounds(int channel, int fade_out, int time)
{
	FAIL_IF(channel >= NumChannels) return;
	FAIL_IF(channel < 0) return;
	CurrentChannel = channel;
	Stop_Sounds(fade_out);

	for (std::vector<std::string>::iterator looping = LoopingSounds[CurrentChannel].begin(); looping != LoopingSounds[CurrentChannel].end(); looping++) {
		AudibleSoundClass *playing = SoundLibraryClass::Play_2D_Sound(looping->c_str(), 1);
		PlayingSounds.push_back(playing);
	}

	for (std::vector<LoopingSound3DStruct *>::iterator looping3d = LoopingSounds3D[CurrentChannel].begin(); looping3d != LoopingSounds3D[CurrentChannel].end(); looping3d++) {
		LoopingSound3DStruct *sound = *looping3d;




		G_ASSERT(sound->RenderObj);
		Sound3DClass *playing = SoundLibraryClass::Play_3D_Sound(sound->Name.c_str(), sound->RenderObj->Get_Bone_Transform(sound->BoneIndex), 0, 1.0f);
		if (playing != NULL) {
			playing->Attach_To_Object(sound->RenderObj, sound->BoneIndex);
			playing->Add_To_Scene(true);

			G_ASSERT(sound->Sound == NULL);
			sound->Sound = playing;
		}
	}
	for (std::vector<RandomSoundStruct *>::iterator random = RandomSounds[CurrentChannel].begin(); random != RandomSounds[CurrentChannel].end(); random++) {
		RandomSoundStruct *sound = *random;
		sound->NextTime = Unknown_BDBB84(sound->MinDelay, sound->MaxDelay) + time;
	}
	for (std::vector<RandomSound3DStruct *>::iterator random3d = RandomSounds3D[CurrentChannel].begin(); random3d != RandomSounds3D[CurrentChannel].end(); random3d++) {
		RandomSound3DStruct *sound = *random3d;
		sound->NextTime = Unknown_BDBB84(sound->MinDelay, sound->MaxDelay) + time;
	}
}



















// @0x435610
void AmbientSoundManagerClass::Update(int time, const Matrix3D &tm)
{
	FAIL_IF(CurrentChannel >= NumChannels) return;
	if (CurrentChannel < 0) return;
	Update_Playing_Sounds(time);
	if (ChannelPending) return;

	for (std::vector<RandomSoundStruct *>::iterator random = RandomSounds[CurrentChannel].begin(); random != RandomSounds[CurrentChannel].end(); random++) {
		RandomSoundStruct *sound = *random;



		FAIL_IF(sound == NULL) return;
		if ((unsigned)(time - sound->NextTime) < 0x80000000) {
			if (!sound->Positional) {
				AudibleSoundClass *playing = SoundLibraryClass::Play_2D_Sound(sound->Name.c_str(), 1);
				if (playing != NULL) {
					playing->Set_Pan(WWMath::Random_Float(sound->MinPan, sound->MaxPan));
					PlayingSounds.push_back(playing);
				}
			} else {
				AudibleSoundClass *playing = SoundLibraryClass::Play_3D_Sound(sound->Name.c_str(), tm, 1, 1.0f);
				PlayingSounds.push_back(playing);
			}
			sound->NextTime = Unknown_BDBB84(sound->MinDelay, sound->MaxDelay) + time;
		}
	}

	for (std::vector<RandomSound3DStruct *>::iterator random3d = RandomSounds3D[CurrentChannel].begin(); random3d != RandomSounds3D[CurrentChannel].end(); random3d++) {
		RandomSound3DStruct *sound = *random3d;




		FAIL_IF(sound == NULL) return;
		if ((unsigned)(time - sound->NextTime) < 0x80000000) {


			FAIL_IF(sound->RenderObj == NULL) return;
			Sound3DClass *playing = SoundLibraryClass::Play_3D_Sound(sound->Name.c_str(), sound->RenderObj->Get_Bone_Transform(sound->BoneIndex), 1, 1.0f);
			sound->NextTime = Unknown_BDBB84(sound->MinDelay, sound->MaxDelay) + time;
			PlayingSounds.push_back(playing);
		}
	}
}
// @0x435AE0
void AmbientSoundManagerClass::Update(int time)
{
	FAIL_IF(CurrentChannel >= NumChannels) return;
	if (CurrentChannel < 0) return;
	Update_Playing_Sounds(time);
	if (ChannelPending) return;

	for (std::vector<RandomSoundStruct *>::iterator random = RandomSounds[CurrentChannel].begin(); random != RandomSounds[CurrentChannel].end(); random++) {
		RandomSoundStruct *sound = *random;



		FAIL_IF(sound == NULL) return;
		if ((unsigned)(time - sound->NextTime) < 0x80000000) {
			AudibleSoundClass *playing = SoundLibraryClass::Play_2D_Sound(sound->Name.c_str(), 1);
			if (playing != NULL) {
				playing->Set_Pan(WWMath::Random_Float(sound->MinPan, sound->MaxPan));
				PlayingSounds.push_back(playing);
			}
			sound->NextTime = Unknown_BDBB84(sound->MinDelay, sound->MaxDelay) + time;
		}
	}
}

// @0x435E50
void AmbientSoundManagerClass::Update_Playing_Sounds(int time)
{
	bool silent = true;
	std::vector<AudibleSoundClass *>::iterator itor = PlayingSounds.begin();
	while (itor != PlayingSounds.end()) {
		AudibleSoundClass *sound = *itor;
		if (sound == NULL) {
			PlayingSounds.erase(itor);
		} else if (sound->Get_State() == AudibleSoundClass::STATE_STOPPED) {
			sound->Release_Ref();
			PlayingSounds.erase(itor);
		} else {
			if (sound->Get_Loop_Count() != 0) {
				silent = false;
			}
			itor++;
		}
	}
	if (ChannelPending && silent) {
		ChannelPending = false;
		Play_Ambient_Sounds(CurrentChannel, 0, time);
	}
}

// @0x435F80
void AmbientSoundManagerClass::Set_Pending_Channel(int channel)
{
	ChannelPending = true;
	CurrentChannel = channel;
}

























































// @0x435FA0
void AmbientSoundManagerClass::Stop_Sounds(int fade_out)
{
	for (std::vector<AudibleSoundClass *>::iterator playing = PlayingSounds.begin(); playing != PlayingSounds.end(); playing++) {
		AudibleSoundClass *sound = *playing;
		if (sound != NULL) {
			if (fade_out == 0) {
				sound->Stop(true);
			} else {
				sound->Fade_Out(fade_out);
			}
			sound->Release_Ref();
		}
	}
	PlayingSounds.clear();
	for (int channel = 0; channel < NumChannels; channel++) {
		for (std::vector<LoopingSound3DStruct *>::iterator itor = LoopingSounds3D[channel].begin(); itor != LoopingSounds3D[channel].end(); itor++) {
			LoopingSound3DStruct *sound = *itor;
			FAIL_IF(sound == NULL) continue;
			if (sound->Sound != NULL) {
				if (fade_out == 0) {
					sound->Sound->Stop(true);
				} else {
					sound->Sound->Fade_Out(fade_out);
				}
				sound->Sound->Remove_From_Scene();
				sound->Sound->Release_Ref();
				sound->Sound = NULL;
			}
		}
	}
}

// @0x436120
AmbientSoundManagerClass::LoopingSound3DStruct::~LoopingSound3DStruct()
{
	if (Sound != NULL) {
		Sound->Stop(true);
		Sound->Remove_From_Scene();
		Sound->Release_Ref();
		Sound = NULL;
	}
	if (RenderObj != NULL) {
		RenderObj->Release_Ref();
		RenderObj = NULL;
	}
}

// @0x436200
AmbientSoundManagerClass::RandomSound3DStruct::~RandomSound3DStruct()
{
	if (RenderObj != NULL) {
		RenderObj->Release_Ref();
		RenderObj = NULL;
	}
}

/*
Reconstruction notes:
ambientsoundmanager.cpp holds AmbientSoundManagerClass, the base of the bridge's, a sector's and a starbase's ambient
sounds (bridgesounds.cpp, sectorsound.cpp, starbasesound.cpp). The object is 0x433C80-0x436A00 (the music
player's object ends before it; bridgesounds.cpp's 0x436A60 follows); its vectors' instances lie between its
functions (0x433F90-0x4342B0: the channel arrays' vector deleting destructors and constructors) and after them
(0x4362A0-0x436A00).
*/
