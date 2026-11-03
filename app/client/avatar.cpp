#include "avatar.h"
#include "tutorial_manager.h"
#include "nstrdup.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\shared\unknown_debug.h"

// 0x77B870
// NOT EXACT: functional for POC (D64)
CharacterViewAvatarClass::CharacterViewAvatarClass(AvatarDescClass *desc, int a, int b, SClientUnknown1338Class *env_maps)
	: CompositeRenderObjClass(),
	  PlayerNLP(NULL),
	  Unknown204(0),
	  Unknown208(0),
	  Unknown20C(2),
	  Unknown210(0),
	  Unknown214(false),
	  Unknown215(false),
	  Unknown220(false),
	  Unknown221(false),
	  Unknown231(false),
	  Unknown232(false),
	  Unknown24C(-1),
	  Unknown2B8(NULL),
	  Unknown2BC(NULL),
	  SpeechSound(NULL),
	  Unknown2EC(NULL),
	  Unknown2F0(0.0f),
	  Unknown2F8(0),
	  Unknown308(0.0f),
	  Unknown314(0)
{
	if (desc) {
		Desc = *desc;
	}
	Desc.Upgrade();
	if (a) {
		Unknown221 = true;
		Unknown220 = true;
	}
}

// 0x77C080
// NOT EXACT: functional for POC (D64)
CharacterViewAvatarClass::~CharacterViewAvatarClass()
{
	Unknown_784E00();
}

// 0x78A890
// NOT EXACT: functional for POC (D64)
RenderObjClass *CharacterViewAvatarClass::Clone() const
{
	return(NULL);
}

// 0x78A6B0
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Render(RenderInfoClass &rinfo)
{
	CompositeRenderObjClass::Render(rinfo);
}

// 0x77E3D0
void CharacterViewAvatarClass::Unknown_77E3D0(int a)
{
	Unknown204 = a;
}

// 0x784A50
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_784A50(const char *name, int a, int b)
{
	if (Unknown2F8 == 0 && Unknown308 == 0.0f) {
		Unknown214 = (a != 0);
		if (a) {
			Unknown24C = -1;
		}
		Unknown220 = false;
		Unknown215 = (b != 0);
	}
}

// 0x784C90
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_784C90(const char *name, int a)
{
	if (!PlayerNLP) {
		// In the client, asserts PlayerNLP != 0
	}
	if (!a) {
		if (TutorialManagerClass::Instance && !TutorialManagerClass::Instance->Is_Completed(name)) {
			TutorialManagerClass::Instance->Set_Completed(name);
		}
	}
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound->Release_Ref();
		SpeechSound = NULL;
	}
	if (TutorialManagerClass::Instance) {
		TutorialManagerClass::Instance->Stop_Step();
	}
	Unknown208 = 0;
	Unknown214 = true;
	if (Unknown2EC) {
		delete[] Unknown2EC;
		Unknown2EC = NULL;
	}
	if (TutorialManagerClass::Instance) {
		TutorialManagerClass::Instance->Start_Step(name);
	}
	Unknown2F0 = 60.0f;
	Unknown2EC = nstrdup(name);
}

// 0x784E00
void CharacterViewAvatarClass::Unknown_784E00()
{
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound->Release_Ref();
		SpeechSound = NULL;
	}
	if (TutorialManagerClass::Instance) {
		TutorialManagerClass::Instance->Stop_Step();
	}
	Unknown208 = 0;
	Unknown214 = true;
	if (Unknown2EC) {
		delete[] Unknown2EC;
		Unknown2EC = NULL;
	}
}

// 0x786370
void CharacterViewAvatarClass::Unknown_786370(bool on)
{
	if (Unknown2BC) {
		Unknown2BC->Set_Hidden(on);
	}
	if (Unknown2B8) {
		Unknown2B8->Set_Hidden(on);
	}
}

// 0x7863E0
bool CharacterViewAvatarClass::Unknown_7863E0()
{
	if (Unknown2BC) {
		if (Unknown2BC->Is_Hidden()) {
			return(true);
		}
	}
	return(false);
}

// 0x783B50
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_1C4()
{
	// Per-frame avatar update. When walk-on completes (or if no animation assets exist in POC),
	// it calls 0x781EA0 which sets Unknown220 = true.
	if (Unknown208 == 0) {
		Unknown220 = true;
	}
}

void CharacterViewAvatarClass::Unknown_788EF0()
{
}

void CharacterViewAvatarClass::Unknown_7896B0(int a)
{
}
