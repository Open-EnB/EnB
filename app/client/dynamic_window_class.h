// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_class.h (name assumed from
**	dynamic_window_class.cpp, whose __FILE__ is 0xB76A30; the client holds no string of this header)
**
**	DynamicWindowClass: a framed window holding rows of elements (table 0xAED40C, 24 slots; its container
**	side 0xAED3FC at +0x4C; 0x80 bytes; constructor 0x517240, destructor 0x517490). The message box
**	(0x587570-0x589D20) and the tooltips (0x6AB320-) build their windows on it.
**
**	The rows and the column that holds them are dynamic_window_horizontal.cpp's and
**	dynamic_window_vertical.cpp's (not rebuilt): declared here as far as this file uses them. Interim
**	placement (D137): they belong in headers of their own.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_CLASS_H  // NAME 3 [dynamic_window_class.h]: header name assumed from the client's dynamic_window_class.cpp
#define DYNAMIC_WINDOW_CLASS_H

#include "dynamic_window_background.h"
#include <vector>

// A row: elements side by side (table 0xAEDxxx, constructor 0x5186F0, 100 bytes; its container side at +0x08)
class DynamicWindowHorizontalClass : public DynamicWindowElementClass, public DynamicWindowContainerClass  // NAME 2 [DynamicWindowHorizontalClass]: class from file (D24), dynamic_window_horizontal.cpp
{
public:
	DynamicWindowHorizontalClass(unsigned char flags);											// 0x5186F0  // NAME 1 [flags]: neutral
	void Add_Element(const RefCountPtr<DynamicWindowElementClass> &element);					// 0x518B10  // NAME 1 [Add_Element, element]
	// the slots (not rebuilt; tables as the background's)
	virtual ~DynamicWindowHorizontalClass();
	virtual void Set_Min_Size(const Vector2 &min_size);
	virtual void Set_Flags(unsigned int flags);
	virtual void Clear_Flags(unsigned int flags);
	virtual void Set_Alignment(unsigned int flags);
	virtual void Invalidate();
	virtual bool Is_Dirty();
	virtual Vector2 Get_Min_Size();
	virtual bool Has_Flags(unsigned int flags);
	virtual void Layout(const Vector2 &position, const Vector2 &size);
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
	virtual void Set_Render_Object(const RefCountPtr<RenderObjClass> &robj);
	virtual Vector2 Fit_Size(const Vector2 &position, const Vector2 &size);
	virtual void Child_Changed(DynamicWindowElementClass &child);
	virtual void Remove_All_Children(bool recursive);
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child);

protected:
	unsigned char Unknown0C[0x58];													// +0x0C (the container side's table at +0x08; 100 bytes in all)  // NAME 1: neutral, not rebuilt
};

// A column: elements one above the other (table 0xAEDB84, constructor 0x51EAE0, 100 bytes; its container side at +0x08)
class DynamicWindowVerticalClass : public DynamicWindowElementClass, public DynamicWindowContainerClass  // NAME 2 [DynamicWindowVerticalClass]: class from file (D24), dynamic_window_vertical.cpp
{
public:
	DynamicWindowVerticalClass(unsigned char flags);											// 0x51EAE0  // NAME 1 [flags]: neutral
	void Insert_Element(const RefCountPtr<DynamicWindowElementClass> &element, int index);		// 0x51ECD0  // NAME 1 [Insert_Element, element, index]
	// the slots (not rebuilt; tables as the background's)
	virtual ~DynamicWindowVerticalClass();
	virtual void Set_Min_Size(const Vector2 &min_size);
	virtual void Set_Flags(unsigned int flags);
	virtual void Clear_Flags(unsigned int flags);
	virtual void Set_Alignment(unsigned int flags);
	virtual void Invalidate();
	virtual bool Is_Dirty();
	virtual Vector2 Get_Min_Size();
	virtual bool Has_Flags(unsigned int flags);
	virtual void Layout(const Vector2 &position, const Vector2 &size);
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
	virtual void Set_Render_Object(const RefCountPtr<RenderObjClass> &robj);
	virtual Vector2 Fit_Size(const Vector2 &position, const Vector2 &size);
	virtual void Child_Changed(DynamicWindowElementClass &child);
	virtual void Remove_All_Children(bool recursive);
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child);

protected:
	unsigned char Unknown0C[0x58];													// +0x0C (the container side's table at +0x08; 100 bytes in all)  // NAME 1: neutral, not rebuilt
};

class DynamicWindowClass : public DynamicWindowBackgroundClass  // NAME 1 [DynamicWindowClass]: from the file name dynamic_window_class.cpp
{
public:
	DynamicWindowClass(const char *name, bool unknown);											// 0x517240 (the callers pass true; not read)  // NAME 1 [name, unknown]: the dynawin.ini section; neutral
	virtual ~DynamicWindowClass();																// +0x04 0x517490 (deleting 0x517460)

	void Remove_All(bool recursive);															// 0x517580  // NAME 1 [Remove_All, recursive]
	void Add_Object(DynamicWindowElementClass *object, int row_index);							// 0x5175B0 (FAIL_IF '!object')  // NAME 1 [Add_Object, object, row_index]
	void Add_Object(DynamicWindowRenderObjectClass *object, int row_index);						// 0x517690 (FAIL_IF '!object')  // NAME 1 [the overload's type]: neutral (the same code)
	void Add_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index);	// 0x517770 (FAIL_IF '!element')  // NAME 1 [Add_Element, element, row_index]
	bool Remove_Object(DynamicWindowElementClass *object, int row_index);						// 0x517980 (FAIL_IF '!object')  // NAME 1 [Remove_Object]
	bool Remove_Object(DynamicWindowRenderObjectClass *object, int row_index);					// 0x517A60 (FAIL_IF '!object')  // NAME 1 [the overload's type]: neutral
	bool Remove_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index);	// 0x517B40 (FAIL_IF 'row_index < 0')  // NAME 1 [Remove_Element]
	void Layout_Size(const Vector2 &size);														// 0x517C10  // NAME 1 [Layout_Size, size]
	void Set_Center(Vector2 &center);															// 0x517C50 (kept on the 640 x 480 screen)  // NAME 1 [Set_Center, center]
	void Get_Center(Vector2 &center);															// 0x517EE0  // NAME 1 [Get_Center, center]
	void Update_Layout();																		// 0x517F30  // NAME 1 [Update_Layout]

	virtual void Set_Position(const Vector2 &position);											// +0x54 0x517FD0

	virtual void Remove_All_Children(bool recursive);											// +0x4C +0x04 0x518000
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child);			// +0x4C +0x08 0x5180E0 (FAIL_IF '!element')

protected:
	RefCountPtr<DynamicWindowVerticalClass> Rows;												// +0x64  // NAME 1 [Rows]
	std::vector< RefCountPtr<DynamicWindowHorizontalClass> > RowElements;						// +0x68 (FAIL_IF '... >= RowElements.size()')
	Vector2 Position;																			// +0x78  // NAME 1 [Position]: Set_Position's
};

#endif
