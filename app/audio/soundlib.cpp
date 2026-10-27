// Earth & Beyond client (reconstruction): app\Audio\soundlib.cpp. Notes at the end of the file.
#include <string.h>
#include "soundlib.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"
#include "..\..\wwlib\ffactory.h"
#include "..\..\wwlib\random.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\..\wwmath\wwmath.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwaudio\Sound3D.h"
#include "..\..\wwaudio\SoundScene.h"
#include "..\..\ww3d2\soundlibrarybridge.h"
#include "..\..\ww3d2\animatedsoundmgr.h"

unsigned int Unknown_Get_Ticks();				// 0x4A3B40, the aux-data clock
void Unknown_442650();							// NAME 1 [Unknown_442650]: 0x442650, surfacetypes.cpp's loader
void Unknown_442870();							// NAME 1 [Unknown_442870]: 0x442870, surfacetypes.cpp's release
extern Random2Class Unknown_BDADC8;			// NAME 1 [Unknown_BDADC8]: 0xBDADC8, a global generator outside this file

class Unknown_BE8B38Class						// NAME 1 [Unknown_BE8B38Class]: option.cpp's object at 0xBE8B38 (tsk_game.h)
{
public:
	float Get_Float(const char *name);		// 0x7F9670: the volume options  // NAME 2: what it does
};
extern Unknown_BE8B38Class *Unknown_BE8B38;	// NAME 1 [Unknown_BE8B38]: neutral (D10)

// The client stores members that the tree's wwaudio headers keep protected (E&B's m_Unknown7C,
// m_Unknown80, Sound3DClass's m_Unknown164-16C): these classes make them reachable, no code of their own.
class SoundAccessClass : public AudibleSoundClass	// NAME 2: what it does
{
public:
	using AudibleSoundClass::m_Unknown7C;
	using AudibleSoundClass::m_Unknown80;
};
class Sound3DAccessClass : public Sound3DClass	// NAME 2: what it does
{
public:
	using Sound3DClass::m_Unknown7C;
	using Sound3DClass::m_Unknown80;
	void Set_Min_Doppler_Pitch(float pitch) { m_Unknown164 = pitch; }
	void Set_Max_Doppler_Pitch(float pitch) { m_Unknown168 = pitch; }
	void Set_Doppler_Tweak(float tweak) { m_Unknown16C = tweak; }
};














































































































