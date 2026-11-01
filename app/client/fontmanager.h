// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\fontmanager.h (name assumed from fontmanager.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\Client\fontmanager.cpp')
**
**	The font manager (0x1C bytes, created by 0x526DE0 into 0xBE67C4): the fonts by id (an SGI
**	hash_map of 100 buckets) and the debug font. Declared as far as the rebuilt functions need.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __FONTMANAGER_H  // NAME 3 [fontmanager.h]: header name assumed from the client's fontmanager.cpp
#define __FONTMANAGER_H

#include <string>
#include <hash_map.h>
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\wwstring.h"
#include "fontdatabase.h"
#include <string.h>
#include "..\..\wwlib\point.h"
#include "..\..\ww3d2\dynamesh.h"

struct FontStringStruct;
class FontDatabaseClass;
class Font32Class;

struct FontStyleColorStruct  // NAME 1 [FontStyleColorStruct]: neutral (D10): 16 bytes, a color and one more word
{
	TPoint3D<int> Color;												// +0x00  // NAME 2: what 0x6D1010 stores
	int Unknown0C;														// +0x0C  // NAME 1: neutral (D10)
};

class Unknown_6D8B90Class  // NAME 1 [Unknown_6D8B90Class]: neutral (D10): 16 bytes, a 32 x 32 texture; counted in 0xBE67CC (FontStyleClass +0x2D0 / +0x2E0)
{
public:
	Unknown_6D8B90Class();												// 0x6D8B90
	~Unknown_6D8B90Class();												// 0x6D8C50

	int Size;															// +0x00: 0x20  // NAME 2: what the constructor gives the texture
	int Unknown04;														// +0x04  // NAME 1: neutral (D10)
	int Unknown08;														// +0x08  // NAME 1: neutral (D10)
	TextureClass *Texture;												// +0x0C  // NAME 2: what it holds

	static int Count;													// 0xBE67CC: live objects (0x6D8C50 frees 0xBE67C8 at 0)  // NAME 2: what 0x6D8B90 / 0x6D8C50 do with it
	static int Unknown_BE67D8;											// 0xBE67D8: 0x6D8B90 counts up  // NAME 1: neutral (D10)
};

class FontStyleClass : public DynamicMeshClass  // NAME 1 [FontStyleClass]: neutral (D10): the client's FontSys (Get_Name 0x67EF20 "FontSys"; 0x4E8 bytes; 0x6D47C0 builds the DynamicMeshClass(2, 4) base, vtable 0xAFECB4)
{
public:
	FontStyleClass(const char *name, int object, const char *text);	// 0x6D47C0: the string template named name (FontDatabaseClass::Find_String)  // NAME 1 [object]: neutral (D10)
	FontStyleClass(FontStringStruct def, unsigned int object, const char *text);	// 0x6D4C70: from a string template (0x6CE990)  // NAME 1 [def, object]: neutral (D10)
	~FontStyleClass() { REF_PTR_RELEASE(Font); }						// 0x67F230 (inline: emitted in chat_window.cpp's object); 0x6D4C40 its deleting form

	virtual int Class_ID(void) const { return 0x1000D; }					// 0x67EF00  // NAME 2: RenderObjClass's
	virtual const char *Get_Name(void) const { return "FontSys"; }		// 0x67EF20  // NAME 2: RenderObjClass's
	virtual void Set_Hidden(int onoff) { Unknown300 = (onoff == 0); RenderObjClass::Set_Hidden(onoff); }	// 0x67EF40  // NAME 2: RenderObjClass's
	virtual bool Is_Complete(void) { return Unknown404; }					// 0x67EEE0  // NAME 2: RenderObjClass's
	virtual void Render(RenderInfoClass &rinfo);							// 0x6D8270  // NAME 2: RenderObjClass's
	virtual void Set_Container(RenderObjClass *con);						// 0x6D5920  // NAME 2: RenderObjClass's
	virtual void Set_Transform(const Matrix3D &m);						// 0x6D87F0  // NAME 2: RenderObjClass's
	virtual void Set_Position(const Vector3 &v);							// 0x6D88C0  // NAME 2: RenderObjClass's
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;	// 0x6D5B90  // NAME 2: RenderObjClass's
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const;		// 0x6D5C10: the text rectangle +0x420 on a 640 x 480 screen, placed by Info.Justify  // NAME 2: RenderObjClass's

