// Earth & Beyond client (reconstruction): app\Client\dynamic_window_render_object.cpp (__FILE__ 0xB76D68). Notes at the end of the file.
#include "dynamic_window_render_object.h"
#include "rendobj.h"
#include "wwmath.h"
#include "..\shared\unknown_debug.h"

void Unknown_496730(RenderObjClass *robj, int a, int b);	// 0x496730 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, a, b]: ui_cpit.cpp's

DynamicWindowRenderObjectClass::DynamicWindowRenderObjectClass(unsigned int flags) :
	RenderObject(),
	MinSize(0.0f, 0.0f),
	NaturalSize(0.0f, 0.0f),
	Size(0.0f, 0.0f),
	Offset(0.0f, 0.0f, 0.0f),
	LayoutOffset(0.0f, 0.0f, 0.0f),
	Flags(flags),
	Dirty(true),
	Level(0),
	Parent(NULL)
{
}

DynamicWindowRenderObjectClass::~DynamicWindowRenderObjectClass()
{
}

void DynamicWindowRenderObjectClass::Set_Min_Size(const Vector2 &min_size)
{
	MinSize = min_size;
	Invalidate();
}
// NOT EXACT: the client keeps the new flags in a second register (esi, then edx) for the compare
void DynamicWindowRenderObjectClass::Set_Flags(unsigned int flags)
{
	unsigned int old_flags = Flags;
	unsigned int new_flags = old_flags | flags;
	Flags = new_flags;
	if (new_flags != old_flags) {
		Invalidate();
	}
}

void DynamicWindowRenderObjectClass::Clear_Flags(unsigned int flags)
{
	unsigned int old_flags = Flags;
	Flags &= ~flags;
	if (Flags != old_flags) {
		Invalidate();
	}
}
// NOT EXACT: the client stores the cleared flags, reloads them and ORs in the new bits; this form folds both into one store
void DynamicWindowRenderObjectClass::Set_Alignment(unsigned int flags)
{
	unsigned int old_flags = Flags;
	Flags = old_flags & ~0xF0;
	unsigned int new_flags = Flags | (flags & 0xF0);
	Flags = new_flags;
	if (new_flags != old_flags) {
		Invalidate();
	}
}
// NOT EXACT: Update_Bounds 0x51D0D0 is inlined here; the client calls it
void DynamicWindowRenderObjectClass::Invalidate()
{
	Dirty = true;
	Update_Bounds(RenderObject);
	if (Parent != NULL) {
		Parent->Child_Changed(*this);
	}
}

bool DynamicWindowRenderObjectClass::Is_Dirty()
{
	return Dirty;
}

Vector2 DynamicWindowRenderObjectClass::Get_Min_Size()
{
	float x = max(MinSize.X, NaturalSize.X);
	float y = max(MinSize.Y, NaturalSize.Y);
	return Vector2(x, y);
}

bool DynamicWindowRenderObjectClass::Has_Flags(unsigned int flags)
{
	return (Flags & flags) == flags;
}
// NOT EXACT: the client reads the Fit_Size result through eax, this form through the stack slot
void DynamicWindowRenderObjectClass::Layout(const Vector2 &position, const Vector2 &size)
{
	Size = size;
	Vector2 offset = Fit_Size(position, size);
	LayoutOffset = Vector3(offset.X, offset.Y, 0.0f);
	Dirty = false;
}

Vector2 DynamicWindowRenderObjectClass::Get_Size()
{
	return Size;
}


























void DynamicWindowRenderObjectClass::Get_Bounds(AABoxClass &box)
{
	FAIL_IF(!RenderObject) return;
	Vector3 position = RenderObject->Get_Position();
	position -= Offset;
	box.Extent.X = Size.X * 0.5f;
	box.Extent.Y = Size.Y * 0.5f;
	box.Extent.Z = 0.0f;
	box.Center.X = position.X + box.Extent.X;
	box.Center.Y = position.Y - box.Extent.Y;
}




void DynamicWindowRenderObjectClass::Set_Parent(DynamicWindowContainerClass *parent)
{
	G_ASSERT(!Parent);
	FAIL_IF(!parent) return;
	Parent = parent;
	Invalidate();
}



void DynamicWindowRenderObjectClass::Clear_Parent()
{
	FAIL_IF(!Parent) return;
	Invalidate();
	Parent = NULL;
}

