// Earth & Beyond client (reconstruction): app\Client\dynamic_window_multi_line_text.cpp (__FILE__ 0xB76C68). Notes at the end of the file.
#include "dynamic_window_multi_line_text.h"
#include "dynamic_window_text.h"
#include "fontmanager.h"
#include "aabox.h"
#include "..\shared\unknown_font_split_string.h"
#include "..\shared\unknown_debug.h"




















RefCountPtr<DynamicWindowMultiLineTextClass> DynamicWindowMultiLineTextClass::Create(const std::string &font_name, const Vector2 &min_size, unsigned int flags)
{
	FAIL_IF(font_name.empty()) return NULL;

	RefCountPtr<FontStyleClass> sample_font = Create_NEW(new FontStyleClass(font_name.c_str(), 0, "W"));

	FAIL_IF(!sample_font) return NULL;

	return Create_NEW(new DynamicWindowMultiLineTextClass(font_name, sample_font, min_size, flags));
}





DynamicWindowMultiLineTextClass::DynamicWindowMultiLineTextClass(const std::string &font_name, const RefCountPtr<FontStyleClass> &sample_font, const Vector2 &min_size, unsigned int flags) :
	Dirty(false),
	Parent(NULL),
	Scene(NULL),
	Level(0),
	FontName(font_name),
	SampleFont(sample_font),
	CharacterHeight(0.0f),
	MinSize(min_size),
	Size(0.0f, 0.0f),
	LayoutPosition(0.0f, 0.0f),
	Position(0.0f, 0.0f),
	Flags(flags),
	Text(""),
	Lines(),
	Unknown78(),
	TopLineIndex(0),
	DisplayedLines(0)
{
	float width = 0.0f;
	SampleFont->Unknown_6D5A80(width, CharacterHeight);
	G_ASSERT(CharacterHeight > 0);
}
// NOT EXACT: the line erase (the client calls the vector helpers 0x51C710 / 0x51C6C0 out of line)
DynamicWindowMultiLineTextClass::~DynamicWindowMultiLineTextClass()
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Lines.begin(); it != Lines.end(); ++it) {
		(*it)->Remove_From_Scene();
		(*it)->Clear_Parent();
	}
	Lines.erase(Lines.begin(), Lines.end());
	TopLineIndex = 0;
	DisplayedLines = 0;
}

void DynamicWindowMultiLineTextClass::Set_Text(const std::string &text)
{
	Text = text;
	Dirty = true;
	Refresh();
}

void DynamicWindowMultiLineTextClass::Set_Min_Size(const Vector2 &min_size)
{
	MinSize = min_size;
	Refresh();
	Invalidate();
}
// NOT EXACT: the client keeps the new flags in a second register (esi, then edx) for the compare
void DynamicWindowMultiLineTextClass::Set_Flags(unsigned int flags)
{
	unsigned int old_flags = Flags;
	unsigned int new_flags = old_flags | flags;
	Flags = new_flags;
	if (new_flags != old_flags) {
		Invalidate();
	}
}

void DynamicWindowMultiLineTextClass::Clear_Flags(unsigned int flags)
{
	unsigned int old_flags = Flags;
	Flags &= ~flags;
	if (Flags != old_flags) {
		Invalidate();
	}
}
// NOT EXACT: the client stores the cleared flags, reloads them and ORs in the new bits; this form folds both into one store
void DynamicWindowMultiLineTextClass::Set_Alignment(unsigned int flags)
{
	unsigned int old_flags = Flags;
	Flags &= ~0xF0;
	Flags |= flags & 0xF0;
	if (Flags != old_flags) {
		Invalidate();
	}
}

void DynamicWindowMultiLineTextClass::Invalidate()
{
	Dirty = true;
	if (Parent != NULL) {
		Parent->Child_Changed(*this);
	}
}

bool DynamicWindowMultiLineTextClass::Is_Dirty()
{
	return Dirty;
}

Vector2 DynamicWindowMultiLineTextClass::Get_Min_Size()
{
	return MinSize;
}