	void Unknown_6D5170(int object);										// 0x6D5170: the set-up both constructors end with  // NAME 1 [Unknown_6D5170, object]: neutral (D10)
	void Unknown_6D6390();													// 0x6D6390  // NAME 1 [Unknown_6D6390]: neutral (D10)
	void Update_Text(bool unknown);										// 0x6D64C0: the client's profile label 'FontSys::Update_Text'  // NAME 3: the client's label; NAME 1 [unknown]: neutral (D10)
	void Set_Color(const TPoint3D<int> &color) { Unknown494.Color = color; Unknown4A4 = Unknown494; }	// inlined in 0x6D10B0  // NAME 2: describes what it does

	void Unknown_6D5140();	// 0x6D5140: its +0x38 virtual, then Release_Ref  // NAME 1 [Unknown_6D5140]: neutral (D10)
	void Unknown_6CE7D0(int unknown);										// 0x6CE7D0  // NAME 1 [Unknown_6CE7D0]: neutral (D10)
	void Change_Text(const char *text, int unknown);						// 0x6CDBC0: the client's profile label 'FontSys::Change_Text'  // NAME 3: the client's label
	void Unknown_6D5D40();													// 0x6D5D40  // NAME 1 [Unknown_6D5D40]: neutral (D10)
	void Unknown_6CD950(int unknown1, int unknown2);						// 0x6CD950  // NAME 1 [Unknown_6CD950]: neutral (D10)
	void Unknown_6CE910(bool on);											// 0x6CE910 (textgadg.cpp: on while the box has the focus)  // NAME 1 [on]: neutral
	void Unknown_6D88F0(int left, int top, int right, int bottom);			// 0x6D88F0 (textgadg.cpp: the box's rectangle, from the bounding box and the position)  // NAME 1 [left, top, right, bottom]: what textgadg.cpp passes
	void Unknown_6D8650(Vector3 &pos);										// 0x6D8650: the text's position (the transform's, or +0x464 with +0x302 set, plus +0x47C and +0x488; ret 4 with the pointer in eax: maybe a Vector3 return)  // NAME 1 [pos]: neutral (D10)
	const char *Unknown_6D87D0();											// 0x6D87D0: Info.Text (fontsys_cursor.cpp's FAIL_IF '!text')  // NAME 1 [Unknown_6D87D0]: neutral (D10)

