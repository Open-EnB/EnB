// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_render_object.h (name assumed from
**	dynamic_window_render_object.cpp, whose __FILE__ is 0xB76D68; the client holds no string of this header)
**
**	The dynamic windows' building blocks, declared as far as dynamic_window_background.cpp and
**	dynamic_window_class.cpp use them (DW1, 2026-11-01). Interim placement (D137): the element and the
**	container interface may sit in a header of their own; dynamic_window_render_object.cpp is the base's
**	file (0x51C930-0x51D240).
**
**	  DynamicWindowElementClass       a ref-counted piece of a window (24 slots; the tables 0xAED8D0,
**	                                  0xAED374, 0xAED40C, 0xAEDB84 share its layout)
**	  DynamicWindowContainerClass     the interface a parent gives its children (3 slots, pure: 0xAED3E8)
**	  DynamicWindowRenderObjectClass  an element shown by a render object (constructor 0x51C930, table
**	                                  0xAED8D0, 0x4C bytes; FAIL_IF '!RenderObject' on its lines 129 / 215)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_RENDER_OBJECT_H  // NAME 3 [dynamic_window_render_object.h]: header name assumed from the client's dynamic_window_render_object.cpp
#define DYNAMIC_WINDOW_RENDER_OBJECT_H

#include "..\..\wwlib\refcount.h"
#include "vector2.h"
#include "vector3.h"
#include "aabox.h"
#include "..\..\wwlib\ref_ptr.h"

class RenderObjClass;
class SceneClass;
class DynamicWindowElementClass;

// The parent side of an element (the background and the window: +0x4C; the rows and columns: +0x08)
class DynamicWindowContainerClass  // NAME 1: from the asserts' 'child' / 'parent' (0x516CA0, 0x51CD50)
{
public:
	virtual void Child_Changed(DynamicWindowElementClass &child) = 0;							// +0x00 (0x516CA0: 'Contents.Peek() == &child')  // NAME 1 [Child_Changed, child]: the assert's 'child'
	virtual void Remove_All_Children(bool recursive) = 0;										// +0x04 (0x516CF0)  // NAME 1 [Remove_All_Children, recursive]: what 0x516CF0 does
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child) = 0;		// +0x08 (0x516D70: FAIL_IF '!child')  // NAME 1 [Remove_Child]: the FAIL_IF's 'child'
};

// A piece of a window (abstract: 24 slots)
class DynamicWindowElementClass : public RefCountClass  // NAME 1: from the asserts' 'element' (0x517770, 0x5180E0)
{
public:
	virtual ~DynamicWindowElementClass() {}														// +0x04
	virtual void Set_Min_Size(const Vector2 &min_size) = 0;									// +0x08  // NAME 1 [Set_Min_Size, min_size]: 0x5168F0's 'contents_min_size'
	virtual void Set_Flags(unsigned int flags) = 0;												// +0x0C  // NAME 1 [Set_Flags, flags]: ORs into +0x3C
	virtual void Clear_Flags(unsigned int flags) = 0;											// +0x10  // NAME 1 [Clear_Flags, flags]: clears bits of +0x3C
	virtual void Set_Alignment(unsigned int flags) = 0;											// +0x14  // NAME 1 [Set_Alignment, flags]: replaces bits 0xF0 of +0x3C (0x516E00 reads 0x10-0x80 as alignment)
	virtual void Invalidate() = 0;																// +0x18  // NAME 1 [Invalidate]: sets the dirty flag and tells the parent
	virtual bool Is_Dirty() = 0;																// +0x1C  // NAME 1 [Is_Dirty]
	virtual Vector2 Get_Min_Size() = 0;															// +0x20  // NAME 1 [Get_Min_Size]: what 0x517C50 lays out with
	virtual bool Has_Flags(unsigned int flags) = 0;												// +0x24  // NAME 1 [Has_Flags, flags]
	virtual void Layout(const Vector2 &position, const Vector2 &size) = 0;						// +0x28  // NAME 1 [Layout, position, size]
	virtual Vector2 Get_Size() = 0;																// +0x2C  // NAME 1 [Get_Size]
	virtual AABoxClass Get_Bounds() = 0;														// +0x30 (0x51CCB0)  // NAME 1 [Get_Bounds]: the render object's position less the offsets, half the size as the extent
	virtual void Set_Parent(DynamicWindowContainerClass *parent) = 0;							// +0x34 (0x51CD50: G_ASSERT '!Parent', FAIL_IF '!parent')  // NAME 1 [Set_Parent, parent]: the asserts'
	virtual void Clear_Parent() = 0;															// +0x38 (0x51CDD0)  // NAME 1 [Clear_Parent]
	virtual DynamicWindowContainerClass *Get_Container() = 0;									// +0x3C  // NAME 1 [Get_Container]
	virtual void Add_To_Scene(SceneClass *scene) = 0;											// +0x40  // NAME 1 [Add_To_Scene, scene]: compares with the render object's Peek_Scene
	virtual void Remove_From_Scene() = 0;														// +0x44  // NAME 1 [Remove_From_Scene]
	virtual SceneClass *Peek_Scene() = 0;														// +0x48 (0x51CEC0: the render object's)  // NAME 1 [Peek_Scene]: RenderObjClass's
	virtual void Set_Level(int level) = 0;														// +0x4C  // NAME 1 [Set_Level, level]: the background passes one less to its contents
	virtual int Get_Level() = 0;																// +0x50  // NAME 1 [Get_Level]
	virtual void Set_Position(const Vector2 &position) = 0;										// +0x54  // NAME 1 [Set_Position, position]
	virtual void Set_Render_Object(const RefCountPtr<RenderObjClass> &robj) = 0;				// +0x58 (0x51CFD0)  // NAME 1 [Set_Render_Object, robj]
	virtual Vector2 Fit_Size(const Vector2 &position, const Vector2 &size) = 0;					// +0x5C  // NAME 1 [Fit_Size, position, size]: 0x51CC10 asks it where the content goes
};