bool DynamicWindowMultiLineTextClass::Has_Flags(unsigned int flags)
{
	return (Flags & flags) == flags;
}

void DynamicWindowMultiLineTextClass::Layout(const Vector2 &position, const Vector2 &size)
{
	Size = size;
	LayoutPosition = position;
	Split_Lines(size.X);
	float width = 0.0f;
	float height = 0.0f;
	SampleFont->Unknown_6D5A80(width, height);



	FAIL_IF(height <= 0) return;
	Vector2 line_position = position;
	Vector2 line_size(size.X, height);
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Lines.begin(); it != Lines.end(); ++it) {
		(*it)->Layout(line_position, line_size);
		line_position.Y -= height;
	}
	Dirty = false;
	Position_Lines();
}

Vector2 DynamicWindowMultiLineTextClass::Get_Size()
{
	return Size;
}

void DynamicWindowMultiLineTextClass::Get_Bounds(AABoxClass &box)
{
	box.Extent.X = Size.X * 0.5f;
	box.Extent.Y = Size.Y * 0.5f;
	Vector2 position = Position + LayoutPosition;
	box.Center.X = position.X + box.Extent.X;
	box.Center.Y = position.Y - box.Extent.Y;
}









void DynamicWindowMultiLineTextClass::Set_Parent(DynamicWindowContainerClass *parent)
{
	G_ASSERT(!Parent);
	FAIL_IF(!parent) return;
	Parent = parent;
	Invalidate();
}


void DynamicWindowMultiLineTextClass::Clear_Parent()
{
	FAIL_IF(!Parent) return;
	Invalidate();
	Parent = NULL;
}

DynamicWindowContainerClass *DynamicWindowMultiLineTextClass::Get_Container()
{
	return NULL;
}

void DynamicWindowMultiLineTextClass::Add_To_Scene(SceneClass *scene)
{
	if (Scene != NULL) {
		Remove_From_Scene();
	}
	Scene = scene;
	int last = TopLineIndex + DisplayedLines - 1;
	for (int i = TopLineIndex; i <= last; i++) {
		Lines[i]->Add_To_Scene(scene);
	}
}

void DynamicWindowMultiLineTextClass::Remove_From_Scene()
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Lines.begin(); it != Lines.end(); ++it) {
		(*it)->Remove_From_Scene();
	}
	Scene = NULL;
}

SceneClass *DynamicWindowMultiLineTextClass::Peek_Scene()
{
	return Scene;
}

void DynamicWindowMultiLineTextClass::Set_Level(int level)
{
	Level = level;
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Lines.begin(); it != Lines.end(); ++it) {
		(*it)->Set_Level(Level);
	}
}

int DynamicWindowMultiLineTextClass::Get_Level()
{
	return Level;
}

void DynamicWindowMultiLineTextClass::Set_Position(const Vector2 &position)
{
	Position = position;
	Position_Lines();
}

void DynamicWindowMultiLineTextClass::Child_Changed(DynamicWindowElementClass &child)
{
	Invalidate();
}















void DynamicWindowMultiLineTextClass::Remove_All_Children(bool recursive)
{
	G_ASSERT(0);
}





bool DynamicWindowMultiLineTextClass::Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child)
{
	G_ASSERT(0);
	return false;
}

void DynamicWindowMultiLineTextClass::Refresh()
{
	Vector2 size = Get_Size();
	if (size.Y > 0) {
		if (Size.X > 0) {
			Split_Lines(Size.X);
		}
		DynamicWindowMultiLineTextClass::Invalidate();
	}
}

