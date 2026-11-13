// Earth & Beyond client (reconstruction): app\Audio\starbasesound.cpp. Notes at the end of the file.
#include <stdio.h>
#include "starbasesound.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\random.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\client\ui_mesh.h"

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's

// the rooms' section names in starbasesounds<n>.ini, by channel (0xB6F73C)
static const char *_RoomSectionNames[StarbaseSoundClass::NUM_ROOMS] = {	// NAME 2: what it holds
	"Hangar",
	"MainHall",
	"Bazaar",
	"NPCLounge",
	"PlayerLounge",
};

// @0x441910
StarbaseSoundClass::StarbaseSoundClass(int starbase) :
	AmbientSoundManagerClass(NUM_ROOMS),
	Starbase(starbase),
	DefaultMinPan(0.5f),
	DefaultMaxPan(0.5f)
{
	Unknown2C = 0;
	FadeOut = 500;
	DefaultMinDelay = 10.0f;
	DefaultMaxDelay = 20.0f;
	INI = NULL;
	Load_INI();
}

// @0x4419B0
StarbaseSoundClass::~StarbaseSoundClass()
{
	AmbientSoundManagerClass::Reset();
	if (INI != NULL) {
		delete INI;
		INI = NULL;
	}
}

// @0x441A30
void StarbaseSoundClass::Set_Room(int room)
{
	Set_Pending_Channel(room);
}

// @0x441A50
void StarbaseSoundClass::Reset(int room)
{
	AmbientSoundManagerClass::Reset();
	AmbientSoundManagerClass::Initialize(NUM_ROOMS);
	Load_INI();
	Set_Pending_Channel(room);
}



























































































// @0x441A90
void StarbaseSoundClass::Update(int time, UIMeshClass *mesh)
{
	FAIL_IF(mesh == NULL) return;
	RenderObjClass *obj = mesh->Peek_Object();
	FAIL_IF(obj == NULL) return;
	Matrix3D tm(1);
	int bone = Unknown_BDBB84(0, obj->Get_Num_Bones() - 1);
	tm = obj->Get_Bone_Transform(bone);
	AmbientSoundManagerClass::Update(time, tm);
}

// @0x441C40
void StarbaseSoundClass::Load_INI()
{
	char filename[128];
	sprintf(filename, "starbasesounds%d.ini", Starbase);
	INI = new INIClass(filename);
	if (INI != NULL && INI->Is_Loaded() && INI->Section_Present("General")) {
		FadeOut = (int)(INI->Get_Float("General", "FadeOut", 0.5f) * 1000.0f);
		DefaultMinDelay = INI->Get_Float("General", "DefaultMinDelay", 10.0f);
		DefaultMaxDelay = INI->Get_Float("General", "DefaultMaxDelay", 20.0f);
		DefaultMinPan = INI->Get_Float("General", "DefaultMinPan", DefaultMinPan);
		DefaultMaxPan = INI->Get_Float("General", "DefaultMaxPan", DefaultMaxPan);
	}
}

















































// @0x441DD0
void StarbaseSoundClass::Load_Section(int room, const char *section_name)
{
	const char *section = _RoomSectionNames[room];
	if (section_name != NULL && INI->Is_Present(section_name)) {
		section = section_name;
	}
	INIClass *ini = INI;
	FAIL_IF(ini == NULL) return;
	if (ini->Section_Present(section)) {
		for (int index = 1; index <= 64; index++) {
			char entry[128];
			char name[128];
			sprintf(entry, "LoopingAmbient%d", index);
			if (ini->Is_Present(section, entry)) {
				ini->Get_String(section, entry, "ERROR", name, sizeof(name));
				Add_Looping_Sound(room, name);
			}
			sprintf(entry, "NonLoopingAmbient%d", index);
			if (ini->Is_Present(section, entry)) {
				ini->Get_String(section, entry, "ERROR", name, sizeof(name));
				sprintf(entry, "MinDelay%d", index);
				int min_delay = (int)(ini->Get_Float(section, entry, DefaultMinDelay) * 1000.0f);
				sprintf(entry, "MaxDelay%d", index);
				int max_delay = (int)(ini->Get_Float(section, entry, DefaultMaxDelay) * 1000.0f);
				sprintf(entry, "MinPan%d", index);
				float min_pan = ini->Get_Float(section, entry, DefaultMinPan);
				sprintf(entry, "MaxPan%d", index);
				float max_pan = ini->Get_Float(section, entry, DefaultMaxPan);
				sprintf(entry, "FixedPosition%d", index);
				bool fixed_position = ini->Get_Bool(section, entry, true);
				Add_Random_Sound(room, name, min_delay, max_delay, min_pan, max_pan, fixed_position);
			}
		}
	}
}