	FontStringStruct Info;	// +0xF4: its string template (0x6D0A70 copies it; +0xF4 its name, what 0x6D10B0 compares)  // NAME 2 [Info]: what it holds
	char Unknown1E0[0x6C];	// +0x1E0  // NAME 1: neutral (D10)
	Font32Class *Font;													// +0x24C (textgadg.cpp's G_ASSERT 'font'; its widths at 640)  // NAME 3: the assert's
	int Unknown250;														// +0x250: 0x6D28C0 sets it in every style of a font  // NAME 1: neutral (D10)
	char Unknown254[0x18];												// +0x254  // NAME 1: neutral (D10)
	int Unknown26C;														// +0x26C: 0x6CF930 sets it per line (ftol)  // NAME 1: neutral (D10)
	int Unknown270;														// +0x270: Render draws while +0x26C <= it  // NAME 1: neutral (D10)
	char Unknown274[0x2B4 - 0x274];									// +0x274  // NAME 1: neutral (D10)
	int Unknown2B4;														// +0x2B4: a screen width (640 / it scales the bounding box)  // NAME 1: neutral (D10)
	int Unknown2B8;														// +0x2B8: a screen height (480 / it)  // NAME 1: neutral (D10)
	char Unknown2BC[0x2D0 - 0x2BC];									// +0x2BC  // NAME 1: neutral (D10)
	Unknown_6D8B90Class Unknown2D0;										// +0x2D0  // NAME 1: neutral (D10)
	Unknown_6D8B90Class Unknown2E0;										// +0x2E0  // NAME 1: neutral (D10)
	StringClass Unknown2F0;												// +0x2F0  // NAME 1: neutral (D10)
	char Unknown2F4[0x300 - 0x2F4];									// +0x2F4  // NAME 1: neutral (D10)
	bool Unknown300;													// +0x300: not hidden (Set_Hidden)  // NAME 1: neutral (D10)
	char Unknown301[0x404 - 0x301];									// +0x301  // NAME 1: neutral (D10)
	bool Unknown404;													// +0x404: Is_Complete's  // NAME 1: neutral (D10)
	char Unknown405[0x420 - 0x405];									// +0x405  // NAME 1: neutral (D10)
	int Unknown420[4];													// +0x420: the text rectangle (left, right, top, bottom: 0x6D5C10)  // NAME 1: neutral (D10)
	char Unknown430[0x440 - 0x430];									// +0x430  // NAME 1: neutral (D10)
	int Unknown440;														// +0x440: Render draws below 3  // NAME 1: neutral (D10)
	char Unknown444[0x454 - 0x444];									// +0x444  // NAME 1: neutral (D10)
	float Unknown454;													// +0x454: 0x6CF930's per-line value, at least +0x480 (0x6D5170 zeroes it)  // NAME 1: neutral (D10)
	int Unknown458;														// +0x458: zeroed by 0x6D5170 and Change_Text  // NAME 1: neutral (D10)
	int LineCount;														// +0x45C: the number of lines 0x6CF930 split the text into  // NAME 2: what 0x6CF930 stores
	int LineNumber;														// +0x460: this style's line, from 1 (0x6CF930)  // NAME 2: what 0x6CF930 stores
	float Unknown464[2];												// +0x464: 0x6CD180's x / y with +0x302 set, added to +0x47C / +0x480  // NAME 1: neutral (D10)
	int Unknown46C;														// +0x46C  // NAME 1: neutral (D10)
	Vector3 Unknown470;													// +0x470: a copy of +0x47C (0x6D5170, 0x6CF930); Change_Text puts its x back  // NAME 1: neutral (D10)
	Vector3 Unknown47C;													// +0x47C: Info's +0x34 / +0x38 times 640 / 480 (0x6D5170, 0x6CDAB0), y moved per line by 0x6CF930  // NAME 1: neutral (D10)
	char Unknown488[0x494 - 0x488];									// +0x488  // NAME 1: neutral (D10)
	FontStyleColorStruct Unknown494;									// +0x494: 0x6D1010's color  // NAME 1: neutral (D10)
	FontStyleColorStruct Unknown4A4;									// +0x4A4: a copy of +0x494 (0x6D1010)  // NAME 1: neutral (D10)
	char Unknown4B4[0x4C4 - 0x4B4];									// +0x4B4  // NAME 1: neutral (D10)
	bool Unknown4C4;													// +0x4C4: Render clears it  // NAME 1: neutral (D10)
	bool Unknown4C5;													// +0x4C5: Set_Transform / Set_Position clear it  // NAME 1: neutral (D10)
	char Unknown4C6[0x4D0 - 0x4C6];									// +0x4C6  // NAME 1: neutral (D10)
	Vector3 Unknown4D0;													// +0x4D0: the last translation Set_Transform saw  // NAME 1: neutral (D10)
	float Unknown4DC;													// +0x4DC: an x scale Render undoes above 1  // NAME 1: neutral (D10)
	float Unknown4E0;													// +0x4E0: a y scale Render undoes above 1  // NAME 1: neutral (D10)
	char Unknown4E4;													// +0x4E4  // NAME 1: neutral (D10)
	bool Unknown4E5;													// +0x4E5: Set_Transform / Set_Position set it  // NAME 1: neutral (D10)
	char Unknown4E6[2];												// +0x4E6  // NAME 1: neutral (D10)
};

