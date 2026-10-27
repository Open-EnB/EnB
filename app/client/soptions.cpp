/*
**	Earth & Beyond client (reconstruction): e:\build\beta\G\app\Client\soptions.cpp (client 0x75A280-0x75AAB0)
**	SOptionsClass (soptions.h), SClient's options. Notes at the end.
*/
#include "soptions.h"
#include "registry.h"
#include "ini.h"
#include "ww3d.h"
#include "wwmath.h"
#include "..\shared\unknown_debug.h"

// 0x75A280
SOptionsClass::SOptionsClass() :
	Unknown04(false), Unknown05(false), Unknown06(true), PolyBudget(0), StarBudget(0), LandPolyBudget(0), LandLOD(0),
	TextureReduction(0), TextureFilter(0), Unknown20(false), Unknown24(2.0f), Unknown28(true), CameraMode(1),
	CameraModeCount(0), LastCameraMode(1)
{
	RegistryClass registry("Software\\Westwood Studios\\Earth and Beyond\\Render");
	PolyBudget = registry.Get_Int("PolyBudget", 10000);
	StarBudget = registry.Get_Int("StarBudget", 2000);
	LandPolyBudget = registry.Get_Int("LandPolyBudget", 10000);
	LandLOD = registry.Get_Int("LandLOD", 1);
	TextureReduction = registry.Get_Int("TextureReduction", 0);
	int filter = registry.Get_Int("TextureFilter", 0);
	PolyBudget = WWMath::Clamp_Int(PolyBudget, 8000, 40000);
	StarBudget = WWMath::Clamp_Int(StarBudget, 500, 2000);
	LandPolyBudget = WWMath::Clamp_Int(LandPolyBudget, 8000, 40000);
	LandLOD = WWMath::Clamp_Int(LandLOD, 0, 7);
	TextureReduction = WWMath::Clamp_Int(TextureReduction, 0, 3);
	WW3D::Set_Texture_Filter(WWMath::Clamp_Int(filter, 0, 2));
}

// 0x75A4E0
SOptionsClass::~SOptionsClass()
{
}












// 0x75A530
void SOptionsClass::Set_Camera_Mode(unsigned cameramode)
{
	if (cameramode >= CameraModeCount) {
		G_ASSERT(cameramode >=0 && cameramode < CameraModeCount);
		cameramode = 0;
	}
	CameraMode = cameramode;
	if (cameramode != 0) {
		LastCameraMode = cameramode;
	}
}

// 0x75A570
void SOptionsClass::Unknown_75A570()
{
	unsigned mode = CameraMode + 1;
	if (mode >= CameraModeCount) {
		mode = 1;
	}
	Set_Camera_Mode(mode);
}

// 0x75A5A0
void SOptionsClass::Unknown_75A5A0()
{
	if (CameraMode <= 1) {
		Set_Camera_Mode(CameraModeCount - 1);
	} else {
		Set_Camera_Mode(CameraMode - 1);
	}
}

// 0x75A5D0
void SOptionsClass::Unknown_75A5D0()
{
	Set_Camera_Mode(LastCameraMode);
}



// 0x75A5F0
void SOptionsClass::Load_Camera_Mode(INIClass &ini, const char *section)
{
	G_ASSERT(CameraModeCount <= (unsigned int)CameraModes.Length());
	if (CameraModeCount >= (unsigned int)CameraModes.Length()) {
		CameraModes.Resize(CameraModeCount + 10);
	}
	CameraModes[CameraModeCount].Heading = DEG_TO_RAD(ini.Get_Float(section, "Heading", 0));
	CameraModes[CameraModeCount].Distance = ini.Get_Float(section, "Distance", 0);
	CameraModes[CameraModeCount].Tilt = DEG_TO_RAD(ini.Get_Float(section, "Tilt", 0));
	CameraModes[CameraModeCount].FOV = DEG_TO_RAD(ini.Get_Float(section, "FOV", 90.0f));
	CameraModes[CameraModeCount].ElevationOffset = DEG_TO_RAD(ini.Get_Float(section, "ElevationOffset", 0));
	CameraModeCount++;
}

/*
**	Reconstruction notes
**
**	soptions.cpp (D90 9d): the constructor reads the render budgets from the registry key (RegistryClass
**	with its default create flag, as the client's push 1), clamps them and sets WW3D's texture filter;
**	'TextureFilter' is read into a local, not the member. The camera modes come from the INI sections
**	SClient::Init loads (Load_Camera_Mode); 0x75A570 / 0x75A5A0 step to the next and previous mode (mode 0 is
**	skipped going up), 0x75A5D0 returns to the last mode that wasn't 0. VectorClass<CameraModeStruct>'s
**	members are emitted here (0x75A750-0x75AA40). The client's __FILE__ has 'Client' (D41: the string
**	differs in case only, the bytes don't). Never run in a program that writes the registry (D69).
*/
