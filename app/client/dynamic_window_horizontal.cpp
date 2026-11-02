// Earth & Beyond client (reconstruction): app\Client\dynamic_window_horizontal.cpp (__FILE__ 0xB76AC4). Notes at the end of the file.
#include "dynamic_window_horizontal.h"
#include "aabox.h"
#include "..\shared\unknown_debug.h"
#include <algorithm>

DynamicWindowHorizontalClass::DynamicWindowHorizontalClass(unsigned int flags) :
	Elements(),
	MinSize(0.0f, 0.0f),
	ContentSize(0.0f, 0.0f),
	ContentDirty(false),
	FillXCount(0),
	FillYCount(0),
	Size(0.0f, 0.0f),
	Position(0.0f, 0.0f),
	LayoutPosition(0.0f, 0.0f),
	Flags(flags),
	Dirty(true),
	Scene(NULL),
	Level(0),
	Parent(NULL)
{
}

DynamicWindowHorizontalClass::~DynamicWindowHorizontalClass()
{
	Remove_All_Children(false);
	Elements.clear();
}




















void DynamicWindowHorizontalClass::Insert_Element(const RefCountPtr<DynamicWindowElementClass> &element, int index)
{
	FAIL_IF(!element) return;
	if ((unsigned int)index < Elements.size()) {
		RefCountPtr<DynamicWindowElementClass> &old_element = Elements[index];
		if (old_element) {
			old_element->Remove_From_Scene();
			old_element->Clear_Parent();
		}
	}
	if ((unsigned int)index >= Elements.size()) {
		Elements.resize(index + 1);
	}
	Elements[index] = element;
	if (Scene != NULL) {
		element->Add_To_Scene(Scene);
	}
	element->Set_Level(Level);
	element->Set_Parent(this);
	Invalidate();
}

void DynamicWindowHorizontalClass::Add_Element(const RefCountPtr<DynamicWindowElementClass> &element)
{
	Insert_Element(element, Elements.size());
}

void DynamicWindowHorizontalClass::Set_Min_Size(const Vector2 &min_size)
{
	MinSize = min_size;
	Invalidate();
}
// NOT EXACT: the client keeps the new flags in a second register (esi, then edx) for the compare
void DynamicWindowHorizontalClass::Set_Flags(unsigned int flags)
{
	unsigned int old_flags = Flags;
	unsigned int new_flags = old_flags | flags;
	Flags = new_flags;
	if (new_flags != old_flags) {
		Invalidate();
	}
}

void DynamicWindowHorizontalClass::Clear_Flags(unsigned int flags)
{
	unsigned int old_flags = Flags;
	Flags &= ~flags;
	if (Flags != old_flags) {
		Invalidate();
	}
}
// NOT EXACT: the client stores the cleared flags, reloads them and ORs in the new bits; this form folds both into one store
void DynamicWindowHorizontalClass::Set_Alignment(unsigned int flags)
{
	unsigned int old_flags = Flags;
	Flags &= ~0xF0;
	Flags |= flags & 0xF0;
	if (Flags != old_flags) {
		Invalidate();
	}
}

void DynamicWindowHorizontalClass::Invalidate()
{
	Dirty = true;
	ContentDirty = true;
	if (Parent != NULL) {
		Parent->Child_Changed(*this);
	}
}

bool DynamicWindowHorizontalClass::Is_Dirty()
{
	return Dirty;
}

Vector2 DynamicWindowHorizontalClass::Get_Min_Size()
{
	if (ContentDirty) {
		Update_Content_Size();
	}
	float x = max(MinSize.X, ContentSize.X);
	float y = max(MinSize.Y, ContentSize.Y);
	return Vector2(x, y);
}

bool DynamicWindowHorizontalClass::Has_Flags(unsigned int flags)
{
	return (Flags & flags) == flags;
}

void DynamicWindowHorizontalClass::Layout(const Vector2 &position, const Vector2 &size)
{
	LayoutPosition = position;
	Size = size;
	Vector2 min_size = Get_Min_Size();
	Vector2 excess_size = size - min_size;
















	FAIL_IF(excess_size.X < 0) { excess_size.X = 0; }
	FAIL_IF(excess_size.Y < 0) { excess_size.Y = 0; }
	if (FillXCount != 0) {
		excess_size.X /= FillXCount;
	}

	float width = 0.0f;
	float x = position.X;
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (!*it) {
			x += width;
			continue;
		}
		Vector2 element_position(x, position.Y);
		Vector2 element_size = (*it)->Get_Min_Size();
		width = element_size.X;
		if ((*it)->Has_Flags(2)) {
			element_size.X += excess_size.X;
		}
		float extra = size.Y - element_size.Y;
		if ((*it)->Has_Flags(4)) {
			element_size.Y = size.Y;
		} else if ((*it)->Has_Flags(0x80)) {
			element_position.Y -= extra;
		} else if (!(*it)->Has_Flags(0x40)) {
			element_position.Y -= extra * 0.5f;
		}
		(*it)->Layout(element_position, element_size);
		x += element_size.X;
	}
}