class ManagedFontClass  // NAME 2 [ManagedFontClass]: what the manager holds per id (0x24 bytes; its +4 is the id)
{
public:
	ManagedFontClass(RenderObjClass *object);							// 0x6CE990: the render object's styles from the font database
	ManagedFontClass(const char *name, int unknown);						// 0x6CF520 (no caller), inlined in 0x6D1FD0  // NAME 1 [unknown]: neutral (D10)
	void Unknown_6CF930(FontStringStruct *def, int unknown);				// 0x6CF930: a 'W' template's lines as styles (unknown is the render object)  // NAME 1 [Unknown_6CF930]: neutral (D10)
	~ManagedFontClass();												// 0x6CF730
	void Unknown_All_Styles(int unknown1, int unknown2);					// 0x6D0950, inlined in 0x6D1C10 (its own begin / ++ called out of line there)  // NAME 1 [Unknown_All_Styles]: neutral (D10)
	void Unknown_Set_Styles(int value);									// 0x6D0710, inlined in 0x6D28C0 (its own begin / ++ called out of line there)  // NAME 1 [Unknown_Set_Styles]: neutral (D10)
	void Unknown_6D07D0(int unknown);									// 0x6D07D0 (no caller): each style's 0x6CE7D0  // NAME 1 [Unknown_6D07D0]: neutral (D10)
	void Unknown_6D0890();												// 0x6D0890 (no caller): each style's 0x6D5D40  // NAME 1 [Unknown_6D0890]: neutral (D10)
	void Unknown_6D10B0(const char *name, TPoint3D<int> color);			// 0x6D10B0 (no caller): the color of every style named name  // NAME 1 [Unknown_6D10B0]: neutral (D10)
	void Unknown_6D1220(FontStyleClass *style);							// 0x6D1220: adds a style under its +0xF8 name, else its name  // NAME 1 [Unknown_6D1220]: neutral (D10)
	FontStyleClass *Unknown_6D1480(std::string name);					// 0x6D1480: the style named name + "_0"  // NAME 1 [Unknown_6D1480]: neutral (D10)
	FontStyleClass *Unknown_6D1600(std::string name);					// 0x6D1600: the named style, else 0x6D1480's  // NAME 1 [Unknown_6D1600]: neutral (D10)
	void Unknown_6D1010(const char *name, TPoint3D<int> color);			// 0x6D1010: the named style's color (+0x494, +0x4A4)  // NAME 1 [Unknown_6D1010]: neutral (D10)
	void Unknown_6D01C0(int level, const char *name);					// 0x6D01C0: Set_Sort_Level of every style (name NULL) or the styles named name  // NAME 1 [Unknown_6D01C0]: neutral (D10)
	void Unknown_6D0390(const char *name);								// 0x6D0390: shows every style (name NULL) or the styles named name (Set_Hidden(false))  // NAME 1 [Unknown_6D0390]: neutral (D10)
	void Unknown_6D0550(const char *name);								// 0x6D0550: hides them (Set_Hidden(true))  // NAME 1 [Unknown_6D0550]: neutral (D10)
	void Unknown_6D0A70(const char *name, const char *text);	// 0x6D0A70: reloads the styles named name with the text  // NAME 1 [Unknown_6D0A70]: neutral (D10)
	void Unknown_6D0A20(const char *name, const char *text, int unknown);	// 0x6D0A20: the named style's Change_Text  // NAME 1 [Unknown_6D0A20]: neutral (D10)