// NOT EXACT: inline budget (explain.py: 66 calls vs 67), reserve inlines size() and calls _Destroy where the client calls size() and inlines _Destroy; one more string destructor called
SoundDefClass::SoundDefClass(INIClass &ini, const char *section) :
	TotalWeight(0),
	LoopCount(1),
	DropOffRadius(SoundLibraryClass::_DefaultDropOffRadius),
	MaxVolRadius(SoundLibraryClass::_DefaultMaxVolRadius),
	Priority(SoundLibraryClass::_DefaultPriority),
	Volume(SoundLibraryClass::_DefaultVolume),
	MinVolumeShift(0),
	MaxVolumeShift(0),
	MinPitchShift(0),
	MaxPitchShift(0),
	MinDopplerShift(1.0F),
	MaxDopplerShift(1.0F),
	DopplerTweak(0.1F),
	Type(AudibleSoundClass::TYPE_SOUND_EFFECT),
	MaxInstances(SoundLibraryClass::_DefaultMaxInstances),
	Pitch(1.0F),
	SecsBetweenCalls(0),
	NextCallTime(0)
{
	if (*section != 0) {
		char name[64];
		strncpy(name, section, sizeof(name) - 1);
		name[sizeof(name) - 1] = 0;
		strupr(name);
		Name = name;

		char filename[80];
		ini.Get_String(section, "SoundFilename", "", filename, sizeof(filename));
		if (filename[0] != 0) {
			Files.push_back(std::pair<std::string, int>(std::string(filename), 0));
			TotalWeight = 0;
		} else {
			std::string files_section = section;
			files_section += "\\Files";
			int num_files = ini.Entry_Count(files_section.c_str());
			if (num_files != 0) {
				Files.reserve(num_files);
				for (int index = 0; index < num_files; index++) {
					const char *entry = ini.Get_Entry(files_section.c_str(), index);
					FAIL_IF(!entry) continue;
					int weight = ini.Get_Int(files_section.c_str(), entry, 0);
					if (weight > 0) {
						if (strcmp(entry, "NULL") == 0) {
							Files.push_back(std::pair<std::string, int>(std::string(""), weight));
						} else {
							Files.push_back(std::pair<std::string, int>(std::string(entry), weight));
						}
						TotalWeight += weight;
					}
				}
			} else {


				G_ASSERT(num_files);
			}
		}
	}

	if (Files.size() == 0) {
		Files.push_back(std::pair<std::string, int>(std::string(""), 0));
		TotalWeight = 0;
	}

	LoopCount = ini.Get_Int(section, "SoundLoopCount", LoopCount);
	DropOffRadius = ini.Get_Float(section, "SoundDropOffRadius", DropOffRadius);
	MaxVolRadius = ini.Get_Float(section, "SoundMaxVolRadius", MaxVolRadius);
	Priority = ini.Get_Float(section, "SoundPriority", Priority);
	Volume = ini.Get_Float(section, "SoundVolume", Volume);
	MinVolumeShift = ini.Get_Float(section, "SoundMinVolumeShift", MinVolumeShift);
	MaxVolumeShift = ini.Get_Float(section, "SoundMaxVolumeShift", MaxVolumeShift);
	MinPitchShift = ini.Get_Float(section, "SoundMinPitchShift", MinPitchShift);
	MaxPitchShift = ini.Get_Float(section, "SoundMaxPitchShift", MaxPitchShift);
	MinDopplerShift = ini.Get_Float(section, "SoundMinDopplerShift", MinDopplerShift);
	MaxDopplerShift = ini.Get_Float(section, "SoundMaxDopplerShift", MaxDopplerShift);
	DopplerTweak = ini.Get_Float(section, "SoundDopplerTweak", DopplerTweak);
	Type = ini.Get_Int(section, "SoundType", AudibleSoundClass::TYPE_SOUND_EFFECT);
	MaxInstances = ini.Get_Int(section, "SoundMaxInstances", MaxInstances);
	Pitch = ini.Get_Float(section, "SoundPitch", Pitch);
	SecsBetweenCalls = ini.Get_Float(section, "SoundSecsBetweenCalls", SecsBetweenCalls);
}


SoundDefClass::SoundDefClass(const char *filename) :
	TotalWeight(0),
	LoopCount(1),
	DropOffRadius(SoundLibraryClass::_DefaultDropOffRadius),
	MaxVolRadius(SoundLibraryClass::_DefaultMaxVolRadius),
	Priority(SoundLibraryClass::_DefaultPriority),
	Volume(SoundLibraryClass::_DefaultVolume),
	MinVolumeShift(0),
	MaxVolumeShift(0),
	MinPitchShift(0),
	MaxPitchShift(0),
	MinDopplerShift(1.0F),
	MaxDopplerShift(1.0F),
	DopplerTweak(0.1F),
	Type(AudibleSoundClass::TYPE_SOUND_EFFECT),
	MaxInstances(SoundLibraryClass::_DefaultMaxInstances),
	Pitch(1.0F),
	SecsBetweenCalls(0),
	NextCallTime(0)
{
	Name = filename;
	Files.push_back(std::pair<std::string, int>(Name, 0));
}


SoundDefClass::~SoundDefClass()
{
}










const char *SoundDefClass::Pick_Filename()
{
	if (TotalWeight != 0) {
		int sum = 0;
		int pick = Pick_Random_Number(Unknown_BDADC8, 1, TotalWeight);
		for (std::vector<std::pair<std::string, int> >::iterator it = Files.begin(); it != Files.end(); ++it) {
			sum += (*it).second;
			if (pick <= sum) {
				const char *filename = (*it).first.c_str();
				if (filename == NULL || *filename == 0) return NULL;
				return filename;
			}
		}
		G_ASSERT(false);
		return NULL;
	}

	const char *filename = Files[0].first.c_str();
	if (filename == NULL || *filename == 0) return NULL;
	return filename;
}


