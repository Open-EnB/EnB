// Earth & Beyond client (reconstruction): the proof-of-concept client's CharacterViewClass (D179). NOT THE CLIENT'S CODE.
/*
**	charview.cpp's view (0x678450, 0x514 bytes; tsk_login.h's declaration) as an empty stand-in: it builds
**	no screen, so the login task runs on without one. The real one (the constructor 0x678450 and the screen
**	builder 0x679160 that the login, character select and creation screens hang off) is the POC's next item
**	after this (.work/poc/runs.md). Delete this file when charview.cpp lands in app/ (the linker reports
**	the duplicates).
*/



#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\tasks\tsk_login.h"

// 0x678450
CharacterViewClass::CharacterViewClass(LoginTaskClass *task, int step)
{
	STUB_CALLED("POC CharacterViewClass (0x678450, charview.cpp): an empty view, no screen");
	Unknown004 = new LoginUnknown0CClass;		// the view's input dispatcher (Pre_Run hands it the events the task does not take)
	memset(Unknown008, 0, sizeof(Unknown008));
	Unknown02C = NULL;
	Unknown034 = NULL;
	Unknown1B4 = NULL;
	Unknown1CD = false;
	Unknown1CE = false;
	Unknown1CF = false;
	Unknown1D0 = false;
}

CharacterViewClass::~CharacterViewClass() {}
void CharacterViewClass::Unknown_04() { STUB_CALLED("POC CharacterViewClass::Unknown_04"); }
void CharacterViewClass::Unknown_08() { STUB_CALLED("POC CharacterViewClass::Unknown_08"); }
void CharacterViewClass::Unknown_0C(bool on) { STUB_CALLED("POC CharacterViewClass::Unknown_0C (LoginTaskClass::Render)"); }
void CharacterViewClass::Unknown_10() { STUB_CALLED("POC CharacterViewClass::Unknown_10"); }
void CharacterViewClass::Unknown_14() { STUB_CALLED("POC CharacterViewClass::Unknown_14"); }
void CharacterViewClass::Unknown_18(int unknown) { STUB_CALLED("POC CharacterViewClass::Unknown_18"); }
void CharacterViewClass::Unknown_1C() { STUB_CALLED("POC CharacterViewClass::Unknown_1C"); }
void CharacterViewClass::Unknown_20() { STUB_CALLED("POC CharacterViewClass::Unknown_20"); }
void CharacterViewClass::Unknown_24() { STUB_CALLED("POC CharacterViewClass::Unknown_24"); }
void CharacterViewClass::Unknown_28() { STUB_CALLED("POC CharacterViewClass::Unknown_28"); }
void CharacterViewClass::Unknown_2C() { STUB_CALLED("POC CharacterViewClass::Unknown_2C"); }
void CharacterViewClass::Unknown_30() { STUB_CALLED("POC CharacterViewClass::Unknown_30"); }
void CharacterViewClass::Unknown_34() { STUB_CALLED("POC CharacterViewClass::Unknown_34"); }
void CharacterViewClass::Unknown_38() { STUB_CALLED("POC CharacterViewClass::Unknown_38"); }
void *CharacterViewClass::Unknown_3C() { STUB_CALLED("POC CharacterViewClass::Unknown_3C: NULL"); return NULL; }
void CharacterViewClass::Unknown_67A300() { STUB_CALLED("POC CharacterViewClass::Unknown_67A300 (0x67A300)"); }
void CharacterViewClass::Unknown_67A370(int step) { STUB_CALLED("POC CharacterViewClass::Unknown_67A370 (0x67A370): the step's screen (none)"); }