void DynamicWindowMultiLineTextClass::Clear_Lines()
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Lines.begin(); it != Lines.end(); ++it) {
		(*it)->Remove_From_Scene();
		(*it)->Clear_Parent();
	}
	Lines.clear();
	TopLineIndex = 0;
	DisplayedLines = 0;
}
// NOT EXACT: functional: the split, the push_back and the per-line temporaries differ
void DynamicWindowMultiLineTextClass::Split_Lines(float width)
{
	for (std::vector< RefCountPtr<DynamicWindowElementClass> >::iterator it = Lines.begin(); it != Lines.end(); ++it) {
		(*it)->Remove_From_Scene();
		(*it)->Clear_Parent();
	}
	Lines.erase(Lines.begin(), Lines.end());
	TopLineIndex = 0;
	DisplayedLines = 0;









	FAIL_IF(CharacterHeight <= 0) return;

	FontSplitStringClass split(*(const RefCountPtr<Font32Class> *)&SampleFont->Font, Text.c_str(), width, 0);
	split.Split(Text.c_str(), width, 0);
	int count = split.Count();
	DisplayedLines = count;
	float height = count * CharacterHeight;
	if (height > Size.Y) {
		Size.Y = height;
		Vector2 min_size = Get_Min_Size();
		min_size.Y = height;
		Set_Min_Size(min_size);
	}

	unsigned int shown = 0;
	for (int i = 0; i < count; i++) {
		std::string line(split.Get(i));
		RefCountPtr<DynamicWindowElementClass> line_element = DynamicWindowTextClass::Create(FontName, line, Flags);


















		FAIL_IF(!line_element) continue;
		Lines.push_back(line_element);
		line_element->Set_Level(Level);
		if (shown < DisplayedLines) {
			line_element->Add_To_Scene(Scene);
			shown++;
		}
		line_element->Set_Parent(this);
	}
}




void DynamicWindowMultiLineTextClass::Scroll_Up()
{
	FAIL_IF(TopLineIndex <= 0) return;
	TopLineIndex--;
	Lines[TopLineIndex + DisplayedLines]->Remove_From_Scene();
	Lines[TopLineIndex]->Add_To_Scene(Scene);
	Position_Lines();
}











void DynamicWindowMultiLineTextClass::Scroll_Down()
{
	FAIL_IF(Lines.size() > TopLineIndex + DisplayedLines) return;
	Lines[TopLineIndex]->Remove_From_Scene();
	TopLineIndex++;
	Lines[TopLineIndex + DisplayedLines - 1]->Add_To_Scene(Scene);
	Position_Lines();
}

void DynamicWindowMultiLineTextClass::Position_Lines()
{
	Vector2 position = Position;
	position.Y += TopLineIndex * CharacterHeight;
	int last = TopLineIndex + DisplayedLines - 1;
	for (int i = TopLineIndex; i <= last; i++) {
		Lines[i]->Set_Position(position);
	}
}

/*
**	Reconstruction notes (DW1, 2026-11-01)
**
**	Client 0x51AEE0-0x51C650: the wrapped text. Create 0x51AFA0 (FAIL_IF 'font_name.empty()' on line 30,
**	'!sample_font' on 34; the sample font measures "W"), the constructor 0x51B180 (G_ASSERT 'CharacterHeight > 0'
**	on 64; the element's abstract table 0xAED7F0 is stored first, its deleting destructor 0x51B370), the
**	slots 0x51B340-0x51BD50 (FAIL_IF 'height <= 0' on 155, G_ASSERT '!Parent' on 190, FAIL_IF '!parent' on 191,
**	'!Parent' on 199), the container slots 0x51BDE0-0x51BE30 (G_ASSERT(0) on 274 / 283: a text has no
**	children), Refresh 0x51BE60, Clear_Lines 0x51BEE0, Split_Lines 0x51BFB0 (FAIL_IF 'CharacterHeight <= 0' on
**	327, '!line_element' on 363), Scroll_Up 0x51C390 (379), Scroll_Down 0x51C470 (398: the client scrolls when
**	the FAIL_IF's condition is false, i.e. only while every line is shown), Position_Lines 0x51C580; then the
**	template instances 0x51C600-0x51C780. 0x51AEE0 (a call through +0xA4's slot +0x30) and 0x51C800-0x51C910
**	(a render-object element with table 0xAED85C) are outside this file's classes and not rebuilt.
**	NOT EXACT (D64): the functions marked, written from the pseudocode, functional.
*/