AudibleSoundClass *SoundDefClass::Create_Sound(int mode)
{
	if (SecsBetweenCalls != 0) {
		unsigned int now = Unknown_Get_Ticks();
		if (now - NextCallTime < 0x80000000) {
			NextCallTime = now + SecsBetweenCalls;
		} else {
			return NULL;
		}
	}

	const char *filename = Pick_Filename();
	if (filename == NULL) return NULL;

	AudibleSoundClass *sound = WWAudioClass::Get_Instance()->Create_Sound_Effect(filename);
	if (sound != NULL) {
		float volume = Volume + WWMath::Random_Float(-MinVolumeShift, MaxVolumeShift);
		sound->Set_Volume(volume);
		float pitch = Pitch + WWMath::Random_Float(-MinPitchShift, MaxPitchShift);
		sound->Set_Pitch_Factor(pitch);
		sound->Set_Priority(Priority);
		sound->Set_Loop_Count(LoopCount);
		sound->Set_Type((AudibleSoundClass::SOUND_TYPE)Type);
		((SoundAccessClass *)sound)->m_Unknown7C = MaxInstances;
		sound->Set_DropOff_Radius(DropOffRadius);
		((SoundAccessClass *)sound)->m_Unknown80 = (int)this;
		switch (mode) {
			case 1:
				sound->Play();
				sound->Update_Volume();
				break;
			case 2:
				sound->Play();
				sound->Update_Volume();
				sound->Release_Ref();
				sound = NULL;
				break;
		}
	}
	return sound;
}


Sound3DClass *SoundDefClass::Create_3D_Sound(const Matrix3D &tm, int mode, float scale)
{
	if (SecsBetweenCalls != 0) {
		unsigned int now = Unknown_Get_Ticks();
		if (now - NextCallTime < 0x80000000) {
			NextCallTime = now + SecsBetweenCalls;
		} else {
			return NULL;
		}
	}

	const char *filename = Pick_Filename();
	if (filename == NULL) return NULL;

	Sound3DClass *sound = WWAudioClass::Get_Instance()->Create_3D_Sound(filename);
	if (sound != NULL) {
		float volume = Volume + WWMath::Random_Float(-MinVolumeShift, MaxVolumeShift);
		sound->Set_Volume(volume);
		float pitch = Pitch + WWMath::Random_Float(-MinPitchShift, MaxPitchShift);
		sound->Set_Pitch_Factor(pitch);
		sound->Set_Transform(tm);
		sound->Set_Priority(Priority);
		sound->Set_Loop_Count(LoopCount);
		sound->Set_DropOff_Radius(scale * DropOffRadius);
		sound->Set_Max_Vol_Radius(scale * MaxVolRadius);
		((Sound3DAccessClass *)sound)->Set_Min_Doppler_Pitch(pitch * MinDopplerShift);
		((Sound3DAccessClass *)sound)->Set_Max_Doppler_Pitch(pitch * MaxDopplerShift);
		((Sound3DAccessClass *)sound)->Set_Doppler_Tweak(DopplerTweak);
		sound->Auto_Calc_Velocity(true);
		if (MinDopplerShift == 1.0F && MaxDopplerShift == 1.0F) {
			sound->Auto_Calc_Velocity(false);
		}
		sound->Set_Type((AudibleSoundClass::SOUND_TYPE)Type);
		((Sound3DAccessClass *)sound)->m_Unknown7C = MaxInstances;
		((Sound3DAccessClass *)sound)->m_Unknown80 = (int)this;
		switch (mode) {
			case 1:
				sound->Add_To_Scene();
				sound->Update_Volume();
				break;
			case 2:
				sound->Add_To_Scene();
				sound->Update_Volume();
				sound->Release_Ref();
				sound = NULL;
				break;
		}
	}
	return sound;
}

std::hash_map<std::string, SoundDefClass *> SoundLibraryClass::_SoundDefs;

class SoundLibBridgeClass : public SoundLibraryBridgeClass	// NAME 2: what it does (vtable 0xAE74EC)
{
public:
	virtual void Play_3D_Audio(const char *name, const Matrix3D &tm) { SoundLibraryClass::Play_3D_Sound(name, tm, 2, 1.0F); }	// 0x440280
	virtual void Play_2D_Audio(const char *name) { SoundLibraryClass::Play_2D_Sound(name, 2); }	// 0x4402B0
	virtual void Stop_Playing_Audio(const char *name) {}	// not in the client's vtable (two slots)
};
static SoundLibBridgeClass _Bridge;		// 0xBDAC40  // NAME 2: what it holds