// An element shown by a render object (0x4C bytes)
class DynamicWindowRenderObjectClass : public DynamicWindowElementClass  // NAME 2 [DynamicWindowRenderObjectClass]: class from file (D24), dynamic_window_render_object.cpp
{
public:
	DynamicWindowRenderObjectClass(unsigned int flags);											// 0x51C930  // NAME 1 [flags]: +0x3C
	virtual ~DynamicWindowRenderObjectClass();													// 0x51C9D0

	virtual void Set_Min_Size(const Vector2 &min_size);										// +0x08 0x51CA50
	virtual void Set_Flags(unsigned int flags);													// +0x0C 0x51CA80
	virtual void Clear_Flags(unsigned int flags);												// +0x10 0x51CAB0
	virtual void Set_Alignment(unsigned int flags);												// +0x14 0x51CAE0
	virtual void Invalidate();																	// +0x18 0x51CB20
	virtual bool Is_Dirty();																	// +0x1C 0x51CB50
	virtual Vector2 Get_Min_Size();																// +0x20 0x51CB70
	virtual bool Has_Flags(unsigned int flags);													// +0x24 0x51CBE0
	virtual void Layout(const Vector2 &position, const Vector2 &size);							// +0x28 0x51CC10
	virtual Vector2 Get_Size();																	// +0x2C 0x51CC80
	virtual AABoxClass Get_Bounds();															// +0x30 0x51CCB0 (FAIL_IF '!RenderObject')
	virtual void Set_Parent(DynamicWindowContainerClass *parent);								// +0x34 0x51CD50
	virtual void Clear_Parent();																// +0x38 0x51CDD0
	virtual DynamicWindowContainerClass *Get_Container();										// +0x3C 0x51CE20
	virtual void Add_To_Scene(SceneClass *scene);												// +0x40 0x51CE40
	virtual void Remove_From_Scene();															// +0x44 0x51CE90
	virtual SceneClass *Peek_Scene();															// +0x48 0x51CEC0
	virtual void Set_Level(int level);															// +0x4C 0x51CEE0
	virtual int Get_Level();																	// +0x50 0x51CF10
	virtual void Set_Position(const Vector2 &position);											// +0x54 0x51CF30
	virtual void Set_Render_Object(const RefCountPtr<RenderObjClass> &robj);					// +0x58 0x51CFD0
	virtual Vector2 Fit_Size(const Vector2 &position, const Vector2 &size);						// +0x5C 0x51D160

	void Replace_Render_Object(const RefCountPtr<RenderObjClass> &robj);						// 0x51D060 (keeps the old one's position)  // NAME 1 [Replace_Render_Object]
	void Update_Bounds(const RefCountPtr<RenderObjClass> &robj);								// 0x51D0D0 (from the model's object-space box)  // NAME 1 [Update_Bounds]

protected:
	RefCountPtr<RenderObjClass> RenderObject;													// +0x08 (FAIL_IF '!RenderObject')
	Vector2 MinSize;																			// +0x0C  // NAME 1 [MinSize]: Set_Min_Size's
	Vector2 NaturalSize;																		// +0x14  // NAME 1 [NaturalSize]: twice the model's box extent (0x51D0D0)
	Vector2 Size;																				// +0x1C  // NAME 1 [Size]: Get_Size's
	Vector3 Offset;																				// +0x24  // NAME 1 [Offset]: Set_Position adds it (the model's top-left corner, 0x51D0D0)
	Vector3 LayoutOffset;																		// +0x30  // NAME 1 [LayoutOffset]: Layout's Fit_Size result
	unsigned int Flags;																			// +0x3C  // NAME 1 [Flags]
	bool Dirty;																					// +0x40  // NAME 1 [Dirty]
	int Level;																					// +0x44  // NAME 1 [Level]
	DynamicWindowContainerClass *Parent;														// +0x48 (G_ASSERT '!Parent')
};

#endif
