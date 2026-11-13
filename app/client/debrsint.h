// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\debrsint.h (PROVISIONAL, D137: debrsint.cpp's header; its __FILE__ 0xB79068)
*/
#ifndef DEBRSINT_H
#define DEBRSINT_H

#include "intrface.h"
#include "vector.h"
#include "rendobj.h"

class INIClass;
class SimpleSceneClass;
class TextureClass;
#include "vector3.h"

#include "debris.h"































// debrsint.cpp's interface (0xA4 bytes, tables 0xAEEB6C / 0xAEEB60 / 0xAEEB54): the main view's +0x138, over its
// +0x10C scene. It reads settings.ini's Debris00, Debris01... sections into one debris field each, shows the one
// the mode picks, and the main view forwards the debug keys and the field's settings to it (0x513CD0-0x513F30).
class DebrisInterfaceClass : public InterfaceClass	// NAME 3: class-from-file (D24) for debrsint.cpp, as backint.cpp's BackInterfaceClass
{
public:
	DebrisInterfaceClass(UIViewClass *view, SimpleSceneClass *scene, INIClass &ini);	// 0x53D250  // NAME 1 [view, scene, ini]: what 0x50C690 passes
	virtual ~DebrisInterfaceClass();					// 0x53D840 (deleting 0x53D7D0)
	virtual void Unknown_58();							// +0x58 0x53D960: the shown field into the scene, then the base's
	virtual void Unknown_5C();							// +0x5C 0x53D9A0: the base's, then the shown field out
	virtual void Unknown_60();							// +0x60 0x53D9D0 (empty)
	virtual bool Unknown_68(int key);					// +0x68 0x53D9F0: the debug keys 0-11 (G_ASSERTs on lines 198 / 223 / 246)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_6C(INIClass &ini);				// +0x6C 0x53DDE0: the fields back into Debris%02d  // NAME 1 [ini]: InterfaceClass's

	void Set_Mode(unsigned mode);						// 0x53DFB0 (SClient's DebrisMode, through the main view's 0x513CD0)  // NAME 2: what it sets; NAME 2 [mode]: DebrisMode
	void Unknown_53E030(float value);					// 0x53E030 (the shown field's +0xC8, 0..1)  // NAME 1 [value]: neutral
	float Unknown_53E0B0();								// 0x53E0B0 (the shown field's +0xC8)
	float Get_Spacing();								// 0x53E0E0  // NAME 2: the field it reads
	void Set_Spacing(float spacing);					// 0x53E110 (at least 1)  // NAME 2: the field it sets
	float Get_Size();									// 0x53E160  // NAME 2: the field it reads
	void Set_Size(float size);							// 0x53E190 (at least 0)  // NAME 2: the field it sets
	void Get_Color(Vector3 &color);						// 0x53E1E0  // NAME 2: the field it reads
	void Set_Color(const Vector3 &color);				// 0x53E220  // NAME 2: the field it sets
	float Get_Alpha();									// 0x53E260  // NAME 2: the field it reads
	void Set_Alpha(float alpha);						// 0x53E290  // NAME 2: the field it sets
	float Get_Min_Dist();								// 0x53E2C0  // NAME 2: the field it reads
	void Set_Min_Dist(float min_dist);					// 0x53E2F0 (at least 0; MaxDist kept above it)  // NAME 2: the field it sets
	float Get_Max_Dist();								// 0x53E3C0  // NAME 2: the field it reads
	void Set_Max_Dist(float max_dist);					// 0x53E3F0 (at least 1; above MinDist)  // NAME 2: the field it sets
	void Unknown_53E470(const Vector3 &offset);			// 0x53E470 (the main view's Update, view mode 5: the hours times the wind)  // NAME 1 [offset]: neutral
	float Get_Blur_Time();								// 0x53E4A0  // NAME 2: the field it reads
	void Set_Blur_Time(float blur_time);				// 0x53E4D0  // NAME 2: the field it sets

	DynamicVectorClass<DebrisFieldClass *> Fields;		// +0x84 (one per Debris%02d section)  // NAME 1: what it holds
	unsigned Mode;										// +0x9C the shown field (the main view's 0x513D00 returns it)  // NAME 2: SClient's DebrisMode
	int UnknownA0;										// +0xA0
};

#endif
