// Earth & Beyond client (reconstruction): app\client\mv3dsnd.cpp, the 3-D movie's sounds (Movie3DSoundClass),
// client 0x7EDCB0-0x7EEF50: each animation id's mesh sounds, read from the movie's 'SFX' section of movie3d.ini. The
// assert sits on the client's line. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sclient.h"
#include "mv3dview.h"
#include "..\shared\g_assert.h"




































































































































// NOT EXACT: functional (M1): inline budget (0.45 with the inline_depth(0) below, 0.30 without; attempts.py show 0x7EDCB0): the client calls GenericList's constructor, HashListClass::Find (0x7EEBD0), the node's constructor (0x7EED40) and Add(node) (0x7EEE00) out of line but expands Add(record, key) around the last two; inline_depth(0) gives Find, leaves Add(record, key) out of line too, and lays the Name assert on the loop's path
#pragma inline_depth(0)
// 0x7EDCB0
Movie3DSoundClass::Movie3DSoundClass(INIClass &ini, const char *section)
{
	int index = 0;
	float fps = ini.Get_Float(section, "FPS", 15.0f);
	for (;;) {
		char entry[256];
		sprintf(entry, "%d", index++);
		if (!ini.Get_String(section, entry, "", entry, 255)) {
			break;
		}
		char entry_section[256];
		strcpy(entry_section, section);
		strcat(entry_section, "\\");
		strcat(entry_section, entry);
		if (!ini.Get_String(entry_section, "Name", "", entry, 255)) {
			G_ASSERT(false);
		} else {
		char bone[256];
		ini.Get_String(entry_section, "Bone", "", bone, 255);
		fps = ini.Get_Float(entry_section, "FPS", 15.0f);
		float time = ini.Get_Float(entry_section, "Time", 0.0f);
		float duration = ini.Get_Float(entry_section, "Duration", -1.0f);
		float drop_off_radius = ini.Get_Float(entry_section, "DropOffRadius", -1.0f);
		float volume = ini.Get_Float(entry_section, "Volume", -1.0f);
		float priority = ini.Get_Float(entry_section, "Priority", -1.0f);
		float pitch_speed = ini.Get_Float(entry_section, "PitchSpeed", -1.0f);
		float pitch_change = ini.Get_Float(entry_section, "PitchChange", 1.0f);
		int playback_rate = ini.Get_Int(entry_section, "PlaybackRate", 0);
		bool loop = ini.Get_Bool(entry_section, "Loop", time == 0.0f);
		bool mono = ini.Get_Bool(entry_section, "Mono", false);
		char anims[256];
		ini.Get_String(entry_section, "Anim", "1", anims, 255);
		for (char *token = strtok(anims, ", 	"); token != NULL; token = strtok(NULL, ", 	")) {
			int id = atoi(token);
			HashNodeClass<MeshSoundListClass *> *node = Find(id);
			MeshSoundListClass *list;
			if (node == NULL || (list = node->Get_Record()) == NULL) {
				list = new MeshSoundListClass;
				Add(list, id);
			}
			char key[256];
			sprintf(key, "Time%d", id);
			int start = ini.Get_Float(entry_section, key, time) * fps;
			sprintf(key, "Duration%d", id);
			float length = ini.Get_Float(entry_section, key, duration);
			int end = (length > 0.0) ? length * fps + start : -1.0f;
			MeshSoundClass *sound = new MeshSoundClass(entry, bone);
			list->Sounds.Add_Head(&sound->Node);
			sound->StartFrame = start;
			if (end > 0.0f) {
				sound->EndFrame = end;
			}
			sound->Volume = volume;
			sound->Priority = priority;
			sound->DropOffRadius = drop_off_radius;
			sound->Loop = loop;
			sound->PitchSpeed = pitch_speed;
			sound->PitchChange = pitch_change;
			sound->PlaybackRate = playback_rate;
			sound->Mono = mono;
		}
		}
	}
}
#pragma inline_depth()
// NOT EXACT: functional (M1): exception states, the client's inlined ~HashListClass sets none for the list's two nodes (base and member forms of the hash list give the same; attempts.py show 0x7EE340)
// 0x7EE340
Movie3DSoundClass::~Movie3DSoundClass()
{
	HashNodeClass<MeshSoundListClass *> *node = First_Valid();
	while (node != NULL) {
		MeshSoundListClass *sounds = node->Get_Record();
		HashNodeClass<MeshSoundListClass *> *next = node->Next_Valid();
		Remove(node);
		delete sounds;
		node = next;
	}
}