WWAudioClass *SoundLibraryClass::_Audio;
AudibleSoundClass *SoundLibraryClass::_CurrentDialog;
SoundDefClass *SoundLibraryClass::_NextDialogDef;
char *SoundLibraryClass::_NextDialogName;
bool SoundLibraryClass::_PlayUndefined;


void SoundLibraryClass::Init()
{
	_Audio = new WWAudioClass;
	_Audio->Initialize("Software\\Westwood Studios\\Earth and Beyond\\Sound");
	_Audio->Set_File_Factory(NULL);
	AnimatedSoundMgrClass::Set_Sound_Library(&_Bridge);
	Init_Definitions();
}


void SoundLibraryClass::Init_Definitions()
{
	Load("sounds.ini");
	Unknown_442650();
	_CurrentDialog = NULL;
	_NextDialogDef = NULL;
	_NextDialogName = NULL;
}


void SoundLibraryClass::Shutdown()
{
	delete _Audio;
	_Audio = NULL;
	Free_Definitions();
}


void SoundLibraryClass::Free_Definitions()
{
	for (std::hash_map<std::string, SoundDefClass *>::iterator it = _SoundDefs.begin(); it != _SoundDefs.end(); ++it) {
		delete (*it).second;
		(*it).second = NULL;
	}
	_SoundDefs.clear();
	Unknown_442870();
	if (_CurrentDialog != NULL) {
		_CurrentDialog->Release_Ref();
		_CurrentDialog = NULL;
	}
	if (_NextDialogName != NULL) {
		delete [] _NextDialogName;
		_NextDialogName = NULL;
	}
	_NextDialogDef = NULL;
}


void SoundLibraryClass::Reload()
{
	Free_Definitions();
	Init_Definitions();
}


SoundDefClass *SoundLibraryClass::Find(const std::string &name)
{
	char upper[64];
	strncpy(upper, name.c_str(), sizeof(upper) - 1);
	upper[sizeof(upper) - 1] = 0;
	strupr(upper);
	std::hash_map<std::string, SoundDefClass *>::iterator it = _SoundDefs.find(std::string(upper));
	if (it == _SoundDefs.end()) return NULL;
	return (*it).second;
}


// NOT EXACT: inline budget (explain.py: 16 calls vs 15), the key string built inline through _Grow / _Eos where the client calls assign; the client inlines the temporary's string destructor
AudibleSoundClass *SoundLibraryClass::Play_2D_Sound(const char *name, int mode)
{
	SoundDefClass *def = Find(std::string(name));
	if (def != NULL && def->Type == AudibleSoundClass::TYPE_DIALOG) {
		return Play_Dialog(def, mode, name);
	}
	if (def == NULL) {
		if (_PlayUndefined) {
			SoundDefClass temp(name);
			return temp.Create_Sound(mode);
		}
		return NULL;
	}
	return def->Create_Sound(mode);
}


Sound3DClass *SoundLibraryClass::Play_3D_Sound(const char *name, const Matrix3D &tm, int mode, float scale)
{
	SoundDefClass *def = Find(std::string(name));
	if (def == NULL) {
		if (_PlayUndefined) {
			SoundDefClass temp(name);
			return temp.Create_3D_Sound(tm, mode, scale);
		}
		return NULL;
	}
	return def->Create_3D_Sound(tm, mode, scale);
}


AudibleSoundClass *SoundLibraryClass::Play_Dialog(const char *name, int mode)
{
	return Play_Dialog(NULL, mode, name);
}


