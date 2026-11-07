// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gutter_data.h (name assumed from gutter_data.cpp)
**
**	A client object's gutter data (vtable 0xB06068, base radar_icon_data.cpp's 0x7BC040), bound to the
**	object through co_data.cpp; declared as far as co_data.cpp needs it (its constructor and size).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GUTTER_DATA_H  // NAME 3 [gutter_data.h]: header name assumed from the client's gutter_data.cpp
#define __GUTTER_DATA_H

#include "radar_icon_data.h"
#include "ref_ptr.h"

class FontSys;
class SimpleSceneClass;
class UIViewClass;
class UserClickIndicatorClass;
class Vector3;

class GutterDataClass : public RadarIconDataClass  // NAME 5: the client's typeid name .PAVGutterDataClass@@ (co_data.cpp's registry)
{
public:
	GutterDataClass(ClientObjectClass *co);			// 0x7B4F30 (gutter_data.cpp)
	virtual ~GutterDataClass();						// +0x04 0x7B50A0 (0x7B57D0)

	virtual bool Remove_From_Scene();				// +0x10 0x7B5A60  // NAME 2: what it does
	virtual RefCountPtr<RenderObjClass> Get_Render_Obj();	// +0x14 0x7B5AD0: the object's model  // NAME 2: what it returns
	virtual bool Add_To_Scene(const RefCountPtr<SimpleSceneClass> &scene);	// +0x18 0x7B58F0 (a new slot: the base's +0x0C takes a SceneClass)  // NAME 2: what it does

	void Set_Sort_Group(int unused, int group);		// 0x7B5B50 (0x7BB8E7)  // NAME 1 [Set_Sort_Group, unused, group]: what it does; the first is never read
	float Update(const Vector3 &pos, float size, bool on_screen, const RefCountPtr<SimpleSceneClass> &scene, float scale);	// 0x7B5BF0 (gutter_radar.cpp's 0x7B7040): the size back, clamped  // NAME 2 [Update, pos, size, on_screen, scene, scale]: what 0x7B7040 passes
	void Hide();									// 0x7B5FF0 (no caller)  // NAME 2: what it does
	bool Set_Click_Indicator(bool turn_on, UIViewClass *view);	// 0x7B6040 (the click indicator packet)  // NAME 2 [turn_on, view]: the packet's, the FAIL_IF's
	void Update_Indicator(const Vector3 &pos, float size);	// 0x7B6170 (gutter_radar.cpp's 0x7B7040)  // NAME 2 [Update_Indicator, pos, size]: what 0x7B7040 passes
	void Set_Scale(float scale);					// 0x7B61F0 (no caller; inlined in 0x7B5BF0)  // NAME 2: what it does

	RefCountPtr<FontSys> NameLabel;					// +0x1C ('RADAR_NAME'; target_reticle.cpp 0x7C5690: the label it places its text under)  // NAME 2: what it holds
	RefCountPtr<FontSys> GuildLabel;				// +0x20 (target_reticle.cpp 0x7C5690: used instead of +0x1C when its +0x300 is set)  // NAME 2: what it holds
	bool OnScreen;									// +0x24  // NAME 2: 0x7B5BF0's on_screen
	UserClickIndicatorClass *Indicator;				// +0x28  // NAME 2: what it holds

private:
	void Update_Name_Label(ClientObjectClass *co);	// 0x7B50D0  // NAME 2: what it does
	void Update_Guild_Label(ClientObjectClass *co);	// 0x7B54E0  // NAME 2: what it does
	void Remove_Labels_From_Scene();				// inlined in 0x7B57D0, 0x7B5A60, 0x7B5FF0 and 0x7B63C0  // NAME 2: what it does
	void Update_Labels();							// 0x7B63C0  // NAME 2: what it does
};

#endif //__GUTTER_DATA_H