	StringClass Name;													// +0x00  // NAME 2: the name 0x6D1FD0 looks up in the font database
	unsigned int Id;													// +0x04: the manager's key  // NAME 2: what it holds
	int Unknown08;														// +0x08  // NAME 1: neutral (D10)
	std::hash_map<std::string, FontStyleClass *> Styles;				// +0x0C: its styles by name (0x6D1C10 walks them)  // NAME 2: what it holds
};

class FontManagerClass  // NAME 3 [FontManagerClass]: by convention (D24) from the client's fontmanager.cpp
{
public:
	FontManagerClass();													// 0x6D16E0
	~FontManagerClass();												// 0x6D1920
	void Remove_All();													// 0x6D1AE0  // NAME 2: describes what it does
	void Unknown_6D1C10(int unknown1, int unknown2);					// 0x6D1C10: each font's 0x6CD950  // NAME 1 [Unknown_6D1C10]: neutral (D10)
	void Add(RenderObjClass *object);										// 0x6D1D80: a new font for the render object, under its address  // NAME 2: describes what it does
	void Add(const RefCountPtr<RenderObjClass> &object);					// 0x6D1FB0  // NAME 2: describes what it does
	ManagedFontClass *Create(const char *name, int unknown);			// 0x6D1FD0: a font from the font database, added under its id  // NAME 2: describes what it does
	void Remove(unsigned int id);										// 0x6D2220  // NAME 2: describes what it does
	void Unknown_6D2310(const unsigned int &id);						// 0x6D2310: Remove through a reference (a screen's member)  // NAME 1 [Unknown_6D2310]: neutral (D10)
	void Unknown_6D2400(unsigned int id);								// 0x6D2400  // NAME 1 [Unknown_6D2400]: neutral (D10)
	void Unknown_6D2480(unsigned int id);								// 0x6D2480  // NAME 1 [Unknown_6D2480]: neutral (D10)
	void Unknown_6D2500(const char *name, const char *text, unsigned int id, int unknown);	// 0x6D2500: the font's 0x6D0A20								// 0x6D2500  // NAME 1 [Unknown_6D2500]: neutral (D10)
	void Unknown_6D2580(const char *name, TPoint3D<int> color, unsigned int id);	// 0x6D2580: a font's style color  // NAME 1 [Unknown_6D2580]: neutral (D10)
	FontStyleClass *Get_Style(const char *name, unsigned int id);		// 0x6D25D0: a font's named style, else the debug font  // NAME 2: describes what it does
	RefCountPtr<FontStyleClass> Unknown_6D2700(const char *name, const unsigned int &id);	// 0x6D2700: a font's named style, else the debug font  // NAME 1 [Unknown_6D2700]: neutral (D10)
	void Unknown_6D28C0(int value, unsigned int id);					// 0x6D28C0: each style's +0x250 of a font  // NAME 1 [Unknown_6D28C0]: neutral (D10)
	void Add(ManagedFontClass *font);									// 0x6D29A0: under its id  // NAME 2: describes what it does
	ManagedFontClass *Peek_Font(unsigned int id);						// inline only: Find's body where the client inlines it (the trio 0x6D2400, Get_Style, 0x6D28C0)  // NAME 1 [Peek_Font]: neutral (D10)
	ManagedFontClass *Find(unsigned int id);							// 0x6D2A70  // NAME 2: describes what it does
	ManagedFontClass *Unknown_6D2AE0(const unsigned int &id);			// 0x6D2AE0: Find through a reference (no caller in the client)  // NAME 1 [Unknown_6D2AE0]: neutral (D10)

	std::hash_map<unsigned int, ManagedFontClass *> Fonts;				// +0x00: 100 buckets  // NAME 2: what it holds
	RefCountPtr<FontStyleClass> DebugFont;								// +0x18: "DebugFont"  // NAME 2: what it holds
};

#endif //__FONTMANAGER_H