// NOT EXACT: shape (explain.py: 22 calls vs 23), ours merges the two releases of _CurrentDialog the client keeps apart; name in ebp, not ebx
AudibleSoundClass *SoundLibraryClass::Play_Dialog(SoundDefClass *def, int mode, const char *name)
{
	if (name == NULL) return NULL;
	if (def != NULL) {
		def->Type = AudibleSoundClass::TYPE_DIALOG;
	}

	if (_CurrentDialog != NULL && _CurrentDialog->Get_State() == AudibleSoundClass::STATE_STOPPED) {
		_CurrentDialog->Release_Ref();
		_CurrentDialog = NULL;
	}
	if (_CurrentDialog != NULL) {
		if (strcmp(_CurrentDialog->Get_Filename(), name) == 0) return NULL;
		float priority = _DefaultPriority;
		float next_priority = _DefaultPriority;
		if (def != NULL) {
			priority = def->Priority;
		}
		if (_NextDialogDef != NULL) {
			next_priority = _NextDialogDef->Priority;
		}
		if (_CurrentDialog->Get_Priority() < priority) {
			_CurrentDialog->Stop();
			_CurrentDialog->Release_Ref();
			_CurrentDialog = NULL;
		} else {
			if (_CurrentDialog->Get_Duration() - _CurrentDialog->Get_Play_Position() < 2000) {
				if (_NextDialogName != NULL && next_priority >= priority) return NULL;
				_NextDialogDef = def;
				if (_NextDialogName != NULL) {
					delete [] _NextDialogName;
				}
				_NextDialogName = nstrdup(name);
				return NULL;
			}
			if (_CurrentDialog != NULL) return NULL;
		}
	}

	AudibleSoundClass *sound;
	if (mode == 2) {
		if (def != NULL) {
			sound = def->Create_Sound(1);
		} else {
			SoundDefClass temp(name);
			temp.Set_Volume(_DefaultDialogVolume);
			temp.Set_Type(AudibleSoundClass::TYPE_DIALOG);
			sound = temp.Create_Sound(1);
		}
	} else {
		if (def != NULL) {
			sound = def->Create_Sound(1);
		} else {
			SoundDefClass temp(name);
			temp.Set_Volume(_DefaultDialogVolume);
			temp.Set_Type(AudibleSoundClass::TYPE_DIALOG);
			sound = temp.Create_Sound(1);
		}
		if (sound != NULL) {
			sound->Add_Ref();
		}
	}
	if (sound == NULL) return NULL;
	_CurrentDialog = sound;
	sound->Set_Type(AudibleSoundClass::TYPE_DIALOG);
	return sound;
}















































































void SoundLibraryClass::Update_Dialog()
{
	if (_CurrentDialog == NULL) {
		G_ASSERT((_NextDialogDef == NULL) && (_NextDialogName == NULL));
		return;
	}
	if (_CurrentDialog->Get_State() != AudibleSoundClass::STATE_STOPPED) return;

	_CurrentDialog->Release_Ref();
	_CurrentDialog = NULL;
	if (_NextDialogDef != NULL) {
		_CurrentDialog = _NextDialogDef->Create_Sound(1);
		if (_CurrentDialog != NULL) {
			_CurrentDialog->Set_Type(AudibleSoundClass::TYPE_DIALOG);
		}
		_NextDialogDef = NULL;
	} else if (_NextDialogName != NULL) {
		SoundDefClass temp(_NextDialogName);
		temp.Set_Volume(_DefaultDialogVolume);
		temp.Set_Type(AudibleSoundClass::TYPE_DIALOG);
		_CurrentDialog = temp.Create_Sound(1);
	} else {
		return;
	}
	if (_NextDialogName != NULL) {
		delete [] _NextDialogName;
		_NextDialogName = NULL;
	}
}


void SoundLibraryClass::Stop_Dialog()
{
	if (_CurrentDialog != NULL) {
		_CurrentDialog->Stop();
		_CurrentDialog->Release_Ref();
		_CurrentDialog = NULL;
	}
	if (_NextDialogName != NULL) {
		delete [] _NextDialogName;
		_NextDialogName = NULL;
	}
}


bool SoundLibraryClass::Is_Dialog_Playing()
{
	return (_CurrentDialog != NULL);
}


void SoundLibraryClass::Set_Listener_Position(const Vector3 &pos)
{
	WWAudioClass::Get_Instance()->Get_Sound_Scene()->Set_Listener_Position(pos);
}


Vector3 SoundLibraryClass::Get_Listener_Position()
{
	return WWAudioClass::Get_Instance()->Get_Sound_Scene()->Get_Listener_Position();
}


void SoundLibraryClass::Set_Listener_Transform(const Matrix3D &tm)
{
	WWAudioClass::Get_Instance()->Get_Sound_Scene()->Set_Listener_Transform(tm);
}


Matrix3D SoundLibraryClass::Get_Listener_Transform()
{
	return WWAudioClass::Get_Instance()->Get_Sound_Scene()->Get_Listener_Transform();
}


