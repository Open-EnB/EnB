// Earth & Beyond client (reconstruction): app\Audio\surfacetypes.cpp. Notes at the end of the file.
#include <string.h>
#include "surfacetypes.h"
#include "soundlib.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"
#include "..\..\wwlib\ffactory.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\..\ww3d2\w3d_file.h"
#include "..\..\wwmath\matrix3d.h"

class SoundINIClass : public INIClass		// NAME 1 [SoundINIClass]: an INIClass with its own vtable 0xAE7500 (destructor only)
{
public:
	SoundINIClass(FileClass &file) : INIClass(file) {}
};

static char *SoundWalking[SURFACE_TYPE_MAX];	// 0xBDAC84  // NAME 2: what it holds
static char *SoundRunning[SURFACE_TYPE_MAX];	// 0xBDAD04  // NAME 2: what it holds

// NOT EXACT: frame (0.51, sub esp 0x90 vs 0x8c)
// @0x442650
void Unknown_442650()							// NAME 1 [Unknown_442650]: 0x442650, surfacetypes.cpp's loader
{
	memset(SoundRunning, 0, sizeof(SoundRunning));
	memset(SoundWalking, 0, sizeof(SoundWalking));

	file_auto_ptr file(_TheFileFactory, "footsteps.ini");
	int surfacetype = 0;
	INIClass *ini = new SoundINIClass(*file);






























	for (INISection *section = ini->Get_Section_List().First_Valid(); section != NULL; section = section->Next_Valid()) {
		const char *name = section->Section;
		if (name[0] != 0) {
			G_ASSERT(surfacetype < SURFACE_TYPE_MAX);
			char buffer[128];
			ini->Get_String(name, "SoundWalking", "", buffer, sizeof(buffer));
			if (buffer[0] != 0) {
				SoundWalking[surfacetype] = nstrdup(buffer);
			}
			ini->Get_String(name, "SoundRunning", "", buffer, sizeof(buffer));
			if (buffer[0] != 0) {
				SoundRunning[surfacetype] = nstrdup(buffer);
			}
		}


		surfacetype++;
	}
	G_ASSERT(surfacetype == SURFACE_TYPE_MAX);
	delete ini;
}


// @0x442870
void Unknown_442870()							// NAME 1 [Unknown_442870]: 0x442870, surfacetypes.cpp's release
{
	for (int i = 0; i < SURFACE_TYPE_MAX; i++) {
		if (SoundRunning[i] != NULL) {
			delete [] SoundRunning[i];
			SoundRunning[i] = NULL;
		}
		if (SoundWalking[i] != NULL) {
			delete [] SoundWalking[i];
			SoundWalking[i] = NULL;
		}
	}
}


// @0x4428E0
void Unknown_4428E0(int surfacetype, const Matrix3D &tm)	// NAME 1 [Unknown_4428E0]: 0x4428E0
{
	if (surfacetype < 0 || surfacetype >= SURFACE_TYPE_MAX) {
		surfacetype = 0;
	}
	if (SoundRunning[surfacetype] == NULL) {
		surfacetype = 0;
	}
	if (SoundRunning[surfacetype] != NULL) {
		SoundLibraryClass::Play_3D_Sound(SoundRunning[surfacetype], tm, 2, 1.0f);
	}
}


// @0x442930
void Unknown_442930(int surfacetype, const Matrix3D &tm)	// NAME 1 [Unknown_442930]: 0x442930
{
	if (surfacetype < 0 || surfacetype >= SURFACE_TYPE_MAX) {
		surfacetype = 0;
	}
	if (SoundWalking[surfacetype] == NULL) {
		surfacetype = 0;
	}
	if (SoundWalking[surfacetype] != NULL) {
		SoundLibraryClass::Play_3D_Sound(SoundWalking[surfacetype], tm, 2, 1.0f);
	}
}

/*
**	Reconstruction notes:
**	surfacetypes.cpp (0x442650-0x442970) loads surface walking/running footstep sound effects
**	from footsteps.ini into two 32-entry tables indexed by surface type (SURFACE_TYPE_MAX = 32).
**	Asserts on lines 65 and 80 confirm the client file name e:\build\beta\G\app\Audio\surfacetypes.cpp.
*/
