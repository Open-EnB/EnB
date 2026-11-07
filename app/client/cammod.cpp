// Earth & Beyond client (reconstruction): app\client\cammod.cpp, PROVISIONAL file name (D137; notes at the end)
#include "cammod.h"
#include "unknown_signalbase.h"

// 0x79B9E0
SignalMap &CameraModuleClass::Get_Signal_Map()
{
	static SignalMap map;
	return(map);
}

// 0x79BA50
void CameraModuleClass::Unknown_04()
{
	Get_Signal_Map().emit(this, CameraModuleSignal(true));
}

// 0x79BAD0
void CameraModuleClass::Unknown_0C()
{
	Get_Signal_Map().emit(this, CameraModuleSignal(false));
}

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object 0x79B9E0-0x79BB6F: after camera_manager.cpp's <string> initialiser 0x79B9B0, ending with its own at
** 0x79BB50, before cammod_chase.cpp's constructor 0x79BB80. No __FILE__: the name is PROVISIONAL (D137). The two
** emits are the base's slots +0x04 / +0x0C (table 0xB054F8); ChaseCameraModuleClass's overrides 0x79BCE0 / 0x79BDC0
** call them. 0x79BA30 is the static's atexit destructor.
*/