// 0x7EE690
void Movie3DSoundClass::Unknown_7EE690(int id, int frame, RenderObjClass *mesh)
{
	HashNodeClass<MeshSoundListClass *> *node = Find(id);
	if (node != NULL && node->Get_Record() != NULL) {
		node->Get_Record()->Unknown_7EDAD0(frame, mesh);
	}
}

// 0x7EE700
void Movie3DSoundClass::Unknown_7EE700(int id)
{
	HashNodeClass<MeshSoundListClass *> *node = Find(id);
	if (node != NULL && node->Get_Record() != NULL) {
		node->Get_Record()->Unknown_7EDB60();
	}
}

// 0x7EE770
void Movie3DSoundClass::Unknown_7EE770(int id, int frame)
{
	HashNodeClass<MeshSoundListClass *> *node = Find(id);
	if (node != NULL && node->Get_Record() != NULL) {
		node->Get_Record()->Frame = frame;
	}
}

// 0x7EE7E0
void Movie3DSoundClass::Unknown_7EE7E0(unsigned int ms)
{
	for (HashNodeClass<MeshSoundListClass *> *node = First_Valid(); node != NULL; node = node->Next_Valid()) {
		node->Get_Record()->Unknown_7EDBE0(ms);
	}
}

// 0x7EE830
MeshSoundListClass *Movie3DSoundClass::Unknown_7EE830(int id)
{
	HashNodeClass<MeshSoundListClass *> *node = Find(id);
	if (node != NULL) {
		return(node->Get_Record());
	}
	return(NULL);
}


/*
**	Notes (M1, run 2026-11-11b)
**
**	The object runs 0x7EDCB0-0x7EEF50; mv3dview.cpp's starts at 0x7EEF60. 0x7EE5C0 .. 0x7EEE00 are the template
**	copies the hash list of MeshSoundListClass pointers (7 buckets) and the lists bring: HashListClass's destructor
**	(0x7EE8A0), Find (0x7EEBD0), Remove (0x7EEC20), the node's constructor (0x7EED40), Add (0x7EEE00) and the List
**	tables 0xB079F4 / 0xB079F8. Whether the hash list is Movie3DSoundClass's base or its first member the bytes do
**	not tell (both forms compile every function the same); the base is kept. asserts.txt's 'movie3d.ini' for this
**	file is mv3dview.cpp's 0x7EEF90: no function here names the file.
**
**	The constructor sits inside '#pragma inline_depth(0)' (a device, not the client's: it sends Find out of line as the
**	client has it, so 0x7EEBD0 is exact, and costs nothing elsewhere; a file-end inline_depth(0..3) loses 0x7EE690 ..
**	0x7EE830). 0x7EE5C0 / 0x7EEAD0 (the two List destructors), their deleting 0x7EEAA0 / 0x7EEBA0 and the node's
**	0x7EED80 / 0x7EEDB0 are compiled byte-identical and tie with other files' copies (objmatch 'multiple'): they are
**	credited by their symbols_named.json rows (the unwind funclets of 0x7EE340 / 0x7EE8A0 and the tables, O5
**	2026-11-12). 0x7EED40 and 0x7EEE00 come with 0x7EDCB0's Add(record, key).
*/
