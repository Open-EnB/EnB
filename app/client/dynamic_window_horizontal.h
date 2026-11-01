// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_horizontal.h (name assumed from
**	dynamic_window_horizontal.cpp, whose __FILE__ is 0xB76AC4; the client holds no string of this header)
**
**	A row: elements side by side (table 0xAED490, its container side 0xAED480 at +0x08; 100 bytes; constructor 0x5186F0,
**	destructor 0x5187B0). It has 22 element slots: no render object of its own.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_HORIZONTAL_H  // NAME 3 [dynamic_window_horizontal.h]: header name assumed from the client's dynamic_window_horizontal.cpp
#define DYNAMIC_WINDOW_HORIZONTAL_H

#include "dynamic_window_render_object.h"
#include <vector>

class DynamicWindowHorizontalClass : public DynamicWindowElementClass, public DynamicWindowContainerClass  // NAME 2 [DynamicWindowHorizontalClass]: class from file (D24), dynamic_window_horizontal.cpp
{
public:
	DynamicWindowHorizontalClass(unsigned int flags);											// 0x5186F0  // NAME 1 [flags]: +0x50
	virtual ~DynamicWindowHorizontalClass();													// +0x04 0x5187B0

	void Insert_Element(const RefCountPtr<DynamicWindowElementClass> &element, int index);	// 0x518900 (FAIL_IF '!element')  // NAME 1 [Insert_Element, element, index]
	void Add_Element(const RefCountPtr<DynamicWindowElementClass> &element);				// 0x518B10 (at the end)  // NAME 1 [Add_Element, element]

	virtual void Set_Min_Size(const Vector2 &min_size);
	virtual void Set_Flags(unsigned int flags);
	virtual void Clear_Flags(unsigned int flags);
	virtual void Set_Alignment(unsigned int flags);
	virtual void Invalidate();
	virtual bool Is_Dirty();
	virtual Vector2 Get_Min_Size();
	virtual bool Has_Flags(unsigned int flags);
	virtual void Layout(const Vector2 &position, const Vector2 &size);						// (FAIL_IF 'excess_size.X < 0')
	virtual Vector2 Get_Size();
	virtual void Get_Bounds(AABoxClass &box);
	virtual void Set_Parent(DynamicWindowContainerClass *parent);
	virtual void Clear_Parent();
	virtual DynamicWindowContainerClass *Get_Container();
	virtual void Add_To_Scene(SceneClass *scene);
	virtual void Remove_From_Scene();
	virtual SceneClass *Peek_Scene();
	virtual void Set_Level(int level);
	virtual int Get_Level();
	virtual void Set_Position(const Vector2 &position);

	virtual void Child_Changed(DynamicWindowElementClass &child);							// +0x08 +0x00
	virtual void Remove_All_Children(bool recursive);										// +0x08 +0x04
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child);		// +0x08 +0x08

protected:
	void Update_Content_Size();																// 0x519460  // NAME 1 [Update_Content_Size]

	std::vector< RefCountPtr<DynamicWindowElementClass> > Elements;						// +0x0C  // NAME 1 [Elements]
	Vector2 MinSize;																		// +0x1C  // NAME 1 [MinSize]: Set_Min_Size's
	Vector2 ContentSize;																	// +0x24  // NAME 1 [ContentSize]: the widths summed, the tallest child
	bool ContentDirty;																		// +0x2C  // NAME 1 [ContentDirty]
	int FillXCount;																			// +0x30 (children with flag 2)  // NAME 1 [FillXCount]
	int FillYCount;																			// +0x34 (children with flag 4)  // NAME 1 [FillYCount]
	Vector2 Size;																			// +0x38  // NAME 1 [Size]: Layout's
	Vector2 Position;																		// +0x40  // NAME 1 [Position]: Set_Position's
	Vector2 LayoutPosition;																	// +0x48  // NAME 1 [LayoutPosition]: Layout's
	unsigned int Flags;																		// +0x50  // NAME 1 [Flags]
	bool Dirty;																				// +0x54  // NAME 1 [Dirty]
	SceneClass *Scene;																		// +0x58  // NAME 1 [Scene]: Peek_Scene's
	int Level;																				// +0x5C  // NAME 1 [Level]
	DynamicWindowContainerClass *Parent;													// +0x60 (G_ASSERT '!Parent')
};

#endif
