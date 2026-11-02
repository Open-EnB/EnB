// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_background.h (name assumed from
**	dynamic_window_background.cpp, whose __FILE__ is 0xB768D0; the client holds no string of this header)
**
**	A window's frame: an animated model whose two animations (AnimX / AnimY in dynawin.ini) stretch it
**	to the size of its contents (table 0xAED374, its container side 0xAED364 at +0x4C; 0x64 bytes;
**	constructor 0x516250, destructor 0x516680). The dynawin.ini entry it reads is
**	DynamicWindowAnimationDataClass (table 0xAED358, 0x50 bytes).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_BACKGROUND_H  // NAME 3 [dynamic_window_background.h]: header name assumed from the client's dynamic_window_background.cpp
#define DYNAMIC_WINDOW_BACKGROUND_H

#include "dynamic_window_render_object.h"
#include "vector2i.h"
#include <string>

class INIClass;
class HAnimComboClass;
class HAnimComboDataClass;

// One dynawin.ini section (0x50 bytes)
class DynamicWindowAnimationDataClass : public RefCountClass  // NAME 1: from the assert's 'AnimationData' (0x516250)
{
public:
	DynamicWindowAnimationDataClass() :
		AnimMin(0, 0),
		AnimMax(0, 0),
		Border(0.0f, 0.0f),
		MeshName(""),
		AnimXName(""),
		AnimYName("")
	{
	}
	virtual ~DynamicWindowAnimationDataClass() {}												// +0x04 0x515E20 (deleting 0x515DF0)

	void Load(INIClass &ini, const char *section);												// 0x515F00 (G_ASSERT 'AnimMin.I >= 0' ...)  // NAME 1 [Load, ini, section]
	static RefCountPtr<DynamicWindowAnimationDataClass> Get(const std::string &name);			// 0x515AB0 (dynawin.ini, its 'Use' entry inherits)  // NAME 1 [Get, name]

	Vector2i AnimMin;																			// +0x08 (MinX / MinY)
	Vector2i AnimMax;																			// +0x10 (MaxX / MaxY)
	Vector2 Border;																				// +0x18 (BorderX / BorderY)  // NAME 1 [Border]: the keys'
	std::string MeshName;																		// +0x20 (MeshFile)
	std::string AnimXName;																		// +0x30 (AnimX)  // NAME 1 [AnimXName]: the key's
	std::string AnimYName;																		// +0x40 (AnimY)  // NAME 1 [AnimYName]: the key's
};

class DynamicWindowBackgroundClass : public DynamicWindowRenderObjectClass, public DynamicWindowContainerClass  // NAME 2 [DynamicWindowBackgroundClass]: class from file (D24)
{
public:
	DynamicWindowBackgroundClass(const std::string &name, unsigned int flags);					// 0x516250  // NAME 1 [name, flags]: the dynawin.ini section, the base's flags
	virtual ~DynamicWindowBackgroundClass();													// +0x04 0x516680 (deleting 0x516650)

	void Set_Contents(const RefCountPtr<DynamicWindowElementClass> &contents);					// 0x516810  // NAME 1 [Set_Contents, contents]: 'Contents'

	virtual void Set_Min_Size(const Vector2 &min_size);										// +0x08 0x5168F0 (FAIL_IF 'contents_min_size.X <= 0.0f')
	virtual Vector2 Get_Min_Size();																// +0x20 0x516A00
	virtual void Layout(const Vector2 &position, const Vector2 &size);							// +0x28 0x516AE0
	virtual DynamicWindowContainerClass *Get_Container();										// +0x3C 0x516BB0
	virtual void Add_To_Scene(SceneClass *scene);												// +0x40 0x516BD0
	virtual void Remove_From_Scene();															// +0x44 0x516C00
	virtual void Set_Level(int level);															// +0x4C 0x516C30
	virtual void Set_Position(const Vector2 &position);											// +0x54 0x516C70
	virtual Vector2 Fit_Size(const Vector2 &position, const Vector2 &size);						// +0x5C 0x516E00

	virtual void Child_Changed(DynamicWindowElementClass &child);								// +0x4C +0x00 0x516CA0
	virtual void Remove_All_Children(bool recursive);											// +0x4C +0x04 0x516CF0
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child);			// +0x4C +0x08 0x516D70

protected:
	RefCountPtr<DynamicWindowElementClass> Contents;											// +0x50 (G_ASSERT 'Contents.Peek() == &child')
	HAnimComboClass *AnimCombo;																	// +0x54  // NAME 1 [AnimCombo]
	HAnimComboDataClass *AnimX;																	// +0x58  // NAME 1 [AnimX]: dynawin.ini's key
	HAnimComboDataClass *AnimY;																	// +0x5C  // NAME 1 [AnimY]: dynawin.ini's key
	RefCountPtr<DynamicWindowAnimationDataClass> AnimationData;									// +0x60 (G_ASSERT 'AnimationData && ...')
};

#endif
