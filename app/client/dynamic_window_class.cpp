// Earth & Beyond client (reconstruction): app\Client\dynamic_window_class.cpp (__FILE__ 0xB76A30). Notes at the end of the file.
#include "dynamic_window_class.h"
#include "..\shared\unknown_debug.h"

DynamicWindowClass::DynamicWindowClass(const char *name, bool unknown) :
	DynamicWindowBackgroundClass(name, 6),
	Rows(),
	RowElements(),
	Position(0.0f, 0.0f)
{
	Rows = Create_NEW(new DynamicWindowVerticalClass(6));
	Set_Contents(Rows);
}

DynamicWindowClass::~DynamicWindowClass()
{
	DynamicWindowBackgroundClass::Remove_All_Children(true);
}

void DynamicWindowClass::Remove_All(bool recursive)
{
	Remove_All_Children(recursive);
}






















void DynamicWindowClass::Add_Object(DynamicWindowElementClass *object, int row_index)
{
	FAIL_IF(!object) return;
	RefCountPtr<DynamicWindowElementClass> element = Create_Peek(object);
	Add_Element(element, row_index);
}


void DynamicWindowClass::Add_Object(DynamicWindowRenderObjectClass *object, int row_index)
{
	FAIL_IF(!object) return;
	RefCountPtr<DynamicWindowElementClass> element = Create_Peek((DynamicWindowElementClass *)object);
	Add_Element(element, row_index);
}


void DynamicWindowClass::Add_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index)
{
	FAIL_IF(!element) return;
	if (row_index < 0) {
		row_index += RowElements.size();
		if (row_index < 0) {
			row_index = 0;
		}
	}
	for (int i = RowElements.size(); i <= row_index; i++) {
		RefCountPtr<DynamicWindowHorizontalClass> row = Create_NEW(new DynamicWindowHorizontalClass(2));
		Rows->Insert_Element(row, i);
		RowElements.push_back(row);
	}
	RowElements[row_index]->Add_Element(element);
}












bool DynamicWindowClass::Remove_Object(DynamicWindowElementClass *object, int row_index)
{
	FAIL_IF(!object) return false;
	RefCountPtr<DynamicWindowElementClass> element = Create_Peek(object);
	return Remove_Element(element, row_index);
}


bool DynamicWindowClass::Remove_Object(DynamicWindowRenderObjectClass *object, int row_index)
{
	FAIL_IF(!object) return false;
	RefCountPtr<DynamicWindowElementClass> element = Create_Peek((DynamicWindowElementClass *)object);
	return Remove_Element(element, row_index);
}


bool DynamicWindowClass::Remove_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index)
{
	FAIL_IF(!element) return false;




	if (row_index < 0) {
		row_index += RowElements.size();
		FAIL_IF(row_index < 0) { row_index = 0; }
	}

	FAIL_IF((unsigned int)row_index >= RowElements.size()) return false;
	return RowElements[row_index]->Remove_Child(element);
}

void DynamicWindowClass::Layout_Size(const Vector2 &size)
{
	DynamicWindowBackgroundClass::Layout(Vector2(0.0f, 0.0f), size);
}

void DynamicWindowClass::Set_Center(Vector2 &center)
{
	if (Is_Dirty()) {
		Vector2 min_size = Get_Min_Size();
		Layout_Size(min_size);
	}
	Vector2 size = Get_Size();
	Vector2 position = center;
	position.X -= size.X * 0.5f;
	position.Y += size.Y * 0.5f;

	static Vector2 screen_size(640.0f, 480.0f);
	static float left = 5.0f - screen_size.X * 0.5f;
	static float right = screen_size.X * 0.5f - 5.0f;
	static float top = screen_size.Y * 0.5f - 5.0f;
	static float bottom = 5.0f - screen_size.Y * 0.5f;

	if (position.X < left) {
		position.X = left;
	} else if (position.X + size.X > right) {
		position.X = right - size.X;
	}
	if (position.Y > top) {
		position.Y = top;
	} else if (position.Y - size.Y < bottom) {
		position.Y = bottom + size.Y;
	}
	Set_Position(position);
	center.X += size.X * 0.5f;
	center.Y -= size.Y * 0.5f;
}
// NOT EXACT: the client stores X before computing Y (an aliasing form of the return not found yet)
Vector2 DynamicWindowClass::Get_Center()
{
	Vector2 size = Get_Size();
	Vector2 center;
	center.X = size.X * 0.5f + Position.X;
	center.Y = Position.Y - size.Y * 0.5f;
	return center;
}
// NOT EXACT: the inlined centre goes through a temporary; the client stores it in place
void DynamicWindowClass::Update_Layout()
{
	if (Is_Dirty()) {
		Vector2 size = Get_Size();
		Vector2 center(size.X * 0.5f + Position.X, Position.Y - size.Y * 0.5f);
		Vector2 min_size = Get_Min_Size();
		Layout_Size(min_size);
		Set_Center(center);
	}
}

void DynamicWindowClass::Set_Position(const Vector2 &position)
{
	Position = position;
	DynamicWindowBackgroundClass::Set_Position(position);
}

void DynamicWindowClass::Remove_All_Children(bool recursive)
{
	for (std::vector< RefCountPtr<DynamicWindowHorizontalClass> >::iterator it = RowElements.begin(); it != RowElements.end(); ++it) {
		(*it)->Remove_All_Children(recursive);
	}
	Rows->Remove_All_Children(true);
	RowElements.clear();
}

































bool DynamicWindowClass::Remove_Child(const RefCountPtr<DynamicWindowElementClass> &element)
{
	FAIL_IF(!element) return false;
	for (std::vector< RefCountPtr<DynamicWindowHorizontalClass> >::iterator it = RowElements.begin(); it != RowElements.end(); ++it) {
		if ((*it)->Remove_Child(element)) {
			return true;
		}
	}
	return false;
}

/*
**	Reconstruction notes (DW1, 2026-11-01)
**
**	Client 0x517240-0x5180E0: the constructor 0x517240 (the column 0x51EAE0 as the background's contents),
**	deleting destructor 0x517460, destructor 0x517490, Remove_All 0x517580, the row adders and removers
**	0x5175B0-0x517B40 (FAIL_IF '!object' on lines 48 / 56 / 92 / 100, '!element' on 64 / 108, 'row_index < 0'
**	on 115, '(unsigned int)row_index >= RowElements.size()' on 118), Layout_Size 0x517C10, Set_Center
**	0x517C50 (its statics 0xBE5A10-0xBE5A2C, guard 0xBE5A24, the screen size's atexit 0x517EC0), Get_Center
**	0x517EE0, Update_Layout 0x517F30, slot +0x54 0x517FD0, the container slots 0x518000 / 0x5180E0 (FAIL_IF
**	'!element' on 228). The direct calls of the background's Layout (0x516AE0), Set_Position (0x516C70) and
**	Remove_All_Children (0x516CF0, from the destructor) are qualified calls.
*/