void SoundLibraryClass::Flush_Playlist()
{
	WWAudioClass::Get_Instance()->Flush_Playlist();
}


void SoundLibraryClass::Update()
{
	float volume = WWAudioClass::Get_Instance()->Get_Sound_Effects_Volume();
	float target = Unknown_BE8B38->Get_Float("Sound_SFX") * 0.01F;
	if (volume != target) {
		float new_volume = target;
		if (target > volume) {
			new_volume = WWMath::Min(target, volume + 0.1F);
		} else if (target < volume) {
			new_volume = WWMath::Max(target, volume - 0.1F);
		}
		WWAudioClass::Get_Instance()->Set_Sound_Effects_Volume(new_volume);
	}

	volume = WWAudioClass::Get_Instance()->Get_Music_Volume();
	target = Unknown_BE8B38->Get_Float("Sound_Music") * 0.01F;
	if (volume != target) {
		float new_volume = target;
		if (target > volume) {
			new_volume = WWMath::Min(target, volume + 0.1F);
		} else if (target < volume) {
			new_volume = WWMath::Max(target, volume - 0.1F);
		}
		WWAudioClass::Get_Instance()->Set_Music_Volume(new_volume);
	}

	volume = WWAudioClass::Get_Instance()->Get_Dialog_Volume();
	target = Unknown_BE8B38->Get_Float("Sound_Dialog") * 0.01F;
	if (volume != target) {
		float new_volume = target;
		if (target > volume) {
			new_volume = WWMath::Min(target, volume + 0.1F);
		} else if (target < volume) {
			new_volume = WWMath::Max(target, volume - 0.1F);
		}
		WWAudioClass::Get_Instance()->Set_Dialog_Volume(new_volume);
	}

	WWAudioClass::Get_Instance()->On_Frame_Update();
	Update_Dialog();
}


void SoundLibraryClass::Apply_Volumes()
{
	float volume = Unknown_BE8B38->Get_Float("Sound_SFX") * 0.01F;
	WWAudioClass::Get_Instance()->Set_Sound_Effects_Volume(volume);
	volume = Unknown_BE8B38->Get_Float("Sound_Music") * 0.01F;
	WWAudioClass::Get_Instance()->Set_Music_Volume(volume);
	volume = Unknown_BE8B38->Get_Float("Sound_Dialog") * 0.01F;
	WWAudioClass::Get_Instance()->Set_Dialog_Volume(volume);
}

class SoundINIClass : public INIClass		// NAME 1 [SoundINIClass]: an INIClass with its own vtable 0xAE7500 (destructor only)
{
public:
	SoundINIClass(FileClass &file) : INIClass(file) {}
};















// NOT EXACT: inline budget (explain.py: 29 calls vs 28, frame 0x84 vs 0x80), ours inlines the key's copy constructor in operator[] where the client calls it
void SoundLibraryClass::Load(const char *filename)
{
	file_auto_ptr file(_TheFileFactory, filename);
	INIClass *ini = new SoundINIClass(*file);
	for (INISection *section = ini->Get_Section_List().First_Valid(); section != NULL; section = section->Next_Valid()) {
		if (strcmp(section->Section, "SOUND_DEFAULTS") == 0) {
			_DefaultDropOffRadius = ini->Get_Float("SOUND_DEFAULTS", "Default_DropOffRadius", _DefaultDropOffRadius);
			_DefaultMaxVolRadius = ini->Get_Float("SOUND_DEFAULTS", "Default_MaxVolRadius", _DefaultMaxVolRadius);
			_DefaultPriority = ini->Get_Float("SOUND_DEFAULTS", "Default_Priority", _DefaultPriority);
			_DefaultVolume = ini->Get_Float("SOUND_DEFAULTS", "Default_Volume", _DefaultVolume);
			_DefaultMaxInstances = ini->Get_Int("SOUND_DEFAULTS", "Default_MaxInstances", _DefaultMaxInstances);
			_DefaultDialogVolume = ini->Get_Float("SOUND_DEFAULTS", "Default_Dialog_Volume", _DefaultDialogVolume);
		} else if (strstr(section->Section, "\\Files") == NULL) {
			SoundDefClass *def = new SoundDefClass(*ini, section->Section);
			if (Find(def->Name) == NULL) {
				_SoundDefs[def->Name] = def;
			} else {
				G_ASSERT(false);
				delete def;
			}
		}
	}
	delete ini;
}

