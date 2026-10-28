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

// NOT EXACT: shape, 18 rows: the client stores the color through [ebp+0x494] directly, ours through the member's address (attempts.csv)
void ManagedFontClass::Unknown_6D10B0(const char *name, TPoint3D<int> color)
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.begin(); it != Styles.end(); ++it) {
		FontStyleClass *style = it->second;
		if (strcmp(name, style->Name) == 0) {
			style->Set_Color(color);
		}
	}
}



































































































































































































































void ManagedFontClass::Unknown_6D1220(FontStyleClass *style)
{
	if (style && style->Name) {
		std::string key = "";
		if (!style->Unknown0F8.Is_Empty()) {
			key = style->Unknown0F8;
		} else {
			key = style->Name;
		}
		if (Unknown_6D1600(key)) {





			G_ASSERT(0);
		} else {
			Styles[key] = style;
		}
	}
}

// NOT EXACT: shape, 38 rows: the key copy and its destruction differ (attempts.csv)
FontStyleClass *ManagedFontClass::Unknown_6D1480(std::string name)
{
	std::string key = name;
	key += "_0";
	std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.find(key);
	if (it == Styles.end()) return NULL;
	return it->second;
}

// NOT EXACT: shape, 39 rows: the client destroys the name in one shared tail, ours in each return (attempts.csv)
FontStyleClass *ManagedFontClass::Unknown_6D1600(std::string name)
{
	std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.find(name);
	if (it == Styles.end()) return Unknown_6D1480(name);
	return it->second;
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
	Peek_Font(id)->Unknown_6D0390(NULL);
}

void FontManagerClass::Unknown_6D2480(unsigned int id)
{
	Peek_Font(id)->Unknown_6D0550(NULL);
}

void FontManagerClass::Unknown_6D2500(unsigned int id)
{
	Peek_Font(id)->Unknown_6D0A20(NULL);
}

// Find 0x6D2A70 out of line here and in 0x6D2700 (auto_inline off); Peek_Font is its inlined twin elsewhere (H19)
void FontManagerClass::Unknown_6D2580(const char *name, TPoint3D<int> color, unsigned int id)
{
	Find(id)->Unknown_6D1010(name, color);
}

inline ManagedFontClass *FontManagerClass::Peek_Font(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it == Fonts.end()) return NULL;
	return it->second;
}
























// NOT EXACT: order-only, 1 row: the inlined std::string(name) stores its terminator through [ebx+eax], ours [eax+ebx] (attempts.csv)
FontStyleClass *FontManagerClass::Get_Style(const char *name, unsigned int id)
{
	ManagedFontClass *font = Peek_Font(id);
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
// NOT EXACT: frame, 56 rows: ours keeps one more stack slot and releases style after the assert (attempts.csv)
RefCountPtr<FontStyleClass> FontManagerClass::Unknown_6D2700(const char *name, const unsigned int &id)
{
	RefCountPtr<FontStyleClass> style;
	ManagedFontClass *font = Find(id);
	if (font) {
		style = Create_Peek(font->Unknown_6D1600(name));
		if (style) {
			return style;
		}
	}

	G_ASSERT(0);
	return DebugFont;
}

// NOT EXACT: inline budget, 75 rows: the client calls the styles table's begin and bucket helpers out of line, ours inlines them (attempts.csv)
void FontManagerClass::Unknown_6D28C0(int value, unsigned int id)
{
	Peek_Font(id)->Unknown_Set_Styles(value);
}

void FontManagerClass::Add(ManagedFontClass *font)
{
	if (font && font->Id) {
		Fonts[font->Id] = font;
	}
}

#pragma auto_inline(off)
ManagedFontClass *FontManagerClass::Find(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it == Fonts.end()) return NULL;
	return it->second;
}
#pragma auto_inline(on)
/*
**	Reconstruction notes
**
**	client.exe: the window 0x6D0F70-0x6D2A70 (asserts on lines 360, 524, 533 and 550), then the
**	containers' instances. Not rebuilt yet: 0x6D0F70 (the out-of-line operator= of a {StringClass,
**	int, int} member of FontStyleClass at +0x68 / +0x74 / +0x80; its callers 0x6D0A70, 0x6D47C0 and
**	0x6D4C70 are in other files). 0x6D2700 is written (NOT EXACT, frame). 0x6D2400 / 0x6D2480 / 0x6D2500 differ only by their callee: one pattern, not counted.
**	The depth lever (workflow.md 3.1) made the destructor and 0x6D1C10 exact: the client calls the
**	nested table helpers out of line when they sit one inline level deeper.
**	Find 0x6D2A70 is never inlined (#pragma auto_inline(off)): the client calls it out of line in
**	0x6D2580 and 0x6D2700 and inlines the same body in the trio, Get_Style and 0x6D28C0, written as
**	the inline twin Peek_Font (builder H19: 0x6D2580 exact; no budget form inside 0x6D2580 sent
**	Find out of line: dead reads, a depth wrapper, a constructed color, a RefCountPtr local).
*/
