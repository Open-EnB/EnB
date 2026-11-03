// Earth & Beyond client (reconstruction): app\Client\ui_registration.cpp. Notes at the end of the file.
#include "ui_registration.h"
#include "..\..\wwlib\registry.h"

static const char *RegistrationKey = "Software\\Westwood Studios\\Earth and Beyond\\Registration";  // NAME 1 [RegistrationKey]: client .data 0xB85EFC

// @0x5FAD50
bool Unknown_5FAD50()
{
	RegistryClass reg(RegistrationKey, true);
	return reg.Get_Bool("Registered", false);
}

// @0x5FADE0
bool Unknown_5FADE0()
{
	RegistryClass reg(RegistrationKey, true);
	return reg.Get_Bool("Trial", false);
}

/*
**	Reconstruction notes (builder ureg, 2026-11-02)
**
**	ui_registration.cpp (the client's object: asserts lines 369-3637; client 0x5EF0B0-0x5FBD30+).
**	0x5FAD50 and 0x5FADE0: two demo/registration predicates (0x90 bytes each with padding, 103 bytes code)
**	called from WinMain (0x526DE0) and menu/login tasks. Both open RegistryClass on RegistrationKey
**	("Software\\Westwood Studios\\Earth and Beyond\\Registration", client .data 0xB85EFC) and read
**	"Registered" and "Trial" bools respectively. Both are byte-exact with VC6 SP4.
*/
