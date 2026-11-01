// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_multi_line_text.h (name assumed from
**	dynamic_window_multi_line_text.cpp, whose __FILE__ is 0xB76C68; the client holds no string of this header)
**
**	A dynamic window's wrapped text: the text split to the element's width (FontSplitStringClass), one
**	DynamicWindowTextClass per line, a window of DisplayedLines lines from TopLineIndex (table 0xAED784,
**	its container side 0xAED774 at +0x08; 0x84 bytes; constructor 0x51B180, destructor 0x51B3A0; made by
**	Create 0x51AFA0).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_MULTI_LINE_TEXT_H  // NAME 3 [dynamic_window_multi_line_text.h]: header name assumed from the client's dynamic_window_multi_line_text.cpp
#define DYNAMIC_WINDOW_MULTI_LINE_TEXT_H

#include "dynamic_window_render_object.h"
#include <string>
#include <vector>

class FontStyleClass;

class DynamicWindowMultiLineTextClass : public DynamicWindowElementClass, public DynamicWindowContainerClass  // NAME 2 [DynamicWindowMultiLineTextClass]: class from file (D24)
{
public:
	DynamicWindowMultiLineTextClass(const std::string &font_name, const RefCountPtr<FontStyleClass> &sample_font, const Vector2 &min_size, unsigned int flags);	// 0x51B180 (G_ASSERT 'CharacterHeight > 0')  // NAME 1 [font_name, sample_font, min_size, flags]: the FAIL_IFs'; Set_Min_Size's; the base's
	virtual ~DynamicWindowMultiLineTextClass();													// +0x04 0x51B3A0 (deleting 0x51B340)

	static RefCountPtr<DynamicWindowMultiLineTextClass> Create(const std::string &font_name, const Vector2 &min_size, unsigned int flags);	// 0x51AFA0 (FAIL_IF 'font_name.empty()', '!sample_font')  // NAME 1 [Create]
	void Set_Text(const std::string &text);														// 0x51B560  // NAME 1 [Set_Text, text]
	void Scroll_Up();																			// 0x51C390 (FAIL_IF 'TopLineIndex <= 0')  // NAME 1 [Scroll_Up]
	void Scroll_Down();																			// 0x51C470 (FAIL_IF 'Lines.size() > TopLineIndex + DisplayedLines')  // NAME 1 [Scroll_Down]

	virtual void Set_Min_Size(const Vector2 &min_size);
	virtual void Set_Flags(unsigned int flags);
	virtual void Clear_Flags(unsigned int flags);
	virtual void Set_Alignment(unsigned int flags);
	virtual void Invalidate();
	virtual bool Is_Dirty();
	virtual Vector2 Get_Min_Size();
	virtual bool Has_Flags(unsigned int flags);
	virtual void Layout(const Vector2 &position, const Vector2 &size);							// (FAIL_IF 'height <= 0')
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

	virtual void Child_Changed(DynamicWindowElementClass &child);								// +0x08 +0x00 0x51BDE0
	virtual void Remove_All_Children(bool recursive);											// +0x08 +0x04 0x51BE00 (G_ASSERT(0))
	virtual bool Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child);			// +0x08 +0x08 0x51BE30 (G_ASSERT(0))

protected:
	void Refresh();																				// 0x51BE60 (re-split at the laid-out width, then tell the parent)  // NAME 1 [Refresh]
	void Clear_Lines();																			// 0x51BEE0  // NAME 1 [Clear_Lines]
	void Split_Lines(float width);																// 0x51BFB0 (FAIL_IF 'CharacterHeight <= 0', '!line_element')  // NAME 1 [Split_Lines, width]
	void Position_Lines();																		// 0x51C580  // NAME 1 [Position_Lines]

	bool Dirty;																					// +0x0C  // NAME 1 [Dirty]
	DynamicWindowContainerClass *Parent;														// +0x10 (G_ASSERT '!Parent')
	SceneClass *Scene;																			// +0x14  // NAME 1 [Scene]
	int Level;																					// +0x18  // NAME 1 [Level]
	std::string FontName;																		// +0x1C ('font_name')
	RefCountPtr<FontStyleClass> SampleFont;														// +0x2C ('sample_font')
	float CharacterHeight;																		// +0x30 (G_ASSERT 'CharacterHeight > 0')
	Vector2 MinSize;																			// +0x34  // NAME 1 [MinSize]
	Vector2 Size;																				// +0x3C  // NAME 1 [Size]
	Vector2 LayoutPosition;																		// +0x44  // NAME 1 [LayoutPosition]
	Vector2 Position;																			// +0x4C  // NAME 1 [Position]
	unsigned int Flags;																			// +0x54  // NAME 1 [Flags]
	std::string Text;																			// +0x58  // NAME 1 [Text]
	std::vector< RefCountPtr<DynamicWindowElementClass> > Lines;								// +0x68 (FAIL_IF 'Lines.size() > ...')
	RefCountPtr<DynamicWindowElementClass> Unknown78;											// +0x78 (released by the destructor)  // NAME 1: neutral
	unsigned int TopLineIndex;																			// +0x7C (FAIL_IF 'TopLineIndex <= 0')
	unsigned int DisplayedLines;																			// +0x80 (FAIL_IF '... TopLineIndex + DisplayedLines')
};

#endif
