// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\orbitalmanager.h (PROVISIONAL, D137: orbitalmanager.cpp's header;
**	its __FILE__ 0xB759A0 is e:\build\beta\G\app\shared\orbitalmanager.cpp)
*/
#ifndef ORBITALMANAGER_H
#define ORBITALMANAGER_H

#include <vector>
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

class SClient;
class ClientClass;
class CameraClass;
class INIClass;
class Curve1DClass;
class RenderObjClass;
class MainViewOrbitalCallbackClass;
class OrbitalObject;

// the sector's orbitals (planets, suns): their objects in the orbital scene, the sun and sky light they give, the
// sector's light rig from light_sector%04d.ini (the main view's +0x140, 0x1D4 bytes; constructor 0x4F3250)
class OrbitalManager		// NAME 2: the client's (OrbitalManager::Update)
{
public:
	OrbitalManager(SClient *client, MainViewOrbitalCallbackClass *callback);	// 0x4F3250 (0x1D4 bytes; the main view's 0x50BA60)  // NAME 1 [client, callback]: what 0x50BA60 passes
	~OrbitalManager();									// 0x4F4FD0 (the main view's Shutdown 0x50C2F0)
	void Unknown_4F3310();								// 0x4F3310 (the orbitals out of the scene, the light back to none)
	void Unknown_4F34C0(INIClass &ini);				// 0x4F34C0 (0x513700: orbital%04d.ini when it loaded)  // NAME 1 [ini]: what it gets
	bool Unknown_4F37C0(INIClass *ini);				// 0x4F37C0 (light_sector%04d.ini, else ini's [LIGHTS] / [PICKER])  // NAME 1 [ini]: neutral
	void Unknown_4F38E0(INIClass &ini);				// 0x4F38E0 ([LIGHTS] and [PICKER])  // NAME 1 [ini]: neutral
	void Unknown_4F3EF0();								// 0x4F3EF0 (writes them to light_sector%04d.ini: the light editor 0x586800)
	float Get_Unknown54() const { return(Unknown54); }	// (inline: the main view's 0x50CCF0 copies it through the FPU)  // NAME 1: neutral (D10): the member it returns

	void Update(CameraClass *camera, float time, bool planet);	// 0x4F45A0 (the main view's Update unless its +0x168 is set: the hours, view mode 1)  // NAME 1 [camera, time, planet]: what Update passes
	float Unknown_4F5090(RenderObjClass *obj);		// 0x4F5090 (an orbital's +0x6C, 1 when obj is none of them)  // NAME 1 [obj]: neutral
	const Vector4 &Unknown_4F50D0(int index);			// 0x4F50D0 (Update, view mode 0: light index's colour and intensity, 4 the fill)  // NAME 1 [index]: neutral
	void Unknown_4F5100(int index, const Vector4 &value);	// 0x4F5100  // NAME 1 [index, value]: neutral
	const Vector3 &Unknown_4F5140(int index);			// 0x4F5140 (Update, view mode 0: light index's offset)  // NAME 1 [index]: neutral
	void Unknown_4F5160(int index, const Vector3 &value);	// 0x4F5160  // NAME 1 [index, value]: neutral
	const Vector3 &Unknown_4F51A0(int index);			// 0x4F51A0 ([PICKER])  // NAME 1 [index]: neutral
	void Unknown_4F51C0(int index, const Vector3 &value);	// 0x4F51C0  // NAME 1 [index, value]: neutral

	std::vector<OrbitalObject *> Orbitals;				// +0x00  // NAME 1: neutral (D10)
	ClientClass *Client;								// +0x10  // NAME 2: what the constructor gets
	Vector3 Unknown14;									// +0x14 (Update: the sun's colour, the land's LightColor)
	Vector3 Unknown20;									// +0x20 (Update: the sun's direction, the land's SunDirection)
	float Unknown2C;									// +0x2C (Update: the land's SunAngle)
	float Unknown30;									// +0x30 (Update: the land's ShadowBias)
	Vector3 Unknown34;									// +0x34 (Update: the fog colour)
	Vector3 Unknown40;									// +0x40 (Update: the lights' sum)
	bool Unknown4C;										// +0x4C (Update: the sun light is added; the land's Valid)
	float Unknown50;									// +0x50 (the main view's 0x50CCF0: the near plane at a unit FOV)
	float Unknown54;									// +0x54 (0x50CCF0: the far plane)
	Vector3 Unknown58;									// +0x58 (Update: the ambient light)
	Curve1DClass *SkyColor[3];							// +0x64 ([ORBITALS] SKYCOLOR)  // NAME 2: the INI key
	Curve1DClass *LightColor[3];						// +0x70 ([ORBITALS] LIGHTCOLOR)  // NAME 2: the INI key
	Curve1DClass *SkyEmissive[3];						// +0x7C ([ORBITALS] SKYEMISSIVE)  // NAME 2: the INI key
	Vector4 Unknown88[5];								// +0x88 ([LIGHTS] KEY, RKEY, CAMERA, RCAMERA, AMBIENT: R, G, B, I)
	Vector3 UnknownD8[5];								// +0xD8 ([LIGHTS] KEY, RKEY, CAMERA, RCAMERA: X, Y, Z)
	Vector3 Unknown114[5];								// +0x114 ([PICKER] PK, PRK, PC, PRC, PA: X, Y, Z)
	Vector2 Unknown150[16];								// +0x150 (QMC samples over the sun's sphere)
	MainViewOrbitalCallbackClass *ViewInterface;		// +0x1D0  // NAME 2: the client's assert 'ViewInterface'
};

#endif