Vector2 DynamicWindowHorizontalClass::Get_Size()
{
	return Size;
}

void DynamicWindowHorizontalClass::Get_Bounds(AABoxClass &box)
{
	Vector2 position = Position + LayoutPosition;
	box.Extent.X = Size.X * 0.5f;
	box.Extent.Y = Size.Y * 0.5f;
	box.Extent.Z = 0.0f;
	box.Center.X = position.X + box.Extent.X;
	box.Center.Y = position.Y - box.Extent.Y;
}


















void DynamicWindowHorizontalClass::Set_Parent(DynamicWindowContainerClass *parent)
{
	G_ASSERT(!Parent);
	FAIL_IF(!parent) return;
	Parent = parent;
	Invalidate();
}



void DynamicWindowHorizontalClass::Clear_Parent()
{
	FAIL_IF(!Parent) return;
	Invalidate();
	Parent = NULL;
}

DynamicWindowContainerClass *DynamicWindowHorizontalClass::Get_Container()
{
	return this;
}

void DynamicWindowHorizontalClass::Add_To_Scene(SceneClass *scene)
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (it != NULL) {
			(*it)->Remove_From_Scene();
			(*it)->Add_To_Scene(scene);
		}
	}
}

void DynamicWindowHorizontalClass::Remove_From_Scene()
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (it != NULL) {
			(*it)->Remove_From_Scene();
		}
	}
}

SceneClass *DynamicWindowHorizontalClass::Peek_Scene()
{
	return Scene;
}

void DynamicWindowHorizontalClass::Set_Level(int level)
{
	Level = level;
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (it != NULL) {
			(*it)->Set_Level(level);
		}
	}
}

int DynamicWindowHorizontalClass::Get_Level()
{
	return Level;
}

void DynamicWindowHorizontalClass::Set_Position(const Vector2 &position)
{
	Position = position;
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (it != NULL) {
			(*it)->Set_Position(position);
		}
	}
}

void DynamicWindowHorizontalClass::Child_Changed(DynamicWindowElementClass &child)
{
	FillXCount = 0;
	FillYCount = 0;
	Flags &= ~6;
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (*it) {
			if ((*it)->Has_Flags(2) && ++FillXCount == 0) {
				Set_Flags(2);
			}
			if ((*it)->Has_Flags(4) && ++FillYCount == 0) {
				Set_Flags(4);
			}
		}
	}
	if (FillXCount != 0) {
		Flags |= 2;
	}
	if (FillYCount != 0) {
		Flags |= 4;
	}
	Invalidate();
}

void DynamicWindowHorizontalClass::Remove_All_Children(bool recursive)
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (it != NULL) {
			if (recursive) {
				DynamicWindowContainerClass *container = (*it)->Get_Container();
				if (container != NULL) {
					container->Remove_All_Children(recursive);
				}
			}
			(*it)->Remove_From_Scene();
			(*it)->Clear_Parent();
		}
	}
	Elements.clear();
	Invalidate();
}

bool DynamicWindowHorizontalClass::Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child)
{
	std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = std::find(Elements.begin(), Elements.end(), child);
	if (it == Elements.end()) {
		return false;
	}
	child->Remove_From_Scene();
	child->Clear_Parent();
	Invalidate();
	*it = NULL;
	return true;
}
// NOT EXACT: the client keeps the null element's self-assignment of the summed size (a load and a store); VC6 drops this one
void DynamicWindowHorizontalClass::Update_Content_Size()
{
	ContentSize.X = 0.0f;
	ContentSize.Y = 0.0f;
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (!*it) {
			ContentSize.X = ContentSize.X;
		} else {
			Vector2 element_size = (*it)->Get_Min_Size();
			ContentSize.X += element_size.X;
			if (element_size.Y > ContentSize.Y) {
				ContentSize.Y = element_size.Y;
			}
		}
	}
	ContentDirty = false;
}

/*
**	Reconstruction notes (DW1, 2026-11-01)
**
**	Client 0x5186F0-0x5194C5: the row, dynamic_window_vertical.cpp with X and Y exchanged (the same
**	lines). The constructor 0x5186F0 (table 0xAED490, container side 0xAED480), deleting destructor
**	0x518780, destructor 0x5187B0, Insert_Element 0x518900 (FAIL_IF '!element' on line 52), Add_Element
**	0x518B10, the slots 0x518B50-0x5191F0 (FAIL_IF 'excess_size.X < 0' on 163, G_ASSERT '!Parent' on 229,
**	FAIL_IF '!parent' on 230, FAIL_IF '!Parent' on 239), the container slots 0x519240 / 0x5192E0 / 0x5193D0
**	and Update_Content_Size 0x519460. The vector's template instances 0x519570-0x519A20 (size, resize's
**	insert and erase) follow.
*/