DynamicWindowContainerClass *DynamicWindowRenderObjectClass::Get_Container()
{
	return NULL;
}

void DynamicWindowRenderObjectClass::Add_To_Scene(SceneClass *scene)
{
	if (RenderObject && scene != RenderObject->Peek_Scene()) {
		Remove_From_Scene();
		if (RenderObject && scene != NULL) {
			RenderObject->Add(scene);
		}
	}
}

void DynamicWindowRenderObjectClass::Remove_From_Scene()
{
	if (RenderObject && RenderObject->Peek_Scene() != NULL) {
		RenderObject->Remove();
	}
}

SceneClass *DynamicWindowRenderObjectClass::Peek_Scene()
{
	if (!RenderObject) {
		return NULL;
	}
	return RenderObject->Peek_Scene();
}

void DynamicWindowRenderObjectClass::Set_Level(int level)
{
	Level = level;
	if (RenderObject) {
		Unknown_496730(RenderObject.Peek(), level, 0);
	}
}

int DynamicWindowRenderObjectClass::Get_Level()
{
	return Level;
}












void DynamicWindowRenderObjectClass::Set_Position(const Vector2 &position)
{
	FAIL_IF(!RenderObject) return;
	Vector3 pos(position.X, position.Y, 0.0f);
	RenderObject->Set_Position(pos + LayoutOffset + Offset);
}
// NOT EXACT: Update_Bounds 0x51D0D0 is inlined here; the client calls it
void DynamicWindowRenderObjectClass::Set_Render_Object(const RefCountPtr<RenderObjClass> &robj)
{
	SceneClass *scene = Peek_Scene();
	Remove_From_Scene();
	RenderObject = robj;
	if (scene != NULL) {
		Add_To_Scene(scene);
	}
	Set_Level(Level);
	Update_Bounds(RenderObject);
	Invalidate();
}

void DynamicWindowRenderObjectClass::Replace_Render_Object(const RefCountPtr<RenderObjClass> &robj)
{
	Vector3 position;
	if (RenderObject) {
		position = RenderObject->Get_Position();
	}
	Set_Render_Object(robj);
	if (RenderObject) {
		RenderObject->Set_Position(position);
	}
}

void DynamicWindowRenderObjectClass::Update_Bounds(const RefCountPtr<RenderObjClass> &robj)
{
	if (!robj) {
		NaturalSize.Set(0.0f, 0.0f);
		Offset.Set(0.0f, 0.0f, 0.0f);
		return;
	}
	AABoxClass box;
	robj->Get_Obj_Space_Bounding_Box(box);
	NaturalSize.Set(box.Extent.X * 2.0f, box.Extent.Y * 2.0f);
	Offset.X = -(box.Center.X - box.Extent.X);
	Offset.Y = -(box.Center.Y + box.Extent.Y);
}

Vector2 DynamicWindowRenderObjectClass::Fit_Size(const Vector2 &position, const Vector2 &size)
{
	Vector2 result = position;
	Vector2 min_size = Get_Min_Size();
	float dx = Size.X - min_size.X;
	if (!Has_Flags(0x10)) {
		if (Has_Flags(0x20)) {
			result.X += dx;
		} else {
			result.X += dx * 0.5f;
		}
	}
	float dy = Size.Y - min_size.Y;
	if (!Has_Flags(0x40)) {
		if (Has_Flags(0x80)) {
			result.Y -= dy;
		} else {
			result.Y -= dy * 0.5f;
		}
	}
	return result;
}

/*
**	Reconstruction notes (DW1, 2026-11-01)
**
**	Client 0x51C930-0x51D240: the base of the dynamic windows' model-backed elements. The constructor
**	0x51C930 (table 0xAED8D0), deleting destructor 0x51C9A0, destructor 0x51C9D0, the slots 0x51CA50-0x51CF30
**	(FAIL_IF '!RenderObject' on lines 129 / 215, G_ASSERT '!Parent' on 144, FAIL_IF '!parent' on 145,
**	FAIL_IF '!Parent' on 154), Set_Render_Object 0x51CFD0, Replace_Render_Object 0x51D060, Update_Bounds
**	0x51D0D0 (the model's object-space box: twice its extent the natural size, its top-left corner the
**	offset), Fit_Size 0x51D160 (alignment flags 0x10 / 0x20 horizontal, 0x40 / 0x80 vertical).
*/
