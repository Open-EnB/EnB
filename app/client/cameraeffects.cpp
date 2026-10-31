// Earth & Beyond client (reconstruction): app\client\cameraeffects.cpp. Notes at the end of the file.
#include "co.h"
#include "sclient.h"
#include "ww3d.h"
#include "random.h"

// The file's own generator (0x6E2B90 constructs it at 0xBE67E8; only this file's effects call it)
static Random2Class CameraEffectsRandom;	// NAME 1: neutral (D10)

// The effects' base (table 0xAFFE6C, one slot: the shared return-false copy 0x9CCD10); each effect
// registers itself with the camera effects object it gets (that object's slot +0x10)
class UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	UnknownAFFE6CClass(CameraEffectsClass *owner) : Owner(owner) {}	// NAME 1 [owner]: neutral
	virtual bool Unknown_00() { return(false); }	// +0x00 (true while the effect still moves the camera)
	CameraEffectsClass *Owner;						// +0x04
};

// The shake 0x6E2CC0 makes (0x24 bytes, table 0xAFFE64): the shake's force, ramped up and down
class UnknownAFFE64Class : public UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	virtual bool Unknown_00();						// +0x00 0x6E3710
	Vector3 Unknown08;								// +0x08 (the force, 'Shake.ForceX'-'Z')
	Unknown498C10Class Unknown14;					// +0x14
};

// The shield hit's jitter (0x1C bytes, table 0xAFFE84; constructor 0x6E37F0, not rebuilt)
class UnknownAFFE84Class : public UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	virtual bool Unknown_00();						// +0x00 0x6E3B60
	float Unknown08;								// +0x08
	Unknown498C10Class Unknown0C;					// +0x0C
};

// 0x6E3710
bool UnknownAFFE64Class::Unknown_00()
{
	float value = Unknown14.Unknown_Value(WW3D::Get_Frame_Time());
	if (value >= 1.0f) {
		return(false);
	}
	if (value <= 0.33f) {
		value = value * 3.030303f;
	} else {
		value = 1.0 - (value - 0.33f) * 1.4925374f;
	}
	value = value * 10.0f;
	Owner->Unknown18.X += value * Unknown08.X;
	Owner->Unknown18.Y += value * Unknown08.Y;
	Owner->Unknown18.Z += value * Unknown08.Z;
	Owner->Unknown24 = true;
	return(true);
}

// 0x6E3B60
bool UnknownAFFE84Class::Unknown_00()
{
	float value = Unknown0C.Unknown_Value(WW3D::Get_Frame_Time());
	if (value <= 0.0f) {
		return(false);
	}
	float amount = value * Unknown08;
	Owner->Unknown18.X += CameraEffectsRandom(0, 100) * amount;
	Owner->Unknown18.Y += CameraEffectsRandom(0, 100) * amount;
	Owner->Unknown18.Z += CameraEffectsRandom(0, 100) * amount;
	Owner->Unknown24 = true;
	return(true);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\cameraeffects.cpp (its __FILE__, 0x6E32E0's assert)
**
**	The camera effects, client 0x6E2B40-0x6E46A0 (eff_attach.cpp's static initialiser 0x6E46C0 after it):
**	so far the two effects' slot 0x6E3710 and 0x6E3B60 (TG4, claim #62). The file's evidence: the
**	effects' tables 0xAFFE64 / 0xAFFE6C / 0xAFFE84 sit between the floats 0xAFFE60-0xAFFE80 that
**	0x6E2CC0 and 0x6E37F0 read; 0x6E2CC0 and 0x6E37F0 (called by 0x6E32E0, the assert's) store them;
**	the generator 0xBE67E8 is constructed by the initialiser at 0x6E2B90 and called only from
**	0x6E3B60, 0x6E3D30 and 0x6E3F00. Not yet rebuilt: CameraEffectsClass's own functions
**	(constructor 0x6E2BD0, 0x6E2C40, 0x6E2CC0, 0x6E32E0, 0x6E3590), the jitter's constructor 0x6E37F0
**	(the shield lookups 'MaxShieldPower' / 'ShieldPercent', co.cpp's form), the effects 0x6E3C70-0x6E3F00.
*/
