// Earth & Beyond client (reconstruction): app\Client\fontmanager.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "..\..\wwlib\wwstring.h"
































































































// NOT EXACT: inline budget: the client calls the std::string(const char *, allocator) constructor out of line, ours inlines it (attempts.csv)
void ManagedFontClass::Unknown_6D1010(const char *name, TPoint3D<int> color)
{
	FontStyleClass *style = Unknown_6D1600(name);
	style->Unknown494.Color = color;
	style->Unknown4A4 = style->Unknown494;
}

















































































































































































































































































FontManagerClass::FontManagerClass()
{
	Fonts.clear();
	DebugFont = Create_NEW(new FontStyleClass("DebugFont", 0, 0));
}

FontManagerClass::~FontManagerClass()
{
	DebugFont = NULL;
	Remove_All();
}

void FontManagerClass::Remove_All()
{
	for (std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		delete it->second;
	}
	Fonts.clear();
}

void FontManagerClass::Unknown_6D1C10(int unknown1, int unknown2)
{
	for (std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		it->second->Unknown_All_Styles(unknown1, unknown2);
	}
}

// NOT EXACT: register-only, 3 rows: the client loads the id into ecx and keeps the font in eax (attempts.csv)
void FontManagerClass::Add(const char *name)
{
	ManagedFontClass *font = new ManagedFontClass(name);
	if (font && font->Id) {
		Fonts[font->Id] = font;
	}
}

// NOT EXACT: register-only, 2 rows: the client reloads the buffer into eax, ours into edx (attempts.csv)
void FontManagerClass::Add(const StringClass &name)
{
	Add((const char *)name);
}

ManagedFontClass *FontManagerClass::Create(const char *name, int unknown)
{
	ManagedFontClass *font = new ManagedFontClass(name, unknown);
	Add(font);
	return font;
}

void FontManagerClass::Remove(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it != Fonts.end()) {
		delete it->second;
		Fonts.erase(it);
	}
}

void FontManagerClass::Unknown_6D2310(const unsigned int &id)
{
	unsigned int key = id;
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(key);
	if (it != Fonts.end()) {
		delete it->second;
		Fonts.erase(it);
	}
}

void FontManagerClass::Unknown_6D2400(unsigned int id)
{
	Find(id)->Unknown_6D0390(NULL);
}

void FontManagerClass::Unknown_6D2480(unsigned int id)
{
	Find(id)->Unknown_6D0550(NULL);
}

void FontManagerClass::Unknown_6D2500(unsigned int id)
{
	Find(id)->Unknown_6D0A20(NULL);
}

// NOT EXACT: inline budget: the client calls Find 0x6D2A70 out of line (after pushing the arguments), ours inlines it (attempts.csv)
void FontManagerClass::Unknown_6D2580(const char *name, TPoint3D<int> color, unsigned int id)
{
	Find(id)->Unknown_6D1010(name, color);
}



















































// NOT EXACT: order-only, 1 row: the inlined std::string(name) stores its terminator through [ebx+eax], ours [eax+ebx] (attempts.csv)
FontStyleClass *FontManagerClass::Get_Style(const char *name, unsigned int id)
{
	ManagedFontClass *font = Find(id);
	if (!font) {
		G_ASSERT(0);
		return DebugFont.Peek();
	}

	FontStyleClass *style = font->Unknown_6D1600(name);
	if (!style) {



		G_ASSERT(0);
		return DebugFont.Peek();
	}
	return style;
}

// NOT EXACT: inline budget, 75 rows: the client calls the styles table's begin and bucket helpers out of line, ours inlines them (attempts.csv)
void FontManagerClass::Unknown_6D28C0(int value, unsigned int id)
{
	Find(id)->Unknown_Set_Styles(value);
}

void FontManagerClass::Add(ManagedFontClass *font)
{
	if (font && font->Id) {
		Fonts[font->Id] = font;
	}
}

ManagedFontClass *FontManagerClass::Find(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it == Fonts.end()) return NULL;
	return it->second;
}
/*
**	Reconstruction notes
**
**	client.exe: the window 0x6D0F70-0x6D2A70 (asserts on lines 360, 524, 533 and 550), then the
**	containers' instances. Not rebuilt yet: the font's own functions 0x6D0F70, 0x6D10B0-0x6D1600
**	(the asserting 0x6D1220 on line 360) and the manager's 0x6D2700 (line 550, a style by name as
**	a RefCountPtr). 0x6D2400 / 0x6D2480 / 0x6D2500 differ only by their callee: one pattern, not counted.
**	The depth lever (workflow.md 3.1) made the destructor and 0x6D1C10 exact: the client calls the
**	nested table helpers out of line when they sit one inline level deeper.
*/