float SoundLibraryClass::_DefaultDropOffRadius = 1200.0F;
float SoundLibraryClass::_DefaultMaxVolRadius = 400.0F;
float SoundLibraryClass::_DefaultPriority = 0.9F;
float SoundLibraryClass::_DefaultVolume = 0.8F;
float SoundLibraryClass::_DefaultDialogVolume = 0.8F;
int SoundLibraryClass::_DefaultMaxInstances = 4;

/*
**	Reconstruction notes
**
**	client.exe 0x43D3E0-0x440280 (then the bridge's two slots 0x440280 / 0x4402B0 and the templates'
**	instances to 0x441A50; starbasesound.cpp follows at 0x441A90): SoundDefClass's constructors
**	0x43D3E0 (one sounds.ini section: SoundFilename, else the weighted entries of "<name>\Files",
**	"NULL" standing for silence; the reports on the client's lines 198 / 212, D25) and 0x43DF90 (one
**	file), its destructor 0x43E160, Pick_Filename 0x43E230 (Pick_Random_Number over the weights,
**	the generator 0xBDADC8; the assert on line 292), Create_Sound 0x43E360 and Create_3D_Sound
**	0x43E530 (mode 1 plays the sound, 2 plays and releases it; SoundSecsBetweenCalls throttles
**	through the aux-data clock). _SoundDefs's initializer 0x43E780 / destructor 0x43E7B0 and the
**	table's destructor 0x43E7E0 follow, then the bridge object's initializer 0x43E8B0 (vtable
**	0xAE74EC, handed to AnimatedSoundMgrClass::Set_Sound_Library 0x90A0A0 by Init). Then
**	SoundLibraryClass: Init 0x43E8D0 (WWAudio from the registry key 'Software\Westwood
**	Studios\Earth and Beyond\Sound', D69: the code only reads it), Init_Definitions 0x43E9A0 (inlined
**	into Init and Reload), Shutdown 0x43E9E0, Free_Definitions 0x43EA10, Reload 0x43EB90, Find
**	0x43EBD0 (upper-case key), Play_2D_Sound 0x43ED30, Play_3D_Sound 0x43EFE0, the dialog channel
**	(Play_Dialog 0x43F2A0 / 0x43F2D0, Update_Dialog 0x43F720 with the assert on line 669, Stop_Dialog
**	0x43F920, Is_Dialog_Playing 0x43F9A0), the listener and playlist forwarders 0x43F9C0-0x43FA80,
**	Update 0x43FAA0 (each volume eased 0.1 per frame toward the options' Sound_SFX / Sound_Music /
**	Sound_Dialog), Apply_Volumes 0x43FD70 and Load 0x43FE30 (SOUND_DEFAULTS, then one definition per
**	section that isn't a "\Files" list; a duplicate asserts on line 838).
**	Forms: the definition's Volume and Type set through inline setters (the client copies the dialog
**	volume through the FPU); Sound3DClass's E&B Doppler members m_Unknown164-16C likewise through
**	setters, m_Unknown7C / m_Unknown80 stored directly (Sound3DAccessClass / SoundAccessClass make
**	the tree's protected members reachable; no code of their own). The volumes eased with
**	WWMath::Min / Max into a third local; the dialog's release written out (no REF_PTR_RELEASE: the
**	client tests nothing after the state check).
**	SoundLibBridgeClass implements the tree's three-slot SoundLibraryBridgeClass; the client's vtable
**	has two slots (no Stop_Playing_Audio): ww3d2's header is newer than E&B's.
**	Callers: 0x526DE0 (Init, Shutdown), 0x435390 / 0x435610 / 0x435AE0 (Play_2D_Sound, Play_3D_Sound),
**	0x64D770 / 0x67CBD0 (Play_Dialog), 0x744690 (Update), 0x766350 (Apply_Volumes, tsk_game.cpp's
**	Unknown_43FD70), 0x7675D0 / 0x8008B0 (Flush_Playlist, tsk_login.cpp's Unknown_43FA80); no caller
**	of Reload or Init_Definitions out of line.
*/