// @0x4420E0
void StarbaseSoundClass::Load_Section(int room, RenderObjClass *rendobj, const char *section_name)
{
	if (room < 0 || room >= NUM_ROOMS || rendobj == NULL || INI == NULL) return;
	const char *section = _RoomSectionNames[room];
	const char *model_name = rendobj->Get_Name();
	if (INI->Is_Present(section_name)) {
		section = section_name;
	}
	for (int index = 1; index <= 64; index++) {
		char entry[128];
		char mesh[128];
		char sound[128];
		char bone[128];
		sprintf(entry, "TrackedMesh%d", index);
		if (INI->Is_Present(section, entry)) {
			INI->Get_String(section, entry, "ERROR", mesh, sizeof(mesh));
			if (strcmp(model_name, mesh) == 0) {
				sprintf(entry, "TrackedSound%d", index);
				if (INI->Is_Present(section, entry)) {
					INI->Get_String(section, entry, "ERROR", sound, sizeof(sound));
					sprintf(entry, "TrackedBone%d", index);
					INI->Get_String(section, entry, "ROOTTRANSFORM", bone, sizeof(bone));
					sprintf(entry, "OnlyAddSoundIfBoneUsed%d", index);
					if (!INI->Get_Bool(section, entry, false) || rendobj->Get_Num_Sub_Objects_On_Bone(rendobj->Get_Bone_Index(bone)) > 0) {
						sprintf(entry, "TrackedMinDelay%d", index);
						int min_delay = (int)(INI->Get_Float(section, entry, DefaultMinDelay) * 1000.0f);
						sprintf(entry, "TrackedMaxDelay%d", index);
						int max_delay = (int)(INI->Get_Float(section, entry, DefaultMaxDelay) * 1000.0f);
						Add_Random_Sound(room, sound, min_delay, max_delay, rendobj, bone);
					}
				}
			}
		}
		char looping_mesh[128];
		char looping_sound[128];
		char looping_bone[128];
		sprintf(entry, "LoopingTrackedMesh%d", index);
		if (INI->Is_Present(section, entry)) {
			INI->Get_String(section, entry, "ERROR", looping_mesh, sizeof(looping_mesh));
			if (strcmp(model_name, looping_mesh) == 0) {
				sprintf(entry, "LoopingTrackedSound%d", index);
				if (INI->Is_Present(section, entry)) {
					INI->Get_String(section, entry, "ERROR", looping_sound, sizeof(looping_sound));
					sprintf(entry, "LoopingTrackedBone%d", index);
					INI->Get_String(section, entry, "ROOTTRANSFORM", looping_bone, sizeof(looping_bone));
					sprintf(entry, "OnlyAddLoopingSoundIfBoneUsed%d", index);
					if (!INI->Get_Bool(section, entry, false) || rendobj->Get_Num_Sub_Objects_On_Bone(rendobj->Get_Bone_Index(looping_bone)) > 0) {
						Add_Looping_Sound(room, looping_sound, rendobj, looping_bone);
					}
				}
			}
		}
	}
}

/*
Reconstruction notes:
starbasesound.cpp holds StarbaseSoundClass (derived from AmbientSoundManagerClass): a starbase's ambient sounds, read
from starbasesounds<n>.ini, one channel per room. The object is 0x441910-0x4428E0 (after soundlib.cpp's
instances, before surfacetypes.cpp). The second Load_Section 0x4420E0 reads the sounds tracked on a model's
bones (TrackedMesh<n> must name the model); OnlyAdd...IfBoneUsed is vtable +0x74 on the bone's index (+0xB0),
read as Get_Num_Sub_Objects_On_Bone > 0. It is followed by int3 padding to 0x442640 (appfile's PREFIX).
*/
